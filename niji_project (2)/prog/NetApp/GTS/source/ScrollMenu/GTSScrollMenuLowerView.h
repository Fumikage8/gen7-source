// ============================================================================
/*
 * @file GTSScrollMenuLowerView.h
 * @brief スクロールするメニューを表示するクラスです。
 * @date 2015.03.31
 */
// ============================================================================
#if !defined( GTSSCROLLMENULOWERVIEW_H_INCLUDE )
#define GTSSCROLLMENULOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include <str/include/gfl2_MsgData.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSWork.h"
#include "NetApp/GTS/source/ScrollMenu/GTSScrollMenuLowerViewListener.h"
#include "PokeTool/include/PokeToolPersonalSort.h"
#include "AppLib/include/tool/app_tool_PokeIcon.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)
class NetAppMenuCursor;
GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(ScrollMenu)


class GTSScrollMenuLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( GTSScrollMenuLowerView );

public:

  static const u32 LIST_ITEM_PANE_MAX                   = 12;  //!< リスト項目ペイン数

private:

  static const u32 MONS_NAME_MESSAGE_ID_TABLE_MAX       = MONSNO_MAX;
  static const u32 LIST_POS_MAX                         = 8;

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

  enum MenuType
  {
    MENU_TYPE_INITIAL_FIRST,
    MENU_TYPE_INITIAL_SECOND,
    MENU_TYPE_POKEMON,
    MENU_TYPE_GENDER,
    MENU_TYPE_LEVEL_BAND,
    MENU_TYPE_MAX
  };

  typedef struct
  {
    u32       pos;
    f32       scroll;
    u32       listMax;
  } PaneListCursorInfo;

  typedef struct
  {
    u32   initialMessageID;
    bool  isPassive;
  } PassiveInfo;

  typedef struct
  {
    gfl2::lyt::LytPane*       pBallIcon;
    gfl2::lyt::LytPicture*    pPokeIcon;
  } MyPaneInfo;

public:

  GTSScrollMenuLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork );
  virtual~GTSScrollMenuLowerView();

  void SetListener( NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener* pGTSScrollMenuLowerViewListener ) { m_pScrollMenuDrawListener = pGTSScrollMenuLowerViewListener; }
  void RemoveListener() { m_pScrollMenuDrawListener = NULL; }

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

  void ChangePokemonSelectMenu(){ changeMenu( MENU_TYPE_INITIAL_FIRST, 0 ); }
  void ChangeGenderSelectMenu(){ changeMenu( MENU_TYPE_GENDER, 0 ); }
  void ChangeLevelBandSelectMenu(){ changeMenu( MENU_TYPE_LEVEL_BAND, 0 ); }

private:

  void createPassiveInfoList();
  void addPassiveInfo( PokeTool::PersonalSort& sort, const u32* pMessageIDTable, u32 tableSize );
  bool checkPassiveInfo( const u32* pMessageIDTable, u32 tableSize );
  bool checkPassiveInfo( u32 initialMessageID );

  void setupButton();
  void setActive( u32 paneIndex );
  void setPassive( u32 paneIndex );
  bool isPassive( u32 initialMessageID );

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );

  void changeMenu( MenuType eMenuType, u32 param );
  const u32* getInitialSecondMessageIDTable( u32& tableSize, u32 initial );
  const u32* getMonsNameMessageIDTable( u32& tableSize, u32 initial );

private:

  NetApp::GTS::ApplicationSystem::GTSWork*                   m_pGTSWork;
  NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener*   m_pScrollMenuDrawListener;
  app::tool::PaneListView*                                   m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                            m_pPaneListData;
  MyPaneInfo*                                                m_pMyPaneListData;
  NetAppLib::UI::NetAppMenuCursor*                           m_pMenuCursor;
  MenuType                                                   m_eMenuType;
  u32                                                        m_SelectInitial;
  u32                                                        m_SelectInitialFirst;
  u32                                                        m_PaneListMenuMax;
  const u32*                                                 m_pMessageIDTable;
  u32                                                        m_MonsNameMessageIDTable[ MONS_NAME_MESSAGE_ID_TABLE_MAX ];
  PaneListCursorInfo                                         m_aPaneListCursorInfos[ MENU_TYPE_MAX ];
  gfl2::util::List<PassiveInfo>                              m_PassiveInfoList;
  app::tool::PokeIconToolEx*                                 m_pPokeIconToolEx;
  pml::pokepara::PokemonParam                                m_PokemonParam;
  bool                                                       m_bPaneListInputEnable;

};


GFL_NAMESPACE_END(ScrollMenu)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)

#endif // GTSSCROLLMENULOWERVIEW_H_INCLUDE
