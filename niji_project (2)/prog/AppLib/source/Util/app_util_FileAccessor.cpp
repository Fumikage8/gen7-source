//=============================================================================
/**
 * @file    app_util_FileAccessor.cpp
 * @brief   ファイルアクセス関連
 * @author  Hiroyuki Nakamura
 * @date    15.02.26
 */
//=============================================================================
#include <AppLib/include/Util/app_util_FileAccessor.h>

#include <base/include/gfl2_Singleton.h>

#include <GameSys/include/GameManager.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

//-----------------------------------------------------------------------------
/**
 * @func    SetFileOpenData
 * @brief   アークファイルオープンのデータを設定
 * @date    2015.02.26
 *
 * @param   arc_id      アークID
 * @param   heap        ヒープ
 * @param   close_soon  処理の最初に読み込みを終わらせてすぐに閉じるときtrue、処理の最後まで開きっぱなしにするときfalse
 * @param   lang        対象言語
 * @param   req         データ設定先
 */
//-----------------------------------------------------------------------------
void FileAccessor::SetFileOpenData(
      gfl2::fs::ArcFile::ARCID arc_id,
      gfl2::heap::HeapBase * heap,
      bool close_soon,
      gfl2::fs::ToolDefine::ArcLangType::Tag lang,
      gfl2::fs::AsyncFileManager::ArcFileOpenReq * req )
{
  req->arcId       = arc_id;                                        // ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
  req->heapForFile = (close_soon)?(heap->GetLowerHandle()):(heap);  // ヒープ。ファイルインスタンスの作成と索引テーブルの作成に使用。Closeするまで左記のものが保持される。
  req->heapForReq  = heap->GetLowerHandle();                        // リクエスト終了まで一時的に使用するヒープ。索引テーブルの作成のためにも使用する。ArcSrcのインスタンスやソーステーブルの作成のためにも使用する。
  req->lang        = lang;
}

//-----------------------------------------------------------------------------
/**
 * @func    FileOpenSync
 * @brief   アークファイルを開く（同期版）
 * @date    2015.02.26
 *
 * @param   arc_id      アークID
 * @param   heap        ヒープ
 * @param   close_soon  処理の最初に読み込みを終わらせてすぐに閉じるときtrue、処理の最後まで開きっぱなしにするときfalse
 * @param   lang        対象言語
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileOpenSync( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap, bool close_soon, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  SetFileOpenData( arc_id, heap, close_soon, lang, &req );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->SyncArcFileOpen( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    FileOpen
 * @brief   アークファイルを開く（非同期版）
 * @date    2015.02.26
 *
 * @param   arc_id      アークID
 * @param   heap        ヒープ
 * @param   close_soon  処理の最初に読み込みを終わらせてすぐに閉じるときtrue、処理の最後まで開きっぱなしにするときfalse
 * @param   lang        対象言語
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileOpen( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap, bool close_soon, gfl2::fs::ToolDefine::ArcLangType::Tag lang )
{
  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  SetFileOpenData( arc_id, heap, close_soon, lang, &req );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->AddArcFileOpenReq( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileOpen
 * @brief   アークファイルオープンチェック
 * @date    2015.02.26
 *
 * @param   arc_id  アークID
 *
 * @retval  false = オープン中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool FileAccessor::IsFileOpen( gfl2::fs::ArcFile::ARCID arc_id )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  return man->IsArcFileOpenFinished( arc_id );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetFileCloseData
 * @brief   アークファイルクローズのデータを設定
 * @date    2015.02.26
 *
 * @param   arc_id  アークID
 * @param   heap    ヒープ
 * @param   req     データ設定先
 */
//-----------------------------------------------------------------------------
void FileAccessor::SetFileCloseData( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap, gfl2::fs::AsyncFileManager::ArcFileCloseReq * req )
{
  req->arcId = arc_id;
  if( heap != NULL )
  {
    req->heapForReq = heap->GetLowerHandle(); // リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
  }
}

