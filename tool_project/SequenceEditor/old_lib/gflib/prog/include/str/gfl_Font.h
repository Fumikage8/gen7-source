#ifndef __GFL_FONT_H__
#define __GFL_FONT_H__
//===========================================================================
/**
 * @file  gfl_Font.h
 * @brief GAMEFREAK Library  String Print System ( Font )
 * @author  GAMEFREAK inc.
 * @date  2010.10.27
 */
//===========================================================================
#pragma once

#include <nn/types.h>
#include <nw/font.h>
#include <gfl_Heap.h>

namespace gfl {
namespace str {

/**
 *  フォント基底クラス（インスタンス作成不可）
 */
typedef nw::font::Font           Font;

/**
 *  リソースフォントクラス
 * （他タイプのフォントを導入するかどうかは未確定。基本的にこれを使ってください）
 */
typedef nw::font::ResFont        ResFont;


  //----------------------------------------------------------------------
  /**
   * @brief   リソースフォントクラス作成
   *
   * @param   fontFilePath    [in] フォントファイルのパスを指定
   * @param   heap            [in] 構築用のメモリ確保に使うヒープハンドル
   * @param   devHeap         [in] 構築用のデバイスメモリ確保に使うヒープハンドル
   *
   * @retval  ResFont*        構築できたら有効なフォントクラスポインタ／失敗したらNULL
   */
  //----------------------------------------------------------------------
    ResFont*
      CreateResFont(  const char* fontFilePath,
                      gfl::heap::HeapBase* heap,
                      gfl::heap::HeapBase* devHeap
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


} /* end of namespace str */
} /* end of namespace gfl */



#endif // __GFL_FONT_H__
