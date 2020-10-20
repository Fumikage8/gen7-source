//============================================================================================
/**
 * @file  wordsetloader.cpp
 * @brief wordsetでの事前バッファ読み込み管理
 * @author  tamada
 * @date  2012.08.23
 */
//============================================================================================
#include "System/include/PokemonVersion.h"
#include "Print/include/PrintSystem.h"
#include "Print/include/WordSetLoader.h"

#include "arc_index/message.gaix"


//------------------------------------------------------------------
/**
 * @brief 読み込み対象のGARC指定
 * @note  include/print/WordsetLoader.hのenumと並びをそろえること
 */
//------------------------------------------------------------------
static const u16 garc_id_table[ PRELOAD_NO_MAX ] =
{
  GARC_message_itemname_wordset_DAT,
  GARC_message_wazaname_DAT,
  GARC_message_typename_DAT,
  GARC_message_tokusei_DAT,
  GARC_message_seikaku_DAT,
  GARC_message_place_name_DAT,
  GARC_message_trname_DAT,
  GARC_message_bag_pocket_DAT,
/* @note niji未実装
  GARC_message_trtype_DAT,
  GARC_message_assist_power_DAT,
  GARC_message_kawaigari_goods_DAT,
  GARC_message_pofure_DAT,
  GARC_message_contest_DAT,
  GARC_message_pmn_trend_DAT,
*/
};

//------------------------------------------------------------------
//------------------------------------------------------------------
static inline u32 countBits( u32 bits )
{
u32 count = 0;
while ( bits != 0 )
{
  if (bits & 1) count ++;
  bits >>= 1;
}
return count;
}

//------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param[in]   pHeap         使用するヒープの指定
 * @param[in]   p_readman     使用するFileReadManagerへのポインタ
 * @param[in]   read_id_bits  事前読み込みする名前の指定
 */
//------------------------------------------------------------------
WordSetLoader::WordSetLoader( gfl2::heap::HeapBase *pHeap, u32 read_id_bits )
  : m_pHeap( pHeap )
  , m_ReadBits(read_id_bits)
  , m_ReadCount(countBits(read_id_bits))
  , m_State(STATE_INIT)
  , m_BufferArray( NULL )
  , m_bAppended(false)
  , m_MessageArcId(-1)
{
  // m_ReadBitsを修正する
  u32 temp_bits = m_ReadBits;
  for ( u32 bit_idx = 0; temp_bits != 0; ++ bit_idx, temp_bits >>= 1 )
  {
    GFL_ASSERT( bit_idx < GFL_NELEMS(garc_id_table) );
    if ( temp_bits & 1 )
    {
      if( GetGlobalMsgData( garc_id_table[bit_idx] ) != NULL )
      {
        // グローバルに存在する場合は読み込まないようにフラグを落としておく
        m_ReadBits &= ~( 1 << bit_idx );
      }
    }
  }
 
  // m_ReadCountも修正する
  m_ReadCount = countBits(m_ReadBits);
}

//------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//------------------------------------------------------------------
WordSetLoader::~WordSetLoader()
{
  Release();
}

//------------------------------------------------------------------
/**
 * @brief   読み込み開始
 */
