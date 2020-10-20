// ============================================================================
/*
 * @file GTSSearchPokemonFrame.cpp
 * @brief 探したいポケモンを選択するプロセスです。
 * @date 2015.03.18
 */
// ============================================================================
#include "NetApp/GTS/source/SearchPokemon/GTSSearchPokemonFrame.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchPokemonLowerView.h"
#include "NetApp/GTS/source/SearchPokemon/GTSSearchOptionLowerView.h"
#include "NetApp/GTS/source/ScrollMenu/GTSScrollMenuLowerView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSSmallMessageWindowUpperView.h"
#include "NetApp/GTS/source/RequestSequence/GTSRequestFacade.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSPokeNameSearch.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <niji_conv_header/message/msg_gts.h>
#include <pml/include/personal/pml_PersonalSystem.h>

#include "AppLib/include/Ui/UIViewManager.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(SearchPokemon)


GTSSearchPokemonFrame::GTSSearchPokemonFrame(
NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects ),
  m_SearchPokemonRequestHandle( gflnet2::ServerClient::INVALID_HANDLE ),
  m_GTSScrollMenuLowerViewListener(),
  m_GTSSearchPokemonLowerViewListener(),
  m_GTSSearchOptionLowerViewListener(),
  m_GTSMessageWindowLowerViewListener(),
  m_bEnableSelectGenderMenu( true )
{
}


GTSSearchPokemonFrame::~GTSSearchPokemonFrame()
{
  if( m_pGTSWork->GetFrameResult() == NetApp::GTS::SEARCH_POKEMON_RESULT_NAME_INPUT )
  {
    removeUpperView();
    removeLowerView();
  }
  m_pGTSWork->GetInputName().Clear();
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSSearchPokemonFrame::startup()
{
  GFL_SINGLETON_INSTANCE( app::ui::UIViewManager )->SetTopPriorityUIView(NULL);

  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSSearchPokemonLowerView() );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBlackFilter( true );

  gfl2::str::StrBuf& inputName = m_pGTSWork->GetInputName();
  if( inputName.GetLength() > 0 )
  {
    NetApp::GTS::GtsPokeNameSearch pokeNameSearch( m_pGTSWork->GetAppHeap(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData() );
    u32 monsNo = pokeNameSearch.CheckString2PokeNo( &inputName, m_pGTSWork );
    m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetPokemonName( inputName );
    m_pGTSWork->SetSearchPokemonMonsNo( monsNo );
    m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
  }
  else
  {
    m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetPokemon( m_pGTSWork->GetSearchPokemonMonsNo() );
  }
  m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetGender( NetApp::GTS::GTSUtility::GetGenderMessageID( (u8)m_pGTSWork->GetSearchPokemonGender() ) );
  m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetLevelBand( NetApp::GTS::GTSUtility::GetLevelBandMessageID( (u8)m_pGTSWork->GetSearchPokemonLevelBand() ) );

  if( m_pGTSWork->GetInputName().GetLength() == 0 && m_pGTSWork->GetSearchPokemonMonsNo() == MONSNO_NULL )
  {
    m_pGTSWork->SetNameInputFlag( false );
  }

  if( m_pGTSWork->GetNameInputFlag() == false && m_pGTSWork->GetSearchPokemonMonsNo() != MONSNO_NULL )
  {
    pml::personal::LoadPersonalData( static_cast<MonsNo>( m_pGTSWork->GetSearchPokemonMonsNo() ), 0 );

    switch( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ) )
    {
      case pml::personal::SEX_VECTOR_UNKNOWN:
      {
        m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
        m_bEnableSelectGenderMenu = false;
      }
      break;

      case pml::personal::SEX_VECTOR_ONLY_MALE:
      {
        m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_MALE );
        m_bEnableSelectGenderMenu = false;
      }
      break;

      case pml::personal::SEX_VECTOR_ONLY_FEMALE:
      {
        m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_FMALE );
        m_bEnableSelectGenderMenu = false;
      }
      break;
    }

    m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetGender( NetApp::GTS::GTSUtility::GetGenderMessageID( (u8)m_pGTSWork->GetSearchPokemonGender() ) );
  }

  m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetGenderButtonSE( m_bEnableSelectGenderMenu );

  return true;
}


void GTSSearchPokemonFrame::setListener()
{
  m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetListener( &m_GTSSearchPokemonLowerViewListener );
  m_pGTSViewObjects->GetGTSSearchOptionLowerView()->SetListener( &m_GTSSearchOptionLowerViewListener );
  m_pGTSViewObjects->GetGTSScrollMenuLowerView()->SetListener( &m_GTSScrollMenuLowerViewListener );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetListener( &m_GTSMessageWindowLowerViewListener );

  m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetInputEnabled( true );
}


