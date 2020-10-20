// ============================================================================
/*
 * @file GTSTradePersonSelectLowerView.cpp
 * @brief 交換相手を選択する画面を表示するクラスです。
 * @date 2015.04.15
 */
// ============================================================================
#include "NetApp/GTS/source/TradePersonSelect/GTSTradePersonSelectLowerView.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSResourceID.h"
#include "NetStatic/NetAppLib/include/System/ResourceManager.h"
#include "AppLib/include/Tool/app_tool_MenuCursor.h"
#include "System/include/PokemonLanguage.h"
#include <arc_index/message.gaix>
#include <niji_conv_header/app/GTS/GTS.h>
#include <niji_conv_header/app/GTS/GTS_anim_list.h>
#include <niji_conv_header/app/GTS/GTS_pane.h>
#include <niji_conv_header/message/msg_gts.h>
#include <niji_conv_header/battle/chara/p2_base.h>
#include <niji_conv_header/chara_test/dressup_item_id.h>
#include <niji_conv_header/chara_test/dressup_color_id.h>
#include <niji_conv_header/sound/SoundMiddleID.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePersonSelect)


static const u32 NULLPaneTable[] =
{
  PANENAME_GTS_USR_LOW_000_PANE_PLAYER_00,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYER_01,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYER_02,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYER_03,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYER_04
};


static const u32 PicturePaneTable[] =
{
  PANENAME_GTS_USR_LOW_000_PANE_PLAYERPHOTO_00,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYERPHOTO_01,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYERPHOTO_02,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYERPHOTO_03,
  PANENAME_GTS_USR_LOW_000_PANE_PLAYERPHOTO_04
};

static const u32 WindowFramePaneTable[] =
{
  PANENAME_GTS_USR_LOW_000_PANE_WINDOW_PLAYER_FRAME_00,
  PANENAME_GTS_USR_LOW_000_PANE_WINDOW_PLAYER_FRAME_01,
  PANENAME_GTS_USR_LOW_000_PANE_WINDOW_PLAYER_FRAME_02,
  PANENAME_GTS_USR_LOW_000_PANE_WINDOW_PLAYER_FRAME_03,
  PANENAME_GTS_USR_LOW_000_PANE_WINDOW_PLAYER_FRAME_04
};


