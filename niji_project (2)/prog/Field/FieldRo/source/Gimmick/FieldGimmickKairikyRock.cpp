//=================================================================================================
/**
*  @file   FieldGimmickKairikyRock.h
*  @brief  フィールドギミック カイリキ岩
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
// niji
#include "GameSys/include/GameManager.h"
// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/Gimmick/FieldGimmickKairikyRock.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
// gfl2
#include <debug/include/gfl2_DebugPrint.h>
// conv
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include "niji_conv_header/field/chara/it0006_00_riderock21.h"
#include "niji_conv_header/field/chara/it0007_00_riderock22.h"
// debug
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldStatic/include/Debug/FieldDebugTypes.h"
// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockDown.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockSmoke2.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectKairikyRockDown2.h"
// サウンド
#include "Sound/include/Sound.h"
#include "niji_conv_header/sound/SoundMiddleID.h"
// イベント
#include "Field/FieldRo/source/EventCheck/FieldEventCheck.h"
#include "Field/FieldRo/include/Event/FieldEventKairikyRock.h"

// Script
#include "FieldScript/include/EventScriptCall.h"
#include "niji_conv_header/field/script/inc/z_f0701ph0501.inc"

GFL_NAMESPACE_BEGIN( Field )

const f32 INTERP_FRAME = 6;
const f32 BLOCK_SIZE   = 300.0f;

// コンストラクタ
FieldGimmickKairikyRock::FieldGimmickKairikyRock( gfl2::heap::HeapBase* pHeap, Fieldmap *pMap, u32 zoneId, PlacementDataManager* pPlacementDataManager ) 
: FieldGimmick             (pMap)
, m_deleteState            (0)
, m_pHeap                  (pHeap)
, m_zoneId                 (zoneId)
, m_pPlacementDataManager  (pPlacementDataManager)
, m_enableKairikyEvent     (false)
, m_isKairikyEventLocation (false)
, m_isKeepPushing          (false)
, m_eventState             (0)
, m_eventCnt               (0)
, m_eventRockActor         (NULL)
, m_eventInterpFrame       (0)
, m_pEffectKairikyRockSmoke(NULL)
, m_pEffectKairikyRockDown (NULL)
, m_pEffectKairikyRockSmokeL(NULL)
, m_pEffectKairikyRockDownL(NULL)
, m_padding                (0)
{
}

// デストラクタ
FieldGimmickKairikyRock::~FieldGimmickKairikyRock()
{
}

/**
 *  @brief  生成
 */
void FieldGimmickKairikyRock::Create( void )
{
  m_deleteState = 0;
  m_pEffectKairikyRockSmoke   = NULL;
  m_pEffectKairikyRockDown    = NULL;
  m_pEffectKairikyRockSmokeL  = NULL;
  m_pEffectKairikyRockDownL   = NULL;

  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  pEffectManager->LoadDynamicEffectResource ( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE , m_pHeap , true );
  pEffectManager->LoadDynamicEffectResource ( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN  , m_pHeap , true );
  pEffectManager->SetupDynamicEffectResource( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE , m_pHeap );
  pEffectManager->SetupDynamicEffectResource( Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN  , m_pHeap );
  

  // 巨大岩が存在するかどうかを確認する
  for (u32 index = 0; index < PlacementDataManager::PUSHROCK_ACTOR_MAX; index++)
  {
    FieldMoveModelActor* const pActor(m_pPlacementDataManager->GetPushRockActor(index));
    if (pActor && pActor->GetMoveModel())
    {
      if (pActor->GetMoveModel()->GetCharacterId() == CHARA_MODEL_ID_IT0009_00_RIDEROCK24)
      {
        // 存在する場合は専用エフェクトをセットアップ
        pEffectManager->LoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L, m_pHeap, true);
        pEffectManager->LoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L, m_pHeap, true);
        pEffectManager->SetupDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L, m_pHeap);
        pEffectManager->SetupDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L, m_pHeap);
        break;
      }
    }
  }

}

/**
 *  @biref  破棄更新
 *  @return 破棄が完了したかどうか
 *
 *  破棄が完了するまでマイフレーム呼出がなされる関数
 */
