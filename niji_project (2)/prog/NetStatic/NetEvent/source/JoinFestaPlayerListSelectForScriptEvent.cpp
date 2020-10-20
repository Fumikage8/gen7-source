
// ============================================================================
/*
 * @file JoinFestaPlayerListSelectForScriptEvent.cpp
 * @brief プレイヤー単体選択アプリ(スクリプトから起動)のイベント
 * @date 2015.10.26
 * @author endo_akira
 */
// ============================================================================
#include "../include/JoinFestaPlayerListSelectForScriptEvent.h"
#include "NetApp/JoinFestaPlayerListSelectForScript/include/JoinFestaPlayerListSelectForScriptProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/DllProc.h"
#include "System/include/HeapDefine.h"

//　ホームスリープ制御
#include "System/include/HomeSleepAndPowerSystem.h"
#include "NetStatic/NetLib/include/NijiNetworkSystem.h" // NijiNetworkSystem

#include <Fade/include/gfl2_FadeManager.h>
#include <Field/FieldStatic/include/BattleFes/BattleFesTool.h>

GFL_NAMESPACE_BEGIN(NetEvent)
GFL_NAMESPACE_BEGIN(JoinFestaPlayerListSelectForScript)





/*  prohibited unnamed namespace  */
#define FILELOCAL namespace_JoinFestaPlayerListSelectForScriptEvent
namespace FILELOCAL {
  enum  Sequence
  {
    SEQ_RunningProc,        /**<  通常実行            */
    /*  pokestatus  */
    SEQ_FadeBeforePokeStatus, /**<  PokeStatus呼出し前  */
    SEQ_WaitForPokeStatus,    /**<  PokeStatus待ち      */
    SEQ_FadeAfterPokeStatus,  /**<  PokeStatus呼出し後  */
    
    SEQ_Exit
  };
}





void JoinFestaPlayerListSelectForScriptEvent::StartEvent( GameSys::GameManager* pGameManager, const NetApp::JoinFestaPlayerListSelectForScript::APP_PARAM& appParam, NetApp::JoinFestaPlayerListSelectForScript::APP_RESULT* pAppResult )
{
  GameSys::GameEventManager* pGameEventManager = pGameManager->GetGameEventManager();
  JoinFestaPlayerListSelectForScriptEvent* pJoinFestaPlayerListSelectForScriptEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, JoinFestaPlayerListSelectForScriptEvent>( pGameEventManager );
  pJoinFestaPlayerListSelectForScriptEvent->SetAppParam( appParam );
  pJoinFestaPlayerListSelectForScriptEvent->SetAppResult( pAppResult );
}


JoinFestaPlayerListSelectForScriptEvent::JoinFestaPlayerListSelectForScriptEvent( gfl2::heap::HeapBase* pHeap ) :
 NetEvent::NetEventBase( pHeap ),
 m_AppParam(),
 m_pAppResult( NULL ),
 m_pJoinFestaPlayerListSelectForScriptProc( NULL ),
 m_PokeStatusParam(),
 m_BFCTrainerInfo()
{
  if( NetLib::NijiNetworkSystem::GetNetworkMode() == NetLib::NijiNetworkSystem::NETWORK_MODE_WIFI )//これらはWifiのときだけHomeSleep禁止
  {
    System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
  }
}


JoinFestaPlayerListSelectForScriptEvent::~JoinFestaPlayerListSelectForScriptEvent()
{
  System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC );
}


bool JoinFestaPlayerListSelectForScriptEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


void JoinFestaPlayerListSelectForScriptEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  m_pJoinFestaPlayerListSelectForScriptProc = GameSys::CallJoinFestaPlayerListSelectForScriptProc( &m_AppParam );
  m_pJoinFestaPlayerListSelectForScriptProc->SetAppResult( m_pAppResult );
  m_pJoinFestaPlayerListSelectForScriptProc->SetBFCInfo( &m_BFCInfo );
  
  SetSeqNo(FILELOCAL::SEQ_RunningProc);
}


void JoinFestaPlayerListSelectForScriptEvent::EndFunc( GameSys::GameManager* pGameManager )
{
}


