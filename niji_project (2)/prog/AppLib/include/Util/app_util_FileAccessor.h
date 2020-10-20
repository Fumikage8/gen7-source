//=============================================================================
/**
 * @file    app_util_FileAccessor.h
 * @brief   ファイルアクセス関連
 * @author  Hiroyuki Nakamura
 * @date    15.02.26
 */
//=============================================================================
#if !defined( APP_UTIL_FILE_ACCESSOR_H_INCLUDED )
#define APP_UTIL_FILE_ACCESSOR_H_INCLUDED

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

class FileAccessor
{
	GFL_FORBID_COPY_AND_ASSIGN(FileAccessor);
public:
	//! リクエストパラメータ（メモリ確保版）
	struct RequestData
	{
		gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
		gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadr;
		gfl2::fs::AsyncFileManager::ArcFileCloseReq closer;
	};

	//! リクエストパラメータ（メモリ確保済み版）
	struct BufferRequestData
	{
		gfl2::fs::AsyncFileManager::ArcFileOpenReq openr;
		gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq loadr;
		gfl2::fs::AsyncFileManager::ArcFileCloseReq closer;
	};
public:
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
  static void FileOpenSync( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap, bool close_soon = false, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

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
  static void FileOpen( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap, bool close_soon = false, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

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
  static bool IsFileOpen( gfl2::fs::ArcFile::ARCID arc_id );


  //-----------------------------------------------------------------------------
  /**
   * @func    FileCloseSync
   * @brief   アークファイルを閉じる（同期版）
   * @date    2015.02.26
   *
   * @param   arc_id  アークID
   */
  //-----------------------------------------------------------------------------
  static void FileCloseSync( gfl2::fs::ArcFile::ARCID arc_id );

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
  static void FileClose( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap );

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
  static bool IsFileClose( gfl2::fs::ArcFile::ARCID arc_id );

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
  static void FileLoadSync(
                gfl2::fs::ArcFile::ARCID arc_id,
                gfl2::fs::ArcFile::ARCDATID dat_id,
                void ** buf,
                gfl2::heap::HeapBase * heap,
                bool is_comp = true,
                u32 align = 4,
                gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG,
                size_t * buf_size = NULL,
                size_t * read_size = NULL );

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
  static void FileLoad(
                gfl2::fs::ArcFile::ARCID arc_id,
                gfl2::fs::ArcFile::ARCDATID dat_id,
                void ** buf,
                gfl2::heap::HeapBase * heap,
                bool is_comp = true,
                u32 align = 4,
                gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG,
                size_t * buf_size = NULL,
                size_t * read_size = NULL );

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
  static bool IsFileLoad( void ** buf );

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
  static void FileLoadBufSync(
                gfl2::fs::ArcFile::ARCID arc_id,
                gfl2::fs::ArcFile::ARCDATID dat_id,
                void * buf,
                size_t buf_size,
                gfl2::heap::HeapBase * heap,
                bool is_comp = true,
                u32 align_comp = 4,
                gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

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
  static void FileLoadBuf(
                gfl2::fs::ArcFile::ARCID arc_id,
                gfl2::fs::ArcFile::ARCDATID dat_id,
                void * buf,
                size_t buf_size,
                gfl2::heap::HeapBase * heap,
                bool is_comp = true,
                u32 align_comp = 4,
                gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

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
  static bool IsFileLoadBuf( void * buf );

#if PM_DEBUG
  static void DebugPrintFileManager( bool flg );
#endif



private:
  //-----------------------------------------------------------------------------
  /**
   * @func    SetFileOpenData
   * @brief   アークファイルオープンのデータを設定
   * @date    2015.02.26
   *
   * @param   arc_id      アークID
   * @param   heap        ヒープ
   * @param   close_soon  処理の最初に読み込みを終わらせてすぐに閉じるときtrue、処理の最後まで開きっぱなしにするときfalse
   * @param   req         データ設定先
   * @param   lang        対象言語
   */
  //-----------------------------------------------------------------------------
  static void SetFileOpenData(
          gfl2::fs::ArcFile::ARCID arc_id,
          gfl2::heap::HeapBase * heap,
          bool close_soon,
          gfl2::fs::ToolDefine::ArcLangType::Tag lang,
          gfl2::fs::AsyncFileManager::ArcFileOpenReq * req );

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
  static void SetFileCloseData( gfl2::fs::ArcFile::ARCID arc_id, gfl2::heap::HeapBase * heap, gfl2::fs::AsyncFileManager::ArcFileCloseReq * req );

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
  static void SetFileLoadData(
          gfl2::fs::ArcFile::ARCID arc_id,
          gfl2::fs::ArcFile::ARCDATID dat_id,
          void ** buf,
          size_t * buf_size,
          size_t * read_size,
          gfl2::heap::HeapBase * heap,
          gfl2::fs::AsyncFileManager::ArcFileLoadDataReq * req,
          bool is_comp,
          u32 align,
          gfl2::fs::ToolDefine::ArcLangType::Tag lang );

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
  static void SetFileLoadBufData(
                gfl2::fs::ArcFile::ARCID arc_id,
                gfl2::fs::ArcFile::ARCDATID dat_id,
                void * buf,
                size_t buf_size,
                gfl2::heap::HeapBase * heap,
                gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq * req,
                bool is_comp,
                u32 align_comp,
                gfl2::fs::ToolDefine::ArcLangType::Tag lang );
};


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

#endif  // APP_UTIL_FILE_ACCESSOR_H_INCLUDED
