//===========================================================================
/**
 * @file  gfl2_MsgData.cpp
 * @brief GAMEFREAK Library  String Print System ( MsgData )
 * @author  GAMEFREAK inc.
 * @date  2010.11.08
 */
//===========================================================================
#include <fs/include/gfl2_FsSystem.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <str/include/gfl2_MsgData.h>
#include <util/include/gfl2_std_string.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

//----------------------------------------------------------------------
// デバッグ出力有効
//----------------------------------------------------------------------
enum {
  MSGDAT_PRINT_FLAG = (gfl2::debug::PRINT_FLAG_CATEGORY_STRING) | (gfl2::debug::PRINT_FLAG_LEVEL2)
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
MsgData::MsgData( const char* arcFilePath, gfl2::fs::ArcFile::ARCDATID datID, gfl2::heap::HeapBase* pHeap, LoadType loadType )
 : m_pHeap(pHeap)
 , m_loadType(loadType)
 , m_dataID(0)
 , m_targetLangID(0)
 , m_pDataHeader(NULL)
 , m_pLangHeader(NULL)
 , m_pFileManager(NULL)
 , m_fileName(NULL)
 , m_arcID(gfl2::fs::ArcFile::ARCID_NULL)
{
  CreateFileName( arcFilePath );
  InitFileManager();
  construct_common( datID, loadType );
}
//--------------------------------------------------------------
// アーカイブID＆データIDから構築
//--------------------------------------------------------------
MsgData::MsgData( gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID datID, gfl2::heap::HeapBase* pHeap, LoadType loadType )
 : m_pHeap(pHeap)
 , m_loadType(loadType)
 , m_dataID(0)
 , m_targetLangID(0)
 , m_pDataHeader(NULL)
 , m_pLangHeader(NULL)
 , m_pFileManager(NULL)
 , m_fileName(NULL)
 , m_arcID(arcID)
{
  InitFileManager();
  construct_common( datID, loadType );
}

//--------------------------------------------------------------
// メモリ上に読み込み済みのデータから構築
//--------------------------------------------------------------
MsgData::MsgData( const void* dataAdress, gfl2::heap::HeapBase* pHeap )
 : m_pHeap(pHeap)
 , m_loadType(LOAD_FULL)
 , m_pFileManager(NULL)
 , m_fileName(NULL)
 , m_arcID(gfl2::fs::ArcFile::ARCID_NULL)
{
  m_pDataHeader    = static_cast<const MSG_HEADER_BLOCK*>( dataAdress );
  m_dataID         = 0;
  m_targetLangID   = getCurrentLangID( m_pDataHeader );
  loadLangHeader( &m_pLangHeader, m_pDataHeader, m_dataID, m_targetLangID );
}

//--------------------------------------------------------------
// デストラクタ
//--------------------------------------------------------------
MsgData::~MsgData()
{
  if( m_pFileManager != NULL )
  {
    if( m_pLangHeader != NULL ){
      GflHeapFreeMemory( m_pLangHeader );
    }

    if( m_pDataHeader != NULL ){
      GflHeapFreeMemory( const_cast<MSG_HEADER_BLOCK*>(m_pDataHeader) );
    }
  }

  if( m_pFileManager != NULL )
  {
    EndFileManager();
  }

  DeleteFileName();
}

//-----------------------------------------------------------------------------
/**
 * @func    CreateFileName
 * @brief   ファイル名を保存
 * @date    2014.10.28
 *
 * @param   file  ファイル名
 */
//-----------------------------------------------------------------------------
void MsgData::CreateFileName( const char * file )
{
  u32 len = strlen(file) + 1;
  m_fileName = static_cast<char*>( GflHeapAllocMemory(m_pHeap,len) );
  gfl2::std::MemCopy( file, m_fileName, len );
}

//-----------------------------------------------------------------------------
/**
 * @func    DeleteFileName
 * @brief   ファイル名を削除
 * @date    2014.10.28
 */
//-----------------------------------------------------------------------------
void MsgData::DeleteFileName(void)
{
  if( m_fileName != NULL )
  {
    GflHeapFreeMemory( m_fileName );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFileManager
 * @brief   ファイル読み込みマネージャ初期設定
 * @date    2014.10.28
 */
//-----------------------------------------------------------------------------
void MsgData::InitFileManager(void)
{
  m_pFileManager = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.fileName    = m_fileName;
  req.arcId       = m_arcID;
  req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
  req.heapForFile = m_pHeap;
  req.heapForReq  = m_pHeap->GetLowerHandle();
  req.memorizeCaller = false;   // リクエスト発行者を記録しない（メッセージは頻繁に開かれるため。）

  m_pFileManager->SyncArcFileOpen( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFileManager
 * @brief   ファイル読み込みマネージャ終了設定
 * @date    2014.10.28
 */
//-----------------------------------------------------------------------------
void MsgData::EndFileManager(void)
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.fileName    = m_fileName;
  req.arcId       = m_arcID;
  req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
  req.heapForReq  = m_pHeap;
  req.memorizeCaller = false;   // リクエスト発行者を記録しない（メッセージは頻繁に開かれるため。）

  m_pFileManager->SyncArcFileClose( req );
}

/**
 *  ↑コンストラクタ共通下請け／m_pArcが作成された後に呼び出すこと
 */
void MsgData::construct_common( gfl2::fs::ArcFile::ARCDATID datID, LoadType loadType )
{
  GFL_ASSERT_STOP( m_pFileManager != NULL );

  m_strParamBody.len = 0;
  m_strParamBody.offset = 0;
  m_strParamBody.userParam = 0;

  m_pDataHeader    = loadDataHeader( datID );
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
    m_pLangHeader = static_cast<MSG_LANG_BLOCK*>GflHeapAllocMemory( m_pHeap, m_pDataHeader->maxLangBlockSize );
    loadLangHeader( &m_pLangHeader, m_pDataHeader, m_dataID, m_targetLangID );
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    FileSeek
 * @brief   ファイルシーク
 * @date    2014.10.28
 *
 * @param   dataID  アークデータID
 * @param   offset  データIDからのオフセット
 */
//-----------------------------------------------------------------------------
/*
// 【使用禁止】代わりにFileSeekLoadDataPtrBufを使って下さい！
void MsgData::FileSeek( gfl2::fs::ArcFile::ARCDATID dataID, size_t offset )
{
  GFL_ASSERT_STOP( m_pFileManager );

  gfl2::fs::AsyncFileManager::ArcFileSeekDataPtrReq req;
  req.fileName       = m_fileName;
  req.arcId          = m_arcID;
  req.datId          = dataID;
  req.offsetFromData = offset;
  req.prio           = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;
  req.heapForReq     = NULL;

  m_pFileManager->SyncArcFileSeekDataPtr( req );
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    FileLoadDataPtrBuf
 * @brief   シークしたポインタからデータ読み込む
 * @date    2014.10.28
 *
 * @param   buf         読み込み場所
 * @param   read_size   読み込みサイズ
 */
//-----------------------------------------------------------------------------
/*
// 【使用禁止】代わりにFileSeekLoadDataPtrBufを使って下さい！
void MsgData::FileLoadDataPtrBuf( void * buf, size_t read_size )
{
  GFL_ASSERT_STOP( m_pFileManager );

  gfl2::fs::AsyncFileManager::ArcFileLoadDataPtrBufReq req;
  req.fileName      = m_fileName;                                   // [in]  ファイル名。終端は'\0'になっていること。
  req.arcId         = m_arcID;                                      // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
  req.readSize      = read_size;                                    // [in]  読み込みサイズ
  req.prio          = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
  req.pBuf          = buf;                                          // [out] 読み込んだデータの格納先( 4 byte alignment )
  req.bufSize       = read_size;                                    // [in]  pBufのサイズ。readSize以上にして下さい。
  req.pRealReadSize = NULL;                                         // [out] 実際に読み込んだバイト数。NULLでもよい。
  req.heapForReq    = NULL;                                         // [in]  リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。

  m_pFileManager->SyncArcFileLoadDataPtrBuf( req );
}
*/

//-----------------------------------------------------------------------------
/**
 * @func    FileSeekLoadDataPtrBuf
 * @brief   ファイルシーク＆シークしたポインタからデータ読み込む
 * @date    2016.05.23
 *
 * @param   dataID  アークデータID
 * @param   offset  データIDからのオフセット
 * @param   buf         読み込み場所
 * @param   read_size   読み込みサイズ
 */
//-----------------------------------------------------------------------------
void MsgData::FileSeekLoadDataPtrBuf( gfl2::fs::ArcFile::ARCDATID dataID, size_t offset, void * buf, size_t read_size )
{
  GFL_ASSERT_STOP( m_pFileManager );

  gfl2::fs::AsyncFileManager::ArcFileLoadDataPieceBufReq req;
  req.fileName       = m_fileName;                                   //!< [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
  req.arcId          = m_arcID;                                      //!< [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
  req.datId          = dataID;                                       //!< [in]  アーカイブファイル内データID
  req.offsetFromData = offset;                                       //!< [in]  読み込み開始オフセット(byte)(datIdデータ先頭からのオフセット)
  req.readSize       = read_size;                                    //!< [in]  読み込みサイズ。offsetFromDataでシークしたポインタからreadSizeだけ読み込む。データ1件のサイズより大きなサイズを指定することも可。
  req.prio           = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  //!< [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
  req.pBuf           = buf;                                          //!< [out] 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
  req.bufSize        = read_size;                                    //!< [in]  bufのサイズ。データサイズ(パディングを含む)以上にして下さい。
  req.pRealReadSize  = NULL;                                         //!< [out] 実際に読み込んだサイズ。NULLでもよい。
  req.heapForReq     = NULL;                                         //!< [in]  リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
  
  m_pFileManager->SyncArcFileLoadDataPieceBuf( req );
}


MsgData::MSG_HEADER_BLOCK* MsgData::loadDataHeader( u32 dataID )
{
  u8 tmp_buffer[ sizeof(MSG_HEADER_BLOCK) ];

  MSG_HEADER_BLOCK *header;
  MSG_HEADER_BLOCK *tmp_header;

  FileSeekLoadDataPtrBuf( dataID, 0, static_cast<void *>(tmp_buffer), sizeof(tmp_buffer) );

  tmp_header = reinterpret_cast<MsgData::MSG_HEADER_BLOCK*>(tmp_buffer);
  {
    u32 headerSize = sizeof(MSG_HEADER_BLOCK) + (sizeof(u32)*tmp_header->numLangs);
    header = static_cast<MSG_HEADER_BLOCK*>( GflHeapAllocMemory( m_pHeap, headerSize ) );
  }

  if( header )
  {
    gfl2::std::MemCopy( tmp_header, header, sizeof(MSG_HEADER_BLOCK) );
    FileSeekLoadDataPtrBuf( dataID, 0+sizeof(tmp_buffer), header->ofsLangBlocks, sizeof(u32)*header->numLangs );  // 前回のFileSeekLoadDataPtrBufの続きから読めるように開始オフセットを設定する
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
void MsgData::loadLangHeader(
      MSG_LANG_BLOCK** ppLangHeader, const MSG_HEADER_BLOCK* pDataHeader, gfl2::fs::ArcFile::ARCDATID dataID, u32 langID )
{
  u32  langBlockOfs = getTargetLangBlockOfs( pDataHeader, langID );

  // m_pFileManager!=NULL なら自前でロードしたデータ
  if( m_pFileManager != NULL )
  {
    GFL_ASSERT( *ppLangHeader );

    FileSeekLoadDataPtrBuf( dataID, langBlockOfs, static_cast<void*>(*ppLangHeader), sizeof(MSG_LANG_BLOCK) );

    FileSeekLoadDataPtrBuf( dataID, langBlockOfs, static_cast<void*>(*ppLangHeader), (*ppLangHeader)->size );
  }
  // m_pFileManager==NULL なら外部データ構築とみなす
  else
  {
    *(ppLangHeader) = reinterpret_cast<MSG_LANG_BLOCK*>( reinterpret_cast<uptr>(pDataHeader) + langBlockOfs );
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
u32 MsgData::getCurrentLangID( const MSG_HEADER_BLOCK* dataHeader ) const
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
u32 MsgData::getTargetLangBlockOfs( const MSG_HEADER_BLOCK* dataHeader, u32 langID ) const
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
u16 MsgData::GetString( u32 strID, StrBuf& dst )
{
  if( strID >= m_pDataHeader->numStrings )
  {
    GFL_ASSERT_MSG(0, "strID=%d, numStr=%d dataID=%d", strID, m_pDataHeader->numStrings, m_dataID);
    dst.Clear();
    return 0;
  }

  checkTargetLangChanged();

  {
    const MSG_STRPARAM_BLOCK* param = getStrParamPointer( strID );

    // 逐次モード
    if( m_loadType == LOAD_PART )
    {
      wchar_t*  buf;
      u32  size;

      size = param->len * sizeof(wchar_t);
      buf = static_cast<wchar_t*>GflHeapAllocMemoryAlign( m_pHeap, size, -(m_pHeap->GetDefaultAlign()) );
      if( buf )
      {
        u32  langBlockOfs = getTargetLangBlockOfs( m_pDataHeader, m_targetLangID ) + param->offset;
        FileSeekLoadDataPtrBuf( m_dataID, langBlockOfs, buf, size );

        putToStrBuf( buf, param->len, strID, dst );

        GflHeapFreeMemory( buf );
      }
    }
    // 高速モード
    else
    {
      wchar_t* str = reinterpret_cast<wchar_t*>( reinterpret_cast<uptr>(m_pLangHeader) + param->offset );
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
u16 MsgData::GetStringByLanguage( u32 strID, u32 langID, StrBuf& dst )
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
u16 MsgData::GetUserParam( u32 strID )
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
void MsgData::putToStrBuf( const wchar_t* src, u32 strLen, u32 strID, StrBuf& dst ) const
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
u32 MsgData::GetStrCount( void ) const
{
  return m_pDataHeader->numStrings;
}


//--------------------------------------------------------------------------
/**
 * 読み込み対象の言語IDが変更されているかどうかチェック
 * 変更されていれば、必要に応じて内部データを更新する。
 */
//--------------------------------------------------------------------------
void MsgData::checkTargetLangChanged( void )
{
  u8 currentLangID = getCurrentLangID( m_pDataHeader );

  if( m_targetLangID != currentLangID )
  {
    GFL_PRINT_EX( MSGDAT_PRINT_FLAG, "[MSGDAT] targetLangChanged %d->%d\n", m_targetLangID, currentLangID );

    m_targetLangID = currentLangID;

    if( m_loadType != LOAD_PART )
    {
      loadLangHeader( &m_pLangHeader, m_pDataHeader, m_dataID, m_targetLangID );
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
const MsgData::MSG_STRPARAM_BLOCK* MsgData::getStrParamPointer( u32 strID )
{
  // 逐次モード
  if( m_loadType == LOAD_PART )
  {
    // ファイルからハンドラが内包する変数領域に読み込み、そのアドレスを返している
    u32 offset = getTargetLangBlockOfs( m_pDataHeader, m_targetLangID )
                + sizeof(MSG_LANG_BLOCK) + sizeof(MSG_STRPARAM_BLOCK) * strID;

    FileSeekLoadDataPtrBuf( m_dataID, offset, &m_strParamBody, sizeof(MSG_STRPARAM_BLOCK) );

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
/*  日本語版の『ひらがな-漢字』切り替えとか、                                                */
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
void MsgData::SetLangID( u32 langID )
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
u32 MsgData::GetLangID( void )
{
  return s_CurrentLangID;
}

GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)
