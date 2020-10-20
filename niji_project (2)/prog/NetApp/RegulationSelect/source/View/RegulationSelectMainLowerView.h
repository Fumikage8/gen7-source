// ============================================================================
/*
 * @file RegulationSelectMainLowerView.h
 * @brief レギュレーション選択の下画面を表示するクラスです。
 * @date 2015.06.12
 * @author endo_akira
 */
// ============================================================================
#if !defined( REGULATIONSELECTMAINLOWERVIEW_H_INCLUDE )
#define REGULATIONSELECTMAINLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "AppLib/include/Ui/UIInputListener.h"
#include "NetApp/RegulationSelect/source/ApplicationSystem/RegulationSelectWork.h"
#include "NetApp/RegulationSelect/source/View/RegulationSelectMainLowerViewListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(RegulationSelect)
GFL_NAMESPACE_BEGIN(Draw)


class RegulationSelectMainLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationSelectMainLowerView );

private:

  typedef struct
  {
    u32       pos;
    f32       scroll;
    u32       listMax;
  } PaneListCursorInfo;

public:

  enum ListType
  {
    LIST_TYPE_PRESET_DOWNLOADED,
    LIST_TYPE_DOWNLOAD,
    LIST_TYPE_DOWNLOADED,
    LIST_TYPE_MAX
  };

public:

  RegulationSelectMainLowerView( NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork* pRegulationSelectWork );
  virtual~RegulationSelectMainLowerView();

  void SetListener( NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener* pRegulationSelectMainLowerViewListener ) { m_pRegulationSelectMainLowerViewListener = pRegulationSelectMainLowerViewListener; }
  void RemoveListener() { m_pRegulationSelectMainLowerViewListener = NULL; }

  virtual void OnSetInputEnabled( bool isEnable )
  {
    setInputEnablePaneList( isEnable );
  }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsDrawing(void) const;

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

  void CheckBoxEnable();
  void CheckBoxDisable();
  void SetRegulationList( gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pRegulationList, bool bCheckMarkEnable, ListType eListType );

  void ResetPaneListCursorInfo( ListType eListType );

private:

  // UIView
  virtual void OnAddChild();
  virtual void OnAddedToParent();
  virtual void OnRemoveChild();
  virtual void OnRemovedFromParent();

  bool isDownloaded( NetAppLib::UI::RegulationDrawInfo* pRegulationDrawInfo, gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>* pDownloadedList );

  NetAppLib::UI::RegulationDrawInfo* getRegulationDrawInfo( u32 index );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_REGULATIONSELECT,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_MAX,
  };


  void setupButton();

  void checkBoxVisible( bool bVisible );

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );

private:

  typedef struct
  {
    gfl2::lyt::LytTextBox* pTextBox00;
    gfl2::lyt::LytTextBox* pTextBox01;
    gfl2::lyt::LytPane*    pCheckMark;
  } RegulationPaneData;

  NetApp::RegulationSelect::ApplicationSystem::RegulationSelectWork*                       m_pRegulationSelectWork;
  PaneListCursorInfo                                                                       m_aPaneListCursorInfos[ LIST_TYPE_MAX ];
  ListType                                                                                 m_eListType;
  NetApp::RegulationSelect::Draw::RegulationSelectMainLowerViewListener*                   m_pRegulationSelectMainLowerViewListener;
  app::tool::PaneListView*                                                                 m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                                          m_pPaneListData;
  RegulationPaneData*                                                                      m_pRegulationPaneListData;
  NetAppLib::UI::NetAppMenuCursor                                                          m_MenuCursor;
  gfl2::util::List<NetAppLib::UI::RegulationDrawInfo*>*                                    m_pRegulationList;
  bool                                                                                     m_bPaneListInputEnable;

};


GFL_NAMESPACE_END(Draw)
GFL_NAMESPACE_END(RegulationSelect)
GFL_NAMESPACE_END(NetApp)

#endif // REGULATIONSELECTMAINLOWERVIEW_H_INCLUDE
