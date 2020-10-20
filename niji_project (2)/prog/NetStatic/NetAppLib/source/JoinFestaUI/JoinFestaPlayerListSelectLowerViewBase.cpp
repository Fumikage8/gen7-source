// ============================================================================
/*
 * @file JoinFestaPlayerListSelectLowerViewBase.cpp
 * @brief プレイヤーリストアプリの下画面を表示するクラスです。
 * @date 2015.10.05
 * @author endo_akira
 */
// ============================================================================
#include <stdio.h>

#include "../../include/JoinFestaUI/JoinFestaPlayerListSelectLowerViewBase.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"

#include "Sound/include/Sound.h"

#include <arc_index/message.gaix>

#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_anim_list.h>
#include <niji_conv_header/app/join_festa_playerlist/join_festa_playerlist_pane.h>
#include <niji_conv_header/message/msg_jf_playerlist.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_JoinFestaPlayerListSelectLowerViewBase
namespace FILELOCAL {
  /** pokeiconが有効となるタイプ  */
  static const JoinFestaPlayerListSelectLowerViewBase::ListType  usingPokeIconTypeList[] =
  {
    JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST,
    JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP,
  };

  static u32 GetPokeIconIndex(const JoinFestaPlayerListSelectLowerViewBase::ListType listType, const u32 dataIndex)
  {
    const u32   dataIndexMax  = NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX;
    const bool  isValidType   = (listType == JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST) || (listType == JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_VIP);
    const bool  isValid       = (isValidType) && (dataIndex < dataIndexMax);
    GFL_ASSERT(isValid);
    
    if(!isValid)  return 0;
    
    return (((listType == JoinFestaPlayerListSelectLowerViewBase::LIST_TYPE_GUEST) ? 0 : 1) * dataIndexMax + dataIndex);
  }
}



static const gfl2::lyt::LytPaneIndex btns_tbl[JoinFestaPlayerListSelectLowerViewBase::LIST_ITEM_PANE_MAX] =
{
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_18,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_19,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_20,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_21,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_22,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_23,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_24,
  PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_25,
};

static const u32 anm[JoinFestaPlayerListSelectLowerViewBase::LIST_ITEM_PANE_MAX][4] =
{
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_18_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_18_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_18_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_18_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_19_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_19_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_19_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_19_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_20_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_20_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_20_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_20_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_21_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_21_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_21_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_21_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_22_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_22_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_22_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_22_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_23_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_23_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_23_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_23_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_24_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_24_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_24_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_24_TOUCH_RELEASE
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_25_TOUCH,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_25_RELEASE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_25_CANSEL,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_25_TOUCH_RELEASE
  },
};

static const u32 active_passive_anm[JoinFestaPlayerListSelectLowerViewBase::LIST_ITEM_PANE_MAX][2] =
{
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_18_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_18_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_19_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_19_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_20_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_20_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_21_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_21_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_22_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_22_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_23_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_23_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_24_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_24_PASSIVE,
  },
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_25_ACTIVE,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_25_PASSIVE,
  },
};


JoinFestaPlayerListSelectLowerViewBase::JoinFestaPlayerListSelectLowerViewBase(
  NetAppLib::System::ApplicationWorkBase* pWorkBase,
  NetAppLib::System::ResourceID layoutResourceID,
  NetAppLib::System::ResourceID menuCursorResourceID ) :
  NetAppLib::JoinFestaUI::JoinFestaPlayerListLowerViewBase( pWorkBase, layoutResourceID, menuCursorResourceID  ),
  m_aPaneListCursorInfos(),
  m_eListType( LIST_TYPE_VIP ),
  m_pJoinFestaPlayerListSelectLowerViewListener( NULL ),
  m_pPaneListView( NULL ),
  m_pPaneListData( NULL ),
  m_pMyPaneListData( NULL ),
  m_NullString( 4, pWorkBase->GetDeviceHeap() ),
  m_pActiveList( NULL ),
  m_bCheckBoxVisible( false ),
  m_bPaneListInputEnable( false ),
  m_bIsResetPaneListCursorInfo( false ),
  /*  pokeicon  */
  m_pPokeIconTool(NULL),
  m_pPokeparaProvider(NULL),
  /*  photo  */
  m_bPhotoIconEnable(false),
  m_bPhotoButtonEnable(false),
  /*  BFC  */
  m_bShowBFCGrade(false)
{
  gfl2::std::MemClear( m_aPaneListCursorInfos, sizeof( m_aPaneListCursorInfos ) );

  setupLayout( LAYOUT_WORK_ID_LOWER, LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000_LAYOUT_00 );
  setupButton();

  createPaneList();

  SetInputListener( this );
}


JoinFestaPlayerListSelectLowerViewBase::~JoinFestaPlayerListSelectLowerViewBase()
{
  deletePaneList();
}


void JoinFestaPlayerListSelectLowerViewBase::Update()
{
  JoinFestaPlayerListLowerViewBase::Update();
}


