//=============================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file EventBattleCall.cpp
 *  @brief バトル呼び出し
 *  @author tomoya takahshi
 *  @date 2011.06.06
 *
 *  @niji_author saita_kazuki
 *  @date 2015.05.25
 */
//=============================================================================

#include "Field/FieldStatic/include/Event/EventBattleCall.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <Fade/include/gfl2_FadeManager.h>

// field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
#include "Field/FieldRo/include/WeatherControl/WeatherControl.h"
#include "Field/FieldStatic/include/Zone/ZoneDataLoader.h"
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"
#include "Field/FieldStatic/include/Event/EventBattleCallDefine.h"
#include "Field/FieldStatic/include/Event/EventProcessCall.h"
#include "Field/FieldStatic/include/Event/EventBattleReturn.h"
#include "Field/FieldStatic/include/Event/EventGameOver.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldStatic/include/FieldAttribute.h"
#include "Field/FieldStatic/include/Encount/FieldEncountEffect.h"
#include "Field/FieldStatic/include/Event/EventBattleDll.h"
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "Field/FieldStatic/source/Event/BattleReturnBGM.h"

// gamesys
#include "GameSys/include/DllProc.h"
#include "GameSys/include/GameEventManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "GameSys/include/TimeZone.h"

// pmlib
#include <pml/include/pmlib.h>
#include "PokeTool/include/NakayoshiManager.h"

// battle
#include <Battle/include/battle_proc.h>
#include <Battle/Background/include/BgSetupUtil.h>

// savedata
#include "Savedata/include/EventWork.h"
#include "Savedata/include/MyStatus.h"

// system
#include "System/include/Skybox/Skybox.h"

// poke_tool
#include "PokeTool/include/EvolutionUtil.h"

// conv_header
#include <niji_conv_header/field/chara/p2_base_fi.h>

// バトル施設
#include "Field/FieldStatic/include/BattleInst/BattleInst.h"
#include "Field/FieldStatic/include/BattleInst/BattleInstManager.h"
#include "App/AppEvent/include/BattleRoyalResult/BattleRoyalResultEvent.h"
#include "App/BattleRoyalResult/include/BattleRoyalResultAppParam.h"
#include "App/AppEvent/include/BattleTree/BattleTreeEvent.h"
#include "Savedata/include/BattleInstSave.h"

#include "Battle/Regulation/include/Regulation.h"
#include "Field/FieldStatic/include/BattleFes/BattleFesManager.h"
#include "Field/FieldStatic/include/BattleFes/BattleFes.h"

#include "Field/FieldRos/FieldGimmickBattleFes/include/BattleFes/BattleFesModelManager.h"
#include "Field/FieldRos/FieldGimmickBattleFes/include/FieldGimmickBattleFes.h"

#if 0
#include <nn/prof.h>
// プロファイル開始
#define BTL_CTR_PROFILING_START nnprofRuntimeControlInitialize(); \
                                  nnprofRuntimeControlSetFlags            (NN_PROF_FLAGS_CALLSTACKS); \
                                  nnprofRuntimeControlSetSampleMethod     (NN_PROF_SAMPLEMETHOD_TIME); \
                                  nnprofRuntimeControlSetSampleTime       (NN_PROF_SETTINGS_100X); \
                                  nnprofRuntimeControlSetThreadIDToProfile(nn::os::Thread::GetCurrentId()); \
                                  nnprofRuntimeControlSetCoreToProfile    (NN_PROF_CORE_APPLICATION); \
                                  nnprofRuntimeControlStart();
// プロファイル終了
#define BTL_CTR_PROFILING_END   nnprofRuntimeControlStop();
#else
#define BTL_CTR_PROFILING_START
#define BTL_CTR_PROFILING_END
#endif

#if PM_DEBUG
#include "AppLib/include/Fade/DemoFade.h"
#include "DemoLib/SequencePlayer/include/SequencePlayerCore.h"
#endif // PM_DEBUG

GFL_NAMESPACE_BEGIN( Field )

//-----------------------------------------------
// Tick計測ワーク定義
enum
{
  TICK_EFFECT_TOTAL,
  TICK_EFFECT_LOAD,
  TICK_FIELD_CLOSE,
  TICK_EFFECT_SETUP,
  TICK_MAX,
};
GFL_DEBUG_TICK_SETUP(EventBattleCallTick,TICK_MAX);



static const u32 SAKASA_BATTLE_PENALTY_TURN( 200 );    ///< サカサバトル、ターン数がこれ以上ならポイント0


//----------------------------------------------------------------------------
/**
 *  @brief  野性戦(イベント生成用)
 */
//-----------------------------------------------------------------------------
EventBattleCall* EventBattleCall::CreateWild( GameSys::GameEventManager* p_eventman, GameSys::GameManager* p_gameman, Encount::PokeSet& poke_set, u32 call_option, u32 ret_bgm_id /*=Sound::SOUND_ITEM_ID_NONE*/ )
{
  EventBattleCall* p_event;
  p_event = GFL_NEW(p_eventman->GetHeap()) EventBattleCall(p_eventman->GetHeap());
  p_event->SetUpWildPoke( p_gameman, poke_set, call_option );
  p_event->SetReturnBGMID( ret_bgm_id );
  return p_event;
}

//----------------------------------------------------------------------------
/**
 *  @brief  野性戦(プログラム呼び出し用)
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventBattleCall::CallWild( GameSys::GameEventManager* p_eventman, GameSys::GameManager* p_gameman, Encount::PokeSet& poke_set, u32 call_option /*=EVBTL_CALLOPT_NONE*/, const pml::PokeParty * cp_party /* = NULL */ )
{
  EventBattleCall* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleCall > ( p_eventman );
  p_event->SetUpWildPoke( p_gameman, poke_set, call_option, cp_party );

  return static_cast < GameSys::GameEvent* >( p_event );
}
//----------------------------------------------------------------------------
/**
 *  @brief  野性戦(プログラム呼び出し用＆戦闘戻りのBGM指定可能)
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventBattleCall::CallWildEx( GameSys::GameEventManager* p_eventman, GameSys::GameManager* p_gameman, Encount::PokeSet& poke_set, u32 call_option, u32 ret_bgm_id, const pml::PokeParty * cp_party /* = NULL */ )
{
  EventBattleCall* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleCall > ( p_eventman );
  p_event->SetUpWildPoke( p_gameman, poke_set, call_option, cp_party );
  p_event->SetReturnBGMID( ret_bgm_id );

  return static_cast < GameSys::GameEvent* >( p_event );
}

//----------------------------------------------------------------------------
/**
 *  @brief  トレーナー戦
 *
 *  @param  p_gameman ゲームマネージャ
 *  @param  param     トレーナーバトル設定パラメータ
 *  @param  call_option EVBTL_CALLOPT_NONE他 イベントシーケンス制御オプション
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventBattleCall::CallTrainer( GameSys::GameManager* p_gameman, const TrainerBattleParam& param, u32 call_option )
{
  EventBattleCall* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleCall > ( p_gameman->GetGameEventManager( ) );
  p_event->SetUpTrainer( p_gameman, param, call_option );
  p_event->SetReturnBGMID(param.retBGMID);

  return static_cast < GameSys::GameEvent* >( p_event );
}

//----------------------------------------------------------------------------
/**
 *  @brief  デフォルト設定でのバトル呼び出し（通常）
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventBattleCall::Call( GameSys::GameEventManager* p_eventman, BATTLE_SETUP_PARAM* p_param )
{
  EventBattleCall* p_event;
  p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleCall > ( p_eventman );
  p_event->SetUpBattleParam( p_param );
  return static_cast < GameSys::GameEvent* >( p_event );
}

//----------------------------------------------------------------------------
/**
 *  @brief  戦闘結果から勝ち負け(勝負に負けたか？)の判定
 *
 *  @note スカイバトルが導入されたことで「勝負に負けたか？」と「全滅してゲームオーバーになるか？」
 *        は区別するべき事象になった
 */
//-----------------------------------------------------------------------------
bool EventBattleCall::IsLoseResult( BtlResult result )
{
  if( result == BTL_RESULT_LOSE ||
      result == BTL_RESULT_DRAW ){
    return true;
  }
  return false;
}
//----------------------------------------------------------------------------
/**
 *  @brief  戦闘結果からゲームオーバーか否かの判定
 *
 *  @note スカイバトルが導入されたことで「勝負に負けたか？」と「全滅してゲームオーバーになるか？」
 *        は区別するべき事象になった
 *
 *  @niji スカイバトルは無いが、このクッション自体は残しておきましょう。
 *  スカイバトルフラグは要らないです
 */
