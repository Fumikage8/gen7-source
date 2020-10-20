//===========================================================================
/**
 * @file  gfl2_Font.cpp
 * @brief GAMEFREAK Library  String Print System ( Font )
 * @author  GAMEFREAK inc.
 * @date  2010.10.27
 */
//===========================================================================
#include <fs/include/gfl2_FsSystem.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <str/include/gfl2_Font.h>
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

//----------------------------------------------------------------------
// デバッグ出力有効
//----------------------------------------------------------------------
enum {
  FONT_PRINT_FLAG = gfl2::debug::PRINT_FLAG_CATEGORY_STRING | gfl2::debug::PRINT_FLAG_LEVEL2
};


//----------------------------------------------------------------------
/**
 * @brief   リソースフォントクラス作成
 *
 * @param   fontFilePath    [in] フォントファイルのパスを指定
 * @param   heap            [in] 構築用のメモリ確保に使うヒープハンドル
 * @param   devHeap         [in] 構築用のデバイスメモリ確保に使うヒープハンドル
 * @param   heapForTemp     [in] 構築中に一時的に使用するヒープハンドル
 *
 * @retval  ResFont*        構築できたら有効なフォントクラスポインタ／失敗したらNULL
 */
//----------------------------------------------------------------------
ResFont*
CreateResFont( const char* fontFilePath, gfl2::heap::HeapBase* heap, gfl2::heap::HeapBase* devHeap, gfl2::heap::HeapBase* heapForTemp )
{
  ResFont* pFont = GFL_NEW(heap) ResFont();
  void * buffer = NULL;

  // フォントリソースをロードします
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();

  gfl2::fs::AsyncFileManager::FileEasyReadReq req;
  req.fileName          = fontFilePath;                                 // [in]  ファイル名。終端は'\0'になっていること。
  req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
  req.ppBuf             = &buffer;                                      // [out] heapForBufから確保したメモリが渡されるポインタ。呼び出し元でGflHeapFreeMemory(*ppBuf)で解放して下さい。  // ****キー****
  req.pBufSize          = NULL;                                         // [out] heapForBufから確保したメモリのサイズ。heapの管理領域のサイズは含まれていない。NULLでもよい。
  req.pFileSize         = NULL;                                         // [out] ファイルサイズ。(*pBufSize >= *pFileSize)となる。NULLでもよい。
  req.heapForBuf        = devHeap;                                      // [in]  ファイルから読み込んだ内容を入れておくバッファを確保するためのヒープ。
  req.align             = nw::font::GlyphDataAlignment;                 // [in]  heapForBufから確保する際のアラインメント
  req.heapForReq        = heapForTemp;                                         // [in]  リクエスト終了まで一時的に使用するヒープ。
  req.heapForCompressed = NULL;                                         // [in]  解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。

  man->SyncFileEasyRead( req );

  // フォントリソースをセットします
  bool bSuccess = pFont->SetResource( buffer );

  //--- 既にリソースをセット済みであるか，ロード済みであるか、リソースが不正な場合に失敗します。
  if( !bSuccess )
  {
    GFL_ASSERT(0);
    GflHeapFreeMemory( buffer );
    return NULL;
  }

  GFL_PRINT_EX( FONT_PRINT_FLAG, "[FONT] ResFont create succeed\n" );

  return pFont;
}

//----------------------------------------------------------------------
/**
 * @brief   リソースフォントクラス破棄
 *
 * @param   pFont       [in] フォントクラスポインタ
 */
//----------------------------------------------------------------------
void
DeleteResFont( ResFont* pFont )
{
  GFL_ASSERT(pFont);
  if( pFont ){
    void * buffer;

    buffer = pFont->RemoveResource();
    GflHeapFreeMemory( buffer );

    GFL_DELETE( pFont );
  }
}


GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)