void JoinFestaPlayerListSelectLowerViewBase::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  JoinFestaPlayerListLowerViewBase::Draw( displayNo );
}


bool JoinFestaPlayerListSelectLowerViewBase::IsDrawing(void) const
{
  return JoinFestaPlayerListLowerViewBase::IsDrawing();
}


void JoinFestaPlayerListSelectLowerViewBase::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void JoinFestaPlayerListSelectLowerViewBase::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


void JoinFestaPlayerListSelectLowerViewBase::setupButton()
{

  app::ui::ButtonInfoEx buttonInfoTable[BUTTON_MAX] =
  {
    {
      BUTTON_ID_BACK_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_00_PASSIVE,
    },

    {
      BUTTON_ID_UPDATE_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__RELOADBUTTON_PASSIVE,
    },

    {
      BUTTON_ID_DECIDE_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_02_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_02_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_02_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_02_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_02_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_02_PASSIVE,
    },

    {
      BUTTON_ID_ARROW_LEFT, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_04_PASSIVE,
    },

    {
      BUTTON_ID_ARROW_RIGHT, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PARTS_03_PASSIVE,
    },

    {
      BUTTON_ID_INFO_BUTTON, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__INFOBUTTON_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__INFOBUTTON_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__INFOBUTTON_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__INFOBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__INFOBUTTON_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__INFOBUTTON_PASSIVE,
    },

    /*  photo  */
    {
      BUTTON_ID_PHOTOVIEW, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PHOTOBUTTON_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PHOTOBUTTON_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PHOTOBUTTON_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PHOTOBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PHOTOBUTTON_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__PHOTOBUTTON_PASSIVE,
    },

    {
      BUTTON_ID_SLIDESHOW, NULL, NULL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__SLIDESHOWBUTTON_TOUCH,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__SLIDESHOWBUTTON_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__SLIDESHOWBUTTON_CANSEL,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__SLIDESHOWBUTTON_TOUCH_RELEASE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__SLIDESHOWBUTTON_ACTIVE,
      LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000__SLIDESHOWBUTTON_PASSIVE,
    },

  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_00 );
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BACK_BUTTON ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pUpdateButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_RELOADBUTTON );
    buttonInfoTable[ BUTTON_ID_UPDATE_BUTTON ].picture_pane = pUpdateButton;
    buttonInfoTable[ BUTTON_ID_UPDATE_BUTTON ].bound_pane = pLower->GetBoundingPane( pUpdateButton, PANENAME_FESTA_BTN_LOW_009_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pUpdateButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_02 );
    buttonInfoTable[ BUTTON_ID_DECIDE_BUTTON ].picture_pane = pUpdateButton;
    buttonInfoTable[ BUTTON_ID_DECIDE_BUTTON ].bound_pane = pLower->GetBoundingPane( pUpdateButton, PANENAME_COMMON_BTN_LOW_013_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pArrowLeft = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_04 );
    buttonInfoTable[ BUTTON_ID_ARROW_LEFT ].picture_pane = pArrowLeft;
    buttonInfoTable[ BUTTON_ID_ARROW_LEFT ].bound_pane = pLower->GetBoundingPane( pArrowLeft, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pArrowRight = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PARTS_03 );
    buttonInfoTable[ BUTTON_ID_ARROW_RIGHT ].picture_pane = pArrowRight;
    buttonInfoTable[ BUTTON_ID_ARROW_RIGHT ].bound_pane = pLower->GetBoundingPane( pArrowRight, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pInfoButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_INFOBUTTON );
    buttonInfoTable[ BUTTON_ID_INFO_BUTTON ].picture_pane = pInfoButton;
    buttonInfoTable[ BUTTON_ID_INFO_BUTTON ].bound_pane = pLower->GetBoundingPane( pInfoButton, PANENAME_COMMON_BTN_LOW_002_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pInfoButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_PHOTOBUTTON );
    buttonInfoTable[ BUTTON_ID_PHOTOVIEW ].picture_pane = pInfoButton;
    buttonInfoTable[ BUTTON_ID_PHOTOVIEW ].bound_pane = pLower->GetBoundingPane( pInfoButton, PANENAME_FESTA_BTN_LOW_009_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pInfoButton = pLower->GetPartsPane( PANENAME_JFPL_MIN_LOW_000_PANE_SLIDESHOWBUTTON );
    buttonInfoTable[ BUTTON_ID_SLIDESHOW ].picture_pane = pInfoButton;
    buttonInfoTable[ BUTTON_ID_SLIDESHOW ].bound_pane = pLower->GetBoundingPane( pInfoButton, PANENAME_FESTA_BTN_LOW_009_PANE_BOUND_00, &gtsResourceID );
  }


  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BACK_BUTTON,   SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_UPDATE_BUTTON, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_DECIDE_BUTTON, SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_LEFT,    SEQ_SE_SELECT1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_ARROW_RIGHT,   SEQ_SE_SELECT1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_INFO_BUTTON,   SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_PHOTOVIEW,     SEQ_SE_DECIDE1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_SLIDESHOW,     SEQ_SE_DECIDE1 );

  /*  photo  */
  SetVisiblePhotoButton(false);
  SetVisibleSlideshowButton(false);
}


