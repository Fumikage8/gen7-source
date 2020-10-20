/**
 *  GAME FREAK inc.
 *
 *  @file   EventFishingAction.cpp
 *  @brief  釣りイベント(調べたとき)
 *  @author Masanori Kanamaru
 *  @date   2015.10.26
 */

#include <Fade/include/gfl2_FadeTypes.h>
#include <Fade/include/gfl2_FadeManager.h>

#include "Field/FieldStatic/include/Event/EventFishingAction.h"

#include "System/include/GflUse.h"

#include "Field/FieldRo/include/Fieldmap.h"

#include "Field/FieldRo/include/PlacementData/actor/FieldFishingSpotActor.h"
#include "Field/FieldRo/include/PlacementData/accessor/FieldFishingSpotAccessor.h"

#include "Field/FieldStatic/source/Event/EventFieldmapControl.h"

// バトル
#include "Field/FieldStatic/include/Event/EventBattleCall.h"
#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"
#include "Field/FieldRo/include/Encount/FieldEncount.h"
#include "Field/FieldRo/include/Encount/FieldEncountPokeParam.h"

// BGM操作
#include "Field/FieldStatic/include/Sound/FieldSound.h"

// エフェクト
#include "Field/FieldRo/include/Effect/content/FieldEffectFishingBuoy.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectExclamation2.h"

// ドロップアイテム
#include "Field/FieldRo/include/DropItem/FieldDropItemCreator.h"

// スクリプト
#include "FieldScript/include/EventScriptCall.h"
#include "FieldScript/include/FieldScriptSystem.h"
#include "FieldScript/include/ScriptWork.h"
#include "fieldScript/include/ScriptObject.h"
#include "Field/FieldRo/include/Window/FieldWindowManager.h"

#include "niji_conv_header/field/script/inc/fishing.inc"

GFL_NAMESPACE_BEGIN( Field );

#if PM_DEBUG
int EventFishingAction::stabilizingTargetFlag = EventFishingAction::Disable;
#endif

const f32 EventFishingAction::BuoyYOffset = 20;

GameSys::GameEvent* EventFishingAction::Call(GameSys::GameEventManager* pGameEventManager, FieldFishingSpotActor* pActor)
{
  EventFishingAction* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventFishingAction>(pGameEventManager);

  pEvent->pActor  = pActor;
  pEvent->playerState.Set(PlayerState::STATE_WAIT);

  pEvent->waitFrame     = EventFishingAction::LotFrame(1, 3);
  pEvent->pullFrame     = EventFishingAction::LotFrame(0.5f, 1.0f);
  return pEvent;
}


u32 EventFishingAction::LotFrame(f32 minSec, f32 maxSec)
{
  u32 fps = 30;
  return  System::GflUse::GetPublicRand((maxSec - minSec)*fps) + minSec * fps;
}

bool EventFishingAction::LotHit(GameSys::GameManager* pGameManager)
{
  // タマゴを避けて先頭ポケモンを選択
  const pml::PokeParty* pParty = pGameManager->GetGameData()->GetPlayerParty();
  u32 idx = pParty->GetMemberTopIndex(pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG);

  //「ねんちゃく」「きゅうばん」の時は必ず成功
  if( idx != pml::PokeParty::MEMBER_INDEX_ERROR )
  {
    switch(pParty->GetMemberPointerConst(idx)->GetTokuseiNo())
    {
    case TOKUSEI_NENTYAKU:
    case TOKUSEI_KYUUBAN:
      return true;
    }
  }

  return System::GflUse::GetPublicRand(100) < EventFishingAction::HitPercentage;
}

/**
  *  @brief コンストラクタ
  */
EventFishingAction::EventFishingAction(gfl2::heap::HeapBase* pHeap)
  : GameSys::GameEvent(pHeap),
  pExclamationIcon(NULL),
  pBuoy(NULL),
  pActor(NULL),
  waitFrame(1),
  pullFrame(0.5f),
  elapsedFrame(0),
  isSuccess(false),
  playerState()
{
}

