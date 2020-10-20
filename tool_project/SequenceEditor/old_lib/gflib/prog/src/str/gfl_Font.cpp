//===========================================================================
/**
 * @file  gfl_Font.cpp
 * @brief GAMEFREAK Library  String Print System ( Font )
 * @author  GAMEFREAK inc.
 * @date  2010.10.27
 */
//===========================================================================

#include <nw/font.h>

#include <gfl_Common.h>
#include <gfl_fs.h>
#include <gfl_Heap.h>

#include "str/gfl_Font.h"

namespace gfl {
namespace str {

//----------------------------------------------------------------------
// デバッグ出力有効
//----------------------------------------------------------------------
enum {
  FONT_PRINT_FLAG = gfl::debug::PRINT_FLAG_CATEGORY_STRING | gfl::debug::PRINT_FLAG_LEVEL2
};




//----------------------------------------------------------------------
/**
 * @brief   リソースフォントクラス作成
 *
 * @param   fontFilePath    [in] フォントファイルのパスを指定
 * @param   heap            [in] 構築用のメモリ確保に使うヒープハンドル
 *
 * @retval  ResFont*        構築できたら有効なフォントクラスポインタ／失敗したらNULL
 */
//----------------------------------------------------------------------
ResFont*
CreateResFont( const char* fontFilePath, gfl::heap::HeapBase* heap, gfl::heap::HeapBase* devHeap )
{
  ResFont  *pFont = GFL_NEW(heap) ResFont();

  // フォントリソースをロードします
  gfl::fs::ReadFile fontReader( fontFilePath );

  u32 fileSize = fontReader.GetSize();
  if ( fileSize == 0 ){
    GFL_ASSERT(0);
    return false;
  }

  NN_LOG( "[FONT] Begin create ResFont, path=%s, size=%08x, align=%d\n",
              fontFilePath, fileSize, nw::font::GlyphDataAlignment );

  void* buffer = GflHeapAllocMemoryAlign( devHeap, fileSize, nw::font::GlyphDataAlignment );
  if( buffer == NULL )
  {
    GFL_ASSERT(0);
    return NULL;
  }

  s32 readSize = fontReader.Read( buffer, fileSize );
  if (readSize != fileSize)
  {
    GFL_ASSERT(0);
    GflHeapFreeMemory( buffer );
    return NULL;
  }

  // フォントリソースをセットします
  bool bSuccess = pFont->SetResource( buffer );

  //--- 既にリソースをセット済みであるか，ロード済みであるか、リソースが不正な場合に失敗します。
  if( !bSuccess )
  {
    GFL_ASSERT(0);
    GflHeapFreeMemory( buffer );
    return NULL;
  }

  // 描画用バッファを設定します。
  const u32 drawBufferSize = ResFont::GetDrawBufferSize( buffer );

  GFL_PRINT_EX( FONT_PRINT_FLAG, "[FONT] drawBufferSize=%08x\n", drawBufferSize );

  void* drawBuffer = GflHeapAllocMemoryAlign( heap, drawBufferSize, 4 );
  if( drawBuffer == NULL ){
    GFL_ASSERT(0);
    GflHeapFreeMemory( buffer );
    return NULL;
  }

  pFont->SetDrawBuffer( drawBuffer );

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

    buffer = pFont->GetDrawBuffer();
    GflHeapFreeMemory( buffer );

    buffer = pFont->RemoveResource();
    GflHeapFreeMemory( buffer );

    GFL_DELETE( pFont );
  }
}



} /* end of namespace str */
} /* end of namespace gfl */