void JoinFestaPlayerListSelectLowerViewBase::createPaneList()
{
  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );
  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST );

  // -----------------------------------------------------------------------
  { // m_pPaneListData 初期化 ここから

    m_pPaneListData   = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) app::tool::PaneList::PANE_DATA[ LIST_ITEM_PANE_MAX ];
    m_pMyPaneListData = GFL_NEW_LOW_ARRAY( pAppHeap->GetDeviceHeap() ) MyPaneData[ LIST_ITEM_PANE_MAX ];
   
    for( u32 i = 0; i < LIST_ITEM_PANE_MAX; ++i )
    {
      gfl2::lyt::LytParts* pBTNS = pLower->GetPartsPane( btns_tbl[i] );
      m_pPaneListData[i].base       = pBTNS;
      m_pPaneListData[i].text       = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_TEXTBOX_00, &resourceID );
      m_pPaneListData[i].bound      = pLower->GetBoundingPane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_BOUND_00, &resourceID );
      m_pPaneListData[i].cursor_pos = pLower->GetPane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_NULL_CUR, &resourceID );

      m_pPaneListData[i].holdAnime    = anm[i][0];
      m_pPaneListData[i].releaseAnime = anm[i][1];
      m_pPaneListData[i].cancelAnime  = anm[i][2];
      m_pPaneListData[i].selectAnime  = anm[i][3];

      m_pMyPaneListData[i].pTextBox00     = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_TEXTBOX_00,  &resourceID );
      m_pMyPaneListData[i].pTextBox01     = pLower->GetTextBoxPane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_TEXTBOX_01,  &resourceID );
      m_pMyPaneListData[i].pCheckBox      = pLower->GetPane(        pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_BG_CHECK,    &resourceID );
      m_pMyPaneListData[i].pCheckMark     = pLower->GetPane(        pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_MARK_CHECK,  &resourceID );
      m_pMyPaneListData[i].pPokeIcon      = pLower->GetPicturePane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_POKEICON,    &resourceID );
      m_pMyPaneListData[i].pPhotoIcon    = pLower->GetPicturePane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_FSPHOTOICON, &resourceID );
      m_pMyPaneListData[i].pPhotoNewIcon = pLower->GetPicturePane( pBTNS, PANENAME_FESTA_BTN_LOW_008_PANE_ICON_00,     &resourceID );
      
      m_pMyPaneListData[i].pPokeIcon->SetVisible(false);
      m_pMyPaneListData[i].pPhotoIcon->SetVisible(false);
      m_pMyPaneListData[i].pPhotoNewIcon->SetVisible(false);
    }
  } // m_pPaneListData 初期化 ここまで
  // -----------------------------------------------------------------------

  // -----------------------------------------------------------------------
  { // m_pPaneListView 初期化 ここから

    app::tool::PaneList::SETUP_PARAM list_setup;
    setPaneListParam( &list_setup, 0, 0.0f );

    app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
    {
      pAppHeap->GetDeviceHeap(),
      app::tool::ScrollBar::SCROLL_DIRECTION_VERTICAL,
      list_setup.valMax-list_setup.listPosMax,

      pLower,
      resourceID,
      PANENAME_JFPL_MIN_LOW_000_PANE_SCROLL_BAR,
      PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
      PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

      NULL,
      NULL,
      NULL,
      NULL
    };

    m_pPaneListView = GFL_NEW( pAppHeap->GetDeviceHeap() ) app::tool::PaneListView( pAppHeap, true );
    m_pPaneListView->CreatePaneList( &list_setup );
    m_pPaneListView->CreateScrollBar( &scroll_bar_setup );
    m_pPaneListView->SetListener( this );
    setPaneListConfig( &list_setup );
    AddSubView( m_pPaneListView );
    setInputEnablePaneList( true );

  } // m_pPaneListView 初期化 ここまで
  // -----------------------------------------------------------------------
}


void JoinFestaPlayerListSelectLowerViewBase::deletePaneList()
{
  {
    m_pPaneListView->RemoveFromSuperView();
    m_pPaneListView->DeleteScrollBar();
    m_pPaneListView->DeletePaneList();
    GFL_SAFE_DELETE( m_pPaneListView );
  }

  {
    GFL_SAFE_DELETE_ARRAY( m_pMyPaneListData );
    GFL_SAFE_DELETE_ARRAY( m_pPaneListData );
  }
}


