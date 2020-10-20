//=============================================================================
/**
 *
 *  @file   EventScriptCall.cpp
 *  @brief  スクリプト呼び出し
 *  @author hosaka genya
 *  @data   2011.08.24
 *
 */
//=============================================================================

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_DebugWinSystem.h>

#include "GameSys/include/GameManager.h"

#include "FieldScript/include/EventScriptCall.h"
//#include "assets/field/common_scr_id.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"   //for field::script::SystemWork
#include "FieldScript/include/EventScriptEditCall.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"

#include "Field/FieldStatic/include/EventList/EventListDataManager.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"

#if PM_DEBUG
#include "niji_conv_header/field/script/inc/debug_event_jump.inc"
// debug
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldRo/include/Debug/FieldDebugMapJump.h"
#endif

GFL_NAMESPACE_BEGIN( Field )

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプトコール
 *
 *  @param  p_gman    ゲームマネージャ
 *  @param  scriptID  スクリプトID
 *  @param  p_model   会話相手  
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventScriptCall::CallScript( GameSys::GameManager* p_gman, u32 scriptID, IFieldActorBase* p_actor, MoveModel::FieldMoveModel* p_model, u32 p0, u32 p1, u32 p2, u32 p3 )
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();

#if PM_DEBUG
  if( script->DebugCheckSceneEditEnable(scriptID) ){
    return EventScriptEditCall::CallScriptEditEvent( p_gman, scriptID, p_actor, p_model, p0, p1, p2, p3);
  }
#endif

  EventScriptCall* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventScriptCall>( p_gman->GetGameEventManager() );
  script->RunRequest( scriptID, static_cast<GameSys::GameEvent*>(p_event), p_actor, p_model );
  script->SetParameters( p0, p1, p2, p3 );

  // ダミースクリプトは許可しない
  //GFL_ASSERT(scriptID!=SCRID_DUMMY);
  //GFL_ASSERT(scriptID!=SCRID_NULL);

  // イベントに動作モデルを通達
  p_event->SetTarget( p_actor, p_model );

  //スクリプトIDを保存
  p_event->SetScriptID( scriptID );


  return p_event;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプトコール ForceStopしない版
 *
 *  @param  p_gman    ゲームマネージャ
 *  @param  scriptID  スクリプトID
 *  @param  p_model   会話相手  
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventScriptCall::CallScriptNotForceStop( GameSys::GameManager* p_gman, u32 scriptID,IFieldActorBase* p_actor, MoveModel::FieldMoveModel* p_model, u32 p0, u32 p1, u32 p2, u32 p3 )
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();
  EventScriptCall* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventScriptCall>( p_gman->GetGameEventManager() );
  script->RunRequest( scriptID, static_cast<GameSys::GameEvent*>(p_event), p_actor, p_model );
  script->SetParameters( p0, p1, p2, p3 );

  // ダミースクリプトは許可しない
  //GFL_ASSERT(scriptID!=SCRID_DUMMY);
  //GFL_ASSERT(scriptID!=SCRID_NULL);

  p_event->SetForceStop( false );
  
  // イベントにターゲットを通達
  p_event->SetTarget( p_actor, p_model );

  //スクリプトIDを保存
  p_event->SetScriptID( scriptID );

  return p_event;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプトコール BG
 *
 *  @param  p_gman    ゲームマネージャ
 *  @param  scriptID  スクリプトID
 *  @param  bgid      BGのID
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventScriptCall::CallScriptBG( GameSys::GameManager* p_gman, u32 scriptID, u32 bgid )
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();
  EventScriptCall* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventScriptCall>( p_gman->GetGameEventManager() );
  script->RunRequest( scriptID, static_cast<GameSys::GameEvent*>(p_event) );

  script->SetBGID( bgid );

//  script->SetParameters( p0, p1, p2, p3 );

  // ダミースクリプトは許可しない
  //GFL_ASSERT(scriptID!=SCRID_DUMMY);
  //GFL_ASSERT(scriptID!=SCRID_NULL);

  //スクリプトIDを保存
  p_event->SetScriptID( scriptID );

  return p_event;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプトコール POS
 *
 *  @param  p_gman    ゲームマネージャ
 *  @param  scriptID  スクリプトID
 *  @param  bgid      BGのID
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventScriptCall::CallScriptPos( GameSys::GameManager* p_gman, u32 scriptID, u32 posid )
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();
  EventScriptCall* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventScriptCall>( p_gman->GetGameEventManager() );
  script->RunRequest( scriptID, static_cast<GameSys::GameEvent*>(p_event) );

  script->SetPosID( posid );

//  script->SetParameters( p0, p1, p2, p3 );

  // ダミースクリプトは許可しない
  //GFL_ASSERT(scriptID!=SCRID_DUMMY);
  //GFL_ASSERT(scriptID!=SCRID_NULL);

  // @fix SBTS[140]:看板に話しかけると注目が切れてしまう問題の対処、注目切りを設定
  p_event->SetClearInterestViewFlag( true );

  //スクリプトIDを保存
  p_event->SetScriptID( scriptID );

  return p_event;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプトコール シーン
 *
 *  @param  p_gman    ゲームマネージャ
 *  @param  scriptID  スクリプトID
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventScriptCall::CallScriptScene( GameSys::GameManager* p_gman, u32 scriptID, u32 p0, u32 p1, u32 p2, u32 p3 )
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();

#if PM_DEBUG
  if( script->DebugCheckSceneEditEnable(scriptID) ){
    return EventScriptEditCall::CallScriptEditEvent( p_gman, scriptID, NULL, NULL, p0, p1, p2, p3);
  }
