//=============================================================================
/**
 * @file    BagEvent.cpp
 * @brief   バッグ画面イベント
 * @author  Hiroyuki Nakamura
 * @date    2015.03.04
 */
//=============================================================================

// module
#include "App/AppEvent/include/Bag/BagEvent.h"
// niji
#include "GameSys/include/GameProcManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/DllProc.h"
#include "App/FieldBag/include/BagProc.h"
#include "App/WazaOshie/include/WazaOshieProc.h"
#include "App/AppEvent/include/Bag/BagGamedata.h"
//#include "NetApp/Evolution/include/EvolutionProcParam.h"
#include <PokeTool/include/EvolutionUtil.h>

#if PM_DEBUG
#include "Debug/StartMenu/include/StartLauncher.h"
#endif


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Event)

//-----------------------------------------------------------------------------
/**
 * @brief   コンストラクタ
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
BagEvent::BagEvent( gfl2::heap::HeapBase * heap )
  : GameSys::GameEvent( heap )
{
  m_appParam = &m_defAppParam;
  gfl2::std::MemClear( m_appParam, sizeof(app::bag::APP_PARAM) );
#if PM_DEBUG
  SetupLauncherCall( false ); // デバッグフラグ初期化
#endif  // PM_DEBUG
}

//-----------------------------------------------------------------------------
/**
 * @brief   デストラクタ
 */
//-----------------------------------------------------------------------------
BagEvent::~BagEvent()
{
}


//-----------------------------------------------------------------------------
/**
 * @func    StartEvent
 * @brief   イベント起動
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  BagEventクラス
 */
//-----------------------------------------------------------------------------
BagEvent * BagEvent::StartEvent( GameSys::GameManager * gmgr )
{
  GameSys::GameEventManager * ev_man = gmgr->GetGameEventManager();
  return GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, BagEvent >( ev_man );
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupAppParam
 * @brief   パラメータ設定
 * @date    2015.12.09
 *
 * @param   prm   バッグ画面の外部設定パラメータ
 */
//-----------------------------------------------------------------------------
void BagEvent::SetupAppParam( app::bag::APP_PARAM * prm )
{
  m_appParam = prm;
}

//-----------------------------------------------------------------------------
/**
 * @func    SetupFieldParam
 * @brief   フィールド関連のパラメータ設定
 * @date    2015.12.09
 *
 * @param   work      アイテム使用ワーク
 * @param   weather   天候
 *
 * @note  APP_PARAMを外部指定する場合、SetupAppParam()の後に呼ぶこと！
 */
//-----------------------------------------------------------------------------
void BagEvent::SetupFieldParam( Field::FieldItemUse::ITEM_USE_CHECK_WORK * work, Field::weather::WeatherKind weather )
{
  m_appParam->itemuse_work = *work;
  m_appParam->weather      = weather;
}

//-----------------------------------------------------------------------------
/**
 * @func    BookChk
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @retval  true  = 起動してよい
 * @retval  false = 起動しない
 */
//-----------------------------------------------------------------------------
bool BagEvent::BootChk( GameSys::GameManager * /*gmgr*/ )
{
  return true;
}

//-----------------------------------------------------------------------------
/**
 * @func    InitFunc
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void BagEvent::InitFunc( GameSys::GameManager * /*gmgr*/ )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    MainFunc
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 *
 * @return  イベント制御関数の戻り値
 */
