//=============================================================================================
/**
 * @file  btl_rec.cpp
 * @brief ポケモンXY バトルシステム クライアント操作記録データ生成
 * @author  taya
 *
 * @date  2010.12.29  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "btl_rec.h"

#if defined(GF_PLATFORM_WIN32)
#pragma warning( disable  : 4800 )  //ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
#endif

GFL_NAMESPACE_BEGIN(btl)
namespace rec {


/**
 * @brief 録画データのヘッダ部分のインデックス
 */
enum HeaderIndex
{
  HEADER_TIMING_CODE, // タイミングコード
  HEADER_FIELD_TAG,   // フィールドタグ
  HEADER_SIZE,        // ヘッダーサイズ
};

typedef enum {
  RECFIELD_NULL = 0,
  RECFIELD_ACTION,          ///< 通常行動選択
  RECFIELD_BTLSTART,        ///< バトル開始
  RECFIELD_TIMEOVER,        ///< 時間制限が来たので終了
  RECFIELD_SIZEOVER,        ///< これ以上コマンド記憶できないので終了
}RecFieldType;



//=============================================================================================
/**
 *  ツール関数群
 */
//=============================================================================================
inline u8 MakeRecFieldTag( RecFieldType type, u8 numClient, u8 fChapter )
{
  GFL_ASSERT(numClient < 16);
  GFL_ASSERT(type < 8);
  GFL_ASSERT(fChapter < 2);
  return ((fChapter&1) << 7) | ((type&0x07) << 4) | ((numClient)&0x0f);
}
inline void ReadRecFieldTag( u8 tagCode, FieldType* type, u8* numClient, u8* fChapter )
{
  *numClient = tagCode & 0x0f;
  *type      = static_cast<FieldType>( ((tagCode >> 4) & 0x07) );
  *fChapter  = ((tagCode>>7)&0x01);
}
inline u8 MakeClientActionTag( u8 clientID, u8 numAction )
{
  GFL_ASSERT(clientID < 8);
  GFL_ASSERT(numAction < 32);

  return (clientID << 5) | (numAction);
}
inline void ReadClientActionTag( u8 tagCode, u8* clientID, u8* numAction )
{
  *clientID  = ((tagCode>>5) & 0x07);
  *numAction = (tagCode & 0x01f);
}

/**
 * @brief タイミングコードを作成する
 * @param timing               録画タイミング
 * @param isRecordTargetData   録画データとして保存するデータか？
 * @return タイミングコード
 */
inline u8 MakeRecTimingCode( rec::Timing timing, bool isRecordTargetData )
{
  const u8 bit_isRecordTarget = static_cast<u8>( isRecordTargetData ) << 7;
  const u8 bit_timingCode     = static_cast<u8>( timing & 0x7f );
  const u8 timingCode = ( bit_isRecordTarget | bit_timingCode );
  return timingCode;
}

/**
 * @brief タイミングコードを読み取る
 * @param      timingCode           タイミングコード
 * @param[out] timing               録画タイミング
 * @param[out] isRecordTargetData   録画データとして保存するデータか？
 */
inline void ReadRecTimingCode( u8 timingCode, rec::Timing* timing, bool* isRecordTargetData )
{
  static const u8 mask_isRecordTarget = 0x80;
  static const u8 mask_timingCode     = 0x7f;
  *isRecordTargetData = ( ( timingCode & mask_isRecordTarget ) != 0 );
  *timing             = static_cast<rec::Timing>( timingCode & mask_timingCode );
}



//=============================================================================================
/**
 *  Data: サーバーから送られてきたデータを格納
 */
//=============================================================================================

Data::Data( void )
 :  m_writePtr(0),
    m_fSizeOver(false)
{

}
Data::~Data()
{

}

//------------------------------------------------------------------------------
/**
 * Data: データ書き込み
 *
 * @param   data
 * @param   size
 */
