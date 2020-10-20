//=============================================================================
/**
 *
 *  @file   EventScriptEditCall.cpp
 *  @brief  スクリプト エディットモード呼び出し
 *  @author Miyuki Iwasawa 
 *  @data   2015.10.24
 *
 */
//=============================================================================

#if PM_DEBUG

#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_DebugWinSystem.h>
#include <Fade/include/gfl2_FadeManager.h>

#include "GameSys/include/GameManager.h"

#include "FieldScript/include/EventScriptEditCall.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Script/ScriptFuncSetCommon.h"
#include "Field/FieldStatic/include/FieldLocation.h"
#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"

#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"   //for field::script::SystemWork
#include "FieldScript/include/ScriptDebug.h"
#include "FieldScript/include/EventScriptEditCall.h"

#include "Sound/include/Sound.h"

#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"

namespace Field{
  namespace MoveModel{
    class FieldMoveModel;
  }
}

GFL_NAMESPACE_BEGIN( Field )

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプト エディットモードコール
 *
 *  @param  p_gman    ゲームマネージャ
 *  @param  scriptID  スクリプトID
 *  @param  p_model   会話相手  
 *
 *  @return 生成したイベント
 */
//-----------------------------------------------------------------------------
GameSys::GameEvent* EventScriptEditCall::CallScriptEditEvent( GameSys::GameManager* p_gman, u32 scriptID, IFieldActorBase* p_actor, MoveModel::FieldMoveModel* p_model, u32 p0, u32 p1, u32 p2, u32 p3 )
{
  EventScriptEditCall* p_event = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventScriptEditCall>( p_gman->GetGameEventManager() );

  p_event->SetInitialParam( p_gman, scriptID, p_actor, p_model );

  return p_event;
}

/**
 *  コンストラクタ
 */
EventScriptEditCall::EventScriptEditCall( gfl2::heap::HeapBase* heap ) :
  GameSys::GameEvent( heap ),
  m_Seq(0),
  m_SubSeq(0),
  m_ScriptID(0),
  m_SkipPoint(0),
  m_ExitFlag(false),
  m_pTargetModel(NULL)
{
}

/*
 *  パラメータ初期化
 */
void EventScriptEditCall::SetInitialParam( GameSys::GameManager* p_gman, int scriptID, IFieldActorBase* p_actor, MoveModel::FieldMoveModel* p_mdl )
{
  m_ScriptID = scriptID;
  m_SkipPoint = 0;
  m_TargetActorID = p_actor->GetDataEventID();
  m_TargetActorType = p_actor->GetType();

  m_pTargetModel = p_mdl;

  //現在地点を保存
  {
    Field::Location::SaveData locSave;
    p_gman->GetGameData()->GetStartLocation()->Get(&locSave);
    m_StartLoc.Set(locSave);
  }
  m_pFadeManager = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  m_InitialFade = !m_pFadeManager->IsEndStatus(); //はじめが暗い
}

bool EventScriptEditCall::BootChk( GameSys::GameManager* p_gman )
{

#if PM_DEBUG
  if( FieldScript::FieldScriptSystem::DebugHeapCheckOffEnableFlag ){
    //常駐ヒープ
    FieldScript::FieldScriptSystem::DebugRegdentHeapCheckDisableFlag = true;
  }
#endif
  return true;
}


void EventScriptEditCall::InitFunc(GameSys::GameManager* p_gman)
{
  MoveModel::FieldMoveModelManager* pMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  // 地名表示Off
  p_gman->GetFieldmap()->GetEffectManager()->RequestEndPlaceName();

  // 動作モデルの動きを停止
  pMoveModelManager->AddEventRequestAll();

  // イベント中はプレイヤーのコリジョンをOff
  pMoveModelManager->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER )->OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );

  m_pScriptSys = p_gman->GetFieldScriptSystem();
}