GameSys::GMEVENT_RESULT JoinFestaPlayerListSelectForScriptEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  const gfl2::Fade::DISP    fadeDisp      = gfl2::Fade::DISP_DOUBLE;
  GameSys::GMEVENT_RESULT   result        = GameSys::GMEVENT_RES_CONTINUE;


  switch(GetSeqNo())
  {
    case  FILELOCAL::SEQ_RunningProc:        /**<  通常実行            */
    {
      if( pGameManager->GetProcManager()->IsRegisterProcess( m_pJoinFestaPlayerListSelectForScriptProc ) )
      {
        /*  実行時  */
        /*
          for BattleFes
        */
        if(m_AppParam.bootMode == NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_BattleFesCircle)
        {
          if(m_BFCInfo.IsStatusTargetAvailable())
          {
            /*  fade  */
            pFadeManager->ForceDefaultOut(fadeDisp);
            SetSeqNo(FILELOCAL::SEQ_FadeBeforePokeStatus);
          }
        }
      }
      else
      {
        /*  終了時  */
        /*
          m_pAppResult->pSelectJoinFestaPersonalData は通常モード時にProc終了時にセットされている
        */
        /*
          for BattleFes
        */
        if(m_pAppResult->bIsSelectedJoinFestaPersonalData)
        {
          if(m_AppParam.bootMode == NetApp::JoinFestaPlayerListSelectForScript::BOOT_MODE_BattleFesCircle)
          {
            /*  bfcInfoからResult構築  */
            if(1 <= m_BFCInfo.dataNum) m_pAppResult->pSelectJoinFestaPersonalData  ->Copy(m_BFCInfo.selectedPlayerData[0]);
            if(2 <= m_BFCInfo.dataNum) m_pAppResult->pSelectJoinFestaPersonalData_2->Copy(m_BFCInfo.selectedPlayerData[1]);

            m_pAppResult->bIsSelectedJoinFestaPersonalData  = (0 < m_BFCInfo.dataNum);
          }
        }

        /*  exit  */
        SetSeqNo(FILELOCAL::SEQ_Exit);
        result = GameSys::GMEVENT_RES_FINISH;
      }
    }
      break;

    /*    */
    case  FILELOCAL::SEQ_FadeBeforePokeStatus:
    {
      if(pFadeManager->IsEnd(fadeDisp))
      {
        gfl2::heap::HeapBase*   pHeapBase = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
        const bool              isValid   = m_BFCTrainerInfo.Setup(m_BFCInfo.statusTargetPlayerData);

        GFL_ASSERT(isValid);

        if(isValid)
        {
          m_PokeStatusParam.partnerInfos[0].pTrainerInfo  = &m_BFCTrainerInfo;
          m_PokeStatusParam.partnerInfos[0].pTarget       = BattleFesTool::CreateBattleTreePokemon(pHeapBase, m_BFCInfo.statusTargetPlayerData);  /*  破棄が必要  */
          m_PokeStatusParam.bLoadRadarChartCRO            = false;

          /*  start pokestatus  */
          NetEvent::JoinFestaPokeStatus::JoinFestaPokeStatusEvent::StartEvent(GFL_SINGLETON_INSTANCE(GameSys::GameManager), &m_PokeStatusParam);
        }

        SetSeqNo(FILELOCAL::SEQ_WaitForPokeStatus);
      }
    }
      break;

    /*    */
    case  FILELOCAL::SEQ_WaitForPokeStatus:  /**<  PokeStatus  */
    {
      /*  fade  */
      pFadeManager->RequestIn(fadeDisp, gfl2::Fade::FADE_TYPE_ALPHA);

      SetSeqNo(FILELOCAL::SEQ_FadeAfterPokeStatus);
    }
      break;

    /*    */
    case  FILELOCAL::SEQ_FadeAfterPokeStatus:
    {
      if(pFadeManager->IsEnd(fadeDisp))
      {
        m_BFCInfo.ResetStatusTarget();  /**<  requestの無効化  */
        for(u32 index = 0; index < GFL_NELEMS(m_PokeStatusParam.partnerInfos); ++index)
        {
          GFL_SAFE_DELETE(m_PokeStatusParam.partnerInfos[index].pTarget);
        }
        SetSeqNo(FILELOCAL::SEQ_RunningProc);
      }
    }
      break;

    /*    */
    case  FILELOCAL::SEQ_Exit:
    default:
      result = GameSys::GMEVENT_RES_FINISH;
      break;
  }

  return result;
}


GFL_NAMESPACE_END(JoinFestaPlayerListSelectForScript)
GFL_NAMESPACE_END(NetEvent)
