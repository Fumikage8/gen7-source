// ============================================================================
/*
 * @file GTSUploadMessageSelectLowerView.h
 * @brief ポケモンを預ける際のメッセージを選択する画面を表示するクラスです。
 * @date 2015.05.11
 */
// ============================================================================
#if !defined( GTSUPLOADMESSAGESELECTLOWERVIEW_H_INCLUDE )
#define GTSUPLOADMESSAGESELECTLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadMessageSelectLowerViewListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppMenuCursor;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(UploadPokemon)


class GTSUploadMessageSelectLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSUploadMessageSelectLowerView );
public:
  GTSUploadMessageSelectLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSUploadMessageSelectLowerView();

  void SetListener( NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerViewListener* pGTSUploadMessageSelectLowerViewListener ) { m_pUploadMessageSelectDrawListener = pGTSUploadMessageSelectLowerViewListener; }
  void RemoveListener() { m_pUploadMessageSelectDrawListener = NULL; }

  virtual void OnSetInputEnabled( bool isEnable )
  {
    setInputEnablePaneList( isEnable );
  }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsDrawing(void) const;
  virtual void OnAddChild( void );
  virtual void OnRemoveChild( void );

  // UIInputListener
  virtual ::app::ui::UIInputListener::ListenerResult OnLayoutPaneEvent( const u32 painId );
  virtual ::app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

  // PaneList::CallBack
  virtual void CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos );
  virtual void CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos );
  virtual void CallBack_ItemChange( void * work, u32 pos1, u32 pos2 );

  // PaneListView::Listener
  virtual void PaneListItemSelectListener( u32 pos );

private:

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
    BUTTON_ID_BTN_BACK,   //『戻る』ボタン
    BUTTON_MAX,
  };

  static const u32 LIST_ITEM_PANE_MAX                   = 8;  //!< リスト項目ペイン数


  void setupButton();

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, u32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                                m_pGTSWork;
  NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerViewListener*    m_pUploadMessageSelectDrawListener;
  app::tool::PaneListView*                                                m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                         m_pPaneListData;
  NetAppLib::UI::NetAppMenuCursor*                                        m_pMenuCursor;
  bool                                                                    m_bPaneListInputEnable;

};


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSUPLOADMESSAGESELECTLOWERVIEW_H_INCLUDE
