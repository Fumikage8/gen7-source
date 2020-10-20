// ============================================================================
/*
 * @file GTSUploadPokemonFrame.cpp
 * @brief ポケモンを預けるプロセスです。
 * @date 2015.04.08
 */
// ============================================================================
#include "NetApp/GTS/source/UploadPokemon/GTSUploadPokemonFrame.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadPokemonLowerView.h"
#include "NetApp/GTS/source/UploadPokemon/GTSUploadMessageSelectLowerView.h"
#include "NetApp/GTS/source/ScrollMenu/GTSScrollMenuLowerView.h"
#include "NetApp/GTS/source/Window/GTSMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSWarningMessageWindowLowerView.h"
#include "NetApp/GTS/source/Window/GTSSmallMessageWindowUpperView.h"
#include "NetApp/GTS/source/RequestSequence/GTSRequestFacade.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetApp/GTS/source/ApplicationSystem/GTSViewObjects.h"
#include "NetApp/GTS/source/GTSUtility.h"
#include "NetApp/GTS/source/GTSPokeNameSearch.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "App/Box/source/BoxUppPokeStatusDraw.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <niji_conv_header/message/msg_gts.h>
#include <pml/include/personal/pml_PersonalSystem.h>


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(UploadPokemon)


GTSUploadPokemonFrame::GTSUploadPokemonFrame(
  NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork,
  NetApp::GTS::ApplicationSystem::GTSViewObjects* pGTSViewObjects ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_pGTSViewObjects( pGTSViewObjects ),
  m_GTSUploadPokemonLowerViewListener(),
  m_GTSUploadMessageSelectLowerViewListener(),
  m_GTSScrollMenuLowerViewListener(),
  m_GTSMessageWindowLowerViewListener(),
  m_Timer(),
  m_bEnableSelectGenderMenu( true ),
  m_eRequestState( REQUEST_STATE_NULL )
{
}


GTSUploadPokemonFrame::~GTSUploadPokemonFrame()
{
  if( m_pGTSWork->GetFrameResult() == NetApp::GTS::UPLOAD_POKEMON_RESULT_NAME_INPUT )
  {
    removeUpperView();
    removeLowerView();
  }
  m_pGTSWork->GetInputName().Clear();
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSUploadPokemonFrame::startup()
{
  m_pGTSViewObjects->InputDisableAll();
  m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
  m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

  addUpperViewInputDisable( m_pGTSViewObjects->GetBoxPokeStatusUpperView() );
  addLowerViewInputDisable( m_pGTSViewObjects->GetGTSUploadPokemonLowerView() );

  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBackButton( false );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetVisibleBlackFilter( true );


  // 上画面のセットアップ
  {
    pml::pokepara::PokemonParam pokemonParam( m_pGTSWork->GetDeviceHeap(), m_pGTSWork->GetSelectPokemonCoreParam() );
    m_pGTSViewObjects->GetBoxPokeStatusUpperView()->SetParam( pokemonParam, false, false );
    m_pGTSViewObjects->GetBoxPokeStatusUpperView()->SetDrawMode( App::Box::BoxUppPokeStatusDraw::DRAW_MODE_STATUS );
    m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Open();
  }

  // 下画面のセットアップ
  {
    gfl2::str::StrBuf& inputName = m_pGTSWork->GetInputName();
    if( inputName.GetLength() > 0 )
    {
      NetApp::GTS::GtsPokeNameSearch pokeNameSearch( m_pGTSWork->GetAppHeap(), GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData() );
      u32 monsNo = pokeNameSearch.CheckString2PokeNo( &inputName, m_pGTSWork );
      m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetPokemonName( inputName );
      m_pGTSWork->SetRequestPokemonMonsNo( monsNo );
      m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
    }
    else
    {
      m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetPokemon( m_pGTSWork->GetRequestPokemonMonsNo() );
    }
    m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetGender( NetApp::GTS::GTSUtility::GetGenderMessageID( (u8)m_pGTSWork->GetRequestPokemonGender() ) );
    m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetLevelBand( NetApp::GTS::GTSUtility::GetLevelBandMessageID( (u8)m_pGTSWork->GetRequestPokemonLevelBand() ) );
    m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetMessage( m_pGTSWork->GetRequestMessageID() );
  }

  if( m_pGTSWork->GetInputName().GetLength() == 0 && m_pGTSWork->GetRequestPokemonMonsNo() == MONSNO_NULL )
  {
    m_pGTSWork->SetNameInputFlag( false );
  }

  if( m_pGTSWork->GetNameInputFlag() == false && m_pGTSWork->GetRequestPokemonMonsNo() != MONSNO_NULL )
  {
    pml::personal::LoadPersonalData( static_cast<MonsNo>( m_pGTSWork->GetRequestPokemonMonsNo() ), 0 );

    switch( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ) )
    {
      case pml::personal::SEX_VECTOR_UNKNOWN:
      {
        m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
        m_bEnableSelectGenderMenu = false;
      }
      break;

      case pml::personal::SEX_VECTOR_ONLY_MALE:
      {
        m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_MALE );
        m_bEnableSelectGenderMenu = false;
      }
      break;

      case pml::personal::SEX_VECTOR_ONLY_FEMALE:
      {
        m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_FMALE );
        m_bEnableSelectGenderMenu = false;
      }
      break;
    }

    m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetGender( NetApp::GTS::GTSUtility::GetGenderMessageID( (u8)m_pGTSWork->GetRequestPokemonGender() ) );
  }

  m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetGenderButtonSE( m_bEnableSelectGenderMenu );

  return true;
}


