// ============================================================================
/*
 * @file GTSNameInputFrame.cpp
 * @brief GTSの名前入力のプロセスです。
 * @date 2015.07.03
 */
// ============================================================================
#include "NetApp/GTS/source/NameInput/GTSNameInputFrame.h"
#include "NetApp/GTS/source/GTSFrameResult.h"
#include "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"
#include "AppLib/include/Frame/AppFrameManager.h"
#include "App/StrInput/include/StrInputFrame.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/PokemonLanguage.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(GTS)
GFL_NAMESPACE_BEGIN(NameInput)


GTSNameInputFrame::GTSNameInputFrame( NetApp::GTS::ApplicationSystem::GTSWork* pGTSWork ) :
  NetApp::GTS::GTSFrameBase( pGTSWork ),
  m_pGTSWork( pGTSWork ),
  m_StrInputAppParam()
{
}


GTSNameInputFrame::~GTSNameInputFrame()
{
}


//-----------------------------------------------------------------------------
// GTSFrameBase
//-----------------------------------------------------------------------------
bool GTSNameInputFrame::startup()
{
  bool result = false;

  NetAppLib::Util::NetAppTrainerIconUtility* pTrainerIconUtility = m_pGTSWork->GetTrainerIconUtility();

  bool isDelete = pTrainerIconUtility->IsDelete();

  if( isDelete )
  {
    m_pGTSWork->GetRootUpperView()->RemoveAllSubView();
    m_pGTSWork->GetRootLowerView()->RemoveAllSubView();

    GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager );

    m_StrInputAppParam.mpGameProcManager     = pGameManager->GetProcManager();
    m_StrInputAppParam.pSaveMisc             = pGameManager->GetGameData()->GetMisc();
    m_StrInputAppParam.type                  = App::StrInput::TYPE_POKE_WANT;
    m_StrInputAppParam.param                 = 0;
    m_StrInputAppParam.cp                    = NULL;
    m_StrInputAppParam.isFieldSubProc        = false;

    m_StrInputAppParam.language              = System::GetLang();
    m_StrInputAppParam.strBuf                = &m_pGTSWork->GetInputName();
    m_StrInputAppParam.strCode               = NULL;

    m_StrInputAppParam.changeFlag            = false;
    m_StrInputAppParam.isInputCancel         = false;

    m_StrInputAppParam.skipInitGraphicMemory = true;

    m_StrInputAppParam.mpHeap                = m_pGTSWork->GetAppHeap();
    m_StrInputAppParam.mpAppRenderingManager = m_pGTSWork->GetAppRenderingManager();

    App::StrInput::StrInputFrame* pStrInputFrame = GFL_NEW( m_pGTSWork->GetAppHeap()->GetDeviceHeap() ) App::StrInput::StrInputFrame();
    pStrInputFrame->Setup( &m_StrInputAppParam );

    GetMyManager()->CallProc( pStrInputFrame );

    result = true;
  }

  return result;
}


void GTSNameInputFrame::setListener()
{
}


void GTSNameInputFrame::removeListener()
{
}


void GTSNameInputFrame::updateSequence()
{
  if( m_StrInputAppParam.changeFlag || m_StrInputAppParam.isInputCancel )
  {
    m_pGTSWork->FadeInRequest();

    NetAppLib::System::FrameResult frameResult = NetApp::GTS::NAME_INPUT_RESULT_TO_SEARCH_POKEMON;

    if( m_pGTSWork->GetNameInputBootFrame() == NetApp::GTS::ApplicationSystem::GTSWork::NAME_INPUT_BOOT_FRAME_UPLOAD_POKEMON )
    {
      frameResult = NetApp::GTS::NAME_INPUT_RESULT_TO_UPLOAD_POKEMON;
    }

    exitFrame( frameResult );
  }
}


GFL_NAMESPACE_END(NameInput)
GFL_NAMESPACE_END(GTS)
GFL_NAMESPACE_END(NetApp)