//-----------------------------------------------------------------------------
bool EventBattleCall::IsGameoverResult( BtlResult result, const pml::PokeParty* pParty, bool* is_overwrite )
{
  (*is_overwrite) = false; //先ずは上書き無しに初期化

  if( IsLoseResult( result))
  {
    return true;
  }
  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  戦闘結果から勝ち負け（勝ったか？）の判定
 *
 *  @param  result 
 *
 *  @retval true  勝利
 *  @retval false  勝利未満
 */
//-----------------------------------------------------------------------------
bool EventBattleCall::IsWinResult( BtlResult result )
{
  return ( result == BTL_RESULT_WIN );
}

//----------------------------------------------------------------------------
/**
 *  @brief  戦闘結果からトレーナーで負けたかの判定
 *
 *  @param  result 
 *
 *  @retval true 負け扱い
 *  @retval false それ以外
 */
//-----------------------------------------------------------------------------
bool EventBattleCall::IsLoseTrainerResult( BtlResult result )
{
  // スクリプト内で行っていたフラグセットをEventBattleCall層でやることになったので
  // スクリプトで使用していた勝ち負け取得関数と実装を合わせる
  // (=ScriptFuncSetCommon::BattleGetResult_()でBTL_RESULT_LOSEになるパターン)
  switch( result)
  {
  case BTL_RESULT_LOSE:
  case BTL_RESULT_DRAW:
  case BTL_RESULT_COMM_ERROR:
    return true;
  };
  return false;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief バトル結果を野生戦の結果に丸め込む
 *
 * @param result バトル結果
 *
 * @return 野生戦バトル結果
 */
/* ------------------------------------------------------------------------- */
u32 EventBattleCall::GetWildBattleResult( BtlResult result )
{
  switch( result ){
  case BTL_RESULT_WIN:         ///< 勝った
    return BTL_RESULT_WILD_WIN;

  case BTL_RESULT_RUN:         ///< 逃げた
    return BTL_RESULT_WILD_RUN;
  
  case BTL_RESULT_RUN_ENEMY:   ///< 相手が逃げた（野生のみ）
    return BTL_RESULT_WILD_RUN_ENEMY;
  
  case BTL_RESULT_CAPTURE:     ///< 捕まえた（野生のみ）
    return BTL_RESULT_WILD_CAPTURE;

  case BTL_RESULT_LOSE_NOT_GAMEOVER_FOR_SKY: ///<スカイバトルでしかありえないので、ここに来たらおかしい
    GFL_ASSERT(0); //@check
    return BTL_RESULT_WILD_WIN;
  
  case BTL_RESULT_LOSE:        ///< 負けた
  case BTL_RESULT_DRAW:        ///< ひきわけ
  case BTL_RESULT_COMM_ERROR:  ///< 通信エラーによる
  default:
    break;
  }
  return BTL_RESULT_WILD_LOSE;
}


//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventBattleCall::BootChk( GameSys::GameManager* /*p_gman*/ )
{
  return true;
}

//--------------------------------------------------------------
/**
  // さかさバトル(効果抜群判定が逆になる特殊バトル)
  if ( m_ExFlag & TRAINER_BTL_FLAG_SAKASA_BATTLE )
  {
    ONOUE_PRINT( ">サカサバトル\n" );
    BATTLE_PARAM_SetSakasaBattle( m_pParam );
  }
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------

void EventBattleCall::InitFunc( GameSys::GameManager* p_gman )
{
  Fieldmap * p_fieldmap = p_gman->GetFieldmap();
  Attr attr = ATTR_ERROR;

  GFL_DEBUG_TICK_SET_FLAG(EventBattleCallTick,true);
  GFL_DEBUG_TICK_RESET(EventBattleCallTick);

  GFL_DEBUG_TICK_START(EventBattleCallTick, TICK_EFFECT_TOTAL, "TICK_EFFECT_TOTAL");

  if (m_cpZoneDataLoader == NULL)
  {
    m_cpZoneDataLoader = p_gman->GetGameData()->GetFieldZoneDataLoader();
  }

  //フィールドが存在する時のみの処理
  if( p_fieldmap != NULL )
  {
    attr = m_Attr;
    if (attr == ATTR_ERROR)
    {
      //足元アトリビュート取得
      attr = p_gman->GetFieldmap()->GetPlayerCharacter()->GetGroundAttribute();
    }

    // 天候を保存しておく
    m_weather = p_fieldmap->GetWeatherControl()->GetNowWeatherKind();
  }
  
  //エンカウントワークアップデート(エンカウント率初期化)
  p_gman->GetGameData()->GetEncountWork()->BattleCallUpdate( attr );

  // さかさバトル(効果抜群判定が逆になる特殊バトル)
  if ( m_ExFlag & TRAINER_BTL_FLAG_SAKASA_BATTLE )
  {
    ONOUE_PRINT( ">サカサバトル\n" );
    BATTLE_PARAM_SetSakasaBattle( m_pParam );
  }

  if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF ) == false )
  {
    // 戦闘導入演出取得
    m_pEncountEffect = p_gman->GetGameData()->GetEncountWork()->GetEncountEffect();
  }

  if( (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false )
  {
    // 戦闘導入演出を外部で再生していなければシーケンス関連のDLLを読み込む (外部再生の場合はもう読み込まれている)
    // バトルチームとの取り決めで、必ずシーケンス関連のDLLは読み込む必要がある
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
    gfl2::heap::HeapBase* p_heap_dll = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );
    p_gman->GetGameData()->GetEncountWork()->StartLoadEncountEffectDllThread( p_heap, p_heap_dll );
  }
  else
  {
    GFL_ASSERT_STOP_MSG( p_gman->GetGameData()->GetEncountWork()->IsLoadEncountEffectDll(), "SequenceDLL is not loaded.\n" );
  }

#if PM_DEBUG
  if( p_fieldmap != NULL)
  {
    m_isBattleThrough = GFL_BOOL_CAST( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENCOUNT_THROUGH) );
    m_isBattleHio = GFL_BOOL_CAST( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_BATTLE_HIO) );
    m_isBattleRareEffect = GFL_BOOL_CAST( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_BATTLE_RARE_EFFECT) );
    m_isBattleWeakEnemy = GFL_BOOL_CAST( FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::ENABLE_BATTLE_WEAK_ENEMY) );
  }
#endif // PM_DEBUG
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------

GameSys::GMEVENT_RESULT EventBattleCall::MainFunc( GameSys::GameManager* p_gman )
{
  // m_Optionの値により特定のシーケンスを飛ばしているので、順番変更は注意すること
  enum
  {
    SEQ_ENCOUNT_EFFECT_START_FLASH,
    SEQ_ENCOUNT_EFFECT_WAIT_DLL,
    SEQ_ENCOUNT_EFFECT_START,
    SEQ_ENCOUNT_EFFECT_WAIT,
    SEQ_FIELDMAP_CLOSE,
    SEQ_BATTLE_CALL,
    SEQ_BATTLE_READ_WAIT,
    SEQ_BATTLE_RET,
    SEQ_BATTLE_DELETE,
    SEQ_FIELDMAP_OPEN,
    SEQ_LOSE,

    SEQ_END,
  };

  switch ( m_Seq )
  {
    case SEQ_ENCOUNT_EFFECT_START_FLASH:

#if PM_DEBUG
      if( m_isBattleThrough )
      {
        // 外部で戦闘導入演出を再生していない場合はDLLを読み込みを待つ
        if( (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false )
        {
          // DLL読み込みを待つ
          if( p_gman->GetGameData()->GetEncountWork()->WaitLoadEncountEffectDllThread() )
          {
            // 戦闘導入演出スキップ
            m_Seq = SEQ_FIELDMAP_CLOSE;
          }
          break;
        }
        // 外部で戦闘導入演出を再生している場合はシーケンス再生を終了させる
        // DLLは既に読み込まれてい：る
        else
        {
          DemoLib::Sequence::SequenceViewSystem* pSequenceViewSystem = m_pEncountEffect->GetDemoFade()->GetSequence();

          if( pSequenceViewSystem )
          {
            pSequenceViewSystem->SetSystemSync( false );
            break;
          }

          m_Seq = SEQ_FIELDMAP_CLOSE;
          break;
        }
      }
#endif // PM_DEBUG

      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF ) == false &&
          (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false
        )
      {
        // @fix フラッシュ開始
        m_pEncountEffect->StartFlash();
      }
      m_Seq++;
      break;

    case SEQ_ENCOUNT_EFFECT_WAIT_DLL:

      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF ) == false &&
          (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false
        )
      {
        m_pEncountEffect->UpdateFlash();
      }

      if( (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false )
      {
        // @fix DLL読み込みを待つ
        if( p_gman->GetGameData()->GetEncountWork()->WaitLoadEncountEffectDllThread() == false )
        {
          break;
        }
      }

      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF ) == false &&
          (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false
        )
      {
        // 戦闘導入演出初期化
        gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

        Encount::EncountEffect::Description desc;
        desc.effectKind = static_cast<Encount::EncountEffect::EffectKind>( m_pParam->btlEffData.enceff_fld.eff_kind);
        m_pEncountEffect->Initialize( p_heap, desc);

        // 再生待機モードに設定
        m_pEncountEffect->SetSequenceStartupWaitMode( true);
      }
      m_Seq++;
      // break;

    case SEQ_ENCOUNT_EFFECT_START:
#if 0 //NMCat[3555] nijiのソルガレオ・ルナアーラ戦では不要
      if( BATTLE_PARAM_CheckBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_LEGEND_EX ) )
      {
        //ボックスが満タンかどうかのフラグを落とす
        BATTLE_PARAM_ResetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_BOXFULL );
        //経験値が入らない戦闘
        BATTLE_PARAM_SetNoGainMode( m_pParam );
      }
#endif

      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF) == false )
      {
        // 旧作でエンカウントエフェクトイベント内で行っていた処理はここで行います
        // BGM開始
        if( m_SkipBGMStart == false)
        {
          p_gman->GetGameData()->GetFieldSound()->StartBattleBGM( m_pParam->btlEffData.bgm_default );
        }

        //BTL_CTR_PROFILING_START

        if( (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false )
        {
          // 再生開始 (SequenceSystemの初期化から)
          m_pEncountEffect->Start( false );
        }

        GFL_DEBUG_TICK_START(EventBattleCallTick, TICK_EFFECT_LOAD, "TICK_EFFECT_LOAD");

        m_Seq = SEQ_ENCOUNT_EFFECT_WAIT;
        break;
      }

      if( (m_Option & EVBTL_CALLOPT_PLAY_BTL_BGM )){
        //エンカウントエフェクトスキップ時にサウンドだけはスタートさせる
        p_gman->GetGameData()->GetFieldSound()->StartBattleBGM( m_pParam->btlEffData.bgm_default );
      }

      m_Seq = SEQ_FIELDMAP_CLOSE;
      break;

    case SEQ_ENCOUNT_EFFECT_WAIT:

      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF) == false )
      {
        bool isStart     = m_pEncountEffect->IsStart( false );
        bool isEndFlash  = m_pEncountEffect->UpdateFlash();

        // 戦闘導入演出が再生できるようになるまで待つ
        if( (isStart == false) || (isEndFlash == false) )
        {
          break;
        }

        if( (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false )
        {
          // キャプチャーが必要ならリクエストを出す
          if( m_pEncountEffect->IsNeedFieldCapture())
          {
            m_pEncountEffect->RequestFieldCapture();
          }
        }
      }

      m_Seq = SEQ_FIELDMAP_CLOSE;
      break;

    case SEQ_FIELDMAP_CLOSE:

#if PM_DEBUG
      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF) == false &&
          (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false &&
          ( m_isBattleThrough == false )
        )
#else
      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF) == false &&
          (m_Option & EVBTL_CALLOPT_ENCEFF_EXTERNAL) == false
        )
#endif
      {
        GFL_DEBUG_TICK_START(EventBattleCallTick, TICK_EFFECT_SETUP, "TICK_EFFECT_SETUP");
        // 戦闘導入演出の再生前処理
        m_pEncountEffect->Setup();
        GFL_DEBUG_TICK_END(EventBattleCallTick, TICK_EFFECT_SETUP);

        // 戦闘導入演出の再生待機を解除
        m_pEncountEffect->SetSequenceStartupWaitMode( false);
      }

      GFL_DEBUG_TICK_END(EventBattleCallTick, TICK_EFFECT_LOAD);
      //BTL_CTR_PROFILING_END
      GFL_DEBUG_TICK_START(EventBattleCallTick, TICK_FIELD_CLOSE, "TICK_FIELD_CLOSE");

      ++m_Seq;

      if( m_Option & EVBTL_CALLOPT_SKIP_FIELD_CLOSE )
      {
        // 復帰時に設定するため、現在のロケーションを保持しておく
        m_playerLocation = *p_gman->GetGameData()->GetStartLocation();

        //FieldRoが残っている場合破棄
        p_gman->GetFieldResident()->TerminateFieldDLL();

        if( m_Option & EVBTL_CALLOPT_PLAY_BTL_BGM)
        {
          p_gman->GetGameData()->GetFieldSound()->StartBattleBGM( m_pParam->btlEffData.bgm_default );
        }
      }
      else
      {

        if ( m_pCallBackFieldClose )
        {
          m_pCallBackFieldClose( p_gman );
        }

        // 復帰時に設定するため、現在のロケーションを保持しておく
        m_playerLocation.worldId  = p_gman->GetFieldmap()->GetWorldID();
        m_playerLocation.zoneId   = p_gman->GetFieldmap()->GetZoneID();
        m_playerLocation.position = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
        m_playerLocation.quaternion = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;

        // クローズイベント
        EventFieldmapClose* p_event;
        p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
              p_gman->GetGameEventManager( ) );
        EventFieldmapClose::Desc desc;
        desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
        desc.isSkyboxKeep = true;
        p_event->SetDesc( desc );

        /*
         * フェードオプションが明示指定された場合はブラックフェードを行う
         * 原則、エンカウントエフェクトでフェードを行う
         */
        if( m_Option & EVBTL_CALLOPT_SKIP_ENCEFF &&
            m_Option & EVBTL_CALLOPT_WITH_FADEOUT )
        {
          p_event->SetFadeCallBackDefault();
        }
        else
        {
          // NMCat[351] 下画面のみのフェードリクエストをしていたが、
          // その処理は戦闘導入演出有りのときに必要なことなので、戦闘導入演出開始時に任せる
          // ここに入った場合は、外部でフェードをしている前提なので、FieldmapCloseにはフェードをさせない
          p_event->SetFadeCallBack( NULL, NULL, NULL );
        }

      }

#if PM_DEBUG
      if( m_isBattleThrough )
      {
        //必ず勝ちにする
        m_pParam->result = BTL_RESULT_WIN;
        m_Seq = SEQ_BATTLE_RET;
        break;
      }
#endif // PM_DEBUG

      break;

    case SEQ_BATTLE_CALL:

      {
        GFL_DEBUG_TICK_END(EventBattleCallTick, TICK_FIELD_CLOSE);
        GFL_DEBUG_TICK_END(EventBattleCallTick, TICK_EFFECT_TOTAL);
#ifdef DEBUG_ONLY_FOR_takahashi_tomoya
        gfl2::debug::AddPrintFlag( GFL_DEBUG_TICK_DUMP_PRINT_FLG );
        GFL_DEBUG_TICK_DUMP_ALL(EventBattleCallTick);
        gfl2::debug::ResetPrintFlag();
#endif

        //戦闘突入前のパーティメンバー数を覚えておく
        m_beforePartyPokeNum = p_gman->GetGameData()->GetPlayerParty()->GetMemberCount();

#if PM_DEBUG
        //バトルをHIOを有効にして起動
        if( m_isBattleHio )
        {
          m_pParam->btlvDebugParam.isHioEffResource = true;
          m_pParam->btlvDebugParam.isHioBgResource = true;
        }
        if( m_isBattleRareEffect )
        {
          m_pParam->btlvDebugParam.mForceRareEffect = true;
        }
        if( m_isBattleWeakEnemy )
        {
          //敵を強制的に弱くする
          gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
          pml::pokepara::PokemonParam *pp = GFL_NEW( p_heap ) pml::pokepara::PokemonParam( p_heap, (MonsNo)MONSNO_MANGUUSU1, 1, 0 );
          m_pParam->party[1]->Clear();
          m_pParam->party[1]->AddMember(*pp);
          if( m_pParam->rule == BTL_RULE_DOUBLE )
          {
            m_pParam->party[1]->AddMember(*pp);
          }
          GFL_SAFE_DELETE(pp);
        }
#endif // PM_DEBUG

        btl::BATTLE_PROC_PARAM battleProcParam;
        battleProcParam.pAppMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE);
        battleProcParam.pAppDeviceMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE);
        battleProcParam.pGameManager = p_gman;
        battleProcParam.pSetupParam = m_pParam;
        EventBattleDll * dll_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleDll>(p_gman->GetGameEventManager());
        dll_event->SetParameter( &battleProcParam );
      }

      //レコード追加
      {
        switch( m_pParam->competitor ){
        case BTL_COMPETITOR_WILD:
          Savedata::IncRecord(Savedata::Record::RECID_BTL_ENCOUNT);
          break;
        case BTL_COMPETITOR_TRAINER:
          Savedata::IncRecord(Savedata::Record::RECID_BTL_TRAINER);
          Savedata::IncRecord(Savedata::Record::RECID_DAYCNT_TRAINER_BATTLE);
          break;
        }
      }

      ++m_Seq;
      break;

    case SEQ_BATTLE_READ_WAIT:
      if( GameSys::IsFinishedLoadBattleDll())
      {
        EventProcessCall::CallRoEventCall<btl::BattleProc>(p_gman);

        ++m_Seq;
      }
      break;

    case SEQ_BATTLE_RET:
    {
#if PM_DEBUG
      //デバッグ処理・LR押しっぱなしで強制負け
      gfl2::ui::Button * button = p_gman->GetUiDeviceManager()->GetButton(0);
      if ( button->GetHold() == (gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R) )
      {
        m_pParam->result = BTL_RESULT_LOSE;
      }
#endif

      // @fix EVENT_DEVICEの空きを確保するため、ここで戦闘導入演出を破棄する
      // EventBattleCall、スクリプト、どちらで初期化したかに関わらず、
      // 戦闘導入演出を使用していればここでインスタンスの破棄処理を呼ぶ
      if( (m_Option & EVBTL_CALLOPT_SKIP_ENCEFF) == false )
      {
        if( m_pEncountEffect)
        {
          m_pEncountEffect->Terminate();
          m_pEncountEffect = NULL;
        }
      }

      // EventBattleCall、スクリプト、どちらで初期化したかに関わらず、
      // シーケンス関連のDLLは必ず破棄する
      p_gman->GetGameData()->GetEncountWork()->DisposeEncountEffectDll();
      p_gman->GetGameData()->GetEncountWork()->DisposeEncountEffectDllThread();


      // ポケモンパラメータの内容を反映。
      EventBattleReturn* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleReturn > ( p_gman->GetGameEventManager( ) );

      EventBattleReturn::SetupParam param;
      param.weather = m_weather;

      p_event->SetBattleParam( m_pParam, m_cpZoneDataLoader );
      p_event->SetSetupParam( param );
    }

      ++m_Seq;

    return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

    case SEQ_BATTLE_DELETE:
      {
        //結果反映
        bool is_lose = this->ReflectBattleResult( p_gman );

#if 0 //NMCat[3555] nijiのソルガレオ・ルナアーラ戦では不要
        if(m_pParam->result != BTL_RESULT_CAPTURE && BATTLE_PARAM_CheckBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_LEGEND_EX ) )
        {
          //捕獲していない場合、イベント戦専用のトレイのオープンを取り消すリクエストを発行
          //必要なくなったので削除
        }
