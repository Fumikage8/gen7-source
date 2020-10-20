// ============================================================================
/*
 * @file JoinFestaProfileLowerView.h
 * @brief プロフィールアプリの下画面を表示するクラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPROFILELOWERVIEW_H_INCLUDE )
#define JOINFESTAPROFILELOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaProfile/source/System/JoinFestaProfileWork.h"
#include "NetApp/JoinFestaProfile/source/View/JoinFestaProfileLowerViewListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaProfileLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaProfileLowerView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAPROFILE,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_ID_INFO_BUTTON,
    BUTTON_MAX
  };

  typedef struct
  {
    gfl2::lyt::LytTextBox* pTextBox00;
    gfl2::lyt::LytTextBox* pTextBox01;
  } MyPaneData;

public:

  static const u32 LIST_ITEM_PANE_MAX                   = 5;  //!< リスト項目ペイン数
  
public:

  JoinFestaProfileLowerView( NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork );
  virtual~JoinFestaProfileLowerView();

  void SetListener( NetApp::JoinFestaProfile::View::JoinFestaProfileLowerViewListener* pJoinFestaProfileLowerViewListener ) { m_pJoinFestaProfileLowerViewListener = pJoinFestaProfileLowerViewListener; }
  void RemoveListener() { m_pJoinFestaProfileLowerViewListener = NULL; }

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

  void UpdateLikePokemonTextPane();
  void UpdateHonorTextPane();
  void UpdateFestaNameTextPane();
  void UpdateOKNGTextPane();

  void GetInfoButtonPos( gfl2::math::Vector3 * vec );

private:

  void setupTextMessage();
  void setupButton();

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );
  void backupPaneListCursorInfo();

  NetApp::JoinFestaProfile::System::EditMenuInfo* getEditMenuInfo( u32 index );

private:

  NetApp::JoinFestaProfile::System::JoinFestaProfileWork*                      m_pJoinFestaProfileWork;
  NetApp::JoinFestaProfile::View::JoinFestaProfileLowerViewListener*           m_pJoinFestaProfileLowerViewListener;
  app::tool::PaneListView*                                                     m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                              m_pPaneListData;
  MyPaneData*                                                                  m_pMyPaneListData;
  NetAppLib::UI::NetAppMenuCursor                                              m_MenuCursor;
  gfl2::str::StrBuf                                                            m_NullString;
  gfl2::lyt::LytTextBox*                                                       m_pLikePokemonTextPane;
  gfl2::lyt::LytTextBox*                                                       m_pHonorTextPane;
  gfl2::lyt::LytTextBox*                                                       m_pFestaNameTextPane;
  u32                                                                          m_SelectListIndex;
  bool                                                                         m_bPaneListInputEnable;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAPROFILELOWERVIEW_H_INCLUDE
