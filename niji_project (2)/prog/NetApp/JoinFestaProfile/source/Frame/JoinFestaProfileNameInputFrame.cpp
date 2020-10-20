// ============================================================================
/*
 * @file JoinFestaProfileNameInputFrame.cpp
 * @brief 
 * @date 2015.11.05
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaProfile/source/Frame/JoinFestaProfileNameInputFrame.h"
#include "NetApp/JoinFestaProfile/source/JoinFestaProfileFrameResult.h"

#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGUpperView.h"
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListBGLowerView.h"

#include "AppLib/include/Frame/AppFrameManager.h"
#include "App/StrInput/include/StrInputFrame.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/PokemonVersion.h"
#include "System/include/PokemonLanguage.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaProfile)
GFL_NAMESPACE_BEGIN(Frame)


JoinFestaProfileNameInputFrame::JoinFestaProfileNameInputFrame(
  NetApp::JoinFestaProfile::System::JoinFestaProfileWork* pJoinFestaProfileWork,
  NetApp::JoinFestaProfile::System::JoinFestaProfileViewSystemAccessor* pJoinFestaProfileViewSystemAccessor ) :
  NetApp::JoinFestaProfile::Frame::JoinFestaProfileFrameBase( pJoinFestaProfileWork, pJoinFestaProfileViewSystemAccessor ),
  m_StrInputAppParam()
{
}


JoinFestaProfileNameInputFrame::~JoinFestaProfileNameInputFrame()
{
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool JoinFestaProfileNameInputFrame::startup()
{
  m_pJoinFestaProfileViewObjects->InputDisableAll();
  m_pJoinFestaProfileViewList->RemoveAllView();

  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

  u32 inputType = App::StrInput::TYPE_POKEMON_NAME;

  gfl2::str::StrBuf* pInpuStrBuf = NULL;

  NetApp::JoinFestaProfile::System::EditMenuInfo* pSelectEditMenu = m_pJoinFestaProfileWork->GetSelectEditMenu();
  switch( pSelectEditMenu->editMenuID )
  {
#if defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )
    // 好きなポケモン(6文字)
    case EDIT_MENU_ID_LIKE_POKEMON:
    {
      inputType = App::StrInput::TYPE_POKE_SEARCH;
      pInpuStrBuf = &m_pJoinFestaProfileWork->GetLikePokemonName();
    }
    break;
    
    // 肩書き(8文字)
    case EDIT_MENU_ID_HONOR:
    {
      inputType = App::StrInput::TYPE_INTERVIEW_SHORT;
      pInpuStrBuf = &m_pJoinFestaProfileWork->GetHonorString();
    }
    break;
#endif // defined( JOINFESTAPROFILEDEFINE_LIKE_POKEMON_HONOR_ENABLE )

    // ジョインフェスタの名前(8文字)
    case EDIT_MENU_ID_NAME:
    {
      inputType = App::StrInput::TYPE_FESCIRCLE_NAME;
      pInpuStrBuf = &m_pJoinFestaProfileWork->GetJoinFestaName();
    }
    break;
  }

  m_StrInputAppParam.mpGameProcManager     = pGameManager->GetProcManager();
  m_StrInputAppParam.pSaveMisc             = pGameManager->GetGameData()->GetMisc();
  m_StrInputAppParam.type                  = inputType;
  m_StrInputAppParam.param                 = 0;
  m_StrInputAppParam.cp                    = NULL;
  m_StrInputAppParam.isFieldSubProc        = false;

  m_StrInputAppParam.language              = ::System::GetLang();
  m_StrInputAppParam.strBuf                = pInpuStrBuf;
  m_StrInputAppParam.strCode               = NULL;

  m_StrInputAppParam.changeFlag            = false;
  m_StrInputAppParam.isInputCancel         = false;

  m_StrInputAppParam.skipInitGraphicMemory = true;

  m_StrInputAppParam.mpHeap                = m_pJoinFestaProfileWork->GetAppHeap();
  m_StrInputAppParam.mpAppRenderingManager = m_pJoinFestaProfileWork->GetAppRenderingManager();

  App::StrInput::StrInputFrame* pStrInputFrame = GFL_NEW( m_pJoinFestaProfileWork->GetDeviceHeap() ) App::StrInput::StrInputFrame();
  pStrInputFrame->Setup( &m_StrInputAppParam );

  GetMyManager()->CallProc( pStrInputFrame );

  return true;
}


void JoinFestaProfileNameInputFrame::setListener()
{
}


void JoinFestaProfileNameInputFrame::removeListener()
{
}


void JoinFestaProfileNameInputFrame::updateSequence()
{
  if( m_StrInputAppParam.changeFlag )
  {
    Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();

    pJoinFestaDataSave->SetJoinFestaNameString( m_StrInputAppParam.strBuf );
  }

  m_pJoinFestaProfileWork->FadeInRequest();

  exitFrame( NetApp::JoinFestaProfile::NAME_INPUT_FRAME_RESULT_BACK );
}


GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(JoinFestaProfile)
GFL_NAMESPACE_END(NetApp)