//-----------------------------------------------------------------------------
/**
 * @func    FileCloseSync
 * @brief   アークファイルを閉じる（同期版）
 * @date    2015.02.26
 *
 * @param   arc_id  アークID
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileCloseSync( gfl2::fs::ArcFile::ARCID arc_id )
{
  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  SetFileCloseData( arc_id, NULL, &req );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->SyncArcFileClose( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    FileClose
 * @brief   アークファイルを閉じる（非同期版）
 * @date    2015.02.26
 *
 * @param   arc_id  アークID
 * @param   heap    ヒープ
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileClose( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap )
{
  GFL_ASSERT_STOP( heap != NULL );

  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  SetFileCloseData( arc_id, heap, &req );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->AddArcFileCloseReq( req );
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileClose
 * @brief   アークファイルクローズチェック
 * @date    2015.02.26
 *
 * @param   arc_id  アークID
 *
 * @retval  false = クローズ中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool FileAccessor::IsFileClose( gfl2::fs::ArcFile::ARCID arc_id )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  return man->IsArcFileCloseFinished( arc_id );
}

//-----------------------------------------------------------------------------
/*
 * @func    SetFileLoadData
 * @brief   ファイル読み込みのデータを設定
 * @date    2015.02.26
 *
 * @param   arc_id    アークID
 * @param   dat_id    アークデータID
 * @param   buf       データ展開場所
 * @param   buf_size  確保したバッファサイズ
 * @param   read_size 読み込んだデータサイズ
 * @param   heap      ヒープ
 * @param   req       データ設定先
 * @param   is_comp   true = 圧縮データ
 * @param   align     アライメント
 * @param   lang      対象言語
 */
//-----------------------------------------------------------------------------
void FileAccessor::SetFileLoadData(
      gfl2::fs::ArcFile::ARCID arc_id,
      gfl2::fs::ArcFile::ARCDATID dat_id,
      void ** buf,
      size_t * buf_size,
      size_t * read_size,
      gfl2::heap::HeapBase * heap,
      gfl2::fs::AsyncFileManager::ArcFileLoadDataReq * req,
      bool is_comp,
      u32 align,
      gfl2::fs::ToolDefine::ArcLangType::Tag lang )
{
  req->arcId      = arc_id;                  // ARCID。ARCID_NULLのときはfileNameが使われる。
  req->datId      = dat_id;                  // アーカイブファイル内データID
  req->ppBuf      = buf;                     // heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
  req->pBufSize   = buf_size;                // heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
  req->pRealReadSize = read_size;            // 実際に読み込んだサイズ。heapForCompressedがNULLでないときは解凍後のサイズ。NULLでもよい。
  req->heapForBuf = heap;                    // ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
  req->align      = align;                   // heapForBufから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。圧縮ファイルも同じアラインメントで読みます。
  req->heapForReq = heap->GetLowerHandle();  // リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。
  req->heapForCompressed = (is_comp)?(heap->GetLowerHandle()):(NULL);  // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
  req->lang       = lang;
}

//-----------------------------------------------------------------------------
/*
 * @func    FileLoadSync
 * @brief   ファイル読み込み（同期版）
 * @date    2015.02.26
 *
 * @param   arc_id    アークID
 * @param   dat_id    アークデータID
 * @param   buf       データ展開場所
 * @param   heap      ヒープ
 * @param   is_comp   true = 圧縮データ
 * @param   align     アライメント
 * @param   lang      対象言語
 * @param   buf_size  確保したバッファサイズ
 * @param   read_size 読み込んだデータサイズ
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileLoadSync(
      gfl2::fs::ArcFile::ARCID arc_id,
      gfl2::fs::ArcFile::ARCDATID dat_id,
      void ** buf,
      gfl2::heap::HeapBase * heap,
      bool is_comp,
      u32 align,
      gfl2::fs::ToolDefine::ArcLangType::Tag lang,
      size_t * buf_size,
      size_t * read_size )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  SetFileLoadData( arc_id, dat_id, buf, buf_size, read_size, heap, &req, is_comp, align, lang );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->SyncArcFileLoadData( req );
}

//-----------------------------------------------------------------------------
/*
 * @func    FileLoad
 * @brief   ファイル読み込み（非同期版）
 * @date    2015.02.26
 *
 * @param   arc_id    アークID
 * @param   dat_id    アークデータID
 * @param   buf       データ展開場所
 * @param   heap      ヒープ
 * @param   is_comp   true = 圧縮データ
 * @param   align     アライメント
 * @param   lang      対象言語
 * @param   buf_size  確保したバッファサイズ
 * @param   read_size 読み込んだデータサイズ
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileLoad(
      gfl2::fs::ArcFile::ARCID arc_id,
      gfl2::fs::ArcFile::ARCDATID dat_id,
      void ** buf,
      gfl2::heap::HeapBase * heap,
      bool is_comp,
      u32 align,
      gfl2::fs::ToolDefine::ArcLangType::Tag lang,
      size_t * buf_size,
      size_t * read_size )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
  SetFileLoadData( arc_id, dat_id, buf, buf_size, read_size, heap, &req, is_comp, align, lang );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->AddArcFileLoadDataReq( req );
}

//-----------------------------------------------------------------------------
/*
 * @func    IsFileLoad
 * @brief   ファイル読み込み終了チェック
 * @date    2015.02.26
 *
 * @param   buf   データ展開場所
 *
 * @retval  false = 読み込み中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool FileAccessor::IsFileLoad( void ** buf )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  return man->IsArcFileLoadDataFinished( buf );
}

//-----------------------------------------------------------------------------
/*
 * @func    SetFileLoadBufData
 * @brief   ファイル読み込みのデータを設定（メモリ確保済み版）
 * @date    2015.02.26
 *
 * @param   arc_id      アークID
 * @param   dat_id      アークデータID
 * @param   buf         データ展開場所
 * @param   buf_size    bufのサイズ
 * @param   heap        ヒープ
 * @param   req         データ設定先
 * @param   is_comp     true = 圧縮データ
 * @param   align_comp  is_compがtrueのときの圧縮データを読み込む際のアラインメント
 * @param   lang        対象言語
 */
