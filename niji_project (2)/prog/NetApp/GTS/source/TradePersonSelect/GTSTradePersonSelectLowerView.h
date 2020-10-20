// ============================================================================
/*
 * @file GTSTradePersonSelectLowerView.h
 * @brief 交換相手を選択する画面を表示するクラスです。
 * @date 2015.04.15
 */
// ============================================================================
#if !defined( GTSTRADEPERSONSELECTLOWERVIEW_H_INCLUDE )
#define GTSTRADEPERSONSELECTLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_MsgData.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "AppLib/include/Tool/AppToolNewPaneList.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/TradePersonSelect/GTSTradePersonSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePersonSelect)


class GTSTradePersonSelectLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public App::Tool::NewPaneList::ListListener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSTradePersonSelectLowerView );
public:
  GTSTradePersonSelectLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSTradePersonSelectLowerView();

  void SetListener( NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener* pGTSTradePersonSelectLowerViewListener ) { m_pTradePersonSelectDrawListener = pGTSTradePersonSelectLowerViewListener; }
  void RemoveListener() { m_pTradePersonSelectDrawListener = NULL; }

  virtual void OnSetInputEnabled( bool isEnable )
  {
    if( !m_bPaneListSetInputInvalidMode )
    {
      setInputEnablePaneList( isEnable );
    }
  }

  void SetPaneListSetInputInvalidMode( bool flag )
  {
    m_bPaneListSetInputInvalidMode = flag;
  }

  u32 InputCustom();

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsDrawing(void) const;
  virtual void OnAddChild( void );
  virtual void OnRemoveChild( void );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  void ResetView( bool bBegin );

  bool IsPaneListAction();

  void ReUpdate();
  void SetCursorVisible( bool bVisible );

private:

  void createPaneList( NetApp::GTS::ApplicationSystem::GtsSearchResultList* pSearchResultList, bool bBegin );
  void deletePaneList();
  void setInputEnablePaneList( bool bInputEnable );

  NetApp::GTS::GtsSearchResultInfo* getSearchResultInfo( u32 index );

  // NewPaneList::ListListener
  virtual void PaneListListener_Write( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void PaneListListener_MoveCursor( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void PaneListListener_Decide( u32 pos );
  virtual void PaneListListener_StartItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void PaneListListener_EndItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos1, u32 pos2 );

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_GTS,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BTN_BACK,
    BUTTON_ID_BTN_SEARCH,
    BUTTON_ID_CURSOR_L,
    BUTTON_ID_CURSOR_R,
    BUTTON_MAX,
  };

  static const u32 PANE_MAX = 5;

  void setupTextMessage();
  void setupButton();
  void setPokemonInfo( u32 index );
  void setCursorVisible( bool bLeft, bool bRight );

  void setVisibleRefineSearchButton( bool bVisible );

  // ---------------------------------------------------------------
  // MessageData関連
  void createMessageData();
  void deleteMessageData();

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                 m_pGTSWork;
  NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener*   m_pTradePersonSelectDrawListener;
  App::Tool::NewPaneList*                                                  m_pNewPaneList;
  App::Tool::NewPaneList::PANE_DATA                                        m_listPaneData[PANE_MAX];
  NetApp::GTS::ApplicationSystem::GtsSearchResultList*                     m_pSearchResultList;
  u32                                                                      m_SelectIndex;
  bool                                                                     m_bRefineSearch;
  gfl2::str::StrBuf                                                        m_NullString;
  bool                                                                     m_bPaneListInputEnable;
  bool                                                                     m_bPaneListSetInputInvalidMode;
  NetAppLib::UI::NetAppMenuCursor                                          m_MenuCursor;

};


GFL_NAMESPACE_END(TradePersonSelect)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSTRADEPERSONSELECTLOWERVIEW_H_INCLUDE
