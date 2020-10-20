// ============================================================================
/*
 * @file NBRProc.cpp
 * @brief 通信対戦受付アプリのプロッククラスです。
 * @date 2015.06.22
 * @author ichiraku_katsuhiko
 */
// ============================================================================
#include "NetApp/NBR/include/NBRProc.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRBuilder.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRApplicationSystem.h"
#include "NetApp/NBR/source/ApplicationSystem/NBRWork.h"
#include "System/include/HeapDefine.h"


GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(NBR)


NBRProc::NBRProc() :
  m_pAppParam( NULL ),
  m_pNBRWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


NBRProc::NBRProc( NetApp::NBR::APP_PARAM* pAppParam ) :
  m_pAppParam( pAppParam ),
  m_pNBRWork( NULL ),
  m_pApplicationSystem( NULL )
{
}


NBRProc::~NBRProc()
{
}


gfl2::proc::Result NBRProc::InitFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch( GetSubSeq() )
  {
    case INIT_SEQUENCE_BUILD_APPLICATION_SYSTEM:
    {
	    gfl2::heap::HeapBase *pDevHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

      m_pNBRWork = GFL_NEW( pDevHeap ) NetApp::NBR::ApplicationSystem::NBRWork();

      // Workにコピー
      m_pNBRWork->m_pRuleSelectorNetId        = &m_pAppParam->ruleSelectorNetId;
      m_pNBRWork->m_priority                  = m_pAppParam->priority;
      m_pNBRWork->m_hostNetID                 = m_pAppParam->hostNetID;
      m_pNBRWork->m_battleRule                = m_pAppParam->battleRule;
      m_pNBRWork->m_pSelectRegulationDrawInfo = m_pAppParam->pSelectRegulationDrawInfo;
      m_pNBRWork->m_isSelectedRegulation      = ( m_pAppParam->startupMode == STARTUP_RULE_SELECT_DECIDE) ? true : false;
      m_pNBRWork->m_pSelectBgmIndex           = &m_pAppParam->selectBgmIndex;

      // バトルチーム選択情報
      if( m_pAppParam->startupMode == STARTUP_TEAM_SELECT_AGAIN )
      {
        m_pNBRWork->m_isTeamSelectReturn = true;
        m_pNBRWork->m_teamSelectReturnMode = m_pAppParam->teamSelectReturnMode;
        m_pNBRWork->m_teamSelectIndex = m_pAppParam->teamSelectIndex;
      }
      else
      {// 最初からの場合は決定していない事とする
        m_pNBRWork->m_isTeamSelectReturn = false;
        m_pNBRWork->m_teamSelectReturnMode = TEAM_SELECT_RETURN_CANCEL;
      }

      // @fix GFNMcat[1359]：「マルチバトル」「バトルロイヤル」の「チーム&BGM選択アプリ」で「さんか　できません」を実行すると、はまる
      //　→原因は、NBR以外のアプリに行った時にルール選択者が初期化された為。
      //　→初期起動のみ処理するように変更
      if( m_pAppParam->startupMode == STARTUP_FIRST )
      {
        // ルール選択者の優先度初期データ
        m_pNBRWork->RuleSelectorPriorityCalc( m_pAppParam->pNet->GetConnectionNum() );
      }

      app::util::Heap* pAppHeap = m_pNBRWork->GetAppHeap();
      NetApp::NBR::ApplicationSystem::NBRBuilder netAppSkeletonBuilder( m_pAppParam, m_pNBRWork );
      m_pApplicationSystem = GFL_NEW( pAppHeap->GetDeviceHeap() ) NetApp::NBR::ApplicationSystem::NBRApplicationSystem();
      m_pApplicationSystem->Build( pAppHeap, &netAppSkeletonBuilder, m_pAppParam->pNet );
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


gfl2::proc::Result NBRProc::EndFunc( gfl2::proc::Manager* pProcManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  if( !m_pApplicationSystem->End() )
  {
    GFL_SAFE_DELETE( m_pApplicationSystem );

    GFL_SAFE_DELETE( m_pNBRWork );
    
    result = gfl2::proc::RES_FINISH;
  }
  return result;
}


gfl2::proc::Result NBRProc::UpdateFunc( gfl2::proc::Manager* pProcManager )
{
  bool isExitProc = m_pApplicationSystem->Update();
  if( isExitProc )
  {
    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}


void NBRProc::PreDrawFunc( gfl2::proc::Manager* pProcManager )
{
  m_pApplicationSystem->PreDraw();
}


void NBRProc::DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  m_pApplicationSystem->Draw( displayNo );
}


GFL_NAMESPACE_END(NBR)
GFL_NAMESPACE_END(NetApp)