/**
  *  @brief デストラクタ
  */
EventFishingAction::~EventFishingAction()
{
}


//--------------------------------------------------------------
/**
  * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  * @retval  true  起動してよい
  * @retval  false 起動しない
  */
//--------------------------------------------------------------
bool EventFishingAction::BootChk(GameSys::GameManager* pGameManager)
{
  return true;
}


//--------------------------------------------------------------
/**
  * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  */
//--------------------------------------------------------------
void EventFishingAction::InitFunc(GameSys::GameManager* pGameManager)
{}

//--------------------------------------------------------------
/**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
  */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventFishingAction::MainFunc(GameSys::GameManager* pGameManager)
{
  enum
  {
    SEQ_READY,

    SEQ_START_ROTATION,
    SEQ_WAIT_ROTATION,
    SEQ_CAST,
    SEQ_WAITING_FINISH_CAST,
    SEQ_WAIT_START,
    SEQ_WAITING,

    SEQ_FAIL_TOO_EARLY,
    SEQ_FAIL_TOO_EARLY_END,

    SEQ_FAIL_LOTTERY,
    SEQ_FAIL_LOTTERY_PULL,
    SEQ_FAIL_LOTTERY_END,

    SEQ_HIT,
    SEQ_WAIT_HOOK_MOTION,
    SEQ_WAIT_PULL_BUTTON,

    SEQ_HOOK_SUCCESS,

    SEQ_GET_ITEM,
    SEQ_GET_ITEM_END,

    SEQ_ENCOUNT,
    SEQ_ENCOUNT_RESULT,
    SEQ_ENCOUNT_DISPOSE,
    SEQ_ENCOUNT_FADE_IN,
    SEQ_ENCOUNT_WAIT_FADE,

    SEQ_HOOK_FAIL,
    SEQ_HOOK_FAIL_END,
    
    SEQ_END,
    SEQ_END_DISPOSE_RESOURCE,
  };

  // PC状態更新
  MoveModel::FieldMoveModelManager* pMoveModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
  MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
  GFL_ASSERT_MSG(pPlayer != NULL, "Error : pPlayer is null \n");
  this->playerState.Update(pPlayer, pMoveModelManager);
  
  // NMCat[4088] ダングリングポインタの対応
  // 本来であればpMistとpActorのNULLチェックを以後にも追加すべきだが、
  // シーケンス上はアクセスしないこと、コード変更の位置を鑑みて今回はここのみで行う。
  FieldFishingSpotActor::Mist* pMist = NULL;
  if (this->pActor != NULL)
  {
    pMist = this->pActor->GetMist();
    pMist->Update();
  }

  switch (this->GetSeqNo())
  {
    //---------------------------------------------------------------------
    // ★★★★ START
  case  SEQ_READY:
    {
      Effect::EffectManager* pEffectManager = pGameManager->GetFieldmap()->GetEffectManager();

      // 浮きのリソース読み込み
      pEffectManager->LoadDynamicEffectResource( Effect::EFFECT_TYPE_FISHING_BUOY, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ), true );
      pEffectManager->SetupDynamicEffectResource( Effect::EFFECT_TYPE_FISHING_BUOY, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );

      // うき生成
      const FieldFishingSpotAccessor::Data* pData = this->pActor->GetData();
      gfl2::math::Vector3 pos(pData->position[0], pData->position[1] + EventFishingAction::BuoyYOffset, pData->position[2]);
      this->pBuoy = static_cast<Effect::EffectFishingBuoy*>(pEffectManager->CreateEffect(
        Effect::EFFECT_TYPE_FISHING_BUOY,
        pos));

      if (this->pBuoy == NULL)
      {
        GFL_ASSERT_MSG(false, "Fail Creating Buoy\n");

        this->SetSeqNo(SEQ_END);
        break;
      }

      this->AddSeqNo();
    }
    break;
    //---------------------------------------------------------------------

    //---------------------------------------------------------------------
    // ★★★★ キャスト ～ ヒット判定 - START
  case SEQ_START_ROTATION:
    {
      enum
      {
        ThresoldDegree = 20,
        RotationFrame = 10,
      };

      const gfl2::math::Vector3 direction = (this->pActor->GetPosision() - pPlayer->GetPositionPrevUpdateStart()).Normalize();

      const gfl2::math::Vector3 forward = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform(gfl2::math::Vector3(0,0,1));

      f32 dot = gfl2::math::Abs((direction).Dot(forward));
      f32 thresold = gfl2::math::CosDeg(ThresoldDegree);

      if(dot > thresold)
      {
        this->SetSeqNo(SEQ_CAST);
        break;
      }

      f32 y = 0;

      if (gfl2::math::Abs(direction.z) < EPSILON * 2)
      {
        y = (direction.x > 0) ? 90 : -90;
      }
      else
      {
        y = gfl2::math::Atan2Deg(direction.x, direction.z);
      }

      pPlayer->SetActionCommandRotation(MoveModel::FIELD_ACTION_COMMAND_ROTATION, y, 0, 0, RotationFrame);
      this->AddSeqNo();
    }
    break;
  case SEQ_WAIT_ROTATION:
    {
      if (pPlayer->IsExecuteActionCommand())
      {
        break;
      }
      this->AddSeqNo();
    }
    break;
  case SEQ_CAST:
    {
      const u32 DEFAULT_DISABLE_FRAME = 8;
      pPlayer->DisableInterest(DEFAULT_DISABLE_FRAME);
      this->playerState.Set(PlayerState::STATE_START_CAST);
      this->AddSeqNo();
    }
    break;
  case SEQ_WAITING_FINISH_CAST:
    {
      if(this->playerState.Get() == PlayerState::STATE_WAIT_WITH_ROD)
      {
        this->AddSeqNo();
      }
    }
    break;
  case SEQ_WAIT_START:
    {
      // うき表示
      pBuoy->ChangeAnimation(BL_IDX_FISHING_BUOY_FISHING_UKI_ENTER_GFBMOT);

      // 着水SE再生
      Sound::PlaySE(SEQ_SE_FLD_114);

      // BGMを小さくする      
      pGameManager->GetGameData()->GetFieldSound()->ChangeBGMVolume( 0.3f, Sound::BGM_FADE_FAST);

      this->elapsedFrame = 0;
      this->AddSeqNo();
    }
    break;
  case SEQ_WAITING:
    {
      //ボタン入力があったら失敗
      gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ))
      {
        this->SetSeqNo(SEQ_FAIL_TOO_EARLY);
        break;
      }

      // 特定フレーム経過するまで待機
      if (this->elapsedFrame < this->waitFrame)
      {
        this->elapsedFrame++;
        break;
      }

      // 経過後、「ヒットするか」の抽選      
      if (EventFishingAction::LotHit(pGameManager))
      {
        this->SetSeqNo(SEQ_HIT);
      }
      else
      {
        this->SetSeqNo(SEQ_FAIL_LOTTERY);
      }
    }
    break;
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
    // ★★★★ 早すぎて失敗 - START
  case SEQ_FAIL_TOO_EARLY:
    {
      // レアフラグをおろす
      this->pActor->EscapeRare();
      // 靄の非表示化
      pMist->StartDilute();
      // 浮き消滅アニメーション
      this->pBuoy->ChangeAnimation(BL_IDX_FISHING_BUOY_FISHING_UKI_EXIT_GFBMOT);
      // 竿をしまう
      this->playerState.Set(PlayerState::STATE_PULL_ROD_FAIL);
      this->AddSeqNo();
    }
    break;
  case SEQ_FAIL_TOO_EARLY_END:
    {
      if (this->playerState.Get() == PlayerState::STATE_DISPOSE_END && pMist->GetState() == FieldFishingSpotActor::Mist::Ditutied)
      {
        // メッセージ再生
        EventScriptCall::CallScript(pGameManager, SCRID_FISHING_TOO_EARLY, NULL );
        this->SetSeqNo(SEQ_END);
      }
    }
    break;
    //---------------------------------------------------------------------

    //---------------------------------------------------------------------
    // ★★★★ 抽選漏れ - START
  case SEQ_FAIL_LOTTERY:
    {
      // メッセージ再生
      EventScriptCall::CallScript(pGameManager, SCRID_FISHING_LOTTERY_FAIL, NULL );
      this->AddSeqNo();
    }
    break;
  case SEQ_FAIL_LOTTERY_PULL:
    {
      this->pBuoy->ChangeAnimation(BL_IDX_FISHING_BUOY_FISHING_UKI_EXIT_GFBMOT);
      this->playerState.Set(PlayerState::STATE_PULL_ROD_FAIL);
      this->AddSeqNo();
    }
    break;
  case SEQ_FAIL_LOTTERY_END:
    {
      if (this->playerState.Get() == PlayerState::STATE_DISPOSE_END)
      {
        this->SetSeqNo(SEQ_END);
      }
    }
    break;
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
    // ★★★★ ヒット ～ 入力待ち - START
  case SEQ_HIT:
    {
      // 魚ヒット      
      // 靄の非表示化
      pMist->StartDilute();

      this->pBuoy->ChangeAnimation(BL_IDX_FISHING_BUOY_FISHING_UKI_HIT_GFBMOT);

      this->playerState.Set(PlayerState::STATE_START_HOOK);

      //「!」アイコン表示
      Effect::EffectManager* pEffectManager = pGameManager->GetFieldmap()->GetEffectManager();
      this->pExclamationIcon = static_cast<Effect::EffectExclamation2*>(pEffectManager->CreateEffect(
        Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY,
        pPlayer,
        true));

      if (this->pExclamationIcon == NULL)
      {
        GFL_ASSERT_MSG(false, "CanNot Get Icon\n");
      }
      else
      {
        this->pExclamationIcon->SetMode( Effect::EffectExclamation2::MODE_WAIT );
      }

      this->isSuccess = false;

      this->AddSeqNo();
    }
    break;
  case SEQ_WAIT_HOOK_MOTION:
    {
      //ボタン入力があったら成功
      gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
      if( pButton->IsTrigger( gfl2::ui::BUTTON_A ))
      {
        this->isSuccess = true;
      }
      
      if (this->playerState.Get() == PlayerState::STATE_REELING && pMist->GetState() == FieldFishingSpotActor::Mist::Ditutied)
      {
        this->elapsedFrame = 0;
        this->AddSeqNo();
      }
    }
    break;
  case SEQ_WAIT_PULL_BUTTON :
    {
      if (this->elapsedFrame < this->pullFrame && this->isSuccess == false)
      {
        //ボタン入力があったら成功
        gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
        if( pButton->IsTrigger( gfl2::ui::BUTTON_A ))
        {
          this->isSuccess = true;
        }
        else
        {
          this->elapsedFrame++;
          break;
        }
      }

      //「!」アイコン消す
      if (this->pExclamationIcon != NULL)
      {
        Effect::EffectManager* pEffectManager = pGameManager->GetFieldmap()->GetEffectManager();
        this->pExclamationIcon->RequestClose();
        pEffectManager->DeleteEffect(this->pExclamationIcon);
        this->pExclamationIcon = NULL;
      }
      
      // 分岐
      this->SetSeqNo(this->isSuccess ? SEQ_HOOK_SUCCESS : SEQ_HOOK_FAIL);
    }
    break;
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
    // 成功
  case SEQ_HOOK_SUCCESS:
    {
      // うき水しぶきエフェクト
      this->pBuoy->ChangeAnimation(BL_IDX_FISHING_BUOY_FISHING_UKI_SUCCESS_GFBMOT);

      this->playerState.Set(PlayerState::STATE_PULL_ROD_SUCCESS);

      bool isEncount = (System::GflUse::GetPublicRand(100) <  this->pActor->GetLotData()->encountPercentage);

#if PM_DEBUG
      switch(EventFishingAction::stabilizingTargetFlag)
      {
      case EventFishingAction::ForceEnount:
        isEncount = (this->pActor->GetLotData()->encountPercentage > 0);
        break;
      case EventFishingAction::ForceItem:
        isEncount = (this->pActor->GetLotData()->encountPercentage == 100);
        break;
      }
#endif

      this->SetSeqNo(isEncount ? SEQ_ENCOUNT : SEQ_GET_ITEM);
    }
    break;
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
  case SEQ_GET_ITEM :
    {
      if(this->playerState.Get() == PlayerState::STATE_PULL_ROD_SUCCESS_END)
      {
        this->playerState.Set(PlayerState::STATE_PULL_ROD_SUCCESS_PACK);
        this->AddSeqNo();
      }
    }
    break;
    //---------------------------------------------------------------------
  case SEQ_GET_ITEM_END:
    {
      if(this->playerState.Get() == PlayerState::STATE_DISPOSE_END)
      {
        // レコード加算 : アイテムを釣り上げた回数
        Savedata::IncRecord(Savedata::Record::RECID_FISHING_ITEM);
        if (this->pActor->ExistRare())
        {  
          // レコード加算 : レア魚影を釣り上げた回数
          Savedata::IncRecord(Savedata::Record::RECID_FISHING_BUBBLE);
        }

        // アイテム抽選
        u32 itemID = pGameManager->GetFieldmap()->GetDropItemCreator()->LotteryDropItem(this->pActor->GetLotData()->dropItemTableID);
        EventScriptCall::CallScript(pGameManager, SCRID_FISHING_GET_ITEM, NULL, NULL, itemID);

        // レアフラグをおろす
        this->pActor->EscapeRare();

        this->SetSeqNo(SEQ_END);
      }
    }
    break;
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
    // ★★★★ ポケモンとエンカウント - START
  case SEQ_ENCOUNT :
    {
      if(this->playerState.Get() == PlayerState::STATE_PULL_ROD_SUCCESS_END)
      {
        // レコード加算 : ポケモンを釣り上げた回数
        Savedata::IncRecord(Savedata::Record::RECID_FISHING_POKEMON);
        if (this->pActor->ExistRare())
        {  
          // レコード加算 : レア魚影を釣り上げた回数
          Savedata::IncRecord(Savedata::Record::RECID_FISHING_BUBBLE);
        }

        Fieldmap* pFieldMap = pGameManager->GetFieldmap();

        // 浮きの破棄
        pFieldMap->GetEffectManager()->DeleteEffect(this->pBuoy);
        this->pBuoy = NULL;
 
        // ポケモンの抽選
        const Encount::EncountPokeParam* pResult = NULL;
        pResult = pFieldMap->GetEncount()->LotteryEncountPoke(pGameManager, this->pActor->GetLotData()->encountTableID, true);
        GFL_ASSERT_STOP( pResult );

        // レアフラグをおろす
        this->pActor->EscapeRare();

        // 復帰時に設定するため、現在のロケーションを保持しておく
        gfl2::math::SRT srt = pFieldMap->GetPlayerCharacter()->GetLocalSRT();
        this->playerLocation.worldId    = pFieldMap->GetWorldID();
        this->playerLocation.zoneId     = pFieldMap->GetZoneID();        
        this->playerLocation.position   = srt.translate;
        this->playerLocation.quaternion = srt.rotation;

        Encount::PokeSet& pokeSet = pGameManager->GetGameData()->GetEncountWork()->GetPokeSet();

        this->pActor = NULL;

        // バトル処理
        u32 callOpt = EVBTL_CALLOPT_NOT_SCRIPT | EVBTL_CALLOPT_SKIP_FIELD_OPEN;
        EventBattleCall::CallWild(pGameManager->GetGameEventManager(), pGameManager, pokeSet, callOpt);

        this->AddSeqNo();
      }
    }
    break;
  case SEQ_ENCOUNT_RESULT :
    {
      u32 result = EventBattleCall::GetWildBattleResult(pGameManager->GetGameData()->GetLastBattleResult());

      // バトル敗北時はフィールドオープンを呼んではいけない
      if(result == BTL_RESULT_WILD_LOSE)
      {
        this->SetSeqNo(SEQ_END);
        break;
      }

      // フィールドオープンイベント
      EventFieldmapOpen* pEvent;
      FieldmapDesc desc;
      desc.openMode = FM_OPEN_MODE_APP_RECOVER;
      desc.playerLocation = this->playerLocation;
      pEvent = GAMESYSTEM_EVENT_CALL< GameSys::GameEventManager, EventFieldmapOpen>( pGameManager->GetGameEventManager());
      pEvent->SetDesc( desc);
      pEvent->SetFadeCallBack(NULL,NULL, NULL);

      this->AddSeqNo();
    }
    break;
  case SEQ_ENCOUNT_DISPOSE :
    {
      this->playerState.Set(PlayerState::STATE_START_DISPOSE_ROD);
      this->AddSeqNo();
    }
    break;
  case SEQ_ENCOUNT_FADE_IN :
    {
      if (this->playerState.Get() == PlayerState::STATE_DISPOSE_END)
      {
        // フェードリクエスト
        GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager )->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA );
        this->AddSeqNo();
      }

    }
    break;
  case SEQ_ENCOUNT_WAIT_FADE :
    {
      // フェードを待つ
      if( GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd( gfl2::Fade::DISP_DOUBLE))
      {
        this->SetSeqNo(SEQ_END);
      }
    }
    break;
    //---------------------------------------------------------------------

    //---------------------------------------------------------------------
    // ★★★★ 時間内に押さずに失敗 - START
  case SEQ_HOOK_FAIL:
    {
      // レコード加算 : 反応が遅すぎた回数
      Savedata::IncRecord(Savedata::Record::RECID_FISHING_FAILURE);

      this->pBuoy->ChangeAnimation(BL_IDX_FISHING_BUOY_FISHING_UKI_EXIT_GFBMOT);
      this->playerState.Set(PlayerState::STATE_PULL_ROD_FAIL);

      this->AddSeqNo();
    }
    break;
  case SEQ_HOOK_FAIL_END:
    {
      if (this->playerState.Get() == PlayerState::STATE_DISPOSE_END)
      {
        // レアフラグをおろす
        this->pActor->EscapeRare();

          // メッセージ再生
        EventScriptCall::CallScript(pGameManager, SCRID_FISHING_HOOK_FAIL, NULL );
        this->SetSeqNo(SEQ_END);
      }
    }
    break;
    //---------------------------------------------------------------------


    //---------------------------------------------------------------------
    // 終了処理
  case SEQ_END:
    {
      // 浮き消す
      if (this->pBuoy != NULL)
      {
        Effect::EffectManager* pEffectManager = pGameManager->GetFieldmap()->GetEffectManager();
        pEffectManager->DeleteEffect(this->pBuoy);
      }

      // BGMを音量を戻す
      pGameManager->GetGameData()->GetFieldSound()->ChangeBGMVolume( 1, Sound::BGM_FADE_FAST);

      this->AddSeqNo();
    }
    break;

  case SEQ_END_DISPOSE_RESOURCE:
    {
      Effect::EffectManager* pEffectManager = pGameManager->GetFieldmap()->GetEffectManager();
      // 浮きのリソース破棄
      pEffectManager->UnLoadDynamicEffectResource( Effect::EFFECT_TYPE_FISHING_BUOY, gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT_DEVICE ) );
    }
    return GameSys::GMEVENT_RES_FINISH;
    //---------------------------------------------------------------------

  default:
    GFL_ASSERT_MSG(false, "EventFishing : Invalid SEQ. Why? \n");
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  */
//--------------------------------------------------------------
void EventFishingAction::EndFunc(GameSys::GameManager* pGameManager)
{}


