//==============================================================================
/**
 * @file   EvolutionEvent.cpp
 * @date   2015/08/28 Fri. 19:55:56
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <pml/include/personal/pml_PersonalSystem.h>
#include  <pml/include/personal/pml_WazaOboeData.h>
#include  <Sound/include/Sound.h>

#include  "System/include/HeapDefine.h"
#include  "GameSys/include/GameEventManager.h"
#include  "GameSys/include/GameProcManager.h"
#include  "GameSys/include/GameData.h"
#include  "GameSys/include/DllProc.h"
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"

#if PM_DEBUG
  #include  "Debug/Launcher/include/LauncherProc.h"
  #include  "Debug/StartMenu/include/StartLauncher.h"
  #include  "Debug/DebugWin/include/DebugWinSystem.h"
#endif

/*  SaveData  */
#include  "Savedata/include/Record.h"

/*   Evolution */
#include  "NetApp/Evolution/include/EvolutionProcParam.h"
#include  "NetApp/Evolution/include/EvolutionProc.h"
#include  "NetApp/Evolution/source/System/EvolutionBuilder.h"
#include  "NetApp/Evolution/source/EvolutionFrameDef.h"

#include  "../include/EvolutionEvent.h"

#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"



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
    SEQ_ProcRunning_Evolution,


    /*  pokedex  */
    SEQ_AppRunning_Zukan,          /**<    */

    /*  waza  */
    SEQ_AppRunning_Waza,           /**<    */


    SEQ_Exit,

    SEQ_MAX,
  };
}