GTSTradePersonSelectLowerView::GTSTradePersonSelectLowerView( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetAppLib::System::NetApplicationViewBase(
    pGTSWork,
    NetApp::GTS::GTS_RESOURCE_ID_LAYOUT_GTS,
    LYTRES_GTS_GTS_USR_LOW_000_BFLYT,
    LA_GTS_GTS_USR_LOW_000___NUM,
    app::util::G2DUtil::SETUP_LOWER,
    pGTSWork->GetMessageUtility(),
    GARC_message_gts_DAT ),
  m_pGTSWork( pGTSWork ),
  m_pTradePersonSelectDrawListener( NULL ),
  m_pNewPaneList( NULL ),
  m_listPaneData(),
  m_pSearchResultList( NULL ),
  m_SelectIndex( 0 ),
  m_bRefineSearch( false ),
  m_NullString( 4, pGTSWork->GetDeviceHeap() ),
  m_bPaneListInputEnable( false ),
  m_bPaneListSetInputInvalidMode( false ),
  m_MenuCursor( pGTSWork, GetG2DUtil(), NetApp::GTS::GTS_RESOURCE_ID_MENU_CURSOR, LAYOUT_RESOURCE_ID_GTS, LAYOUT_WORK_ID_LOWER )
{
  setupTextMessage();
  setupButton();

  SetInputListener( this );
}


GTSTradePersonSelectLowerView::~GTSTradePersonSelectLowerView()
{
  deletePaneList();
}


bool GTSTradePersonSelectLowerView::IsDrawing(void) const
{
  bool result = false;
  
  if( !m_MenuCursor.CanDestroy() )
  {
    result = true;
  }

  if( UIView::IsDrawing() )
  {
    result = true;
  }

  return result;
}


void GTSTradePersonSelectLowerView::createPaneList( NetApp::GTS::ApplicationSystem::GtsSearchResultList* pSearchResultList, bool bBegin )
{
  m_pSearchResultList = pSearchResultList;

  deletePaneList();

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytMultiResID resourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  f32 fListScroll = 0.0f;

  if( !bBegin )
  {
    int nListSize = static_cast<int>( pSearchResultList->Size() );
    nListSize -= 1;
    if( nListSize < 0 )
    {
      nListSize = 0;
    }
    fListScroll = static_cast<f32>( nListSize );
  }

  App::Tool::NewPaneList::SETUP_PARAM param =
  {
    App::Tool::NewPaneList::TYPE_H,

    pLower,
    m_listPaneData,
    PANE_MAX,

    pSearchResultList->Size(),
    0,
    0,
    fListScroll,

    0.0f,
    38.0f,

    107.0f,

    -1,
    1,

    NULL,
  };

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  for( u32 i = 0; i < PANE_MAX; i++ )
  {
    m_listPaneData[i].base        = param.lytwk->GetPane( NULLPaneTable[i] );
    m_listPaneData[i].bound       = m_listPaneData[i].base;
    m_listPaneData[i].cursor_pos  = m_listPaneData[i].base;
    m_listPaneData[i].anm_touch   = app::tool::ButtonManager::ANIMATION_NULL;
    m_listPaneData[i].anm_release = app::tool::ButtonManager::ANIMATION_NULL;
    m_listPaneData[i].anm_cancel  = app::tool::ButtonManager::ANIMATION_NULL;
    m_listPaneData[i].anm_select  = app::tool::ButtonManager::ANIMATION_NULL;
  }

  app::util::ScrollBar::SETUP_PARAM scroll_bar_setup =
  {
    pAppHeap->GetDeviceHeap(),
    app::tool::ScrollBar::SCROLL_DIRECTION_HORIZONTAL,
    param.list_max - param.list_pos_max - 1,

    pLower,
    resourceID,
    PANENAME_GTS_USR_LOW_000_PANE_SCROLL,
    PANENAME_COMMON_SCR_LOW_000_PANE_ICON_SLIDER,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_TOP,
    PANENAME_COMMON_SCR_LOW_000_PANE_NULL_BOTTOM,

    NULL,
    NULL,
    NULL,
    NULL
  };

  m_pNewPaneList = GFL_NEW( pAppHeap->GetDeviceHeap() ) App::Tool::NewPaneList( pAppHeap, &param, this );
  m_pNewPaneList->SetupTouchArea( 40, 160, 0, 320 );
  m_pNewPaneList->SetupNoLoop();
  m_pNewPaneList->SetupInputMode( true, true, true );
  m_pNewPaneList->SetupSkipMode( App::Tool::NewPaneList::SKIP_MODE_NONE );

  m_pNewPaneList->Initialize();
  m_pNewPaneList->CreateScrollBar( &scroll_bar_setup );

  AddSubView( m_pNewPaneList );
  setInputEnablePaneList( true );

  m_MenuCursor.GetAppToolMenuCursor()->PutNonVisible( pLower, pLower->GetPane( PANENAME_GTS_USR_LOW_000_PANE_CUR_MAIN ) );
}


void GTSTradePersonSelectLowerView::deletePaneList()
{
  if( m_pNewPaneList != NULL )
  {
    m_pNewPaneList->RemoveFromSuperView();
    m_pNewPaneList->DeleteScrollBar();
    m_pNewPaneList->Terminate();
    GFL_SAFE_DELETE( m_pNewPaneList );
  }
}


void GTSTradePersonSelectLowerView::setupTextMessage()
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  // 下画面のTextを置き換え
  {
    // 画面上のテキスト
    {
      gfl2::lyt::LytParts* pMsgL1 = pLower->GetPartsPane( PANENAME_GTS_USR_LOW_000_PANE_MSG_L1 );
      gfl2::lyt::LytTextBox* pTextBox = pLower->GetTextBoxPane( pMsgL1, PANENAME_COMMON_MSG_LOW_003_PANE_TEXTBOX_00, &gtsResourceID );
      pTextBox->SetVisible( true );
      pG2DUtil->SetTextBoxPaneStringExpand( pTextBox, msg_gts_win_14 );
    }
  }
}


