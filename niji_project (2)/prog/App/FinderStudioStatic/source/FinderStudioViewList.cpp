//======================================================================
/**
 * @file    FinderStudioViewList.cpp
 * @date    2016/11/18 12:01:42
 * @author  fukushima_yuya
 * @brief   ファインダースタジオ：Viewリスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <App/FinderStudioStatic/include/FinderStudioViewList.h>
#include <App/FinderStudioStatic/include/FinderStudioViewBase.h>
#include <App/FinderStudioStatic/include/FinderStudioMessageMenuView.h>

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>


GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( FinderStudio )
GFL_NAMESPACE_BEGIN( Static )

//----------------------------------------------------------------------
// @brief   コンストラクタ・デストラクタ
//----------------------------------------------------------------------
ViewList::ViewList( gfl2::heap::HeapBase* heap, u32 viewMax )
  : m_pMessageMenuView( NULL )
  , m_ViewMax( viewMax )
{
  // View格納配列を生成
  m_pList = GFL_NEW_ARRAY( heap ) ViewBase*[viewMax];

  for( u32 i = 0; i < viewMax; ++i )
  {
    m_pList[i] = NULL;
  }
}

ViewList::~ViewList( void )
{
  RemoveView();

  m_pMessageMenuView = NULL;

  GFL_SAFE_DELETE_ARRAY( m_pList );
}


//----------------------------------------------------------------------
// @brief   追加・削除
//----------------------------------------------------------------------
void ViewList::AddView( u32 id, ViewBase* view )
{
  GFL_ASSERT( id < m_ViewMax );
  GFL_ASSERT( m_pList[id] == NULL );

  m_pList[id] = view;

  GFL_PRINT( "Add View ID[%d] Success!!\n", id );
}

void ViewList::RemoveView( void )
{
  for( u32 i = 0; i < m_ViewMax; ++i )
  {
    m_pList[i] = NULL;
  }
}



//----------------------------------------------------------------------
// @brief   取得
//----------------------------------------------------------------------
ViewBase* ViewList::GetView( u32 id )
{
  GFL_ASSERT( id < m_ViewMax );

  return m_pList[id];
}

GFL_NAMESPACE_END( Static )
GFL_NAMESPACE_END( FinderStudio )
GFL_NAMESPACE_END( App )
