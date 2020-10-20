// ============================================================================
/*
 * @file JoinFestaSelectWordLowerView.h
 * @brief 挨拶選択アプリの下画面を表示するクラスです。
 * @date 2015.12.24
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASELECTWORDLOWERVIEW_H_INCLUDE )
#define JOINFESTASELECTWORDLOWERVIEW_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "NetStatic/NetAppLib/include/System/NetApplicationViewBase.h"
#include "NetApp/JoinFestaSelectWord/source/System/JoinFestaSelectWordWork.h"
#include "NetApp/JoinFestaSelectWord/source/View/JoinFestaSelectWordLowerViewListener.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "NetStatic/NetAppLib/include/UI/NetAppMenuCursor.h"
#include <niji_conv_header/message/msg_jf_phrase.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaSelectWord)
GFL_NAMESPACE_BEGIN(View)


class JoinFestaSelectWordLowerView :
  public NetAppLib::System::NetApplicationViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSelectWordLowerView );

private:

  enum LayoutResourceID
  {
    LAYOUT_RESOURCE_ID_JOINFESTASELECTWORD,
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

  enum ListType
  {
    LIST_TYPE_CATEGORY,
    LIST_TYPE_MESSAGE,
    LIST_TYPE_MAX
  };

  typedef struct
  {
    u32       pos;
    f32       scroll;
    u32       listMax;
  } PaneListCursorInfo;

  static const u32 MESSAGE_TABLE_MAX = msg_jf_phrase_max / 2;

public:

  static const u32 LIST_ITEM_PANE_MAX                   = 9;  //!< リスト項目ペイン数

public:

  JoinFestaSelectWordLowerView( NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork* pJoinFestaSelectWordWork );
  virtual~JoinFestaSelectWordLowerView();

  void SetListener( NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener* pJoinFestaSelectWordLowerViewListener ) { m_pJoinFestaSelectWordLowerViewListener = pJoinFestaSelectWordLowerViewListener; }
  void RemoveListener() { m_pJoinFestaSelectWordLowerViewListener = NULL; }

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

  void SetCategoryTable( NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* pMessageTable, u32 messageTableSize );
  void SetMessageTable( NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* pMessageTable, u32 messageTableSize );

  void GetInfoButtonPos( gfl2::math::Vector3 * vec );

private:

  void setList( NetAppLib::JoinFesta::JoinFestaSelectWordTable::Message* pMessageTable, u32 messageTableSize, u32 se );

  void setupButton();

  // ---------------------------------------------------------------
  // PaneList関連
  void createPaneList();
  void deletePaneList();
  void setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll );
  void setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam );
  void setInputEnablePaneList( bool bInputEnable );
  void backupPaneListCursorInfo();

private:

  NetApp::JoinFestaSelectWord::System::JoinFestaSelectWordWork*                         m_pJoinFestaSelectWordWork;
  PaneListCursorInfo                                                                    m_aPaneListCursorInfos[ LIST_TYPE_MAX ];
  ListType                                                                              m_eListType;
  NetApp::JoinFestaSelectWord::View::JoinFestaSelectWordLowerViewListener*              m_pJoinFestaSelectWordLowerViewListener;
  app::tool::PaneListView*                                                              m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                                       m_pPaneListData;
  NetAppLib::UI::NetAppMenuCursor                                                       m_MenuCursor;
  u32                                                                                   m_aMessageTable[ MESSAGE_TABLE_MAX ];
  u32                                                                                   m_MessageTableSize;
  bool                                                                                  m_bPaneListInputEnable;


};


GFL_NAMESPACE_END(View)
GFL_NAMESPACE_END(JoinFestaSelectWord)
GFL_NAMESPACE_END(NetApp)

#endif // JOINFESTASELECTWORDLOWERVIEW_H_INCLUDE
