//===========================================================================
/**
 * @file  gfl_MsgData.cpp
 * @brief GAMEFREAK Library  String Print System ( MsgData )
 * @author  GAMEFREAK inc.
 * @date  2010.11.08
 */
//===========================================================================


#include <gfl_Common.h>
#include <gfl_Std.h>
#include <gfl_Fs.h>
#include <gfl_Heap.h>

#include <str/gfl_MsgData.h>

namespace gfl {
namespace str {

//----------------------------------------------------------------------
// デバッグ出力有効
//----------------------------------------------------------------------
enum {
  MSGDAT_PRINT_FLAG = (gfl::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl::debug::PRINT_FLAG_LEVEL2)
};

//----------------------------------------------------------------------
// statics
//----------------------------------------------------------------------
u32 MsgData::s_CurrentLangID = 0;


//----------------------------------------------------------------------
/**
 * コンストラクタ＆デストラクタ
 */
//----------------------------------------------------------------------

//--------------------------------------------------------------
// アーカイブパス＆データIDから構築
//--------------------------------------------------------------
MsgData::MsgData( const wchar_t* arcFilePath, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType, u32 arc_init_flag )
 : m_pHeap(pHeap), m_pArc(NULL), m_loadType(loadType), m_dataID(0), m_targetLangID(0), m_pDataHeader(NULL),  m_pLangHeader(NULL)
{
  m_pArc = GFL_NEW(pHeap) gfl::fs::ArcFile( pHeap, arcFilePath, arc_init_flag );
  construct_common( datID, pHeap, loadType );
}
//--------------------------------------------------------------
// アーカイブID＆データIDから構築
//--------------------------------------------------------------
MsgData::MsgData( gfl::fs::ArcFile::ARCID arcID, u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType, u32 arc_init_flag )
 : m_pHeap(pHeap), m_pArc(NULL), m_loadType(loadType), m_dataID(0), m_targetLangID(0), m_pDataHeader(NULL),  m_pLangHeader(NULL)
{
  m_pArc = GFL_NEW(pHeap) gfl::fs::ArcFile( pHeap, arcID, arc_init_flag );
  construct_common( datID, pHeap, loadType );
}
/**
 *  ↑コンストラクタ共通下請け／m_pArcが作成された後に呼び出すこと
 */
void MsgData::construct_common( u32 datID, gfl::heap::HeapBase* pHeap, LoadType loadType )
{
  GFL_ASSERT_STOP(m_pArc!=NULL);

  m_strParamBody.len = 0;
  m_strParamBody.offset = 0;
  m_strParamBody.userParam = 0;

  m_pDataHeader    = loadDataHeader( m_pArc, datID, pHeap );
  m_dataID         = datID;
  m_targetLangID   = getCurrentLangID( m_pDataHeader );

  // 通常は言語ブロックを持たない
  if( m_loadType == LOAD_PART )
  {
    m_pLangHeader = NULL;
  }
  // 高速モードでは言語ブロックを読み込んでおく
  else
  {
    m_pLangHeader = reinterpret_cast<MSG_LANG_BLOCK*>GflHeapAllocMemory( pHeap, m_pDataHeader->maxLangBlockSize );
    loadLangHeader( &m_pLangHeader, m_pDataHeader, m_pArc, m_dataID, m_targetLangID );
  }
}
//--------------------------------------------------------------
// メモリ上に読み込み済みのデータから構築
//--------------------------------------------------------------
MsgData::MsgData( const void* dataAdress, gfl::heap::HeapBase* pHeap )
 : m_pHeap(pHeap), m_pArc(NULL), m_loadType(LOAD_FULL)
{
  m_pDataHeader    = reinterpret_cast<const MSG_HEADER_BLOCK*>( dataAdress );
  m_dataID         = 0;
  m_targetLangID   = getCurrentLangID( m_pDataHeader );
  loadLangHeader( &m_pLangHeader, m_pDataHeader, NULL, m_dataID, m_targetLangID );
}

//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
MsgData::~MsgData()
{
  // pArc != NULL なら自前でロードしたデータなので解放
  if( m_pArc != NULL )
  {
    if( m_pLangHeader != NULL ){
      GflHeapFreeMemory( m_pLangHeader );
    }

    if( m_pDataHeader != NULL ){
      GflHeapFreeMemory( const_cast<MSG_HEADER_BLOCK*>(m_pDataHeader) );
    }
  }

  if( m_pArc != NULL ){
    GFL_DELETE( m_pArc );
  }
}


MsgData::MSG_HEADER_BLOCK*
MsgData::loadDataHeader( gfl::fs::ArcFile* pArc, u32 dataID, gfl::heap::HeapBase* pHeap )
{
  u8   tmp_buffer[ sizeof(MSG_HEADER_BLOCK) ];

  MSG_HEADER_BLOCK *header, *tmp_header;

  pArc->SeekDataPtr( dataID, 0 );
  pArc->LoadDataPtr( sizeof(tmp_buffer), tmp_buffer );

  tmp_header = reinterpret_cast<MsgData::MSG_HEADER_BLOCK*>(tmp_buffer);
  {
    u32 headerSize = sizeof(MSG_HEADER_BLOCK) + (sizeof(u32)*tmp_header->numLangs);
    header = reinterpret_cast<MSG_HEADER_BLOCK*>( GflHeapAllocMemory( pHeap, headerSize ) );
  }

  if( header )
  {
    gfl::std::MemCopy( tmp_header, header, sizeof(MSG_HEADER_BLOCK) );
    pArc->LoadDataPtr( sizeof(u32)*header->numLangs, header->ofsLangBlocks );
  }

  return header;
}

//--------------------------------------------------------------------------
/**
 * @brief 言語ブロックヘッダを読み込み
 *
 * @param[out]  ppLangHeader    言語ブロックデータ読み込み先バッファへのポインタ
 * @param[in]   pDataHeader     読み込み済みデータヘッダへのポインタ
 * @param[in]   pArc            メッセージデータアーカイブハンドラ（外部データ構築の場合はNULL）
 * @param[in]   dataID          対象データID
 * @param[in]   langID          対象言語ID
 */
//--------------------------------------------------------------------------
void
MsgData::loadLangHeader( MSG_LANG_BLOCK** ppLangHeader, const MSG_HEADER_BLOCK* pDataHeader,
    gfl::fs::ArcFile* pArc, u32 dataID, u32 langID )
{
  u32  langBlockOfs = getTargetLangBlockOfs( pDataHeader, langID );

  // pArc!=NULL なら自前でロードしたデータ
  if( pArc != NULL )
  {
    GFL_ASSERT( *ppLangHeader );

    MSG_LANG_BLOCK  langBlockHeader;

    pArc->SeekDataPtr( dataID, langBlockOfs );
    pArc->LoadDataPtr( sizeof(MSG_LANG_BLOCK), reinterpret_cast<void*>(*ppLangHeader) );

    pArc->SeekDataPtr( dataID, langBlockOfs );
    pArc->LoadDataPtr( (*ppLangHeader)->size, reinterpret_cast<void*>(*ppLangHeader) );
  }
  // pArc==NULL なら外部データ構築とみなす
  else
  {
    *(ppLangHeader) = reinterpret_cast<MSG_LANG_BLOCK*>(
                        reinterpret_cast<uptr>(pDataHeader) + langBlockOfs
                      );
  }
}
//------------------------------------------------------------------------------
/**
 * @brief   読み込むべき言語IDを取得する
 *
 * @param[in]     dataHeader    メッセージデータヘッダ
 *
 * @retval  u32   言語ID
 */
//------------------------------------------------------------------------------
u32
MsgData::getCurrentLangID( const MSG_HEADER_BLOCK* dataHeader ) const
{
  if( s_CurrentLangID >= dataHeader->numLangs )
  {
    return 0;
  }
  return s_CurrentLangID;
}
//------------------------------------------------------------------------------
/**
 * @brief  ロード対象言語ブロックの、データ先頭からのオフセットを取得する
 *
 * @param[in]   dataHeader  メッセージデータヘッダ
 * @param[in]   langID      ロード対象言語ID
 *
 * @retval  u32   言語ブロックのデータ先頭からのオフセット（byte）
 */
//------------------------------------------------------------------------------
u32
MsgData::getTargetLangBlockOfs( const MSG_HEADER_BLOCK* dataHeader, u32 langID ) const
{
  return dataHeader->ofsLangBlocks[ langID ];
}

//----------------------------------------------------------------------
/**
 * @brief   指定文字列をバッファに読み込む
 *
 * @param[in]    strID   文字列ID
 * @param[out]   dst     読み込み先文字列バッファ
 *
 * @return  u16      ユーザー設定パラメータ
 */
//----------------------------------------------------------------------
u16
MsgData::GetString( u32 strID, StrBuf& dst )
{
  //GF_ASSERT_MSG(strID < handle->dataHeader->numStrings, "strID=%d, numStr=%d", strID, handle->dataHeader->numStrings);
  if( strID >= m_pDataHeader->numStrings )
  {
    GFL_ASSERT_MSG(0, "strID=%d, numStr=%d dataID=%d", strID, m_pDataHeader->numStrings, m_dataID);
    dst.Clear();
    return 0;
  }

  checkTargetLangChanged();

//  GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] ReadStart, StrID=%d, LangID=%d, bufSize=%d\n", strID, m_targetLangID, dst.GetSize() );

  {
    const MSG_STRPARAM_BLOCK* param = getStrParamPointer( strID );

//    GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] StrParam. ofs=%08x, len=%d\n", param->offset, param->len );

    // 逐次モード
    if( m_loadType == LOAD_PART )
    {
      wchar_t*  buf;
      u32  size;

      size = param->len * sizeof(wchar_t);
      buf = reinterpret_cast<wchar_t*>GflHeapAllocMemoryAlign( m_pHeap, size, -(m_pHeap->GetDefaultAlign()) );
      if( buf )
      {
        u32  langBlockOfs = getTargetLangBlockOfs( m_pDataHeader, m_targetLangID ) + param->offset;
//        GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] AllocTmpBuf, size=%d\n", size );
        m_pArc->SeekDataPtr( m_dataID, langBlockOfs );
        m_pArc->LoadDataPtr( size, buf );

        putToStrBuf( buf, param->len, strID, dst );

        GflHeapFreeMemory( buf );
      }
    }
    // 高速モード
    else
    {
      wchar_t* str = reinterpret_cast<wchar_t*> (
                       reinterpret_cast<uptr>(m_pLangHeader) + param->offset
                     );
      putToStrBuf( str, param->len, strID, dst );
    }

    return param->userParam;
  }
}


//----------------------------------------------------------------------
/**
 * @brief   一時的に言語を切り替えて指定文字列をバッファに読み込む
 *
 * @param[in]    strID   文字列ID
 * @param[in]    langID  言語
 * @param[out]   dst     読み込み先文字列バッファ
 *
 * @return  u16      ユーザー設定パラメータ
 */
//----------------------------------------------------------------------
u16
MsgData::GetStringByLanguage( u32 strID, u32 langID, StrBuf& dst )
{
  u32 default_langID = GetLangID();

  //ターゲットのIDを一時的に変更していやることで、
  //GetString内部のcheckTargetLangChangedにてヘッダを再読み込みする
  SetLangID( langID );

  u16 userParam = GetString( strID, dst );

  SetLangID( default_langID );

  return userParam;
}

//----------------------------------------------------------------------
/**
 * @brief   ユーザー設定パラメータを取得
 *
 * @param[in]    strID   文字列ID
 *
 * @return  u16      ユーザー設定パラメータ
 */
//----------------------------------------------------------------------
u16
MsgData::GetUserParam( u32 strID )
{
  if( strID >= m_pDataHeader->numStrings )
  {
    GFL_ASSERT_MSG(0, "strID=%d, numStr=%d", strID, m_pDataHeader->numStrings);
    return 0;
  }

  const MSG_STRPARAM_BLOCK* param = getStrParamPointer( strID );
  return param->userParam;
}



//------------------------------------------------------------------------------
/**
 * 文字列データを、StrBufクラスにセットする（暗号済みデータ対応）
 *
 * @param[in]   src      セットする文字列
 * @param[in]   strLen   文字列長（終端コードを含む）
 * @param[in]   strID    src の文字列ID（複合キーとして使用）
 * @param[out]  dst      セット先文字列クラス
 */
//------------------------------------------------------------------------------
void
MsgData::putToStrBuf( const wchar_t* src, u32 strLen, u32 strID, StrBuf& dst ) const
{
  u32 bufLen = dst.GetSize();
  if ( bufLen < strLen )
  {
    GFL_ASSERT_MSG( 0, "buffer size over ! strID=%d strLen=%d bufLen=%d \n", strID, strLen, bufLen ); //@check
  }

  dst.Clear();

  if( m_pDataHeader->reserved == DATA_CODED )
  {
    u16 mask = static_cast<u16>(strID);
    mask = (0x2983 * (mask+3)) & 0xffff;

    for(u32 i=0; i<strLen; ++i)
    {
      wchar_t  code = (*src);
      (code) ^= mask;

      if( !dst.AddCode( code ) )
      { // 文字を追加できなかった場合、中途半端なタグ表示等を避けるため潔くクリア
        dst.Clear();
        return;
      }
      ++src;
      mask = (((mask&0xe000)>>13) | ((mask&0x1fff)<<3));
    }
  }
  else
  {
    for(u32 i=0; i<strLen; ++i)
    {
      wchar_t  code = (*src);
      if( !dst.AddCode( code ) )
      { //文字を追加できなかった場合、中途半端なタグ表示等を避けるため潔くクリア
        dst.Clear();
        return;
      }
      ++src;
    }
  }

  dst.AddCodeFinish();
}
//--------------------------------------------------------------------------------------
/**
 * メッセージデータに含まれる文字列数を取得
 *
 * @retval  u32     文字列数
 */
//--------------------------------------------------------------------------------------
u32
MsgData::GetStrCount( void ) const
{
  return m_pDataHeader->numStrings;
}



//--------------------------------------------------------------------------
/**
 * 読み込み対象の言語IDが変更されているかどうかチェック
 * 変更されていれば、必要に応じて内部データを更新する。
 */
//--------------------------------------------------------------------------
void
MsgData::checkTargetLangChanged( void )
{
  u8 currentLangID = getCurrentLangID( m_pDataHeader );

  if( m_targetLangID != currentLangID )
  {
//    GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] targetLangChanged %d->%d\n", m_targetLangID, currentLangID );

    m_targetLangID = currentLangID;

    if( m_loadType != LOAD_PART )
    {
      loadLangHeader( &m_pLangHeader, m_pDataHeader, m_pArc, m_dataID, m_targetLangID );
    }
  }
}
//--------------------------------------------------------------------------
/**
 * 指定文字列のパラメータ部分ポインタを返す
 *
 * @param   strID     文字列ID
 *
 * @retval  const MSG_STRPARAM_BLOCK*   パラメータ部分へのポインタ
 */
//--------------------------------------------------------------------------
const MsgData::MSG_STRPARAM_BLOCK*
MsgData::getStrParamPointer( u32 strID )
{
  // 逐次モード
  if( m_loadType == LOAD_PART )
  {
    // ファイルからハンドラが内包する変数領域に読み込み、そのアドレスを返している
    u32 offset = getTargetLangBlockOfs( m_pDataHeader, m_targetLangID )
                + sizeof(MSG_LANG_BLOCK) + sizeof(MSG_STRPARAM_BLOCK) * strID;

    m_pArc->SeekDataPtr( m_dataID, offset );
    m_pArc->LoadDataPtr( sizeof(MSG_STRPARAM_BLOCK), &m_strParamBody );
    return &m_strParamBody;
  }
  // 高速モード
  else
  {
    // 既に読み込み済みのパラメータアドレスを返している
    return &(m_pLangHeader->params[ strID ]);
  }
}


/*============================================================================================*/
/*                                                                                            */
/*  ロードする言語の切り替え処理                                                              */
/*                                                                                            */
/*  同一の文字列IDを使って、実際に読み出される文字列を切り替えるための仕組み。                */
/*  日本語版の『ひらがな－漢字』切り替えとか、                                                */
/*  欧州版の『英・仏・独・伊・西』５ヶ国語切り替えとかに使ってもらうことを想定。              */
/*                                                                                            */
/*============================================================================================*/

//--------------------------------------------------------------------------------------
/**
 * ロード対象となる言語IDを設定する。
 * 以降、MsgData クラスを介して取得できる文字列が切り替わる。
 *
 * @param[in]   langID    言語ID
 *
 */
//--------------------------------------------------------------------------------------
void
MsgData::SetLangID( u32 langID )
{
  s_CurrentLangID = langID;
}

//--------------------------------------------------------------------------------------
/**
 * 設定されたロード対象の言語IDを取得。
 *
 * @retval  u8    設定されている言語ID
 */
//--------------------------------------------------------------------------------------
u32
MsgData::GetLangID( void )
{
  return s_CurrentLangID;
}

} /* end of namespace str */
} /* end of namespace gfl */



