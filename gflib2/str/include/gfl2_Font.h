//===========================================================================
/**
 * @file  gfl2_Font.h
 * @brief GAMEFREAK Library  String Print System ( Font )
 * @author  GAMEFREAK inc.
 * @date  2010.10.27
 */
//===========================================================================
#if !defined( GFL2_FONT_H_INCLUDED )
#define GFL2_FONT_H_INCLUDED
#pragma once

#if defined( GF_PLATFORM_CTR )
#include <nw/font2.h>
#elif defined( GF_PLATFORM_WIN32 )
#include <nw/font.h>
#endif
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)

/**
 *  フォント基底クラス（インスタンス作成不可）
 */
typedef nw::font::Font Font;

/**
 *  リソースフォントクラス
 * （他タイプのフォントを導入するかどうかは未確定。基本的にこれを使ってください）
 */
typedef nw::font::ResFont ResFont;


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
CreateResFont(  const char* fontFilePath,
                  gfl2::heap::HeapBase* heap,
                  gfl2::heap::HeapBase* devHeap,
                  gfl2::heap::HeapBase* heapForTemp
  );

//----------------------------------------------------------------------
/**
 * @brief   リソースフォントクラス破棄
 *
 * @param   pFont       [in] フォントクラスポインタ
 */
//----------------------------------------------------------------------
void
  DeleteResFont( ResFont* pFont );


GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)


#endif // GFL2_FONT_H_INCLUDED
