#if !defined( __FINDER_STUDIO_VIEW_LIST_H_INCLUDED__ )
#define __FINDER_STUDIO_VIEW_LIST_H_INCLUDED__
#pragma once
//======================================================================
/**
 * @file    FinderStudioViewList.h
 * @date    2016/11/18 11:59:31
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <Applib/include/UI/UIView.h>


//----------------------------------------------------------------------
// @brief   前方宣言
//----------------------------------------------------------------------
GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Tool )
class MessageMenuView;
GFL_NAMESPACE_END( Tool )
GFL_NAMESPACE_END( App )
// ---[ end ]---


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

class ViewBase;
class MessageMenuView;

class ViewList
{
  GFL_FORBID_COPY_AND_ASSIGN( ViewList );

public:
  //-------------------------------------------------------
  // @brief   コンストラクタ・デストラクタ
  //-------------------------------------------------------
  ViewList( gfl2::heap::HeapBase* heap, u32 viewMax );
  ~ViewList( void );

public:
  //-------------------------------------------------------
  // @brief   追加・削除
  //-------------------------------------------------------
  void AddView( u32 id, ViewBase* view );
  void RemoveView( void );

  //-------------------------------------------------------
  // @brief   設定・取得
  //-------------------------------------------------------
  ViewBase* GetView( u32 id );

  void             SetMessageMenuView( MessageMenuView* view ) { m_pMessageMenuView = view; }
  MessageMenuView* GetMessageMenuView( void ) const { return m_pMessageMenuView; }

private:
  ViewBase**  m_pList;

  MessageMenuView*   m_pMessageMenuView;

  u32         m_ViewMax;
};

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )

#endif // __FINDER_STUDIO_VIEW_LIST_H_INCLUDED__