void GTSTradePersonSelectLowerView::setupButton()
{
  app::ui::ButtonInfoEx buttonInfoTable[] =
  {
    {
      BUTTON_ID_BTN_BACK, NULL, NULL,
      LA_GTS_GTS_USR_LOW_000__BTN_BACK_TOUCH,
      LA_GTS_GTS_USR_LOW_000__BTN_BACK_RELEASE,
      LA_GTS_GTS_USR_LOW_000__BTN_BACK_CANSEL,
      LA_GTS_GTS_USR_LOW_000__BTN_BACK_TOUCH_RELEASE,
      LA_GTS_GTS_USR_LOW_000__BTN_BACK_ACTIVE,
      LA_GTS_GTS_USR_LOW_000__BTN_BACK_PASSIVE,
    },
    {
      BUTTON_ID_BTN_SEARCH, NULL, NULL,
      LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_TOUCH,
      LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_RELEASE,
      LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_CANSEL,
      LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_TOUCH_RELEASE,
      LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_ACTIVE,
      LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_PASSIVE,
    },
    {
      BUTTON_ID_CURSOR_L, NULL, NULL,
      LA_GTS_GTS_USR_LOW_000__CUR_L_TOUCH,
      LA_GTS_GTS_USR_LOW_000__CUR_L_RELEASE,
      LA_GTS_GTS_USR_LOW_000__CUR_L_CANSEL,
      LA_GTS_GTS_USR_LOW_000__CUR_L_TOUCH_RELEASE,
      LA_GTS_GTS_USR_LOW_000__CUR_L_ACTIVE,
      LA_GTS_GTS_USR_LOW_000__CUR_L_PASSIVE,
    },
    {
      BUTTON_ID_CURSOR_R, NULL, NULL,
      LA_GTS_GTS_USR_LOW_000__CUR_R_TOUCH,
      LA_GTS_GTS_USR_LOW_000__CUR_R_RELEASE,
      LA_GTS_GTS_USR_LOW_000__CUR_R_CANSEL,
      LA_GTS_GTS_USR_LOW_000__CUR_R_TOUCH_RELEASE,
      LA_GTS_GTS_USR_LOW_000__CUR_R_ACTIVE,
      LA_GTS_GTS_USR_LOW_000__CUR_R_PASSIVE,
    },
  };

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  {
    gfl2::lyt::LytParts* pBtnBack = pLower->GetPartsPane( PANENAME_GTS_USR_LOW_000_PANE_BTN_BACK );
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].picture_pane = pBtnBack;
    buttonInfoTable[ BUTTON_ID_BTN_BACK ].bound_pane = pLower->GetBoundingPane( pBtnBack, PANENAME_COMMON_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pBtnSearch = pLower->GetPartsPane( PANENAME_GTS_USR_LOW_000_PANE_BTN_SEARCH );
    buttonInfoTable[ BUTTON_ID_BTN_SEARCH ].picture_pane = pBtnSearch;
    buttonInfoTable[ BUTTON_ID_BTN_SEARCH ].bound_pane = pLower->GetBoundingPane( pBtnSearch, PANENAME_GTS_BTN_LOW_000_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pCursorL = pLower->GetPartsPane( PANENAME_GTS_USR_LOW_000_PANE_CUR_L );
    buttonInfoTable[ BUTTON_ID_CURSOR_L ].picture_pane = pCursorL;
    buttonInfoTable[ BUTTON_ID_CURSOR_L ].bound_pane = pLower->GetBoundingPane( pCursorL, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  {
    gfl2::lyt::LytParts* pCursorR = pLower->GetPartsPane( PANENAME_GTS_USR_LOW_000_PANE_CUR_R );
    buttonInfoTable[ BUTTON_ID_CURSOR_R ].picture_pane = pCursorR;
    buttonInfoTable[ BUTTON_ID_CURSOR_R ].bound_pane = pLower->GetBoundingPane( pCursorR, PANENAME_COMMON_CUR_LOW_004_PANE_BOUND_00, &gtsResourceID );
  }

  app::util::Heap* pAppHeap = m_pGTSWork->GetAppHeap();

  app::ui::UIResponder::CreateButtonManager( pAppHeap, pLower, buttonInfoTable, BUTTON_MAX );
  app::tool::ButtonManager* pButtonManager = GetButtonManager();
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_BACK, SEQ_SE_CANCEL1 );
  pButtonManager->SetButtonSelectSE( BUTTON_ID_BTN_SEARCH, SEQ_SE_DECIDE1 );
}


void GTSTradePersonSelectLowerView::setPokemonInfo( u32 index )
{
  NetApp::GTS::GtsSearchResultInfo* pSearchResultInfo = getSearchResultInfo( index );

  if( pSearchResultInfo == NULL )
  {
    return;
  }

  m_SelectIndex = index;

  NetApp::GTS::GtsIndexData& gtsIndexData = pSearchResultInfo->indexData;

  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytMultiResID gtsResourceID = pG2DUtil->GetLayoutResourceID( LAYOUT_RESOURCE_ID_GTS );

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );


  u32 listSize = ( m_pSearchResultList ) ? m_pSearchResultList->Size() : 0;
  u32 listLastIndex = ( listSize > 0 ) ? listSize - 1 : 0;

  if( m_bRefineSearch )
  {
    setCursorVisible( false, false );
  }
  else
  if( index >= listLastIndex && listLastIndex > 0 )
  {
    if( listSize >= 100 )
    {
      setCursorVisible( false, true );
    }
    else
    {
      setCursorVisible( false, false );
    }
  }
  else
  if( index == 0 )
  {
    if( m_pGTSWork->GetSearchRangeStartOffset() == 0 )
    {
      setCursorVisible( false, false );
    }
    else
    {
      setCursorVisible( true, false );
    }
  }
  else
  {
    setCursorVisible( false, false );
  }

  // 下画面
  {
    // ポケモントレーナーの名前
    {
      gfl2::lyt::LytTextBox* pTextBox_00 = pLower->GetTextBoxPane( PANENAME_GTS_USR_LOW_000_PANE_TEXTBOX_00 );
      pG2DUtil->SetTextBoxPaneString( pTextBox_00, reinterpret_cast<gfl2::str::STRCODE*>(gtsIndexData.TrainerName) );
    }

    // 国
    gfl2::lyt::LytTextBox* pTextBox_01 = pLower->GetTextBoxPane( PANENAME_GTS_USR_LOW_000_PANE_TEXTBOX_01 );
    // 地域
    gfl2::lyt::LytTextBox* pTextBox_02 = pLower->GetTextBoxPane( PANENAME_GTS_USR_LOW_000_PANE_TEXTBOX_02 );

#if defined( GF_PLATFORM_CTR )

    app::util::AppCountryRegionNameUtility* pAppCountryRegionNameUtility = m_pGTSWork->GetAppCountryRegionNameUtility();
    if( pAppCountryRegionNameUtility )
    {
      // 国名
      {
        pG2DUtil->SetTextBoxPaneString( pTextBox_01, pAppCountryRegionNameUtility->GetCountryName( gtsIndexData.Country ) );
      }

      // 地域名
      {
        pG2DUtil->SetTextBoxPaneString( pTextBox_02, pAppCountryRegionNameUtility->GetRegionName( gtsIndexData.Country, gtsIndexData.Region ) );
      }
    }
    else
    {
      pG2DUtil->SetTextBoxPaneString( pTextBox_01, &m_NullString );
      pG2DUtil->SetTextBoxPaneString( pTextBox_02, &m_NullString );
    }

#endif
  }

  if( m_pTradePersonSelectDrawListener )
  {
    m_pTradePersonSelectDrawListener->UpdateView( &gtsIndexData );
  }
}


void GTSTradePersonSelectLowerView::ReUpdate()
{
  setPokemonInfo( m_SelectIndex );
}


void GTSTradePersonSelectLowerView::SetCursorVisible( bool bVisible )
{
  setCursorVisible( bVisible, bVisible );
}


void GTSTradePersonSelectLowerView::setCursorVisible( bool bLeft, bool bRight )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

  gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

  gfl2::lyt::LytParts* pCursorL = pLower->GetPartsPane( PANENAME_GTS_USR_LOW_000_PANE_CUR_L );
  gfl2::lyt::LytParts* pCursorR = pLower->GetPartsPane( PANENAME_GTS_USR_LOW_000_PANE_CUR_R );

  pCursorL->SetVisible( bLeft );
  pCursorR->SetVisible( bRight );
}