//------------------------------------------------------------------
void WordSetLoader::StartLoad( gfl2::heap::HeapBase * pTempHeap )
{
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  m_BufferArray = GFL_NEW_ARRAY(m_pHeap) Buffer[ m_ReadCount ];
  u32 count = 0;
  u32 temp_bits = m_ReadBits;

  m_MessageArcId = print::GetMessageArcId();

  for ( u32 bit_idx = 0; temp_bits != 0; ++ bit_idx, temp_bits >>= 1 )
  {
    GFL_ASSERT( bit_idx < GFL_NELEMS(garc_id_table) );
    if ( temp_bits & 1 )
    {
      m_BufferArray[count].mGarcId = garc_id_table[bit_idx];

      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
      req.fileName = NULL;           //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
      req.arcId = m_MessageArcId;              //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
      req.datId = m_BufferArray[count].mGarcId;              //!< [in]  アーカイブファイル内データID
      req.prio = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;               //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
      req.ppBuf = &m_BufferArray[count].mAddress;              //!< [out] heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
      req.pBufSize = &m_BufferArray[count].mSize;           //!< [out] heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
      req.pRealReadSize = NULL;      //!< [out] 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
      req.heapForBuf = m_pHeap;         //!< [in]  ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
      req.align = 4;              //!< [in]  heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。
      req.heapForReq = pTempHeap->GetLowerHandle();         //!< [in]  リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
      req.heapForCompressed = NULL;  //!< [in]  解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。

      man->AddArcFileLoadDataReq( req );

      ++ count;
    }
  }
  GFL_ASSERT( count == m_ReadCount );

  m_State = STATE_LOADING;
}

//------------------------------------------------------------------
/**
 * @brief   読み込み待ち処理
 * @return  bool  trueのときセットアップ完了
 *
 * @note    全データ読み込み時に初めてMsgDataの生成を行っている
 */