#endif
        //ワーク削除
        this->RemoveDefault();

        //ゲームオーバーフローでFieldRoの関数を使うために、ここでDLLを読み込んでおく
        p_gman->GetFieldResident()->InitializeFieldDLL( p_gman->GetAsyncFileManager() );

        if ( !m_NotScriptEvent ){
          //スクリプトからの呼び出し

          if( is_lose && !m_NoLoseBattle ){
            // スクリプト起動の場合、負けたらTrainerLose処理へ行く
            p_gman->GetGameData()->GetFieldSound()->EndBattleBGMForGameOver();
            m_Seq = SEQ_END;
          }else{
            m_Seq = SEQ_FIELDMAP_OPEN;
          }
        }
        else{
          //非スクリプト経由呼び出し
          if ( is_lose ){
            m_Seq = SEQ_LOSE; //敗戦処理をこの場で行う
          }else{
            m_Seq = SEQ_FIELDMAP_OPEN;
          }
        }
      }

      ///-----------------------------------------------------------------------
      return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

    case SEQ_FIELDMAP_OPEN:
      if ( (m_Option & EVBTL_CALLOPT_SKIP_FIELD_OPEN) == false )
      {
        EventFieldmapOpen* p_event;

        FieldmapDesc desc;
        desc.openMode = FM_OPEN_MODE_APP_RECOVER;
        desc.playerLocation = m_playerLocation;

        // オープンイベント
        p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
                p_gman->GetGameEventManager( ) );
        p_event->SetDesc( desc);

        // スクリプトから呼び出された場合、フェードはスクリプトから行う
        if( (m_NotScriptEvent) && (m_NoFadeIn == false) )
        {
          p_event->SetFadeCallBackDefault();
        }
        else
        {
          p_event->SetFadeCallBack( NULL, NULL, NULL );
        }
      }
      if( m_Option & EVBTL_CALLOPT_SKIP_BGM_RECOVER )
      {
        u32 win_bgm_long = m_pParam->btlEffData.bgm_win;
        u32 win_bgm_short  = BattleReturnBGM::GetWinBGM(win_bgm_long);
        p_gman->GetGameData()->GetFieldSound()->EndBattleBGMForSkip( win_bgm_long, win_bgm_short ); // 勝利BGMの継続処理
      }else{
        p_gman->GetGameData()->GetFieldSound()->EndBattleBGM( m_ReturnBGMID ); // フィールドBGMを復元
      }
      m_Seq = SEQ_END;
      break;

    case SEQ_LOSE:
      p_gman->GetGameData()->GetFieldSound()->EndBattleBGMForGameOver();

      // 全滅処理へ
      GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventNormalLose > ( p_gman->GetGameEventManager( ) );
      m_Seq = SEQ_END;
      break;

    case SEQ_END:
      return GameSys::GMEVENT_RES_FINISH;

    default:
      break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventBattleCall::EndFunc( GameSys::GameManager* p_gman )
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  デフォルト設定で呼び出し(プログラム呼び出し用)
 */