void GTSUploadPokemonFrame::setListener()
{
  m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetListener( &m_GTSUploadPokemonLowerViewListener );
  m_pGTSViewObjects->GetGTSUploadMessageSelectLowerView()->SetListener( &m_GTSUploadMessageSelectLowerViewListener );
  m_pGTSViewObjects->GetGTSScrollMenuLowerView()->SetListener( &m_GTSScrollMenuLowerViewListener );
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetListener( &m_GTSMessageWindowLowerViewListener );

  m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetInputEnabled( true );
}


void GTSUploadPokemonFrame::removeListener()
{
  m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSUploadMessageSelectLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSScrollMenuLowerView()->RemoveListener();
  m_pGTSViewObjects->GetGTSMessageWindowLowerView()->RemoveListener();

  m_pGTSViewObjects->InputDisableAll();
}


void GTSUploadPokemonFrame::updateSequence()
{
  switch( m_eRequestState )
  {
    case REQUEST_STATE_NULL:
    {
      defaultSequence();
    }
    break;

    case REQUEST_STATE_COMPLETE_SUCCESS:
    {
      if( m_Timer.IsTimeOut() )
      {
        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_DEMO_UPLOAD );

        exitFrame( NetApp::GTS::UPLOAD_POKEMON_RESULT_SUCCESS );
      }
    }
    break;

    case REQUEST_STATE_COMPLETE_ERROR:
    {
      if( m_Timer.IsTimeOut() )
      {
        removeLowerView();

        m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_BOX );

        exitFrame( NetApp::GTS::UPLOAD_POKEMON_RESULT_ERROR );
      }
    }
    break;
  }

  m_GTSUploadPokemonLowerViewListener.ResetInputEvent();
  m_GTSUploadMessageSelectLowerViewListener.ResetInputEvent();
  m_GTSScrollMenuLowerViewListener.ResetInputEvent();
  m_GTSMessageWindowLowerViewListener.ResetInputEvent();
}


