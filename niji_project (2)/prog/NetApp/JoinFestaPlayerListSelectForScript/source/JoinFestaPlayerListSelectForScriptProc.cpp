// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptProc.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のプロッククラスです。
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "NetApp/JoinFestaPlayerListSelectForScript/include/JoinFestaPlayerListSelectForScriptProc.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptBuilder.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptApplicationSystem.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/source/System/JoinFestaPlayerListSelectForScriptWork.h"

#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)


JoinFestaPlayerListSelectForScriptProc::JoinFestaPlayerListSelectForScriptProc( NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM* pAppParam ) :
  m_AppParam( *pAppParam ),
  m_pAppResult( NULL ),
  m_pJoinFestaPlayerListSelectForScriptWork( NULL ),
  m_pApplicationSystem( NULL ),
  m_pBFCModeInfo(NULL)
{
}


JoinFestaPlayerListSelectForScriptProc::~JoinFestaPlayerListSelectForScriptProc()
{
}


gfl2::proc::Result JoinFestaPlayerListSelectForScriptProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pJoinFestaPlayerListSelectForScriptWork = GFL_NEW( pDevHeap ) NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptWork();
      m_pJoinFestaPlayerListSelectForScriptWork->SetBootMode( static_cast<NetApp::JoinFestaPlayerListSelectForScript::BootMode>( m_AppParam.bootMode ) );
      m_pJoinFestaPlayerListSelectForScriptWork->SetListSize( m_AppParam.listSize );
      m_pJoinFestaPlayerListSelectForScriptWork->SetBFCModeInfo(m_pBFCModeInfo);

      app::util::Heap* pAppHeap = m_pJoinFestaPlayerListSelectForScriptWork->GetAppHeap();
      NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptBuilder netAppSkeletonBuilder( m_pJoinFestaPlayerListSelectForScriptWork, m_AppParam );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::JoinFestaPlayerListSelectForScript::System::JoinFestaPlayerListSelectForScriptApplicationSystem();
      m_pApplicationSystem->Build( m_pJoinFestaPlayerListSelectForScriptWork, &netAppSkeletonBuilder );
      SetSubSeq( INIT_SEQUENCE_RESOURCE_LOADING );
    }
    break;

    case INIT_SEQUENCE_RESOURCE_LOADING:
    {
      if( !m_pApplicationSystem->Loading() )
      {
        result = gfl2::proc::RES_FINISH;
      }
    }
    break;
  }

  return result;
}


gfl2::proc::Result JoinFestaPlayerListSelectForScriptProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    if( m_pAppResult )
    {
      if( m_pAppResult->pSelectJoinFestaPersonalData )
      {
        NetAppLib::JoinFestaUI::JoinFestaPlayerData* pSelectPlayer = m_pJoinFestaPlayerListSelectForScriptWork->GetSelectPlayer();
        if( pSelectPlayer && !m_pJoinFestaPlayerListSelectForScriptWork->IsSelectCancel() )
        {
          m_pAppResult->pSelectJoinFestaPersonalData->Copy( *pSelectPlayer );
          m_pAppResult->bIsSelectedJoinFestaPersonalData = true;
        }
        else
        {
          m_pAppResult->bIsSelectedJoinFestaPersonalData = false;
        }
      }
    }

    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pJoinFestaPlayerListSelectForScriptWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result JoinFestaPlayerListSelectForScriptProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void JoinFestaPlayerListSelectForScriptProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void JoinFestaPlayerListSelectForScriptProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetApp)
