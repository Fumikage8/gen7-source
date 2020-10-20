//=============================================================================
/**
 * @file    QuickMatchDrawListener.h
 * @brief   クイックマッチ画面リスナー
 * @author  kousaka_shin
 * @date    
 */
//=============================================================================
#if !defined( QUICKMATCH_DRAW_LISTENER_H_INCLUDED )
#define QUICKMATCH_DRAW_LISTENER_H_INCLUDED

#pragma once

// gflib2
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(FieldMenu)

//=============================================================================
/**
 * @class QuickMatchDrawListener
 * @brief クイックマッチ画面リスナー
 * @date  
 */
//=============================================================================
class QuickMatchDrawListener
{
public:
  //--------------------------------------------------------------------------
  /**
   * @func    OnQuickMatchDrawExit
   * @brief   Bボタンで終了した時に呼ばれる
   */
  //--------------------------------------------------------------------------
  virtual void OnQuickMatchDrawExit( void ) = 0;

  //--------------------------------------------------------------------------
  /**
   * @func    OnQuickMatchDrawNext
   * @brief   マッチングが成功して、クイックマッチアプリに遷移するときに呼ばれる
   */
  //--------------------------------------------------------------------------
  virtual void OnQuickMatchDrawNext( void ) = 0;
};

GFL_NAMESPACE_END(FieldMenu)
GFL_NAMESPACE_END(App)

#endif  // QUICKMATCH_DRAW_LISTENER_H_INCLUDED
