// ============================================================================
/*
 * @file GTSTradePersonRefineSearchFrame.cpp
 * @brief 交換相手を絞り込むプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#include "NetApp/GTS/source/TradePersonRefineSearch/GTSTradePersonRefineSearchFrame.h"
#include "NetApp/GTS/source/TradePersonRefineSearch/GTSTradePersonRefineSearchLowerView.h"
#include "NetApp/GTS/source/SelectMenu/GTSTwoSelectMenuLowerView.h"
#include "NetApp/GTS/source/SelectMenu/GTSThreeSelectMenuLowerView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include <niji_conv_header/message/msg_gts.h>

#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "NetApp/GTS/source/GTSUtility.h"
#include "AppLib/include/Ui/UIViewManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(TradePersonRefineSearch)


static const int SEQUENCE_DEFAULT                = 0;
static const int SEQUENCE_REFINE_SEARCH_START    = 1;
static const int SEQUENCE_REFINE_SEARCH_WAIT     = 2;


GTSTradePersonRefineSearchFrame::GTSTradePersonRefineSearchFrame(
  NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
  NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects ),
  m_ePokemonCriteria( POKEMON_CRITERIA_NONE ),
  m_eAreaCriteria( AREA_CRITERIA_NONE ),
  m_GTSTradePersonRefineSearchLowerViewListener(),
  m_GTSTwoSelectMenuLowerViewListener(),
  m_GTSThreeSelectMenuLowerViewListener(),
  m_GTSMessageWindowLowerViewListener(),
  m_GtsSearchResultIterator(),
  m_Timer()
{
}


GTSTradePersonRefineSearchFrame::~GTSTradePersonRefineSearchFrame()
{
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSTradePersonRefineSearchFrame::startup()
{
  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(NULL);

  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );

  m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetRequestPokemonMessage( msg_gts_sel_30 );
  m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetAreaMessage( msg_gts_sel_30 );

  m_pGTSViewObjects->GetGTSTwoSelectMenuLowerView()->SetMessage( msg_gts_win_22, msg_gts_sel_30, msg_gts_sel_31 );
  m_pGTSViewObjects->GetGTSThreeSelectMenuLowerView()->SetMessage( msg_gts_win_23, msg_gts_sel_30, msg_gts_sel_33, msg_gts_sel_34 );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBlackFilter( true );

  NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();

  // 絞込み検索結果のリストを削除しておく
  refineSearchPokemonResultList.Erase();

  return true;
}


void GTSTradePersonRefineSearchFrame::setListener()
{
  m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetListener( &m_GTSTradePersonRefineSearchLowerViewListener );
  m_pGTSViewObjects->GetGTSTwoSelectMenuLowerView()->SetListener( &m_GTSTwoSelectMenuLowerViewListener );
  m_pGTSViewObjects->GetGTSThreeSelectMenuLowerView()->SetListener( &m_GTSThreeSelectMenuLowerViewListener );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetListener( &m_GTSMessageWindowLowerViewListener );

  m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetInputEnabled( true );
}


void GTSTradePersonRefineSearchFrame::removeListener()
{
  m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSTwoSelectMenuLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSThreeSelectMenuLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSTradePersonRefineSearchFrame::updateSequence()
{
  switch( GetSubSeq() )
  {
    case SEQUENCE_DEFAULT:
    {
      sequenceDefault();
    }
    break;

    case SEQUENCE_REFINE_SEARCH_START:
    {
      NetApp::GTS::ApplicationSystem::GtsSearchResultList& searchPokemonResultList = m_pGTSWork->GetSearchPokemonResultList();
      NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();

      refineSearchPokemonResultList.Erase();

      m_GtsSearchResultIterator = searchPokemonResultList.Begin();
      
      m_Timer.TimeOutStart();
      
      SetSubSeq( SEQUENCE_REFINE_SEARCH_WAIT );
    }
    break;

    case SEQUENCE_REFINE_SEARCH_WAIT:
    {
      NetApp::GTS::ApplicationSystem::GtsSearchResultList& searchPokemonResultList = m_pGTSWork->GetSearchPokemonResultList();
      NetApp::GTS::ApplicationSystem::GtsSearchResultList& refineSearchPokemonResultList = m_pGTSWork->GetRefineSearchPokemonResultList();

      if( m_GtsSearchResultIterator != searchPokemonResultList.End() )
      {
        Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();

        u16 myCountry = static_cast<u16>( pMyStatus->GetCountry() );

        bool bPokemonCriteria = false;
        bool bAreaCriteria = false;

        if( m_ePokemonCriteria == POKEMON_CRITERIA_MY_BOX )
        {
          if( findRequestPokemonInMyBox( m_GtsSearchResultIterator->indexData ) )
          {
            bPokemonCriteria = true;
          }
        }
        else
        {
          bPokemonCriteria = true;
        }

        if( m_eAreaCriteria == AREA_CRITERIA_SAME_AREA )
        {
          if( myCountry == m_GtsSearchResultIterator->indexData.Country )
          {
            bAreaCriteria = true;
          }
        }
        else
        if( m_eAreaCriteria == AREA_CRITERIA_DIFFERENT_AREA )
        {
          if( myCountry != m_GtsSearchResultIterator->indexData.Country )
          {
            bAreaCriteria = true;
          }
        }
        else
        {
          bAreaCriteria = true;
        }

        if( bPokemonCriteria && bAreaCriteria )
        {
          refineSearchPokemonResultList.PushBack( *m_GtsSearchResultIterator );
        }

        ++m_GtsSearchResultIterator;
      }
      else
      {
        if( m_Timer.IsTimeOut() )
        {
          if( refineSearchPokemonResultList.Size() == 0 )
          {
            m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_13 ); //『この条件では見つかりません』

            SetSubSeq( SEQUENCE_DEFAULT );
          }
          else
          {
            removeLowerView();
            exitFrame( NetApp::GTS::TRADE_PERSON_REFINE_SEARCH_RESULT_BACK );
          }
        }
      }
    }
    break;
  }

  m_GTSTradePersonRefineSearchLowerViewListener.ResetInputEvent();
  m_GTSTwoSelectMenuLowerViewListener.ResetInputEvent();
  m_GTSThreeSelectMenuLowerViewListener.ResetInputEvent();
  m_GTSMessageWindowLowerViewListener.ResetInputEvent();

}


void GTSTradePersonRefineSearchFrame::sequenceDefault()
{
  switch( m_GTSTradePersonRefineSearchLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      exitFrame( NetApp::GTS::TRADE_PERSON_REFINE_SEARCH_RESULT_BACK );
    }
    break;

    case NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerViewListener::INPUT_EVENT_ID_TOUCH_REQUEST_POKEMON_BUTTON:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTwoSelectMenuLowerView() );
    }
    break;

    case NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerViewListener::INPUT_EVENT_ID_TOUCH_AREA_BUTTON:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSThreeSelectMenuLowerView() );
    }
    break;

    case NetApp::GTS::TradePersonRefineSearch::GTSTradePersonRefineSearchLowerViewListener::INPUT_EVENT_ID_TOUCH_REFINE_SEARCH_BUTTON:
    {
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );

      m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_25 );

      SetSubSeq( SEQUENCE_REFINE_SEARCH_START );
    }
    break;
  }

  switch( m_GTSTwoSelectMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );
    }
    break;

    case NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_1:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );
      m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetRequestPokemonMessage( msg_gts_sel_30 );
      m_ePokemonCriteria = POKEMON_CRITERIA_NONE;
    }
    break;

    case NetApp::GTS::SelectMenu::GTSTwoSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_2:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );
      m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetRequestPokemonMessage( msg_gts_sel_31 );
      m_ePokemonCriteria = POKEMON_CRITERIA_MY_BOX;
    }
    break;
  }

  switch( m_GTSThreeSelectMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );
    }
    break;

    case NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_1:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );
      m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetAreaMessage( msg_gts_sel_30 );
      m_eAreaCriteria = AREA_CRITERIA_NONE;
    }
    break;

    case NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_2:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );
      m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetAreaMessage( msg_gts_sel_33 );
      m_eAreaCriteria = AREA_CRITERIA_SAME_AREA;
    }
    break;

    case NetApp::GTS::SelectMenu::GTSThreeSelectMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_SELECT_BUTTON_3:
    {
      removeLowerView();
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView() );
      m_pGTSViewObjects->GetGTSTradePersonRefineSearchLowerView()->SetAreaMessage( msg_gts_sel_34 );
      m_eAreaCriteria = AREA_CRITERIA_DIFFERENT_AREA;
    }
    break;
  }

  switch( m_GTSMessageWindowLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_13: //『この条件では見つかりません』
        {
          removeLowerView();
        }
        break;
      }
    }
    break;
  }

}


bool GTSTradePersonRefineSearchFrame::findRequestPokemonInMyBox( const NetApp::GTS::GtsIndexData& gtsIndexData )
{
  bool bFind = false;

  if( gtsIndexData.PokeInfo.requestGender != NetApp::GTS::GENDER_UNKNOWN && gtsIndexData.PokeInfo.requestLevelBand != NetApp::GTS::LEVEL_BAND_NONE )
  {
    bFind = findPokemonCheckGenderAndLevelBand( gtsIndexData.PokeInfo.requestKind, gtsIndexData.PokeInfo.requestGender, gtsIndexData.PokeInfo.requestLevelBand );
  }
  else
  if( gtsIndexData.PokeInfo.requestGender != NetApp::GTS::GENDER_UNKNOWN )
  {
    bFind = findPokemonCheckGender( gtsIndexData.PokeInfo.requestKind, gtsIndexData.PokeInfo.requestGender );
  }
  else
  if( gtsIndexData.PokeInfo.requestLevelBand != NetApp::GTS::LEVEL_BAND_NONE )
  {
    bFind = findPokemonCheckLevelBand( gtsIndexData.PokeInfo.requestKind, gtsIndexData.PokeInfo.requestLevelBand );
  }
  else
  {
    bFind = findPokemon( gtsIndexData.PokeInfo.requestKind );
  }

  return bFind;
}


bool GTSTradePersonRefineSearchFrame::findPokemonCheckGenderAndLevelBand( u16 requestKind, u8 requestGender, u8 requestLevelBand )
{
  pml::Sex sex = NetApp::GTS::GTSUtility::ConvertGenderToPmlSex( requestGender );

  // 手持ち
  {
    pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

    u32 memberCount = pPokeParty->GetMemberCount();
    
    for( u32 i = 0; i < memberCount; ++i )
    {
      pml::pokepara::PokemonParam* pPokemonParam = pPokeParty->GetMemberPointer(i);

      if( comparePokemonCheckGenderAndLevelBand( *pPokemonParam, requestKind, sex, requestLevelBand ) )
      {
        return true;
      }
    }
  }

  // Box
  {
    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

    pml::pokepara::CoreParam coreParam( m_pGTSWork->GetDeviceHeap() );

    for( u32 i = 0; i < Savedata::BoxPokemon::TRAY_MAX; ++i )
    {
      for( u32 j = 0; j < Savedata::BoxPokemon::TRAY_POKE_MAX; ++j )
      {
        pBoxPokemon->GetPokemon( &coreParam, i, j );

        if( comparePokemonCheckGenderAndLevelBand( coreParam, requestKind, sex, requestLevelBand ) )
        {
          return true;
        }
      }
    }
  }

  return false;
}


bool GTSTradePersonRefineSearchFrame::findPokemonCheckGender( u16 requestKind, u8 requestGender )
{
  pml::Sex sex = NetApp::GTS::GTSUtility::ConvertGenderToPmlSex( requestGender );

  // 手持ち
  {
    pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

    u32 memberCount = pPokeParty->GetMemberCount();
    
    for( u32 i = 0; i < memberCount; ++i )
    {
      pml::pokepara::PokemonParam* pPokemonParam = pPokeParty->GetMemberPointer(i);

      if( comparePokemonCheckGender( *pPokemonParam, requestKind, sex ) )
      {
        return true;
      }
    }
  }

  // Box
  {
    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

    pml::pokepara::CoreParam coreParam( m_pGTSWork->GetDeviceHeap() );

    for( u32 i = 0; i < Savedata::BoxPokemon::TRAY_MAX; ++i )
    {
      for( u32 j = 0; j < Savedata::BoxPokemon::TRAY_POKE_MAX; ++j )
      {
        pBoxPokemon->GetPokemon( &coreParam, i, j );

        if( comparePokemonCheckGender( coreParam, requestKind, sex ) )
        {
          return true;
        }
      }
    }
  }

  return false;
}


bool GTSTradePersonRefineSearchFrame::findPokemonCheckLevelBand( u16 requestKind, u8 requestLevelBand )
{
  // 手持ち
  {
    pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

    u32 memberCount = pPokeParty->GetMemberCount();
    
    for( u32 i = 0; i < memberCount; ++i )
    {
      pml::pokepara::PokemonParam* pPokemonParam = pPokeParty->GetMemberPointer(i);

      if( comparePokemonCheckLevelBand( *pPokemonParam, requestKind, requestLevelBand ) )
      {
        return true;
      }
    }
  }

  // Box
  {
    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

    pml::pokepara::CoreParam coreParam( m_pGTSWork->GetDeviceHeap() );

    for( u32 i = 0; i < Savedata::BoxPokemon::TRAY_MAX; ++i )
    {
      for( u32 j = 0; j < Savedata::BoxPokemon::TRAY_POKE_MAX; ++j )
      {
        pBoxPokemon->GetPokemon( &coreParam, i, j );

        if( comparePokemonCheckLevelBand( coreParam, requestKind, requestLevelBand ) )
        {
          return true;
        }
      }
    }
  }

  return false;
}


bool GTSTradePersonRefineSearchFrame::findPokemon( u16 requestKind )
{
  // 手持ち
  {
    pml::PokeParty* pPokeParty = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();

    u32 memberCount = pPokeParty->GetMemberCount();
    
    for( u32 i = 0; i < memberCount; ++i )
    {
      pml::pokepara::PokemonParam* pPokemonParam = pPokeParty->GetMemberPointer(i);

      if( comparePokemon( *pPokemonParam, requestKind ) )
      {
        return true;
      }
    }
  }

  // Box
  {
    Savedata::BoxPokemon* pBoxPokemon = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

    pml::pokepara::CoreParam coreParam( m_pGTSWork->GetDeviceHeap() );

    for( u32 i = 0; i < Savedata::BoxPokemon::TRAY_MAX; ++i )
    {
      for( u32 j = 0; j < Savedata::BoxPokemon::TRAY_POKE_MAX; ++j )
      {
        pBoxPokemon->GetPokemon( &coreParam, i, j );

        if( comparePokemon( coreParam, requestKind ) )
        {
          return true;
        }
      }
    }
  }

  return false;
}


bool GTSTradePersonRefineSearchFrame::comparePokemonCheckGenderAndLevelBand( pml::pokepara::CoreParam& coreParam, u16 requestKind,  pml::Sex sex, u8 requestLevelBand )
{
  if( coreParam.GetMonsNo() == requestKind )
  {
    if( coreParam.GetSex() == sex )
    {
      u8 levelBand = NetApp::GTS::GTSUtility::ConvertLevelToLevelBand( coreParam.GetLevel() );
      
      if( levelBand == requestLevelBand )
      {
        return true;
      }
    }
  }
  return false;
}


bool GTSTradePersonRefineSearchFrame::comparePokemonCheckGender( pml::pokepara::CoreParam& coreParam, u16 requestKind,  pml::Sex sex )
{
  if( coreParam.GetMonsNo() == requestKind )
  {
    if( coreParam.GetSex() == sex )
    {
      return true;
    }
  }
  return false;
}


bool GTSTradePersonRefineSearchFrame::comparePokemonCheckLevelBand( pml::pokepara::CoreParam& coreParam, u16 requestKind, u8 requestLevelBand )
{
  if( coreParam.GetMonsNo() == requestKind )
  {
    u8 levelBand = NetApp::GTS::GTSUtility::ConvertLevelToLevelBand( coreParam.GetLevel() );
      
    if( levelBand == requestLevelBand )
    {
      return true;
    }
  }
  return false;
}


bool GTSTradePersonRefineSearchFrame::comparePokemon( pml::pokepara::CoreParam& coreParam, u16 requestKind )
{
  if( coreParam.GetMonsNo() == requestKind )
  {
    return true;
  }
  return false;
}


GFL_NAMESPACE_END(TradePersonRefineSearch)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