GameSys::GMEVENT_RESULT EventScriptEditCall::MainFunc(GameSys::GameManager* p_gman)
{
  switch( m_Seq )
  {
  case SEQ_SCENE_INIT:
    m_Seq = SeqSceneInit(p_gman);
    break;
  case SEQ_SCENE_WAIT:
    m_Seq = SeqSceneWait(p_gman);
    break;
  case SEQ_SCRIPT_INIT:
    m_Seq = SeqScriptInit(p_gman);
    break;
  case SEQ_SCRIPT_UPDATE:
    m_Seq = SeqScriptUpdate(p_gman);
    break;
  case SEQ_SCRIPT_END:
    m_Seq = SeqScriptEnd(p_gman);
    break;
  case SEQ_SCENE_RESET:
    m_Seq = SeqSceneReset(p_gman);
    break;
  case SEQ_END:
  default:
    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

void EventScriptEditCall::EndFunc(GameSys::GameManager* p_gman)
{
  MoveModel::FieldMoveModelManager* pMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();

  //バミリチェックに引っかかってしまうので抜け道を用意する
  {
    // @todo
    pMoveModelManager->ClearCheckSafetyPositionForEventScript();
#if 0
    Field::MoveModel::FieldMoveModelPlayer* pPlayer = 
      reinterpret_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
    pPlayer->EnablePointMoveForEventScript(); //バミリに移動しましたステータスに変更する
#endif
  }

  // 動作モデルの動きを再開
  pMoveModelManager->PullEventRequestAll();

  // イベント中はプレイヤーのコリジョンをOff
  pMoveModelManager->GetFieldMoveModel( MoveModel::FIELD_MOVE_MODEL_PLAYER )->OnStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_COLLISION_OSHIKAESHI_DISABLE );
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
bool EventScriptEditCall::EndCheck( GameSys::GameManager* p_gman )
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
void EventScriptEditCall::WordsetBufClear( GameSys::GameManager* p_gman )
{
  FieldScript::FieldScriptSystem* script = p_gman->GetFieldScriptSystem();
  print::WordSet* wset = script->GetSystemWork()->GetWordSet();

  //デバッグの時は見た目で分かりやすい数字文字列でクリア
  for( int i = 0;i < FieldScript::SystemWork::BUF_ID_MAX; i++ ){
    wset->RegisterNumber( i, 4936, 4, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプト エディットシーン初期化
 */
//-----------------------------------------------------------------------------
int EventScriptEditCall::SeqSceneInit( GameSys::GameManager* p_gman )
{
  switch( m_SubSeq ){
  case 0:
    FadeIn(p_gman);

    m_pScriptSys->RunRequest( m_ScriptID, static_cast<GameSys::GameEvent*>(this), m_TargetActorID, m_TargetActorType, m_pTargetModel );
    m_pScriptSys->SetParameters( 0, 0, 0, 0 );
    m_SubSeq++;
    break;
  case 1:
    {
      int ret = m_pScriptSys->DebugGetSceneEditParam( &m_SceneEditParam );
      if( ret < 0 ) break;
    }
    m_SkipPoint = m_SceneEditParam.skip_code_start;
    m_SkipPointEnd = m_SceneEditParam.skip_code_end;
    m_SkipPointMax = m_SceneEditParam.skip_point_max;

    //初期化パラメータ取ったので、一旦スクリプトを開放
    m_pScriptSys->ForceEnd();
    m_pScriptSys->Update();

    m_SubSeq++;
    //break throw
  case 2:
    if( FadeWait(p_gman) ) break;
    m_SubSeq = 0;
    return SEQ_SCENE_WAIT;
  }
  
  return SEQ_SCENE_INIT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプト エディット待機
 */
//-----------------------------------------------------------------------------
int EventScriptEditCall::SeqSceneWait( GameSys::GameManager* p_gman )
{
  gfl2::ui::DeviceManager* pDeviceManager = p_gman->GetUiDeviceManager();
  gfl2::ui::Button*        pButton        = pDeviceManager->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  
  if( pButton->IsTrigger( gfl2::ui::BUTTON_A )){
    Sound::PlaySE( SEQ_SE_DECIDE1 );
    return SEQ_SCRIPT_INIT;
  }
  if( pButton->IsHold( gfl2::ui::BUTTON_L )){
    if( pButton->IsTrigger( gfl2::ui::BUTTON_B )){
      Sound::PlaySE( SEQ_SE_CANCEL1 );
      m_ExitFlag = true;
      return SEQ_END;
    }
  }
  if( pButton->IsRepeat( gfl2::ui::DIR_RIGHT ) ){

    if( pButton->IsHold( gfl2::ui::BUTTON_R )){
      if( m_SkipPointEnd < m_SkipPointMax ){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPointEnd++;
      }
    }else{
      if( m_SkipPoint < (m_SkipPointEnd-1)){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPoint++;
      }
    }
  }
  if( pButton->IsRepeat( gfl2::ui::DIR_LEFT ) ){

    if( pButton->IsHold( gfl2::ui::BUTTON_R )){
      if( m_SkipPointEnd > (m_SkipPoint+1) ){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPointEnd--;
      }
    }else{
      if( m_SkipPoint > 0 ){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPoint--;
      }
    }
  }
  if( pButton->IsRepeat( gfl2::ui::DIR_UP ) ){

    if( pButton->IsHold( gfl2::ui::BUTTON_R )){
      if( m_SkipPointMax < 99 ){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPointMax++;
      }
    }else{
      if( m_SkipPointEnd < m_SkipPointMax ){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPoint++;
        m_SkipPointEnd++;
      }
    }
  }
  if( pButton->IsRepeat( gfl2::ui::DIR_DOWN ) ){

    if( pButton->IsHold( gfl2::ui::BUTTON_R )){
      if( m_SkipPointMax > 1 ){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPointMax--;
      }
    }else{
      if( m_SkipPoint > 0 ){
        Sound::PlaySE( SEQ_SE_SELECT1 );
        m_SkipPoint--;
        m_SkipPointEnd--;
      }
    }
  }
  if( Debug::DebugTypes::s_pDrawUtilTextUpper )
  {
    wchar_t     label[64];

    swprintf( label, 64, L"play[%2d-%2d] / %2d", m_SkipPoint, m_SkipPointEnd,m_SkipPointMax );

    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 0.0f, 0.0f, 0.0f, 1.0f ) );
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextScale( 1.0f, 1.0f );
  #if defined(GF_PLATFORM_WIN32)
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 302, 222, label );
  #else
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawText( 302, 222, label );
  #endif
      
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextColor( gfl2::math::Vector4( 1.0f, 1.0f, 1.0f, 1.0f ) );
    Debug::DebugTypes::s_pDrawUtilTextUpper->SetTextScale( 1.0f, 1.0f );
  #if defined(GF_PLATFORM_WIN32)
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawTextW( 300, 220, label );
  #else
    Debug::DebugTypes::s_pDrawUtilTextUpper->DrawText( 300, 220, label );
  #endif
  }
  return SEQ_SCENE_WAIT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプト初期化
 */
//-----------------------------------------------------------------------------
int EventScriptEditCall::SeqScriptInit( GameSys::GameManager* p_gman )
{
  switch( m_SubSeq ){
  case 0:
    if(m_InitialFade) FadeOut(p_gman);

    m_pScriptSys->RunRequest( m_ScriptID, static_cast<GameSys::GameEvent*>(this), m_TargetActorID, m_TargetActorType, m_pTargetModel );
    m_pScriptSys->SetParameters( 0, 0, 0, 0 );
    m_pScriptSys->SetReserveScript( FieldScript::SCRID_DUMMY );  //繰り返し実行するため、リザーブスクリプト設定をクリア
    m_SubSeq++;
    break;
  case 1:
    {
      int ret = m_pScriptSys->DebugGetSceneEditParam( &m_SceneEditParam );
      if( ret < 0 ) break;
    }
    m_pScriptSys->DebugSetSceneSkipCode(m_SkipPoint,m_SkipPointEnd,m_SkipPointMax);

    m_SubSeq++;
    //break throw

  case 2:
    if( FadeWait(p_gman) ) break;
    m_SubSeq = 0;
    return SEQ_SCRIPT_UPDATE;
  }
  
  return SEQ_SCRIPT_INIT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプト アップデート
 */
//-----------------------------------------------------------------------------
int EventScriptEditCall::SeqScriptUpdate( GameSys::GameManager* p_gman )
{
  // デバッグメニュー起動中は、スクリプトを実行しない。
  if( gfl2::debug::DebugWin_IsOpen() ){
    return SEQ_SCRIPT_UPDATE;
  }
  // スクリプト更新処理
  m_pScriptSys->Update();
  if( m_pScriptSys->IsRunning() == false )
  {
    FieldScript::FuncSetCommon::SetupScriptFuncEndCheck( p_gman );
    return SEQ_SCRIPT_END;
  }
  return SEQ_SCRIPT_UPDATE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプト 終了処理
 */
//-----------------------------------------------------------------------------
int EventScriptEditCall::SeqScriptEnd( GameSys::GameManager* p_gman )
{
  if( !EndCheck( p_gman ) ) return SEQ_SCRIPT_END;
  
  //Wordsetのバッファーをクリア
  WordsetBufClear( p_gman );

  if( m_ExitFlag ) return SEQ_END;

  // 予約したイベントをコール
  m_pScriptSys->CallEntryNextEvent();
  return SEQ_SCENE_RESET;
}

//----------------------------------------------------------------------------
/**
 *  @brief  スクリプト リセット処理
 */
//-----------------------------------------------------------------------------
int EventScriptEditCall::SeqSceneReset( GameSys::GameManager* p_gman )
{
  switch( m_SubSeq ){
  case 0:
    FadeOut(p_gman);
    m_SubSeq++;
    //break throw
  case 1:
    if( FadeWait(p_gman) ){
      break;
    }
    if( p_gman->GetFieldmap() ) p_gman->GetFieldmap()->GetMoveModelManager()->PullEventRequestAll();
    m_SubSeq++;
    //break throw
  case 2:
    {
      //EventMapJumpCore* p_event = 
      EventMapJump::CallMapJumpPos( p_gman,
        m_StartLoc.GetZoneID(), m_StartLoc.GetPosition(), EventMapJumpCore::EV_MAPJUMP_NO_EFFECT, false );
    }
    m_SubSeq++;
    break;
  case 3:
    {
      Field::MoveModel::FieldMoveModelManager *pFieldMoveModelManager = p_gman->GetGameData()->GetFieldCharaModelManager();
      // 対象となる動作モデルを取得する
      Field::MoveModel::FieldMoveModel* pCharaModel = 
        pFieldMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER );
      pCharaModel->GetCharaDrawInstance()->SetRotationQuat( m_StartLoc.GetQuaternion() );

      //フェードイン
      FadeIn(p_gman);
    }
    m_SubSeq++;
    //break throw
  case 4:
    if( FadeWait(p_gman) ){
      break;
    }
    if( p_gman->GetFieldmap() ) p_gman->GetFieldmap()->GetMoveModelManager()->AddEventRequestAll();
    m_SubSeq = 0;
    return SEQ_SCENE_WAIT;
  }
  
  return SEQ_SCENE_RESET;
}


/**/
void EventScriptEditCall::FadeOut( GameSys::GameManager* p_gman )
{
  gfl2::math::Vector4 col_bs(0.0f,0.0f,0.0f,0.0f);
  gfl2::math::Vector4 col_be(0.0f,0.0f,0.0f,255.0f);

  if( m_pFadeManager->IsEndStatus() ){
     m_pFadeManager->RequestOut(
          gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &col_bs, &col_be, 4 );  
  }
}

void EventScriptEditCall::FadeIn( GameSys::GameManager* p_gman )
{
  if( !m_pFadeManager->IsEndStatus() ){
     m_pFadeManager->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 4 );
  }
}

bool EventScriptEditCall::FadeWait( GameSys::GameManager* p_gman )
{
  return !(m_pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE));
}

GFL_NAMESPACE_END( Field )


#endif //PM_DEBUG