void GTSTradePersonSelectLowerView::Update()
{
  NetAppLib::System::NetApplicationViewBase::Update();
  m_MenuCursor.Update();
}


void GTSTradePersonSelectLowerView::Draw( gfl2::gfx::CtrDisplayNo displayNo )
{
  app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();
  app::util::AppRenderingManager* pAppRenderingManager = m_pGTSWork->GetAppRenderingManager();
  pG2DUtil->AddDrawableLytWkForOneFrame( pAppRenderingManager, displayNo, LAYOUT_WORK_ID_LOWER );
  m_MenuCursor.Draw( displayNo );
}


void GTSTradePersonSelectLowerView::OnAddChild( void )
{
  setInputEnablePaneList( false );
}


void GTSTradePersonSelectLowerView::OnRemoveChild( void )
{
  setInputEnablePaneList( true );
}


void GTSTradePersonSelectLowerView::setInputEnablePaneList( bool bInputEnable )
{
  m_bPaneListInputEnable = bInputEnable;

  if( m_pNewPaneList )
  {
    m_pNewPaneList->SetInputEnabled( bInputEnable );
  }
}


::app::ui::UIInputListener::ListenerResult GTSTradePersonSelectLowerView::OnLayoutPaneEvent( const u32 painId )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  switch( painId )
  {
    //『戻る』ボタン
    case BUTTON_ID_BTN_BACK:
    {
      if( m_pTradePersonSelectDrawListener )
      {
        m_pTradePersonSelectDrawListener->TouchBackButton();
      }
      result = DISABLE_ACCESS;
    }
    break;

    //『絞込み検索』ボタン
    case BUTTON_ID_BTN_SEARCH:
    {
      if( m_bRefineSearch )
      {
        NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();
        refineSearchPokemonResultList.Erase();

        setVisibleRefineSearchButton( false );
        createPaneList( &m_pGTSWork->GetSearchPokemonResultList(), true );
        setPokemonInfo( 0 );
      }
      else
      {
        if( m_pTradePersonSelectDrawListener )
        {
          m_pTradePersonSelectDrawListener->TouchRefineSearchButton();
        }
      }
      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_CURSOR_L:
    {
      if( m_pTradePersonSelectDrawListener )
      {
        m_pTradePersonSelectDrawListener->SearchPokemonPrevious();
      }

      result = DISABLE_ACCESS;
    }
    break;

    case BUTTON_ID_CURSOR_R:
    {
      if( m_pTradePersonSelectDrawListener )
      {
        m_pTradePersonSelectDrawListener->SearchPokemonNext();
      }

      result = DISABLE_ACCESS;
    }
    break;
  }

  return result;
}