//-----------------------------------------------------------------------------
void EventBattleCall::SetUpWildPoke( GameSys::GameManager* p_gman, Encount::PokeSet& poke_set, u32 call_option, const pml::PokeParty * cp_party /* = NULL */  )
{
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  GameSys::GameData* p_gdata = p_gman->GetGameData();
  btl::bg::bgsys_far_typeID farType = poke_set.GetBgFarTypeId();
  btl::bg::bgsys_near_typeID nearType = poke_set.GetBgNearTypeId();
  Field::EventWork* pEventWork = p_gdata->GetEventWork();

  // SBTS[1104] 釣りのアトリビュートに生じる不具合対処
  if (call_option & EVBTL_CALLOPT_ATTR_POKESET)
  {
    m_Attr = poke_set.GetEncountAttr();
  }

  SetUpFieldSituation( &m_BtlFieldSit, p_gman, &m_cpZoneDataLoader, m_Attr, farType, nearType );

  {
    BtlRule rule = BTL_RULE_SINGLE;

    GFL_ASSERT( m_BtlEnemyParty == NULL ); //@check 130329 iwasawa 現実装上あり得ないので特にケアはしない
    m_BtlEnemyParty = GFL_NEW( p_heap ) pml::PokeParty( p_heap );

    if (cp_party)
    {
      // 既に生成済みのパーティがある場合は、受け渡す
      // ※AddMemberでデータがコピーされるため、元データへの影響はない
      u32 cnt = cp_party->GetMemberCount();
      for( u32 i=0; i < cnt; i++ )
      {
        const pml::pokepara::PokemonParam* cp_pp;
        cp_pp = cp_party->GetMemberPointerConst(i);
        m_BtlEnemyParty->AddMember( *cp_pp );
      }
    }
    else
    {
      //セットアップ済みのエンカウントデータからPokePartyを取得
      poke_set.GetPokeParty( p_heap, m_BtlEnemyParty );
    }

    // レコード レアがいたぶんだけ加算
    {
      u32 enemy_count = m_BtlEnemyParty->GetMemberCount();
      for (u32 i = 0; i < enemy_count; i++ )
      {
        const pml::pokepara::PokemonParam* cp_pp = m_BtlEnemyParty->GetMemberPointerConst(i);
        if (cp_pp->IsRare())
        {
          Savedata::IncRecord(Savedata::Record::RECID_RARE_COLOUR_ENC_COUNT);
        }
      }
    }
   
    //バトルパラメータをセットアップ
    BATTLE_SETUP_Wild( &m_DefaultParam, p_gman, m_BtlEnemyParty, &m_BtlFieldSit, rule, poke_set.GetBattleEffectId(), &poke_set, p_heap );
  }

  m_pParam = &m_DefaultParam;


  //セットアップ済みのエンカウントデータからDefaultPowerUpDescを取得
  poke_set.GetDefaultPowerUp( p_heap, m_pParam->partyParam[ BTL_CLIENT_ENEMY1 ].defaultPowerUpDesc );


  // 専用フラグがONなら捕獲率を100%にする
  if( pEventWork->CheckEventFlag( SYS_FLAG_ROAD1_HOKAKU_PERFECT ) )
  {
    BATTLE_PARAM_SetMustCaptureMode( m_pParam );
  }


  //特定ゾーンで特定条件で捕獲を制限する
  {
    u16 zoneID = p_gman->GetGameData()->GetNowZoneID();

    // 捕獲NGゾーンか
    if( m_cpZoneDataLoader->CheckCaptureNGZone( zoneID, pEventWork) )
    {
      BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_SIREN_BATTLE);   
    }

    // 保護区か
    if( ZoneDataAccessor::IsSanctuaryZone( zoneID))
    {
      BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_IN_POKE_SANCTUARY);
    }
  }

  //野生戦のモードフラグセット
  {
    Encount::WildEncMode mode = poke_set.GetWildEncMode();
    switch( mode ){
    case Encount::WILDENC_MODE_SYMBOL: //通常話しかけ
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_WILD_TALK);
      break;
    case Encount::WILDENC_MODE_NUSI: //ぬし
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_SIREN_BATTLE);
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_VS_NUSI);
      break;
    case Encount::WILDENC_MODE_LEGEND: //伝説
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_LEGEND);
      break;
    case Encount::WILDENC_MODE_LEGEND_IMMORTAL:  //伝説無限
      //NMCat[3555]【ev2840】ソルガレオ/ルナアーラ戦で経験値が入ることについて
      //nijiではソルガレオ・ルナアーラ用とする。WILDENC_MODE_LEGENDとの差分は経験値が入らない、倒したメッセージが出ない、落ちてるお金を拾わない4点
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_LEGEND_EX);
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_NO_WILD_MESSAGE);// 野生って言わない
      break;
    case Encount::WILDENC_MODE_LEGEND_NOLOSE: //伝説&全滅無し sangoでは初回デオキシスのみ
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_LEGEND);
      call_option |= EVBTL_CALLOPT_NO_LOSE;  //負けないバトルフラグを立てておく

      //D要望：設定上手持ちレックウザがバトル直前のデモでメガなので、レックウザをメガ状態でバトルをはじめたい
      //SBTS[2133]の対応でバトル内でメガシンカさせるようにしたためここでは進化させない
      //SetDeokisisuFirstBattleParty( p_gman->GetGameData()->GetPlayerStatus(), m_DefaultParam.party[BTL_CLIENT_PLAYER] );
      break;
    case Encount::WILDENC_MODE_BEAST: //ビースト
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_VS_ULTRA_BEAST);
      break;
    }

    //釣りチェック
    if( poke_set.IsFishing() ){
      // フェードなし
      m_NoFadeIn = true;
      // 釣りフラグを設定
      BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_FISHING);
    }
  }

  //プレイヤーのフォームによる分岐ステータスをセット
  this->SetUpPlayerFormStatus( p_gman );

  m_SetupDefault = true;
  m_Option = call_option;

  //スクリプトからのコールでない
  if( m_Option & EVBTL_CALLOPT_NOT_SCRIPT ){
    m_NotScriptEvent = true;
  }
  // 負けないバトル
  if (m_Option & EVBTL_CALLOPT_NO_LOSE)
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_NO_LOSE );
    m_NoLoseBattle = true;
  }
  // バトルBGM再生スキップ
  if (m_Option & EVBTL_CALLOPT_SKIP_BTL_BGM)
  {
    m_SkipBGMStart = true;
  }
  // バトルBGMの状態維持
  if (m_Option & EVBTL_CALLOPT_SKIP_BGM_RECOVER)
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_WIN_BGM_CONTINUE );
  }
  // お手入れスキップ
  if (m_Option & EVBTL_CALLOPT_SKIP_REFLE)
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_OTEIRE_DISABLE );
  }
  // 規定ターンで相手が逃げる
  if (m_Option & EVBTL_CALLOPT_RUN_ENEMY_TURN )
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_VS_FIRST_UTUROIDO );
  }  
  // ポケモンに載ってる状態にする
  if( m_Option & EVBTL_CALLOPT_POKE_RIDE ){
    //BATTLE_PARAM_SetBtlStatusFlag(m_pParam, BTL_STATUS_FLAG_POKE_RIDE);
  }
  //momiji追加　合体ネクロズマ、光りドラゴンで使用。捕獲できない。
  if (m_Option & EVBTL_CALLOPT_CAN_NOT_CAPTURE)
  {
    BATTLE_PARAM_SetBtlStatusFlag(m_pParam, BTL_STATUS_FLAG_NO_CAPTURE);
  }
  //momiji追加　光りドラゴンで使用。オーラを纏う設定にする。
  if (m_Option & EVBTL_CALLOPT_AURA)
  {
    BATTLE_PARAM_SetBtlStatusFlag(m_pParam, BTL_STATUS_FLAG_VS_NEKUROZUMA);//名前がオーラっぽくないけど、momijiで追加したオーラを纏う設定の定義
  }
  //momiji追加　合体ネクロズマ、光ドラゴンで使用。やせいの～メッセージを使用しない
  if (m_Option & EVBTL_CALLOPT_NO_WILD_MESSAGE)
  {
    BATTLE_PARAM_SetBtlStatusFlag(m_pParam, BTL_STATUS_FLAG_NO_WILD_MESSAGE);
  }

  // 乱入設定
  if( pEventWork->CheckEventFlag( SYS_FLAG_SIREN1_CLEAR_NORMAL ) == true )
  {
    if( (m_Option & EVBTL_CALLOPT_NOT_INTRUDE) == false )
    {
      // @fix GFNMCat[1268] シンボルエンカウント時にエンカウントデータが未設定状態で乱入が呼ばれていたのでデータの有無でフックする
      if( poke_set.IsEncountDataExist())
      {
        m_pParam->intrudeParam.intrudeEnableFlag = true;
        m_pParam->intrudeParam.intrudePokeParamGenerator = &poke_set;
      }
    }
  }

  // 進化設定
  PokeTool::SetupEvolveSituation( &m_pParam->evolveSituation, p_gman, false, m_weather);
}

//----------------------------------------------------------------------------
/**
 *  @brief  トレーナー戦
 *  @param  p_gameman ゲームマネージャ
 *  @param  param     トレーナーバトル設定パラメータ
 *  @param  call_option EVBTL_CALLOPT_NONE他 イベントシーケンス制御オプション
 */