//------------------------------------------------------------------------------
void Data::Write( const void* data, u32 size )
{
  if( size == 0 ) 
  {
    return;
  }

  if( m_fSizeOver == false )
  {
    const u8* pData = reinterpret_cast<const u8*>( data );

    // 保存する必要があるかチェック
    {
      rec::Timing timing;
      bool isWriteTarget;
      ReadRecTimingCode( pData[ HEADER_TIMING_CODE ], &timing, &isWriteTarget );

      if( !isWriteTarget )
      {
        return;
      }
    }

    // HEADER_TIMING_CODE 部分は不要なのでスキップ
    ++pData;
    --size;

    u32 endPtr = m_writePtr + size;
    if( endPtr <= sizeof(m_buf) )
    {
      gfl2::std::MemCopy( pData, &m_buf[m_writePtr], size );
      m_writePtr = endPtr;
    }
    else
    {
      m_fSizeOver = true;
    }
  }
}
//------------------------------------------------------------------------------
/**
 * Data: タイミングコード取得
 *
 * @retval  BtlRecTiming
 */
//------------------------------------------------------------------------------
Timing  Data::GetTimingCode( const void* data )
{
  rec::Timing timing;
  bool isRecordTarget;
  const u8* pByte = reinterpret_cast<const u8*>( data );
  ReadRecTimingCode( pByte[ HEADER_TIMING_CODE ], &timing, &isRecordTarget );
  return timing;
}
//------------------------------------------------------------------------------
/**
 * Data: 最後まで正常に書き込まれているかチェック
 *
 * @retval  bool
 */
//------------------------------------------------------------------------------
bool Data::IsCorrect( void ) const
{
  return !(m_fSizeOver);
}
//------------------------------------------------------------------------------
/**
 * Data: 書き込みデータ取得
 *
 * @param[out]   size  データサイズ取得用変数ポインタ
 *
 * @retval  const void*   データ先頭ポインタ
 */
//------------------------------------------------------------------------------
const void* Data::GetDataPtr( u32* size ) const
{
  *size = m_writePtr;
  return m_buf;
}



//=============================================================================================
/**
 *  Reader: 格納したデータを読み取り
 */
//=============================================================================================

Reader::Reader( void )
 :  m_recordData(NULL)
{

}
Reader::~Reader()
{

}

//------------------------------------------------------------------------------
/**
 * Reader: 読み込み開始
 *
 * @param   recordData
 * @param   dataSize
 */
//------------------------------------------------------------------------------
void Reader::Init( const void* recordData, u32 dataSize )
{
  m_recordData = reinterpret_cast<const u8*>( recordData );
  m_dataSize   = dataSize;
  m_fReadOver  = false;

  for(u32 i=0; i<GFL_NELEMS(m_readPtr); ++i)
  {
    m_readPtr[i] = 0;
  }
}
//------------------------------------------------------------------------------
/**
 * Reader: 読み込み位置をリセット
 */
//------------------------------------------------------------------------------
void Reader::Reset( void )
{
  for(u32 i=0; i<GFL_NELEMS(m_readPtr); ++i)
  {
    m_readPtr[i] = 0;
  }
}
//------------------------------------------------------------------------------
/**
 * Reader: バトル開始チャプターが打たれているかチェック（＆読み込みポインタをその分すすめる）
 *
 * @param   clientID    対象クライアントID
 *
 * @retval  bool        打たれていたら true
 */
//------------------------------------------------------------------------------
bool Reader::CheckBtlInCmd( u8 clientID )
{
  FieldType type;
  u8 numClient, fChapter;

  u32* rp;

  rp = &m_readPtr[ clientID ];
  ReadRecFieldTag( m_recordData[ (*rp) ], &type, &numClient, &fChapter );

  if( type == RECFIELD_BTLSTART )
  {
    (*rp)++;
    return fChapter;
  }
  return false;
}
//------------------------------------------------------------------------------
/**
 * Reader: アクションデータ１件読み込み
 *
 * @param[in]    clientID    対象クライアントID
 * @param[out]   numAction   アクションデータ数
 * @param[out]   fChapter    チャプター（区切り）コードが埋め込まれていたらtrue
 *
 * @retval  const BTL_ACTION_PARAM*   読み込んだアクションデータ先頭
 */