bool FieldGimmickKairikyRock::Finalize( void )
{
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  switch( m_deleteState )
  {
  case 0:
    // エフェクトの削除リクエスト
    if( m_pEffectKairikyRockSmoke)
    { 
      pEffectManager->DeleteEffect(m_pEffectKairikyRockSmoke);
      m_pEffectKairikyRockSmoke = NULL;
    }
    if( m_pEffectKairikyRockDown )
    {
      pEffectManager->DeleteEffect(m_pEffectKairikyRockDown);
      m_pEffectKairikyRockDown = NULL;
    }
    if (m_pEffectKairikyRockSmokeL)
    {
      pEffectManager->DeleteEffect(m_pEffectKairikyRockSmokeL);
      m_pEffectKairikyRockSmokeL = NULL;
    }
    if (m_pEffectKairikyRockDownL)
    {
      pEffectManager->DeleteEffect(m_pEffectKairikyRockDownL);
      m_pEffectKairikyRockDownL = NULL;
    }
    m_deleteState++;
    return false;
  case 1:
    // エフェクトリソースの削除
    pEffectManager->UnLoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE, m_pHeap, true );
    pEffectManager->UnLoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN , m_pHeap, true );
    pEffectManager->UnLoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L, m_pHeap, true);    // ┬ セットアップされていない場合は内部でアンロード処理がスルーされる
    pEffectManager->UnLoadDynamicEffectResource(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L, m_pHeap, true);     // ┘
    m_deleteState++;
    return true;
  }
  return true;
}

/**
 *  @brief  更新
 */
