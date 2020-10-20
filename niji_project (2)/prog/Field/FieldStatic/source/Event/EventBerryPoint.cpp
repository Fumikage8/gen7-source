/**
 *  GAME FREAK inc.
 *
 *  @file   EventBerryPoint.cpp
 *  @brief  きのみイベント
 *  @author Masanori Kanamaru
 *  @date   2015.10.08
 *
 */

//gfl2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_std_string.h>

#include "GameSys/include/GameData.h"
#include "System/include/GflUse.h"

#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/PlacementData/FieldPlacementDataManager.h"

#include "Field/FieldStatic/include/Event/EventBerryPoint.h"
#include "Field/FieldStatic/include/Event/EventBattleCall.h"

#include "Field/FieldStatic/include/Encount/FieldEncountWork.h"
#include "Field/FieldStatic/include/Encount/FieldEncountPokeSet.h"

#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Encount/FieldEncount.h"

#include "FieldScript/include/EventScriptCall.h"

#include "Field/FieldRo/include/PlacementData/actor/FieldBerryPointActor.h"

// スクリプト
#include "niji_conv_header/field/script/inc/berry.inc"

GFL_NAMESPACE_BEGIN( Field );

/**
 *  @brief きのみイベントの実行
 */
GameSys::GameEvent* EventBerryPoint::CallBerryPointEvent(
  GameSys::GameEventManager* pGameEventManager,
  const FieldBerryLottedData* pData,
  const gfl2::math::Vector3* pPosition)
{
  EventBerryPoint* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, EventBerryPoint>(pGameEventManager);
  pEvent->SetData(pData);
  pEvent->pPosition = pPosition;
  return pEvent;
}

/**
  *  @brief コンストラクタ
  */
EventBerryPoint::EventBerryPoint(gfl2::heap::HeapBase* pHeap)
  : GameSys::GameEvent(pHeap)
  , gottenAmount(0)
  , pPosition(NULL)
  , desc()
{}

/**
  *  @brief デストラクタ
  */
EventBerryPoint::~EventBerryPoint()
{}

/**
 * @brief 実行に必要なデータの設定
 */
void EventBerryPoint::SetData(const FieldBerryLottedData* pData)
{
  gfl2::std::MemCopy(pData, &(this->desc), sizeof(FieldBerryLottedData));
}

//--------------------------------------------------------------
/**
  * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  * @retval  true  起動してよい
  * @retval  false 起動しない
  */
//--------------------------------------------------------------
bool EventBerryPoint::BootChk(GameSys::GameManager* pGameManager)
{
  return true;
}

//--------------------------------------------------------------
/**
  * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  */
//--------------------------------------------------------------
void EventBerryPoint::InitFunc(GameSys::GameManager* pGameManager)
{
}

//--------------------------------------------------------------
/**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
  */