//------------------------------------------------------------------------------
const BTL_ACTION_PARAM* Reader::ReadAction( u8 clientID, u8 *numAction, u8* fChapter )
{
  FieldType type;
  u8 numClient, readClientID, readNumAction;
  u32* rp;
  u32 i;

  if( m_fReadOver )
  {
    BTL_ACTION_PARAM* actionParam = (BTL_ACTION_PARAM*)(m_readBuf[ clientID ]);
    BTL_ACTION_SetRecPlayError( actionParam );
    *numAction = 1;
    *fChapter = false;
    return actionParam;
  }

  rp = &m_readPtr[ clientID ];

  BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadActStart, *rp );
  while( (*rp) < m_dataSize )
  {
    ReadRecFieldTag( m_recordData[ (*rp) ], &type, &numClient, fChapter );
    (*rp)++;

    if( (*rp) > m_dataSize ){
      break;
    }
    if( type == RECFIELD_ACTION )
    {
      const BTL_ACTION_PARAM* returnPtr = NULL;
      BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_SeekClient, numClient);
      if( numClient==0 || (numClient>BTL_CLIENT_MAX) ){
        break;
      }
      for(i=0; i<numClient; ++i)
      {
        ReadClientActionTag( m_recordData[ (*rp)++ ], &readClientID, &readNumAction );

        BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_CheckMatchClient, readClientID, clientID, readNumAction );

        if( ((*rp) >= m_dataSize) ){ break; }
        if( readClientID != clientID )
        {
          (*rp) += (sizeof(BTL_ACTION_PARAM) * readNumAction);
        }
        else
        {
          returnPtr = reinterpret_cast<const BTL_ACTION_PARAM*>(&m_recordData[(*rp)]);
          gfl2::std::MemCopy( returnPtr, m_readBuf[clientID], readNumAction * sizeof(BTL_ACTION_PARAM) );
          returnPtr = reinterpret_cast<const BTL_ACTION_PARAM*>(m_readBuf[ clientID ]);
          BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadActParam, (*rp), returnPtr->gen.cmd, returnPtr->fight.waza);
          (*rp) += (sizeof(BTL_ACTION_PARAM) * readNumAction);
          *numAction = readNumAction;
        }
      }
      if( returnPtr ){
        return returnPtr;
      }
    }
    else if( type == RECFIELD_TIMEOVER )
    {
      BTL_ACTION_PARAM* actionParam = (BTL_ACTION_PARAM*)(m_readBuf[ clientID ]);

      BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadTimeOverCmd, (*rp) );
      BTL_ACTION_SetRecPlayOver( actionParam );
      *numAction = 1;
      *fChapter = false;
      return actionParam;
    }
    else if( type == RECFIELD_BTLSTART )
    {
      BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_ReadActSkip, numClient, (*rp) );
    }
    else{
      break;
    }
    if( ((*rp) >= m_dataSize) ){ break; }
  }


//  GFL_ASSERT_MSG(0, "不正なデータ読み取り clientID=%d, type=%d, readPtr=%d, datSize=%d",
//            clientID, type, (*rp), m_dataSize);

  m_fReadOver = true;
  {
    BTL_ACTION_PARAM* actionParam = reinterpret_cast<BTL_ACTION_PARAM*>(m_readBuf[ clientID ]);
    BTL_ACTION_SetRecPlayError( actionParam );
    *numAction = 1;
    *fChapter = false;
    return actionParam;
  }
}
//------------------------------------------------------------------------------
/**
 * Reader: 記録されているチャプタ数を数える
 *
 * @retval  u32
 */
//------------------------------------------------------------------------------
u32 Reader::GetTurnCount( void ) const
{
  u32 p = 0;
  u32 turnCount = 0;

  FieldType type;
  u8 fChapter, numClient, readClientID, readNumAction;

  while( p < m_dataSize )
  {
    ReadRecFieldTag( m_recordData[p++], &type, &numClient, &fChapter );
    if( fChapter ){
      ++turnCount;
    }
    if( (p >= m_dataSize) ){ break; }
    if( type != RECFIELD_ACTION )
    {
      p += numClient;
    }
    else
    {
      for(u32 i=0; i<numClient; ++i)
      {
        ReadClientActionTag( m_recordData[p++], &readClientID, &readNumAction );
        p += (sizeof(BTL_ACTION_PARAM) * readNumAction);
        if( (p >= m_dataSize) ){ break; }
      }
    }
  }
  return turnCount;
}
//------------------------------------------------------------------------------
/**
 * Reader: 最後まで読み込まれたか判定
 *
 * @retval  bool
 */
