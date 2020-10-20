// ============================================================================
/*
 * @file JoinFestaRankingLowerView.h
 * @brief アンケート下画面
 * @date 2015.10.21
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARANKINGLOWERVIEW_H_INCLUDE )
#define JOINFESTARANKINGLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingListManager.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaRankingLowerView :
  public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaRankingLowerView );

private:

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_ID_ARROW_LEFT,
    BUTTON_ID_ARROW_RIGHT,
    BUTTON_MAX
  };

  typedef struct
  {
    gfl2::lyt::LytPane*       pRankIcon1;
    gfl2::lyt::LytPane*       pRankIcon2;
    gfl2::lyt::LytPane*       pRankIcon3;
    gfl2::lyt::LytTextBox*    pTextRank;
    gfl2::lyt::LytTextBox*    pTextName;
    gfl2::lyt::LytTextBox*    pTextScore;
  } MyPaneData;

public:

  static const u32 LIST_ITEM_PANE_MAX                   = 9;  //!< リスト項目ペイン数

public:

  JoinFestaRankingLowerView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID,
    NetAppLib::System::ResourceID menuCursorResourceID );
  virtual~JoinFestaRankingLowerView();

  void SetListener( NetAppLib::JoinFestaUI::JoinFestaRankingLowerViewListener* pJoinFestaRankingLowerViewListener ) { m_pJoinFestaRankingLowerViewListener = pJoinFestaRankingLowerViewListener; }
  void RemoveListener() { m_pJoinFestaRankingLowerViewListener = NULL; }

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

  void SetList( NetAppLib::JoinFestaUI::JoinFestaRankingDataList* pList );
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

  NetAppLib::JoinFestaUI::JoinFestaRankingData* getJoinFestaRankingData( u32 index );

  void setText( s32 rank, s32 score, gfl2::str::STRCODE* pName,
    gfl2::lyt::LytTextBox* pRankText, gfl2::lyt::LytTextBox* pScoreText, gfl2::lyt::LytTextBox* pNameText );

protected:

  NetAppLib::JoinFestaUI::JoinFestaRankingLowerViewListener*                   m_pJoinFestaRankingLowerViewListener;
  app::tool::PaneListView*                                                     m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                              m_pPaneListData;
  MyPaneData*                                                                  m_pMyPaneListData;
  NetAppLib::JoinFestaUI::JoinFestaRankingDataList*                            m_pActiveList;
  bool                                                                         m_bPaneListInputEnable;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARANKINGLOWERVIEW_H_INCLUDE