//-----------------------------------------------------------------------------
GameSys::GMEVENT_RESULT BagEvent::MainFunc( GameSys::GameManager * gmgr )
{
  enum
  {
    EVSEQ_PROC_CREATE = 0,
    EVSEQ_PROC_WAIT,

    EVSEQ_WAZAWASURE_PROC_CREATE,
    EVSEQ_WAZAWASURE_PROC_WAIT,

    EVSEQ_EVOLUTION_PROC_CREATE,
    EVSEQ_EVOLUTION_PROC_WAIT,

    EVSEQ_PROC_END,
  };

  switch( GetSeqNo() )
  {
  case EVSEQ_PROC_CREATE:
    {
      m_i.SetParameter(gmgr->GetGameData()->GetPlayerParty());                      // ポケモンデータ
      m_appParam->pokemon   = &m_i;
      m_appParam->call_mode = app::bag::CALL_FIELD_MENU;                             // 呼び出しモード
      m_appParam->pocket_no = gmgr->GetGameData()->GetBagPlayData()->GetPocketPos(); // 開始時のポケット番号

      app::bag::BagProc * proc = GameSys::SetNextBagProc();
      proc->Setup( m_appParam );
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_WAIT:
    // プロセスの終了待ち
    if( !gmgr->IsProcessExists() )
    {
      gmgr->GetGameData()->GetBagPlayData()->SetPocketPos(m_appParam->pocket_no);  // 終了時のポケット番号
      // 技忘れ画面を呼び出す
      if( m_appParam->end_mode == app::bag::END_CALL_WAZA_WASURE )
      {
        SetSeqNo( EVSEQ_WAZAWASURE_PROC_CREATE );
      }
      // 進化画面を呼び出す
      else if( m_appParam->end_mode == app::bag::END_CALL_EVOLUTION )
      {
        SetSeqNo( EVSEQ_EVOLUTION_PROC_CREATE );
      }
      else
      {
        SetSeqNo( EVSEQ_PROC_END );
      }
    }
    break;

  case EVSEQ_WAZAWASURE_PROC_CREATE:
    SetWazaOhsieParam( m_appParam, &m_wazaOshieParam );
    {
      App::WazaOshie::Proc * proc = GameSys::CallWazaOshieProc();
      proc->SetAppParam( &m_wazaOshieParam );
    }
    SetSeqNo( EVSEQ_WAZAWASURE_PROC_WAIT );
    break;

  case EVSEQ_WAZAWASURE_PROC_WAIT:
    if( !gmgr->IsProcessExists() )
    {
      m_appParam->rcv_mode = app::bag::RCV_WAZAOSHIE;
      if( m_wazaOshieParam.end_mode == 0 )
      {
        m_appParam->select_skill_pos = pml::MAX_WAZA_NUM;
      }
      else
      {
        m_appParam->select_skill_pos = m_wazaOshieParam.select_index;  // 選択した技の位置
      }
      app::bag::BagProc * proc = GameSys::SetNextBagProc();
      proc->Setup( m_appParam );
      SetSeqNo( EVSEQ_PROC_WAIT );
    }
    break;

  case EVSEQ_EVOLUTION_PROC_CREATE:
    SetEvolutionDemoParam( gmgr->GetGameData(), m_appParam, &m_evolutionParam );
    NetEvent::Evolution::EvolutionEvent::StartEvent( gmgr, m_evolutionParam );
    SetSeqNo( EVSEQ_EVOLUTION_PROC_WAIT );
    break;

  case EVSEQ_EVOLUTION_PROC_WAIT:
    {
      m_appParam->rcv_mode = app::bag::RCV_EVOLUTION;
      app::bag::BagProc * proc = GameSys::SetNextBagProc();
      proc->Setup( m_appParam );
    }
    SetSeqNo( EVSEQ_PROC_WAIT );
    break;

  case EVSEQ_PROC_END:
#if PM_DEBUG
    if( IsLauncherCall() != false )
    {
      debug::launcher::LauncherProc::CallParam param;
      debug::launcher::LauncherProc::InitCallParam( &param );
      debug::launcher::LauncherProc * proc = NIJI_PROC_CALL< GameSys::GameProcManager, debug::launcher::LauncherProc, debug::launcher::LauncherProc::CallParam >( gmgr->GetProcManager(), &param );
      proc->CreateStartListData< debug::startmenu::StartLauncher >();
    }
#endif  // PM_DEBUG
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//-----------------------------------------------------------------------------
/**
 * @func    EndFunc
 * @brief   呼び出される関数 純粋仮想関数
 * @date    2015.06.11
 *
 * @param   gmgr  ゲームマネージャークラス
 */
//-----------------------------------------------------------------------------
void BagEvent::EndFunc( GameSys::GameManager * gmgr )
{
}

//-----------------------------------------------------------------------------
/**
 * @func    SetWazaOshieParam
 * @brief   技教え画面パラメータ設定
 * @date    2015.11.25
 *
 * @param   bag_param   バッグの外部設定パラメータ
 * @param   wo_param    技教えのパラメータ設定場所
 */
//-----------------------------------------------------------------------------
void BagEvent::SetWazaOhsieParam( app::bag::APP_PARAM * bag_param, App::WazaOshie::APP_PARAM * wo_param )
{
  gfl2::std::MemClear( wo_param, sizeof(App::WazaOshie::APP_PARAM) );
  wo_param->mode = App::WazaOshie::MODE_WAZA_OBOE;
  wo_param->select_poke_index = bag_param->select_pokemon_pos;          // ポケモン位置
  wo_param->upper_draw = 1;                                             // 上画面の描画フラグ
  wo_param->waza_no = static_cast<WazaNo>( bag_param->select_skill );   // 覚えさせる技
}

//-----------------------------------------------------------------------------
/**
 * @func    SetEvolutionDemoParam
 * @brief   進化デモ用パラメータ設定
 * @date    2015.11.25
 *
 * @param   gd          ゲームデータ
 * @param   bag_param   バッグの外部設定パラメータ
 * @param   evo_param   進化デモのパラメータ設定場所
 */
//-----------------------------------------------------------------------------
void BagEvent::SetEvolutionDemoParam( GameSys::GameData * gd, app::bag::APP_PARAM * bag_param, NetEvent::Evolution::EvolutionEventParam * evo_param )
{
  gfl2::std::MemClear( evo_param, sizeof(NetEvent::Evolution::EvolutionEventParam) );

  pml::PokeParty * party = gd->GetPlayerParty();
  pml::pokepara::PokemonParam * pp = party->GetMemberPointer( bag_param->select_pokemon_pos );

  evo_param->m_pTargetPokemon = pp;
  evo_param->m_pPokeParty = party;
  evo_param->m_pMyItem = gd->GetMyItem();

  if( bag_param->select_item != ITEM_HUSIGINAAME )
  {
    // アローラ以外の場所では通常ポケモンに進化したい要望
    pml::pokepara::EvolveSituation evoSituation;
    // 進化設定 場所の情報だけ欲しいので天気と逆さ状況は適当
    PokeTool::SetupEvolveSituation( &evoSituation, GFL_SINGLETON_INSTANCE(GameSys::GameManager), false, Field::weather::SUNNY );
    pp->CanEvolveByItem( &evoSituation, bag_param->select_item, &evo_param->m_MonsNoEvolve, &evo_param->m_EvolveRootNum );
    evo_param->m_Reason = NetApp::Evolution::REASON_Item;
//    wk->prm.InitForItem( party, swk->selPokemon.curPos, new_monsno, evolution_root );
	}else{
    pp->CanEvolve( &bag_param->evoSituation, party, &evo_param->m_MonsNoEvolve, &evo_param->m_EvolveRootNum );
    evo_param->m_Reason = NetApp::Evolution::REASON_LevelUp;
//    wk->prm.InitForFieldLevelUp( party, swk->selPokemon.curPos, new_monsno, evolution_root );
	}
}



//=============================================================================
//  デバッグ用
//=============================================================================
#if PM_DEBUG
bool DebugBagLauncherCall = false;

void BagEvent::SetupLauncherCall( bool flg )
{
  DebugBagLauncherCall = flg;
}

bool BagEvent::IsLauncherCall(void)
{
  return DebugBagLauncherCall;
}
#endif  // PM_DEBUG


GFL_NAMESPACE_END(Event)
GFL_NAMESPACE_END(App)