//------------------------------------------------------------------------------
bool Reader::IsReadComplete( u8 clientID ) const
{
  if( m_fReadOver == false )
  {
    const u32* rp = &m_readPtr[ clientID ];
    BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_CheckReadComp, clientID, (*rp), m_dataSize );
    return ( (*rp) == m_dataSize );
  }
  return true;
}



//=============================================================================================
/**
 *  SendTool: サーバ送信データ構築
 */
//=============================================================================================

//------------------------------------------------------------------------------
/**
 * SendTool: 初期化
 *
 * @param   fChapter    チャプター（ターン先頭）フラグ
 */
//------------------------------------------------------------------------------
void SendTool::Init( bool fChapter )
{
  gfl2::std::MemClear( m_buffer, sizeof(m_buffer) );

  m_writePtr = 0;      ///< 書き込みポインタ
  m_clientBit = 0;     ///< クライアント毎の書き込み完了ビット
  m_numClients = 0;    ///< 書き込んだクライアント数
  m_type = 0;          ///< 書き込み中のデータタイプ
  m_fError = 0;        ///< エラー発生フラグ

  m_fChapter = fChapter;
}
//=============================================================================================
/**
 * SendTool: バトル開始タイミングコードを生成
 *
 * @param   recTool
 * @param   dataSize    送信データサイズ
 *
 * @retval  void*   送信データポインタ
 */
//=============================================================================================
void* SendTool::PutBtlInTiming( u32* dataSize )
{
  m_buffer[ HEADER_TIMING_CODE ] = MakeRecTimingCode( RECTIMING_BtlIn, false );
  m_buffer[ HEADER_FIELD_TAG ]   = MakeRecFieldTag( RECFIELD_NULL, 0, false );
  *dataSize = HEADER_SIZE;
  return m_buffer;
}
//=============================================================================================
/**
 * SendTool: バトル開始チャプタデータを生成
 *
 * @param   recTool
 * @param   dataSize    送信データサイズ
 *
 * @retval  void*   送信データポインタ
 */
//=============================================================================================
void* SendTool::PutBtlInChapter( u32* dataSize, bool fChapter )
{
  m_buffer[ HEADER_TIMING_CODE ] = MakeRecTimingCode( RECTIMING_None, true );
  m_buffer[ HEADER_FIELD_TAG ]   = MakeRecFieldTag( RECFIELD_BTLSTART, 0, fChapter );
  *dataSize = HEADER_SIZE;
  return m_buffer;
}

//=============================================================================================
/**
 * SendTool: アクション選択データを１クライアント分、追加
 *
 * @param   recTool
 * @param   clientID
 * @param   action
 * @param   numAction
 */
//=============================================================================================
void SendTool::PutSelActionData( u8 clientID, const BTL_ACTION_PARAM* action, u8 numAction )
{
  // ヘッダ記述用に空けておく
  if( m_writePtr == 0 ){
    m_type = RECFIELD_ACTION;
    m_writePtr = HEADER_SIZE;
  }

  GFL_ASSERT(m_type == RECFIELD_ACTION);

  if( (m_clientBit & (1 << clientID)) == 0 )
  {
    u32 endPtr = m_writePtr + ((sizeof(BTL_ACTION_PARAM) * numAction) + 1);
    if( endPtr <= sizeof(m_buffer) )
    {
      m_clientBit |= (1 << clientID);
      m_numClients++;

      m_buffer[ m_writePtr ] = MakeClientActionTag( clientID, numAction );
      gfl2::std::MemCopy( action, &m_buffer[m_writePtr+1], sizeof(BTL_ACTION_PARAM)*numAction );
      m_writePtr = endPtr;
    }
    else
    {
      GFL_ASSERT(0);   // データ容量が多すぎる
      m_fError = 1;
    }
  }
  else
  {
    GFL_ASSERT_MSG(0, "client_%d のデータ書き込みが２度発生している", clientID);
  }
}
//=============================================================================================
/**
 * SendTool: アクション選択データの書き込み処理を終了
 *
 * @param   recTool
 * @param   dataSize  [out] クライアントに送信するデータサイズが入る
 *
 * @retval  void*   正常終了ならクライアントに送信するデータポインタ／エラー時はNULL
 */