bool FieldGimmickKairikyRock::ExecuteKairiky( void )
{
  if( !m_enableKairikyEvent )
  {
    return true;
  }

  // エフェクトの生存チェック
  if( m_pEffectKairikyRockSmoke && !m_pEffectKairikyRockSmoke->IsAlive() )
  { 
    GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->DeleteEffect( m_pEffectKairikyRockSmoke );
    m_pEffectKairikyRockSmoke = NULL;
  }
  if( m_pEffectKairikyRockDown  && !m_pEffectKairikyRockDown->IsAlive() )
  { 
    GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->DeleteEffect( m_pEffectKairikyRockDown  );
    m_pEffectKairikyRockDown = NULL;
  }
  if (m_pEffectKairikyRockSmokeL && !m_pEffectKairikyRockSmokeL->IsAlive())
  {
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager()->DeleteEffect(m_pEffectKairikyRockSmokeL);
    m_pEffectKairikyRockSmokeL = NULL;
  }
  if (m_pEffectKairikyRockDownL  && !m_pEffectKairikyRockDownL->IsAlive())
  {
    GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager()->DeleteEffect(m_pEffectKairikyRockDownL);
    m_pEffectKairikyRockDownL = NULL;
  }

  Field::MoveModel::FieldMoveModelPlayer* pPlayer( m_pFieldmap->GetPlayerCharacter() );
  Field::MoveModel::FieldMoveModel*       pRide  ( pPlayer->GetOnMoveModel() );
  Field::MoveModel::FieldMoveModel*       pRock  ( m_eventRockActor->GetMoveModel() );

  // ステートに係わらず常に行う処理
  {

    // イベントの開始地点へ吸着
    if( m_eventCnt < m_eventInterpFrame )
    {
      // 1フレームでの移動の補完分を足しこむ
      pPlayer->GetCharaDrawInstance()->SetPosition( pPlayer->GetCharaDrawInstance()->GetPosition() + m_eventOffsetPos );

      // 1フレームでの姿勢の補完分を足しこむ
      gfl2::math::Quaternion playerQua( m_eventStartQua );
      playerQua.Slerp( m_eventTargetQuaForPlayer , (f32)m_eventCnt / (f32)m_eventInterpFrame );
      pPlayer->GetCharaDrawInstance()->SetRotationQuat( playerQua );

      m_eventCnt++;
    }
  }

  // カイリキーの動き始めステート
  if( m_eventState == 0 )
  {
    // 動作コードの停止時にデフォルトモーションに書き換えられえることを防ぐ
    pPlayer->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND);
      pRide->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND);

    // 歩数がカウントされエンカウントされるのを防ぐ
    pPlayer->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_STEP_CNT);
      pRide->OnStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_STEP_CNT);

    // カイリキ開始モーションを再生
    pPlayer->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_RI095_STRENGTHPUSH01);
      pRide->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_RI095_STRENGTHPUSH01);

    // 岩の動きモーションを再生
    pRock->GetCharaDrawInstance()->ChangeAnimation(IT0006_00_RIDEROCK21_F);

    // モーションによる移動方向を制御
    pPlayer->SetCustomValueForOrigin(1.0f, m_eventTargetQuaForPlayer);
    pRide->SetCustomValueForOrigin(1.0f, m_eventTargetQuaForPlayer);
    pRock->SetCustomValueForOrigin(1.0f, m_eventTargetQuaForRock);

    // 岩の動きエフェクトを再生
    if (pRock->GetCharacterId() == CHARA_MODEL_ID_IT0009_00_RIDEROCK24)
    {
      // フェローチェマップ用の巨大岩の場合
      if (!m_pEffectKairikyRockSmokeL)
      {
        m_pEffectKairikyRockSmokeL = reinterpret_cast<Field::Effect::EffectKairikyRockSmoke2*>(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager()->CreateEffect(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE_L, pRock));
        // [fix]GFCHECK[2651] CreateEffectがNULLを返した際の対応
        if (m_pEffectKairikyRockSmokeL)
        {
          m_pEffectKairikyRockSmokeL->Start();
          m_pEffectKairikyRockSmokeL->SetQuaternion(m_eventTargetQuaForPlayer);
        }
      }
    }
    else
    {
      // 通常の岩の場合
      if (!m_pEffectKairikyRockSmoke)
      {
        m_pEffectKairikyRockSmoke = reinterpret_cast<Field::Effect::EffectKairikyRockSmoke*>(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager()->CreateEffect(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_SMOKE, pRock));
        // [fix]GFCHECK[2651] CreateEffectがNULLを返した際の対応
        if (m_pEffectKairikyRockSmoke)
        {
          m_pEffectKairikyRockSmoke->Start();
          m_pEffectKairikyRockSmoke->SetQuaternion(m_eventTargetQuaForPlayer);
        }
      }
    }
    

    m_eventState++;
  }
  // カイリキーの動き始め待ちステート
  else if( m_eventState == 1 )
  {
    // モーションによる移動方向を制御
    pPlayer->SetCustomValueForOrigin( 1.0f , m_eventTargetQuaForPlayer );
      pRide->SetCustomValueForOrigin( 1.0f , m_eventTargetQuaForPlayer );
      pRock->SetCustomValueForOrigin( 1.0f , m_eventTargetQuaForRock   );

    if( pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame() )
    {
      // 岩が穴に落ちたか調べる
      u32 outNearestRockIndex = 0;
      u32 outNearestRockX     = 0;
      u32 outNearestRockZ     = 0;
      b32 isHole              = false;
      if( m_pPlacementDataManager->GetPushRockMatrix( m_zoneId, m_eventRockActor->GetMoveModel()->GetLocalSRT().translate, &outNearestRockIndex, &outNearestRockX, &outNearestRockZ ) )
      {
        // 穴に落ちているかどうか
        u32 tempHoleId;
        if( m_pPlacementDataManager->GetPushRockHoleID( m_zoneId, outNearestRockIndex, outNearestRockX, outNearestRockZ, &tempHoleId ) )
        {
          // 穴を防ぐ処理
          m_pPlacementDataManager->UnRegistZonePushRockHoleAndSetSaveWork( m_pFieldmap->GetGameManager()->GetGameData() , tempHoleId, pRock->GetEventId() );

          // 穴に落ちるモーションを再生
          pRock->GetCharaDrawInstance()->ChangeAnimation(IT0006_00_RIDEROCK21_D);

          // 穴に落ちるモーションによりY軸移動ができるように押し返しを無効
          pRock->SetCollisionSceneContainerForGround(NULL);

          isHole = true;
          m_isKeepPushing = false;      // 押し続け状態を解除
        }
      }

      // 岩が穴に落ちていなかったら
      b32 isContinue = false;
      if( !isHole )
      {
        // 更に一つ奥の番目情報を取得し壁ならSEを再生
        gfl2::math::Vector3 tempBlockPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventTargetQuaForRock.Transform( gfl2::math::Vector3( 0.0f, 0.0f ,-BLOCK_SIZE ) ) );
        u32 outTempRockIndex = 0;
        u32 outTempRockX     = 0;
        u32 outTempRockZ     = 0;
        if( m_pPlacementDataManager->GetPushRockMatrix( m_zoneId, tempBlockPos, &outTempRockIndex, &outTempRockX, &outTempRockZ ) )
        {
          // 壁だったら壁とぶつかりSEを再生及び押し続け状態を解除
          if( m_pPlacementDataManager->GetPushRockGridType(m_zoneId,outTempRockIndex,outTempRockX,outTempRockZ) == FieldPushRockAccessor::DEF_WALL )
          {
            Sound::PlaySE(SEQ_SE_FLD_0011);
            m_isKeepPushing = false;
          }
        }

        // ストレングス状態以外且つ押し続け状態ではない場合は処理を中断
        if (pPlayer->IsInputDashButton() || pPlayer->IsRunLoopState() || m_isKeepPushing)
        {
          // 岩の移動先の番目情報を取得
          gfl2::math::Vector3 newBlockPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventTargetQuaForRock.Transform( gfl2::math::Vector3( 0.0f, 0.0f ,-BLOCK_SIZE ) ) );
          u32 outNewRockIndex = 0;
          u32 outNewRockX     = 0;
          u32 outNewRockZ     = 0;
          if( m_pPlacementDataManager->GetPushRockMatrix( m_zoneId, newBlockPos, &outNewRockIndex, &outNewRockX, &outNewRockZ ) )
          {
            // 岩の移動先が有効かどうかを取得
            if( EnablePush( m_zoneId,outNewRockIndex,outNewRockX,outNewRockZ ) )
            {
              isContinue = true;
            }
          }
        }

        // 岩を待機状態にする
        pRock->GetCharaDrawInstance()->ChangeAnimation(IT0006_00_RIDEROCK21);
      }

      if( !isContinue )
      {
        // 岩を押す音を停止
        Sound::StopSE(SEQ_SE_FLD_0009);

        // 岩の動きエフェクトを停止
        if(m_pEffectKairikyRockSmoke)
        {
          m_pEffectKairikyRockSmoke->EndFade();
        }
        if (m_pEffectKairikyRockSmokeL)
        {
          m_pEffectKairikyRockSmokeL->EndFade();
        }

        // カイリキ終了モーションを再生
        pPlayer->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_RI096_STRENGTHPUSH01_END);
          pRide->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_RI096_STRENGTHPUSH01_END);

        m_eventState++;
      }
      else
      {
        m_eventState = 0;
      }
    }
  }
  // カイリキーの終了待ちステート
  else if( m_eventState == 2 )
  {
    // 岩が穴に落ちているとき
    const u32 EFFECT_FRAME = 18;
    if( pRock->GetCharaDrawInstance()->GetAnimationId() == IT0006_00_RIDEROCK21_D && pRock->GetCharaDrawInstance()->GetAnimationFrame() == EFFECT_FRAME )
    {
      // 岩が落ちる音を再生
      Sound::PlaySE(SEQ_SE_FLD_0010);
     
      // 岩落下エフェクト / フェローチェマップ用スクリプト
      if (pRock->GetCharacterId() == CHARA_MODEL_ID_IT0009_00_RIDEROCK24)
      {
        // フェローチェマップ用の巨大岩の場合
        if (!m_pEffectKairikyRockDownL)
        {
          // 専用スクリプト起動
          EventScriptCall::CallScript(GFL_SINGLETON_INSTANCE(GameSys::GameManager), SCRID_Z_F0701PH0501_ENCOUNT_BEAUTY, NULL, NULL, 0);

          m_pEffectKairikyRockDownL = reinterpret_cast<Field::Effect::EffectKairikyRockDown2*> (GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager()->CreateEffect(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN_L, m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + gfl2::math::Vector3(0.0f, BLOCK_SIZE, -BLOCK_SIZE)));
        }
        else
        {
          m_pEffectKairikyRockDownL->End();
        }
        // [fix]GFCHECK[2651] CreateEffectがNULLを返した際の対応
        if (m_pEffectKairikyRockDownL)
        {
          m_pEffectKairikyRockDownL->Start();
        }
        
      }
      else
      {
        // 通常の岩の場合
        if (!m_pEffectKairikyRockDown)
        {
          m_pEffectKairikyRockDown = reinterpret_cast<Field::Effect::EffectKairikyRockDown*> (GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager()->CreateEffect(Field::Effect::EFFECT_TYPE_KAIRIKY_ROCK_DOWN, m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + gfl2::math::Vector3(0.0f, BLOCK_SIZE, 0.0f)));
        }
        else
        {
          m_pEffectKairikyRockDown->End();
        }
        // [fix]GFCHECK[2651] CreateEffectがNULLを返した際の対応
        if (m_pEffectKairikyRockDown)
        {
          m_pEffectKairikyRockDown->Start();
        }
        
      }
      
    }

    // カイリキ終了モーションが再生しきると待機モーションを再生
    if( pPlayer->GetCharaDrawInstance()->IsAnimationLastFrame() )
    {
      pPlayer->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_RI080_WAIT01);
        pRide->GetCharaDrawInstance()->ChangeAnimation(P2_BASE_RI080_WAIT01);
      m_eventState++;
    }
  }
  else if( m_eventState == 3 )
  {
    // 岩が落ちるエフェクトが再生しきるまで待ち待機状態に戻す
    bool isAlive = true;
    if (pRock->GetCharacterId() == CHARA_MODEL_ID_IT0009_00_RIDEROCK24)
    {
      // フェローチェマップ用の巨大岩の場合
      if (!m_pEffectKairikyRockDownL || !m_pEffectKairikyRockDownL->IsAlive())
      {
        isAlive = false;
      }
    }
    else
    {
      // 通常の岩の場合
      if (!m_pEffectKairikyRockDown || !m_pEffectKairikyRockDown->IsAlive())
      {
        isAlive = false;
      }
    }
    if (!isAlive)
    {
      pPlayer->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND);
      pRide->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_DEFAULTMOITON_TO_SUSPEND);
      pPlayer->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_STEP_CNT);
      pRide->OffStateBit(Field::MoveModel::FIELD_MOVE_MODEL_DISABLE_STEP_CNT);
      m_enableKairikyEvent = false;
      return true;
    }
    
  }

  return false;
}

