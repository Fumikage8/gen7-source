#if !defined(LOWERVIEW_SELECTIONSCROLLLISTBASE_H_INCLUDED)
#define LOWERVIEW_SELECTIONSCROLLLISTBASE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LowerView_SelectionScrollListBase.h
 * @date   2015/07/31 Fri. 18:22:10
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================

// =============================================================================
/**/
//==============================================================================
#include  <AppLib/include/Tool/app_tool_PaneListView.h>
#include  "./LowerView_SelectionBase.h"


/*
  forward declare
*/


namespace NetAppLib {
namespace UI  {
  class NetAppMenuCursor;
}
}


namespace NetApp  {
namespace Gift {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015.06.25 Thu.
  */
//==============================================================================
class LowerView_SelectionScrollListBase
  : public  LowerView_SelectionBase
  , public  app::tool::PaneList::CallBack
  , public  app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( LowerView_SelectionScrollListBase );
public:
  /**
  */

  //------------------------------------------------------------------
  /**
    *  @func     LowerView_SelectionScrollListBase
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  LowerView_SelectionScrollListBase(GiftWork* pWork);
  //------------------------------------------------------------------
  /**
    *  @func     ~LowerView_SelectionScrollListBase
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~LowerView_SelectionScrollListBase();


  void  SetupList(const u32 itemNum );
  void  CleanupList(void);



  /*
    UIView
  */
  //--------------------------------------------------------------------------------------------
  /**
  * @brief  更新処理
  *         子ウインドウについても自動的にコールする(子→親の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Update(void);

  //--------------------------------------------------------------------------------------------
  /**
  * @brief  描画処理
  *         子ウインドウについても自動的にコールする(親→子の順）
  */
  //--------------------------------------------------------------------------------------------
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
  /*
    end UIView
  */

  /*
    UIInputListener
  */
  //--------------------------------------------------------------------------------------------
  /**
  * @brief ペインイベントの検知
  */
  //--------------------------------------------------------------------------------------------
  virtual app::ui::UIInputListener::ListenerResult  OnLayoutPaneEvent( const u32 buttonID );
//  virtual app::ui::UIInputListener::ListenerResult  OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );
  /*
    end UIInputListener
  */

  virtual void OnSetInputEnabled( bool isEnable );


protected:
  virtual void  OnExit(void)  {}                                                /**<  派生側への終了通知  */
  
  /*  panelist  */
  virtual void  OnUpdateItem(gfl2::lyt::LytTextBox*, const u32 listIndex) {}    /**<  list項目更新委譲    */
  virtual void  OnCursorMove(const u32 listIndex)                         {}    /**<  カーソル移動通知    */
  virtual void  OnSelectedItem(const u32 selectedIndex)                   {}    /**<  list項目選択通知    */

  static const u32 PaneDataNum  = 9;


  app::tool::PaneListView*          m_pPaneListView;
  app::tool::PaneList::PANE_DATA    m_PaneData[PaneDataNum];
  NetAppLib::UI::NetAppMenuCursor*  m_pScrollListCursor;        /**<  NetAppCursorViewを継承しているが、ボタンの上に乗せる使い方はしないので自前で持つ  */

  gfl2::lyt::LytPane*               m_pScrollBarPane;


private:
  void          PaneListResultHandler(const app::tool::PaneList::Result result);
/*  PaneList  */
  virtual void  CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
  virtual void  CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )       {}
  virtual void  CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void  CallBack_ItemChange( void * work, u32 pos1, u32 pos2 ) {}
/*  PaneListView  */
  virtual void  PaneListItemSelectListener( u32 pos );
  virtual void  PaneListItemCatchListener( u32 pos );




  gfl2::lyt::LytBounding* GetBoundingPaneFromItemParts(gfl2::lyt::LytParts* pParts);
};





} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LOWERVIEW_SELECTIONSCROLLLISTBASE_H_INCLUDED)  */