//=============================================================================================
void* SendTool::FixSelActionData( Timing timingCode, u32* dataSize )
{
  GFL_ASSERT(m_type == RECFIELD_ACTION);

  if( m_fError == 0 )
  {
    m_buffer[ HEADER_TIMING_CODE ] = MakeRecTimingCode( timingCode, true );
    m_buffer[ HEADER_FIELD_TAG ]   = MakeRecFieldTag( static_cast<RecFieldType>(m_type), m_numClients, m_fChapter );
    *dataSize = m_writePtr;
    return m_buffer;
  }
  return NULL;
}

//=============================================================================================
/**
 * SendTool: タイムオーバー通知コードの書き込み処理
 *
 * @param   recTool
 * @param   dataSize
 *
 * @retval  void*
 */
//=============================================================================================
void* SendTool::PutTimeOverData( u32* dataSize )
{
  m_buffer[ HEADER_TIMING_CODE ] = MakeRecTimingCode( RECTIMING_None, true );
  m_buffer[ HEADER_FIELD_TAG ]   = MakeRecFieldTag( RECFIELD_TIMEOVER, 0, false );
  *dataSize = HEADER_SIZE;

  BTL_N_PrintfEx( PRINT_CHANNEL_RECTOOL, DBGSTR_REC_TimeOverCmdWrite );
  return m_buffer;
}
//=============================================================================================
/**
 * SendTool: [コマンド整合性チェック用] クライアントが受け取ったアクション選択データを書き戻す
 *
 * @param   recTool
 * @param   data
 * @param   dataSize
 */
//=============================================================================================
void SendTool::RestoreStart( const void* data, u32 dataSize )
{
  GFL_ASSERT(dataSize<sizeof(m_buffer));

  gfl2::std::MemCopy( data, m_buffer, dataSize );
  m_writePtr = dataSize;
  m_clientBit = 0;
  m_numClients = 0;
  m_type = 0;
  m_fChapter = 0;
  m_fError = 0;
}

//=============================================================================================
/**
 * SendTool: [コマンド整合性チェック用] クライアントが受け取ったアクション選択データ読み取り
 *
 * @param   recTool
 * @param   rp          [io]
 * @param   clientID    [out]
 * @param   posIdx      [out]
 * @param   action      [out]
 *
 * @retval  bool    これ以上読めなくなったらfalse
 */
//=============================================================================================
bool SendTool::RestoreFwd( u32* rp, u8* clientID, u8* numAction, BTL_ACTION_PARAM* action )
{
  if( (*rp) == 0 ){
    (*rp) = HEADER_SIZE;  // ヘッダ分をスキップ
  }

  if( (*rp) < m_writePtr )
  {
    u32 dataSize;

    ReadClientActionTag( m_buffer[ (*rp) ], clientID, numAction );

    GFL_ASSERT((*clientID) < BTL_CLIENT_MAX);
    GFL_ASSERT_MSG((*numAction) <= BTL_POSIDX_MAX, "numAction=%d", (*numAction));

    dataSize = sizeof(BTL_ACTION_PARAM) * (*numAction);
    ++(*rp);
    gfl2::std::MemCopy( &(m_buffer[(*rp)]), action, dataSize );
    (*rp) += dataSize;


    return true;
  }

  return false;
}



}  // end of namespace 'rec'

GFL_NAMESPACE_END(btl)

#if defined(GF_PLATFORM_WIN32)
#pragma warning( default  : 4800 )
#endif