u32 GTSTradePersonSelectLowerView::InputCustom()
{
  u32 inputEventID = NetAppLib::System::NetAppViewListenerBase::INPUT_EVENT_ID_NONE;

  if( IsInputEnabled() && !IsButtonAnimation() && m_pTradePersonSelectDrawListener && !m_bRefineSearch )
  {
    gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();

    gfl2::ui::VectorDevice* pCrossKey = pDeviceManager->GetCrossKey( gfl2::ui::DeviceManager::CROSSKEY_STICK_EMU );

    if( m_pSearchResultList )
    {
      u32 listSize = m_pSearchResultList->Size();
      u32 listLastIndex = ( listSize > 0 ) ? listSize - 1 : 0;

      if( pCrossKey->IsTrigger( gfl2::ui::DIR_LEFT ) && m_SelectIndex == 0 )
      {
        inputEventID = NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener::INPUT_EVENT_ID_SEARCH_POKEMON_PREVIOUS;
      }
      else
      if( pCrossKey->IsTrigger( gfl2::ui::DIR_RIGHT ) && m_SelectIndex >= listLastIndex && listSize >= 100 )
      {
        inputEventID = NetApp::GTS::TradePersonSelect::GTSTradePersonSelectLowerViewListener::INPUT_EVENT_ID_SEARCH_POKEMON_NEXT;
      }
    }
  }

  return inputEventID;
}


::app::ui::UIInputListener::ListenerResult GTSTradePersonSelectLowerView::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  ::app::ui::UIInputListener::ListenerResult result = ENABLE_ACCESS;

  u32 listSize = ( m_pSearchResultList ) ? m_pSearchResultList->Size() : 0;
  u32 listLastIndex = ( listSize > 0 ) ? listSize - 1 : 0;

  if( IsButtonAnimation() || IsPaneListAction() )
  {
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_B ) )
  {
    m_button_manager->StartSelectedAct( BUTTON_ID_BTN_BACK );
    result = DISABLE_ACCESS;
  }
  else
  if( pButton->IsTrigger( gfl2::ui::BUTTON_Y ) )
  {
    if( m_bRefineSearch )
    {
      NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();
      refineSearchPokemonResultList.Erase();

      setVisibleRefineSearchButton( false );
      createPaneList( &m_pGTSWork->GetSearchPokemonResultList(), true );
      setPokemonInfo( 0 );
    }
    else
    {
      m_button_manager->StartSelectedAct( BUTTON_ID_BTN_SEARCH );
    }
    result = DISABLE_ACCESS;
  }

  return result;
}