void JoinFestaPlayerListSelectLowerViewBase::setPaneListParam( app::tool::PaneList::SETUP_PARAM* pParam, u32 pos, f32 scroll )
{
  app::util::Heap* pAppHeap = m_pWorkBase->GetAppHeap();

  gfl2::lyt::LytWk* pLower = GetG2DUtil()->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  pParam->heap = pAppHeap->GetDeviceHeap();  // ヒープ

  pParam->pLytWk = pLower;    // レイアウトワーク
  pParam->paneList = m_pPaneListData;   // ペインデータ
  pParam->paneListMax = LIST_ITEM_PANE_MAX;              // ペインデータ数

  pParam->valMax = ( m_pActiveList ) ? m_pActiveList->Size() : 1;					// 実際に使用するリストの項目数（ペインの数ではない）
  pParam->listPosMax = 6;			// カーソル動作範囲（画面にはみ出る部分を除いた実際に入るリストの数）
  pParam->listPos = pos;				// 初期カーソル位置 ( 0 ～ listPosMax-1 の範囲 )
  pParam->listScroll = scroll;			// 初期スクロール位置 ( 0 ～ valMax-listPosMax-1 の範囲 )
                          //   一番下にカーソルを位置させた状態で始めたいのであれば、listScroll+listposが
                          //   valMax-1になるようにする設定する。

  pParam->defPY = 73.0f;					// ペインのデフォルトY座標 ( listPos = 0 の位置のペイン座標 )
                          //   この座標で指定した場所にあるペインが描画開始位置のペインになる

  /**
    @fix MMcat[499]   【フェスサークル】プレイヤーリスト画面のタッチ判定の横幅が小さい
          momijiでのPaneサイズ変更に対応できていなかったためサイズ調整
  */
  pParam->defSX = 288.0f;					// ペインのXサイズ(レイアウトデディターで厳密に確認）
  pParam->defSY = 28.0f;					// ペインのYサイズ(レイアウトデディターで厳密に確認）

  pParam->writeOffsetY = -1;		// 上方向の表示限界(リストが半分はみ出ているなどの場合は-1, きっちり画面に入っている場合は0)

  /** @fix  GFMMcat[1032]   【フェスサークル】ゲストのリストで一部ボタンの表示優先が手前になっている  */
  pParam->listStartPane = m_pPaneListData[0].base->GetParent();   // 項目基準位置のペイン ( NULLの場合、動作時にプライオリティ変更を行わない )

  pParam->cbFunc = this;	// コールバック関数
  pParam->cbWork = NULL;			// コールバック関数に渡すワーク

  pParam->uiDevMan = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();		// UIデバイスマネージャ
  pParam->menuCursor = m_MenuCursor.GetAppToolMenuCursor();		// カーソル
}


void JoinFestaPlayerListSelectLowerViewBase::setPaneListConfig( app::tool::PaneList::SETUP_PARAM* pParam )
{
  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->SetConfig( app::tool::PaneList::PAGE_SKIP_LR_BUTTON, true, false, true, false );
  pPaneList->SetSE( SEQ_SE_SELECT1, SEQ_SE_DECIDE1, SEQ_SE_CANCEL1 );

  { // タッチ範囲
    gfl2::math::VEC3 pos = m_pPaneListData[0].bound->GetTranslate();
    u32 pos_max = pParam->listPosMax;
    if( pParam->valMax < pos_max )
    {
      pos_max = pParam->valMax;
    }

    /**
      @fix MMcat[499]   【フェスサークル】プレイヤーリスト画面のタッチ判定の横幅が小さい
            momijiでのPaneサイズ変更に対応できていなかったため位置調整
    */
    f32 lx = 15.0f - pParam->defSX / 2.0f;
    f32 rx = lx + pParam->defSX;
    f32 uy = pParam->defPY + pos.y + pParam->defSY / 2.0f;
    f32 dy = uy - pParam->defSY * pos_max;
    pPaneList->MakeTouchArea( lx, rx, uy, dy );
  }

}


void JoinFestaPlayerListSelectLowerViewBase::setInputEnablePaneList( bool bInputEnable )
{
  m_bPaneListInputEnable = bInputEnable;

  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      pPaneList->SetInputEnabled( bInputEnable );
    }
  }
}


void JoinFestaPlayerListSelectLowerViewBase::backupPaneListCursorInfo()
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    
    if( pPaneList )
    {
      pPaneList->GetCursorData( &m_aPaneListCursorInfos[ m_eListType ].pos, &m_aPaneListCursorInfos[ m_eListType ].scroll );
    }
  }
}


void JoinFestaPlayerListSelectLowerViewBase::ResetPaneListCursorInfo()
{
  gfl2::std::MemClear( m_aPaneListCursorInfos, sizeof( m_aPaneListCursorInfos ) );
  m_bIsResetPaneListCursorInfo = true;
}