/**
 *  @brief  イベント起動チェック
 */
bool FieldGimmickKairikyRock::EventRequestCheck( const EventCheckRequest *cpReq )
{
  m_isKairikyEventLocation = false;

  // フィールドがないなら処理を中断
  if( !m_pFieldmap )
  {
    return false;
  }

  // イベント中なら処理を中断
  Field::MoveModel::FieldMoveModelPlayer* pPlayer( m_pFieldmap->GetPlayerCharacter() );
  if( !pPlayer || pPlayer->GetEventRequestCnt() || m_enableKairikyEvent )
  {
    return false;
  }

  // 現在のゾーンにあるプレイヤーから一番近い岩を取得
  gfl2::math::Vector3 playerPos( pPlayer->GetLocalSRT().translate );
  f32 minLength = -1.0f;
  m_eventRockActor = NULL;
  for( u32 index=0 ; index<PlacementDataManager::PUSHROCK_ACTOR_MAX ; index++ )
  {
    FieldMoveModelActor* pActor( m_pPlacementDataManager->GetPushRockActor( index ) );
    if( pActor && pActor->GetMoveModel() )
    {
      gfl2::math::Vector3 defVec( playerPos - pActor->GetMoveModel()->GetLocalSRT().translate );
      if( minLength < 0.0f || minLength > defVec.Length() )
      {
        m_eventRockActor = pActor;
        minLength        = defVec.Length();
      }
    }
  }

  // 一つも岩が存在しないなら処理を中断
  if( !m_eventRockActor )
  {
    return false;
  }

  // 一番近い岩の番目情報を取得
  u32 outNearestRockIndex = 0;
  u32 outNearestRockX     = 0;
  u32 outNearestRockZ     = 0;
  if( !m_pPlacementDataManager->GetPushRockMatrix( m_zoneId, m_eventRockActor->GetMoveModel()->GetLocalSRT().translate, &outNearestRockIndex, &outNearestRockX, &outNearestRockZ ) )
  {
    return false;
  }

  // カイリキを発動させる条件を取得
  f32 thresholdAngle   ( 89.0f);
  f32 thresholdDistance(145.0f);
#if PM_DEBUG
  // デバッグメニューから
  Debug::FieldDebugMenu* pFieldDebugMenu = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetFieldDebugMenu();
  if( pFieldDebugMenu && pFieldDebugMenu->IsUseKairikyDebugValue() )
  {
    thresholdAngle    = pFieldDebugMenu->GetKairikyAngle();
    thresholdDistance = pFieldDebugMenu->GetKairikyDistance();
  }
#endif

  // プレイヤーが岩押し可能範囲内(距離)にいるかどうか調べる
  const f32 BLOCK_HALF_SIZE = 150.0f;
  gfl2::math::Quaternion offsetQuaForRock  (0.0f,0.0f,0.0f,1.0f);
  gfl2::math::Quaternion offsetQuaForPlayer(0.0f,0.0f,0.0f,1.0f);
  gfl2::math::Vector3      basePos(0.0f,0.0f,0.0f);
  gfl2::math::Vector3     frontPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventRockActor->GetMoveModel()->GetLocalSRT().rotation.Transform( gfl2::math::Vector3( 0.0f, 0.0f , BLOCK_HALF_SIZE ) ) );
  gfl2::math::Vector3     rightPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventRockActor->GetMoveModel()->GetLocalSRT().rotation.Transform( gfl2::math::Vector3( BLOCK_HALF_SIZE, 0.0f , 0.0f ) ) );
  gfl2::math::Vector3      backPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventRockActor->GetMoveModel()->GetLocalSRT().rotation.Transform( gfl2::math::Vector3( 0.0f, 0.0f ,-BLOCK_HALF_SIZE ) ) );
  gfl2::math::Vector3      leftPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventRockActor->GetMoveModel()->GetLocalSRT().rotation.Transform( gfl2::math::Vector3(-BLOCK_HALF_SIZE, 0.0f , 0.0f ) ) );
  if     ( gfl2::math::Vector3( playerPos - frontPos ).Length() < thresholdDistance )
  { 
    basePos = frontPos;
    offsetQuaForPlayer.RadianYToQuaternion( gfl2::math::ConvDegToRad(180.0f) );
    offsetQuaForRock.RadianYToQuaternion( gfl2::math::ConvDegToRad(0.0f) );
  }
  else if( gfl2::math::Vector3( playerPos - rightPos ).Length() < thresholdDistance )
  { 
    basePos = rightPos;
    offsetQuaForPlayer.RadianYToQuaternion( gfl2::math::ConvDegToRad(270.0f) );
    offsetQuaForRock.RadianYToQuaternion( gfl2::math::ConvDegToRad(90.0f) );
  }
  else if( gfl2::math::Vector3( playerPos -  backPos ).Length() < thresholdDistance )
  { 
    basePos = backPos;
    offsetQuaForPlayer.RadianYToQuaternion( gfl2::math::ConvDegToRad(0.0f) );
    offsetQuaForRock.RadianYToQuaternion( gfl2::math::ConvDegToRad(180.0f) );
  }
  else if( gfl2::math::Vector3( playerPos -  leftPos ).Length() < thresholdDistance )
  {
    basePos = leftPos;
    offsetQuaForPlayer.RadianYToQuaternion( gfl2::math::ConvDegToRad(90.0f) );
    offsetQuaForRock.RadianYToQuaternion( gfl2::math::ConvDegToRad(270.0f) );
  }
  else
  { 
    return false; 
  }

  // プレイヤーが動く方向を取得
  m_eventTargetQuaForPlayer = m_eventRockActor->GetMoveModel()->GetLocalSRT().rotation * offsetQuaForPlayer;
  // 岩が動く方向を取得(プレイヤーと岩ではジョイントの向きは同じだがモーションの移動方向が逆)
  m_eventTargetQuaForRock   = m_eventRockActor->GetMoveModel()->GetLocalSRT().rotation * offsetQuaForRock;

  // プレイヤーが岩押し可能範囲内(角度)にいるかどうか調べる
  gfl2::math::Vector3 baseToPlayerDir( playerPos - basePos );
  gfl2::math::Vector3 axisDir        ( m_eventTargetQuaForRock.Transform( gfl2::math::Vector3( 0.0f, 0.0f , 1.0f ) ) );
  baseToPlayerDir = baseToPlayerDir.Normalize();
  axisDir         = axisDir.Normalize();
  if ( gfl2::math::ConvRadToDeg( acosf( axisDir.Dot(baseToPlayerDir) ) ) > thresholdAngle ){ return false; }

  // ここまでくればカイリキ岩を押せる場所にいると断言できる
  m_isKairikyEventLocation = true;

  // カイリキ以外では処理を中断
  if( !pPlayer->GetOnMoveModel() || pPlayer->GetOnMoveModel()->GetCharacterId() != CHARA_MODEL_ID_PM0068_00 )
  {
    return false;
  }

  // ストレングス状態以外では処理を中断
  if( !pPlayer->IsInputDashButton() && !pPlayer->IsRunLoopState() )
  {
    return false;
  }

  // プレイヤーが岩の方に突っ込んでいるか調べる 
  gfl2::math::Vector3 playerDir( pPlayer->GetLocalSRT().rotation.Transform( gfl2::math::Vector3( 0.0f, 0.0f , 1.0f ) ) );
  gfl2::math::Vector3 rockDir  ( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate - basePos );
  playerDir = playerDir.Normalize();
  rockDir   = rockDir.Normalize();
  if( playerDir.Dot(rockDir) < 0.91f )
  {
    return false;
  }

  // 岩の移動先の番目情報を取得
  gfl2::math::Vector3 newBlockPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventTargetQuaForRock.Transform( gfl2::math::Vector3( 0.0f, 0.0f ,-BLOCK_SIZE ) ) );
  u32 outNewRockIndex = 0;
  u32 outNewRockX     = 0;
  u32 outNewRockZ     = 0;
  if( !m_pPlacementDataManager->GetPushRockMatrix( m_zoneId, newBlockPos, &outNewRockIndex, &outNewRockX, &outNewRockZ ) )
  {
    return false;
  }

  // 岩の移動先が有効かどうかを取得
  if( !EnablePush( m_zoneId,outNewRockIndex,outNewRockX,outNewRockZ ) )
  {
    return false;
  }

  // 押し続け状態にする（フェローチェマップ用の巨大岩のときのみ）
  if (m_eventRockActor->GetMoveModel()->GetCharacterId() == CHARA_MODEL_ID_IT0009_00_RIDEROCK24)
  {
    // フェローチェマップ用の巨大岩の場合
    m_isKeepPushing = true;
  }
  
  // 吸着にかけるフレーム数を取得
  m_eventInterpFrame = INTERP_FRAME;
