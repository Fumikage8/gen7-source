// ============================================================================
/*
 * @file JoinFestaAttractionSelectLowerView.h
 * @brief アトラクション選択下画面
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONSELECTLOWERVIEW_H_INCLUDE )
#define JOINFESTAATTRACTIONSELECTLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaAttractionSelectLowerView :
  public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionSelectLowerView );

private:

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_ID_UPDATE_BUTTON,
    BUTTON_ID_ARROW_LEFT,
    BUTTON_ID_ARROW_RIGHT,
    BUTTON_MAX
  };

  enum ListType
  {
    LIST_TYPE_OPEN,
    LIST_TYPE_ENTRY,
    LIST_TYPE_MAX
  };

  typedef struct
  {
    u32       pos;
    f32       scroll;
    u32       listMax;
  } PaneListCursorInfo;

public:

  static const u32 LIST_ITEM_PANE_MAX                   = 8;  //!< リスト項目ペイン数


public:

  JoinFestaAttractionSelectLowerView(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID,
    NetAppLib::System::ResourceID menuCursorResourceID );
  virtual~JoinFestaAttractionSelectLowerView();

  void SetListener( NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener* pJoinFestaAttractionSelectLowerViewListener ) { m_pJoinFestaAttractionSelectLowerViewListener = pJoinFestaAttractionSelectLowerViewListener; }
  void RemoveListener() { m_pJoinFestaAttractionSelectLowerViewListener = NULL; }

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

  void ResetPaneListCursorInfo();
  void SetOpenList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList* pList );
  void SetEntryList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList* pList );
  void UpdateList();

  void SetPassive();

  void SetVisibleArrowIcon( bool bVisible );


protected:

  void setupButton();
  void setActive( u32 paneIndex );
  void setPassive( u32 paneIndex );

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );
  void backupPaneListCursorInfo();

  NetAppLib::JoinFestaUI::JoinFestaAttractionData* getJoinFestaAttractionData( u32 index );

protected:

  PaneListCursorInfo                                                               m_aPaneListCursorInfos[ LIST_TYPE_MAX ];
  ListType                                                                         m_eListType;
  NetAppLib::JoinFestaUI::JoinFestaAttractionSelectLowerViewListener*              m_pJoinFestaAttractionSelectLowerViewListener;
  app::tool::PaneListView*                                                         m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                                  m_pPaneListData;
  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList*                             m_pActiveList;
  bool                                                                             m_bPaneListInputEnable;
  bool                                                                             m_bIsResetPaneListCursorInfo;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAATTRACTIONSELECTLOWERVIEW_H_INCLUDE