void JoinFestaPlayerListSelectLowerViewBase::SetList( NetAppLib::JoinFestaUI::JoinFestPlayerDataList* pList, u32 screenTitleMessageID, ListType eListType )
{
  setScreenTitleName( screenTitleMessageID );

  m_pActiveList = pList;
  m_eListType = eListType;

  const u32 animeIDTable[ LIST_TYPE_MAX ] =
  {
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000_PLAYERPATERN_01,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000_PLAYERPATERN_00,
    LA_JOIN_FESTA_PLAYERLIST_JFPL_MIN_LOW_000_PLAYERPATERN_02
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  pG2DUtil->StartAnime( LAYOUT_WORK_ID_LOWER, animeIDTable[ m_eListType ] );

  UpdateList();
}


void JoinFestaPlayerListSelectLowerViewBase::UpdateList()
{
  // 同じリストが設定される時、リストサイズに変動があったらカーソル位置をリセットする
  if( m_aPaneListCursorInfos[ m_eListType ].listMax > 0 )
  {
    if( m_aPaneListCursorInfos[ m_eListType ].listMax != m_pActiveList->Size() )
    {
      m_aPaneListCursorInfos[ m_eListType ].pos = 0;
      m_aPaneListCursorInfos[ m_eListType ].scroll = 0.0f;
    }
  }

  app::tool::PaneList::SETUP_PARAM list_setup;
  setPaneListParam( &list_setup, m_aPaneListCursorInfos[ m_eListType ].pos, m_aPaneListCursorInfos[ m_eListType ].scroll );
  m_pPaneListView->ResetPaneList( &list_setup );
  setPaneListConfig( &list_setup );
  m_aPaneListCursorInfos[ m_eListType ].listMax = m_pActiveList->Size();

  app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
  pPaneList->InitListPut();



  NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData = NULL;

  if( m_bIsResetPaneListCursorInfo )
  {
    m_bIsResetPaneListCursorInfo = false;
  }
  pJoinFestaPlayerData = getJoinFestaPlayerData(pPaneList->GetItemPos());

  if( m_pJoinFestaPlayerListSelectLowerViewListener )
  {
    m_pJoinFestaPlayerListSelectLowerViewListener->SetActivePlayer( pJoinFestaPlayerData );
  }

  /*  photo  */
  updatePhotoButton(pJoinFestaPlayerData);
}


::app::ui::UIInputListener::ListenerResult JoinFestaPlayerListSelectLowerViewBase::OnLayoutPaneEvent( const u32 painId )
{
  if( m_pPaneListView->IsButtonAnimation() )
  {
    return DISABLE_ACCESS;
  }

  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    case BUTTON_ID_BACK_BUTTON:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_UPDATE_BUTTON:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->TouchUpdateButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_DECIDE_BUTTON:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->TouchDecideButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_ARROW_LEFT:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->PushLButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;
    
    case BUTTON_ID_ARROW_RIGHT:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->PushRButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_INFO_BUTTON:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->TouchInfoButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;

    case  BUTTON_ID_PHOTOVIEW:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->TouchPhotoViewButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;
      
    case  BUTTON_ID_SLIDESHOW:
    {
      if( m_pJoinFestaPlayerListSelectLowerViewListener )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->TouchSlideShowButton();
      }
      backupPaneListCursorInfo();
      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


::app::ui::UIInputListener::ListenerResult JoinFestaPlayerListSelectLowerViewBase::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  if( IsButtonAnimation() || m_pPaneListView->IsButtonAnimation() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_BACK_BUTTON );
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_X ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_UPDATE_BUTTON );
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    /*  同時には有効にならないので両方コールしてしまう  */
    m_button_manager->StartSelectedAct( BUTTON_ID_DECIDE_BUTTON );
    m_button_manager->StartSelectedAct( BUTTON_ID_PHOTOVIEW );
    result = DISABLE_ACCESS;
  }
  else
  if( pKey->IsTrigger( gfl2::ui::DIR_LEFT ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_ARROW_LEFT );
    result = DISABLE_ACCESS;
  }
  else
  if( pKey->IsTrigger( gfl2::ui::DIR_RIGHT ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_ARROW_RIGHT );
    result = DISABLE_ACCESS;
  }

  return result;
}


