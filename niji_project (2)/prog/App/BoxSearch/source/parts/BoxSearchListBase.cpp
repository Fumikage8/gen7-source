//======================================================================
/**
 * @file    BoxSearchListBase.cpp
 * @date    2016/02/01 17:33:05
 * @author  fukushima_yuya
 * @brief   ボックス検索：リストのベースクラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// niji
#include "BoxSearchListBase.h"

#include <App/BoxSearch/source/BoxSearchDrawerBase.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(BoxSearch)

//------------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 */
//------------------------------------------------------------------------------
ListBase::ListBase( DrawerBase* base, MenuCursor* cursor )
  : m_pBase(base)
  , m_pCursor(cursor)
  , m_pPaneListView(NULL)
  , m_pPaneData(NULL)
{
  ;
}


//------------------------------------------------------------------------------
/**
 * @brief   リストの破棄
 */
//------------------------------------------------------------------------------
void ListBase::DeleteList( void )
{
  if( m_pPaneListView )
  {
    if( m_pPaneListView->GetPaneList() )
    {
      // スクロールバーの破棄
      m_pPaneListView->DeleteScrollBar();

      // ペインリストの破棄
      m_pPaneListView->DeletePaneList();
    }

    // 入力を無効に
    SetInputEnable( false );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   スクロール値の計算
 *
 * @param   list_setup    リストのセットアップパラメータ
 *
 * @return  スクロール値
 */
//------------------------------------------------------------------------------
u32 ListBase::GetScrollValue( app::tool::PaneList::SETUP_PARAM& list_setup )
{
  u32 value = 0;

  if( list_setup.valMax > list_setup.listPosMax )
  {
    value = list_setup.valMax - list_setup.listPosMax;
  }

  return value;
}

//------------------------------------------------------------------------------
/**
 * @brief   スクロールバーのセットアップパラメータの取得(デフォルト)
 *
 * @return  app::util::ScrollBar::SETUP_PARAM
 */
//------------------------------------------------------------------------------
app::util::ScrollBar::SETUP_PARAM& ListBase::GetDefaultScrollBarParam( void )
{
  gfl2::heap::HeapBase* heap = m_pBase->GetDeviceHeap();

  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup = {
    heap,
    app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
    0,
    m_pBase->GetLytWk(),
    m_pBase->GetG2D()->GetLayoutResourceID(0),
    0, 0, 0, 0,
    NULL, NULL, NULL
  };

  m_ScrollBarSetup = scroll_bar_setup;

  return m_ScrollBarSetup;
}

//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの入力有効フラグの設定
 *
 * @param   enable    入力有効フラグ
 */
//------------------------------------------------------------------------------
void ListBase::SetInputEnable( bool enable )
{
  if( m_pPaneListView )
  {
    m_pPaneListView->SetInputEnabled( enable );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   ペインリストの更新リザルトの取得
 *
 * @return  更新リザルト
 */
//------------------------------------------------------------------------------
app::tool::PaneList::Result ListBase::GetPaneListUpdateResult( void ) const
{
  if( m_pPaneListView )
  {
    if( m_pPaneListView->GetPaneList() )
    {
      return m_pPaneListView->GetUpdateResult();
    }
  }

  return app::tool::PaneList::RET_NONE;
}

GFL_NAMESPACE_END(BoxSearch)
GFL_NAMESPACE_END(App)