namespace NetApp    {
namespace Evolution {


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  APP_PARAM::Setup_FromZukan(void)
{
  m_BootMode  = BOOTMODE_CheckWaza;
  m_Result    = APPRESULT_Initial;
}


void  APP_PARAM::Setup_FromWazaWasure(const u32 oldWazaIndex)
{
  m_BootMode  = BOOTMODE_CheckWaza;
  m_Result    = APPRESULT_Initial;

  /*  新旧どちらも設定されている状態  */
  m_OldWazaIndex  = oldWazaIndex;
  if(m_OldWazaIndex < m_pTargetPokemon->GetWazaCount())
  {
    /*  有効な値であれば、差し替えられるWazaNoを設定する  */
    m_OldWaza = m_pTargetPokemon->GetWazaNo(m_OldWazaIndex);
  }
  else
  {
    /*  無効な値であれば、「覚えるのをあきらめた」が選択されたものとみなす  */
    m_OldWaza = WAZANO_NULL;
  }

//  m_NewWaza は技忘れ呼び出し前にセットされている
}


void  APP_PARAM::Setup_FromWazaWasureCancel(void)
{
  m_BootMode  = BOOTMODE_CheckWaza;
  m_Result    = APPRESULT_Initial;

  /*  新のみ設定されている状態  */
  m_OldWazaIndex  = 0;
  m_OldWaza       = WAZANO_NULL;

//  m_NewWaza は呼び出し前にセットされている
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    進化処理
  *            ヌケニンなど進化の過程で発生する特別な処理は基本的にここで行う
  */
//------------------------------------------------------------------
void  APP_PARAM::DoEvolve(gfl2::heap::HeapBase* pTempHeapBaseForNukenin)
{
  m_pTargetPokemon->Evolve(m_MonsNoEvolve, m_EvolveRootNum);

  /*  record対応  */
  {
    Savedata::IncRecord(Savedata::Record::RECID_POKE_EVOLUTION);   ///< 進化させたポケモンの匹数                 999,999,999  
    Savedata::IncRecord(Savedata::Record::RECID_DAYCNT_EVOLUTION); ///< 1日にポケモン進化させた回数              9,999
  }

  GFL_PRINT("DoEvolve: MonsNo[-> %d], vacancy[%d], partyResrv[%d]\n", m_pTargetPokemon->GetMonsNo(), (pml::PokeParty::MAX_MEMBERS - m_pPokeParty->GetMemberCount()), m_PokePartyReserveNum);
  /*
    ヌケニン処理
    orasより移植

    （１）進化したポケモンがテッカニンで、
    （２）モンスターボールを持っていて、
    （３）手持ちの空きがreserveNumより多い場合  ※旧：（３）手持ちに空きがあるとき、
     ヌケニンが生成される
  */
  {
    const MonsNo  REQUIRE_MONSNO    = MONSNO_TEKKANIN;      /*  要求ポケモン（テッカニン）        */
    const u16     REQUIRE_ITEM      = ITEM_MONSUTAABOORU;   /*  要求アイテム（モンスターボール）  */
    const u16     ITEM_REQUIRE_NUM  = 1;                    /*  要求アイテム消費                  */

    /**
      @note fix:GFNMcat[1575] 手持ちの空きがひとつで、且つBOXが満杯のときに戦闘相手を捕獲した結果ツチニンを進化させ、ヌケニンを発生させると、捕獲対象の格納先がなくなってしまう
      2016/03/15 Yu Muto
      ポケモンを捕獲して戦闘を終了、ツチニンがレベルアップの結果進化しヌケニンが発生した場合、
      nijiではその後に捕獲ポケモンが手持ち/BOXに追加されるため、ヌケニンの発生により空きがなくなると破綻する
      この問題を回避するため、呼び出し元で手持ちの空きを予約するm_PokePartyReserveNumを追加
      手持ち（pokeparty）の空きの数がm_PokePartyReserveNumより大きければヌケニン発生チェックに合格するようにする
    */
    if(
      (m_pTargetPokemon->GetMonsNo() == MONSNO_TEKKANIN)
      &&  m_pMyItem->Check(REQUIRE_ITEM, ITEM_REQUIRE_NUM)
      && (m_PokePartyReserveNum < (pml::PokeParty::MAX_MEMBERS - m_pPokeParty->GetMemberCount()))   /*  fix:GFNMcat[1575] により"&&  (!m_pPokeParty->IsFull())" より変更 */
    )
    {
      pml::pokepara::PokemonParam*  pNukeninParam;
      
      GFL_PRINT("<spawn NUKENIN>\n");

      /*  テッカニンからヌケニンを生成、手持ちに加える  */
      pNukeninParam = pml::pokepara::Factory::CreateNukenin(pTempHeapBaseForNukenin, m_pTargetPokemon);

      /*  ずかん登録  */
      {
        PokeTool::PokeGetUtil::ReturnParam      ret;

        /*                  図鑑詳細画面 | 名前入力画面 | 送り先選択画面 | 配置 | フラグ処理 | 備考 */
        /*  SHINKA  進化         ○      |      ×      |       ×       |  ×  |     ○     |      */
        PokeTool::PokeGetUtil::UpdatePokeAtPokeGet(
                                pTempHeapBaseForNukenin,
                                pNukeninParam,
                                PokeTool::PokeGetUtil::CallerType::SHINKA
                              );

        /**
          @note CALL_ZUKAN_REGISTERが返されたときだけ、（Putが後回しにされているので）自前でPutする
                2016/02/19  川田さん、太田さんと確認済み
          @note fix:GFNMcat[994]
                ↑この措置自体が間違っており、CallerType::SHINKA での CheckThenPutは「配置:×」であるため、Putは必ずコールする必要があるとのこと
                前回の対処でif(pokeGetUtilReturnType == ...REGISTER) で括ったが、ヌケニンがPutされなくなったため削除した
                2016/02/23  川田さんと共に確認済
        */
        PokeTool::PokeGetUtil::PutPoke(
          pTempHeapBaseForNukenin,
          pNukeninParam,
          PokeTool::PokeGetUtil::PutPlace::PARTY,
          PokeTool::PokeGetUtil::PutSpace::FREE,
          0,  /*  dummy (PutPlace::PARTY)   */
          0,  /*  dummy (PutSpace::FREE)    */
          &ret
        );

        /*  record対応  */
        {
          Savedata::IncRecord(Savedata::Record::RECID_CAPTURE_POKE);     ///< 捕獲したポケモンの数                     999,999
          Savedata::IncRecord(Savedata::Record::RECID_DAYCNT_CAPTURE);   ///< 1日に捕獲したポケモンの数                9,999
        }
      }

      /*  アイテム消費  */
      m_pMyItem->Sub(ITEM_MONSUTAABOORU, ITEM_REQUIRE_NUM);
      
      GFL_SAFE_DELETE(pNukeninParam);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
APP_PARAM::WazaResult  APP_PARAM::NextWaza(void)
{
  WazaResult  result  = WAZARESULT_None;

  m_OldWaza =
  m_NewWaza = WAZANO_NULL;

  /*  targetPokemonはこの時点で進化済み  */
  /*
    進化技確認
  */
  {
    bool  more  = false;

    do  {
      more  = false;
      switch(m_pTargetPokemon->LearnNewWazaOnEvolution(&m_PersonalWazaCheckCount, &m_NewWaza, &m_WazaWork))
      {
        case  pml::pokepara::WAZA_LEARNING_SUCCEEDED:         result  = WAZARESULT_Learned; break;  /*  成功：新しい技を覚えた  */
        case  pml::pokepara::WAZA_LEARNING_FAILED_FULL:       result  = WAZARESULT_Full;    break;  /*  失敗：手持ち技がいっぱいで覚えられなかった  */
        case  pml::pokepara::WAZA_LEARNING_FAILED_SAME:       more    = true;   /*  次  */  break;  /*  失敗：すでに覚えている  */
        case  pml::pokepara::WAZA_LEARNING_FAILED_NOT_EXIST:  result  = WAZARESULT_None;    break;  /*  失敗：覚える技が無かった  */
      }
    }while(more);
  }


  /*
    通常技確認
  */
  if(result  == WAZARESULT_None)
  {
    bool  more  = false;

    do  {
      more  = false;
      switch(m_pTargetPokemon->LearnNewWazaOnCurrentLevel(&m_FindWazaIndex, &m_NewWaza, &m_WazaWork))
      {
        case  pml::pokepara::WAZA_LEARNING_SUCCEEDED:         result  = WAZARESULT_Learned; break;  /*  成功：新しい技を覚えた  */
        case  pml::pokepara::WAZA_LEARNING_FAILED_FULL:       result  = WAZARESULT_Full;    break;  /*  失敗：手持ち技がいっぱいで覚えられなかった  */
        case  pml::pokepara::WAZA_LEARNING_FAILED_SAME:       more    = true;   /*  次  */  break;  /*  失敗：すでに覚えている  */
        case  pml::pokepara::WAZA_LEARNING_FAILED_NOT_EXIST:  result  = WAZARESULT_None;    break;  /*  失敗：覚える技が無かった  */
      }
    }while(more);
  }

  return result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  APP_PARAM::ForgetAndLearnWaza(void)
{
//  m_pTargetPokemon->RemoveWaza( static_cast<u8>(m_OldWazaIndex) );    // 位置を指定して技を削除する
//  m_pTargetPokemon->CloseUpWazaPos();                           // 技を前詰めにシフトする

  m_pTargetPokemon->SetWaza(static_cast<u8>(m_OldWazaIndex), m_NewWaza);       // 位置を指定して技をセットする
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  APP_PARAM::IsForgotWaza(void) const
{
  /*  新旧双方が設定されている  */
  return((m_OldWaza != WAZANO_NULL) && (m_NewWaza != WAZANO_NULL));
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
bool  APP_PARAM::IsForgetCancel(void) const
{
  /*  候補は設定されているが、差し替えられる技が未設定  */
  return((m_OldWaza == WAZANO_NULL) && (m_NewWaza != WAZANO_NULL));
}


//------------------------------------------------------------------
/**
  *  @brief    覚えるのをあきらめたか
  *  @note     IsForgetCancelの条件も同時に満たすので、こちらを先に評価する
  */
//------------------------------------------------------------------
bool  APP_PARAM::IsAbandonedForget(void) const
{
  /*  差し替えられる対象のWazaIndexが範囲外  */
  return (m_pTargetPokemon->GetWazaCount() <= m_OldWazaIndex);
}


//------------------------------------------------------------------
/**
  *  @brief    任意のポケモンがアローラ種であるかを判定
  */
//------------------------------------------------------------------
bool APP_PARAM::IsAlolaSpecies(const pml::pokepara::CoreParam& rCoreParam)
{
    pml::personal::LoadPersonalData(rCoreParam.GetMonsNo(), rCoreParam.GetFormNo());

    return  (pml::personal::GetPersonalParam(pml::personal::PARAM_ID_region) == pml::personal::REGION_ALOLA);
}

} /*  namespace Evolution */
} /*  namespace NetApp    */












namespace NetEvent  {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     StartEvent
  *  @brief    EvolutionEventの開始
  */
//------------------------------------------------------------------
void EvolutionEvent::StartEvent(GameSys::GameManager* pGameManager)
{
  GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EvolutionEvent>( pGameManager->GetGameEventManager() );
}


void EvolutionEvent::StartEvent(GameSys::GameManager* pGameManager, EvolutionEventParam& rEventParam)
{
  EvolutionEvent*   pEvent  = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EvolutionEvent>( pGameManager->GetGameEventManager() );

  GFL_PRINT("Evolution: MonsNo[%d -> %d], reason[%d], partyResrv[%d]\n", rEventParam.m_pTargetPokemon->GetMonsNo(), rEventParam.m_MonsNoEvolve, rEventParam.m_Reason, rEventParam.m_PokePartyReserveNum);
  
  pEvent->m_EventParam  = rEventParam;
}


//------------------------------------------------------------------
/**
  *  @func     EvolutionEvent
  *  @brief    ctor
  */
//------------------------------------------------------------------
EvolutionEvent::EvolutionEvent( gfl2::heap::HeapBase* pHeap )
  : GameSys::GameEvent(pHeap)
  , m_ProcParam()
  , m_pProc(NULL)
  /*  zukan  */
  , m_ZukanParam()
  , m_pNukeninTempParam(NULL)
  /*  waza  */
  , m_pDummyPartyForWazaProc(NULL)
  , m_WazaParam()
  , m_pWazaProc(NULL)
  /*  others  */
  , m_IsNeedBGMResume(false)
{
}



//------------------------------------------------------------------
/**
  *  @func     ~EvolutionEvent
  *  @brief    dtor
  */
//------------------------------------------------------------------
EvolutionEvent::~EvolutionEvent()
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
bool  EvolutionEvent::BootChk( GameSys::GameManager* pGameManager )
{
  return true;
}


//------------------------------------------------------------------
/**
  *  @func    InitFunc
  *  @brief   初期化
  */
//------------------------------------------------------------------
void  EvolutionEvent::InitFunc( GameSys::GameManager* pGameManager )
{
  gfl2::heap::HeapBase* pHeapBase = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );


  /*  dummyparty  */
  {
    m_pDummyPartyForWazaProc  = GFL_NEW(pHeapBase)  pml::PokeParty(pHeapBase);
    m_pDummyPartyForWazaProc->AddMember(*(m_EventParam.m_pTargetPokemon));
  }

  /*  setup appparam  */
  {
    m_ProcParam.m_BootMode        = NetApp::Evolution::BOOTMODE_Evolve;

    /*  result  */
    m_ProcParam.m_Result          = NetApp::Evolution::APPRESULT_Initial;

    /*  進化の要因  */
    m_ProcParam.m_Reason          = m_EventParam.m_Reason;

    /*  進化関連  */
    m_ProcParam.m_pTargetPokemon  = m_EventParam.m_pTargetPokemon;        /**<  対象となるポケモンのパラメータ                */
    m_ProcParam.m_MonsNoEvolve    = m_EventParam.m_MonsNoEvolve;          /**<  進化先MonsNo                                  */
    m_ProcParam.m_EvolveRootNum   = m_EventParam.m_EvolveRootNum;         /**<  進化Root                                      */

    /*  所持品関連  */
    m_ProcParam.m_pPokeParty          = m_EventParam.m_pPokeParty;            /**<  所持ポケモン（主にヌケニン用）                */
    m_ProcParam.m_pMyItem             = m_EventParam.m_pMyItem;               /**<  所持アイテム（主にヌケニン用）                */
    m_ProcParam.m_PokePartyReserveNum = m_EventParam.m_PokePartyReserveNum;   /**<  [in]  手持ちの空きが外部で予約されている数（ヌケニン発生確認用、手持ちの空きがこの数より多ければ合格）  */
  }

  /*  BGM  */
  /*
    2016/06/22
    @caution
    CROSSCHECK[8417]:IsBGMPlaying: EggHatchingEvent.cpp/EvolutionEvent.cpp
    本来は
      Sound::StartAppBGMReq(STRM_BGM_SILENCE);
      Sound::EndAppBGMReq();
    で取り回すべきだが、
      ・時期
      ・範囲
      ・現状では実装上問題が発生しないことがコードから予想される（EvolutionはAppの接続タイミングが比較的固定されている）
    以上のことから、対処が見送られた
    ※EggHatchingはユーザの操作によりBGM状態が流動的であるため、上記対処を行った
  */
  if(Sound::IsBGMPlaying() && (!Sound::IsBGMFade()))
  {
    Sound::PushBGMReq();
    m_IsNeedBGMResume = true;
  }
  Sound::StartBGMReq(STRM_BGM_SILENCE);

  /*  bootProc  */
  if(m_EventParam.m_pResult)
  {
    /*  Cancel時以外はDemoに2度以上入ることがあるので、Defaultは真としてCancel時には偽にする  */
    m_EventParam.m_pResult->m_IsEvolved  = true;
  }
  m_pProc = GameSys::CallEvolutionProc( &m_ProcParam );
  SetSeqNo(SEQ_ProcRunning_Evolution);

  /*  clearsetting  */
  PushClearColor();
}


//------------------------------------------------------------------
/**
  *  @func    EndFunc
  *  @brief   終了
  */
//------------------------------------------------------------------
void  EvolutionEvent::EndFunc( GameSys::GameManager* pGameManager )
{
  GFL_SAFE_DELETE(m_pDummyPartyForWazaProc);
  Sound::FadeOutBGMReq();
  if(m_IsNeedBGMResume)
  {
    Sound::PopBGMReq(Sound::BGM_FADE_VERY_FAST);
  }
  PopClearColor();

  if(m_EventParam.m_pResult)
  {
    GFL_PRINT("evolutionResult: %s\n", m_EventParam.m_pResult->m_IsEvolved ? "true" : "false");
  }
}


//------------------------------------------------------------------
/**
  *  @func    MainFunc
  *  @brief   更新
  */
//------------------------------------------------------------------
GameSys::GMEVENT_RESULT EvolutionEvent::MainFunc( GameSys::GameManager* pGameManager )
{
  GameSys::GMEVENT_RESULT result  = GameSys::GMEVENT_RES_CONTINUE;

  switch(GetSeqNo())
  {
    case  SEQ_ProcRunning_Evolution:
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pProc) )
      {
        /*  結果  */
        if(m_EventParam.m_pResult)
        {
          if(m_ProcParam.m_Result  == NetApp::Evolution::APPRESULT_Cancel)
          {
            m_EventParam.m_pResult->m_IsEvolved  = false;
          }
        }

        m_pProc = NULL;
        /*  proc結果により分岐  */
        ResultHandler(pGameManager);
      }
      break;

    /*  zukan  */
    case  SEQ_AppRunning_Zukan:
      /*  ※図鑑から戻ってきた  */
      m_ProcParam.Setup_FromZukan();
      m_pProc = GameSys::CallEvolutionProc( &m_ProcParam );
      SetSeqNo(SEQ_ProcRunning_Evolution);
      break;

    /*  wazaoshie  */
    case  SEQ_AppRunning_Waza:
      /*  ※技忘れ起動中  */
      if( !pGameManager->GetProcManager()->IsRegisterProcess(m_pWazaProc) )
      {
        /*  終了  */
        m_pWazaProc = NULL;

        if(m_WazaParam.end_mode)
        {
          m_ProcParam.Setup_FromWazaWasure(m_WazaParam.select_index);
        }
        else
        {
          m_ProcParam.Setup_FromWazaWasureCancel();
        }
        /*  bootProc  */
        m_pProc = GameSys::CallEvolutionProc( &m_ProcParam );
        SetSeqNo(SEQ_ProcRunning_Evolution);
      }
      break;
      
    case  SEQ_Exit:   /*  fallthrough  */
    default:
      result  = GameSys::GMEVENT_RES_FINISH;
      break;
  }


  return  result;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EvolutionEvent::ResultHandler(GameSys::GameManager* pGameManager)
{
  
  switch(m_ProcParam.m_Result)
  {
    case  NetApp::Evolution::APPRESULT_CallZukan:        /**<  図鑑登録要求  */
    {
      m_ZukanParam.pp         = m_ProcParam.m_pTargetPokemon;       //  pml::pokepara::PokemonParam*            pp;
      m_ZukanParam.callerType = m_ProcParam.GetZukanCallerType();   //  PokeTool::PokeGetUtil::CallerType::Tag  callerType
      
      app::event::ZukanRegisterEvent::StartEvent(pGameManager, &m_ZukanParam);
      SetSeqNo(SEQ_AppRunning_Zukan);

      // @note momiji追加：ロトムのお願い発生
      if( m_EventParam.m_Reason != NetApp::Evolution::REASON_Trade )
      {
        GameSys::GameData * gd = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData();
        if( gd->GetEventWork()->CheckEventFlag(SYS_FLAG_FS_TUTORIAL_END) != false )
        {
          gd->GetFieldMenuPlayData()->SetRotomRequestEvolution( true );
        }
      }
    }
      break;


    case  NetApp::Evolution::APPRESULT_CallWazaWasure:   /**<  技忘れ要求    */
    {
      /*  wazaoshie起動  */
      m_pWazaProc = GameSys::CallWazaOshieProc();

      m_pDummyPartyForWazaProc->ReplaceMember(0, *(m_EventParam.m_pTargetPokemon));

      m_WazaParam.mode              = 0;  /**<  画面モード  0:技教え 1:技忘れ */
      m_WazaParam.select_poke_index = 0;
      m_WazaParam.waza_no           = m_ProcParam.m_NewWaza;
      m_WazaParam.upper_draw        = 1;
      m_WazaParam.partyData         = m_pDummyPartyForWazaProc;
      
      m_pWazaProc->SetAppParam(&m_WazaParam);
      SetSeqNo(SEQ_AppRunning_Waza);
    }
      break;

    case  NetApp::Evolution::APPRESULT_Cancel:
      Savedata::IncRecord(Savedata::Record::RECID_CANCEL_EVOLUTION);   ///< 進化キャンセルの回数                                        9,999
      /*  fall through  */
    
    case  NetApp::Evolution::APPRESULT_Initial:          /**<  初期値        */
    case  NetApp::Evolution::APPRESULT_Exit:             /**<  終了          */
    default:
      SetSeqNo(SEQ_Exit);
      break;
  }
}



} /*  namespace Evolution    */
} /*  namespace NetApp  */
