
// ============================================================================
/**
  * @file   MysteryGiftEvent.cpp
  * @brief  ふしぎなおくりものイベント
  * @date   2015.05.12
  */
// ============================================================================


// ============================================================================
/*	includes	*/
// ============================================================================
#include  <pml/include/pmlib.h>

/*  sound */
#include  <Sound/include/Sound.h>

#include  "System/include/GflUse.h"
#include  "System/include/HeapDefine.h"
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameProcManager.h"
#include  "GameSys/include/DllProc.h"
#include  "App/AppDemo/include/AppDemoProcParam.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"

#if PM_DEBUG
  #include  "Debug/Launcher/include/LauncherProc.h"
  #include  "Debug/StartMenu/include/StartLauncher.h"
  #include  "Debug/DebugWin/include/DebugWinSystem.h"
#endif


/*  ホームスリープ制御  */
#include  "System/include/HomeSleepAndPowerSystem.h"


#include  <NetStatic/NetEvent/include/FatalErrorEvent.h>
#include  "../include/MysteryGiftEvent.h"


/*   mysterygift */
#include  "NetApp/MysteryGift/include/GiftAppParam.h"
#include  "NetApp/MysteryGift/include/GiftProc.h"
#include  "NetApp/MysteryGift/source/ApplicationSystem/GiftBuilder.h"
#include  "NetApp/MysteryGift/source/GiftFrameDef.h"

// ============================================================================
/**/
// ============================================================================
/*  local  */
namespace {
  static const u32  BGMID = STRM_BGM_WIFI_PRESENT;

  /**
    @brief  seqNo
  */
  enum  Sequence
  {
    /*  おくりもの  */
    SEQ_BootProc_MysteryGift,
    SEQ_ProcRunning_MysteryGift,

    /*  受信デモ  */
    SEQ_BootProc_ReceiveDemo,
    SEQ_ProcRunning_ReceiveDemo,

    /*  終了  */
    SEQ_End,
    SEQ_End_Eshop,

    /*  fatalerror  */
    SEQ_FatalError,

    SEQ_MAX,
    SEQ_DEFAULT = SEQ_BootProc_MysteryGift
  };
}


namespace NetEvent  {
namespace MysteryGift {
// ============================================================================
/**/
// ============================================================================



// ============================================================================
/*  funcs */
// ============================================================================
//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    MysteryGiftEventの開始
  */
//------------------------------------------------------------------
void MysteryGiftEvent::StartEvent(GameSys::GameManager* pGameManager)
{
  GAMESYSTEM_EVENT_CHANGE<GameSys::GameEventManager, MysteryGiftEvent>(pGameManager->GetGameEventManager());
}


//------------------------------------------------------------------
/**
  *  @func     MysteryGiftEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
MysteryGiftEvent::MysteryGiftEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent(pHeap)
  , m_pEventHeapBase(gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ))
  , m_AppParam()
  , m_pPokemonParamForReceiveDemo(NULL)
{
}



//------------------------------------------------------------------
/**
  *  @func     ~MysteryGiftEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
MysteryGiftEvent::~MysteryGiftEvent()
{
}


/*
    GameEvent I/F
*/

//------------------------------------------------------------------
/**
  *  @func    BootChk
  *  @brief   起動判定
  */
//------------------------------------------------------------------
bool  MysteryGiftEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  MysteryGiftEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  /*  home/sleep禁止  */
  System::HomeSleepAndPowerSystem::AddConditions(System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC);

  /*  BGM  */
  Sound::StartBGMReq(BGMID);

  SetSeqNo(SEQ_DEFAULT);
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  MysteryGiftEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_SAFE_DELETE(m_pPokemonParamForReceiveDemo);

  /*  home/sleep禁止解除  */
  System::HomeSleepAndPowerSystem::SubConditions(System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC);
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
#if 0
  enum GMEVENT_RESULT{
    GMEVENT_RES_CONTINUE = 0, ///<イベント継続中
    GMEVENT_RES_FINISH,       ///<イベント終了
    GMEVENT_RES_OFF,       ///<イベント終了中
    GMEVENT_RES_CONTINUE_DIRECT = 33,  ///<イベント継続（もう一周する）
  };
#endif
GameSys::GMEVENT_RESULT MysteryGiftEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result  = GameSys::GMEVENT_RES_CONTINUE;

  const int   seqNo = GetSeqNo();