void GTSUploadPokemonFrame::defaultSequence()
{
  switch( m_GTSUploadPokemonLowerViewListener.GetInputEventID() )
  {
    // --------------------------------------------------------------------------------------------
    // 戻るボタン
    case NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_BOX );

      exitFrame( NetApp::GTS::UPLOAD_POKEMON_RESULT_BACK );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 欲しいポケモンボタン
    case NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_POKEMON_BUTTON:
    {
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSScrollMenuLowerView() );
      m_pGTSViewObjects->GetGTSScrollMenuLowerView()->ChangePokemonSelectMenu();

      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Close();
      removeUpperView();

      addUpperViewInputEnable( m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView() );
      m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView()->SetMessage( msg_gts_win_37 );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 性別ボタン
    case NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_GENDER_BUTTON:
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
      }
      else
      {
        removeLowerView();

        addLowerViewInputEnable( m_pGTSViewObjects->GetGTSScrollMenuLowerView() );
        m_pGTSViewObjects->GetGTSScrollMenuLowerView()->ChangeGenderSelectMenu();

        m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Close();
        removeUpperView();

        addUpperViewInputEnable( m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView() );
        m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView()->SetMessage( msg_gts_win_10 );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    // レベルボタン
    case NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_LEVEL_BUTTON:
    {
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSScrollMenuLowerView() );
      m_pGTSViewObjects->GetGTSScrollMenuLowerView()->ChangeLevelBandSelectMenu();

      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Close();
      removeUpperView();

      addUpperViewInputEnable( m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView() );
      m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView()->SetMessage( msg_gts_win_11 );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 定型文のメッセージ(挨拶メッセージ)ボタン
    case NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_MESSAGE_BUTTON:
    {
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSUploadMessageSelectLowerView() );

      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Close();
      removeUpperView();

      addUpperViewInputEnable( m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView() );
      m_pGTSViewObjects->GetGTSSmallMessageWindowUpperView()->SetMessage( msg_gts_win_33 );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 預けるボタン
    case NetApp::GTS::UploadPokemon::GTSUploadPokemonLowerViewListener::INPUT_EVENT_ID_TOUCH_UPLOAD_BUTTON:
    {
      if( m_pGTSWork->GetRequestPokemonMonsNo() != MONSNO_NULL
       || m_pGTSWork->GetInputName().GetLength() > 0 )
      {
        addLowerViewInputDisable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetMessage( msg_gts_win_01 );
        NetApp::GTS::RequestSequence::GTSRequestFacade::AddUploadPokemonVerifyRequest( this, m_pGTSWork );
      }
      else
      {
        addLowerViewInputEnable( m_pGTSViewObjects->GetGTSMessageWindowLowerView() );
        m_pGTSViewObjects->GetGTSMessageWindowLowerView()->SetStreamMessage( msg_gts_win_24 ); //『欲しいポケモンを決めないとGTSに預けられません』
      }
    }
    break;
  }

  switch( m_GTSUploadMessageSelectLowerViewListener.GetInputEventID() )
  {
    // --------------------------------------------------------------------------------------------
    // 戻るボタン
    case NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSUploadPokemonLowerView() );

      addUpperViewInputDisable( m_pGTSViewObjects->GetBoxPokeStatusUpperView() );
      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Open();
    }
    break;

    // --------------------------------------------------------------------------------------------
    // メッセージ選択
    case NetApp::GTS::UploadPokemon::GTSUploadMessageSelectLowerViewListener::INPUT_EVENT_ID_SELECT_MESSAGE:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSUploadPokemonLowerView() );

      addUpperViewInputDisable( m_pGTSViewObjects->GetBoxPokeStatusUpperView() );
      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Open();

      u32 messageID = m_GTSUploadMessageSelectLowerViewListener.GetSelectMessageID();

      m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetMessage( messageID );
      m_pGTSWork->SetRequestMessageID( messageID );
    }
    break;
  }

  switch( m_GTSScrollMenuLowerViewListener.GetInputEventID() )
  {
    // --------------------------------------------------------------------------------------------
    // 戻るボタン
    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_BACK_BUTTON:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSUploadPokemonLowerView() );

      addUpperViewInputDisable( m_pGTSViewObjects->GetBoxPokeStatusUpperView() );
      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Open();
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 欲しいポケモンを選択
    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_SELECT_POKEMON:
    {
      u32 monsNoOld = m_pGTSWork->GetRequestPokemonMonsNo();

      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSUploadPokemonLowerView() );

      addUpperViewInputDisable( m_pGTSViewObjects->GetBoxPokeStatusUpperView() );
      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Open();

      m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetPokemon( m_GTSScrollMenuLowerViewListener.GetMessageID() );
      m_pGTSWork->SetRequestPokemonMonsNo( m_GTSScrollMenuLowerViewListener.GetSelectMonsNo() );
      m_pGTSWork->GetInputName().Clear();
      m_pGTSWork->SetNameInputFlag( false );

      u32 monsNoNew = m_pGTSWork->GetRequestPokemonMonsNo();

      if( monsNoOld != monsNoNew )
      {
        pml::personal::LoadPersonalData( static_cast<MonsNo>( monsNoNew ), 0 );

        switch( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_sex ) )
        {
          case pml::personal::SEX_VECTOR_UNKNOWN:
          {
            m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
            m_bEnableSelectGenderMenu = false;
          }
          break;

          case pml::personal::SEX_VECTOR_ONLY_MALE:
          {
            m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_MALE );
            m_bEnableSelectGenderMenu = false;
          }
          break;

          case pml::personal::SEX_VECTOR_ONLY_FEMALE:
          {
            m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_FMALE );
            m_bEnableSelectGenderMenu = false;
          }
          break;

          default:
          {
            m_pGTSWork->SetRequestPokemonGender( NetApp::GTS::GENDER_UNKNOWN );
            m_bEnableSelectGenderMenu = true;
          }
          break;
        }

        m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetGenderButtonSE( m_bEnableSelectGenderMenu );
        m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetGender( NetApp::GTS::GTSUtility::GetGenderMessageID( (u8)m_pGTSWork->GetRequestPokemonGender() ) );
      }
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 性別を選択
    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_SELECT_GENDER:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSUploadPokemonLowerView() );

      addUpperViewInputDisable( m_pGTSViewObjects->GetBoxPokeStatusUpperView() );
      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Open();

      m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetGender( m_GTSScrollMenuLowerViewListener.GetMessageID() );
      m_pGTSWork->SetRequestPokemonGender( m_GTSScrollMenuLowerViewListener.GetSelectGender() );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // レベルを選択を選択
    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_SELECT_LEVEL_BAND:
    {
      removeUpperView();
      removeLowerView();

      addLowerViewInputEnable( m_pGTSViewObjects->GetGTSUploadPokemonLowerView() );

      addUpperViewInputDisable( m_pGTSViewObjects->GetBoxPokeStatusUpperView() );
      m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Open();

      m_pGTSViewObjects->GetGTSUploadPokemonLowerView()->SetLevelBand( m_GTSScrollMenuLowerViewListener.GetMessageID() );
      m_pGTSWork->SetRequestPokemonLevelBand( m_GTSScrollMenuLowerViewListener.GetSelectLevelBand() );
    }
    break;

    // --------------------------------------------------------------------------------------------
    // 名前入力ボタンを選択
    case NetApp::GTS::ScrollMenu::GTSScrollMenuLowerViewListener::INPUT_EVENT_ID_TOUCH_INPUT_BUTTON:
    {
      m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_NAME_INPUT );
      exitFrame( NetApp::GTS::UPLOAD_POKEMON_RESULT_NAME_INPUT );
    }
    break;
  }

  switch( m_GTSMessageWindowLowerViewListener.GetInputEventID() )
  {
    case NetApp::GTS::Window::GTSMessageWindowLowerViewListener::INPUT_EVENT_ID_TOUCH_SCREEN:
    {
      switch( m_GTSMessageWindowLowerViewListener.GetMessageID() )
      {
        case msg_gts_win_24: // 『欲しいポケモンを決めないとGTSに預けられません』
        case msg_gts_win_20: // 『名前を入力した時は性別を選ぶ事は出来ません』
        {
          removeLowerView();
        }
        break;
      }
    }
    break;
  }

}