void GTSSearchPokemonFrame::removeListener()
{
  m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSSearchOptionLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSScrollMenuLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSSearchPokemonFrame::updateSequence()
{
  switch( m_GTSScrollMenuLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSSearchPokemonLowerView() );
    }
    break;

    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_SELECT_POKEMON:
    {
      u32 monsNoOld = m_pGTSWork->GetSearchPokemonMonsNo();

      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSSearchPokemonLowerView() );
  
      m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetPokemon( m_GTSScrollMenuLowerViewListener.GetMessageID() );
      m_pGTSWork->SetSearchPokemonMonsNo( m_GTSScrollMenuLowerViewListener.GetSelectMonsNo() );
      m_pGTSWork->GetInputName().Clear();
      m_pGTSWork->SetNameInputFlag( false );

      u32 monsNoNew = m_pGTSWork->GetSearchPokemonMonsNo();

      if( monsNoOld != monsNoNew )
      {
        pml::personal::LoadPersonalData( static_cast<MonsNo>( monsNoNew ), 0 );

        switch( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ) )
        {
          case pml::personal::SEX_VECTOR_UNKNOWN:
          {
            m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
            m_bEnableSelectGenderMenu = false;
          }
          break;

          case pml::personal::SEX_VECTOR_ONLY_MALE:
          {
            m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_MALE );
            m_bEnableSelectGenderMenu = false;
          }
          break;

          case pml::personal::SEX_VECTOR_ONLY_FEMALE:
          {
            m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_FMALE );
            m_bEnableSelectGenderMenu = false;
          }
          break;

          default:
          {
            m_pGTSWork->SetSearchPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
            m_bEnableSelectGenderMenu = true;
          }
          break;
        }

        m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetGenderButtonSE( m_bEnableSelectGenderMenu );
        m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetGender( NetApp::GTS::GTSUtility::GetGenderMessageID( (u8)m_pGTSWork->GetSearchPokemonGender() ) );
      }
    }
    break;

    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_SELECT_GENDER:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSSearchPokemonLowerView() );

      m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetGender( m_GTSScrollMenuLowerViewListener.GetMessageID() );
      m_pGTSWork->SetSearchPokemonGender( m_GTSScrollMenuLowerViewListener.GetSelectGender() );
    }
    break;

    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_SELECT_LEVEL_BAND:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSSearchPokemonLowerView() );

      m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->SetLevelBand( m_GTSScrollMenuLowerViewListener.GetMessageID() );
      m_pGTSWork->SetSearchPokemonLevelBand( m_GTSScrollMenuLowerViewListener.GetSelectLevelBand() );
    }
    break;

    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_INPUT_BUTTON:
    {
      m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_NAME_INPUT );
      exitFrame( NetApp::GTS::SEARCH_POKEMON_RESULT_NAME_INPUT );
    }
    break;
  }

  switch( m_GTSSearchPokemonLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      exitFrame( NetApp::GTS::SEARCH_POKEMON_RESULT_BACK );
    }
    break;

    case NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_POKEMON_BUTTON:
    {
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSScrollMenuLowerView() );
      m_pGTSViewObjects->GetGTSScrollMenuLowerView()->ChangePokemonSelectMenu();

      addUpperViewInputEnable( m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView() );
      m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView()->SetMessage( msg_gts_win_09 );
    }
    break;

    case NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_GENDER_BUTTON:
    {
      if( !m_bEnableSelectGenderMenu )
      {
        // ただ無反応にするだけなので何もしない
      }
      else
      if( m_pGTSWork->GetInputName().GetLength() > 0 || m_pGTSWork->GetNameInputFlag() )
      {
        addLowerViewInputEnable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_20 ); //『名前を入力した時は性別を選ぶ事は出来ません』
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
      }
      else
      {
        removeLowerView();

        addLowerViewInputEnable( m_pGTSViewObjects->GetGTSScrollMenuLowerView() );
        m_pGTSViewObjects->GetGTSScrollMenuLowerView()->ChangeGenderSelectMenu();

        addUpperViewInputEnable( m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView() );
        m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView()->SetMessage( msg_gts_win_10 );
      }
    }
    break;

    case NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_LEVEL_BUTTON:
    {
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSScrollMenuLowerView() );
      m_pGTSViewObjects->GetGTSScrollMenuLowerView()->ChangeLevelBandSelectMenu();

      addUpperViewInputEnable( m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView() );
      m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView()->SetMessage( msg_gts_win_11 );
    }
    break;

    case NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_SEARCH_BUTTON:
    {
      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );

      if( m_pGTSWork->GetSearchPokemonMonsNo() != MONSNO_NULL
       || m_pGTSWork->GetInputName().GetLength() > 0 )
      {
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_16 );//『ポケモンを探しています』
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false ); // [NCL要望]:検索のキャンセルは行わないで欲しい。サーバー負荷対策の為。
        m_pGTSWork->SetSearchRangeStartOffset( 0 );
        m_SearchPokemonRequestHandle = NetApp::GTS::RequestSequence::GTSRequestFacade::AddSearchPokemonRequest( this, m_pGTSWork );
      }
      else
      {
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_07 );//『欲しいポケモンを決めてください』
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
      }
    }
    break;

    case NetApp::GTS::SearchPokemon::GTSSearchPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_OPTION_BUTTON:
    {
      removeAllView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSSearchOptionLowerView() );
    }
    break;
  }

  switch( m_GTSSearchOptionLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::SearchPokemon::GTSSearchOptionLowerViewListener::INPUT_EVENT_ID_TOUCH_SEARCH_BUTTON:
    {
      removeAllView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSSearchPokemonLowerView() );

      m_pGTSWork->SetSearchPokemonLegend( SEARCH_LEGEND_OPTION_ON );
    }
    break;

    case NetApp::GTS::SearchPokemon::GTSSearchOptionLowerViewListener::INPUT_EVENT_ID_TOUCH_NOT_SEARCH_BUTTON:
    {
      removeAllView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSSearchPokemonLowerView() );

      m_pGTSWork->SetSearchPokemonLegend( SEARCH_LEGEND_OPTION_OFF );
    }
    break;
  }

  switch( m_GTSMessageWindowLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_07: //『欲しいポケモンを決めてください』
        case msg_gts_win_08: //『見つかりませんでした』
        case msg_gts_win_20: //『名前を入力した時は性別を選ぶ事は出来ません』
        {
          // [NCL要望]:検索した結果見つからなかった時に『欲しいポケモン』ボタンの位置にカーソルを戻す。サーバー負荷対策の為。
          if( m_GTSMessageWindowLowerViewListener.GetMessageID() == msg_gts_win_08 )
          {
            m_pGTSViewObjects->GetGTSSearchPokemonLowerView()->CursorMoveToIndex(0);
          }

          removeLowerView();
          m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
        }
        break;
      }
    }
    break;

    // [NCL要望]:検索のキャンセルは行わないで欲しい。サーバー負荷対策の為。
    /*
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_16: //『ポケモンを探しています』
        {
          if( m_SearchPokemonRequestHandle != gflnet2::ServerClient::INVALID_HANDLE )
          {
            NetApp::GTS::RequestSequence::GTSRequestFacade::CancelSearchPokemonRequest( m_SearchPokemonRequestHandle );
            m_SearchPokemonRequestHandle = gflnet2::ServerClient::INVALID_HANDLE;
          }

          m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_36 ); //『通信を中止しています。しばらくお待ちください。』
          m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetInputEnabled( false );
          m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
        }
        break;
      }
    }
    break;
    */
  }

  m_GTSScrollMenuLowerViewListener.ResetInputEvent();
  m_GTSSearchPokemonLowerViewListener.ResetInputEvent();
  m_GTSSearchOptionLowerViewListener.ResetInputEvent();
  m_GTSMessageWindowLowerViewListener.ResetInputEvent();

}


//-----------------------------------------------------------------------------
// GTSSearchPokemonResponseListener
//-----------------------------------------------------------------------------
void GTSSearchPokemonFrame::OnSearchPokemonSuccess()
{
  NetApp::GTS::ApplicationSystem::GtsSearchResultList& searchPokemonResultList = m_pGTSWork->GetSearchPokemonResultList();

  if( searchPokemonResultList.Size() == 0 )
  {
    m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_08 ); //『見つかりませんでした』
    m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  }
  else
  {
    removeLowerView();
    m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );

    exitFrame( NetApp::GTS::SEARCH_POKEMON_RESULT_SUCCESS );
  }

  m_SearchPokemonRequestHandle = gflnet2::ServerClient::INVALID_HANDLE;
}


void GTSSearchPokemonFrame::OnSearchPokemonCancel()
{
  removeLowerView();
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
}


void GTSSearchPokemonFrame::OnSearchPokemonError()
{
  m_SearchPokemonRequestHandle = gflnet2::ServerClient::INVALID_HANDLE;

  removeLowerView();
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );

  exitFrame( NetApp::GTS::SEARCH_POKEMON_RESULT_ERROR );
}


GFL_NAMESPACE_END(SearchPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