inline void EventFishingAction::PlayerState::Set(EventFishingAction::PlayerState::State state)
{
  this->state = state;
}

inline EventFishingAction::PlayerState::State EventFishingAction::PlayerState::Get() const
{
  return this->state;
}

inline void EventFishingAction::PlayerState::Update(MoveModel::FieldMoveModelPlayer* pPlayer, MoveModel::FieldMoveModelManager* pMoveModelManager)
{
  switch(this->state)
  {
  case STATE_WAIT :
    break;


  case STATE_START_CAST :
    {
      // 釣竿読み込み開始
      pMoveModelManager->LoadDynamicAsync(MoveModel::FieldMoveModelManager::DYNAMIC_RES_LOAD_INDEX_SUBOBJECT, this->GetRodModelID(pPlayer));
      this->state = STATE_LOADING_ROD;
    }
    break;
  case STATE_LOADING_ROD :
    {
      u32 rodModelID = this->GetRodModelID(pPlayer);
      if( pMoveModelManager->IsLoaded(rodModelID))
      {
        pMoveModelManager->SetupModel(rodModelID);

        // 釣竿を持たせる
        Field::MoveModel::FieldSubobjectHeader header;

        header.characterId = rodModelID;
        
        pMoveModelManager->SetSubobjectToMoveModel( pPlayer, 0, &header,true);
        pPlayer->GetSubobject(0)->GetCharaDrawInstance()->SetScale(0.0f,0.0f,0.0f);
        this->state = STATE_CAST;
      }
    }
    break;
  case STATE_CAST :
    {
      // SE再生
      MoveModel::FieldMoveModel* pRideModel = pPlayer->GetOnMoveModel();
      if (pRideModel == NULL)
      {
        Sound::PlaySE(SEQ_SE_FLD_113);
      }
      else
      {
        Sound::PlaySE(SEQ_SE_FLD_113_RAPLACE);
      }

      // 投げモーション再生
      this->ChangeAnimation(pPlayer, P2_BASE_FI091_FISHCAST01, P2_BASE_RI130_FISHCAST01);
      this->state = STATE_CASTING;
    }
    break;
  case STATE_CASTING :
    {
      pPlayer->GetSubobject(0)->GetCharaDrawInstance()->SetScale(1.0f,1.0f,1.0f);

      if (pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame())
      {
        // 待機モーション再生
        this->ChangeAnimation(pPlayer, P2_BASE_FI092_FISHWAIT01, P2_BASE_RI131_FISHWAIT01);
        this->state = STATE_WAIT_WITH_ROD;
      }
    }
    break;
  case STATE_WAIT_WITH_ROD:
    break;


  case STATE_START_HOOK :
    {
      // フッキングモーション再生
      this->ChangeAnimation(pPlayer, P2_BASE_FI093_FISHHIT01, P2_BASE_RI132_FISHHIT01);
      this->state = STATE_START_HOOKING;
    }
    break;
  case STATE_START_HOOKING :
    {
      if (pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame())
      {
        // SE再生
        Sound::PlaySE(SEQ_SE_FLD_046);
        
        // リーリングモーション再生
        this->ChangeAnimation(pPlayer, P2_BASE_FI094_FISHREEL01, P2_BASE_RI133_FISHREEL01);
        this->state = STATE_REELING;
      }
    }
    break;
  case STATE_REELING :
    break;


  case STATE_PULL_ROD_FAIL :
    {
      // STATE_START_HOOKINGのSEの停止
      Sound::StopSE(SEQ_SE_FLD_046);

      // SE再生
      Sound::PlaySE(SEQ_SE_FLD_218);

      // 引き上げモーション再生
      this->ChangeAnimation(pPlayer, P2_BASE_FI097_FISHLOSE01, P2_BASE_RI135_FISHLOSE01);
      this->state = STATE_PULL_ROD_FAIL_WAIT;
    }
    break;
  case STATE_PULL_ROD_FAIL_WAIT:
    {
      if (pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame())
      {
        // しまうモーション
        this->ChangeAnimation(pPlayer, P2_BASE_FI099_FISHLOSE01_END, P2_BASE_RI137_FISHLOSE01_END);
        this->state = STATE_PULLING_ROD;
      }
    }
    break;

  case STATE_PULL_ROD_SUCCESS :
    {
      // STATE_START_HOOKINGのSEの停止
      Sound::StopSE(SEQ_SE_FLD_046);

      // SE再生
      Sound::PlaySE(SEQ_SE_FLD_115);

      // 引き上げモーション再生
      this->ChangeAnimation(pPlayer, P2_BASE_FI096_FISHSUCCESS01, P2_BASE_RI134_FISHSUCCESS01);
      this->state = STATE_PULL_ROD_SUCCESS_WAIT;
    }
    break;
  case STATE_PULL_ROD_SUCCESS_WAIT:
    {
      if (pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame())
      {
        this->state = STATE_PULL_ROD_SUCCESS_END;
      }
    }
    break;
  case STATE_PULL_ROD_SUCCESS_END:
    break;
  case STATE_PULL_ROD_SUCCESS_PACK:
    {
      // しまうモーション
      this->ChangeAnimation(pPlayer, P2_BASE_FI098_FISHSUCCESS01_END, P2_BASE_RI136_FISHSUCCESS01_END);
      this->state = STATE_PULLING_ROD;
    }
    break;

  case STATE_PULLING_ROD :
    {
      if (pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame())
      {
        // @fix GFMMCat[8]:特定のタイミングでラプラスと自機のモーションが停止している、対処。該当アニメーション終了時に待機モーション再生と釣竿の非表示を追加。
        pPlayer->SetSubobject(0, NULL, NULL); // @note このタイミングで消さないと、一瞬おかしな位置に表示されてしまう
        this->ChangeAnimation(pPlayer, pPlayer->GetDefaultIdleAnimationId(), pPlayer->GetDefaultIdleAnimationId() );
        
        this->state = STATE_START_DISPOSE_ROD;
      }
    }
    break;

  case STATE_START_DISPOSE_ROD:
    {
        // 釣竿しまう
        pPlayer->SetSubobject(0, NULL, NULL);
        this->state = STATE_DISPOSE_ROD;
    }
    break;
  case STATE_DISPOSE_ROD :
    {
      // 釣竿破棄
      pMoveModelManager->Unload(this->GetRodModelID(pPlayer));
      this->state = STATE_DISPOSE_END;
    }
    break;
  case STATE_DISPOSE_END:
    break;

  default:
    GFL_ASSERT_MSG(false, "Invalid Player State \n");
  }
}

inline u32 EventFishingAction::PlayerState::GetRodModelID(MoveModel::FieldMoveModelPlayer* pPlayer)
{
  return pPlayer->IsMale() ? CHARA_MODEL_ID_OB0002_00_PC0001_00 : CHARA_MODEL_ID_OB0002_00_PC0002_00;
}


inline void EventFishingAction::PlayerState::ChangeAnimation(MoveModel::FieldMoveModelPlayer* pPlayer, u32 singleAnimationID, u32 rideAnimationID)
{
  MoveModel::FieldMoveModel* pRideModel = pPlayer->GetOnMoveModel();
  if (pRideModel == NULL)
  {
    pPlayer->GetCharaDrawInstance()->ChangeAnimation(singleAnimationID);
  }
  else
  {
    pPlayer->GetCharaDrawInstance()->ChangeAnimation(rideAnimationID);
    pRideModel->GetCharaDrawInstance()->ChangeAnimation(rideAnimationID);
  }
}


GFL_NAMESPACE_END( Field );