  switch(seqNo)
  {
    case  SEQ_BootProc_MysteryGift:
      /*  bootProc  */
      GameSys::CallGiftProc( &m_AppParam );
      SetSeqNo(SEQ_ProcRunning_MysteryGift);
      break;

    /*  giftからの戻り  */
    case  SEQ_ProcRunning_MysteryGift:
      if(!pGameManager->IsProcessExists())
      {
        if(m_AppParam.procResult == NetApp::Gift::APP_PARAM::PROCRESULT_Exit)
        {
          /*  終了  */
          SetSeqNo(SEQ_End);
        }
        else if(m_AppParam.procResult == NetApp::Gift::APP_PARAM::PROCRESULT_Exit_Eshop)
        {
          /*  終了(eshop)  */
          SetSeqNo(SEQ_End_Eshop);
        }
        else if(m_AppParam.procResult == NetApp::Gift::APP_PARAM::PROCRESULT_ReceiveDemo)
        {
          /*  デモ  */
          SetSeqNo(SEQ_BootProc_ReceiveDemo);
        }
        else if(m_AppParam.procResult == NetApp::Gift::APP_PARAM::PROCRESULT_InternalNetworkErrorOccured)
        {
          /*  fatalEventへ遷移し、以降戻らない  */
          NetEvent::FatalError::EventParam eventParam;
          eventParam.eFatalErrorNo = NetEvent::FatalError::FATAL_ERROR_NO_NET_ERROR;
          NetEvent::FatalError::FatalErrorEvent::StartEvent( pGameManager, eventParam );
          SetSeqNo(SEQ_FatalError);
        }
        else
        {
          GFL_ASSERT(false);
        }
      }
      break;


    /*  受信デモ  */
    case  SEQ_BootProc_ReceiveDemo:
    {
      const Savedata::MysteryGiftSave::MYSTERY_GIFT_DATA  rGiftData     = *(m_AppParam.GetLastRemovedGiftData());
      App::AppDemo::AppDemoProcParam_Gift                 demoParam;



      /*  演出分岐  */
//      if((rGiftData.dataType == Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke) || (rGiftData.dataType == Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke_item))
      if(rGiftData.dataType == Savedata::MysteryGiftSave::MYSTERY_GIFT_TYPE_poke)
      {
        /*  ポケモン  */
        /*  ポケモン+item  */
        const Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*  pPokeData = reinterpret_cast<const Savedata::MysteryGiftSave::MYSTERY_GIFT_POKE_DATA*>(rGiftData.body);

        /*  demo用pokepara  */
        m_pPokemonParamForReceiveDemo = Savedata::MysteryGiftSave::CreatePokemonParamFromGiftData(pPokeData, m_pEventHeapBase, pPokeData->level);
        demoParam.pPokemonParams[0] = m_pPokemonParamForReceiveDemo;

        if(rGiftData.rarity == 2)       demoParam.type = App::AppDemo::AppDemoProcParam_Gift::TYPE_Pokemon_Gold;
        else if(rGiftData.rarity == 1)  demoParam.type = App::AppDemo::AppDemoProcParam_Gift::TYPE_Pokemon_Silver;
        else                            demoParam.type = App::AppDemo::AppDemoProcParam_Gift::TYPE_Pokemon_Bronze;
      }
      else
      {
        /*  others  */
        demoParam.pPokemonParams[0] = NULL;
        
        if(rGiftData.rarity == 2)       demoParam.type = App::AppDemo::AppDemoProcParam_Gift::TYPE_Item_Gold;
        else if(rGiftData.rarity == 1)  demoParam.type = App::AppDemo::AppDemoProcParam_Gift::TYPE_Item_Silver;
        else                            demoParam.type = App::AppDemo::AppDemoProcParam_Gift::TYPE_Item_Bronze;
      }

      /*  デモ呼び出し  */
//      Sound::PushBGMReq(); 
      GameSys::CallAppDemoProc_Gift(&demoParam);
      SetSeqNo(SEQ_ProcRunning_ReceiveDemo);
    }
      break;

    case  SEQ_ProcRunning_ReceiveDemo:
      if(!pGameManager->IsProcessExists())
      {
//        Sound::PopBGMReq(Sound::BGM_FADE_VERY_FAST);
        GFL_SAFE_DELETE(m_pPokemonParamForReceiveDemo);
        m_AppParam.bootMode  = NetApp::Gift::APP_PARAM::BOOTMODE_FromReceiveDemo;
        SetSeqNo(SEQ_BootProc_MysteryGift);
      }
      break;


    case  SEQ_FatalError:
      /*  nop  */
      break;


    case  SEQ_End:
      /*  home/sleep禁止解除  */
      System::HomeSleepAndPowerSystem::SubConditions(System::HomeSleepAndPowerSystem::HOME_SLEEP_NETAPP_ACC);
      System::GflUse::SetSoftResetRequest();    //!< ソフトウェアリセット
      break;

    case  SEQ_End_Eshop:
      System::GflUse::TransitToEshopRequest(true);
      break;

    default:
      break;
  }



  /*  ここまで到達したら終了  */
  return  result;
}


} /*  namespace MysteryGift */
} /*  namespace NetEvent      */