#endif

  EventScriptCall* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventScriptCall>( p_gman->GetGameEventManager() );
  script->RunRequest( scriptID, static_cast<GameSys::GameEvent*>(p_event) );
  script->SetParameters( p0, p1, p2, p3 );

  // ダミースクリプトは許可しない
  //GFL_ASSERT(scriptID!=SCRID_DUMMY);
  //GFL_ASSERT(scriptID!=SCRID_NULL);


  // @fix SBTS[140]:看板に話しかけると注目が切れてしまう問題の対処、注目切りを設定
  p_event->SetClearInterestViewFlag( true );

  //スクリプトIDを保存
  p_event->SetScriptID( scriptID );

  return p_event;
}

bool EventScriptCall::BootChk( GameSys::GameManager* p_gman )
{
  GFL_UNUSED(p_gman);
  return true;
}


void EventScriptCall::InitFunc(GameSys::GameManager* p_gman)
{

#if PM_DEBUG
  if( FieldScript::FieldScriptSystem::DebugHeapCheckOffEnableFlag ){
    // 常駐ヒープ監視終了
    FieldScript::FieldScriptSystem::DebugRegdentHeapCheckDisableFlag = true;
  }
#endif

  GFL_UNUSED(p_gman);
}

GameSys::GMEVENT_RESULT EventScriptCall::MainFunc(GameSys::GameManager* p_gman)
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();
  
  switch( m_Seq )
  {
    case 0:
      // 地名表示Off
      if( p_gman->GetFieldmap() ){
        p_gman->GetFieldmap()->GetEffectManager()->RequestEndPlaceName();
      }
      {
        MoveModel::FieldMoveModelManager* pMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

        // 動作モデルの動きを停止
        pMoveModelManager->AddEventRequestAll();

        // イベント中はプレイヤーのコリジョンをOff
        pMoveModelManager->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER )->OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );
      }
      m_Seq = 1;
      /* FALLTHROW */

    case 1:

      // デバッグメニュー起動中は、スクリプトを実行しない。
      if( gfl2::debug::DebugWin_IsOpen()  ){
        break;
      }
      // スクリプト更新処理
      script->Update();
      if( script->IsRunning() == false )
      {
        //フィールド破棄状態でスクリプトが中断するケースだとRoが無いケースがある
        m_bFieldDLLRecover = p_gman->GetFieldResident()->InitializeFieldDLL(p_gman->GetAsyncFileManager());
        FieldScript::FuncSetCommon::SetupScriptFuncEndCheck( p_gman );
        
        m_Seq = 2;
      }
      break;
      
    case 2:
      if( EndCheck( p_gman ) )
      {
        {
          MoveModel::FieldMoveModelManager* pMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
          Field::MoveModel::FieldMoveModelPlayer* pPlayer = 
            reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

          //GFNMCat[5416] デバッグスクリプト中にリザーブ回避が発生すると、回避フラグが落ちないので条件を修正
#if PM_DEBUG
          //EventDebugJumpがバミリチェックに引っかかってしまうので抜け道を用意する
          // フェスサークルのスクリプトもバミリチェック回避する
          if( m_ScriptID >= SCRID_DEBUG_EVENT_JUMP ||
              p_gman->GetGameData()->IsNowJoinFestaFieldData() )
          {
            // 最終座標チェックをクリア
            pMoveModelManager->ClearCheckSafetyPositionForEventScript();
          }
          else
#endif //PM_DEBUG

#if PM_DEBUG
          // 最終座標チェック
          script->SetDebugScriptEventAfterCheckFlag(true);
#endif
          // プレイヤーのコリジョンをOn
          pMoveModelManager->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER )->OffStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );

          // 動作モデルの動きを再開
          pMoveModelManager->PullEventRequestAll();
        }
        
        //Wordsetのバッファーをクリア
        WordsetBufClear( p_gman );
        if( m_bFieldDLLRecover ){
          p_gman->GetFieldResident()->TerminateFieldDLL();
        }
        // 予約したイベントに切り替え
        script->ChangeEntryNextEvent();
        return GameSys::GMEVENT_RES_FINISH;
      }
      break;

    default : GFL_ASSERT(0);
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}
  
void EventScriptCall::EndFunc(GameSys::GameManager* p_gman)
{
  GFL_UNUSED(p_gman);
}


/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプトの終了チェック
 *
 * @param p_gman ゲームマネージャーへのポインタ
 *
 * @return true:終了
 */
/* ------------------------------------------------------------------------- */
bool EventScriptCall::EndCheck( GameSys::GameManager* p_gman )
{
  // 全てのチェック関数を回す
  return FieldScript::FuncSetCommon::CheckScriptFuncEnd( p_gman );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief スクリプト終了時のWordsetバッファークリア処理
 *
 * @param p_gman ゲームマネージャーへのポインタ
 */
/* ------------------------------------------------------------------------- */
void EventScriptCall::WordsetBufClear( GameSys::GameManager* p_gman )
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();
  print::WordSet* wset = script->GetSystemWork()->GetWordSet();
#if PM_DEBUG
  //デバッグの時は見た目で分かりやすい数字文字列でクリア
  for( int i = 0;i < FieldScript::SystemWork::BUF_ID_MAX; i++ ){
    wset->RegisterNumber( i, 4936, 4, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  }
#else
  wset->ClearAllBuffer();
#endif
}

GFL_NAMESPACE_END( Field )


