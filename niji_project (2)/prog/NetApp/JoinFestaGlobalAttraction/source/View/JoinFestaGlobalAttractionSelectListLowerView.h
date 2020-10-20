// ============================================================================
/*
 * @file JoinFestaGlobalAttractionSelectListLowerView.h
 * @brief グローバルアトラクションアプリの下画面を表示するクラスです。
 * @date 2016.01.13
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAGLOBALATTRACTIONSELECTLISTLOWERVIEW_H_INCLUDE )
#define JOINFESTAGLOBALATTRACTIONSELECTLISTLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaGlobalAttraction/source/System/JoinFestaGlobalAttractionWork.h"
#include "NetApp/JoinFestaGlobalAttraction/source/View/JoinFestaGlobalAttractionSelectListLowerViewListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaGlobalAttraction)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaGlobalAttractionSelectListLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaGlobalAttractionSelectListLowerView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTAGLOBALATTRACTION,
  };

  enum LayoutWorkID
  {
    LAYOUT_WORK_ID_LOWER,
  };

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_MAX
  };

  typedef struct
  {
    u32       pos;
    f32       scroll;
    u32       listMax;
  } PaneListCursorInfo;

public:

  static const u32 LIST_ITEM_PANE_MAX                   = 9;  //!< リスト項目ペイン数

public:

  JoinFestaGlobalAttractionSelectListLowerView( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork* pJoinFestaGlobalAttractionWork );
  virtual~JoinFestaGlobalAttractionSelectListLowerView();

  void SetListener( NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerViewListener* pJoinFestaGlobalAttractionSelectListLowerViewListener ) { m_pJoinFestaGlobalAttractionSelectListLowerViewListener = pJoinFestaGlobalAttractionSelectListLowerViewListener; }
  void RemoveListener() { m_pJoinFestaGlobalAttractionSelectListLowerViewListener = NULL; }

  virtual void OnSetInputEnabled( bool isEnable )
  {
    setInputEnablePaneList( isEnable );
  }

  // UIView
  virtual void Update();
  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsDrawing(void) const;
  virtual void OnAddChild();
  virtual void OnRemoveChild();

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

  void SetList( NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* pDownloadGlobalAttractionInfoList, u32 downloadGlobalAttractionInfoListSize );


private:

  void setupButton();

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo* getGlobalAttractionInfo( u32 pos );

private:

  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork*                             m_pJoinFestaGlobalAttractionWork;
  PaneListCursorInfo                                                                                    m_PaneListCursorInfo;
  NetApp::JoinFestaGlobalAttraction::View::JoinFestaGlobalAttractionSelectListLowerViewListener*        m_pJoinFestaGlobalAttractionSelectListLowerViewListener;
  app::tool::PaneListView*                                                                              m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                                                       m_pPaneListData;
  NetAppLib::UI::NetAppMenuCursor                                                                       m_MenuCursor;
  NetApp::JoinFestaGlobalAttraction::System::JoinFestaGlobalAttractionWork::GlobalAttractionInfo*       m_pDownloadGlobalAttractionInfoList;
  u32                                                                                                   m_DownloadGlobalAttractionInfoListSize;
  bool                                                                                                  m_bPaneListInputEnable;

};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaGlobalAttraction)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTAGLOBALATTRACTIONSELECTLISTLOWERVIEW_H_INCLUDE