//--------------------------------------------------------------
GameSys::GMEVENT_RESULT EventBerryPoint::MainFunc(GameSys::GameManager* pGameManager)
{
  enum 
  {
    SEQ_START_ROTATION,
    SEQ_WAIT_ROTATION,
    SEQ_ENCOUNT,
    SEQ_BATTLE_START,
    SEQ_BATTLE_RESULT,
    SEQ_GET_BERRYS,
    SEQ_REMOVE_BERRY,

    SEQ_END,
  };

  switch (this->GetSeqNo())
  {
  case SEQ_START_ROTATION :
    {
      enum
      {
        ThresoldDegree = 20,
        RotationFrame = 10,
      };

      MoveModel::FieldMoveModelManager* pMoveModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
      MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

      const gfl2::math::Vector3 direction = ((*this->pPosition) - pPlayer->GetPositionPrevUpdateStart()).Normalize();

      const gfl2::math::Vector3 forward = pPlayer->GetCharaDrawInstance()->GetRotationQuat().Transform(gfl2::math::Vector3(0,0,1));

      f32 dot = gfl2::math::Abs((direction).Dot(forward));
      f32 thresold = gfl2::math::CosDeg(ThresoldDegree);

      if(dot > thresold)
      {
        this->SetSeqNo(SEQ_ENCOUNT);
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

      // 大森さん承認：回転後の不要な待ちをカット
      pPlayer->SetActionCommandRotation(MoveModel::FIELD_ACTION_COMMAND_ROTATION, y, 0, 0, RotationFrame, false );
      this->AddSeqNo();
    }
    break;
  case SEQ_WAIT_ROTATION :
    {
      MoveModel::FieldMoveModelManager* pMoveModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
      MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

      if (pPlayer->IsExecuteActionCommand())
      {
        break;
      }

      // 待機モーションを再生する @note レスポンス最適化のため、回転後の待機遷移をカットしているため、自前で設定
      {
        // 待機モーションを再生する
        pPlayer->GetCharaDrawInstance()->ChangeAnimation( (u32)( pPlayer->GetDefaultIdleAnimationId() ) );

        // ライドポケモンの待機モーションも再生する
        if( pPlayer->GetOnMoveModel() )
        {
          pPlayer->GetOnMoveModel()->GetCharaDrawInstance()->ChangeAnimation( pPlayer->GetDefaultIdleAnimationId() );
        }
      }

      this->AddSeqNo();
    }
    break;
  case SEQ_ENCOUNT:         // エンカウント判定
    {
      if (this->desc.existMonster)
      {
        // モーション再生
        FieldBerryPointActor* pActor = pGameManager->GetFieldResident()->GetPlacementDataManager()->SearchZoneBerryPoint(this->desc.berryPointID);
        pActor->StartSwing();
        // SE再生
        Sound::PlaySE(SEQ_SE_FLD_KINOMISHAKE);
        
        // エンカウント演出
        EventScriptCall::CallScript(pGameManager, SCRID_BERRY_ENCOUNT, NULL );
        this->AddSeqNo();
      }
      else
      {
        //エンカウントしない場合はきのみ取得処理までジャンプ
        this->SetSeqNo(SEQ_GET_BERRYS);
      }
    }
    break;
  case SEQ_BATTLE_START:   // バトル処理開始
    {
      // モーション停止
      FieldBerryPointActor* pActor = pGameManager->GetFieldResident()->GetPlacementDataManager()->SearchZoneBerryPoint(this->desc.berryPointID);
      pActor->StopSwing();
      // SE停止
      Sound::StopSE(SEQ_SE_FLD_KINOMISHAKE);

      pGameManager->GetFieldmap()->GetEncount()->SetEncountPokeFromFirstRecord(pGameManager, this->desc.encountTableID, this->desc.monsterLevel, false);
      u32 callOpt = EVBTL_CALLOPT_NOT_SCRIPT;
      EventBattleCall::CallWild(pGameManager->GetGameEventManager(), pGameManager, pGameManager->GetGameData()->GetEncountWork()->GetPokeSet(), callOpt);
      this->AddSeqNo();
    }
    break;
  case SEQ_BATTLE_RESULT:   // バトル結果の確認
    {
      u32 result = EventBattleCall::GetWildBattleResult(pGameManager->GetGameData()->GetLastBattleResult());
      if ((result == BTL_RESULT_WILD_RUN) || (result == BTL_RESULT_WILD_LOSE)) // バトルから逃げた or 負けた場合はイベント終了
      {
        this->SetSeqNo(SEQ_END);
      }
      else
      {
        // レコード値加算 : ヤシガニ倒した回数
        Savedata::IncRecord( Savedata::Record::RECID_YASHIGANI);
        this->AddSeqNo();
      }
    }
    break;
  case SEQ_GET_BERRYS:    // きのみ取得 : 指定回数獲得するまでループする
    {
      this->gottenAmount += 1;

      KANAMARU_PRINT("Get Berry: %d / %d \n", this->gottenAmount, this->desc.berryAmount);

      u32 itemID;
      //きのみ抽選
      // レアきのみが取れる(大量である)ならば、1つ目は必ずレアきのみ
      if (this->gottenAmount == 1 && this->desc.rareBerryItemID != 0)
      {
        itemID = this->desc.rareBerryItemID;
      }
      else
      {
        itemID = this->desc.berryItemIDSlots[System::GflUse::GetPublicRand(FieldBerryManager::BERRY_SLOT_AMOUNT)];
      }

      bool isFirst = this->gottenAmount == 1;
      bool isLast = this->gottenAmount == this->desc.berryAmount;
      u32 stateFlag = (isFirst << 0) | (isLast << 1);

      EventScriptCall::CallScript(pGameManager, SCRID_BERRY_GET, NULL, NULL, itemID, stateFlag, this->desc.berryPointID);
      if (this->gottenAmount >= this->desc.berryAmount)
      {
        // レコード値加算 : きのみ塊ゲットした回数
        if(this->desc.rareBerryItemID != 0)
        {
          Savedata::IncRecord( Savedata::Record::RECID_MANY_NUTS);
        }
        else
        {
          Savedata::IncRecord( Savedata::Record::RECID_NUTS);
        }

        this->AddSeqNo();
      }
    }
    break;  
  case SEQ_REMOVE_BERRY:  // きのみの除去, Actorの削除
    {
      pGameManager->GetGameData()->GetFieldBerryManager()->ConsumeBerry(&(this->desc));
      this->AddSeqNo();
    }
    break;
  case SEQ_END:
    {
      const u32 DEFAULT_DISABLE_FRAME = 8;
      MoveModel::FieldMoveModelManager* pMoveModelManager = pGameManager->GetGameData()->GetFieldCharaModelManager();
      MoveModel::FieldMoveModelPlayer*  pPlayer = static_cast<Field::MoveModel::FieldMoveModelPlayer*>( pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));
      pPlayer->DisableInterest(DEFAULT_DISABLE_FRAME);
    }

    return GameSys::GMEVENT_RES_FINISH;
  }

  return GameSys::GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
  * @brief   呼び出される関数 純粋仮想関数
  * @param   pGameManager       ゲームマネージャークラス
  */
//--------------------------------------------------------------
void EventBerryPoint::EndFunc(GameSys::GameManager* pGameManager)
{}

GFL_NAMESPACE_END( Field );