void GTSTradePersonSelectLowerView::ResetView( bool bBegin )
{
  NetApp::GTS::ApplicationSystem::GtsSearchResultList& searchPokemonResultList = m_pGTSWork->GetSearchPokemonResultList();
  NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();
  
  if( 0 < refineSearchPokemonResultList.Size() && refineSearchPokemonResultList.Size() < searchPokemonResultList.Size() )
  {
    setVisibleRefineSearchButton( true );
    createPaneList( &refineSearchPokemonResultList, true );
    setPokemonInfo( 0 );
  }
  else
  {
    u32 index = 0;
    if( searchPokemonResultList.Size() > 0 )
    {
      index = ( bBegin ) ? 0 : searchPokemonResultList.Size() - 1;
    }

    setVisibleRefineSearchButton( false );
    createPaneList( &searchPokemonResultList, bBegin );
    setPokemonInfo( index );
  }

}


bool GTSTradePersonSelectLowerView::IsPaneListAction()
{
  bool bIsAction = false;

  if( m_pNewPaneList )
  {
    bIsAction = m_pNewPaneList->IsModuleAction();
  }

  return bIsAction;
}


void GTSTradePersonSelectLowerView::setVisibleRefineSearchButton( bool bVisible )
{
  m_bRefineSearch = bVisible;

  if( bVisible )
  {
    GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_SEARCH_OFF );
  }
  else
  {
    GetG2DUtil()->StartAnime( LAYOUT_WORK_ID_LOWER, LA_GTS_GTS_USR_LOW_000__BTN_SEARCH_SEARCH_ON );
  }
}


NetApp::GTS::GtsSearchResultInfo* GTSTradePersonSelectLowerView::getSearchResultInfo( u32 index )
{
  if( m_pSearchResultList )
  {
    if( m_pSearchResultList->Size() > 0 )
    {
      u32 count = 0;
      for( NetApp::GTS::ApplicationSystem::GtsSearchResultIterator it = m_pSearchResultList->Begin(); it != m_pSearchResultList->End(); ++it )
      {
        if( index == count )
        {
          return &(*it);
        }
        ++count;
      }
    }
  }
  return NULL;
}


void GTSTradePersonSelectLowerView::PaneListListener_Write( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  NetApp::GTS::GtsSearchResultInfo* pSearchResultInfo = getSearchResultInfo( pos );

  if( pSearchResultInfo )
  {
    app::util::G2DUtil* pG2DUtil = UIView::GetG2DUtil();

    gfl2::lyt::LytWk* pLower = pG2DUtil->GetLayoutWork( LAYOUT_WORK_ID_LOWER );

    gfl2::lyt::LytPicture* pPicturePane     = pLower->GetPicturePane( PicturePaneTable[ pane_index ] );
    gfl2::lyt::LytPane*    pWindowFramePane = pLower->GetPane( WindowFramePaneTable[ pane_index ] );
    
    if( pPicturePane )
    {
      NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility = m_pGTSWork->GetTrainerIconUtility();

      pane[ pane_index ].base->SetScale( nw::math::VEC2( 1.0f, 1.0f ) );
      pPicturePane->SetVisible( true );

      app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA iconObjectData;

      iconObjectData.ConvFromMyStatusIconData( pSearchResultInfo->indexData.iconData );
      
      iconObjectData.pReplacePane             = pPicturePane;
      iconObjectData.pSyncVisiblePane         = pane[ pane_index ].base;
      iconObjectData.pInLoadVisibleChangePane = pWindowFramePane;
      iconObjectData.pTimerIconRootPane       = pane[ pane_index ].base;

      pTrainerIconUtility->SetIconObjectData( pane_index, iconObjectData );
    }
  }
}


void GTSTradePersonSelectLowerView::PaneListListener_MoveCursor( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
  setPokemonInfo( pos );
}


void GTSTradePersonSelectLowerView::PaneListListener_Decide( u32 pos )
{
  if( IsButtonAnimation() )
  {
    return;
  }

  if( !m_bPaneListInputEnable )
  {
    return;
  }

  if( m_pTradePersonSelectDrawListener )
  {
    m_pTradePersonSelectDrawListener->TradePersonSelect( getSearchResultInfo( m_SelectIndex ) );
  }

}


void GTSTradePersonSelectLowerView::PaneListListener_StartItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos )
{
}


void GTSTradePersonSelectLowerView::PaneListListener_EndItemChange( const App::Tool::NewPaneList::PANE_DATA * pane, u32 pane_index, u32 pos1, u32 pos2 )
{
}


GFL_NAMESPACE_END(TradePersonSelect)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
