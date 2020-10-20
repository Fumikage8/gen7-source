// ============================================================================
/*
 * @file JoinFestaSurveyLowerView.h
 * @brief アンケート下画面
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASURVERLOWERVIEW_H_INCLUDE )
#define JOINFESTASURVERLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyListManager.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaSurveyLowerView :
  public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSurveyLowerView );

private:

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_ID_ARROW_LEFT,
    BUTTON_ID_ARROW_RIGHT,
    BUTTON_ID_ARROW_UP,
    BUTTON_ID_ARROW_DOWN,
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

  JoinFestaSurveyLowerView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID,
    NetAppLib::System::ResourceID menuCursorResourceID );
  virtual~JoinFestaSurveyLowerView();

  void SetListener( NetAppLib::JoinFestaUI::JoinFestaSurveyLowerViewListener* pJoinFestaSurveyLowerViewListener ) { m_pJoinFestaSurveyLowerViewListener = pJoinFestaSurveyLowerViewListener; }
  void RemoveListener() { m_pJoinFestaSurveyLowerViewListener = NULL; }

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

  void SetList( NetAppLib::JoinFestaUI::JoinFestaSurveyDataList* pList, bool bIsVip );
  void UpdateList();

protected:

  void setupButton();

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, u32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );

  NetAppLib::JoinFestaUI::JoinFestaSurveyData* getJoinFestaSurveyData( u32 index );

protected:

  NetAppLib::JoinFestaUI::JoinFestaSurveyLowerViewListener*                    m_pJoinFestaSurveyLowerViewListener;
  app::tool::PaneListView*                                                     m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                              m_pPaneListData;
  MyPaneData*                                                                  m_pMyPaneListData;
  NetAppLib::JoinFestaUI::JoinFestaSurveyDataList*                             m_pActiveList;
  bool                                                                         m_bIsExScroll;
  bool                                                                         m_bKeyInput;
  bool                                                                         m_bPaneListInputEnable;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTASURVERLOWERVIEW_H_INCLUDE