//-----------------------------------------------------------------------------
void FileAccessor::SetFileLoadBufData(
      gfl2::fs::ArcFile::ARCID arc_id,
      gfl2::fs::ArcFile::ARCDATID dat_id,
      void * buf,
      size_t buf_size,
      gfl2::heap::HeapBase * heap,
      gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq * req,
      bool is_comp,
      u32 align_comp,
      gfl2::fs::ToolDefine::ArcLangType::Tag lang )
{
  req->arcId              = arc_id;                  // ARCID。ARCID_NULLのときはfileNameが使われる。
  req->datId              = dat_id;                  // アーカイブファイル内データID
  req->pBuf               = buf;                     // 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
  req->bufSize            = buf_size;                // pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
  req->alignForCompressed = align_comp;              // 圧縮ファイルを読み込む際にheapForCompressedから確保する際のアラインメント。2のべき乗(1, 2, 4, 8, 16, 32, 64, 128, ...)にして下さい。解凍しない場合は0でよい。
  req->heapForReq         = heap->GetLowerHandle();  // リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
  req->heapForCompressed  = (is_comp)?(heap->GetLowerHandle()):(NULL);  // 解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。
  req->lang               = lang;
}

//-----------------------------------------------------------------------------
/*
 * @func    FileLoadBufSync
 * @brief   ファイル読み込み（メモリ確保済み/同期版）
 * @date    2015.02.26
 *
 * @param   arc_id      アークID
 * @param   dat_id      アークデータID
 * @param   buf         データ展開場所
 * @param   buf_size    bufのサイズ
 * @param   heap        ヒープ
 * @param   is_comp     true = 圧縮データ
 * @param   align_comp  is_compがtrueのときの圧縮データを読み込む際のアラインメント
 * @param   lang        対象言語
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileLoadBufSync(
      gfl2::fs::ArcFile::ARCID arc_id,
      gfl2::fs::ArcFile::ARCDATID dat_id,
      void * buf,
      size_t buf_size,
      gfl2::heap::HeapBase * heap,
      bool is_comp,
      u32 align_comp,
      gfl2::fs::ToolDefine::ArcLangType::Tag lang )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
  SetFileLoadBufData( arc_id, dat_id, buf, buf_size, heap, &req, is_comp, align_comp, lang );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->SyncArcFileLoadDataBuf( req );
}

//-----------------------------------------------------------------------------
/*
 * @func    FileLoadBuf
 * @brief   ファイル読み込み（メモリ確保済み/非同期版）
 * @date    2015.02.26
 *
 * @param   arc_id      アークID
 * @param   dat_id      アークデータID
 * @param   buf         データ展開場所
 * @param   buf_size    bufのサイズ
 * @param   heap        ヒープ
 * @param   is_comp     true = 圧縮データ
 * @param   align_comp  is_compがtrueのときの圧縮データを読み込む際のアラインメント
 * @param   lang        対象言語
 */
//-----------------------------------------------------------------------------
void FileAccessor::FileLoadBuf(
      gfl2::fs::ArcFile::ARCID arc_id,
      gfl2::fs::ArcFile::ARCDATID dat_id,
      void * buf,
      size_t buf_size,
      gfl2::heap::HeapBase * heap,
      bool is_comp,
      u32 align_comp,
      gfl2::fs::ToolDefine::ArcLangType::Tag lang )
{
  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
  SetFileLoadBufData( arc_id, dat_id, buf, buf_size, heap, &req, is_comp, align_comp, lang );

  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  man->AddArcFileLoadDataBufReq( req );
}

//-----------------------------------------------------------------------------
/*
 * @func    IsLoadBuf
 * @brief   ファイル読み込み終了チェック（メモリ確保済み版）
 * @date    2015.02.26
 *
 * @param   buf   データ展開場所
 *
 * @retval  false = 読み込み中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool FileAccessor::IsFileLoadBuf( void * buf )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  return man->IsArcFileLoadDataBufFinished( &buf );
}


#if PM_DEBUG
void FileAccessor::DebugPrintFileManager( bool flg )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();

  if( flg != false )
  {
    man->PrintFsDebugLogArcFileInfo();
  }
  man->ClearFsDebugLogArcFileInfo();
}
#endif


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