//------------------------------------------------------------------
bool WordSetLoader::WaitLoad( void )
{
  if( m_State == STATE_LOADED )
  {
    return true;
  }

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  bool ret = true;
  for (u32 i = 0; i< m_ReadCount; ++i )
  {
    if (man->IsArcFileLoadDataFinished( &m_BufferArray[i].mAddress ) == false )
    {
      ret = false;
      break;
    }
  }
  if ( ret )
  {
    for (u32 i = 0; i < m_ReadCount; ++i )
    {
      m_BufferArray[i].mMsgData = GFL_NEW(m_pHeap) gfl2::str::MsgData(
          m_BufferArray[i].mAddress, m_pHeap );
    }
    m_State = STATE_LOADED;
  }
  return ret;
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void WordSetLoader::Release( void )
{
  switch ( m_State )
  {
  case STATE_INIT:
    return;

  case STATE_LOADING:
    GFL_ASSERT_STOP( 0 );

  case STATE_LOADED:
    if ( m_bAppended )
    {
      m_MessageArcId = -1;
      m_bAppended = false;
    }
    for (u32 i = 0; i < m_ReadCount; ++i )
    {
      GflHeapFreeMemory( m_BufferArray[i].mAddress );
      GFL_DELETE( m_BufferArray[i].mMsgData );
    }
    if ( m_BufferArray )
    {
      GFL_DELETE_ARRAY( m_BufferArray );
      m_BufferArray = NULL;
    }
  }
  m_State = STATE_INIT;
}

//------------------------------------------------------------------
/**
 * @brief 指定IDのMsgDataを取得する
 * @param[in]   dat_id
 * @return      MsgDataへのポインタ、存在しない場合はNULLを返す
 */
//------------------------------------------------------------------
gfl2::str::MsgData * WordSetLoader::GetMsgData( u32 dat_id )
{
  // グローバルに存在する場合は、グローバル側を返す。
  gfl2::str::MsgData* globalMsgData = GetGlobalMsgData( dat_id );
  if( globalMsgData )
  {
    return globalMsgData;
  }


  for (u32 i = 0; i < m_ReadCount; ++ i )
  {
    if ( m_BufferArray[i].mGarcId == dat_id )
    {
      return m_BufferArray[i].mMsgData;
    }
  }
  return NULL;
}

WordSetLoader::Buffer               WordSetLoader::s_StaticBuffer[ PRELOAD_NO_MAX ];
u32                  WordSetLoader::s_ReadBits;
gfl2::heap::HeapBase * WordSetLoader::s_BufferHeap;

//------------------------------------------------------------------
/**
 * @brief 永続的にWordSetに読み込んでおく仕組み
 */
//------------------------------------------------------------------
void WordSetLoader::LoadGlobal( gfl2::heap::HeapBase * pHeap, u32 bits )
{
  s_ReadBits = bits;
  s_BufferHeap = pHeap;

  u32 temp_bits = bits;
  for ( u32 count = 0; temp_bits != 0; ++ count, temp_bits >>= 1 )
  {
    GFL_ASSERT( count < GFL_NELEMS(garc_id_table) );
    if ( temp_bits & 1 )
    {
      u16 dat_id = garc_id_table[count];
      s_StaticBuffer[count].mGarcId   = dat_id;
      s_StaticBuffer[count].mAddress  = NULL;
      s_StaticBuffer[count].mSize     = 0;
      if ( s_StaticBuffer[count].mMsgData )
      {
        GFL_DELETE s_StaticBuffer[count].mMsgData;
      }
      s_StaticBuffer[count].mMsgData  = GFL_NEW( pHeap ) gfl2::str::MsgData(
          print::GetMessageArcId(), dat_id, pHeap, gfl2::str::MsgData::LOAD_FULL );
    }
    else
    {
      s_StaticBuffer[count].mGarcId   = 0;
      s_StaticBuffer[count].mAddress  = NULL;
      s_StaticBuffer[count].mSize     = 0;
      s_StaticBuffer[count].mMsgData  = NULL;
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief   常駐バッファ再読み込み処理
 */
//------------------------------------------------------------------
void WordSetLoader::ReloadGlobal( void )
{
  if ( s_BufferHeap == NULL || s_ReadBits == 0 ) {
    //  コンフィグ設定でここにくる可能性がある。
    //  GFL_ASSERT( 0 );  // @check 読み込んでいないのに再読み込みリクエスト
    return;
  }
/*
  u32 temp_bits = s_ReadBits;
  for ( u32 count = 0; temp_bits != 0; ++ count, temp_bits >>= 1 )
  {
    if ( s_StaticBuffer[count].mMsgData )
    {
      GFL_DELETE s_StaticBuffer[count].mMsgData;
      GFL_ASSERT( temp_bits & 1 );  //@check フラグとの整合性チェック
    }
    s_StaticBuffer[count].mGarcId   = 0;
    s_StaticBuffer[count].mAddress  = NULL;
    s_StaticBuffer[count].mSize     = 0;
    s_StaticBuffer[count].mMsgData  = NULL;
  }
*/
  //削除
  ReleaseGlobal();

  //再読み込みする
  LoadGlobal( s_BufferHeap, s_ReadBits );
}

//------------------------------------------------------------------
/**
 * @brief 指定IDのMsgDataを取得する
 * @param[in]   dat_id
 * @return      MsgDataへのポインタ、存在しない場合はNULLを返す
 */
//------------------------------------------------------------------
gfl2::str::MsgData * WordSetLoader::GetGlobalMsgData( u32 dat_id )
{
  for (u32 i = 0; i < PRELOAD_NO_MAX; ++ i )
  {
    if ( s_StaticBuffer[i].mGarcId == dat_id )
    {
      return s_StaticBuffer[i].mMsgData;
    }
  }
  return NULL;
}

//------------------------------------------------------------------
/**
 * @brief   常駐バッファ削除
 */
//------------------------------------------------------------------
void WordSetLoader::ReleaseGlobal( void )
{
  u32 temp_bits = s_ReadBits;
  for ( u32 count = 0; temp_bits != 0; ++ count, temp_bits >>= 1 )
  {
    if ( s_StaticBuffer[count].mMsgData )
    {
      GFL_DELETE s_StaticBuffer[count].mMsgData;
      GFL_ASSERT( temp_bits & 1 );  //@check フラグとの整合性チェック
    }
    s_StaticBuffer[count].mGarcId   = 0;
    s_StaticBuffer[count].mAddress  = NULL;
    s_StaticBuffer[count].mSize     = 0;
    s_StaticBuffer[count].mMsgData  = NULL;
  }
}























