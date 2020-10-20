// ============================================================================
/*
 * @file JoinFestaPlayerListSelectLowerViewBase.h
 * @brief プレイヤーリストアプリの下画面を表示するクラスです。
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAPLAYERLISTSELECTLOWERVIEWBASE_H_INCLUDE )
#define JOINFESTAPLAYERLISTSELECTLOWERVIEWBASE_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListLowerViewBase.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewListener.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"
#include "AppLib/include/Tool/app_tool_PaneListView.h"
#include "AppLib/include/Tool/app_tool_PokeIcon.h"

namespace pml { namespace pokepara {  class  CoreParam; } }


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaPlayerListSelectLowerViewBase :
  public NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBase,
  public app::ui::UIInputListener,
  public app::tool::PaneList::CallBack,
  public app::tool::PaneListView::Listener
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaPlayerListSelectLowerViewBase );

public:

  enum ListType
  {
    LIST_TYPE_VIP,
    LIST_TYPE_GUEST,
    LIST_TYPE_BLACK,
    LIST_TYPE_MAX
  };

protected:

  enum ButtonID
  {
    BUTTON_ID_BACK_BUTTON,
    BUTTON_ID_UPDATE_BUTTON,
    BUTTON_ID_DECIDE_BUTTON,
    BUTTON_ID_ARROW_LEFT,
    BUTTON_ID_ARROW_RIGHT,
    BUTTON_ID_INFO_BUTTON,
    /*  photo  */
    BUTTON_ID_PHOTOVIEW,
    BUTTON_ID_SLIDESHOW,
    BUTTON_MAX
  };

  struct PaneListCursorInfo
  {
    u32       pos;
    f32       scroll;
    u32       listMax;
    
    /**  ctor  */
    PaneListCursorInfo(void)
      : pos(0)
      , scroll(0.0f)
      , listMax(0)
    {}
  };

  struct MyPaneData
  {
    gfl2::lyt::LytTextBox* pTextBox00;
    gfl2::lyt::LytTextBox* pTextBox01;
    gfl2::lyt::LytPane*    pCheckBox;
    gfl2::lyt::LytPane*    pCheckMark;
    gfl2::lyt::LytPicture* pPokeIcon;
    gfl2::lyt::LytPicture* pPhotoIcon;
    gfl2::lyt::LytPicture* pPhotoNewIcon;
    
    /**  ctor  */
    MyPaneData(void)
      : pTextBox00(NULL)
      , pTextBox01(NULL)
      , pCheckBox(NULL)
      , pCheckMark(NULL)
      , pPokeIcon(NULL)
      , pPhotoIcon(NULL)
      , pPhotoNewIcon(NULL)
    {}
  };

public:

  static const u32 LIST_ITEM_PANE_MAX                   = 8;  //!< リスト項目ペイン数

public:

  JoinFestaPlayerListSelectLowerViewBase(
    NetAppLib::System::ApplicationWorkBase* pWorkBase,
    NetAppLib::System::ResourceID layoutResourceID,
    NetAppLib::System::ResourceID menuCursorResourceID );
  virtual~JoinFestaPlayerListSelectLowerViewBase();

  void SetListener( NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener* pJoinFestaPlayerListSelectLowerViewListener ) { m_pJoinFestaPlayerListSelectLowerViewListener = pJoinFestaPlayerListSelectLowerViewListener; }
  void RemoveListener() { m_pJoinFestaPlayerListSelectLowerViewListener = NULL; }

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

  void SetVisibleCheckBox( bool bVisible ){ m_bCheckBoxVisible = bVisible; }
  /** カーソル位置のCheckBoxの状態を更新する  */
  void RefreshCheckBox(void);
  /** カーソル位置のカメラアイコンの状態を更新する  */
  void RefreshPhotoIcon(void);
  void RefreshPhotoIcon(const u32 targetPaneListIndex);
  void RefreshPhotoIconAll(void);
 
 
  void ResetPaneListCursorInfo();
  void SetList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList* pList, u32 screenTitleMessageID, ListType eListType );
  void UpdateList();

  void SetPassive();

  void SetActiveDecideButton();
  void SetPassiveDecideButton();

  void GetInfoButtonPos( gfl2::math::Vector3 * vec );

  /*  pokeicon  */
  class IPokeparaProvider
  {
  public:
    virtual const pml::pokepara::CoreParam* OnRequestPokepara(JoinFestaPlayerListSelectLowerViewBase* pCaller, const ListType listType, const u32 dataIndex) =0;
    virtual ~IPokeparaProvider(){}
  };
  void SetupPokeIcon(IPokeparaProvider* pObject, app::util::Heap* pIconHeap = NULL);
  bool IsPokeIconLoading(void);
  bool CleanupPokeIcon(void);

  /*  photo  */
  void  SetPhotoEnable(const bool bIconEnable, const bool bViewButtonEnable)    {m_bPhotoIconEnable = bIconEnable; m_bPhotoButtonEnable = bViewButtonEnable;}
  void  SetVisibleSlideshowButtonIfAvailable( bool bVisible )                   {SetVisibleSlideshowButton(bVisible && m_bPhotoButtonEnable);}


  /*  BFC  */
  void  ShowBFCGrade(const bool bEnable)  {m_bShowBFCGrade = bEnable;}

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
  void updatePhotoIcon(const u32 paneListIndex, const JoinFestaPlayerData& rJFPlayerData);
  void updatePhotoButton(const NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJFPlayerData);
  void updateSlideshowButton(void);

  NetAppLib::JoinFestaUI::JoinFestaPlayerData* getJoinFestaPlayerData( u32 index );

protected:

  PaneListCursorInfo                                                               m_aPaneListCursorInfos[ LIST_TYPE_MAX ];
  ListType                                                                         m_eListType;
  NetAppLib::JoinFestaUI::JoinFestaPlayerListSelectLowerViewListener*              m_pJoinFestaPlayerListSelectLowerViewListener;
  app::tool::PaneListView*                                                         m_pPaneListView;
  app::tool::PaneList::PANE_DATA*                                                  m_pPaneListData;
  MyPaneData*                                                                      m_pMyPaneListData;
  gfl2::str::StrBuf                                                                m_NullString;
  NetAppLib::JoinFestaUI::JoinFestPlayerDataList*                                  m_pActiveList;
  bool                                                                             m_bCheckBoxVisible;
  bool                                                                             m_bPaneListInputEnable;
  bool                                                                             m_bIsResetPaneListCursorInfo;

  /*  pokeicon  */
  app::tool::PokeIconToolEx*  m_pPokeIconTool;
  IPokeparaProvider*          m_pPokeparaProvider;

  /*  photo  */
  bool    m_bPhotoIconEnable;
  bool    m_bPhotoButtonEnable;
  
  /*  BFC  */
  bool    m_bShowBFCGrade;  /**<  「フレンド」表示部分にBFCのグレードを表示する  */
};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAPLAYERLISTSELECTLOWERVIEW_H_INCLUDE