//-----------------------------------------------------------------------------
void EventBattleCall::SetUpTrainer( GameSys::GameManager* p_gman, const TrainerBattleParam& param, u32 call_option )
{
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
  s8 trPokeLvAdjust = 0;

  // トレーナーIDの保存
  m_trainerID0 = param.trID0;
  m_trainerID1 = param.trID1;

  // 拡張設定フラグの保存
  m_ExFlag = param.exFlag;

  // シーケンス制御オプションの保存
  m_Option = call_option;

  // バトルパラメータのデフォルト値を設定
  {
    m_BtlEnemyParty = NULL;

    //トレーナー手持ちポケモンレベル調整
    if( m_ExFlag & TRAINER_BTL_FLAG_LV_UP5 ){
      trPokeLvAdjust = 5; //Lv5アップ
    }else if( m_ExFlag & TRAINER_BTL_FLAG_LV_UP10 ){
      trPokeLvAdjust = 10;  //Lv10アップ
    }else if( m_ExFlag & TRAINER_BTL_FLAG_LV_UP20 ){
      trPokeLvAdjust = 20;  //Lv20アップ
    }else if( m_ExFlag & TRAINER_BTL_FLAG_LV_DOWN10){
      trPokeLvAdjust = -10; //Lv10ダウン
    }
  }

  SetUpFieldSituation( &m_BtlFieldSit, p_gman, &m_cpZoneDataLoader, m_Attr, param.bgFarType, param.bgNearType, trPokeLvAdjust );

  switch( param.rule )
  {
  case BTL_RULE_ROYAL:
    //break throw
  case BTL_RULE_DOUBLE:
    if( param.partnerID != trainer::TRID_NULL )
    { // AIマルチ パートナーがいて敵も2人いる
      BTL_SETUP_Trainer_Multi( &m_DefaultParam, p_gman, param.partnerID, param.trID0, param.trID1, &m_BtlFieldSit, param.rule, p_heap );
    }
    else if( param.trID1 != trainer::TRID_NULL )
    { // タッグ パートナーがいないけど敵は2人いる
      BTL_SETUP_Trainer_Tag( &m_DefaultParam, p_gman, param.trID0, param.trID1, &m_BtlFieldSit, param.rule, p_heap );
    }
    else
    {
      // ダブル
      BTL_SETUP_Trainer( &m_DefaultParam, p_gman, param.trID0, &m_BtlFieldSit, param.rule, p_heap );
    }
    break;
  case BTL_RULE_SINGLE:
    { // シングル
      BTL_SETUP_Trainer( &m_DefaultParam, p_gman, param.trID0, &m_BtlFieldSit, param.rule, p_heap );
    }
    break;

  default : GFL_ASSERT_STOP(0); //@fix ruleが不正なケースの適切なフォローが無理なのでストップ iwasawa 130329
  }


  // 要望対応：シナリオ上のバトル施設ロイヤル戦は戦闘可能なポケモンの内、先頭に近いポケモン1匹のみのパーティーで戦う
  // nijiではシナリオ上で呼ばれるロイヤル戦が施設内でしかない かつ 1箇所のみ なので一律この設定にする
  // アッパーで拡張するなら、手持ちを外部から渡せるようにするとか、オプション指定とかにする
  if( param.rule == BTL_RULE_ROYAL )
  {
    pml::PokeParty* pPlayerParty = p_gman->GetGameData()->GetPlayerParty();

    u32 index = pPlayerParty->GetMemberTopIndex( pml::PokeParty::POKE_SEARCH_TYPE_BATTLE_ENABLE );
    GFL_ASSERT_STOP( index != pml::PokeParty::MEMBER_INDEX_ERROR ); // これは有り得ないはずなのでストップ

    // パーティーに上書き
    pml::PokeParty* pScenarioRoyalParty = GFL_NEW( p_heap->GetLowerHandle() ) pml::PokeParty( p_heap->GetLowerHandle() );
    pScenarioRoyalParty->AddMember( *(pPlayerParty->GetMemberPointer( index )) );
    m_DefaultParam.party[ BTL_CLIENT_PLAYER ]->CopyFrom( *pScenarioRoyalParty );
    GFL_DELETE( pScenarioRoyalParty );

    // 施設の設定にする
    m_DefaultParam.competitor = BTL_COMPETITOR_INST;
  }


  m_pParam = &m_DefaultParam;

  //---------------------------
  /// バトル設定の変更
  
  //プレイヤーのフォームによる分岐ステータスをセット
  this->SetUpPlayerFormStatus( p_gman );

  //バトルエフェクト指定があったら上書き
  if( param.btlEffId != BATTLE_EFFECT_DEFAULT ){
    BATTLE_PARAM_SetBattleEffect( m_pParam, param.btlEffId );
  }

  // 進化設定
  PokeTool::SetupEvolveSituation( &m_pParam->evolveSituation, p_gman, false, m_weather);

  if( m_ExFlag == TRAINER_BTL_FLAG_NONE ){
    m_SetupDefault = true;  //特殊フラグがなければここで終了
    return;
  }
  //---------------------------
  //各種フラグ設定

  //負けない(全滅処理に移行しない)バトル
  if ( m_ExFlag & TRAINER_BTL_FLAG_NO_LOSE || m_Option & EVBTL_CALLOPT_NO_LOSE )
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_NO_LOSE );
    m_NoLoseBattle = true;  //フラグを立てておく
  }
  // バトルBGM再生スキップ
  if (m_ExFlag & TRAINER_BTL_FLAG_SKIP_BTLBGM || m_Option & EVBTL_CALLOPT_SKIP_BTL_BGM)
  {
    m_SkipBGMStart = true;
  }
  // バトルBGMからフィールド曲に戻す処理をスキップ
  if ( m_ExFlag & TRAINER_BTL_FLAG_SKIP_BGM_RECOVER ){
    m_Option |= EVBTL_CALLOPT_SKIP_BGM_RECOVER;
  }
  // バトルBGMの状態維持
  if (m_Option & EVBTL_CALLOPT_SKIP_BGM_RECOVER)
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_WIN_BGM_CONTINUE );
  }
  // お手入れスキップ
  if (m_ExFlag & TRAINER_BTL_FLAG_SKIP_REFLE || m_Option & EVBTL_CALLOPT_SKIP_REFLE)
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_OTEIRE_DISABLE );
  }
  //フィールド破棄をスキップ
  if ( m_ExFlag & TRAINER_BTL_FLAG_SKIP_FIELD_CLOSE )
  {
    m_Option |= EVBTL_CALLOPT_SKIP_FIELD_CLOSE;
    m_Option |= EVBTL_CALLOPT_SKIP_ENCEFF;
  }
  //フィールドオープンをスキップ
  if ( m_ExFlag & TRAINER_BTL_FLAG_SKIP_FIELD_OPEN )
  {
    m_Option |= EVBTL_CALLOPT_SKIP_FIELD_OPEN;
  }
  
  //エンカウントエフェクトをスキップ
  if ( m_ExFlag & TRAINER_BTL_FLAG_SKIP_EFFECT )
  {
    m_Option |= EVBTL_CALLOPT_SKIP_ENCEFF;
  }
  //`エフェクトスキップ時のBGMとフェードの指定
  if ( m_Option & EVBTL_CALLOPT_SKIP_ENCEFF )
  {
    if ( m_ExFlag & TRAINER_BTL_FLAG_PLAY_BTLBGM )
    {
      m_Option |= EVBTL_CALLOPT_PLAY_BTL_BGM;
    }
    if ( m_ExFlag & TRAINER_BTL_FLAG_PLAY_FADEOUT )
    {
      m_Option |= EVBTL_CALLOPT_WITH_FADEOUT;
    }
  }
  // さかさバトル(効果抜群判定が逆になる特殊バトル)
  if ( m_ExFlag & TRAINER_BTL_FLAG_SAKASA_BATTLE )
  {
    BATTLE_PARAM_SetSakasaBattle( m_pParam );
  }

  // 賞金調整
  if( m_ExFlag & TRAINER_BTL_FLAG_MONEY_UP50 ){
    BATTLE_PARAM_SetMoneyRate( m_pParam, 1.5f );  //50%up = 1.5倍
  }


  //代表ビースト戦闘
  if( m_ExFlag & TRAINER_BTL_FLAG_BOSS_BEAST ){
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_VS_BOSS );
  }
  // AIがZ技をつかいやすくなる
  if (m_ExFlag & TRAINER_BTL_FLAG_TEND_USE_ZPOWER || m_Option & EVBTL_CALLOPT_TEND_USE_ZPOWER )
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_AI_TEND_USE_ZPOWER );
  }

  //momiji追加　トレーナーかみつかい、イベントのハウはボールを投げない。
  if (m_ExFlag & TRAINER_BTL_FLAG_NO_BALL )
  {
    BATTLE_PARAM_SetBtlStatusFlag( m_pParam, BTL_STATUS_FLAG_TRAINER_NO_BALL );
  }

  //momiji追加　トレーナーかみつかいはボールを戻さない。
  if (m_ExFlag & TRAINER_BTL_FLAG_NO_RETURN_BALL)
  {
    BATTLE_PARAM_SetBtlStatusFlag(m_pParam, BTL_STATUS_FLAG_TRAINER_NO_RETURN_BALL);
  }

  m_SetupDefault = true;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 自機フォルム状態をバトルに通知
 *
 * @param p_gman ゲームマネージャーへのポインタ
 */