void JoinFestaPlayerListSelectLowerViewBase::CallBack_CursorMove( void * work, const app::tool::PaneList::PANE_DATA * cur_pane, u32 pane_index, u32 pos )
{
  NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData = getJoinFestaPlayerData( pos );
  if( pJoinFestaPlayerData )
  {
    if( m_pJoinFestaPlayerListSelectLowerViewListener )
    {
      m_pJoinFestaPlayerListSelectLowerViewListener->SetActivePlayer( pJoinFestaPlayerData );
    }

  }
  /*  photo  */
  updatePhotoButton(pJoinFestaPlayerData);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::updatePhotoButton(const NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJFPlayerData)
{
  if(m_bPhotoButtonEnable && pJFPlayerData)
  {
    const JoinFestaPlayerData::PhotoDataStatus  photoStatus = pJFPlayerData->FS_GetPhotoDataStatus();
    const bool                                  bPhotoDataAvailable = (photoStatus != JoinFestaPlayerData::PhotoData_NotAvailable);
    SetVisiblePhotoButton(bPhotoDataAvailable);
  }
  else
  {
    SetVisiblePhotoButton(false);
  }
}


void JoinFestaPlayerListSelectLowerViewBase::CallBack_CursorOff( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::CallBack_ListWrite( void * work, const app::tool::PaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  NetAppLib::Message::MessageUtility* pMessageUtility = m_pWorkBase->GetMessageUtility();

  m_pMyPaneListData[ pane_index ].pCheckBox->SetVisible( m_bCheckBoxVisible );

  NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData = getJoinFestaPlayerData( pos );
  
  if( pJoinFestaPlayerData )
  {
    gfl2::str::StrBuf* pTempStrBuf = pG2DUtil->GetTempStrBuf(0);

    if( pJoinFestaPlayerData->IsBlack() )
    {
      pTempStrBuf->SetStr( pJoinFestaPlayerData->GetBlackPlayerCoreData().name );
    }
    else
    {
      pTempStrBuf->SetStr( pJoinFestaPlayerData->GetName() );
    }

    pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox00, pTempStrBuf );

    if( pJoinFestaPlayerData->IsBlack() )
    {
      pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &m_NullString );
    }
    else
    {
      /*  「フレンド」/「グレード」  */
      if(m_bShowBFCGrade)
      {
        /*  pokeparaが設定済みということはPacketGameStatus1が有効ということであるが、明示的に両方確認しておく  */
        NetAppLib::JoinFesta::PacketGameStatus1 val;
        const bool                              bPokeparaAvailable  = pJoinFestaPlayerData->BFC_GetPokeparaAvailability();
        const bool                              bPacketAvailable    = pJoinFestaPlayerData->GetPacketGameStatus1(&val);

        /*  グレード表示  */
        if(bPokeparaAvailable && bPacketAvailable)
        {
          pG2DUtil->SetRegisterNumber( 0, val.battleFesRank, 2);
          pG2DUtil->SetTextBoxPaneStringExpand(m_pMyPaneListData[ pane_index ].pTextBox01, pMessageUtility->GetString(GARC_message_jf_playerlist_DAT, jf_playerlist_cap_79));
        }
        else
        {
          pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &m_NullString );
        }
      }
      else
      {
        /*  フレンド表示  */
        if( pJoinFestaPlayerData->IsCtrFriend() )
        {
          pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &pMessageUtility->GetString( GARC_message_jf_playerlist_DAT, jf_playerlist_cap_02 ) );
        }
        else
        {
          pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &m_NullString );
        }
      }
    }

    if( pJoinFestaPlayerData->IsPassive() )
    {
      setPassive( pane_index );
    }
    else
    {
      setActive( pane_index );
    }

    /*  checkmark */
    m_pMyPaneListData[ pane_index ].pCheckMark->SetVisible( pJoinFestaPlayerData->IsSelect() );


    /*  pokeicon  */
    {
      gfl2::lyt::LytPicture*  pIconPane     = m_pMyPaneListData[pane_index].pPokeIcon;
      bool                    bIconVisible  = false;

      if(m_pPokeIconTool && pIconPane)
      {
        const pml::pokepara::CoreParam* const pCoreParam  = pJoinFestaPlayerData->BFC_GetPokepara();
        if(pCoreParam)
        {
          m_pPokeIconTool->ReplaceIcon(FILELOCAL::GetPokeIconIndex(m_eListType, pos), pIconPane);
        }
        pIconPane->SetVisible(!!pCoreParam);
      }
    }
    
    /*  photo  */
    updatePhotoIcon(pane_index, *pJoinFestaPlayerData);

    /*  逆引き用にposをuserdataとしてセットしておく  */
    {
      char      buf[nw::lyt::UserDataStrMax+1]={0};
      const u32 elems = GFL_NELEMS(buf);
      const u32 chars = elems-1;
      
      nw::ut::snprintf(buf, elems, chars, "%d", pos);
      pane[pane_index].base->SetUserData(buf);
    }
  }
  else
  {
    pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox00, &m_NullString );
    pG2DUtil->SetTextBoxPaneString( m_pMyPaneListData[ pane_index ].pTextBox01, &m_NullString );
  }

}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::RefreshCheckBox(void)
{
  app::tool::PaneList*  pPaneList     = m_pPaneListView->GetPaneList();
  const u32             dataIndex     = pPaneList->GetItemPos();
  u32                   cursorPos     = 0;
  u32                   paneListIndex = 0;

  pPaneList->GetCursorData( &cursorPos, NULL );
  paneListIndex = pPaneList->GetPosPaneIndex( cursorPos );
  
  /**
    @fix  MMCat[181]   【フェスサークル】プレイヤーリストの「ゲスト」以外のページから写真を閲覧すると、アサートが発生してとまる
  */
  {
    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJFPD  = getJoinFestaPlayerData(dataIndex);
    if(pJFPD)
    {
      m_pMyPaneListData[paneListIndex].pCheckMark->SetVisible( pJFPD->IsSelect() );
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    カーソル位置のカメラアイコンの状態を更新する
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::RefreshPhotoIcon(void)
{
  app::tool::PaneList*  pPaneList     = m_pPaneListView->GetPaneList();
  u32                   cursorPos     = 0;
  u32                   paneListIndex = 0;

  pPaneList->GetCursorData( &cursorPos, NULL );
  paneListIndex = pPaneList->GetPosPaneIndex( cursorPos );
  RefreshPhotoIcon(paneListIndex);
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::RefreshPhotoIcon(const u32 targetPaneListIndex)
{
  const char* pUserData = m_pPaneListData[targetPaneListIndex].base->GetUserData();

  if(pUserData && (pUserData[0] != '\0'))
  {
    const s32 dataIndex = atoi(pUserData);
    NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJFPD  = getJoinFestaPlayerData(dataIndex);

    /**
      @fix  MMCat[181]   【フェスサークル】プレイヤーリストの「ゲスト」以外のページから写真を閲覧すると、アサートが発生してとまる
    */
    if(pJFPD)
    {
      updatePhotoIcon(targetPaneListIndex, *pJFPD);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::RefreshPhotoIconAll(void)
{
  for(u32 index = 0; index < LIST_ITEM_PANE_MAX; ++index)
  {
    RefreshPhotoIcon(index);
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::updatePhotoIcon(const u32 paneListIndex, const JoinFestaPlayerData& rJFPlayerData)
{
  if(m_bPhotoIconEnable)
  {
    const MyPaneData& rMyPaneData = m_pMyPaneListData[paneListIndex];
    
    gfl2::lyt::LytPicture* const pPhotoIcon    = rMyPaneData.pPhotoIcon;
    gfl2::lyt::LytPicture* const pPhotoNewIcon = rMyPaneData.pPhotoNewIcon;

    if(pPhotoIcon && pPhotoNewIcon)
    {
      const JoinFestaPlayerData::PhotoDataStatus  photoStatus         = rJFPlayerData.FS_GetPhotoDataStatus();
      const bool                                  bPhotoDataAvailable = (photoStatus != JoinFestaPlayerData::PhotoData_NotAvailable);
      const bool                                  bPhotoDataNew       = (photoStatus == JoinFestaPlayerData::PhotoData_AvailableNew);
      
      pPhotoIcon->SetVisible(bPhotoDataAvailable);
      pPhotoNewIcon->SetVisible(bPhotoDataNew);
    }
  }
}



void JoinFestaPlayerListSelectLowerViewBase::CallBack_ItemChange( void * work, u32 pos1, u32 pos2 )
{
}


void JoinFestaPlayerListSelectLowerViewBase::PaneListItemSelectListener( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  backupPaneListCursorInfo();

  NetAppLib::JoinFestaUI::JoinFestaPlayerData* pJoinFestaPlayerData = getJoinFestaPlayerData( pos );

  if( pJoinFestaPlayerData )
  {
    if( m_pJoinFestaPlayerListSelectLowerViewListener )
    {
      if( pJoinFestaPlayerData->IsPassive() )
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->SetSelectPassivePlayer( pJoinFestaPlayerData );
      }
      else
      {
        m_pJoinFestaPlayerListSelectLowerViewListener->SetSelectPlayer( pJoinFestaPlayerData );
      }
      m_pJoinFestaPlayerListSelectLowerViewListener->SetActivePlayer( pJoinFestaPlayerData );
    }
  }
  /*  photo  */
  updatePhotoButton(pJoinFestaPlayerData);
}


NetAppLib::JoinFestaUI::JoinFestaPlayerData* JoinFestaPlayerListSelectLowerViewBase::getJoinFestaPlayerData( u32 index )
{
  if( m_pActiveList )
  {
    u32 count = 0;
    for( NetAppLib::JoinFestaUI::ItJoinFestPlayerDataList it = m_pActiveList->Begin(); it != m_pActiveList->End(); ++it )
    {
      if( count == index )
      {
        return &(*it);
      }
      ++count;
    }
  }
  return NULL;
}


void JoinFestaPlayerListSelectLowerViewBase::SetPassive()
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      u32 cursorPos = 0;
      pPaneList->GetCursorData( &cursorPos, NULL );
      u32 paneIndex = pPaneList->GetPosPaneIndex( cursorPos );
      if( paneIndex < LIST_ITEM_PANE_MAX )
      {
        setPassive( paneIndex );
      }
    }
  }
}

void JoinFestaPlayerListSelectLowerViewBase::setActive( u32 paneIndex )
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      pPaneList->SetButtonSEPaneIndex( paneIndex, SEQ_SE_DECIDE1 );
      
      nw::ut::Color8 color = nw::ut::Color8::BLACK;
      
      m_pPaneListData[ paneIndex ].text->SetTextColor( color, color );
    }
  }
}


void JoinFestaPlayerListSelectLowerViewBase::setPassive( u32 paneIndex )
{
  if( m_pPaneListView )
  {
    app::tool::PaneList* pPaneList = m_pPaneListView->GetPaneList();
    if( pPaneList )
    {
      pPaneList->SetButtonSEPaneIndex( paneIndex, SEQ_SE_BEEP );
      
      nw::ut::Color8 color = nw::ut::Color8::GRAY;

      m_pPaneListData[ paneIndex ].text->SetTextColor( color, color );
    }
  }
}


void JoinFestaPlayerListSelectLowerViewBase::SetActiveDecideButton()
{
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonActive( BUTTON_ID_DECIDE_BUTTON );
}


void JoinFestaPlayerListSelectLowerViewBase::SetPassiveDecideButton()
{
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonPassive( BUTTON_ID_DECIDE_BUTTON );
}


//------------------------------------------------------------------
/**
  *  @brief    pokeIcon準備
  */
//------------------------------------------------------------------
void JoinFestaPlayerListSelectLowerViewBase::SetupPokeIcon(IPokeparaProvider* pObject, app::util::Heap* pIconHeap)
{
  const bool  isValidState  = (GetG2DUtil()) && (pObject) && (!m_pPokeIconTool);

  GFL_ASSERT(isValidState);
  if(!isValidState) return;


  const u32 ListItemMax = NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX;

  gfl2::heap::HeapBase*                         pHeapBase = pIconHeap->GetDeviceHeap();

  /*  icontool  */
  m_pPokeparaProvider = pObject;
  {
    app::tool::PokeIconToolEx::PokeIconToolParam  param;

    param.pUIView     = this;
    param.pHeap       = pIconHeap;
    param.layout_id   = LAYOUT_WORK_ID_LOWER;
    param.res_id      = LAYOUT_RESOURCE_ID_JOINFESTAPLAYERLIST;
    param.iconMax     = ListItemMax * 2;   /*  LIST_MAX * (guest + VIP) * 1(single)  */
    param.sync        = true;
    param.allocBuffer = false;
    
    m_pPokeIconTool  = GFL_NEW(pHeapBase) app::tool::PokeIconToolEx(param);
  }

  /*  loadicon  */
  {
    for(u32 typeIndex = 0; typeIndex < GFL_NELEMS(FILELOCAL::usingPokeIconTypeList); ++typeIndex)
    {
      const ListType  type  = FILELOCAL::usingPokeIconTypeList[typeIndex];
      for(u32 listItemIndex = 0; listItemIndex < ListItemMax; ++listItemIndex)
      {
        const pml::pokepara::CoreParam* const pCoreParam  = m_pPokeparaProvider->OnRequestPokepara(this, type, listItemIndex);
        const u32                             dataIndex   = FILELOCAL::GetPokeIconIndex(type, listItemIndex);
        
        if(pCoreParam)
        {
          m_pPokeIconTool->LoadPokeIcon(dataIndex, pCoreParam);
        }
      }
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool JoinFestaPlayerListSelectLowerViewBase::IsPokeIconLoading(void)
{
  const u32 ListItemMax = NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX;
  bool      isLoading   = false;

  if(m_pPokeIconTool)
  {
    for(u32 typeIndex = 0; (typeIndex < GFL_NELEMS(FILELOCAL::usingPokeIconTypeList)) && (!isLoading); ++typeIndex)
    {
      const ListType  type  = FILELOCAL::usingPokeIconTypeList[typeIndex];
      for(u32 listItemIndex = 0; (listItemIndex < ListItemMax) && (!isLoading); ++listItemIndex)
      {
        if(!(m_pPokeparaProvider->OnRequestPokepara(this, type, listItemIndex))) continue;  /*  LoadPokeIconを行っていないslotはIsLoadPokeIconでfalseが返る  */

        isLoading = !m_pPokeIconTool->IsLoadPokeIcon(FILELOCAL::GetPokeIconIndex(type, listItemIndex));
      }
    }
  }
  
  
  return isLoading;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
bool JoinFestaPlayerListSelectLowerViewBase::CleanupPokeIcon(void)
{
  bool  isEnd = true;

  if(m_pPokeIconTool)
  {
    isEnd = m_pPokeIconTool->EndFunc();
    if(isEnd)
    {
      GFL_SAFE_DELETE(m_pPokeIconTool);
    }
  }
  
  return isEnd;
}


// @fix GFMMCat[27]: インフォボタンの座標取得
void JoinFestaPlayerListSelectLowerViewBase::GetInfoButtonPos( gfl2::math::Vector3 * vec )
{
  gfl2::math::VEC3 pos = GetButtonManager()->GetButtonPos( BUTTON_ID_INFO_BUTTON );
  *vec = gfl2::math::ConvertNwVec3ToGfVec3( pos );
}



GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