#if PM_DEBUG
  // デバッグメニューから
  if( pFieldDebugMenu && pFieldDebugMenu->IsUseKairikyDebugValue() )
  {
    m_eventInterpFrame = pFieldDebugMenu->GetKairikyInterpFrame();
  }
#endif

  // プレイヤーの岩押し開始地点を取得
  const f32 MOTION_OFFSET = 130.0f;
  gfl2::math::Vector3 targetPos( m_eventRockActor->GetMoveModel()->GetLocalSRT().translate + m_eventTargetQuaForRock.Transform( gfl2::math::Vector3( 0.0f, 0.0f , MOTION_OFFSET + BLOCK_HALF_SIZE ) ) );

  // イベントを開始状態にする
  m_enableKairikyEvent = true;
  m_eventState         = 0;
  m_eventCnt           = 0;
  m_eventOffsetPos     = (targetPos - playerPos)/m_eventInterpFrame;
  m_eventStartQua      = pPlayer->GetLocalSRT().rotation;

  // 岩を押す音を再生
  Sound::PlaySE(SEQ_SE_FLD_0009);

  // 泣き声を再生
  Sound::PlaySE(SEQ_SE_FLD_0008);

  Field::Event::EventKairikyRock* pEvent = GAMESYSTEM_EVENT_CALL<GameSys::GameEventManager, Field::Event::EventKairikyRock>( cpReq->p_evman );
  pEvent->SetKairikyGimmick( this );
  return true;
}

b32 FieldGimmickKairikyRock::EnablePush(u32 zoneId,u32 index,u32 x,u32 z)
{
  // 押す先が壁なら押せない
  if( m_pPlacementDataManager->GetPushRockGridType(zoneId,index,x,z) == FieldPushRockAccessor::DEF_WALL )
  {
    return false;
  }

  // 押す先に他の岩がある
  for( u32 i=0 ; i<PlacementDataManager::PUSHROCK_ACTOR_MAX ; i++ )
  {
    FieldMoveModelActor* pActor( m_pPlacementDataManager->GetPushRockActor(i) );
    if( pActor && pActor->GetMoveModel() )
    {
      u32 tempIndex;
      u32 tempX;
      u32 tempZ;
      if( m_pPlacementDataManager->GetPushRockMatrix( zoneId, pActor->GetMoveModel()->GetLocalSRT().translate, &tempIndex, &tempX, &tempZ ) )
      {
        if( tempIndex == index 
         && tempX     == x
         && tempZ     == z )
        {
          return false;
        }
      }
    }
  }

  return true;
}

GFL_NAMESPACE_END( Field )