/* ------------------------------------------------------------------------- */
void EventBattleCall::SetUpPlayerFormStatus( GameSys::GameManager* p_gman )
{
  // 内部でフィールドマップがあるかの判定も行っている
  RIDE_POKEMON_ID id = EventPokemonRideTool::GetPokemonRideOnID( p_gman);

  // 水上ライドか
  if( EventPokemonRideTool::IsNaminori( id ) )
  {
    BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_SWIM );
    BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_RIDEWARE_SWIM );
  }
  // 陸上ライドか
  else if(EventPokemonRideTool::IsGroundRide( id ))
  {
    BATTLE_PARAM_SetBtlStatusFlag(m_pParam,BTL_STATUS_FLAG_RIDEWARE_LAND );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  戦闘戻り時のBGM指定(全滅時は全滅フロー用の遷移が走るため勝利時のみ有効)
 *  @note 呼び出さない、あるいは、Sound::SOUND_ITEM_ID_NONEが指定された場合(←意味はないが)なら素直にPushされた曲に戻る
 */
//-----------------------------------------------------------------------------
void EventBattleCall::SetReturnBGMID( u32 bgm_id  )
{
  if( bgm_id != Sound::SOUND_ITEM_ID_NONE &&
      bgm_id != SMID_NULL ){
    m_ReturnBGMID = bgm_id;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  デフォルト設定の破棄処理
 */
//-----------------------------------------------------------------------------
void EventBattleCall::RemoveDefault( void )
{
  if ( m_SetupDefault == false )
  {
    return;
  }

  if ( m_BtlEnemyParty )
  {
    GFL_DELETE( m_BtlEnemyParty );
    m_BtlEnemyParty = NULL;
  }
  BATTLE_SETUP_Clear( &m_DefaultParam );

  m_SetupDefault = false;
}

//
//----------------------------------------------------------------------------
/**
 *  @brief  バトルパラメータを登録
 *
 *  @param  p_param
 */
//-----------------------------------------------------------------------------
void EventBattleCall::SetUpBattleParam( BATTLE_SETUP_PARAM * p_param )
{
  m_pParam = p_param;

  m_SetupDefault = false;
}

/* -------------------------------------------------------------------------*/
/**
 * @brief 戦闘結果をgamedata等に反映する
 */
/* -------------------------------------------------------------------------*/
bool EventBattleCall::ReflectBattleResult( GameSys::GameManager* p_gman )
{
  GameSys::GameData* p_gdata = p_gman->GetGameData( );
  bool is_lose_result_overwrite = false;

  //全滅したかどうか判定取得
  bool is_gameover = IsGameoverResult( m_pParam->result, p_gdata->GetPlayerParty(), &is_lose_result_overwrite );

  //最新の戦闘結果をゲームデータにとっておく
  BATTLE_RESULT_DATA* data = p_gdata->GetLastBattleResultData();

  /*
   * Skyバトル特殊処理 BTS3364,GFBTS1030 対策
   * 
   * Skyバトルでは LOSE/DROWでも飛行/浮遊以外の手持ちが生き残っている場合があり
   * この時は勝負には負けているが全滅にしない
   *
   * 全滅にはしないが勝負には負けているため、イベントスクリプト側でリベンジは可能にするため
   * スカイバトルにおいて「勝負に負けたが全滅しなかった」特殊コードを返し
   * sky_battle_scr.p でのみ判定に利用することにする
   */
  if( is_lose_result_overwrite != false ){
    data->result = BTL_RESULT_LOSE_NOT_GAMEOVER_FOR_SKY; //戦闘結果の上書き
  }else{
    data->result = m_pParam->result;
  }
  //全力技を使ったか？
  data->b_zenryoku_waza_used = m_pParam->zenryokuWazaUsedFlag;

  //掛かったターン数を返す
  data->turnNum = m_pParam->kenteiResult.TurnNum;


  // 一定ターン以上かかっていたらサカサポイント0、SPフラグ無し
  if( data->turnNum >= SAKASA_BATTLE_PENALTY_TURN )
  {
    data->sakasaPoint = 0;
    data->sakasaSpFlag = false;
  }
  else
  {
    // サカサポイントの計算　効果抜群 + 1  効果なし、今一つ - 1
    if( m_pParam->kenteiResult.VoidAtcNum + m_pParam->kenteiResult.ResistAtcNum > m_pParam->kenteiResult.WeakAtcNum )
      data->sakasaPoint = 0;
    else
      data->sakasaPoint = m_pParam->kenteiResult.WeakAtcNum - m_pParam->kenteiResult.VoidAtcNum - m_pParam->kenteiResult.ResistAtcNum;

    // 瀕死が0ならサカサバトルSPフラグを立てる
    if( m_pParam->kenteiResult.LosePokeNum == 0 )
      data->sakasaSpFlag = true;
    else
      data->sakasaSpFlag = false;
  }

  //野生戦の場合バトルの結果によってレコード追加
  if( m_pParam->competitor == BTL_COMPETITOR_WILD ){

    if( data->result == BTL_RESULT_RUN_ENEMY){
      Savedata::IncRecord(Savedata::Record::RECID_NIGERARETA_FLAG);
    }
    if( data->result == BTL_RESULT_RUN){
      Savedata::IncRecord(Savedata::Record::RECID_ESCAPE);
    }
  }

  // メガ進化を見たかどうかフラグの書き戻し
  {
    p_gdata->GetMisc()->SetMegaSeeFlag( m_pParam->megaFlags );
  }

  // TVナビデータを保存
  data->tvNaviData = m_pParam->tvNaviData;

  // トレーナー戦で負けなかった場合は対戦フラグを立てる
  if( EventBattleCall::IsLoseTrainerResult( data->result) == false &&
      m_pParam->competitor == BTL_COMPETITOR_TRAINER
    )
  {
    p_gdata->GetEventWork()->SetEventFlag( TR_FLAG_AREA_START + m_trainerID0);

    // @fix GFNMCat[4796] ダブルバトル対応。無効値でなければセットする。対戦フラグは動作モデルの復帰でも参照されているのでこのタイミングで行っている
    if( m_trainerID1 != trainer::TRID_NULL )
    {
      p_gdata->GetEventWork()->SetEventFlag( TR_FLAG_AREA_START + m_trainerID1);
    }
  }

  if( m_pParam->rule == BTL_RULE_ROYAL)
  {
    // レコード
    {
      Savedata::IncRecord(Savedata::Record::RECID_BATTLEROYAL);

      // ロイヤル勝利 (clientRankingが0なら勝利)
      if( m_pParam->battleRoyalResult.clientRanking[BTL_CLIENT_PLAYER] == 0)
      {
        Savedata::IncRecord(Savedata::Record::RECID_BATTLEROYAL_WIN);
      }
    }
  }

  // 勝ち・負けを返す。
  return is_gameover;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドクローズ時に呼ばれるコールバックを設定
 *
 * @param p_func
 */
/* ------------------------------------------------------------------------- */
void EventBattleCall::SetCallBackFieldClose( CALLBACK_FUNC p_func )
{
  m_pCallBackFieldClose = p_func;
}

void EventBattleCall::SetZoneDataLoader( const ZoneDataLoader * cp_zone_loader )
{
  m_cpZoneDataLoader = cp_zone_loader;
}

/* ------------------------------------------------------------------------- */
/**
 * @brief フィールドシチュエーション設定
 *
 * @param p_gman ゲームマネージャーへのポインタ
 */
/* ------------------------------------------------------------------------- */
void SetUpFieldSituation( BTL_FIELD_SITUATION* outSit, GameSys::GameManager* p_gman, const ZoneDataLoader ** cpp_zone_loader, Attr in_attr, btl::bg::bgsys_far_typeID farType, btl::bg::bgsys_near_typeID nearType, s8 trPokeLvAdjust )
{
  // 未指定ならグローバルのものを使う
  if ((*cpp_zone_loader) == NULL)
  {
    *cpp_zone_loader = p_gman->GetGameData()->GetFieldZoneDataLoader();
  }
  
  ZoneID zoneID = p_gman->GetGameData()->GetNowZoneID();
  const ZoneDataLoader* cp_zone_loader = *cpp_zone_loader;
  ZoneDataAccessor zone_accessor( cp_zone_loader->GetZoneData( zoneID));

  //構造体初期化
  BATTLE_SETUP_FIELD_SITUATION_Init( outSit );

  // 地名ID（なつき度計算用）
  outSit->place_id = zone_accessor.GetPlaceLabelID();

  //バトル天候の初期化
  outSit->weather = GetBattleWeather( p_gman );

  //トレーナー戦専用ポケモンレベル調整値
  outSit->trPokeLvAdjust = trPokeLvAdjust;
  
  if( p_gman->GetFieldmap() == NULL ){
    //外背景がNONEならゾーンデータから取得
    //nearType==NONEだった場合はGetComponentData()が補完してくれるので問題ない
    if( farType == btl::bg::FAR_TYPE_NONE ){
      farType = static_cast<btl::bg::bgsys_far_typeID>( zone_accessor.GetBattleFarBGID());  //ゾーンデータから取得
    }
    btl::GetBgComponentData( farType, nearType, &(outSit->bgComponent) );
    return;
  }

  //フィールドがあるときのみ
  MoveModel::FieldMoveModelPlayer* p_player = p_gman->GetFieldmap()->GetPlayerCharacter();

  Attr now_attr = in_attr;
  if (now_attr == ATTR_ERROR)
  {
    now_attr = p_player->GetGroundAttributeLastSuccesful();
  }

  //プレイヤーの向きを初期化
  f32 playerRotateRadianY = p_player->GetCharaDrawInstance()->GetRotationQuat().QuaternionToRadianY();
  outSit->player_rotate = gfl2::math::ConvRadToDeg( playerRotateRadianY );
  
  //ダイブボール判定のために波乗り状態を取得
  
  //バトル背景パラメータセット
  {
    //指定がなければデフォルト値を取得
    if( farType == btl::bg::FAR_TYPE_NONE ){
      farType = static_cast<btl::bg::bgsys_far_typeID>( zone_accessor.GetBattleFarBGID());  //ゾーンデータから取得
    }
    if( nearType == btl::bg::NEAR_TYPE_NONE ){
      nearType = static_cast<btl::bg::bgsys_near_typeID>( Attribute::GetBattleNearBGID( now_attr, zone_accessor.GetAttributeBgNearPattern()));  //アトリビュートデータから取得
    }
    btl::GetBgComponentData( farType, nearType, &(outSit->bgComponent) );
  }

  outSit->sky_type = p_gman->GetSkybox()->GetSkyType();
  outSit->is_cloud_enable = p_gman->GetSkybox()->IsEnableParts( System::Skybox::Skybox::PARTS_TYPE_CLOUD);

}

/* ------------------------------------------------------------------------- */
/**
 * @brief バトル天候の取得
 *
 * @param p_gman ゲームマネージャーへのポインタ
 */
/* ------------------------------------------------------------------------- */
BtlWeather GetBattleWeather( GameSys::GameManager* p_gman )
{
  if( p_gman->GetFieldmap() == NULL )
  {
    return BTL_WEATHER_NONE;
  }
  weather::WeatherKind weather = p_gman->GetFieldmap()->GetWeatherControl()->GetNowWeatherKind();

  switch( weather )
  {
  // はれ (ひざしがつよい)
  case weather::DRY:
    return BTL_WEATHER_SHINE;

  // あめ
  // @fix GFNMCat[1546] 霧はBTL_WEATHER_NONEに仕様変更となったのでswitchから削除
  case weather::RAIN:
  case weather::THUNDERSTORM:
  case weather::SUNSHOWER:
    return BTL_WEATHER_RAIN;

  // あられ
  case weather::SNOWSTORM:
    return BTL_WEATHER_SNOW;

  // すなあらし
  case weather::SANDSTORM:
    return BTL_WEATHER_SAND;
  }

  return BTL_WEATHER_NONE;
}

//--------------------------------------------------------------
/**
 * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @retval  true  起動してよい
 * @retval  false 起動しない
 */
//--------------------------------------------------------------
bool EventBattleInstBattleCall::BootChk( GameSys::GameManager* /*p_gman*/ )
{
  return true;
}

//--------------------------------------------------------------
/**
 * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventBattleInstBattleCall::InitFunc( GameSys::GameManager* p_gman )
{
  Fieldmap * p_fieldmap = p_gman->GetFieldmap();
  Attr attr = ATTR_ERROR;

  // 戦闘導入演出取得
  m_pEncountEffect = p_gman->GetGameData()->GetEncountWork()->GetEncountEffect();

  // 戦闘導入演出初期化
  gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );

  Encount::EncountEffect::Description desc;
  desc.effectKind = static_cast<Encount::EncountEffect::EffectKind>( m_pParam->btlEffData.enceff_fld.eff_kind);
  m_pEncountEffect->Initialize( p_heap, desc);

  // 再生待機モードに設定
  m_pEncountEffect->SetSequenceStartupWaitMode( true);

  {
    // 戦闘導入演出を外部で再生していなければシーケンス関連のDLLを読み込む (外部再生の場合はもう読み込まれている)
    // バトルチームとの取り決めで、必ずシーケンス関連のDLLは読み込む必要がある
    gfl2::heap::HeapBase* p_heap_dll = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_DLL_LOAD );
    p_gman->GetGameData()->GetEncountWork()->LoadEncountEffectDll( p_heap_dll );
  }
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 * @return  GMEVENT_RESULT  イベント制御関数の戻り値
 */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleInstBattleCall::MainFunc( GameSys::GameManager* p_gman )
{
  // m_Optionの値により特定のシーケンスを飛ばしているので、順番変更は注意すること
  enum
  {
    SEQ_ENCOUNT_EFFECT_START,
    SEQ_ENCOUNT_EFFECT_WAIT,
    SEQ_FIELDMAP_CLOSE,
    SEQ_BATTLE_CALL,
    SEQ_BATTLE_READ_WAIT,
    SEQ_BATTLE_RET,
    SEQ_CALL_ROYAL_RESULT_APP,
    SEQ_CALL_ROYAL_BATTLE_VIDEO_APP,
    SEQ_CALL_TREE_BATTLE_VIDEO_APP,
    SEQ_FIELDMAP_OPEN,
    SEQ_LOSE,

    SEQ_END,
  };

  switch ( m_Seq )
  {
    case SEQ_ENCOUNT_EFFECT_START:
      {
        // 旧作でエンカウントエフェクトイベント内で行っていた処理はここで行います
        // BGM開始
        p_gman->GetGameData()->GetFieldSound()->StartBattleBGM( m_pParam->btlEffData.bgm_default );
        // 再生開始 (SequenceSystemの初期化から)
        m_pEncountEffect->Start();
        m_Seq = SEQ_ENCOUNT_EFFECT_WAIT;
      }
      break;

    case SEQ_ENCOUNT_EFFECT_WAIT:

        // 戦闘導入演出が再生できるようになるまで待つ
      if( m_pEncountEffect->IsStart() == false)
      {
        break;
      }
      // キャプチャーが必要ならリクエストを出す
      if( m_pEncountEffect->IsNeedFieldCapture())
      {
        m_pEncountEffect->RequestFieldCapture();
      }
      m_Seq = SEQ_FIELDMAP_CLOSE;
      break;

    case SEQ_FIELDMAP_CLOSE:
      {
        // 戦闘導入演出の再生前処理
        m_pEncountEffect->Setup();
        // 戦闘導入演出の再生待機を解除
        m_pEncountEffect->SetSequenceStartupWaitMode( false);

        // 復帰時に設定するため、現在のロケーションを保持しておく
        m_playerLocation.worldId  = p_gman->GetFieldmap()->GetWorldID();
        m_playerLocation.zoneId   = p_gman->GetFieldmap()->GetZoneID();
        m_playerLocation.position = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
        m_playerLocation.quaternion = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;

        // クローズイベント
        EventFieldmapClose* p_event;
        p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose > (
              p_gman->GetGameEventManager( ) );
        EventFieldmapClose::Desc desc;
        desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
        desc.isSkyboxKeep = true;
        p_event->SetDesc( desc );

        // 下画面のフェードだけしてもらう
        p_event->SetFadeCallBackDefaultLowerOnly();
      }

      m_Seq = SEQ_BATTLE_CALL;
      break;

    case SEQ_BATTLE_CALL:
      {
        btl::BATTLE_PROC_PARAM battleProcParam;
        battleProcParam.pAppMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE);
        battleProcParam.pAppDeviceMemory = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE);
        battleProcParam.pGameManager = p_gman;
        battleProcParam.pSetupParam = m_pParam;
        EventBattleDll * dll_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleDll>(p_gman->GetGameEventManager());
        dll_event->SetParameter( &battleProcParam );

        //レコード追加 トレーナー戦扱い
        Savedata::IncRecord(Savedata::Record::RECID_BTL_TRAINER);
        Savedata::IncRecord(Savedata::Record::RECID_DAYCNT_TRAINER_BATTLE);
      }

      m_Seq = SEQ_BATTLE_READ_WAIT;
      break;

    case SEQ_BATTLE_READ_WAIT:
      if( GameSys::IsFinishedLoadBattleDll())
      {
        EventProcessCall::CallRoEventCall<btl::BattleProc>(p_gman);

        m_Seq = SEQ_BATTLE_RET;
      }
      break;

    case SEQ_BATTLE_RET:
      {
#if PM_DEBUG
        //デバッグ処理・LR押しっぱなしで強制負け
        gfl2::ui::Button * button = p_gman->GetUiDeviceManager()->GetButton(0);
        if ( button->GetHold() == (gfl2::ui::BUTTON_L|gfl2::ui::BUTTON_R) )
        {
          m_pParam->result = BTL_RESULT_LOSE;
        }
#endif
        //通信エラーでフェードが完了していない可能性がある。
        if( m_pEncountEffect->IsFinish() == false  )
        {
          return GameSys::GMEVENT_RES_CONTINUE;
        }
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);

        // @fix EVENT_DEVICEの空きを確保するため、ここで戦闘導入演出を破棄する
        m_pEncountEffect->Terminate();
        m_pEncountEffect = NULL;

        // シーケンス関連のDLLは必ず破棄する
        p_gman->GetGameData()->GetEncountWork()->DisposeEncountEffectDll();

        // ロイヤルはこの後にアプリコール
        if( m_pParam->rule == BTL_RULE_ROYAL)
        {
          m_Seq = SEQ_CALL_ROYAL_RESULT_APP;
        }
        // ツリーは負けの場合にバトルビデオをコール
        else
        {
          BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();
          // @fix GFNMCat[3469] バトルツリーの通信マルチで通信エラー時にバトルビデオが録画できてしまう問題を修正
          if( !p_inst->IsCanSaveBattleRecord() || p_inst->IsWin() )
          {
            m_Seq = SEQ_FIELDMAP_OPEN;
          }else{
            m_Seq = SEQ_CALL_TREE_BATTLE_VIDEO_APP;
          }
        }
      }
      return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

    case SEQ_CALL_ROYAL_RESULT_APP:

      if( m_pParam->rule == BTL_RULE_ROYAL)
      {
        BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

        // レコード
        {
          Savedata::IncRecord(Savedata::Record::RECID_BATTLEROYAL);

          // ロイヤル勝利 (clientRankingが0なら勝利)
          if( m_pParam->battleRoyalResult.clientRanking[BTL_CLIENT_PLAYER] == 0)
          {
            Savedata::IncRecord(Savedata::Record::RECID_BATTLEROYAL_WIN);

            /**
              2017/04/21 Fri. Yu Muto
              RECID_GLOBAL_ATTRACTION_BATTLEROYAL
              単純な勝利数のカウントとなったため削除し、Record.cppでのフックで扱うようにする
            */
/*
            // 遠藤さん要望：マスターランク勝利にレコード追加
            if( p_inst->GetSelectRankRoyal() == Savedata::BATTLE_ROYAL_RANK_MASTER )
            {
              Savedata::IncRecord(Savedata::Record::RECID_GLOBAL_ATTRACTION_BATTLEROYAL);
            }
*/
          }
        }

        // ダイアリー
        {
          u16 monsNo = p_inst->GetBattleRoyalEntoryFirstMonsNo();
          // @fix GFNMCat[4342] データ2にランクを追加
          PokeDiary::Set( PDID_COUNT_BATTLEROYAL, monsNo, p_inst->GetSelectRankRoyal());
        }

        p_inst->SetupBattleRoyalResultParam();
        App::BattleRoyalResult::APP_PARAM* param = p_inst->GetBattleRoyalResultParam();

        App::Event::BattleRoyalResultEvent::StartEvent( p_gman, param);

        // @fix GFNMCat[4641] 施設のみ、結果アプリ～バトルビデオ録画アプリの間、勝利BGMを再生するためここで再生する
        Sound::ChangeBGMReq( STRM_BGM_WIN8 );
      }

      m_Seq = SEQ_CALL_ROYAL_BATTLE_VIDEO_APP;
      break;

    case SEQ_CALL_ROYAL_BATTLE_VIDEO_APP:

      if( m_pParam->rule == BTL_RULE_ROYAL)
      {
        // バトルビデオ録画用パラメーター初期化
        gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE );
        BattleInst::BattleInst* p_inst = p_gman->GetBattleInstManager()->GetBattleInst();

        m_pBattleRecorderSaveData = GFL_NEW( p_heap) ExtSavedata::BattleRecorderSaveData( p_heap, p_gman );
        m_pBattleRecorderSaveData->CreateSaveData(  p_inst->GetBattleSetupParam(), &p_inst->GetRegulation() );

        m_pBattleVideoRecordingParam = GFL_NEW( p_heap) NetEvent::BattleVideoRecording::EVENT_APP_PARAM();
        m_pBattleVideoRecordingParam->in.appInParam.pRecorderSaveData = m_pBattleRecorderSaveData;

        NetEvent::BattleVideoRecording::Event::StartEvent( p_gman, m_pBattleVideoRecordingParam );
      }

      m_Seq = SEQ_FIELDMAP_OPEN;
      break;

    case SEQ_CALL_TREE_BATTLE_VIDEO_APP:
      {
        BattleInst::BattleInst*   p_inst  = p_gman->GetBattleInstManager()->GetBattleInst();
        Savedata::BattleInstSave* sv = p_gman->GetGameData()->GetBattleInstSave();
        int btl_count = sv->GetWinCount( p_inst->GetBattleTreeType(), p_inst->GetBattleTreeRank() ) + 1;
        // fix NMCat[1396]  バトルツリーの連戦数が4桁のバトルビデオで連戦数が正常に表示されない
        if (btl_count > 9999)
        {
          btl_count = 9999;
        }
        App::Event::EventBattleInstBattleVideoRecord::StartEvent( p_gman, btl_count );
        p_gman->GetGameData()->GetFieldSound()->EndBattleBGM(); // フィールドBGMを復元
        isEndBattleBGM = true; 
      }
      m_Seq = SEQ_FIELDMAP_OPEN;
      break;
    case SEQ_FIELDMAP_OPEN:
      {
        EventFieldmapOpen* p_event;

        FieldmapDesc desc;
        desc.openMode = FM_OPEN_MODE_APP_RECOVER;
        desc.playerLocation = m_playerLocation;

        // オープンイベント
        p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen > (
                p_gman->GetGameEventManager( ) );
        p_event->SetDesc( desc);
        
        // ツリーはフェードインなし
        // @fix ロイヤルもフェードインなし
        p_event->SetFadeCallBack( NULL, NULL, NULL );
        if( isEndBattleBGM == false)
        {
          p_gman->GetGameData()->GetFieldSound()->EndBattleBGM(); // フィールドBGMを復元
        }
      }

      m_Seq = SEQ_END;
      break;

    case SEQ_END:
      return GameSys::GMEVENT_RES_FINISH;

    default:
      break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   呼び出される関数 純粋仮想関数
 * @param   p_gman       ゲームマネージャークラス
 */
//--------------------------------------------------------------
void EventBattleInstBattleCall::EndFunc( GameSys::GameManager* p_gman )
{
   // ロイヤルのみ。バトルビデオ録画用パラメーター破棄
  if( m_pParam->rule == BTL_RULE_ROYAL)
  {
    GFL_SAFE_DELETE( m_pBattleRecorderSaveData);
    GFL_SAFE_DELETE( m_pBattleVideoRecordingParam);
  } 
}

//----------------------------------------------------------------------------
/**
 *  @brief  バトルパラメータを登録
 *
 *  @param  p_param
 */
//-----------------------------------------------------------------------------
void EventBattleInstBattleCall::SetUpBattleParam( BATTLE_SETUP_PARAM * p_param )
{
  m_pParam = p_param;
}

//--------------------------------------------------------------
/**
* @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
* @retval  true  起動してよい
* @retval  false 起動しない
*/
//--------------------------------------------------------------
bool EventBattleFesBattleCall::BootChk(GameSys::GameManager* /*p_gman*/)
{
  return true;
}

//--------------------------------------------------------------
/**
* @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleFesBattleCall::InitFunc(GameSys::GameManager* p_gman)
{
  
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
* @return  GMEVENT_RESULT  イベント制御関数の戻り値
*/
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBattleFesBattleCall::MainFunc(GameSys::GameManager* p_gman)
{
  // m_Optionの値により特定のシーケンスを飛ばしているので、順番変更は注意すること
  enum
  {
    SEQ_ENCOUNT_EFFECT_START,
    SEQ_FIELDMAP_CLOSE,
    SEQ_BATTLE_CALL,
    SEQ_BATTLE_READ_WAIT,
    SEQ_BATTLE_RET,
    SEQ_CALL_BATTLE_VIDEO_APP,
    SEQ_CLOSE_BATTLE_VIDEO_APP,
    SEQ_FIELDMAP_OPEN,
    SEQ_LOSE,

    SEQ_END,
  };

  switch (m_Seq)
  {
  case SEQ_ENCOUNT_EFFECT_START:
  {
    // 旧作でエンカウントエフェクトイベント内で行っていた処理はここで行います
    // BGM開始
    p_gman->GetGameData()->GetFieldSound()->StartBattleBGM(m_pParam->btlEffData.bgm_default);
    //レコーダーの時下画面はフェードしてない
    gfl2::math::Vector4 sCol(0, 0, 0, 0);
    gfl2::math::Vector4 eCol(0, 0, 0, 255);
    //AppLib::Fade::FADE_DEFAULT_SYNC デフォルト2フレ
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_BALL_TILT, &sCol, &eCol, 4, true);
    m_Seq = SEQ_FIELDMAP_CLOSE;
  }
  break;

  case SEQ_FIELDMAP_CLOSE:
  {
    // 復帰時に設定するため、現在のロケーションを保持しておく
    m_playerLocation.worldId = p_gman->GetFieldmap()->GetWorldID();
    m_playerLocation.zoneId = p_gman->GetFieldmap()->GetZoneID();
    m_playerLocation.position = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().translate;
    m_playerLocation.quaternion = p_gman->GetFieldmap()->GetPlayerCharacter()->GetLocalSRT().rotation;

    // クローズイベント
    EventFieldmapClose* p_event;
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapClose >(
      p_gman->GetGameEventManager());
    EventFieldmapClose::Desc desc;
    desc.closeType = FM_CLOSE_MODE_APP; // メモリを全破棄
    desc.isSkyboxKeep = true;
    p_event->SetDesc(desc);
    p_event->SetFadeCallBack(NULL, NULL, NULL);
  }

  m_Seq = SEQ_BATTLE_CALL;
  break;

  case SEQ_BATTLE_CALL:
  {
    btl::BATTLE_PROC_PARAM battleProcParam;
    battleProcParam.pAppMemory = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
    battleProcParam.pAppDeviceMemory = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
    battleProcParam.pGameManager = p_gman;
    battleProcParam.pSetupParam = m_pParam;
    EventBattleDll * dll_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBattleDll>(p_gman->GetGameEventManager());
    dll_event->SetParameter(&battleProcParam);
    dll_event->SetIsBattleFes(true);

    // レコード追加 トレーナー戦扱い
    Savedata::IncRecord(Savedata::Record::RECID_BTL_TRAINER);
    Savedata::IncRecord(Savedata::Record::RECID_DAYCNT_TRAINER_BATTLE);
    Savedata::IncRecord(Savedata::Record::RECID_BATTLEFES_CHALLENGE_CNT);
  }

  m_Seq = SEQ_BATTLE_READ_WAIT;
  break;

  case SEQ_BATTLE_READ_WAIT:
    if (GameSys::IsFinishedLoadBattleDll())
    {
      EventProcessCall::CallRoEventCall<btl::BattleProc>(p_gman);

      m_Seq = SEQ_BATTLE_RET;
    }
    break;

  case SEQ_BATTLE_RET:
  {
#if PM_DEBUG
    //デバッグ処理・L押しっぱなしで強制負け R押しっぱなしで強制勝ち
    gfl2::ui::Button * button = p_gman->GetUiDeviceManager()->GetButton(0);
    if (button->GetHold() == gfl2::ui::BUTTON_L)
    {
      m_pParam->result = BTL_RESULT_LOSE;
    }
    else if (button->GetHold() == gfl2::ui::BUTTON_R)
    {
      m_pParam->result = BTL_RESULT_WIN;
    }
#endif
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceDefaultOut(gfl2::Fade::DISP_DOUBLE);

    // レインボーイベント判定
    Field::EventWork* ev = p_gman->GetGameData()->GetEventWork();
    bool isEvent = ev->CheckEventFlag(SYS_FLAG_BFES_ROCKET_NOTTORI);

    if (m_pParam->result == BTL_RESULT_WIN)
    {
      // レコード追加 バトルフェス勝利レコード
      Savedata::IncRecord(Savedata::Record::RECID_LARGE_BATTLEFES_WIN);
    }

    // 3戦目勝利時も録画アプリに飛ぶ
    if (m_pParam->result == BTL_RESULT_WIN && 
        p_gman->GetGameData()->GetSaveData()->GetBattleFesSave()->GetWinCount() == BattleFes::BattleFesDefine::FIGHT_TRAINER_MAX - 1 &&
        isEvent == false)
    {
      m_Seq = SEQ_CALL_BATTLE_VIDEO_APP;
    }
    else if (m_pParam->result == BTL_RESULT_WIN || isEvent)
    {
      m_Seq = SEQ_FIELDMAP_OPEN;
    }
    else
    {
      m_Seq = SEQ_CALL_BATTLE_VIDEO_APP;
    }
  }
  return GameSys::GMEVENT_RES_CONTINUE_DIRECT;

  case SEQ_CALL_BATTLE_VIDEO_APP:
  {
    // バトルビデオ録画用パラメーター初期化
    gfl2::heap::HeapBase* p_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT_DEVICE);
    Regulation* pBtlRegulation = GFL_NEW(p_heap) Regulation(p_heap);
    pBtlRegulation->LoadData(Regulation::PRESET_BATTLE_FES);
    p_gman->GetGameData()->GetBattleFesManager()->SetRegulation(pBtlRegulation);

    m_pBattleRecorderSaveData = GFL_NEW(p_heap) ExtSavedata::BattleRecorderSaveData(p_heap, p_gman);
    m_pBattleRecorderSaveData->CreateSaveData(m_pParam, pBtlRegulation);

    m_pBattleVideoRecordingParam = GFL_NEW(p_heap) NetEvent::BattleVideoRecording::EVENT_APP_PARAM();
    m_pBattleVideoRecordingParam->in.appInParam.pRecorderSaveData = m_pBattleRecorderSaveData;

    NetEvent::BattleVideoRecording::Event::StartEvent(p_gman, m_pBattleVideoRecordingParam);

    p_gman->GetGameData()->GetFieldSound()->EndBattleBGM(); // フィールドBGMを復元
    isEndBattleBGM = true;

    m_Seq = SEQ_CLOSE_BATTLE_VIDEO_APP;
    break;
  }

  case SEQ_CLOSE_BATTLE_VIDEO_APP:
  {
    // SEQ_CALL_BATTLE_VIDEO_APPの後処理
    Regulation* pBtlRegulation = p_gman->GetGameData()->GetBattleFesManager()->GetRegulation();
    GFL_ASSERT(pBtlRegulation != NULL);
    pBtlRegulation->DeleteData();
    GFL_SAFE_DELETE(pBtlRegulation);
    p_gman->GetGameData()->GetBattleFesManager()->SetRegulation(NULL);
    m_Seq = SEQ_FIELDMAP_OPEN;
  }// breakしない

  case SEQ_FIELDMAP_OPEN:
  {
    EventFieldmapOpen* p_event;

    FieldmapDesc desc;
    desc.openMode = FM_OPEN_MODE_APP_RECOVER;
    desc.playerLocation = m_playerLocation;

    // オープンイベント
    p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFieldmapOpen >(
      p_gman->GetGameEventManager());
    p_event->SetDesc(desc);

    // ツリーはフェードインなし
    // @fix ロイヤルもフェードインなし
    p_event->SetFadeCallBack(NULL, NULL, NULL);
    if (isEndBattleBGM == false)
    {
      p_gman->GetGameData()->GetFieldSound()->EndBattleBGM(); // フィールドBGMを復元
    }
    m_Seq = SEQ_END;
  }
  break;

  case SEQ_END:
    return GameSys::GMEVENT_RES_FINISH;

  default:
    break;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
* @brief   呼び出される関数 純粋仮想関数
* @param   p_gman       ゲームマネージャークラス
*/
//--------------------------------------------------------------
void EventBattleFesBattleCall::EndFunc(GameSys::GameManager* p_gman)
{
  // バトルビデオ録画用パラメーター破棄
  GFL_SAFE_DELETE(m_pBattleRecorderSaveData);
  GFL_SAFE_DELETE(m_pBattleVideoRecordingParam);
}

//----------------------------------------------------------------------------
/**
*  @brief  バトルパラメータを登録
*
*  @param  p_param
*/
//-----------------------------------------------------------------------------
void EventBattleFesBattleCall::SetUpBattleParam(BATTLE_SETUP_PARAM * p_param)
{
  m_pParam = p_param;
}

GFL_NAMESPACE_END( Field )
