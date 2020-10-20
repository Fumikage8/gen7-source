//==============================================================================
/**
 * @file   EggHatchingEvent.cpp
 * @date   2015/11/24 Tue. 17:32:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <Sound/include/Sound.h>

#include  "System/include/HeapDefine.h"
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameProcManager.h"
#include  "GameSys/include/DllProc.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"

/*  zukanRegister  */
#include  "PokeTool/include/PokeToolPokeGet.h"

#include  "Savedata/include/Record.h"


/*   EggHatching */
#include  "NetApp/EggHatching/include/EggHatchingProcParam.h"
#include  "NetApp/EggHatching/include/EggHatchingProc.h"
#include  "NetApp/EggHatching/source/System/EggHatchingBuilder.h"
#include  "NetApp/EggHatching/source/EggHatchingFrameDef.h"

#include  "../include/EggHatchingEvent.h"




// =============================================================================
/**/
//==============================================================================
/*  local  */
namespace {
  /**
    @brief  seqNo
  */
  enum  Sequence
  {
    SEQ_ProcRunning_EggHatching,
    SEQ_AppRunning_Zukan,

    SEQ_Exit,

    SEQ_MAX,
  };
}


namespace NetEvent  {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    EggHatchingEventの開始
  */
//------------------------------------------------------------------
void EggHatchingEvent::StartEvent(GameSys::GameManager* pGameManager, EggHatchingEventParam& rEventParam)
{
  EggHatchingEvent*   pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EggHatchingEvent>( pGameManager->GetGameEventManager() );

  pEvent->m_EventParam  = rEventParam;
}


//------------------------------------------------------------------
/**
  *  @func     EggHatchingEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
EggHatchingEvent::EggHatchingEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent(pHeap)
  , m_ProcParam()
  , m_pProc(NULL)
  /*  zukan  */
  , m_ZukanParam()
  , m_zukanCheckerReturnType()
  /*  others  */
{
}



//------------------------------------------------------------------
/**
  *  @func     ~EggHatchingEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
EggHatchingEvent::~EggHatchingEvent()
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
bool  EggHatchingEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  EggHatchingEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  gfl2::heap::HeapBase*   pHeapBase = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);

  /*  setup appparam  */
  {
    /*
      GFNMcat[1775]： 「通信交換」で「色違い」のパラメータを持つ「タマゴ」を受け取った場合、孵化デモのときのみ色違いで表示される
      
      0) タマゴから付加するモンスターがレアとなるかどうかは、孵化した環境（セーブデータ）によって決定される。ある端末でレア色が孵化するタマゴデータを別端末に送ったとしても、送信先でレア色となるかとは限らない
      
      1) タマゴから孵化したモンスターがRareとなるかどうかには、環境（セーブデータ）固有のIDが影響し、IsRare()コールのたびに算出されている
      2) タマゴには、データが作られた環境での固有IDがセットされる
      3) 通信交換で受け取ったタマゴには、送信元の固有IDがセットされた状態である
      4) タマゴを孵化させ、PokeTool::PokeGetUtil::UpdatePokeAtPokeGet()に通すと、内部で行われるPokememoの処理で受信側の固有IDがセットされる
      5) 以降4)でセットされた受信側IDによってIsRare()が評価される
      
      以上のことから、手順4)の前のデータでデモを再生してしまうと、デモとその後のゲームとでRare状態が変わってしまうこととなる
      そのため、デモ再生前に手順4）を行い、その結果をデモで使用する必要がある
      
      # 以前はタマゴが交換に出ることはなかったとのこと
    */

    m_ProcParam.m_pTargetEgg    = GFL_NEW(pHeapBase)  pml::pokepara::PokemonParam(pHeapBase, *(m_EventParam.m_pTargetEgg));     /*  デモ用PokeParaをClone */
    GFL_PRINT("[1/2]EggHatchingEvent::InitFunc: targetEgg->IsRare(): %d\n", m_EventParam.m_pTargetEgg->IsRare());


    /*  ここで元データを実際に孵化させ、必要な処理を施しておく  */
    m_EventParam.m_pTargetEgg->Birth();

    /*  UpdatePokeAtPokeGet() で m_ProcParam.m_pTargetEgg の固有IDが変化する場合があるので、ここで行う   */
    {
      /*  図鑑関連  */
      m_ZukanParam.pp           = m_EventParam.m_pTargetEgg;                               //  pml::pokepara::PokemonParam*                  pp;
      m_ZukanParam.callerType   = (m_EventParam.m_inResort)?(PokeTool::PokeGetUtil::CallerType::TAMAGO_FUKA_RESORT):(PokeTool::PokeGetUtil::CallerType::TAMAGO_FUKA);   //  PokeTool::PokeGetUtil::CallerType::Tag  callerType;

      m_zukanCheckerReturnType  = PokeTool::PokeGetUtil::UpdatePokeAtPokeGet
                                  (
                                    pHeapBase,
                                    m_ZukanParam.pp,
                                    m_ZukanParam.callerType
                                  );
      GFL_PRINT("EggHatchingEvent::InitFunc: call PokeTool::PokeGetUtil::UpdatePokeAtPokeGet()\n");
    }
    
    
    /*  孵化後データが正常になったので、デモ用MonsをClone  */
    m_ProcParam.m_pTargetMons   = GFL_NEW(pHeapBase)  pml::pokepara::PokemonParam(pHeapBase, *(m_EventParam.m_pTargetEgg));     /*  デモ用PokeParaをClone */
    GFL_PRINT("[2/2]EggHatchingEvent::InitFunc: targetEgg->IsRare(): %d\n", m_EventParam.m_pTargetEgg->IsRare());
  }

  /*  BGM  */
  {
    Sound::StartAppBGMReq(STRM_BGM_SILENCE);
  }

  /*  bootProc  */
  m_pProc = GameSys::CallEggHatchingProc( &m_ProcParam );
  SetSeqNo(SEQ_ProcRunning_EggHatching);

  /*  clearsetting  */
  PushClearColor();
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  EggHatchingEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_SAFE_DELETE(m_ProcParam.m_pTargetEgg);
  GFL_SAFE_DELETE(m_ProcParam.m_pTargetMons);


  /*  BGM  */
  {
    Sound::EndAppBGMReq();
  }

  PopClearColor();

  /*  record処理  */
  {
    Savedata::IncRecord(Savedata::Record::RECID_TAMAGO_HATCHING);
  }
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
GameSys::GMEVENT_RESULT EggHatchingEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result  = GameSys::GMEVENT_RES_CONTINUE;

  switch(GetSeqNo())
  {
    case  SEQ_ProcRunning_EggHatching:
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pProc) )
      {
        m_pProc = NULL;

        {
          /*  UpdatePokeAtPokeGet()はInitでコール済み  */
          if(m_zukanCheckerReturnType == PokeTool::PokeGetUtil::ReturnType::CALL_ZUKAN_REGISTER)
          {
            app::event::ZukanRegisterEvent::StartEvent(pGameManager, &m_ZukanParam);
            SetSeqNo(SEQ_AppRunning_Zukan);
          }
          else
          {
            SetSeqNo(SEQ_Exit);
          }
        }
      }
      break;
      
    case  SEQ_AppRunning_Zukan:
      SetSeqNo(SEQ_Exit);
      break;
      
    case  SEQ_Exit:   /*  fallthrough  */
    default:
      result  = GameSys::GMEVENT_RES_FINISH;
      break;
  }

  return  result;
}



} /*  namespace Evolution    */
} /*  namespace NetApp  */