//-----------------------------------------------------------------------------
// GTSUploadPokemonVerifyResponseListener
//-----------------------------------------------------------------------------
void GTSUploadPokemonFrame::OnUploadPokemonVerifySuccess()
{
  m_pGTSViewObjects->GetBoxPokeStatusUpperView()->Close();

  removeAllView();

  addLowerViewInputEnable( m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView() );

  m_pGTSViewObjects->GetGTSWarningMessageWindowLowerView()->SetMessage( msg_gts_win_34 );

  NetApp::GTS::RequestSequence::GTSRequestFacade::AddUploadPokemonRequest( this, m_pGTSWork );

  m_eRequestState = REQUEST_STATE_EXECUTE;

  m_Timer.TimeOutStart( NetApp::GTS::GTS_WARNING_MESSAGE_WAIT_TIME_MILLI_SECOND );
}


void GTSUploadPokemonFrame::OnUploadPokemonVerifyError()
{
  removeLowerView();

  m_pGTSWork->SetBootProcID( NetApp::GTS::ApplicationSystem::GTSWork::BOOT_PROC_ID_BOX );

  exitFrame( NetApp::GTS::UPLOAD_POKEMON_RESULT_ERROR );
}


//-----------------------------------------------------------------------------
// GTSUploadPokemonResponseListener
//-----------------------------------------------------------------------------
void GTSUploadPokemonFrame::OnUploadPokemonSuccess()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_SUCCESS;
}


void GTSUploadPokemonFrame::OnUploadPokemonError()
{
  m_eRequestState = REQUEST_STATE_COMPLETE_ERROR;
}


GFL_NAMESPACE_END(UploadPokemon)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
