//=================================================================================================
/**
*  @file   FieldMoveModelPlayer.cpp
*  @brief  フィールド用 プレイヤー動作モデル
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include <niji_conv_header/field/field_resident/field_resident_pack.h>
#include <niji_conv_header/field/chara/chara_model_id.h>
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldStatic/include/FieldNode.h"
#include "Field/FieldStatic/include/Sound/FieldSound.h"
#include "niji_conv_header/field/chara/p2_base_fi.h"
#include <niji_conv_header/field/attribute/attribute_id.h>
#include "../../include/MoveModel/FieldMoveModelPlayer.h"
#include "../../include/MoveCode/FieldMoveCode_define.h"
#include "../../include/MoveCode/FieldMoveCodeManager.h"
#include "../../include/MoveCode/FieldMoveCodeBase.h"
#include "../../include/MoveCode/FieldMoveCodePlayer.h"
#include "../../include/MoveCode/FieldMoveCodeRideMuurando.h"
#include "../../include/MoveModel/FieldMoveModelManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

// エフェクト
#include "Field/FieldRo/include/Effect/content/FieldEffectFootLand.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootSand.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootWater.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideKenta.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideMurando.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideKairiky.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectFootRideZimenba.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2RunSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2WalkSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectRoba2AccelSmoke.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectCharaBreath.h"

// RTTIデバッグシステム
#include "Field/FieldRo/include/RttiDebug/RttiEditor.h"
#include "Field/FieldRo/include/RttiDebug/RttiManager.h"

// コリジョンシステム
#include "Field/FieldRo/include/Collision/BaseShape.h"
#include "Field/FieldRo/include/Collision/DynamicActor.h"
#include "Field/FieldRo/include/Collision/CollisionSceneDefault.h"

// zoneData
#include "Field/FieldStatic/include/Zone/ZoneDataAccessor.h"

// debug
#include "Field/FieldRo/include/Debug/FieldDebugMenu.h"
#include "Field/FieldRo/include/Debug/FieldDebugMapJump.h"

#include "Field/FieldRo/include/Event/FieldEventPokemonRide.h"

namespace Field{ namespace MoveModel {

// ライドポケモンの動作コードID
const u32 FieldMoveModelPlayer::RIDE_POKEMON_MOVE_CODE_ARRAY[ RIDE_POKEMON_MAX_CNT ] = { FIELD_MOVE_CODE_RIDE_KENTAROS,
                                                                                         FIELD_MOVE_CODE_RIDE_SAMEHADER,
                                                                                         FIELD_MOVE_CODE_RIDE_MUURAND,
                                                                                         FIELD_MOVE_CODE_RIDE_ROBA2,
                                                                                         FIELD_MOVE_CODE_RIDE_KAIRIKY,
                                                                                         FIELD_MOVE_CODE_RIDE_LAPLACE };

// ライドポケモンのキャラID
const u32 FieldMoveModelPlayer::RIDE_POKEMON_CHARA_ID_ARRAY[ RIDE_POKEMON_MAX_CNT ] = { CHARA_MODEL_ID_PM0128_00,
                                                                                        CHARA_MODEL_ID_PM0319_00,
                                                                                        CHARA_MODEL_ID_PM0508_00,
                                                                                        CHARA_MODEL_ID_PM0840_00,
                                                                                        CHARA_MODEL_ID_PM0068_00,
                                                                                        CHARA_MODEL_ID_PM0131_00 };


const f32 FieldMoveModelPlayer::MAX_COLLISION_R = 100.0f; ///< コリジョン最大幅

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelPlayer::FieldMoveModelPlayer()
: m_lastSeaRideCharacterId           ( CHARA_MODEL_ID_PM0131_00 )  // 狙ってコンストラクタのみに記述しています
, m_pRideOnMoveModel                 ( NULL )
, m_pRideOnMoveModelParentDagNode    ( NULL )
, m_defaultMoveCodeId                ( FIELD_MOVE_CODE_NONE )
, m_pEffectDashSmoke                 ( NULL )
, m_pEffectSharkJet                  ( NULL )
, m_pEffectSharkRun                  ( NULL )
, m_pEffectSharkWait                 ( NULL )
, m_pEffectLaplaceWait               ( NULL )
, m_pEffectLaplaceFlow               ( NULL )
, m_pEffectMuurandoExclamationNormal ( NULL )
, m_pEffectMuurandoExclamationEx     ( NULL )
, m_isRideMoveModelPrevClearResource ( false )
, m_defaultIdleAnimationIdPrevRide   ( 0 )
{
  for( u32 i = 0; i < ROBA2_ACCEL_EFFECT_NUM; i++ )
  {
    m_pEffectRoba2AccelSmoke[ i ] = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelPlayer::~FieldMoveModelPlayer()
{
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(ワーク)
*
*  @param  pFieldMoveModelHeader  作成リクエスト
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::SetupWork(const FieldMoveModelHeaderWork* pFieldMoveModelHeaderWork)
{
  SetMoveCodeExtendWork( FIELD_MOVE_CODE_EXTEND_WORK_SIZE , m_moveCodeExtendWork );
  FieldMoveModel::SetupWork(pFieldMoveModelHeaderWork);

  // プレイヤーは描画状況を復帰させない
  DisableVisibleForReturn();
}

//----------------------------------------------------------------------------
/**
*  @brief  初期化(リソース)
*
*  @param  pHeap                  ヒープ
*  @param  pMyRenderingPipeLine   レンダリングパイプライン
*  @param  pCharaModelFactory     キャラリソースファクトリー
*  @param  pFieldMoveModelHeader  作成リクエスト
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::SetupResource(gfl2::heap::HeapBase*                        pHeap,
                                         Field::MyRenderingPipeLine*                  pMyRenderingPipeLine,
                                         poke_3d::model::CharaModelFactory*           pCharaModelFactory,
                                         poke_3d::model::DressUpModelResourceManager* pDressUpModelResourceManager,
                                         const FieldMoveModelHeaderResource*          pFieldMoveModelHeaderResource)
{
  FieldMoveModel::SetupResource(pHeap,pMyRenderingPipeLine,pCharaModelFactory,pDressUpModelResourceManager,pFieldMoveModelHeaderResource);

  // 足音SE初期化
  m_accessor.Initialize( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident()->GetResource(BL_IDX_FIELD_RESIDENT__FOOTSOUND_BIN), GFL_BOOL_CAST( IsMale() ) );
}

//----------------------------------------------------------------------------
/**
*  @brief  解放(ワーク)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::ClearWork(void)
{
  FieldMoveModel::ClearWork();

  m_pRideOnMoveModel                 = NULL;
  m_pRideOnMoveModelParentDagNode    = NULL;
  m_defaultMoveCodeId                = FIELD_MOVE_CODE_NONE;
  m_pEffectDashSmoke                 = NULL;
  m_pEffectSharkJet                  = NULL;
  m_pEffectSharkRun                  = NULL;
  m_pEffectSharkWait                 = NULL;
  m_pEffectLaplaceWait               = NULL;
  m_pEffectLaplaceFlow               = NULL;
  m_pEffectMuurandoExclamationNormal = NULL;
  m_pEffectMuurandoExclamationEx     = NULL;

  for( u32 i = 0; i < ROBA2_ACCEL_EFFECT_NUM; i++ )
  {
    m_pEffectRoba2AccelSmoke[ i ] = NULL;
  }
  
  m_isRideMoveModelPrevClearResource = false;
}

//----------------------------------------------------------------------------
/**
*  @brief  解放(リソース)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::ClearResource(void)
{
  // 乗っている動作モデルの情報を確保
  if( m_pRideOnMoveModel && m_pRideOnMoveModelParentDagNode )
  {
    m_isRideMoveModelPrevClearResource             = true;
    m_rideMoveModelHeaderWorkPrevClearResource     = m_pRideOnMoveModel->GetHeaderWork();
    m_rideMoveModelHeaderResourcePrevClearResource = m_pRideOnMoveModel->GetHeaderResource();
  }
  else
  {
    m_isRideMoveModelPrevClearResource = false;
  }

  RideOffMoveModel();
  FieldMoveModel::ClearResource();
}

//-----------------------------------------------------------------------------
/**
*  @brief  後更新(座標の決定後に必要な処理を行う)
*
*  @param  pCharaModelFactory  キャラリソースファクトリー
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::PostUpdate(poke_3d::model::CharaModelFactory* pCharaModelFactory)
{
  FieldMoveModel::PostUpdate(pCharaModelFactory);

  f32 prev_step = GetPrevAnimationStepFrame();
  f32 prev_frame = GetPrevAnimationFrame();
  f32 frame     = GetCharaDrawInstance()->GetAnimationFrame();

  // ループしたとき、フレームがジャンプしたときはprev_frameを無視する。
  if( !(gfl2::math::Abs((prev_frame + prev_step) - frame) <= EPSILON) )
  {
    prev_frame = frame;
  }

  u32 animationId = GetCharaDrawInstance()->GetAnimationId();
  m_accessor.Update( prev_frame, frame, animationId , GetGroundAttribute() );

  // レコード対応(歩数)
  if( !GetEventRequestCnt() && !IsExecuteActionCommand() && IsOneStepFrame() )
  {
    Savedata::IncRecord( Savedata::Record::RECID_WALK_COUNT );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  エフェクト生成
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::UpdateEffect(void)
{
  FieldMoveModel::UpdateEffect();

  UpdateEffectStepJump();
  UpdateEffectFoot();
  UpdateEffectBreath();
}

//-----------------------------------------------------------------------------
/**
*  @brief  エフェクト生成(段差ジャンプ)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::UpdateEffectStepJump(void)
{
  // 足跡エフェクトの対象アニメーションかどうか
  u32 animationId = GetCharaDrawInstance()->GetAnimationId();
  if( animationId != P2_BASE_FI051_STEPJUMP01_START   // プレイヤー
   && animationId != P2_BASE_RI019_STEPJUMP01         // ケンタロス
   && animationId != P2_BASE_RI054_STEPJUMP01         // ムーランド
   && animationId != P2_BASE_RI074_STEPJUMP01         // 地面馬
   && animationId != P2_BASE_RI097_STEPJUMP01 )       // カイリキー
  {
    return;
  }

  // 吐息エフェクトを出すフレームを取得
  u32 animationFrame     = GetCharaDrawInstance()->GetAnimationFrame();
  u32 animationLeftHand  = 0xffffffff;
  u32 animationRightHand = 0xffffffff;
  u32 animationLeftFoot  = 0xffffffff;
  u32 animationRightFoot = 0xffffffff;

  if( animationId == P2_BASE_FI051_STEPJUMP01_START )
  {
    animationLeftFoot  = 18;
    animationRightFoot = 18;
  }
  else if( animationId == P2_BASE_RI019_STEPJUMP01 )
  {
    animationLeftHand  = 18;
    animationRightHand = 18;
    animationLeftFoot  = 18;
    animationRightFoot = 18;
  }
  else if( animationId == P2_BASE_RI054_STEPJUMP01 )
  {
    animationLeftHand  = 17;
    animationRightHand = 17;
  }
  else if( animationId == P2_BASE_RI074_STEPJUMP01 )
  {
    animationLeftHand  = 16;
    animationRightHand = 16;
    animationLeftFoot  = 16;
    animationRightFoot = 16;
  }
  else if( animationId == P2_BASE_RI097_STEPJUMP01 )
  {
    animationLeftFoot  = 16;
    animationRightFoot = 16;
  }

  Effect::EffectManager* pEffectManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager();

  // 足跡エフェクトをだす
  if( m_pRideOnMoveModel )
  {
    poke_3d::model::CharaModel* pCharaModel = m_pRideOnMoveModel->GetCharaDrawInstance();
    if( animationFrame == animationLeftHand  )
    {
      u32 footIndex = pCharaModel->GetJointIndex("LHand");
      pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_STEP_SMOKE , gfl2::math::Vector3( pCharaModel->GetJointWorldPosition(footIndex).x , GetGroundHeight() , pCharaModel->GetJointWorldPosition(footIndex).z ) );
    }
    if( animationFrame == animationRightHand )
    {
      u32 footIndex = pCharaModel->GetJointIndex("RHand");
      pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_STEP_SMOKE , gfl2::math::Vector3( pCharaModel->GetJointWorldPosition(footIndex).x , GetGroundHeight() , pCharaModel->GetJointWorldPosition(footIndex).z ) );
    }
    if( animationFrame == animationLeftFoot  )
    {
      u32 footIndex = pCharaModel->GetJointIndex("LFoot");
      pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_STEP_SMOKE , gfl2::math::Vector3( pCharaModel->GetJointWorldPosition(footIndex).x , GetGroundHeight() , pCharaModel->GetJointWorldPosition(footIndex).z ) );
    }
    if( animationFrame == animationRightFoot )
    {
      u32 footIndex = pCharaModel->GetJointIndex("RFoot");
      pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_STEP_SMOKE , gfl2::math::Vector3( pCharaModel->GetJointWorldPosition(footIndex).x , GetGroundHeight() , pCharaModel->GetJointWorldPosition(footIndex).z ) );
    }
  }
  else
  {
    poke_3d::model::CharaModel* pCharaModel = GetCharaDrawInstance();
    if( animationFrame == animationLeftFoot  )
    {
      gfl2::math::Vector3 offset(0.0f,0.0f,10.0f);
      offset = GetLocalSRT().rotation.Transform(offset);
      u32 footIndex = pCharaModel->GetJointIndex("LFoot");
      pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_STEP_SMOKE , gfl2::math::Vector3( pCharaModel->GetJointWorldPosition(footIndex).x + offset.x, GetGroundHeight() , pCharaModel->GetJointWorldPosition(footIndex).z + offset.z ) );
    }
    if( animationFrame == animationRightFoot )
    {
      u32 footIndex = pCharaModel->GetJointIndex("RFoot");
      pEffectManager->CreateEffect( Field::Effect::EFFECT_TYPE_STEP_SMOKE , gfl2::math::Vector3( pCharaModel->GetJointWorldPosition(footIndex).x , GetGroundHeight() , pCharaModel->GetJointWorldPosition(footIndex).z ) );
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  エフェクト生成(吐息)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::UpdateEffectBreath(void)
{
  if( !Field::ZoneDataAccessor::IsCoolZone( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetZoneID() ) )
  {
    return;
  }

  // 吐息エフェクトの対象アニメーションかどうか
  u32 animationId = GetCharaDrawInstance()->GetAnimationId();
  if( animationId != P2_BASE_FI001_WAIT01         // ライドしていない
   && animationId != P2_BASE_FI005_WAIT05
   && animationId != P2_BASE_FI010_WALK01
   && animationId != P2_BASE_FI012_RUN01
   && animationId != P2_BASE_RI001_WAIT01         // ケンタロス
   && animationId != P2_BASE_RI003_RUN01
   && animationId != P2_BASE_RI012_RUSHWAIT01
   && animationId != P2_BASE_RI040_WAIT01         // ムーランド
   && animationId != P2_BASE_RI042_RUN01
   && animationId != P2_BASE_RI047_SEARCHWAIT01
   && animationId != P2_BASE_RI049_SEARCHWALK01
   && animationId != P2_BASE_RI080_WAIT01         // カイリキー
   && animationId != P2_BASE_RI082_RUN01
   && animationId != P2_BASE_RI087_STRENGTHWAIT01
   && animationId != P2_BASE_RI089_STRENGTHRUN01
   && animationId != P2_BASE_RI060_WAIT01         // 地面馬
   && animationId != P2_BASE_RI062_RUN01
   && animationId != P2_BASE_RI067_ACCELEWAIT01
   && animationId != P2_BASE_RI069_ACCELERUN01 )
  {
    return;
  }

  // 吐息エフェクトを出すフレームを取得
  const u32 ARRAY_SIZE = 4;
  u32 animationFrame = GetCharaDrawInstance()->GetAnimationFrame();
  u32 animationRightFoot[ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};

  // ライドしていない状態
  if( !m_pRideOnMoveModel )
  {
    switch( GetCharaDrawInstance()->GetAnimationId() )
    {
      // 待機
      case P2_BASE_FI001_WAIT01:
      {
        animationRightFoot[0] = 3;
        break;
      }
      // 待機(寒い)
      case P2_BASE_FI005_WAIT05:
      {
        animationRightFoot[0] = 20;
        break;
      }
      // 歩き
      case P2_BASE_FI010_WALK01:
      {
        animationRightFoot[0] = 19;
        break;
      }
      // 走り
      case P2_BASE_FI012_RUN01:
      {
        animationRightFoot[0] = 19;
        break;
      }
    }
  }
  // ケンタロス
  else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0128_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 待機
      case P2_BASE_RI001_WAIT01:
      {
        animationRightFoot[0] = 3;
        break;
      }
      // 走り
      case P2_BASE_RI003_RUN01:
      {
        animationRightFoot[0] = 3;
        break;
      }
      // ラッシュ待機
      case P2_BASE_RI012_RUSHWAIT01:
      {
        animationRightFoot[0] = 17;
        break;
      }
    }
  }
  // ムーランド
  else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0508_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 待機
      case P2_BASE_RI040_WAIT01:
      {
        animationRightFoot[0] = 0;
        break;
      }
      // 走り
      case P2_BASE_RI042_RUN01:
      {
        animationRightFoot[0] = 6;
        break;
      }
      // サーチ待機
      case P2_BASE_RI047_SEARCHWAIT01:
      {
        animationRightFoot[0] = 27;
        break;
      }
      // サーチ
      case P2_BASE_RI049_SEARCHWALK01:
      {
        animationRightFoot[0] =  7;
        animationRightFoot[2] = 38;
        break;
      }
    }
  }
  // カイリキー
  else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0068_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 待機
      case P2_BASE_RI080_WAIT01:
      {
        animationRightFoot[0] =  0;
        break;
      }
      // 走り
      case P2_BASE_RI082_RUN01:
      {
        animationRightFoot[0] =  8;
        break;
      }
      // ストレングス待機
      case P2_BASE_RI087_STRENGTHWAIT01:
      {
        animationRightFoot[0] = 15;
        break;
      }
      // ストレングス
      case P2_BASE_RI089_STRENGTHRUN01:
      {
        animationRightFoot[0] =  9;
        break;
      }
    }
  }
  // 地面馬
  else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0840_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 待機
      case P2_BASE_RI060_WAIT01:
      {
        animationRightFoot[0] =  0;
        break;
      }
      // 通常歩き
      case P2_BASE_RI062_RUN01:
      {
        animationRightFoot[0] = 6;
        break;
      }
      // アクセル待機
      case P2_BASE_RI067_ACCELEWAIT01:
      {
        animationRightFoot[0] = 31;
        break;
      }
      // アクセル
      case P2_BASE_RI069_ACCELERUN01:
      {
        animationRightFoot[0] = 14;
        break;
      }
    }
  }

  // 足跡エフェクトを出すフレームかどうか
  b32 isHit = false;
  for( u32 i=0 ; i<ARRAY_SIZE; i++ )
  {
    if( animationFrame == animationRightFoot[i] )
    {
      isHit = true;
      break;
    }
  }

  if( !isHit )
  {
    return;
  }

  // モデルの原点から足への相対座標を取得(ライド)
  static_cast<Field::Effect::EffectCharaBreath*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_CHARA_BREATH , this ) );
  if( m_pRideOnMoveModel )
  {
    static_cast<Field::Effect::EffectCharaBreath*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_CHARA_BREATH , m_pRideOnMoveModel ) );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  エフェクト生成(足跡)
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::UpdateEffectFoot(void)
{
  // ライドしているかどうか
  if( !m_pRideOnMoveModel || !m_pRideOnMoveModel->GetCharaDrawInstance() )
  {
    return;
  }

  // 足跡エフェクトの対象アニメーションかどうか(ライド)
  u32 animationId = m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId();
  if( animationId != P2_BASE_RI003_RUN01          // ケンタロス
   && animationId != P2_BASE_RI005_RUNSTOP01
   && animationId != P2_BASE_RI042_RUN01          // ムーランド  
   && animationId != P2_BASE_RI049_SEARCHWALK01
   && animationId != P2_BASE_RI082_RUN01          // カイリキー
   && animationId != P2_BASE_RI083_RUNSTART01
   && animationId != P2_BASE_RI084_RUNSTOP01
   && animationId != P2_BASE_RI089_STRENGTHRUN01
   && animationId != P2_BASE_RI088_STRENGTHRUNSTART01
   && animationId != P2_BASE_RI090_STRENGTHRUNSTOP01
   && animationId != P2_BASE_RI095_STRENGTHPUSH01       // ┬─ // @fix MMCat[153] カイリキーライド中の岩押し時に足跡が表示されない。対処、岩押し時のアニメーションを足跡対象に変更
   && animationId != P2_BASE_RI096_STRENGTHPUSH01_END   // ┘
   && animationId != P2_BASE_RI062_RUN01          // 地面馬
   && animationId != P2_BASE_RI064_RUNSTOP01
   && animationId != P2_BASE_RI069_ACCELERUN01
   && animationId != P2_BASE_RI070_ACCELERUNSTOP01
   )
  {
    return;
  }

  // 足跡エフェクトを出すフレームを取得(ライド)
  const u32 ARRAY_SIZE = 4;
  u32 animationFrame     = m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationFrame();
  u32 animationLeftHand [ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
  u32 animationRightHand[ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
  u32 animationLeftFoot [ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};
  u32 animationRightFoot[ARRAY_SIZE] = {0xffffffff,0xffffffff,0xffffffff,0xffffffff};

  // ケンタロス
  if     ( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0128_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 走り
      case P2_BASE_RI003_RUN01:
      {
        animationLeftHand [0] = 2;
        animationRightHand[0] = 1;
        animationLeftFoot [0] = 6;
        animationRightFoot[0] = 5;
        break;
      }
      // 走りストップ
      case P2_BASE_RI005_RUNSTOP01:
      {
        animationLeftHand [0] = 5;
        animationRightHand[0] = 4;
        animationLeftFoot [0] = 3;
        animationRightFoot[0] = 6;
        break;
      }
    }
  }
  // ムーランド
  else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0508_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 走り
      case P2_BASE_RI042_RUN01:
      {
        animationLeftHand [0] = 4;
        animationRightHand[0] = 3;
        animationLeftFoot [0] = 2;
        animationRightFoot[0] = 6;
        break;
      }
      // サーチ
      case P2_BASE_RI049_SEARCHWALK01:
      {
        animationLeftHand [0] = 10;
        animationRightHand[0] = 15;
        animationLeftFoot [0] = 17;
        animationRightFoot[0] =  7;

        animationLeftHand [1] = 25;
        animationRightHand[1] = 30;
        animationLeftFoot [1] = 32;
        animationRightFoot[1] = 21;

        animationLeftHand [2] = 39;
        animationRightHand[2] = 45;
        animationLeftFoot [2] = 47;
        animationRightFoot[2] = 38;

        animationLeftHand [3] = 56;
        animationRightHand[3] = 59;
        animationLeftFoot [3] = 60;
        animationRightFoot[3] = 63;
        break;
      }
    }
  }
  // カイリキー
  else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0068_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 走り
      case P2_BASE_RI082_RUN01:
      {
        animationLeftFoot [0] = 18;
        animationRightFoot[0] =  8;
        break;
      }
      // 走り開始
      case P2_BASE_RI083_RUNSTART01:
      {
        animationLeftFoot [0] =  2;
        animationRightFoot[0] =  3;
        animationLeftFoot [1] =  11;
        break;
      }
      // 走り終了
      case P2_BASE_RI084_RUNSTOP01:
      {
        animationLeftFoot [0] = 17;
        break;
      }
      // ストレングス
      case P2_BASE_RI089_STRENGTHRUN01:
      {
        animationLeftFoot [0] = 18;
        animationRightFoot[0] =  9;
        break;
      }
      // ストレングス開始
      case P2_BASE_RI088_STRENGTHRUNSTART01:
      {
        animationLeftFoot [0] =  3;
        animationRightFoot[0] =  5;
        animationLeftFoot [1] =  11;
        break;
      }
      // ストレングス終了
      case P2_BASE_RI090_STRENGTHRUNSTOP01:
      {
        animationLeftFoot [0] = 17;
        break;
      }
      // @fix MMCat[153] カイリキーライド中の岩押し時に足跡が表示されない。対処、岩押しアニメーションの足跡表示タイミング設定
      // 岩押し
      case P2_BASE_RI095_STRENGTHPUSH01:
      {
        animationRightFoot[0]   = 28;
        animationLeftFoot[0]    = 47;
        animationRightFoot[1]   = 87;
        animationLeftFoot[1]    = 108;
        break;
      }
      // @fix MMCat[153] カイリキーライド中の岩押し時に足跡が表示されない。対処、岩押し終了アニメーションの足跡表示タイミング設定
      // 岩押し終了
      case P2_BASE_RI096_STRENGTHPUSH01_END:
      {
        animationRightFoot[0] = 10;
        break;
      }
    }
  }
  // 地面馬
  else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0840_00 )
  {
    switch( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() )
    {
      // 通常歩き
      case P2_BASE_RI062_RUN01:
      {
        animationLeftHand [0] = 5;
        animationRightHand[0] = 14;
        animationLeftFoot [0] = 15;
        animationRightFoot[0] = 6;
        break;
      }
      // 通常歩き終了
      case P2_BASE_RI064_RUNSTOP01:
      {
        animationLeftHand [0] = 4;
        animationRightHand[0] = 8;
        animationLeftFoot [0] = 9;
        animationRightFoot[0] = 7;
        break;
      }
      // アクセル
      case P2_BASE_RI069_ACCELERUN01:
      {
        animationLeftHand [0] = 8;
        animationRightHand[0] = 5;
        animationLeftFoot [0] = 2;
        animationRightFoot[0] = 14;
        break;
      }
      // アクセル終了
      case P2_BASE_RI070_ACCELERUNSTOP01:
      {
        animationLeftHand [0] = 3;
        animationRightHand[0] = 5;
        animationLeftFoot [0] = 7;
        animationRightFoot[0] = 2;
        break;
      }
    }
  }

  u32 footIndex = 0;
  b32 isLeft    = false;
  b32 isHand    = false;
  b32 isHit     = false;
  for( u32 i=0 ; i<ARRAY_SIZE; i++ )
  {
    // 足跡エフェクトを出すフレームかどうか(ライド)
    if( animationFrame != animationLeftHand [i]
     && animationFrame != animationRightHand[i]
     && animationFrame != animationLeftFoot [i]
     && animationFrame != animationRightFoot[i] )
    {
      continue;
    }

    // 足跡エフェクトの対象を調べる(ライド)]
    if     ( animationFrame == animationLeftHand [i] ){ footIndex = m_pRideOnMoveModel->GetCharaDrawInstance()->GetJointIndex("LHand"); isLeft = true;  isHand = true;  }
    else if( animationFrame == animationRightHand[i] ){ footIndex = m_pRideOnMoveModel->GetCharaDrawInstance()->GetJointIndex("RHand"); isLeft = false; isHand = true;  }
    else if( animationFrame == animationLeftFoot [i] ){ footIndex = m_pRideOnMoveModel->GetCharaDrawInstance()->GetJointIndex("LFoot"); isLeft = true;  isHand = false; }
    else if( animationFrame == animationRightFoot[i] ){ footIndex = m_pRideOnMoveModel->GetCharaDrawInstance()->GetJointIndex("RFoot"); isLeft = false; isHand = false; }

    isHit = true;
    break;
  }

  if( !isHit )
  {
    return;
  }

  // モデルの原点から足への相対座標を取得(ライド)
  gfl2::math::Vector3 offsetPos( m_pRideOnMoveModel->GetCharaDrawInstance()->GetJointWorldMatrix(footIndex).GetElemPosition() - m_pRideOnMoveModel->GetCharaDrawInstance()->GetWorldMatrix().GetElemPosition() );
  offsetPos.y = 0.0f;

  // 特定地面属性の上にいるなら足跡エフェクトを作成する(ライド)
  if( GetGroundAttribute() == ATTR_SUNAHAMA
   || GetGroundAttribute() == ATTR_SABAKU_SUNA
   || GetGroundAttribute() == ATTR_YUKI )
  {
    // ケンタロス向け
    if     ( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0128_00 )
    {
      Field::Effect::EffectFootRideKenta* pEffect = static_cast<Field::Effect::EffectFootRideKenta*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KENTA , this ) );
      if( pEffect )
      {
        pEffect->SetOffset( isLeft ? false:true , offsetPos );
      }
    }
    // ムーランド向け
    else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0508_00 )
    {
      Field::Effect::EffectFootRideMurando* pEffect = static_cast<Field::Effect::EffectFootRideMurando*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_MURANDO , this ) );
      if( pEffect )
      {
        pEffect->SetOffset( isLeft ? false:true , offsetPos );
      }
    }
    // カイリキー向け
    else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0068_00 )
    {
      Field::Effect::EffectFootRideKairiky* pEffect = static_cast<Field::Effect::EffectFootRideKairiky*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_KAIRIKY , this ) );
      if( pEffect )
      {
        pEffect->SetOffset( isLeft ? false:true , offsetPos );
      }
    }
    // 地面馬向け
    else if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0840_00 )
    {
      Field::Effect::EffectFootRideZimenba* pEffect = static_cast<Field::Effect::EffectFootRideZimenba*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_RIDE_ZIMENBA , this ) );
      if( pEffect )
      {
        pEffect->SetOffset( isLeft ? false:true , offsetPos );
      }
    }
  }

  // 地面属性にあったパーティクルエフェクトを作成する(ライド)
  switch( GetGroundAttribute() )
  {
  case ATTR_ASASE:
    {
      Field::Effect::EffectFootWater* pEffect = static_cast<Field::Effect::EffectFootWater*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_WATER , GetCharaDrawInstance()->GetPosition() ) );
      if( pEffect )
      {
        // 水面の高さにあわせる
        const gfl2::math::Vector3 ADD_OFFSET(0.0f,20.0f,0.0f);
        pEffect->SetOffset( offsetPos + ADD_OFFSET ,gfl2::math::Vector3(0.0f,0.0f,0.0f) );
      }
      break;
    }
  case ATTR_SABAKU_SUNA:
  case ATTR_SUNAHAMA:
    {
      Field::Effect::EffectFootSand* pEffect = static_cast<Field::Effect::EffectFootSand*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_FOOT_SAND , GetCharaDrawInstance()->GetPosition() ) );
      if( pEffect )
      {
        pEffect->SetOffset( offsetPos,gfl2::math::Vector3(0.0f,0.0f,0.0f) );
      }
      break;
    }
  case ATTR_YUKI:
    {
      break;
    }
  }

  // 地面属性にあったパーティクルエフェクトを作成する(地面馬)
  if( m_pRideOnMoveModel->GetCharacterId() != CHARA_MODEL_ID_PM0840_00 )
  {
    return;
  }

  switch( GetGroundAttribute() )
  {
    // 地面馬のみが通れる地面
    case ATTR_HORSE_IWABA:
    {
      Field::Effect::EffectRoba2RunSmoke* pEffect = static_cast<Field::Effect::EffectRoba2RunSmoke*>( GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_ROBA2_RUN_SMOKE, GetCharaDrawInstance()->GetPosition() ) );
      if( pEffect )
      {
        pEffect->SetOffset( offsetPos,gfl2::math::Vector3(0.0f,0.0f,0.0f) );
      }

      break;
    }

    // その他の地面ではアクセル時に足元エフェクトを出す
    default:
    {
      if( m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() == P2_BASE_RI069_ACCELERUN01 || m_pRideOnMoveModel->GetCharaDrawInstance()->GetAnimationId() == P2_BASE_RI070_ACCELERUNSTOP01 )
      {
        u32 effectIndex = ROBA2_ACCEL_EFFECT_NUM;

        if ( isLeft )
        {
          if( isHand )    { effectIndex = 0; }
          else            { effectIndex = 1; }
        }
        else if( isHand ) { effectIndex = 2; }
        else              { effectIndex = 3; }
        

        if( effectIndex < ROBA2_ACCEL_EFFECT_NUM )
        {
          gfl2::math::Vector3 rotation;
          GetCharaDrawInstance()->GetRotationQuat().QuaternionToRadian( &rotation.x, &rotation.y, &rotation.z );

          if( m_pEffectRoba2AccelSmoke[ effectIndex ])
          {
            m_pEffectRoba2AccelSmoke[ effectIndex ]->Reset( GetCharaDrawInstance()->GetPosition() + offsetPos, rotation );
          }
        }
      }
      break;
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルに乗る
*
*  @param  pFieldMoveModel    乗りたい動作モデル
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::RideOnMoveModel (FieldMoveModel* pFieldMoveModel)
{
  // 既に指定された動作モデルに乗っていたら失敗
  if( m_pRideOnMoveModel && m_pRideOnMoveModel == pFieldMoveModel )
  {
    return;
  }

  // 降りたとき復帰するために必要な情報を確保
  m_pRideOnMoveModel               = pFieldMoveModel;
  m_pRideOnMoveModelParentDagNode  = m_pRideOnMoveModel->GetCharaDrawInstance()->GetModelInstanceNode()->GetParent();
  m_defaultMoveCodeId              = GetMoveCodeId();
  m_defaultIdleAnimationIdPrevRide = GetDefaultIdleAnimationId();

  // 子にする
  GetCharaDrawInstance()->GetModelInstanceNode()->AddChild( m_pRideOnMoveModel->GetCharaDrawInstance()->GetModelInstanceNode() );

  // 一瞬でセットポジションにする
  const u32 RIDE_OFFSET      = FieldMoveModelPlayer::GetRideIndex( m_pRideOnMoveModel->GetCharacterId() );
  const u32 MOTION_SLOT_WAIT = FieldMoveModelPlayer::RIDE_POKEMON_MOTION_START+0; // 待機、歩きループ、歩き開始、歩き終わり
  const u32 MOTION_WAIT_ID   = MOTION_SLOT_WAIT + FieldMoveModelPlayer::RIDE_POKEMON_MOTION_STRIDE * RIDE_OFFSET;
  m_pRideOnMoveModel->GetCharaDrawInstance()->ChangeAnimationSmooth( MOTION_WAIT_ID , 1 );
  m_pRideOnMoveModel->GetCharaDrawInstance()->UpdateAnimation();
  GetCharaDrawInstance()->ChangeAnimationSmooth( MOTION_WAIT_ID , 1 );
  GetCharaDrawInstance()->UpdateAnimation();

  // デフォルト待機モーションを変える
  SetDefaultIdleAnimationId( MOTION_WAIT_ID );

  // コリジョンの大きさをライド状態によってかえる
  for( u32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
  {
    Field::MoveModel::ApplicationDataForPlayer* pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetApplicationData( m_pRideOnMoveModel->GetCharacterId() );

    if( MAX_COLLISION_R != pPlayerData->collisionR )
    {
      GFL_ASSERT_MSG(false,"(collision) TestShapeSize != OwnShapeSize"); 
    }

    if( GetDynamicActor(i) )
    {
      if     ( GetDynamicActor(i)->GetShapeType() == BaseActor::SHAPE_SPHERE )
      {
        if( i==COLLISION_TYPE_EVENT_NAMINORI )
        {
          static_cast<SphereShape*>(GetDynamicActor(i)->GetShape())->m_r = pPlayerData->collisioFilterR;
        }
        else
        { 
          static_cast<SphereShape*>(GetDynamicActor(i)->GetShape())->m_r = pPlayerData->collisionR;
        }
      }
      else if( GetDynamicActor(i)->GetShapeType() == BaseActor::SHAPE_CYLINDER )
      {
        if( i==COLLISION_TYPE_TRAINER_EYE )
        {
          const f32 PLAYER_EYE_WIDTH = 41.0f; // コリジョン抜けをしないぎりぎりのサイズ
          static_cast<CylinderShape*>(GetDynamicActor(i)->GetShape())->m_r = PLAYER_EYE_WIDTH;
        }
        else
        {
          static_cast<CylinderShape*>(GetDynamicActor(i)->GetShape())->m_r = pPlayerData->collisionR; 
        }
      }
      else
      {
        GFL_ASSERT_MSG(false,"コリジョンシェイプが意図したものではない"); 
      }
    }
  }

  // 動作コードを変更
  ChangeMoveCode( (FIELD_MOVE_CODE_ID)( RIDE_POKEMON_MOVE_CODE_ARRAY[ RIDE_OFFSET ] ) );

  // 移動量を無効化する。
  m_pRideOnMoveModel->OnStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );

  if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0128_00 )
  {
    // エフェクトロード
    m_pEffectDashSmoke = static_cast<Field::Effect::EffectDashSmoke*>(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_DASH_SMOKE, this ));
  }
  if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0319_00 )
  {
    // エフェクトロード
    m_pEffectSharkJet  = static_cast<Field::Effect::EffectSharkJet* >(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_SHARK_JET , this ));
    m_pEffectSharkRun  = static_cast<Field::Effect::EffectSharkRun* >(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_SHARK_RUN , this ));
    m_pEffectSharkWait = static_cast<Field::Effect::EffectSharkWait*>(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_SHARK_WAIT, this ));
  }
  if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0508_00 )
  {
    // エフェクトロード
    // @fix GFNMCat[1172]:ムーランド状態のままイベントに遷移すると、エフェクト数が制限を超える場合がある、対処。発見アイコンエフェクトの確保先をイベント枠からライド枠に変更。
    // @fix NMcat[662]:「！」の吹き出しアイコンの優先順位が異なる対処、描画優先度が高い形式に変更
    m_pEffectMuurandoExclamationNormal = static_cast<Field::Effect::EffectExclamation1* >(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON1_OVERLAY, this, false, Effect::EffectManager::WORK_TYPE_RID ));
    m_pEffectMuurandoExclamationEx = static_cast<Field::Effect::EffectExclamation2* >(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_EXCLAMATION_ICON2_OVERLAY, this, false, Effect::EffectManager::WORK_TYPE_RID ));

    if( m_pEffectMuurandoExclamationNormal )
    {
      m_pEffectMuurandoExclamationNormal->SetMode( Field::Effect::EffectExclamation1::MODE_KEEP );
      m_pEffectMuurandoExclamationNormal->RequestCloseForKeepMode( false );
    }

    if( m_pEffectMuurandoExclamationEx )
    {
      m_pEffectMuurandoExclamationEx->SetMode( Field::Effect::EffectExclamation2::MODE_KEEP );
      m_pEffectMuurandoExclamationEx->RequestCloseForKeepMode( false );
    }
  }
  if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0131_00 )
  {
    // エフェクトロード
    m_pEffectLaplaceWait = static_cast<Field::Effect::EffectLaplaceWait* >(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_LAPLACE_WAIT , this ));
    m_pEffectLaplaceFlow = static_cast<Field::Effect::EffectLaplaceFlow* >(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_LAPLACE_FLOW , this ));
  }

  if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0840_00 )
  {
    // 地面馬エフェクトロード
    for( u32 i = 0; i < ROBA2_ACCEL_EFFECT_NUM; i++ )
    {
      m_pEffectRoba2AccelSmoke[ i ] = static_cast<Field::Effect::EffectRoba2AccelSmoke* >(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap()->GetEffectManager()->CreateEffect( Field::Effect::EFFECT_TYPE_ROBA2_ACCEL_SMOKE , this ));
    }
  }

  // 地面馬に乗る場合は地面馬壁のアクターを無効化
  if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0840_00 )
  {
    GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive( false );
  }
  else
  {
    GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive( true );
  }

  // ライド進入禁止用アクターを有効化
  GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive( true );
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作モデルから降りる
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::RideOffMoveModel(void)
{
  if( m_pRideOnMoveModel && m_pRideOnMoveModelParentDagNode )
  {
    // 一番最後に乗った海用ライドのキャラクターIDを保存
    if( m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0319_00 
     || m_pRideOnMoveModel->GetCharacterId() == CHARA_MODEL_ID_PM0131_00 )
    {
      m_lastSeaRideCharacterId = m_pRideOnMoveModel->GetCharacterId();
    }

    // 移動量を有効にする。
    m_pRideOnMoveModel->OffStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );

    // 元の動作コードに変える
    ChangeMoveCode( m_defaultMoveCodeId );
    // デフォルト待機モーションを元に戻す
    SetDefaultIdleAnimationId( m_defaultIdleAnimationIdPrevRide );
    // 子じゃなくする
    m_pRideOnMoveModelParentDagNode->AddChild( m_pRideOnMoveModel->GetCharaDrawInstance()->GetModelInstanceNode() );
    // 一瞬でセットポジションにする
    m_pRideOnMoveModel->GetCharaDrawInstance()->ChangeAnimationSmooth( m_defaultIdleAnimationIdPrevRide , 1 );
    m_pRideOnMoveModel->GetCharaDrawInstance()->UpdateAnimation();
    // 親のモーションも一瞬でセットポジションにする
    GetCharaDrawInstance()->ChangeAnimationSmooth( m_defaultIdleAnimationIdPrevRide , 1 );
    GetCharaDrawInstance()->UpdateAnimation();
    // コリジョンの大きさをライド状態によってかえる
    for( u32 i=0 ; i<COLLISION_TYPE_MAX ; i++ )
    {
      Field::MoveModel::ApplicationDataForPlayer* pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetApplicationData( GetCharacterId() );
      if( GetDynamicActor(i) )
      {
        if     ( GetDynamicActor(i)->GetShapeType() == BaseActor::SHAPE_SPHERE )
        {
          if( i==COLLISION_TYPE_EVENT_NAMINORI )
          {
            static_cast<SphereShape*>(GetDynamicActor(i)->GetShape())->m_r = pPlayerData->collisioFilterR;
          }
          else
          {
            static_cast<SphereShape*>(GetDynamicActor(i)->GetShape())->m_r = pPlayerData->collisionR;
          }
        }
        else if( GetDynamicActor(i)->GetShapeType() == BaseActor::SHAPE_CYLINDER )
        {
          if( i==COLLISION_TYPE_TRAINER_EYE )
          {
            const f32 PLAYER_EYE_WIDTH = 21.0f; // コリジョン抜けをしないぎりぎりのサイズ
            static_cast<CylinderShape*>(GetDynamicActor(i)->GetShape())->m_r = PLAYER_EYE_WIDTH;
          }
          else
          {
            static_cast<CylinderShape*>(GetDynamicActor(i)->GetShape())->m_r = pPlayerData->collisionR; 
          }
        }
        else
        { 
          GFL_ASSERT_MSG(false,"コリジョンシェイプが意図したものではない"); 
        }
      }
    }

    // 画面外に消す
    m_pRideOnMoveModel->GetCharaDrawInstance()->SetVisible( false );
    m_pRideOnMoveModel->GetCharaDrawInstance()->SetPosition( gfl2::math::Vector3(100000.0f,100000.0f,100000.0f) );
  }

  // 降りたとき復帰するために必要な情報をクリア
  m_pRideOnMoveModel               = NULL;
  m_pRideOnMoveModelParentDagNode  = NULL;
  m_defaultMoveCodeId              = FIELD_MOVE_CODE_NONE;
  m_defaultIdleAnimationIdPrevRide = 0;

  // ライド用SEが流れていたらストップ
  Sound::StopSE(SEQ_SE_FLD_RIDE_SAMEHADA_JET   );
  Sound::StopSE(SEQ_SE_FLD_RIDE_SAMEHADA_CRUISE);

  // ライド用エフェクトのアンロード
  {
    Fieldmap* pFieldmap=GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldmap();
    if ( pFieldmap == NULL )
    {
      return; //この後の処理はスキップ
    }

    // @fix GFNMCat[1156] ムーランドにライドした状態で特定手順を行った後、再度ムーランドにライドするとハングする 
    // フィールドClose時も確実にメンバ変数をクリアする。
    Effect::EffectManager * pEffectManager = pFieldmap->GetEffectManager();

    if( m_pEffectDashSmoke                 )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectDashSmoke                 ); 
      m_pEffectDashSmoke                 = NULL; 
    }
    if( m_pEffectSharkJet                  )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectSharkJet                  ); 
      m_pEffectSharkJet                  = NULL; 
    }
    if( m_pEffectSharkRun                  )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectSharkRun                  ); 
      m_pEffectSharkRun                  = NULL; 
    }
    if( m_pEffectSharkWait                 )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectSharkWait                 ); 
      m_pEffectSharkWait                 = NULL; 
    }
    if( m_pEffectLaplaceWait               )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectLaplaceWait               ); 
      m_pEffectLaplaceWait               = NULL; 
    }
    if( m_pEffectLaplaceFlow               )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectLaplaceFlow               ); 
      m_pEffectLaplaceFlow               = NULL; 
    }
    if( m_pEffectMuurandoExclamationNormal )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectMuurandoExclamationNormal ); 
      m_pEffectMuurandoExclamationNormal = NULL; 
    }
    if( m_pEffectMuurandoExclamationEx     )
    {
      if( pEffectManager ) pFieldmap->GetEffectManager()->DeleteEffect( m_pEffectMuurandoExclamationEx     ); 
      m_pEffectMuurandoExclamationEx     = NULL; 
    }

    for( u32 i = 0; i < ROBA2_ACCEL_EFFECT_NUM; i++ )
    {
      if( m_pEffectRoba2AccelSmoke[i]     )
      { 
        if(pEffectManager) pEffectManager->DeleteEffect( m_pEffectRoba2AccelSmoke[i] ); 
        m_pEffectRoba2AccelSmoke[i]    = NULL; 
      } 
    }

    if( pEffectManager == NULL  )
    {
      return;
    }
  }

  // 地面馬壁のアクターを有効化
  {
    GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive( true );
  }

  // ライド進入禁止用アクターを無効化
  GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive( false );
}

//-----------------------------------------------------------------------------
/**
*  @brief  ジャンプした先で地上系ライドできるかどうか(間の障害物を考慮する)
*
*  @param  check_pos          チェック座標
*  @param  pOutAdjustPos      ライド状態時に降りるであろう座標
*  @param  isCheckNaminori    チェックする壁に押し返しがある波乗り    フィルターも含める
*  @param  isCheckRoba2       チェックする壁に押し返しがあるロバ2     フィルターも含める
*  @param  isCheckRide        チェックする壁に押し返しがあるライド禁止フィルターも含める
*  @param  isUseRideCollision ライドのコリジョン幅を使って判定をするかどうか
*  @return 地上系ライド可否
*  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsEnableGroundRideBetween(const gfl2::math::Vector3& check_pos,gfl2::math::Vector3* pOutAdjustPos,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide,b32 isUseRideCollision)
{
  // 形状を確保
  SphereShape*   sphereShapeTerrain   = NULL;
  CylinderShape* cylinderShapeStatic  = NULL;
  CylinderShape* cylinderShapeTraffic = NULL;  
  SphereShape*   sphereShapeNaminori  = NULL;
  SphereShape*   sphereShapeRoba2     = NULL;
  SphereShape*   sphereShapeRide      = NULL;
  if( GetDynamicActor( COLLISION_TYPE_TERRAIN             )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeTerrain   = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_TERRAIN             )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL       )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ cylinderShapeTraffic = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_TRAFFIC_MODEL       )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_STATIC_MODEL        )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ cylinderShapeStatic  = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_STATIC_MODEL        )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeNaminori  = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeRoba2     = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeRide      = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShape()); }
  if( !sphereShapeTerrain || !cylinderShapeTraffic || !cylinderShapeStatic || !sphereShapeNaminori || !sphereShapeRoba2 || !sphereShapeRide )
  {
    GFL_ASSERT(0);
    return false;
  }

  // 基のスケール値を覚えておく
  f32 prevSphereTerrainR   = sphereShapeTerrain->m_r;
  f32 prevCylinderStaticR  = cylinderShapeStatic->m_r;
  f32 prevCylinderTrafficR = cylinderShapeTraffic->m_r;
  f32 prevSphereNaminoriR  = sphereShapeNaminori->m_r;
  f32 prevSphereRoba2R     = sphereShapeRoba2->m_r;
  f32 prevSphereRideR      = sphereShapeRide->m_r;

  // ライド進入禁止用アクターはライドしていない場合はfalseが入っている
  b32 isPrevActive = GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->IsActive();

  if( isUseRideCollision )
  {
    GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(true);

    // キャラの最大サイズに変更する
    sphereShapeTerrain->m_r   = MAX_COLLISION_R;
    cylinderShapeStatic->m_r  = MAX_COLLISION_R;
    cylinderShapeTraffic->m_r = MAX_COLLISION_R;
    sphereShapeNaminori->m_r  = MAX_COLLISION_R;
    sphereShapeRoba2->m_r     = MAX_COLLISION_R;
    sphereShapeRide->m_r      = MAX_COLLISION_R;
  }
  else
  {
    GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(false);

    // キャラの最大サイズに変更する
    Field::MoveModel::ApplicationDataForPlayer* pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetApplicationData( GetCharacterId() );
    sphereShapeTerrain->m_r   = pPlayerData->collisionR;
    cylinderShapeStatic->m_r  = pPlayerData->collisionR;
    cylinderShapeTraffic->m_r = pPlayerData->collisionR;
    sphereShapeNaminori->m_r  = pPlayerData->collisionR;
    sphereShapeRoba2->m_r     = pPlayerData->collisionR;
    sphereShapeRide->m_r      = pPlayerData->collisionR;
  }

  // 押し返しがあるシーンから自身のコリジョンが何箇所に触れているか調べる
  gfl2::math::Vector3 adjustPos(0.0f,0.0f,0.0f);
  u32 hitCnt = GetHitCntFromOshikaeshiScene(check_pos,&adjustPos);

  b32 isResult      = false;
  b32 isNeedRayCast = false;

  // "押し返し後の座標"と"押し返し前の座標"に差が無い状態なら間の障害物を考慮しない
  gfl2::math::Vector3 def( adjustPos - GetCharaDrawInstance()->GetPosition() );
  if( def.Length() <= FLT_EPSILON )
  {
    // どこにも当たっていなければ表世界にいることが担保できるので地上系ライドができる。
    // また一箇所にしかあたっていない場合でも表世界にいることが担保できるので地上系ライドができる。
    if( hitCnt == 0 || hitCnt == 1 )
    {
      isResult = true;
    }
    else
    {
      GFL_ASSERT(0);
    }
  }
  // "押し返し後の座標"と"押し返し前の座標"に差が有る状態なら間の障害物を考慮する
  else
  {
    // 表世界にいるか裏世界にいるか分からないが衝突が解決できているかどうか
    gfl2::math::Vector3 temp0(adjustPos);
    gfl2::math::Vector3 temp1(0.0f,0.0f,0.0f);
    if( GetHitCntFromOshikaeshiScene(temp0,&temp1) == 0 )
    {
      isNeedRayCast = true;
    }
  }

  // 形状を基に戻す
  sphereShapeTerrain->m_r   = prevSphereTerrainR;
  cylinderShapeStatic->m_r  = prevCylinderStaticR;
  cylinderShapeTraffic->m_r = prevCylinderTrafficR;
  sphereShapeNaminori->m_r  = prevSphereNaminoriR;
  sphereShapeRoba2->m_r     = prevSphereRoba2R;
  sphereShapeRide->m_r      = prevSphereRideR;

  // 衝突がキチンと解決されている状態で、衝突前と衝突後の間に壁が一つもなければライドができる
  if( isNeedRayCast )
  {
    if( GetHitCntFromOshikaeshiScene(GetCharaDrawInstance()->GetPosition(),adjustPos,0,isCheckNaminori,isCheckRoba2,isCheckRide) )
    {
      isResult = true;
    }
  }

  // 動作状況を基に戻す
  GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(isPrevActive);

  // ライド状態時に降りるであろう座標を確保
  if( isResult && pOutAdjustPos )
  {
    *pOutAdjustPos = adjustPos;
  }

  return isResult;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ジャンプした先で地上系ライドできるかどうか(間の障害物を考慮しない)
*
*  @param  check_pos          チェック座標
*  @param  p_pushout_pos      壁に押し出されたのであれば押し出された座標
*  @param  isUseRideCollision ライドのコリジョン幅を使って判定をするかどうか
*  @return 地上系ライド可否
*  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsEnableGroundRide(const gfl2::math::Vector3& check_pos, gfl2::math::Vector3 * p_pushout_pos,b32 isUseRideCollision)
{
  // 形状を確保
  SphereShape*   sphereShapeTerrain   = NULL;
  CylinderShape* cylinderShapeStatic  = NULL;
  CylinderShape* cylinderShapeTraffic = NULL;  
  SphereShape*   sphereShapeNaminori  = NULL;
  SphereShape*   sphereShapeRoba2     = NULL;
  SphereShape*   sphereShapeRide      = NULL;
  if( GetDynamicActor( COLLISION_TYPE_TERRAIN             )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeTerrain   = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_TERRAIN             )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_TRAFFIC_MODEL       )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ cylinderShapeTraffic = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_TRAFFIC_MODEL       )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_STATIC_MODEL        )->GetShapeType() == BaseActor::SHAPE_CYLINDER ){ cylinderShapeStatic  = static_cast<CylinderShape*>(GetDynamicActor(COLLISION_TYPE_STATIC_MODEL        )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeNaminori  = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_NAMINORI )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeRoba2     = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_ROBA2    )->GetShape()); }
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShapeRide      = static_cast<SphereShape*  >(GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE     )->GetShape()); }
  if( !sphereShapeTerrain || !cylinderShapeTraffic || !cylinderShapeStatic || !sphereShapeNaminori || !sphereShapeRoba2 || !sphereShapeRide )
  {
    GFL_ASSERT(0);
    return false;
  }

  // 基のスケール値を覚えておく
  f32 prevSphereTerrainR   = sphereShapeTerrain->m_r;
  f32 prevCylinderStaticR  = cylinderShapeStatic->m_r;
  f32 prevCylinderTrafficR = cylinderShapeTraffic->m_r;
  f32 prevSphereNaminoriR  = sphereShapeNaminori->m_r;
  f32 prevSphereRoba2R     = sphereShapeRoba2->m_r;
  f32 prevSphereRideR      = sphereShapeRide->m_r;

  // ライド進入禁止用アクターはライドしていない場合はfalseが入っている
  b32 isPrevActive = GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->IsActive();

  if( isUseRideCollision )
  {
    GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(true);

    // キャラの最大サイズに変更する
    sphereShapeTerrain->m_r   = MAX_COLLISION_R;
    cylinderShapeStatic->m_r  = MAX_COLLISION_R;
    cylinderShapeTraffic->m_r = MAX_COLLISION_R;
    sphereShapeNaminori->m_r  = MAX_COLLISION_R;
    sphereShapeRoba2->m_r     = MAX_COLLISION_R;
    sphereShapeRide->m_r      = MAX_COLLISION_R;
  }
  else
  {
    GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(false);

    // キャラの最大サイズに変更する
    Field::MoveModel::ApplicationDataForPlayer* pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetApplicationData( GetCharacterId() );
    sphereShapeTerrain->m_r   = pPlayerData->collisionR;
    cylinderShapeStatic->m_r  = pPlayerData->collisionR;
    cylinderShapeTraffic->m_r = pPlayerData->collisionR;
    sphereShapeNaminori->m_r  = pPlayerData->collisionR;
    sphereShapeRoba2->m_r     = pPlayerData->collisionR;
    sphereShapeRide->m_r      = pPlayerData->collisionR;
  }

  // 押し返しがあるシーンから自身のコリジョンが何箇所に触れているか調べる
  gfl2::math::Vector3 adjustPos(0.0f,0.0f,0.0f);
  u32 hitCnt = GetHitCntFromOshikaeshiScene(check_pos,&adjustPos);

  if(p_pushout_pos) *p_pushout_pos = adjustPos; // @fix 押し出し先座標 壁に接触している状態で水上ライドを行った際、ライド後に壁から離れた位置へ瞬間的に移動する

  // 形状を基に戻す
  sphereShapeTerrain->m_r   = prevSphereTerrainR;
  cylinderShapeStatic->m_r  = prevCylinderStaticR;
  cylinderShapeTraffic->m_r = prevCylinderTrafficR;
  sphereShapeNaminori->m_r  = prevSphereNaminoriR;
  sphereShapeRoba2->m_r     = prevSphereRoba2R;
  sphereShapeRide->m_r      = prevSphereRideR;

  // 動作状況を基に戻す
  GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(isPrevActive);

  // どこにも当たっていなければ表世界にいることが担保できるので地上系ライドができる。
  // また一箇所にしかあたっていない場合でも表世界にいることが担保できるので地上系ライドができる。
  if( hitCnt == 0 || hitCnt == 1 )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ジャンプしたい座標にキチンとジャンプできるかどうか
*
*  @param  check_pos       チェック座標
*  @param  isCheckNaminori チェックする壁に押し返しがある波乗り    フィルターも含める
*  @param  isCheckRoba2    チェックする壁に押し返しがあるロバ2     フィルターも含める
*  @param  isCheckRide     チェックする壁に押し返しがあるライド禁止フィルターも含める
*  @return 解決可否
*  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsEnableStepJump(const gfl2::math::Vector3& check_pos,b32 isCheckNaminori,b32 isCheckRoba2,b32 isCheckRide)
{
  // ライド進入禁止用アクターはライドしていない場合はfalseが入っている
  b32 isPrevActive = GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->IsActive();
  GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(true);

  // 押し返しがあるシーンから自身のコリジョンが何箇所触れているか調べる
  b32 isEnableJump = CheckEnableJump(check_pos,2,isCheckNaminori,isCheckRoba2,isCheckRide);

  // 動作状況を基に戻す
  GetDynamicActor(COLLISION_TYPE_OSHIKAESHI_RIDE )->SetActive(isPrevActive);

  // どこにも当たっていなければ段差ジャンプができる
  return isEnableJump;
}

//-----------------------------------------------------------------------------
/**
*  @brief  地上ライドした際に地面馬壁に当たっているかどうか
*
*  @return あたっているかどうか
*  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsHitRoba2WallGroundRide(void)
{
  // @note 地面馬時は無効、などの処理はしていない

  // 形状を確保
  SphereShape* sphereShape = NULL;
  if( GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2)->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShape          = static_cast<SphereShape*  >(GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2         )->GetShape()); }
  if( !sphereShape )
  {
    GFL_ASSERT(0);
    return false;
  }

  // 基のスケール値を覚えておく
  f32 prevSphereR = sphereShape->m_r;

  // Roba2進入禁止用アクターは地面馬ライドしている場合はfalseが入っている
  b32 isPrevActive = GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2)->IsActive();
  GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive(true);

  // キャラの最大サイズに変更する
  sphereShape->m_r = MAX_COLLISION_R;
  
  // 座標を更新
  GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetPosition( GetLocalSRT().translate );

  // リセット
  GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->InitCollisionResult();

  // 衝突チェック
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident()->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_ROBA2 )->Update();

  // 形状を基に戻す
  sphereShape->m_r = prevSphereR;

  // 動作状況を基に戻す
  GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive(isPrevActive);

  // 結果を返す
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->GetCollisionResult(0)->pTargetActor )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  非ライド状態になった際に地面馬壁に当たっているかどうか
*
*  @return あたっているかどうか
*  @note   中でコリジョンシーンのアップデートを回しているので高負荷です、利用は最低限にしてください
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsHitRoba2WallRideOff(void)
{
  // 形状を確保
  SphereShape* sphereShape = NULL;
  if( GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2)->GetShapeType() == BaseActor::SHAPE_SPHERE   ){ sphereShape          = static_cast<SphereShape*  >(GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2         )->GetShape()); }
  if( !sphereShape )
  {
    GFL_ASSERT(0);
    return false;
  }

  // 基のスケール値を覚えておく
  f32 prevSphereR = sphereShape->m_r;

  // Roba2進入禁止用アクターは地面馬ライドしている場合はfalseが入っている
  b32 isPrevActive = GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2)->IsActive();
  GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive(true);

  // キャラのサイズに変更する
  Field::MoveModel::ApplicationDataForPlayer* pPlayerData = (Field::MoveModel::ApplicationDataForPlayer*)GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetFieldCharaModelManager()->GetApplicationData( GetCharacterId() );
  sphereShape->m_r = pPlayerData->collisionR;
  
  // 座標を更新
  GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetPosition( GetLocalSRT().translate );

  // リセット
  GetDynamicActor( Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->InitCollisionResult();

  // 衝突チェック
  GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetFieldResident()->GetCollisionScene( FieldResident::COLLISION_SCENE_OSHIKAESHI_ROBA2 )->Update();

  // 形状を基に戻す
  sphereShape->m_r = prevSphereR;

  // 動作状況を基に戻す
  GetDynamicActor(Field::MoveModel::COLLISION_TYPE_OSHIKAESHI_ROBA2 )->SetActive(isPrevActive);

  // 結果を返す
  if( GetDynamicActor( COLLISION_TYPE_OSHIKAESHI_ROBA2 )->GetCollisionResult(0)->pTargetActor )
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  最後に有効だった入力方向を外部(イベント)からロックする
*
*  @return trueなら成功、falseなら失敗
*  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::LockDirFromOutside(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->LockDir( GetMoveCodeWork(),true );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  最後に有効だった入力方向を外部(イベント)からロックしているかどうか
*
*  @note   解除方法はスティックの入力がニュートラルになる or 別角度に入力がされる
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsLockDirFromOutside(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsLockDirFromOutside( GetMoveCodeWork() );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動いている状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsMoveState(void) const
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsMoveState( GetMoveCodeWork() );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  有効なスティック入力がされているかどうか
*
*  @return trueなら有効な入力あり、falseならなし
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsInputStick(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsInputStick();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ダッシュボタン入力がされているかどうか
*
*  @return trueなら有効な入力あり、falseならなし
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsInputDashButton(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsInputDashButton();
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  歩きループor歩きループ(坂道)状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsWalkLoopState(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsWalkLoopState( GetMoveCodeWork() );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  走りループor走りループ(坂道)状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsRunLoopState(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsRunLoopState( GetMoveCodeWork() );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  歩き終了状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsWalkEndState(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsWalkEndState( GetMoveCodeWork() );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  走り終了状態かどうか
*
*  @return trueなら可能、falseなら不可
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsRunEndState(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsRunEndState( GetMoveCodeWork() );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ムーランドのサーチ状態かどうか
*
*  @return trueならその状態、falseならその状態ではない
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsMuurandoSearchState(void) const
{
  // ムーランド動作でない
  if( GetMoveCodeId() != FIELD_MOVE_CODE_RIDE_MUURAND )
  {
    return false;
  }

  const FieldMoveCodeRideMuurando* pMoveCode = static_cast<const FieldMoveCodeRideMuurando*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsMuurandSearchState( GetMoveCodeWork() );
  }
  return false;
}
b32 FieldMoveModelPlayer::IsMuurandoSearchStateEx(void) const
{
  // ムーランド動作でない
  if( GetMoveCodeId() != FIELD_MOVE_CODE_RIDE_MUURAND )
  {
    return false;
  }

  const FieldMoveCodeRideMuurando* pMoveCode = static_cast<const FieldMoveCodeRideMuurando*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsMuurandSearchStateEx( GetMoveCodeWork() );
  }
  return false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コードが他からの入力を受け付ける状態かどうか
*
*  @return trueなら受け付ける、falseなら受け付けない
*/
//-----------------------------------------------------------------------------
b32 FieldMoveModelPlayer::IsEnbaleInputMoveCode(void)
{
  const FieldMoveCodePlayer* pMoveCode = static_cast<const FieldMoveCodePlayer*>( Field::MoveModel::FieldMoveCodeManager::GetMoveCode( GetMoveCodeId() ) );
  if( pMoveCode )
  {
    return pMoveCode->IsEnableInput( GetMoveCodeWork() );
  }
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  アクションコマンドを開始する際、別のアクションコマンドが不正に打ち切られていないか調べる
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::CheckActionCommand(void)
{
  FieldMoveModel::CheckActionCommand();
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コードを変更
*
*  @param  変更したい動作コード
*/
//-----------------------------------------------------------------------------
void FieldMoveModelPlayer::ChangeMoveCode(FIELD_MOVE_CODE_ID moveCodeId)
{
  FieldMoveModel::ChangeMoveCode(moveCodeId);

  // モーションによる移動禁止を解除
  // note:壁ドン処理により下記フラグが立ったままのケースがある
  OffStateBit( FIELD_MOVE_MODEL_STATEBIT_MOTION_MOVE_DISABLE );
}

}; //end of namespace MoveModel
}; //end of namespace Field
