//=================================================================================================
/**
*  @file   FieldMoveCodeNpc.cpp
*  @brief  フィールドNPC用 動作コード
*  @author Junya Ikuta
*  @date   2015.03.23
*/
//=================================================================================================

// Field
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActorBase.h"

// Collision
#include "Field/FieldRo/include/Collision/BaseShape.h"

// MoveModel
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"

// MoveCode
#include "Field/FieldStatic/include/MoveCode/FieldMoveCode_define.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeManager.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodeNpc.h"

#include <niji_reference_files/script/FieldPawnTypes.h> // for SODATEYA_ID_I02

// Convヘッダー
#include <niji_conv_header/field/chara/p2_base_fi.h>


#if defined(GF_PLATFORM_WIN32)
#include <float.h> // for isnan
#endif

// fureai
#include "Field/FieldRo/include/PlacementData/actor/FieldMoveModelActor.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiTool.h"
#include "Field/FieldStatic/include/FieldResident/FieldResident.h"
#include "Field/FieldRo/include/PokemonFureai/FieldPokemonFureaiDataAccessor.h"
#include "Sound/include/Sound.h"
#include "Field/FieldStatic/include/MoveCode/FieldMoveCodePlayer.h"
#include "Field/FieldStatic/include/MoveModel/FieldFacialAnime.h"
#include <math/include/gfl2_Easing.h>
#include "Field/FieldRo/include/Effect/content/FieldEffectFureaiIconBase.h"

namespace Field{ namespace MoveModel {

static const f32 COL_GETA(1.5f);
//
// 便利関数(時間が無かったので、スクリプトコマンド層からそのまま持ってきている)
//
namespace NpcTool {
/*
 *  Y軸回転の成分のみを抽出
 *  @note XZ成分が0のベクトルを渡すとNaNが返ります
 */
f32 ConvRotationVector3ToYRadian( gfl2::math::Vector3& src )
{
  gfl2::math::Vector3 UnitX    (1.0f,0.0f,0.0f);
  gfl2::math::Vector3 UnitZ    (0.0f,0.0f,1.0f);
  gfl2::math::Vector3 vec=src;

  vec.y = 0.0f;
  vec   = vec.Normalize();
  f32 rad  = gfl2::math::AcosRad( UnitZ.Dot( vec ) );
  if( UnitX.Dot( vec ) < 0.0f ){ rad *= -1.0f; }

  return rad;
}

/*
 *  Y軸回転の成分のみを抽出
 */
f32 ConvRotationQuaternionToYRadian( gfl2::math::Quaternion& src )
{
  gfl2::math::Matrix34  Mtx;
  gfl2::math::Vector3 UnitZ    (0.0f,0.0f,1.0f);

  src.QuaternionToMatrix( Mtx );

  UnitZ = Mtx * UnitZ;

  return ConvRotationVector3ToYRadian(UnitZ);
}

/// * @brief      角度の差を得る.
f32 DiffAngle( f32 _fRadA, f32 _fRadB )
{
#if defined(GF_PLATFORM_WIN32)
  // NaNが渡されると下のWhileで無限ループになる可能性があるため、ケア
  if( _isnan( _fRadA ) || _isnan( _fRadB ) )
  {
    return 0.0f;
  }
#endif

#if defined(GF_PLATFORM_CTR)
  // NaNが渡されると下のWhileで無限ループになる可能性があるため、ケア
  if( isnan( _fRadA ) || isnan( _fRadB ) )
  {
    return 0.0f;
  }
#endif

  f32   fRadA = _fRadA;
  f32   fRadB = _fRadB;
  f32   fSa;

  u32 count = 0;

  while( 0.f > fRadA )  fRadA += PI2;
  while( PI2 < fRadA )  fRadA -= PI2;
  while( 0.f > fRadB )  fRadB += PI2;
  while( PI2 < fRadB )  fRadB -= PI2;
  
  if( fRadA < fRadB )
  {
    fSa = fRadB - fRadA;
    if( PI < fSa )  fRadB -= PI2;
  }else{
    fSa = fRadA - fRadB;
    if( PI < fSa )  fRadA -= PI2;
  }
  
  fSa = fRadA - fRadB;
  return fSa;
}


bool ColSegments(
   const gfl2::math::Vector2& startA,
   const gfl2::math::Vector2& endA,
   const gfl2::math::Vector2& startB,
   const gfl2::math::Vector2& endB,
   //float* outT1 = 0,       // 線分1の内分比（出力）
   //float* outT2 = 0,       // 線分2の内分比（出力
   gfl2::math::Vector2* pOut
) {

   gfl2::math::Vector2 v = startB - startA;
   gfl2::math::Vector2 vecA = endA - startA;
   gfl2::math::Vector2 vecB = endB - startB;

   float Crs_v1_v2 = vecA.Cross( vecB );
   if ( Crs_v1_v2 == 0.0f ) {
      // 平行状態
      return false;
   }

   float Crs_v_v1 = v.Cross( vecA );
   float Crs_v_v2 = v.Cross( vecB );

   float t1 = Crs_v_v2 / Crs_v1_v2;
   float t2 = Crs_v_v1 / Crs_v1_v2;

#if 0
   if ( outT1 )
      *outT1 = Crs_v_v2 / Crs_v1_v2;
   if ( outT2 )
      *outT2 = Crs_v_v1 / Crs_v1_v2;
#endif

   const float eps = 0.00001f;
   if ( t1 + eps < 0 || t1 - eps > 1 || t2 + eps < 0 || t2 - eps > 1 ) {
      // 交差していない
      return false;
   }

   *pOut = startA + vecA * t1;
   return true;
}



static const u32 COUNT_MAX(9999);
static const f32 RETURN_ROT_DIFF_DEG( 0.01f );  // この差分がある場合は初期向きに戻す
static const f32 EFFECTIVE_DISTANCE( 0.1f );    // XZ平面でこれ以上離れていたら有効

}; //end of namespace NpcToo

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
#if PM_DEBUG
  // デバッグ設定を反映
  ApplyDebugSetting( pMoveCodeWork );
#endif 

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  
  b32 continueFlag = false;
  do 
  {
    switch( pWork->state )
    {
    case State::READY:
      {
        continueFlag = SubReadyProcess( pMoveCodeWork );
        break;
      }

    case State::IDLE:
      {
        continueFlag = SubIdleProcess( pMoveCodeWork );
        break;
      }

    case State::ROTATION:
      {
        continueFlag = SubRotationProcess( pMoveCodeWork );
        break;
      }
    }
  }
  while( continueFlag );


#if 0
  // 待機状態でしかとれないようにする
  u32 state = FieldMoveCodeNpc::NPC_MOVE_WAIT;

  // モーションの変更
  if ( pWork->oldState != state )
  {
    // ステートにあったデフォルトモーションを取得
    s32 next_anime_id = m_NpcMoveSettingTable[state].motionId;

    // 待機アニメーションだけは外部から上書き設定可能
    if( state == FieldMoveCodeNpc::NPC_MOVE_WAIT )
    {
      next_anime_id = (u32)( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() );
    }
    pCharaModelInstance->ChangeAnimation( next_anime_id);
  }

  //
  pWork->oldState = state;
#endif
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  ポスト
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // 現在のモーションを記憶
  pWork->prevMotionId = pCharaModelInstance->GetAnimationId();
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  中断
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{  
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();


  switch( pWork->state )
  {
  case State::IDLE:
    {
      break;
    }

  case State::ROTATION:
    {
      // 強制打ち切り
      pWork->state = State::IDLE;

      // モーションを待機に変更
      pCharaModelInstance->ChangeAnimation( GetRotationAfterMotionId( pMoveCodeWork ) );

      break;
    }
  }
  

  
  // 各種カウンターをリセット
  pWork->dirResetWaitCount = 0;
  pWork->isDirResetWait = false;
  pWork->rotEnterWaitCount = 0;
  pWork->frontInterestCount = 0;

  // pWork->rotationCount = 0;    回転した事は記憶しておく

  // @fix Mcat[2042]:待機モーションが特殊なNPCに話しかけ会話済み状態になった後にフィールド破棄/生成を行うと、話しかけた方向のまま特殊モーションが再生される、追加対処。中断によるモーション変更を覚えるように変更。
  pWork->prevMotionId = pCharaModelInstance->GetAnimationId();
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  復帰
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  
  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // @fix Mcat[2042]:待機モーションが特殊なNPCに話しかけ会話済み状態になった後にフィールド破棄/生成を行うと、話しかけた方向のまま特殊モーションが再生される、対処。話しかけ済み、向きが戻っていない場合はフィールド復帰時に直前のモーションを復元する
  if( ( pWork->isTalkEnd || pWork->dirDirtyFlag ) && pWork->prevMotionId != -1 )
  {
    // @fix Mcat[2042]によりセーブ継続不可をケア、追加されたワークが不定値になっていた。最低限動作するように有効範囲チェックを追加
    if( pCharaModelInstance->IsAnimationExist( pWork->prevMotionId ) )
    {
      pCharaModelInstance->ChangeAnimationSmooth( pWork->prevMotionId, 0 );
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  会話開始処理  話しかけによるスクリプト起動の直前に実行される
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::TalkStart(FieldMoveCodeWork* pMoveCodeWork)
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->isTalkEnd = true;

  // その他のリセットはSuspendで行っている
  
  // 現在の向きを保存
  if( pWork->dirDirtyFlag == 0 )
  {
    SaveNowDirForReturn( pMoveCodeWork );
    pWork->dirDirtyFlag = 1;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目終了処理  イベントチェックでイベント起動時に実行される
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::StopInterest(FieldMoveCodeWork* pMoveCodeWork)
{
  // 注目停止処理
  if( pMoveCodeWork->pModel->isEnableInterest() )
  {
    DisableInterest( pMoveCodeWork );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  指定のオプションを有効化する
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  optionBit      指定するオプション
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::OnOption(FieldMoveCodeWork* pMoveCodeWork, u32 optionBit )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option |= optionBit;
}

//-----------------------------------------------------------------------------
/**
*  @brief  指定のオプションを無効化する
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  optionBit      指定するオプション
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::OffOption(FieldMoveCodeWork* pMoveCodeWork, u32 optionBit )
{
  // コリジョンがシリンダー以外の場合は回転禁止をOFFにできない
  StaticActor* pStaticActor = pMoveCodeWork->pModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL );
  if( pStaticActor == NULL || pStaticActor->GetShapeType() != BaseActor::SHAPE_CYLINDER )
  {
    if( optionBit & Option::INVALID_RETURN )
    {
      GFL_ASSERT_MSG( "rect moveModel is RETURN NG \n", 0 );
      optionBit &= (~Option::INVALID_RETURN);
    }
    
    if( optionBit & Option::INVALID_ROTATION )
    {
      GFL_ASSERT_MSG( "rect moveModel is ROTATION NG \n", 0 );
      optionBit &= (~Option::INVALID_ROTATION);
    }
  }

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option &= (~optionBit);
}

//-----------------------------------------------------------------------------
/**
*  @brief  オプションを初期状態に戻します
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  optionBit      指定するオプション
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::ResetOption(FieldMoveCodeWork* pMoveCodeWork )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->option = pWork->defaultOption;
}

//-----------------------------------------------------------------------------
/**
*  @brief  話しかけ済み状態を解除します
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::ClearTalkEnd(FieldMoveCodeWork* pMoveCodeWork )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->isTalkEnd = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  ダーティフラグ状態をクリアします
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::ClearDirtyFlag(FieldMoveCodeWork* pMoveCodeWork )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->dirDirtyFlag = false;
}

//-----------------------------------------------------------------------------
/**
*  @brief  戻すべき向きを上書きします
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  deg            戻すべき向き(度)
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::OverWriteReturnAngle(FieldMoveCodeWork* pMoveCodeWork, f32 deg )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->dirDirtyFlag = true;
  pWork->returnRotationDegY = deg;
}

//-----------------------------------------------------------------------------
/**
*  @brief  NPCアクションが起きていないか取得する
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::IsNoAction(FieldMoveCodeWork* pMoveCodeWork )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // 回転中
  if( pWork->state != State::IDLE )
  {
    return false;
  }

  // 注視が動いているならアクション中
  if( pMoveCodeWork->pModel->isEnableInterest() )
  {
    return false;
  }

  return true;
}


// private

//----------------------------------------------------------------------------
/**
*  @brief  初回更新処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::SubReadyProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->state = State::IDLE;

  // @fix GFNMcat[2359]:メインイベントでキャラが明後日の方向にあるく、対処。会話発動時、回転発動時に向き変更フラグの有効化と方向を保存し、その情報を使用して向きを戻すように変更。ここでは何もしない。
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  待機中処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::SubIdleProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  b32 continueFlag = false;      // true もう一度更新
  b32 noActionFlag = true;       // true 1更新で1度しか起きないようなアクションがあった
  b32 dirResetReqFlag = false;   // true 向きリセットのリクエスト  


  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // 会話済み(機能停止中)
  if( pWork->isTalkEnd )
  {
    // 自機取得のため、グローバルアクセス
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap* pFieldMap = pGameManager->GetFieldmap();

    // 関連座標を取得
    gfl2::math::Vector3 playerPos( pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate );
    gfl2::math::Vector3 npcPos = pMoveCodeWork->pModel->GetLocalSRT().translate;
    gfl2::math::Vector3 npcToPlayerVec = (playerPos - npcPos);
    
    // 厚みを取得
    f32 playerBodyWidth = GetBodyWidth( pFieldMap->GetPlayerCharacter(), true );
    f32 npcBodyWidth = GetBodyWidth( pMoveCodeWork->pModel, false );

    // 自機との距離を取得
    f32 distanceSqForPlayer( npcToPlayerVec.LengthSq() );

    // ある程度遠くに離れたら復帰
    if( HitCheckTalkResetRange( distanceSqForPlayer, playerBodyWidth, npcBodyWidth, IsEnableLookShort( pMoveCodeWork ) ))
    {
      pWork->isTalkEnd = false;
      continueFlag = true;
    }
  }
  // カリング中
  else if( pMoveCodeWork->pModel->IsEnableCulilng() )
  {
    // 自機取得のため、グローバルアクセス
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap* pFieldMap = pGameManager->GetFieldmap();

    // 関連座標を取得
    gfl2::math::Vector3 playerPos( pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate );
    gfl2::math::Vector3 npcPos = pMoveCodeWork->pModel->GetLocalSRT().translate;
    gfl2::math::Vector3 npcToPlayerVec = (playerPos - npcPos);
    
    // 厚みを取得
    f32 playerBodyWidth = GetBodyWidth( pFieldMap->GetPlayerCharacter(), true );
    f32 npcBodyWidth = GetBodyWidth( pMoveCodeWork->pModel, false );

    // 自機との距離を取得
    f32 distanceSqForPlayer( npcToPlayerVec.LengthSq() );

    // リターン制御
    if( HitCheckDirResetRange( distanceSqForPlayer, playerBodyWidth, npcBodyWidth, IsEnableLookShort( pMoveCodeWork ) ) )
    {
      dirResetReqFlag = true;
    }
  }
  else
  {
    // 自機取得のため、グローバルアクセス
    GameSys::GameManager* pGameManager  = GFL_SINGLETON_INSTANCE( GameSys::GameManager );
    Fieldmap* pFieldMap = pGameManager->GetFieldmap();

    // 関連座標を取得
    gfl2::math::Vector3 playerPos( pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate );
    gfl2::math::Vector3 npcPos = pMoveCodeWork->pModel->GetLocalSRT().translate;
    gfl2::math::Vector3 npcToPlayerVec = (playerPos - npcPos);

    // 自機との距離を取得
    f32 distanceSqForPlayer( npcToPlayerVec.LengthSq() );

    // @fix GFMcat[2273]:自機とNPCが重なっていた際に無限ループになる、。相対向きのロジックの結果がNANになるので、ありえないぐらい近い場合は処理を打ち切る。
    gfl2::math::Vector3 npcToPlayerVecXZ = npcToPlayerVec;
    npcToPlayerVecXZ.y = 0.0f;
    if( npcToPlayerVecXZ.LengthSq() <= ( NpcTool::EFFECTIVE_DISTANCE * NpcTool::EFFECTIVE_DISTANCE ))
    {
      return false;
    }

    // 自機との相対角度を取得
    f32 npcToPlayerRotationRad = NpcTool::ConvRotationVector3ToYRadian( npcToPlayerVec );
    gfl2::math::Quaternion npcRotation = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
    f32 npcNowRotationRad = NpcTool::ConvRotationQuaternionToYRadian( npcRotation );
    f32 rotationDiffDegForPlayer = gfl2::math::FAbs( gfl2::math::ConvRadToDeg( NpcTool::DiffAngle( npcToPlayerRotationRad, npcNowRotationRad ) ) );


    // 厚みを取得
    f32 playerBodyWidth = GetBodyWidth( pFieldMap->GetPlayerCharacter(), true );
    f32 npcBodyWidth = GetBodyWidth( pMoveCodeWork->pModel, false );

    // 注目制御
    b32 hitFront;
    if( HitCheckInterestRange( distanceSqForPlayer, rotationDiffDegForPlayer, playerBodyWidth, npcBodyWidth, IsEnableLookWide( pMoveCodeWork ), IsEnableLookShort( pMoveCodeWork ), &hitFront ) )
    {
      // 注目実行
      if( IsEnableInterest( pMoveCodeWork ) )
      {
        gfl2::math::Vector3 interestPos( gfl2::math::Vector3::GetZero() );
        GetInterestPosition( pFieldMap->GetPlayerCharacter(), &interestPos );

        // @fix NMcat[2384]:自機がNPCの真後ろに移動した際、NPCの自機に対する注目挙動が定まらない対処。背後に入った方向に注目座標を加工する処理を追加
        UpdateInterestPosition( pMoveCodeWork, interestPos );
      }
      else
      {
        if( pMoveCodeWork->pModel->isEnableInterest() )
        {
          DisableInterest( pMoveCodeWork );
        }
      }

      noActionFlag = false;

      // 前方HITの場合はカウンタを回す
      if( hitFront )
      {
        if( pWork->frontInterestCount < NpcTool::COUNT_MAX )
        {
          pWork->frontInterestCount++;
        }
      }
      else
      {
        pWork->frontInterestCount = 0;
      }
    }
    else
    {
      FieldMoveModel* pChildPokemon = PokemonFureai::GetChildPokemon( pMoveCodeWork->pModel );
      if (pChildPokemon)
      {
        // ペアとなるふれあいポケモンを注目する
        gfl2::math::Vector3 interestPos(gfl2::math::Vector3::GetZero());
        GetInterestPosition(pChildPokemon, &interestPos);
        UpdateInterestPosition(pMoveCodeWork, interestPos);
      }
      else
      {
        // 注目停止処理
        if (pMoveCodeWork->pModel->isEnableInterest())
        {
          DisableInterest(pMoveCodeWork);
        }
      }

      pWork->frontInterestCount = 0;
    }

    // 回転制御
    if( HitCheckRotationRange( distanceSqForPlayer, rotationDiffDegForPlayer, playerBodyWidth, npcBodyWidth ) )
    {
      // 回転実行
      if( IsEnableRotation( pMoveCodeWork ) && pWork->rotationCount == 0 )
      {
        // 自然にするため、回転をちょっと待つ。ただし前方の注目範囲にある程度入ってた場合は即実行する
        if( pWork->rotEnterWaitCount >= m_Setting.enterWaitForInterestToRotation || pWork->frontInterestCount > m_Setting.enterWaitForInterestToRotation )
        {
          if( rotationDiffDegForPlayer >= m_Setting.degreeDiffForRotationChange )
          {
            // 現在の向きを保存
            if( pWork->dirDirtyFlag == 0 )
            {
              SaveNowDirForReturn( pMoveCodeWork );
              pWork->dirDirtyFlag = 1;
            }

            StartRotation( pMoveCodeWork, gfl2::math::ConvRadToDeg( npcToPlayerRotationRad ), m_Setting.rotationFrame );
            continueFlag = true;
          }

          pWork->rotationCount += 1;
          noActionFlag = false;
        }
        else
        {
          pWork->rotEnterWaitCount++;
        }
      }
    }
    else
    {
      pWork->rotationCount = 0;
      pWork->rotEnterWaitCount = 0;
    }

    // リターン制御
    if( noActionFlag && HitCheckDirResetRange( distanceSqForPlayer, playerBodyWidth, npcBodyWidth, IsEnableLookShort( pMoveCodeWork ) ) )
    {
      dirResetReqFlag = true;
    }
  }

  // リセット処理
  // @fix GFNMcat[2359]:メインイベントでキャラが明後日の方向にあるく、対処。会話発動時、回転発動時に向き変更フラグの有効化と方向を保存し、その情報を使用して向きを戻すように変更。
  if( dirResetReqFlag && pWork->dirDirtyFlag == 1 )
  {
    // 自然に戻すため、ちょっと待つ
    if( !pWork->isDirResetWait )
    {
      pWork->isDirResetWait = true;
      pWork->dirResetWaitCount = m_Setting.dirResetWaitBase + System::GflUse::GetPublicRand( m_Setting.dirResetWaitRand );
    }
    else
    {
      if( pWork->dirResetWaitCount > 0 )
      {
        pWork->dirResetWaitCount--;
      }
    }

    // リセット実行
    if( pWork->dirResetWaitCount == 0 )
    {
      // 初期向きに戻す
      if( IsEnableReturn( pMoveCodeWork ) )
      {
        // 初期向きと一定の差分あれば戻りを実行する　@note 変換を重ねているので、ある程度の誤差を許容している
        gfl2::math::Quaternion npcRotation = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
        f32 npcNowRotationRad = NpcTool::ConvRotationQuaternionToYRadian( npcRotation );
        f32 startRotationRadY = gfl2::math::ConvDegToRad( pWork->returnRotationDegY );


        if( gfl2::math::FAbs( NpcTool::DiffAngle( startRotationRadY, npcNowRotationRad )) >= gfl2::math::ConvDegToRad( NpcTool::RETURN_ROT_DIFF_DEG ) )
        {
          StartRotation( pMoveCodeWork, pWork->returnRotationDegY, m_Setting.rotationFrame, true );
          continueFlag = true;
        }
        else
        {
          // @fix GFNMcat[3735]:ＮＰＣが話しかけ後にデフォルトモーションに戻らない対処、旋廻終了時しかデフォルトモーションに戻していなかった、リセットタイミングに回転が発動しない際も、デフォルトモーションに変更するように修正
          StartDefaultIdleMotion( pMoveCodeWork );

          // @fix NMcat[2863]:戻り回転中に中断が入ると、NPCが元の位置まで向き戻らなくなる、対処。回転命令発効時にダーティフラグをリセットしていたのを修正。回転命令が発効される場合は、回転後にリセットするように修正。
          // @note ほぼありえないが、極小のズレが溜まる可能性がある
          pWork->dirDirtyFlag = 0;
        }
      }
      else
      {
        // @fix GFNMcat[3735]:ＮＰＣが話しかけ後にデフォルトモーションに戻らない対処、旋廻終了時しかデフォルトモーションに戻していなかった、リセットタイミングに回転が発動しない際も、デフォルトモーションに変更するように修正
        StartDefaultIdleMotion( pMoveCodeWork );
          
        // @fix NMcat[2863]:戻り回転中に中断が入ると、NPCが元の位置まで向き戻らなくなる、対処。回転命令発効時にダーティフラグをリセットしていたのを修正。回転命令が発効される場合は、回転後にリセットするように修正。
        // @note ほぼありえないが、この後回転オプションが有効になった際に今の向きを基準向きにするため、ダーティフラグを落とす
        pWork->dirDirtyFlag = 0;
      }

      // 注目停止処理
      if( pMoveCodeWork->pModel->isEnableInterest() )
      {
        DisableInterest( pMoveCodeWork );
      }

      pWork->isDirResetWait = false;
      pWork->dirResetWaitCount = 0;

    }
  }
  else
  {
    pWork->isDirResetWait = false;
    pWork->dirResetWaitCount = 0;
  }


  return continueFlag;
}

//----------------------------------------------------------------------------
/**
*  @brief  回転中処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::SubRotationProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  b32 continueFlag = false;
  
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  
  // ターゲット姿勢にむけて補完する
  gfl2::math::Quaternion qua;
  gfl2::math::Quaternion targetQua;
  qua.RadianYXZToQuaternion( gfl2::math::ConvDegToRad( 0.0f ), gfl2::math::ConvDegToRad( pWork->rotationWork.startRotationY ), gfl2::math::ConvDegToRad( 0.0f ) );
  targetQua.RadianYXZToQuaternion( gfl2::math::ConvDegToRad( 0.0f ), gfl2::math::ConvDegToRad( pWork->rotationWork.targetRotationY ), gfl2::math::ConvDegToRad( 0.0f ) );
  

  qua.Slerp( targetQua, ((f32)pWork->rotationWork.frame+1.0f) / ((f32)pWork->rotationWork.frameMax+1.0f) );
  pCharaModelInstance->SetRotationQuat(qua);

  // 指定フレームたっていなければ継続してアクションコマンドを実行する
  pWork->rotationWork.frame++;
  if( pWork->rotationWork.frame <= (f32)pWork->rotationWork.frameMax )
  {
    return continueFlag;
  }

  // 念の為にターゲット姿勢にあわせる
  pCharaModelInstance->SetRotationQuat( targetQua );

  // 待機モーションに移行
  pWork->state = pWork->nextState;
  pWork->nextState = 0;
  if( pWork->rotationWork.isReturn )
  {
    pCharaModelInstance->ChangeAnimation( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() );

    // @fix NMcat[2863]:戻り回転中に中断が入ると、NPCが元の位置まで向き戻らなくなる、対処。回転命令発効時にダーティフラグをリセットしていたのを修正。回転命令が発効される場合は、回転後にリセットするように修正。
    pWork->dirDirtyFlag = 0;
  }
  else
  {
    pCharaModelInstance->ChangeAnimation( GetRotationAfterMotionId( pMoveCodeWork ) );
  }

  return continueFlag;
}


//----------------------------------------------------------------------------
/**
*  @brief  注視範囲にヒットしているかチェック　
*
*  @param  distanceSq       距離の二乗
*  @param  rotationDiffDeg  向きの差分(度)
*  @param  playerBodyWidth  自機の厚み
*  @param  npcBodyWidth     NPCの厚み
*  @param  isWide           範囲を拡大する
*  @param  isShort          範囲を縮小する
*  @param  pOutHitFront     前方範囲でのHITならtrue
*
*  @retval true ヒットしている
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::HitCheckInterestRange( f32 distanceSq, f32 rotationDiffDeg, f32 playerBodyWidth, f32 npcBodyWidth, b32 isWide, b32 isShort, b32* pOutHitFront ) const
{
  // キャラクターの厚みを考慮する
  f32 baseWidth( 40 + 37 );   // 現在の設定がNPC(40)、PC(37)で調整した値のため、その分を引く
  f32 widthEffect = playerBodyWidth + npcBodyWidth - baseWidth;

  *pOutHitFront = false;

  // 二乗するマクロ
#define PowerOf2( a ) (a)*(a)

  // 距離変更オプションを反映
  f32 distanceRangeForInterest = m_Setting.distanceRangeForInterest;
  if( isWide )
  {
    distanceRangeForInterest = m_Setting.distanceRangeForInterestWide;
  }
  else if( isShort )
  {
    distanceRangeForInterest = m_Setting.distanceRangeForInterestShort;
  }

  // 通常範囲
  if( distanceSq <= PowerOf2( distanceRangeForInterest + widthEffect ) && rotationDiffDeg <= m_Setting.degreeRangeForInterest )
  {
    *pOutHitFront = true;
    return true;
  }

  // 近めの範囲
  if( distanceSq <= PowerOf2( m_Setting.inSideDistanceRangeForInterest + widthEffect ) && rotationDiffDeg <= m_Setting.inSideDegreeRangeForInterest )
  {
    return true;
  }
   

  // 最近接
  if( distanceSq <= PowerOf2( m_Setting.nearestDistanceRangeForInterest + widthEffect ) && rotationDiffDeg <= m_Setting.nearestDegreeRangeForInterest )
  {
    return true;
  }


  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  回転範囲にヒットしているかチェック　
*
*  @param  distanceSq       距離の二乗
*  @param  rotationDiffDeg  向きの差分(度)
*  @param  playerBodyWidth  自機の厚み
*  @param  npcBodyWidth     NPCの厚み
*
*  @retval true ヒットしている
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::HitCheckRotationRange( f32 distanceSq, f32 rotationDiffDeg, f32 playerBodyWidth, f32 npcBodyWidth ) const
{
  // キャラクターの厚みを考慮する
  f32 baseWidth( 40 + 37 );   // 現在の設定がNPC(40)、PC(37)で調整した値のため、その分を引く
  f32 widthEffect = playerBodyWidth + npcBodyWidth - baseWidth;
  
  // 二乗するマクロ
#define PowerOf2( a ) (a)*(a)

  // 通常範囲
  if( distanceSq <= PowerOf2( m_Setting.distanceRangeForRotation + widthEffect ) && rotationDiffDeg <= m_Setting.degreeRangeForRotation )
  {
    return true;
  }
   
  // 近めの範囲
  if( distanceSq <= PowerOf2( m_Setting.inSideDistanceRangeForRotation + widthEffect ) && rotationDiffDeg <= m_Setting.inSideDegreeRangeForRotation )
  {
    return true;
  }
  
  // 最近接
  if( distanceSq <= PowerOf2( m_Setting.nearestDistanceRangeForRotation + widthEffect ) && rotationDiffDeg <= m_Setting.nearestDegreeRangeForRotation )
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  会話リセット範囲にヒットしているかチェック　
*
*  @param  distanceSq       距離の二乗
*  @param  playerBodyWidth  自機の厚み
*  @param  npcBodyWidth     NPCの厚み
*  @param  isShort          範囲を縮小する
*
*  @retval true ヒットしている
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::HitCheckTalkResetRange( f32 distanceSq, f32 playerBodyWidth, f32 npcBodyWidth, b32 isShort ) const
{
  // キャラクターの厚みを考慮する
  f32 baseWidth( 40 + 37 );   // 現在の設定がNPC(40)、PC(37)で調整した値のため、その分を引く
  f32 widthEffect = playerBodyWidth + npcBodyWidth - baseWidth;

  // 二乗するマクロ
#define PowerOf2( a ) (a)*(a)

  // 距離変更オプションを反映
  f32 talkEndResetDistanceRange = m_Setting.talkEndResetDistanceRange;
  if( isShort )
  {
    talkEndResetDistanceRange = m_Setting.talkEndResetDistanceRangeShort;
  }

  // 通常範囲
  if( distanceSq > PowerOf2( talkEndResetDistanceRange + widthEffect ) )
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  挙動リセット範囲にヒットしているかチェック　
*
*  @param  distanceSq       距離の二乗
*  @param  playerBodyWidth  自機の厚み
*  @param  npcBodyWidth     NPCの厚み
*  @param  isShort          範囲を縮小する
*
*  @retval true ヒットしている
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::HitCheckDirResetRange( f32 distanceSq, f32 playerBodyWidth, f32 npcBodyWidth, b32 isShort ) const
{
  // キャラクターの厚みを考慮する
  f32 baseWidth( 40 + 37 );   // 現在の設定がNPC(40)、PC(37)で調整した値のため、その分を引く
  f32 widthEffect = playerBodyWidth + npcBodyWidth - baseWidth;
  
  // 二乗するマクロ
#define PowerOf2( a ) (a)*(a)
  
    // 距離変更オプションを反映
  f32 dirResetDistanceRange = m_Setting.dirResetDistanceRange;
  if( isShort )
  {
    dirResetDistanceRange = m_Setting.dirResetDistanceRangeShort;
  }

  // 通常範囲
  if( distanceSq > PowerOf2( dirResetDistanceRange + widthEffect ) )
  {
    return true;
  }
  
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  回転開始処理　
*
*  @param  pMoveCodeWork        動作コードワーク
*  @param  targetRotationDegY   回転後の方向(度)
*  @param  frame                回転にかけるフレーム
*  @param  isReturn             true = 初期状態に戻る回転
*  @param  isCompressMotion     true = モーションの速度を旋回速度にあわせる
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::StartRotation(FieldMoveCodeWork* pMoveCodeWork, f32 targetRotationDegY, u32 frame, b32 isReturn, b32 isCompressMotion, s32 nextState ) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // キャラクターローカルに変換する逆行列を取得
  gfl2::math::Matrix34 startRot,startRotInv;
  pCharaModelInstance->GetRotationQuat().QuaternionToMatrix(startRot);
  startRotInv = startRot.Inverse();

  // 目標の向きベクトルを取得
  gfl2::math::Matrix34 targetRot;
  gfl2::math::Quaternion targetQua;
  targetQua.RadianYXZToQuaternion( gfl2::math::ConvDegToRad( 0.0f ), gfl2::math::ConvDegToRad( targetRotationDegY ), gfl2::math::ConvDegToRad( 0.0f ) );
  targetQua.QuaternionToMatrix(targetRot);
  gfl2::math::Vector4 targetVec(0.0f,0.0f,1.0f,0.0f);
  targetVec = targetRot.TransformNormal(targetVec);
  gfl2::math::Vector4 targetLocalVec = startRotInv.TransformNormal(targetVec);



  // 右か左にターンする
  if( targetLocalVec.x >= 0.0f )
  {
    pCharaModelInstance->ChangeAnimation( P2_BASE_FI017_TURN01_LEFT );
  }
  else
  {
    pCharaModelInstance->ChangeAnimation( P2_BASE_FI016_TURN01_RIGHT );
  }

  // 旋回モーションがないものがある
  if( pCharaModelInstance->GetAnimationEndFrame() <= 0 )
  {
    pCharaModelInstance->ChangeAnimation( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() );
  }
  else
  {
#if 1
    // 旋回フレームに併せて早送りをする
    if( isCompressMotion )
    {
      f32 stepFrame = pCharaModelInstance->GetAnimationEndFrame() / (f32)frame;
      pCharaModelInstance->SetAnimationStepFrame( stepFrame );
    }
#endif
  }

  // 設定
  pWork->state = State::ROTATION;
  pWork->nextState = nextState;
  pWork->rotationWork.frame = 0;
  gfl2::math::Quaternion startRotation = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  pWork->rotationWork.startRotationY = gfl2::math::ConvRadToDeg( NpcTool::ConvRotationQuaternionToYRadian( startRotation ) );
  pWork->rotationWork.targetRotationY = targetRotationDegY;
  pWork->rotationWork.isReturn = isReturn;
  pWork->rotationWork.frameMax = frame;
}

b32 FieldMoveCodeNpc::CheckRotationDir(FieldMoveCodeWork* pMoveCodeWork, f32 targetRotationDegY) const
{
	// 共有ワークを取得
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

	// キャラモデル描画用インスタンスを取得
	poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

	// キャラクターローカルに変換する逆行列を取得
	gfl2::math::Matrix34 startRot, startRotInv;
	pCharaModelInstance->GetRotationQuat().QuaternionToMatrix(startRot);
	startRotInv = startRot.Inverse();

	// 目標の向きベクトルを取得
	gfl2::math::Matrix34 targetRot;
	gfl2::math::Quaternion targetQua;
	targetQua.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(0.0f), gfl2::math::ConvDegToRad(targetRotationDegY), gfl2::math::ConvDegToRad(0.0f));
	targetQua.QuaternionToMatrix(targetRot);
	gfl2::math::Vector4 targetVec(0.0f, 0.0f, 1.0f, 0.0f);
	targetVec = targetRot.TransformNormal(targetVec);
	gfl2::math::Vector4 targetLocalVec = startRotInv.TransformNormal(targetVec);

	// 右か左にターンする
	if (targetLocalVec.x >= 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

//----------------------------------------------------------------------------
/**
*  @brief  デフォルト待機モーション開始( 既に待機モーションなら何も再生しない )
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval モーションID
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::StartDefaultIdleMotion(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // モーション切り替え
  if( pCharaModelInstance->GetAnimationId() != pMoveCodeWork->pModel->GetDefaultIdleAnimationId() )
  {
    pCharaModelInstance->ChangeAnimation( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  振り向いた後のモーションIDを取得
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval モーションID
*/
//-----------------------------------------------------------------------------
u32 FieldMoveCodeNpc::GetRotationAfterMotionId(FieldMoveCodeWork* pMoveCodeWork)
{
  u32 motionId = pMoveCodeWork->pModel->GetDefaultIdleAnimationId();

  switch( motionId )
  {
  case P2_BASE_FI025_SPEAK01_LOOP:
    {
      // 通常待機に置き換え
      return P2_BASE_FI001_WAIT01;
    }

  default:
    {
      return motionId;
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  現在の向きを戻り先向きとして保存する
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval 無し
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::SaveNowDirForReturn(FieldMoveCodeWork* pMoveCodeWork)
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // 初回更新時の向きを記憶
  gfl2::math::Quaternion npcRotation = pMoveCodeWork->pModel->GetLocalSRT().rotation;
  f32 npcNowRotationRad = NpcTool::ConvRotationQuaternionToYRadian( npcRotation );
  pWork->returnRotationDegY = gfl2::math::ConvRadToDeg( npcNowRotationRad );
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目対象キャラから注目座標を取得する
*
*  @param  pMoveModel 注目対象
*  @param  pOut       注目座標
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::GetInterestPosition( FieldMoveModel* pMoveModel, gfl2::math::Vector3* pOut )
{
  // カリング中 or 目のロケータが無い場合は足元座標を返す @note 対象は自機なのでありえないはずだが念のため
  // 自機のモデルインスタンスが存在しない場合は足元座標を返す @note ライドイベントなどで発生するが、ライドイベント中にこの関数が実行される事は無い
  if( pMoveModel->IsEnableCulilng() || pMoveModel->GetCharaDrawInstance() == NULL )
  {
    *pOut = pMoveModel->GetLocalSRT().translate;
  }
  else
  {
    const poke_3d::model::BaseModel* pModel = pMoveModel->GetCharaDrawInstance();
    s32 jointIndex = pModel->GetJointIndex( "Loc_Eye" );
    if( jointIndex == -1 )
    {
      *pOut = pMoveModel->GetLocalSRT().translate;
    }
    else
    {
      *pOut = pModel->GetJointWorldMatrix( jointIndex ).GetElemPosition();
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目座標を更新する
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  rPos           注目座標
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::UpdateInterestPosition( FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rPos ) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();


  //
  // 注目座標が背後かチェック
  //
  b32 isHide = false;

  // NPCの向きベクトルを取得
  gfl2::math::Matrix34  nowRotMat;
  gfl2::math::Vector3 nowRotVec(0.0f,0.0,1.0f);
  pCharaModelInstance->GetRotationQuat().QuaternionToMatrix( nowRotMat );  
  nowRotVec   = nowRotMat.TransformNormal(nowRotVec);
  nowRotVec.y = 0.0f;
  if( nowRotVec.Length() <= 0.0001f )
  {
    return;
  }

  nowRotVec   = nowRotVec.Normalize();

  // 目標向きベクトルを取得
  gfl2::math::Vector3 charToLookVec( rPos - pCharaModelInstance->GetPosition() );
  charToLookVec.y = 0.0f;
  if( charToLookVec.Length() <= 0.0001f )
  {
    return;
  }

  charToLookVec = charToLookVec.Normalize();

  // 今の向きから注目方向の角度を求める
  f32 dot = charToLookVec.Dot( nowRotVec );
  f32 angleToTargetDeg = gfl2::math::ConvRadToDeg( atan2( gfl2::math::Vector2( nowRotVec.x, nowRotVec.z ).Cross( gfl2::math::Vector2( charToLookVec.x, charToLookVec.z ) ), dot ) );

  if( angleToTargetDeg >= 180.0f - m_Setting.hideRangeDeg || angleToTargetDeg <= (-180.0f + m_Setting.hideRangeDeg ) )
  {
    if( !pWork->hideInterestFlag )
    {
      // 開始時の向きを記憶する 
      pWork->hideInterestFlag = 1;
      pWork->hideInterestFirstDir = ( angleToTargetDeg > 0 );
    }

    // 背後判定領域境界のどちらかに注目座標を加工する事で、首の向きを最初に向いた方向に固定する
    f32 rotDeg = 0.0f;
    if( pWork->hideInterestFirstDir )
    {
      rotDeg = ( 180.0f - m_Setting.hideRangeDeg ) - angleToTargetDeg;
    }
    else
    {
      rotDeg = ( -180.0f + m_Setting.hideRangeDeg ) - angleToTargetDeg;
    }

    // 加工
    gfl2::math::Vector3 charToLookVecWork( rPos - pCharaModelInstance->GetPosition() );
    charToLookVecWork = gfl2::math::Matrix34::GetRotationY( gfl2::math::ConvDegToRad( rotDeg ) * -1.0f ).TransformCoord( charToLookVecWork );
    pMoveCodeWork->pModel->EnableInterest(  pCharaModelInstance->GetPosition() + charToLookVecWork );
  }
  else
  {
    // そのまま注目
    pWork->hideInterestFlag = 0;
    pWork->hideInterestFirstDir = 0;
    pMoveCodeWork->pModel->EnableInterest( rPos );
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目を解除する
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::DisableInterest( FieldMoveCodeWork* pMoveCodeWork )
{
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  
  pMoveCodeWork->pModel->DisableInterest( m_Setting.disableInterestFrame );
  pWork->hideInterestFlag = 0;
  pWork->hideInterestFirstDir = 0;
}


//-----------------------------------------------------------------------------
/**
*  @brief  注目キャラの体の厚みを取得
*
*  @param  pMoveModel 対象
*  @param  isPlayer   対象がプレイヤーならtrueを指定
*
*  @return 厚み(cm)
*/
//-----------------------------------------------------------------------------
f32 FieldMoveCodeNpc::GetBodyWidth( FieldMoveModel* pMoveModel, b32 isPlayer )
{
  // 自機は当たる側、NPCは当たられる側に値が入っている
  if( isPlayer )
  {
    DynamicActor* pDynamicActor = pMoveModel->GetDynamicActor( Field::MoveModel::COLLISION_TYPE_STATIC_MODEL );
    if( pDynamicActor == NULL )
    {
      return 0.0f;
    }

    switch( pDynamicActor->GetShapeType() )
    {
    case BaseActor::SHAPE_CYLINDER:
      {
        CylinderShape* pShape = static_cast< CylinderShape* >( pDynamicActor->GetShape() );
        return pShape->m_r;
      }

    case BaseActor::SHAPE_SPHERE:
      {
        SphereShape* pShape = static_cast< SphereShape* >( pDynamicActor->GetShape() );
        return pShape->m_r;
      }

    default:
      {
        // ここにくる事は無いはず
      }
    }
  }
  else
  {
    StaticActor* pStaticActor = pMoveModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL );
    if( pStaticActor == NULL )
    {
      return 0.0f;
    }

    switch( pStaticActor->GetShapeType() )
    {
    case BaseActor::SHAPE_CYLINDER:
      {
        CylinderShape* pShape = static_cast< CylinderShape* >( pStaticActor->GetShape() );
        return pShape->m_r;
      }

    case BaseActor::SHAPE_BOX:
      {
        BoxShape* pShape = static_cast< BoxShape* >( pStaticActor->GetShape() );
        return gfl2::math::max( pShape->m_e0, pShape->m_e1 );
      }

    default:
      {
        // ここにくる事は無いはず
      }
    }
  }

  return 0.0f;
}

// オプション確認関連

//-----------------------------------------------------------------------------
/**
*  @brief  回転オプションが有効か取得
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 有効
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::IsEnableRotation(FieldMoveCodeWork* pMoveCodeWork) const
{
#if PM_DEBUG
  if( m_DebugSetting.isUse )
  {
    return ( !m_DebugSetting.invalidRotation );
  }
#endif

  // コリジョンがシリンダー以外の場合は回転させない
  StaticActor* pStaticActor = pMoveCodeWork->pModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL );
  if( pStaticActor == NULL || pStaticActor->GetShapeType() != BaseActor::SHAPE_CYLINDER )
  {
    return false;
  }

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return !( pWork->option & Option::INVALID_ROTATION );
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目オプションが有効か取得
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 有効
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::IsEnableInterest(FieldMoveCodeWork* pMoveCodeWork) const
{
#if PM_DEBUG
  if( m_DebugSetting.isUse )
  {
    return ( !m_DebugSetting.invalidInterest );
  }
#endif
  
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return !( pWork->option & Option::INVALID_INTEREST );
}

//-----------------------------------------------------------------------------
/**
*  @brief  初期状態に戻るオプションが有効か取得
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 有効
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::IsEnableReturn(FieldMoveCodeWork* pMoveCodeWork) const
{
#if PM_DEBUG
  if( m_DebugSetting.isUse )
  {
    return ( !m_DebugSetting.invalidReturn );
  }
#endif
  
  // コリジョンがシリンダー以外の場合は回転させない
  StaticActor* pStaticActor = pMoveCodeWork->pModel->GetStaticActor( Field::MoveModel::COLLISION_STATIC_TYPE_STATIC_MODEL );
  if( pStaticActor == NULL || pStaticActor->GetShapeType() != BaseActor::SHAPE_CYLINDER )
  {
    return false;
  }

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return !( pWork->option & Option::INVALID_RETURN );
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目範囲拡大オプションが有効か取得
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 有効
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::IsEnableLookWide(FieldMoveCodeWork* pMoveCodeWork) const
{
#if PM_DEBUG
  if( m_DebugSetting.isUse )
  {
    return ( m_DebugSetting.enableLookWide );
  }
#endif

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return ( pWork->option & Option::ENABLE_LOOK_WIDE );
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目範囲縮小オプションが有効か取得
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 有効
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpc::IsEnableLookShort(FieldMoveCodeWork* pMoveCodeWork) const
{
#if PM_DEBUG
  if( m_DebugSetting.isUse )
  {
    return ( m_DebugSetting.enableLookShort );
  }
#endif

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  return ( pWork->option & Option::ENABLE_LOOK_SHORT );
}

#if PM_DEBUG
//-----------------------------------------------------------------------------
/**
*  @brief  デバッグ設定を反映
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpc::ApplyDebugSetting(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 注目制限上書き
  if( m_DebugSetting.isMcnfOverWrite )
  {
    poke_3d::model::BaseModel* pModel = pMoveCodeWork->pModel->GetCharaDrawInstance();
    if( pModel )
    {
      for( u32 i = 0; i < GFL_NELEMS( m_DebugSetting.interestLimit ); i++ )
      {
        pModel->SetInterestLimitAngleHigh( i, gfl2::math::ConvDegToRad( m_DebugSetting.interestLimit[i]._High ));
        pModel->SetInterestLimitAngleHigh2( i,  gfl2::math::ConvDegToRad(m_DebugSetting.interestLimit[i]._High2 ));
        
        pModel->SetInterestLimitAngleLow( i, gfl2::math::ConvDegToRad( m_DebugSetting.interestLimit[i]._Low ));
        pModel->SetInterestLimitAngleLow2( i, gfl2::math::ConvDegToRad( m_DebugSetting.interestLimit[i]._Low2 ));
 
        // 初期値が不明瞭なので無効化
        // pModel->SetInterestSpeed( i, m_DebugSetting.interestLimit[i]._Speed );
        // GFL_PRINT("GetInterestSpeed %d %f \n", i, pModel->GetInterestSpeed( i ) );
       }
    }
  }
}
#endif // PM_DEBUG


//
// NPC挙動   (預かり屋 ※育て屋前のNPC)
//

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpcAzukariya::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  //
  // NPC基本処理更新
  //
  FieldMoveCodeNpc::MainProcess( pMoveCodeWork );

  //
  // 預かり屋処理
  //
  if( pWork->state == State::IDLE )
  {
    if( IsEggExist() )
    {
      OnOption( pMoveCodeWork, Option::INVALID_ROTATION );
      OnOption( pMoveCodeWork, Option::ENABLE_LOOK_SHORT );
      StartEggExistMotion( pMoveCodeWork );
    }
    else
    {
      // @note 解除処理はスクリプト側でタマゴを処理した際に行っている。sodateya.pを参照。
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  中断
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpcAzukariya::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  FieldMoveCodeNpc::Suspend( pMoveCodeWork );

  // ゾーンチェンジ時にモーション切り替え済みにする
  if( pWork->state == State::READY && IsEggExist() )
  {
    StartEggExistMotion( pMoveCodeWork, true );
  }
  
  // @fix Mcat[2042]:待機モーションが特殊なNPCに話しかけ会話済み状態になった後にフィールド破棄/生成を行うと、話しかけた方向のまま特殊モーションが再生される、追加対処。中断によるモーション変更を覚えるように変更。
  pWork->prevMotionId = pCharaModelInstance->GetAnimationId();
}

/**
*  @brief  動作コード処理  復帰
*/
void FieldMoveCodeNpcAzukariya::Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const
{
  FieldMoveCodeNpc::Comeback( pMoveCodeWork, isLoad );

  // アプリ・バトル復帰時にモーション切り替え済みにする 
  // @note 会話済みフラグは参照しない、会話直後にタマゴモーションに遷移する仕様
  if( IsEggExist() )
  {
    StartEggExistMotion( pMoveCodeWork, true );
  }
}

//----------------------------------------------------------------------------
/**
*  @brief  タマゴ発見しているか？
*
*  @retval true 発見している false 発見していない 
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeNpcAzukariya::IsEggExist( void ) const
{
  Savedata::Sodateya* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetSodateyaSave();
  return pSave->IsEggExist( Savedata::SODATEYA_ID_I02 );
}

//----------------------------------------------------------------------------
/**
*  @brief  タマゴ発見モーション開始( 既に待機モーションなら何も再生しない )
*
*  @param  pMoveCodeWork  動作コードワーク
*  @param  forceFlag      true: モーション補間無しで切り替え
*
*  @retval モーションID
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeNpcAzukariya::StartEggExistMotion(FieldMoveCodeWork* pMoveCodeWork, b32 forceFlag ) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // 現在のデフォルトモーションが違うなら変更
  if( pMoveCodeWork->pModel->GetDefaultIdleAnimationId() != P2_BASE_EV9999_BROOD_LOOP )
  {
    pMoveCodeWork->pModel->SetDefaultIdleAnimationId( P2_BASE_EV9999_BROOD_LOOP );
  }

  if( forceFlag )
  {
    pCharaModelInstance->ChangeAnimationSmooth( P2_BASE_EV9999_BROOD_LOOP, 0 );
  }
  else
  {
    // 現在のモーションが違うなら変更
    if( pCharaModelInstance->GetAnimationId() != P2_BASE_EV9999_BROOD_LOOP )
    {
      pCharaModelInstance->ChangeAnimationSmooth( P2_BASE_EV9999_BROOD_LOOP, 14 );
    }
  }
}


// ふれあい

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  メイン
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::MainProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  
	// レジストされていない場合は何もしない
	Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
	if (pFieldMoveModelActor == NULL || !pFieldMoveModelActor->IsRegist())
	{
		return;
	}

  b32 continueFlag = false;
  do
  {
    switch (pWork->state)
    {
    case State::READY:
    {
      continueFlag = FieldMoveCodeNpc::SubReadyProcess(pMoveCodeWork);
      break;
    }

    case State::IDLE:
    {
      continueFlag = SubIdleProcess(pMoveCodeWork);
      break;
    }

    case State::ROTATION:
    {
      continueFlag = FieldMoveCodeNpc::SubRotationProcess(pMoveCodeWork);
      break;
    }
    case State::URESHII:
    {
      continueFlag = SubUreshiiProcess(pMoveCodeWork);
      break;
    }
    case State::SLEEP:
    {
      continueFlag = SubSleepProcess(pMoveCodeWork);
      break;
    }
    case State::ONE_ROTATION:
    {
		  continueFlag = SubOneRotationProcess(pMoveCodeWork);
      break;
    }
		case State::ICON:
		{
			continueFlag = SubIconProcess(pMoveCodeWork);
			break;
		}
    }
  } while (continueFlag);
}

//----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  ポスト
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::PostProcess(FieldMoveCodeWork* pMoveCodeWork) const
{ 
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  
	// キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  
	FieldMoveCodeNpc::PostProcess( pMoveCodeWork );

#if 0
  // ランダム待機カウント更新
	if (pWork->state == State::IDLE && pWork->subState == SubState::DISCOVER && pCharaModelInstance->GetAnimationId() == GetRotationAfterMotionId(pMoveCodeWork) )
	{
	}
	else
	{
		CountResetRndIdle(pMoveCodeWork);
	}
#endif


  // 主人公の注視リクエスト制御 
  // @fix GFMMcat[396]:ふれあいポケモンへの注視が不要に継続する、対処。発見状態が解除される距離の場合は遅延してリクエストを切る
  if(pWork->subState == SubState::DISCOVER_ICON_WAIT || pWork->subState == SubState::DISCOVER)
  {
    f32 lostDistance = m_Setting.walk.lostDistance;
    if (HitCheckReactionRangeToPlayer(pMoveCodeWork, lostDistance))
    {
      pWork->playerInterestReqCount = 10;
    }
    else
    {
      if (pWork->playerInterestReqCount > 0)
      {
        pWork->playerInterestReqCount--;
      }
    }
  }
  else
  {
    pWork->playerInterestReqCount = 0;
  }

	// PlayerPush( pMoveCodeWork );

  // 移動範囲内に強制吸着
  PositionClampMoveArea( pMoveCodeWork );
}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  中断
*
*  @param  pMoveCodeWork  動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::Suspend(FieldMoveCodeWork* pMoveCodeWork) const
{  
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();


  switch( pWork->state )
  {
	case State::READY:
	  {
		  // ゾーン初期化時に寝る判定を行う
			
			// レジストされていない場合は何もしない
			Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
			if (pFieldMoveModelActor && pFieldMoveModelActor->IsRegist())
			{
				if (PokemonFureai::IsStartSleep(pMoveCodeWork->pModel))
				{
					StartSleep(pMoveCodeWork);
				}
			}
	    break;
    }

  case State::IDLE:
    {
      // 待機モーションに移行
      if (pCharaModelInstance->GetAnimationId() != GetRotationAfterMotionId(pMoveCodeWork))
      {
        pCharaModelInstance->ChangeAnimation(GetRotationAfterMotionId(pMoveCodeWork));
      }
      break;
    }

  case State::ROTATION:
    {
      // 強制打ち切り
      pWork->state = State::IDLE;

      // モーションを待機に変更
      pCharaModelInstance->ChangeAnimation( GetRotationAfterMotionId( pMoveCodeWork ) );

      break;
    }

  case State::URESHII:
    {
      // 強制打ち切り
      pWork->state = State::IDLE;

      // モーションを待機に変更
      pCharaModelInstance->ChangeAnimation( GetRotationAfterMotionId( pMoveCodeWork ) );
      break;
    }

  case State::SLEEP:
    {
      // そのまま
      break;
    }

  case State::ONE_ROTATION:
    {
	    // 強制打ち切り
	    pWork->state = State::IDLE;

	    // モーションを待機に変更
	    pCharaModelInstance->ChangeAnimation(GetRotationAfterMotionId(pMoveCodeWork));
	    break;
    }
  case State::ICON:
    {
	    // 強制打ち切り
	    pWork->state = State::IDLE;

	    // モーションを待機に変更
	    pCharaModelInstance->ChangeAnimation(GetRotationAfterMotionId(pMoveCodeWork));
	    break;
    }
  }

  switch (pWork->subState)
  {
  case SubState::LOST:
  {
    break;
  }
  case SubState::DISCOVER:
  {
    pWork->isMoveStart = false;
    pWork->moveKeepCount = 0;
    pWork->moveWaitCount = m_Setting.moveWaitCount;
    break;
  }
  case SubState::DISCOVER_ICON_WAIT:
  {
    break;
  }
  
  // 未使用
  case SubState::LOST_ICON_WAIT:
  {
    break;
  }

  // 未使用
  case SubState::END:
  {
    pWork->isMoveStart = false;
    pWork->moveKeepCount = 0;
    pWork->moveWaitCount = m_Setting.moveWaitCount;
    break;
  }

  // 未使用
  case SubState::KININARU:
  {
    // DISCOVERに戻す
    pWork->subState = SubState::DISCOVER;
    pWork->isMoveStart = false;
    pWork->moveKeepCount = 0;
    pWork->moveWaitCount = m_Setting.moveWaitCount;
    break;
  }
  }
  
  // 各種カウンターをリセット
  pWork->dirResetWaitCount = 0;
  pWork->isDirResetWait = false;
  pWork->rotEnterWaitCount = 0;
  pWork->frontInterestCount = 0;
	CountResetRndIdle(pMoveCodeWork);

  // 走り継続アイコンの制御
  pWork->runIconWaitCount = 30 * 3;
  pWork->runIconShowCount = 0;

	// 連続回転
	pWork->renzokuRotCount = 0;
	pWork->renzokuRotWait = 0;
	pWork->renzokuRotIsLeft = 0;

	// 主人公アクションリクエスト情報
  pWork->playerActionReq = 0;

  // ムーランド反応
  pWork->isMuurandAciton = 0;
  
  // pWork->rotationCount = 0;    回転した事は記憶しておく
#if 0
  // ゾーンチェンジ時にモーション切り替え済みにする
  if( pWork->state == State::SLEEP )
  {
    StartSleep( pMoveCodeWork );
  }
#endif

  pWork->playerInterestReqCount = 0;

  // @fix Mcat[2042]:待機モーションが特殊なNPCに話しかけ会話済み状態になった後にフィールド破棄/生成を行うと、話しかけた方向のまま特殊モーションが再生される、追加対処。中断によるモーション変更を覚えるように変更。
  pWork->prevMotionId = pCharaModelInstance->GetAnimationId();

}

//-----------------------------------------------------------------------------
/**
*  @brief  動作コード処理  復帰
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::Comeback(FieldMoveCodeWork *pMoveCodeWork, b32 isLoad) const
{
  FieldMoveCodeNpc::Comeback( pMoveCodeWork, isLoad );
  
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  if (pWork->state == State::SLEEP)
  {
    StartSleep(pMoveCodeWork);
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  近づいた後の座標を取得
*
*  @param  rNowPositon 現在座標
*  @param  pOut        座標
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::GetComeNearPosition(FieldMoveCodeWork* pMoveCodeWork, gfl2::math::Vector3* pOut, b32 isRunEnable )
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  
  // ふれあいランク
  s32 rank = PokemonFureai::GetRank( pMoveCodeWork->pModel );
  const SETTING::CAME_NEAR* pCameNearSetting = &m_Setting.cameNear[ rank ];

  gfl2::math::Vector3 pos_player(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  pos_player.y = 0.0;
  gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);
  pos_mine.y = 0.0;
  gfl2::math::Vector3 npcToPlayerVec = (pos_player - pos_mine);
  // npcToPlayerVec.y = 0.0f;

  // 自機との距離を取得
  f32 distanceSqForPlayer(npcToPlayerVec.LengthSq());
  f32 distanceForPlayer(npcToPlayerVec.Length());

  // コリジョンの厚みを取得
  f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
  f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);
  f32 playerBodyWidth2 = GetBodyWidth(pFieldMap->GetPlayerCharacter(), false);
  f32 npcBodyWidth2 = GetBodyWidth(pMoveCodeWork->pModel, true);
  f32 geta = 0.0f;

  f32 distance = distanceForPlayer - playerBodyWidth - npcBodyWidth;

  // その場でOK
  if ( distance <= pCameNearSetting->baseBuffetDistance )
  {
    return PokemonFureai::FPF_COME_NEAR_TYPE_NO_MOVE;
  }

  // 移動判定

  // 移動目標算出
  gfl2::math::Vector3 moveTarget;
  moveTarget = pos_player + npcToPlayerVec.Normalize() * (playerBodyWidth + npcBodyWidth + pCameNearSetting->baseDistance) * -1.0f;
  b32 isMove = true;
  

  // 移動を加工
  gfl2::math::Vector3 newMoveTarget;
  PositionClampMoveAreaForComeNear( pMoveCodeWork, pos_mine, moveTarget, &newMoveTarget );

  gfl2::math::Vector3 npcToNewTargetVec( newMoveTarget - pos_mine );
  if (npcToNewTargetVec.Length() <= 1.0f )
  {
    isMove = false;
  }
  else
  {
    // 逆
    gfl2::math::Vector3 work2( npcToNewTargetVec );
    gfl2::math::Vector3 work1( npcToPlayerVec );
    work2.NormalizeRet();
    work1.NormalizeRet();
    if (work1.Dot(work2) <= 0.0f)
    {
      isMove = false;
    }
  }

  if (isMove)
  {
    b32 isRun = true;
    *pOut = newMoveTarget;

    // 移動手段決定
    f32 moveLength = npcToNewTargetVec.Length();
#if 0
    // 短ければ歩く
    if (moveLength <= pCameNearSetting->baseWalkDistance)
    {
      isRun = false;
    }
#endif
    if( isRunEnable == false )
    {
      isRun = false;
    }

    // 移動後に話せる？
    gfl2::math::Vector3 newTargetToPlayerVec( pos_player - newMoveTarget );
    f32 new_distance = newTargetToPlayerVec.Length() - playerBodyWidth - npcBodyWidth;
    if (new_distance <= pCameNearSetting->baseBuffetDistance)
    {
      if (isRun)
      {
        return PokemonFureai::FPF_COME_NEAR_TYPE_RUN;
      }
      else
      {
        return PokemonFureai::FPF_COME_NEAR_TYPE_WALK;
      }
    }
    else
    {
      if (isRun)
      {
        return PokemonFureai::FPF_COME_NEAR_TYPE_RUN_NG;
      }
      else
      {
        return PokemonFureai::FPF_COME_NEAR_TYPE_WALK_NG;
      }
    }
  }
  else
  {
    return PokemonFureai::FPF_COME_NEAR_TYPE_NO_MOVE_NG;
  }
}

b32 FieldMoveCodeFureaiTypeA::SetEndStatus(FieldMoveCodeWork* pMoveCodeWork, s32 endType )
{

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
	
  // 初移動系は終了した扱いにする
  pWork->isIconEndFlag = true;
  pWork->isFirstMoveEndFlag = true;
  pWork->isFirstRun = false;  // @note 未使用だが念のため

  // @note その他、細かいカウンターはサスペンドでリセットされている
#if 0
  // @note サスペンドでリセットされているもの
  
  // 各種カウンターをリセット
  pWork->dirResetWaitCount = 0;
  pWork->isDirResetWait = false;
  pWork->rotEnterWaitCount = 0;
  pWork->frontInterestCount = 0;
  CountResetRndIdle(pMoveCodeWork);

  // 連続回転
  pWork->renzokuRotCount = 0;
  pWork->renzokuRotWait = 0;
  pWork->renzokuRotIsLeft = 0;

  // 主人公アクションリクエスト情報
  pWork->playerActionReq = 0;

  // @note サスペンドでリセット(DISCOVERのみ)
  pWork->isMoveStart = false;
  pWork->moveKeepCount = 0;
  pWork->moveWaitCount = m_Setting.moveWaitCount;

#endif


  // @fix MMcat[115]:素早く話しかけると、会話後に再発見される、対処。会話実行時は内部ステータスを発見状態に変更
  // 発見した扱いにする
  switch (pWork->subState)
  {
  case SubState::LOST:
  case SubState::DISCOVER_ICON_WAIT:
  {
    // 遅延反応をリセット
    pWork->isMoveStart = false;
    pWork->moveKeepCount = 0;
    pWork->moveWaitCount = m_Setting.moveWaitCount;

    // 初反応 @note SetEndStatusが呼ばれた時点で実行扱いにする
    // pWork->isIconEndFlag = 0;
    // pWork->isFirstMoveEndFlag = 0;

    // 基本カウンタ
    pWork->subStateCount = 0;
    pWork->subState = SubState::DISCOVER;

    // 走り継続アイコンの制御
    pWork->runIconWaitCount = 30 * 3;
    pWork->runIconShowCount = 0;

#if 0 
    // SubState::LOST -> DISCOVER_ICON_WAIT
    pWork->isMoveStart = false;
    pWork->moveKeepCount = 0;
    pWork->moveWaitCount = m_Setting.moveWaitCount;
    CountResetRndIdle(pMoveCodeWork);
    pWork->isIconEndFlag = 0;
    pWork->isFirstMoveEndFlag = 0;

    // SubState::DISCOVER_ICON_WAIT -> DISCOVER
    pWork->subStateCount = 0;
    pWork->subState = SubState::DISCOVER;
    pWork->runIconWaitCount = 30 * 3;
    pWork->runIconShowCount = 0;
    pWork->isMoveStart = true;
    pWork->moveKeepCount = m_Setting.moveKeepCount;

    pWork->isFirstRun = true;
#endif
    break;
  }
  }


	switch (endType)
	{
	case PokemonFureai::FPF_FUREAI_EVENT_END_TYPE_MANZOKU: // 満足した
	{
    // END状態へ
		pWork->isByeByeEnd = 1;
	  pWork->isByeByeClear = 0;

    // 喜ぶ
		StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RANK_A );
		break;
	}
	case PokemonFureai::FPF_FUREAI_EVENT_END_TYPE_FUMAN: // 不満がある
	{
		pWork->isByeByeEnd = 0;
	  pWork->isByeByeClear = 0;
    
		// 「…」後、Discover継続
		StartIcon( pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1, true );
		break;
	}

	case PokemonFureai::FPF_FUREAI_EVENT_END_TYPE_NG: // ふれあいできなかった
	{
		pWork->isByeByeEnd = 0;
	  pWork->isByeByeClear = 0;
		break;
	}

	}
#if 0

  pWork->subState = SubState::END;
  pWork->subStateCount = 0;

  pWork->isComEventEndFlagDiscoverAfter = 1;
  pWork->isComEventEndFlag = 1;
    
  pWork->isMoveStart = false;
  pWork->moveKeepCount = 0;
  pWork->moveWaitCount = m_Setting.moveWaitCount;
#endif
  return true;
}

b32 FieldMoveCodeFureaiTypeA::SetSleepStart(FieldMoveCodeWork* pMoveCodeWork)
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  pWork->state = State::SLEEP;

	StartSleep( pMoveCodeWork );
  return true;
}

b32 FieldMoveCodeFureaiTypeA::SetSleepEndStatus(FieldMoveCodeWork* pMoveCodeWork)
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  
  pWork->state = State::IDLE;
  pWork->subState = SubState::DISCOVER;
  pWork->subStateCount = 0;
  pWork->isMoveStart = false;
  pWork->moveKeepCount = 0;
  pWork->moveWaitCount = m_Setting.moveWaitCount;
	CountResetRndIdle(pMoveCodeWork);
  pWork->isIconEndFlag = 0;
  pWork->isFirstMoveEndFlag = 0;
  
  return true;
}

b32 FieldMoveCodeFureaiTypeA::EventPlayerActionReq(FieldMoveCodeWork* pMoveCodeWork, s32 type )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // 既に反応している
	switch (pWork->state)
	{
	case State::READY:
	case State::URESHII:
	case State::SLEEP:
	case State::ONE_ROTATION:
	{
		return false;
	}
	case State::ICON:
	case State::IDLE:
	case State::ROTATION:
	{
		break;
	}
	}

  // 気づいている場合のみ
  if (pWork->subState == SubState::DISCOVER )
  {
		pWork->playerActionReq = type;
    return true;
  }
  else
  {
    return false;
  }
}

b32 FieldMoveCodeFureaiTypeA::IsPlayerInterest(FieldMoveCodeWork* pMoveCodeWork)
{
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // @fix GFMMcat[396]:ふれあいポケモンへの注視が不要に継続する、対処。発見状態が解除される距離の場合は遅延してリクエストを切る
  if ( pWork->playerInterestReqCount > 0 )
  {
    return true;
  }
  else
  {
    return false;
  }
}

b32 FieldMoveCodeFureaiTypeA::IsPlayerUreshiiAction(FieldMoveCodeWork* pMoveCodeWork)
{
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  
  switch (FieldMoveCodePlayer::GetPlayerMove(pFieldMap->GetPlayerCharacter()->GetMoveCodeWork()))
  {
  case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_LEFT_END:
  case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_RIGHT_END:
  {
    if (pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->GetAnimationFrame() > 30)
    {
      return true;
    }
  }
  }
  return false;
}


b32 FieldMoveCodeFureaiTypeA::IsMuurandHeadNear(FieldMoveCodeWork* pMoveCodeWork)
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  // サーチ中？
  if (pFieldMap->GetPlayerCharacter()->IsMuurandoSearchStateEx())
  {
    gfl2::math::Vector3 npcPos = pMoveCodeWork->pModel->GetLocalSRT().translate;
    f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);

    gfl2::math::Vector3 playerPos(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
    f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);

    gfl2::math::Vector3 offset(0.0f, 0.0f, playerBodyWidth);
    offset = pFieldMap->GetPlayerCharacter()->GetLocalSRT().rotation.Transform(offset);

    playerPos += offset;

    playerPos.y = 0.0f;
    npcPos.y = 0.0f;

    f32 distance = ( playerPos - npcPos ).Length() - npcBodyWidth;
    f32 diffAng = GetDirDiffToTarget( pMoveCodeWork, playerPos );

    if (diffAng < 20 && distance < 10)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  注目対象キャラから注目座標を取得する
*
*  @param  pMoveModel 注目対象
*  @param  pOut       注目座標
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::GetInterestPositionForPokemon( FieldMoveCodeWork* pMoveCodeWork, FieldMoveModelPlayer* pMoveModel, gfl2::math::Vector3* pOut )
{
  if (pMoveModel->GetOnMoveModel())
  {
    gfl2::math::Vector3 player_pos;
    gfl2::math::Vector3 ride_pos;
    gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);

    // 主人公とライドポケモンの近いほうを見る
    GetInterestPosition( pMoveModel, &player_pos );
    GetInterestPosition( pMoveModel->GetOnMoveModel(), &ride_pos );

    // 距離の比較ではY軸を無視
    gfl2::math::Vector3 player_pos_work( player_pos );
    gfl2::math::Vector3 ride_pos_work( ride_pos );
    player_pos_work.y = 0.0f;
    ride_pos_work.y = 0.0f;
    pos_mine.y = 0.0f;
    if ((player_pos_work - pos_mine).LengthSq() <= (ride_pos_work - pos_mine).LengthSq())
    {
      *pOut = player_pos;
      GetInterestPosition( pMoveModel, pOut );
    }
    else
    {
      *pOut = ride_pos;
    }
  }
  else
  {
    GetInterestPosition( pMoveModel, pOut );
  }
}

b32 FieldMoveCodeFureaiTypeA::IsSleep(FieldMoveCodeWork* pMoveCodeWork)
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  return ( pWork->state == State::SLEEP );
}

b32 FieldMoveCodeFureaiTypeA::CanNadeNadeDistance(FieldMoveCodeWork* pMoveCodeWork)
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  gfl2::math::Vector3 pos_player(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  pos_player.y = 0.0;
  gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);
  pos_mine.y = 0.0;
  gfl2::math::Vector3 npcToPlayerVec = (pos_player - pos_mine);
  // npcToPlayerVec.y = 0.0f;

  // 自機との距離を取得
  f32 distanceSqForPlayer(npcToPlayerVec.LengthSq());
  f32 distanceForPlayer(npcToPlayerVec.Length());

  // コリジョンの厚みを取得
  f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
  f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);
  f32 playerBodyWidth2 = GetBodyWidth(pFieldMap->GetPlayerCharacter(), false);
  f32 npcBodyWidth2 = GetBodyWidth(pMoveCodeWork->pModel, true);
  f32 geta = 0.0f;

  f32 distance = distanceForPlayer - playerBodyWidth - npcBodyWidth;

  // 親密の距離以内ならOK
  const SETTING::CAME_NEAR* pCameNearSetting = &m_Setting.cameNear[ PokemonFureai::FPF_RANK_LOVE ];
  if ( distance <= pCameNearSetting->baseBuffetDistance + 0.5f )  // 計算誤差を考慮し、下駄
  {
    return true;
  }
  else
  {
    return false;
  }
}

f32 FieldMoveCodeFureaiTypeA::GetBaseDistance(FieldMoveCodeWork* pMoveCodeWork)
{
	// 自機取得のため、グローバルアクセス
	GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
	Fieldmap* pFieldMap = pGameManager->GetFieldmap();

	f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
	f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);

	return GetCameNearSettingData(pMoveCodeWork)->baseDistance + playerBodyWidth + npcBodyWidth;
}

f32 FieldMoveCodeFureaiTypeA::GetBaseDistanceForModorou(FieldMoveCodeWork* pMoveCodeWork)
{
	// 自機取得のため、グローバルアクセス
	GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
	Fieldmap* pFieldMap = pGameManager->GetFieldmap();

	f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
	f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);

	return GetCameNearSettingData(pMoveCodeWork)->baseDistanceForModorou + playerBodyWidth + npcBodyWidth;
}


//----------------------------------------------------------------------------
/**
*  @brief  設定データへのポインタを取得
*  @note   デバッグメニューで操作するため、それ以外では使用禁止
*/
//-----------------------------------------------------------------------------
FieldMoveCodeFureaiTypeA::SETTING::CAME_NEAR* FieldMoveCodeFureaiTypeA::GetCameNearSettingData(FieldMoveCodeWork* pMoveCodeWork)
{
	// ふれあいランク
	s32 rank = PokemonFureai::GetRank(pMoveCodeWork->pModel);
	return &m_Setting.cameNear[rank ];
}

//----------------------------------------------------------------------------
/**
*  @brief  初回更新処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubReadyProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  pWork->state = State::IDLE;
  return true;
}

//----------------------------------------------------------------------------
/**
*  @brief  待機中処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubIdleProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  
  b32 continueFlag = false;      // true もう一度更新
  b32 noActionFlag = true;       // true 1更新で1度しか起きないようなアクションがあった
  b32 dirResetReqFlag = false;   // true 向きリセットのリクエスト  
  b32 isPlayerInterest = false;
  b32 isParentInterest = false;

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork( pMoveCodeWork, sizeof(Work) );
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  
  // ふれあいランク
  s32 rank = PokemonFureai::GetRank( pMoveCodeWork->pModel );
  s32 fureaiNo = PokemonFureai::GetFureaiNo( pMoveCodeWork->pModel );
  
  // ふれあい設定
  Field::FieldResident* pFieldResident = pGameManager->GetFieldResident();
  Field::PokemonFureai::DataAccessor dataAccessor(pFieldResident->GetResource(BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN));
  const Field::PokemonFureai::POKEMON_SETTING_DATA* pPokemonSettingData = dataAccessor.GetSettingData( pMoveCodeWork->pModel->GetCharacterId() );

    // 自機取得のため、グローバルアクセス
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  b32 isPlayerMove = false;
  if (pFieldMap->GetPlayerCharacter()->IsMoveState() )
  {
    isPlayerMove = true;
  }

  // デバッグ設定反映
#if PM_DEBUG
	pFieldMoveModelActor->SetDebugFureaiMoveAreaCollision(m_DebugSetting.isMoveAreaCollison);
#endif

  switch (pWork->subState)
  {
  case SubState::LOST:
  {
    b32 isReturn( true );
    pWork->isComEventEndFlagDiscoverAfter = 0;

    f32 discoverDitance(m_Setting.walk.discoverDistance);

    // @fix GFMMcat[325]キテルグマの反応範囲が広い、対処。キテルグマの場合は反応範囲を小さくする
    if (fureaiNo == Field::PokemonFureai::FPF_FUREAI_NO_KITERUGUMA )
    {
      m_Setting.walk.discoverDistance = 125.0f;
    }

    if (HitCheckReactionRangeToPlayer(pMoveCodeWork, discoverDitance))
    {
      isPlayerInterest = true;

      if (pWork->moveWaitCount == 0)
      {
        isReturn = false;

        // pWork->subState = SubState::DISCOVER;
        pWork->subState = SubState::DISCOVER_ICON_WAIT;
        pWork->subStateCount = 0;

        // 親密度に応じたアイコン再生
        Field::Effect::Type effectType = Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1;


        switch (rank)
        {
        case PokemonFureai::FPF_RANK_LIKE:
        {
          effectType = Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1;
          PokemonFureai::AddPoint(pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_DISCOVER);    // ふれあいポイント加算
          break;
        }
        case PokemonFureai::FPF_RANK_LOVE:
        {
          effectType = Effect::EFFECT_TYPE_FUREAI_ICON_HEART1;
          PokemonFureai::AddPoint(pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_DISCOVER);    // ふれあいポイント加算
          break;
        }
        }

        PokemonFureai::CreateEffect(effectType, pMoveCodeWork->pModel, true, true);

        pWork->isMoveStart = false;
        pWork->moveKeepCount = 0;
        pWork->moveWaitCount = m_Setting.moveWaitCount;
        CountResetRndIdle(pMoveCodeWork);
        pWork->isIconEndFlag = 0;
        pWork->isFirstMoveEndFlag = 0;
      }
      else
      {
        pWork->moveWaitCount--;
      }
#if 0
      // 向きチェック
      f32 diffToPlayerDeg = GetDirDiffToPlayer(pMoveCodeWork);
      if (diffToPlayerDeg < 45)
      {
        StartWalk(pMoveCodeWork);
        continueFlag = true;

        pWork->subState = SubState::DISCOVER;
        pWork->stopCount = 0;
      }
      else
      {
        // 回転
        StartRotation(pMoveCodeWork, GetDirToPlayer(pMoveCodeWork), m_Setting.rotationFrame);
        continueFlag = true;
      }
#endif
    }
    else
    {
      pWork->moveWaitCount = m_Setting.moveWaitCount;
    }

    if (isReturn)
    {


    }
    
    if (isReturn)
    {
      gfl2::math::Vector3 basePosition;
      pFieldMoveModelActor->GetFureaiBasePosition(&basePosition);
      gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);
      gfl2::math::Vector3 toBasePosition( basePosition - pos_mine );
      
      if (toBasePosition.Length() > m_Setting.basePositionBuffetDistance)
      {
        // 向きチェック
        f32 diffToPlayerDeg = GetDirDiffToTarget(pMoveCodeWork, basePosition);
        if (diffToPlayerDeg < m_Setting.rotRange)
        {
					if (pWork->returnWalkWaitCount > 0)
					{
						pWork->returnWalkWaitCount--;
						StartDefaultIdleMotion(pMoveCodeWork);
					}
					else
					{
            SubWalkToTargetProcess(pMoveCodeWork, basePosition );
					}
        }
        else
        {
          // 回転
          StartRotation(pMoveCodeWork, GetDirToTarget(pMoveCodeWork, basePosition), m_Setting.rotationFrame * (1.0f / pPokemonSettingData->speed));
          continueFlag = true;
        }
        isParentInterest = true;
      }
      else
      {
        StartDefaultIdleMotion(pMoveCodeWork);
      }

    }
    else
    {
      StartDefaultIdleMotion(pMoveCodeWork);
    }

    break;
  }

  case SubState::DISCOVER_ICON_WAIT:
  {
    isPlayerInterest = true;
    
    pWork->subStateCount += 1;
    if (pWork->subStateCount > 15)
    {
      pWork->subStateCount = 0;
      pWork->subState = SubState::DISCOVER;

      // 走りアイコン
      pWork->runIconWaitCount = 30 * 3;
      pWork->runIconShowCount = 0;

      // 直ぐ動くようにする
      pWork->isMoveStart = true;
      pWork->moveKeepCount = m_Setting.moveKeepCount;
      pWork->moveWaitCount = 0;

			pWork->isFirstRun = true;
    
      // 連続回転
      pWork->renzokuRotCount = 0;
      pWork->renzokuRotWait = 0;
      pWork->renzokuRotIsLeft = 0;
		
      // リクエストを念のため削除
      pWork->playerActionReq = PokemonFureai::FPF_PLAYER_ACITON_NONE;

      // ばいばい状態解除
      pWork->isByeByeClear = false;
      pWork->isByeByeEnd = false;

      // ムーランド反応
      pWork->isMuurandAciton = 0;

      // ランダム待機
      CountResetRndIdle(pMoveCodeWork);

      // 初移動系
      pWork->isIconEndFlag = 0;
      pWork->isFirstMoveEndFlag = 0;
    }

    break;
  }
 
  case SubState::DISCOVER:
  {
    b32 walk = false;
    isPlayerInterest = true;

#if 1
    // バイバイ解除判定
		if (pWork->isByeByeEnd)
		{
			f32 distanceToPlayer = GetDistanceToPlayer(pMoveCodeWork);

			if(pWork->isByeByeClear)
			{
        // 再度近づいたら解除
				if (distanceToPlayer < 100.0f)
				{
					pWork->isByeByeClear = false;
					pWork->isByeByeEnd = false;

					StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1, ICON_MODE_VERY_FAST);
					break;
					// PokemonFureai::CreateEffect(Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1, pMoveCodeWork->pModel, true);
					// PokemonFureai::AddPointByFieldIcon(pMoveCodeWork->pModel, Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1);  // ポイント加算
				}
			}
			else
			{
        // 離れた？
				if (distanceToPlayer >= 150.0f)
				{
					pWork->isByeByeClear = true;
				}
			}
		}
#endif

		// 主人公反応処理
		if (CheckAndStartPlayerReaction(pMoveCodeWork))
		{
			break;
		}


		f32 lostDistance = m_Setting.walk.lostDistance;
		if (pWork->isByeByeEnd)
		{
			// lostDistance = 300.0f;
		}

		if (HitCheckReactionRangeToPlayer(pMoveCodeWork, lostDistance ))
    {
      // 範囲内
      if (pWork->isMoveStart)
      {
        if ( isPlayerMove )
        {
          walk = true;
          pWork->moveKeepCount = m_Setting.moveKeepCount;
        }
        else
        {
          if (pWork->moveKeepCount > 0)
          {
            pWork->moveKeepCount--;
            walk = true;
          }
          else
          {
            pWork->moveWaitCount = m_Setting.moveWaitCount;
            pWork->isMoveStart = false;
          }
        }
      }
      else
      {
        if (isPlayerMove)
        {
          if (pWork->moveWaitCount > 0)
          {
            pWork->moveWaitCount--;
          }
          else
          {
            pWork->isMoveStart = true;
            pWork->moveKeepCount = m_Setting.moveKeepCount;
          }
        }
        else
        {
          pWork->moveWaitCount = m_Setting.moveWaitCount;
        }
      }
      pWork->subStateCount = 0;
    }
    else
    {
      pWork->isMoveStart = false;
      pWork->moveKeepCount = 0;
      pWork->moveWaitCount = m_Setting.moveWaitCount;

      pWork->subStateCount++;
      if ( pWork->subStateCount >= m_Setting.walk.stopCount)
      {
        pWork->subState = SubState::LOST;
        pWork->subStateCount = 0;
				pWork->returnWalkWaitCount = 0;

        // StartRotation(pMoveCodeWork, GetDirToPlayer(pMoveCodeWork) + 180, m_Setting.rotationFrame * ( 1.0f / pPokemonSettingData->speed ));
        continueFlag = true;

        // ばいばい状態解除
				pWork->isByeByeClear = false;
				pWork->isByeByeEnd = false;

        // 遅延移動
        pWork->isMoveStart = false;
        pWork->moveKeepCount = 0;
        pWork->moveWaitCount = m_Setting.moveWaitCount;

        // 念のため停止
        SubWalkStopProcess(pMoveCodeWork);
        break;
#if 0
        pWork->subState = SubState::LOST_ICON_WAIT;
        pWork->subStateCount = 0;

        // 親密度に応じたアイコン再生
        Field::Effect::Type effectType = Effect::EFFECT_TYPE_FUREAI_ICON_TEN1;
        Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
        s32 dataIndex;
        pEffectManager->CreateEffect(effectType, pMoveCodeWork->pModel, true, Effect::EffectManager::WORK_TYPE_SYS, &dataIndex);
#endif
      }
    }

    if (walk)
    {
      pWork->isFirstMoveEndFlag = true;
      
			// 向きチェック
      f32 diffToPlayerDeg = GetDirDiffToPlayer(pMoveCodeWork);
      if (diffToPlayerDeg < m_Setting.rotRange )
      {
				if ( pWork->isByeByeEnd ) // BYEBYEした後はついてこない
				{
					SubWalkStopProcess(pMoveCodeWork);
				}
				else
				{
					SubWalkToPlayerProcess(pMoveCodeWork);
				}
      }
      else
      {
        // 回転
				pWork->renzokuRotCount += 1;

				if (pWork->renzokuRotCount > 6)
				{
					isPlayerInterest = false;
					
					if ( rank == PokemonFureai::FPF_RANK_UNKNOWN )
					{
						StartIcon( pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1, ICON_MODE_FAST );
					}
					else
					{
						b32 isLeft = CheckRotationDir( pMoveCodeWork, GetDirToPlayer(pMoveCodeWork) );
						StartOneRotation(pMoveCodeWork, isLeft );
					}

					pWork->renzokuRotWait = 0;
					pWork->renzokuRotCount = 0;
					continueFlag = true;
				}
				else
				{
					pWork->renzokuRotWait = 5;
					// pWork->renzokuRotIsLeft = rotLeft;

					StartRotation(pMoveCodeWork, GetDirToPlayer(pMoveCodeWork), m_Setting.rotationFrame * (1.0f / pPokemonSettingData->speed));
					continueFlag = true;
				}
      }
    }
    else
    {
      SubWalkStopProcess( pMoveCodeWork );
    }


    // 停止中処理
    if (pCharaModelInstance->GetAnimationId() == GetRotationAfterMotionId(pMoveCodeWork))
    {
#if 0
			if (pWork->isRunUreshii)
			{
				StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RANK_B );
				pWork->isRunUreshii = false;
				break;
			}
#endif

      // ランダム待機アクション
      if ( pWork->randActionWaitCount == 0 )
      {
				static const s32 border = 2;

        // 親密度に応じたアイコン再生
        Field::Effect::Type effectType = Effect::EFFECT_TYPE_FUREAI_ICON_TEN1;
        switch (rank)
        {
				case PokemonFureai::FPF_RANK_UNKNOWN:
				{
					StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1, ICON_MODE_NORMAL, false );
					break;
				}
				case PokemonFureai::FPF_RANK_LIKE:
        {
					if (pWork->randActionCount >= border || pWork->isByeByeEnd )
					{
						s32 iconType = (System::GflUse::GetPublicRand() % 2 == 0) ? Effect::EFFECT_TYPE_FUREAI_ICON_TEN1 : Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1;
						StartIcon(pMoveCodeWork, iconType, ICON_MODE_NORMAL, false);
					}
					else
					{
					  StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RND, true, false );
					}

          break;
        }
        case PokemonFureai::FPF_RANK_LOVE:
        {
					if (pWork->randActionCount >= border || pWork->isByeByeEnd )
					{
						s32 iconType = (System::GflUse::GetPublicRand() % 2 == 0) ? Effect::EFFECT_TYPE_FUREAI_ICON_TEN1 : Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1;
						StartIcon(pMoveCodeWork, iconType, ICON_MODE_NORMAL, false);
					}
					else
					{
						StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RND, true, false );
					}
          break;
        }
        }
        pWork->isIconEndFlag = 1;

				CountResetRndIdle( pMoveCodeWork, false ); // 待ちフレームのみ更新

				pWork->randActionCount++;
				pWork->randActionCount = (pWork->randActionCount > 100) ? 100 : pWork->randActionCount;
				break;
      }
      else
      {
        pWork->randActionWaitCount--;
      }
    }
    else
    {
	    CountResetRndIdle(pMoveCodeWork);
    }

    // 走り中処理
    if( pCharaModelInstance->GetAnimationId() == P2_BASE_FI012_RUN01 )
    {
#if 0
			if (pWork->isFirstRun )
			{
				PokemonFureai::AddPoint(pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_RUN_01);    // ふれあいポイント加算
				PokemonFureai::PlayVoice(pMoveCodeWork->pModel, PokemonFureai::FPF_VOICE_PLAY_FIELD_RUN_01);
				pWork->isFirstRun = false;
			}
#endif

      if (pWork->runIconWaitCount == 0 )
      {
        pWork->runIconShowCount++;
        
        // 親密度に応じたアイコン再生
        Field::Effect::Type effectType = Effect::EFFECT_TYPE_FUREAI_ICON_TEN1;
        switch (rank)
        {
        case PokemonFureai::FPF_RANK_LIKE:
        case PokemonFureai::FPF_RANK_LOVE:
        {
          effectType = Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1;
          
          // 3回に1回違うやつ
          if (pWork->runIconShowCount == 3)
          {
            pWork->runIconShowCount = 0;
            effectType = Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1;
            PokemonFureai::AddPoint( pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_RUN_02 );    // ふれあいポイント加算
            PokemonFureai::PlayVoice(pMoveCodeWork->pModel, PokemonFureai::FPF_VOICE_PLAY_FIELD_RUN_02 );

						pWork->isRunUreshii = true;
          }
          else
          {
            PokemonFureai::AddPoint( pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_RUN_01 );    // ふれあいポイント加算
            PokemonFureai::PlayVoice(pMoveCodeWork->pModel, PokemonFureai::FPF_VOICE_PLAY_FIELD_RUN_01 );
          }

          break;
        }
        }
        PokemonFureai::CreateEffect( effectType, pMoveCodeWork->pModel, true );
				pWork->isFirstRun = false;

        f32 waitTable[5] = { 3.0f, 4.0f, 4.0f, 4.5f, 6.0f };
        pWork->runIconWaitCount = waitTable[ System::GflUse::GetPublicRand() % GFL_NELEMS( waitTable ) ] * 30;

      }
      else
      {
        pWork->runIconWaitCount--;
      }
    }
    else
    {
      pWork->runIconWaitCount = 30 * 3;
#if 0
			if (pWork->isFirstRun)
			{
        pWork->runIconWaitCount = 10;
			}
#endif

      pWork->runIconShowCount = 0;
    }

    isPlayerInterest = true;
    break;
  }

  // @note 未使用
  case SubState::LOST_ICON_WAIT:
  {
    isPlayerInterest = true;
    pWork->subStateCount += 1;
    if (pWork->subStateCount > 45 )
    {
      pWork->subStateCount = 0;
      pWork->subState = SubState::LOST;
      
      StartRotation(pMoveCodeWork, GetDirToPlayer(pMoveCodeWork) + 180, m_Setting.rotationFrame * ( 1.0f / pPokemonSettingData->speed ));
      continueFlag = true;
    }
    break;
  }

  // @note 未使用
  case SubState::END:
  {
    isPlayerInterest = true;
    b32 walk = false;
    
    // 範囲外になるまで停止
    if ( HitCheckReactionRangeToPlayer(pMoveCodeWork, m_Setting.walk.endResetDistance))
    {
      // 範囲内
      if (pWork->isMoveStart)
      {
        if ( isPlayerMove )
        {
          walk = true;
          pWork->moveKeepCount = m_Setting.moveKeepCount;
        }
        else
        {
          if (pWork->moveKeepCount > 0)
          {
            pWork->moveKeepCount--;
            walk = true;
          }
          else
          {
            pWork->moveWaitCount = m_Setting.moveWaitCount;
            pWork->isMoveStart = false;
          }
        }
      }
      else
      {
        if (isPlayerMove)
        {
          if (pWork->moveWaitCount > 0)
          {
            pWork->moveWaitCount--;
          }
          else
          {
            pWork->isMoveStart = true;
            pWork->moveKeepCount = m_Setting.moveKeepCount;
          }
        }
        else
        {
          pWork->moveWaitCount = m_Setting.moveWaitCount;
        }
      }

      if (walk)
      {
        f32 diffToPlayerDeg = GetDirDiffToPlayer(pMoveCodeWork);
        if (diffToPlayerDeg < m_Setting.rotRange )
        {
        }
        else
        {
          // 回転
          StartRotation(pMoveCodeWork, GetDirToPlayer(pMoveCodeWork), m_Setting.rotationFrame * ( 1.0f / pPokemonSettingData->speed ));
          continueFlag = true;
        }
        pWork->subStateCount = 0;
      }
    }
    else
    {
      pWork->subStateCount++;
      if (pWork->subStateCount >= 15 )
      {
        pWork->subStateCount = 0;
        pWork->subState = SubState::LOST;
      }
    }
    break;
  }
    // @note 未使用
  case SubState::KININARU:
  {
    isPlayerInterest = true;
    if (this->IsPlayerUreshiiAction(pMoveCodeWork))
    {
      StartUreshii(pMoveCodeWork);
      continueFlag = true;

      // 嬉しいが終わったらdiscoverへ
      pWork->subStateCount = 0;
      pWork->subState = SubState::DISCOVER;
      pWork->runIconWaitCount = 30 * 3;
      pWork->runIconShowCount = 0;
      pWork->isMoveStart = true;
      pWork->moveKeepCount = m_Setting.moveKeepCount;
    }
    else
    {
      // 一定時間後に戻る
      pWork->subStateCount += 1;
      if (pWork->subStateCount > 30)
      {
        pWork->subStateCount = 0;
        pWork->subState = SubState::DISCOVER;
        pWork->runIconWaitCount = 30 * 3;
        pWork->runIconShowCount = 0;
        pWork->isMoveStart = true;
        pWork->moveKeepCount = m_Setting.moveKeepCount;
      }
    }
    break;
  }
  }

  if ( isPlayerInterest )
  {
    gfl2::math::Vector3 interestPos( gfl2::math::Vector3::GetZero() );
    GetInterestPositionForPokemon( pMoveCodeWork, pFieldMap->GetPlayerCharacter(), &interestPos);

    // @fix NMcat[2384]:自機がNPCの真後ろに移動した際、NPCの自機に対する注目挙動が定まらない対処。背後に入った方向に注目座標を加工する処理を追加
    UpdateInterestPosition(pMoveCodeWork, interestPos);
  }
  else
  {
    FieldMoveModel* pParentNpc = NULL;
    if (isParentInterest)
    {
      pParentNpc = PokemonFureai::GetParentNpc(pMoveCodeWork->pModel);
    }

    if (pParentNpc && pParentNpc->GetVisible() ) // @fix GFMMcat[424]:対象が消えている場合は注目しない
    {
      // ペアとなる親を注目する
      gfl2::math::Vector3 interestPos(gfl2::math::Vector3::GetZero());
      GetInterestPosition(pParentNpc, &interestPos);
      UpdateInterestPosition(pMoveCodeWork, interestPos);
    }
    else
    {
      if (pMoveCodeWork->pModel->isEnableInterest())
      {
        DisableInterest(pMoveCodeWork);
      }
    }
  }

	if (pWork->renzokuRotWait > 0)
	{
		pWork->renzokuRotWait--;
	}
	else
	{
		pWork->renzokuRotCount = 0;
	}

  return continueFlag;
}


//----------------------------------------------------------------------------
/**
*  @brief  回転中処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubRotationProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  プレイヤー反応開始処理　
*
*  @param  pMoveCodeWork        動作コードワーク
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::CheckAndStartPlayerReaction(FieldMoveCodeWork* pMoveCodeWork) const
{
	// 共有ワークを取得
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

	// 共有ワークを取得
	Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
	const Field::PokemonFureai::POKEMON_SETTING_DATA* pPokemonSettingData = pFieldMoveModelActor->GetFureaiPokemonSettingData();
  
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

	// 親密度
	s32 rank = PokemonFureai::GetRank(pMoveCodeWork->pModel);

	// 主人公のアクションに反応する処理(リクエスト)
	switch (pWork->playerActionReq)
	{
    // 岩砕き
	case PokemonFureai::FPF_PLAYER_ACITON_IWA_KUDAKI:
	{
		StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1 );
		pWork->playerActionReq = PokemonFureai::FPF_PLAYER_ACITON_NONE;
		return true;
	}
    // ライドOFF
	case PokemonFureai::FPF_PLAYER_ACITON_RIDE_OFF:
	{
		if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
		{
			StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1);
		}
		else
		{
			StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RANK_A );
		}

		pWork->playerActionReq = PokemonFureai::FPF_PLAYER_ACITON_NONE;
		return true;
	}
    // ライドON
	case PokemonFureai::FPF_PLAYER_ACITON_RIDE_ON:
	{
		if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
		{
			StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_EXCLAMATION1);
		}
		else
		{
			StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RANK_B );
		}

		pWork->playerActionReq = PokemonFureai::FPF_PLAYER_ACITON_NONE;
		return true;
	}
	case PokemonFureai::FPF_PLAYER_ACITON_NORMAL:
	{
		if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
		{
			StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1);
		}
		else
		{
			StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RND );
		}

		pWork->playerActionReq = PokemonFureai::FPF_PLAYER_ACITON_NONE;
		return true;
	}

	case PokemonFureai::FPF_PLAYER_ACITON_NORMAL_Q:
	{
		if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
		{
			StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1);
		}
		else
		{
			StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1);
		}

		pWork->playerActionReq = PokemonFureai::FPF_PLAYER_ACITON_NONE;
		return true;
	}

	}

	// 主人公のアクションに反応する処理(うれしい)
  switch (FieldMoveCodePlayer::GetPlayerMove(pFieldMap->GetPlayerCharacter()->GetMoveCodeWork()))
  {
    // 回転
	case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_LEFT_LOOP:
	{
		if (pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->GetAnimationFrame() < 30)
		{
			if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
			{
				StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1);
			}
			else
			{
        StartOneRotation(pMoveCodeWork, true, true );
			}
			return true;
		}
		
		break;
	}
	case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_RIGHT_LOOP:
	{
		if (pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->GetAnimationFrame() < 30)
		{
			if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
			{
				StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1);
			}
			else
			{
				StartOneRotation(pMoveCodeWork, false, true );
			}
			return true;
		}
		
		break;
	}
	case FieldMoveCodePlayer::PLAYER_MOVE_TSUMADUKI:
	{
		if (pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->GetAnimationFrame() > 15)
		{
			if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
			{
				StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_TEN1);
			}
			else
			{
				StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1);
			}
			return true;
		}

		break;
	}
#if 0
  case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_LEFT_END:
  case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_RIGHT_END:
  {
    if (pFieldMap->GetPlayerCharacter()->GetCharaDrawInstance()->GetAnimationFrame() > 30)
    {
      return true;
    }
  }
#endif
  }
#if 0
	if (this->IsPlayerUreshiiAction(pMoveCodeWork))
	{
		if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
		{
			StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1);
		}
		else
		{
			StartOneRotation(pMoveCodeWork, true);
			// StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RND );
		}

		return true;
	}

	// 主人公のアクションに反応する処理(よくわからない)
	if (this->IsPlayerKininaruAction(pMoveCodeWork))
	{
		StartDefaultIdleMotion(pMoveCodeWork);
		StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1 );
		return true;
	}
#endif

	// ムーランドに反応する処理
	if (IsMuurandHeadNear(pMoveCodeWork))
	{
		if (pWork->isMuurandAciton == 0)
		{
			if (rank == PokemonFureai::FPF_RANK_UNKNOWN)
			{
				StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_QUESTION1);
			}
			else
			{
				StartIcon(pMoveCodeWork, Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1);
			}

			pWork->isMuurandAciton = 1;
			return true;
		}
	}
	else
	{
		pWork->isMuurandAciton = 0;
	}

	return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  嬉しい開始処理　
*
*  @param  pMoveCodeWork        動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::StartUreshii(FieldMoveCodeWork* pMoveCodeWork, s32 reqType, b32 isIcon, b32 isRandReset )
{
  // 共有ワークを取得
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
  const Field::PokemonFureai::POKEMON_SETTING_DATA* pPokemonSettingData = pFieldMoveModelActor->GetFureaiPokemonSettingData();
  
  // ランダム
  if ( reqType == PokemonFureai::FPF_YOROKOBI_REQ_RND)
  {
    reqType = ((System::GflUse::GetPublicRand() % 2) == 0) ? PokemonFureai::FPF_YOROKOBI_REQ_RANK_A : PokemonFureai::FPF_YOROKOBI_REQ_RANK_B;
  }

  // モーション
  s32 yorokobiType = PokemonFureai::FPF_YOROKOBI_TYPE_EMOTION_A;
	if (reqType == PokemonFureai::FPF_YOROKOBI_REQ_RANK_A || reqType == PokemonFureai::FPF_YOROKOBI_REQ_RANK_HEART )
  {
    yorokobiType = pPokemonSettingData->yorokobi1_type;
  }
  else
  {
    yorokobiType = pPokemonSettingData->yorokobi2_type;
  }

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();


  switch(yorokobiType)
  {
  case PokemonFureai::FPF_YOROKOBI_TYPE_EMOTION_A:
  {
    pCharaModelInstance->ChangeAnimation( PokemonFureai::FPF_POKEMON_MOTION_EMOTION_A );
    break;
  }
  case PokemonFureai::FPF_YOROKOBI_TYPE_EMOTION_B:
  {
    pCharaModelInstance->ChangeAnimation( PokemonFureai::FPF_POKEMON_MOTION_EMOTION_B );
    break;
  }
  case PokemonFureai::FPF_YOROKOBI_TYPE_TALK:
  {
    pCharaModelInstance->ChangeAnimation(PokemonFureai::FPF_POKEMON_MOTION_TALK_IN);
    break;
  }
  }

  pWork->state = State::URESHII;
  pWork->stateWork.yorokobi.type = reqType;
  pWork->stateWork.yorokobi.count = 0;
	pWork->stateWork.yorokobi.is_icon = isIcon;
	pWork->stateWork.yorokobi.is_end_icon = false;

  // 他のうれしいリクエストをリセット
	pWork->isRunUreshii = false;

  // ランダム待機リセット
	if (isRandReset)
	{
		CountResetRndIdle(pMoveCodeWork);
	}
#if 0
  // ふれあいランク
  s32 rank = PokemonFureai::GetRank(pMoveCodeWork->pModel);
       
  Field::Effect::Type effectType = Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1;
  switch (rank)
  {
  case PokemonFureai::FPF_RANK_LIKE:
  {
    effectType = Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1;
    PokemonFureai::AddPoint( pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_01);    // ふれあいポイント加算
    break;
  }
  case PokemonFureai::FPF_RANK_LOVE:
  {
    effectType = Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1;
    PokemonFureai::AddPoint( pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_01);    // ふれあいポイント加算
    break;
  }
  }
#endif
  // PokemonFureai::CreateEffect( effectType, pMoveCodeWork->pModel, true);
}

//----------------------------------------------------------------------------
/**
*  @brief  一回転開始処理　
*
*  @param  pMoveCodeWork        動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::StartOneRotation(FieldMoveCodeWork* pMoveCodeWork, b32 isLeft, b32 isPiyori ) const
{
	// 共有ワークを取得
	Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
	const Field::PokemonFureai::POKEMON_SETTING_DATA* pPokemonSettingData = pFieldMoveModelActor->GetFureaiPokemonSettingData();

	// 共有ワークを取得
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

	// キャラモデル描画用インスタンスを取得
	poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

	if (isLeft)
	{
		pCharaModelInstance->ChangeAnimation( P2_BASE_FI017_TURN01_LEFT);
	}
	else
	{
		pCharaModelInstance->ChangeAnimation( P2_BASE_FI016_TURN01_RIGHT );
	}


	pWork->state = State::ONE_ROTATION;
	pWork->stateWork.one_rotation.frame = 0;
	pWork->stateWork.one_rotation.frameMax = 16.0f * 4.0f * (1.0f / pPokemonSettingData->speed);
	pCharaModelInstance->SetAnimationStepFrame( 16.0f / (pWork->stateWork.one_rotation.frameMax - 1.0f) );
	pWork->stateWork.one_rotation.type = 0;
	pWork->stateWork.one_rotation.isLeft = isLeft;
	pWork->stateWork.one_rotation.state = 0;
	pWork->stateWork.one_rotation.isPiyori = isPiyori;

	gfl2::math::Quaternion startRotation = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
	pWork->stateWork.one_rotation.startRotationY = gfl2::math::ConvRadToDeg(NpcTool::ConvRotationQuaternionToYRadian(startRotation));
}

//----------------------------------------------------------------------------
/**
*  @brief  アイコン開始処理　
*
*  @param  pMoveCodeWork        動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::StartIcon(FieldMoveCodeWork* pMoveCodeWork, s32 iconType, s32 iconMode, b32 isRandReset )
{
	// 共有ワークを取得
	Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
	const Field::PokemonFureai::POKEMON_SETTING_DATA* pPokemonSettingData = pFieldMoveModelActor->GetFureaiPokemonSettingData();

	// 共有ワークを取得
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

	pWork->state = State::ICON;
	pWork->stateWork.icon.frame = 0;
	pWork->stateWork.icon.icon_type = iconType;
	pWork->stateWork.icon.iconMode = iconMode;
  
	// 停止
	SubWalkStopProcess( pMoveCodeWork );
	
  // ランダム待機リセット
	if (isRandReset)
	{
		CountResetRndIdle(pMoveCodeWork);
	}
}

//----------------------------------------------------------------------------
/**
*  @brief  寝ている開始処理　
*
*  @param  pMoveCodeWork        動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::StartSleep(FieldMoveCodeWork* pMoveCodeWork)
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  
  // 共有ワークを取得
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
  
  // ふれあいランク
  s32 rank = PokemonFureai::GetRank(pMoveCodeWork->pModel);
  
  // 設定取得
  Field::FieldResident* pFieldResident = pGameManager->GetFieldResident();
  Field::PokemonFureai::DataAccessor dataAccessor(pFieldResident->GetResource(BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN));
  const Field::PokemonFureai::POKEMON_SETTING_DATA* pData = dataAccessor.GetSettingData( pMoveCodeWork->pModel->GetCharacterId() );
  
  pWork->state = State::SLEEP;
  pWork->subStateCount = 0;


  // 目パチOFF
  Field::MoveModel::FieldFacialAnimeEye* pAnimeEye = pMoveCodeWork->pModel->GetFacialAnimeEye();
  pAnimeEye->SetAutoAnime(false);

  switch (pData->nemuri_type)
  {
  case PokemonFureai::FPF_NEMURI_TYPE_IDLE:
  {
    pAnimeEye->ChangeAnime( poke_3d::model::CharaModel::EYE_INDEX_CLOSE );
    pCharaModelInstance->SetAnimationStepFrame( 0.5f );
    break;
  }
  case PokemonFureai::FPF_NEMURI_TYPE_SUYASUYA:
  case PokemonFureai::FPF_NEMURI_TYPE_UTOUTO:
  {
    pCharaModelInstance->ChangeAnimation(PokemonFureai::FPF_POKEMON_MOTION_SLEEP);
    break;
  }
  }

	pFieldMoveModelActor->StartFrureaiSleep();

  // ZZZ
  PokemonFureai::CreateEffect(Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1, pMoveCodeWork->pModel, false, false, true );
  
  // @fix MMcat[209]:昼寝アイコンが画面奥の地形内に表示される、対処。ポケモンが自機から遠いorカリング中の際は[ZZZ]アイコンを非表示にする
  SubSleepIconVisibleProcess(pMoveCodeWork);
}



//----------------------------------------------------------------------------
/**
*  @brief  歩き/走り中処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubWalkToPlayerProcess(FieldMoveCodeWork* pMoveCodeWork ) const
{
  gfl2::math::Vector3 targetPos;
  GetMoveTargetPosition( pMoveCodeWork, &targetPos );
  return SubWalkCoreProcess( pMoveCodeWork, targetPos );
}

b32 FieldMoveCodeFureaiTypeA::SubWalkToTargetProcess(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget, b32 isRunEnable) const
{
	return SubWalkCoreProcess(pMoveCodeWork, rTarget, isRunEnable);
}

b32 FieldMoveCodeFureaiTypeA::SubWalkCoreProcess(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget, b32 isRunEnable) const
{
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  
  // ふれあいランク
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
  s32 rank = PokemonFureai::GetRank(pMoveCodeWork->pModel);
  const SETTING::CAME_NEAR* pCameNearSetting = &m_Setting.cameNear[ rank ];
  
  b32 continueFlag = false;

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // 設定取得
  Field::FieldResident* pFieldResident = pGameManager->GetFieldResident();
  Field::PokemonFureai::DataAccessor dataAccessor(pFieldResident->GetResource(BL_IDX_FIELD_RESIDENT__POKEMON_FUREAI_BIN));
  const Field::PokemonFureai::POKEMON_SETTING_DATA* pData = dataAccessor.GetSettingData( pMoveCodeWork->pModel->GetCharacterId() );

  // 移動禁止
  if (pData->move_mode == PokemonFureai::FPF_MOVE_MODE_STOP)
  {
    return false;
  }

  // 走るかどうか？
  b32 isRun = false;
  if ( rank > PokemonFureai::FPF_RANK_UNKNOWN )
  {
    isRun = true;
  }

  // 走り禁止
	if (pData->move_mode == PokemonFureai::FPF_MOVE_MODE_RUN_NG || isRunEnable == false )
  {
    isRun = false;
  }

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // 自機取得のため、グローバルアクセス
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  // 関連座標を取得
  gfl2::math::Vector3 playerPos(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  gfl2::math::Vector3 targetPos( rTarget );
  // GetMoveTargetPosition( pMoveCodeWork, &targetPos );
  gfl2::math::Vector3 npcPos = pMoveCodeWork->pModel->GetLocalSRT().translate;
  gfl2::math::Vector3 npcToPlayerVec = (playerPos - npcPos);
  gfl2::math::Vector3 npcToTargetVec = (targetPos - npcPos);

  // Y座標は無視
	npcToPlayerVec.y = 0.0f;
	npcToTargetVec.y = 0.0f;

  // 厚みを取得
  f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
  f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);
  f32 stopPlayerDistance = 10 + pCameNearSetting->baseDistance;
	f32 stopTargetDistance = playerBodyWidth + npcBodyWidth + 10; // @todo

  // 自機との距離を取得
	f32 distanceForPlayer = this->GetDistanceToPlayer( pMoveCodeWork );
	f32 distanceForTarget = npcToTargetVec.Length();

  // 自機との相対角度を取得
  f32 npcToPlayerRotationRad = NpcTool::ConvRotationVector3ToYRadian(npcToPlayerVec);
  f32 npcToTargetRotationRad = NpcTool::ConvRotationVector3ToYRadian(npcToTargetVec);
  gfl2::math::Quaternion npcRotation = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();

	if (distanceForPlayer <= stopPlayerDistance || distanceForTarget <= stopTargetDistance )
  {
    // 待機モーションに移行
    if (pCharaModelInstance->GetAnimationId() != GetRotationAfterMotionId(pMoveCodeWork))
    {
      pCharaModelInstance->ChangeAnimation(GetRotationAfterMotionId(pMoveCodeWork));
    }
    return continueFlag;
  }
	
  // 移動確定
	CountResetRndIdle(pMoveCodeWork);
  
  // ターゲット姿勢にむけて補完する
  gfl2::math::Quaternion qua = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  gfl2::math::Quaternion targetQua;
  // qua.RadianYXZToQuaternion( gfl2::math::ConvDegToRad( 0.0f ), gfl2::math::ConvDegToRad( pWork->rotationWork.startRotationY ), gfl2::math::ConvDegToRad( 0.0f ) );
  targetQua.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(0.0f), npcToTargetRotationRad, gfl2::math::ConvDegToRad(0.0f));

  qua.Slerp(targetQua, 0.2f * pData->speed );
  pCharaModelInstance->SetRotationQuat(qua);

  switch (pCharaModelInstance->GetAnimationId())
  {
  case P2_BASE_FI010_WALK01:
  {
    // 走り始める
		if (isRun && distanceForPlayer >= m_Setting.walk.walkToRunDistance)
    {
			// PokemonFureai::PlayVoice(pMoveCodeWork->pModel, PokemonFureai::FPF_VOICE_PLAY_FIELD_RUN_01 );

      pCharaModelInstance->ChangeAnimationSmooth(P2_BASE_FI012_RUN01, 4);
      // pFieldMap->GetPlayerCharacter()->DisableInterest(m_Setting.disableInterestFrame);
    }
    else
    {
      // 主人公をこっちに注目させる
      // GetInterestPosition(pMoveCodeWork->pModel, &interestPos);
      // pFieldMap->GetPlayerCharacter()->EnableInterest(interestPos);
    }

    break;
  }

  case P2_BASE_FI012_RUN01:
  {
    // 歩き始める
		if (distanceForPlayer <= m_Setting.walk.runToWalkDistance + pCameNearSetting->baseDistance )
    {
      pCharaModelInstance->ChangeAnimationSmooth(P2_BASE_FI010_WALK01, 4);
    }
    break;
  }

  default:
  {
    // 走り始める
		if (isRun && distanceForPlayer >= m_Setting.walk.walkToRunDistance)
    {
      pCharaModelInstance->ChangeAnimationSmooth(P2_BASE_FI012_RUN01, 4);
    }
    // 歩き始める
    else
    {
      pCharaModelInstance->ChangeAnimationSmooth(P2_BASE_FI010_WALK01, 4);
    }
    break;
  }

  }

  return continueFlag;
}

//----------------------------------------------------------------------------
/**
*  @brief  嬉しい処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubUreshiiProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
  const Field::PokemonFureai::POKEMON_SETTING_DATA* pPokemonSettingData = pFieldMoveModelActor->GetFureaiPokemonSettingData();

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  s32 reqType = pWork->stateWork.yorokobi.type;

  // モーション
  s32 yorokobiType = PokemonFureai::FPF_YOROKOBI_TYPE_EMOTION_A;
  if (reqType == PokemonFureai::FPF_YOROKOBI_REQ_RANK_A)
  {
    yorokobiType = pPokemonSettingData->yorokobi1_type;
  }
  else
  {
    yorokobiType = pPokemonSettingData->yorokobi2_type;
  }

  // 声
  s32 voiceType = (reqType == PokemonFureai::FPF_YOROKOBI_REQ_RANK_A) ? PokemonFureai::FPF_VOICE_TYPE_YOROKOBI : PokemonFureai::FPF_VOICE_TYPE_NORMAL;
  
  // アイコン
  Field::Effect::Type iconType =  (reqType == PokemonFureai::FPF_YOROKOBI_REQ_RANK_A) ? Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1 : Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1;
	if (reqType == PokemonFureai::FPF_YOROKOBI_REQ_RANK_HEART)
	{
		iconType = Effect::EFFECT_TYPE_FUREAI_ICON_HEART1;
	}

  switch (yorokobiType)
  {
  case PokemonFureai::FPF_YOROKOBI_TYPE_EMOTION_A:
  case PokemonFureai::FPF_YOROKOBI_TYPE_EMOTION_B:
  {
    if (pWork->stateWork.yorokobi.count == 0)
    {
      PokemonFureai::PlayVoice(pMoveCodeWork->pModel, voiceType);
    }
    
		if (pWork->stateWork.yorokobi.count == 15 && pWork->stateWork.yorokobi.is_icon )
    {
      PokemonFureai::CreateEffect(iconType, pMoveCodeWork->pModel, true);
			PokemonFureai::AddPointByFieldIcon(pMoveCodeWork->pModel, iconType);  // ポイント加算
			pWork->stateWork.yorokobi.is_end_icon = true;
    }

    if (pCharaModelInstance->GetAnimationEndFrame() <= 0.0f || pCharaModelInstance->IsAnimationLastFrame())
    {
      // 確実にアイコンを出す
			if (pWork->stateWork.yorokobi.is_icon && pWork->stateWork.yorokobi.is_end_icon == false )
			{
				PokemonFureai::CreateEffect(iconType, pMoveCodeWork->pModel, true);
				PokemonFureai::AddPointByFieldIcon(pMoveCodeWork->pModel, iconType);  // ポイント加算
			}

      pCharaModelInstance->ChangeAnimation(pMoveCodeWork->pModel->GetDefaultIdleAnimationId());
      pWork->state = State::IDLE;
      return true;
    }

    break;
  }
  case PokemonFureai::FPF_YOROKOBI_TYPE_TALK:
  {
    if (pCharaModelInstance->GetAnimationEndFrame() <= 0.0f)
    {
      pCharaModelInstance->ChangeAnimation(pMoveCodeWork->pModel->GetDefaultIdleAnimationId());
      pWork->state = State::IDLE;
      return true;
    }

    if (pCharaModelInstance->IsAnimationLastFrame())
    {
      switch (pCharaModelInstance->GetAnimationId())
      {
      case PokemonFureai::FPF_POKEMON_MOTION_TALK_IN:
      {
        PokemonFureai::PlayVoice(pMoveCodeWork->pModel, voiceType);
				if (pWork->stateWork.yorokobi.is_icon)
				{
					PokemonFureai::CreateEffect(iconType, pMoveCodeWork->pModel, true);
			    PokemonFureai::AddPointByFieldIcon(pMoveCodeWork->pModel, iconType);  // ポイント加算
				}
        pCharaModelInstance->ChangeAnimation(PokemonFureai::FPF_POKEMON_MOTION_TALK);
        break;
      }
      case PokemonFureai::FPF_POKEMON_MOTION_TALK:
      {
        pCharaModelInstance->ChangeAnimation(PokemonFureai::FPF_POKEMON_MOTION_TALK_OUT);
        break;
      }
      case PokemonFureai::FPF_POKEMON_MOTION_TALK_OUT:
      default:
      {
        pCharaModelInstance->ChangeAnimation(pMoveCodeWork->pModel->GetDefaultIdleAnimationId());
        pWork->state = State::IDLE;
        return true;
      }
      }
    }
    break;
  }
  }

  pWork->stateWork.yorokobi.count++;
  return false;
}

//----------------------------------------------------------------------------
/**
*  @brief  寝ている処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubSleepProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  
  // @fix MMcat[209]:昼寝アイコンが画面奥の地形内に表示される、対処。ポケモンが自機から遠いorカリング中の際は[ZZZ]アイコンを非表示にする
  SubSleepIconVisibleProcess(pMoveCodeWork);


  if (pMoveCodeWork->pModel->IsEnableCulilng())
  {

  }
  else
  {
    if (pWork->subStateCount > 60)
    {
      // PokemonFureai::CreateEffect(Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1, pMoveCodeWork->pModel, true);
      pWork->subStateCount = 0;
    }
    else
    {
      pWork->subStateCount++;
    }
  }

  return false;
}

b32 FieldMoveCodeFureaiTypeA::SubSleepIconVisibleProcess(FieldMoveCodeWork* pMoveCodeWork)
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

  // ZZ
  Effect::EffectManager* const pEffectManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetFieldmap()->GetEffectManager();
  Effect::IEffectBase* pEffect = PokemonFureai::GetEffect(pMoveCodeWork->pModel);
  if (pEffect == NULL || pEffect->GetType() != Effect::EFFECT_TYPE_FUREAI_ICON_ZZ1)
  {
    return false;
  }
  Field::Effect::EffectFureaiIconBase *pIconEffct = reinterpret_cast<Field::Effect::EffectFureaiIconBase*>(pEffect);

  if (pMoveCodeWork->pModel->IsEnableCulilng())
  {
    pIconEffct->SetVisible(false);
  }
  else
  {
    // 遠かったら「…」を非表示
    if (HitCheckReactionRangeToPlayer(pMoveCodeWork, m_Setting.sleepIconDispDistance ))
    {
      pIconEffct->SetVisible(true);
      return true;
    }
    else
    {
      pIconEffct->SetVisible(false);

    }
  }
  return false;
}


//----------------------------------------------------------------------------
/**
*  @brief  回転開始処理　
*
*  @param  pMoveCodeWork        動作コードワーク
*/
//-----------------------------------------------------------------------------
void FieldMoveCodeFureaiTypeA::StartRotation(FieldMoveCodeWork* pMoveCodeWork, f32 targetRotationDegY, u32 frame, b32 isReturn, b32 isCompressMotion, s32 nextState ) const
{
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
	FieldMoveCodeNpc::StartRotation(pMoveCodeWork, targetRotationDegY, frame, isReturn, isCompressMotion, nextState );
	CountResetRndIdle( pMoveCodeWork );
}

//----------------------------------------------------------------------------
/**
*  @brief  一回転処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubOneRotationProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
	// 共有ワークを取得
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
	Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
	const Field::PokemonFureai::POKEMON_SETTING_DATA* pPokemonSettingData = pFieldMoveModelActor->GetFureaiPokemonSettingData();

	// キャラモデル描画用インスタンスを取得
	poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

	b32 continueFlag = false;

	f32 rotDir = 1.0f;
	if (!pWork->stateWork.one_rotation.isLeft )
	{
		rotDir = -1.0f;
	}

	switch (pWork->stateWork.one_rotation.state)
	{
		// ターゲット姿勢にむけて補完する
	case 0:
		{
			gfl2::math::Quaternion qua;
      
			qua.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(0.0f), gfl2::math::ConvDegToRad(pWork->stateWork.one_rotation.startRotationY), gfl2::math::ConvDegToRad(0.0f));
			
			// こっそり主人公補間
#if 0
			{
			  f32 playerDir = this->GetDirToPlayer( pMoveCodeWork );
			  gfl2::math::Quaternion playerDirQua;
				playerDirQua.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(0.0f), gfl2::math::ConvDegToRad(playerDir), gfl2::math::ConvDegToRad(0.0f));
			  qua.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(0.0f), gfl2::math::ConvDegToRad(pWork->stateWork.one_rotation.startRotationY), gfl2::math::ConvDegToRad(0.0f));
				qua.Slerp(playerDirQua, 0.05f * pPokemonSettingData->speed);
				pWork->stateWork.one_rotation.startRotationY = gfl2::math::ConvRadToDeg(NpcTool::ConvRotationQuaternionToYRadian(qua));
			}
#endif

			f32 value = static_cast<f32>(pWork->stateWork.one_rotation.frame) / static_cast<f32>(pWork->stateWork.one_rotation.frameMax);
			f32 t = gfl2::math::Easing::GetValue(gfl2::math::Easing::INOUT_SINE, 0.0f, 1.0f, value);
			//f32 t = gfl2::math::Easing::GetValue(gfl2::math::Easing::EaseFunc::OUT_QUART, 0.0f, 1.0f, value );
			qua.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(0.0f), gfl2::math::ConvDegToRad(pWork->stateWork.one_rotation.startRotationY + 360 * 2.0f * t * rotDir), gfl2::math::ConvDegToRad(0.0f));

			// アイコン再生
			if (pWork->stateWork.one_rotation.frame == static_cast<s32>( ( 16.0f * (1.0f / pPokemonSettingData->speed ) )))
			{
				// 鳴き声
				PokemonFureai::PlayVoice(pMoveCodeWork->pModel, PokemonFureai::FPF_VOICE_PLAY_FIELD_KURUKURU);
				
				PokemonFureai::CreateEffect(Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1, pMoveCodeWork->pModel, true);
				PokemonFureai::AddPoint(pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_KURUKURU_01);    // ふれあいポイント加算
			}
			if (pWork->stateWork.one_rotation.frame == static_cast<s32>((46.0f * (1.0f / pPokemonSettingData->speed))))
			{
				//PokemonFureai::CreateEffect(Effect::EFFECT_TYPE_FUREAI_ICON_NOTE1, pMoveCodeWork->pModel, true);
				//PokemonFureai::AddPoint(pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_KURUKURU_01);    // ふれあいポイント加算
			}

			// アイコン再生
			if (pWork->stateWork.one_rotation.frame == pWork->stateWork.one_rotation.frameMax - 8.0f)
			{
				// PokemonFureai::CreateEffect(Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1, pMoveCodeWork->pModel, true);
				// PokemonFureai::AddPoint(pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_KURUKURU_02);    // ふれあいポイント加算
			}

			pCharaModelInstance->SetRotationQuat(qua);

			// 指定フレームたっていなければ継続してアクションコマンドを実行する
			pWork->stateWork.one_rotation.frame++;
			if (pWork->stateWork.one_rotation.frame <= (f32)pWork->stateWork.one_rotation.frameMax)
			{
				return continueFlag;
			}

			// 念の為にターゲット姿勢にあわせる
			gfl2::math::Quaternion start_qua;
			start_qua.RadianYXZToQuaternion(gfl2::math::ConvDegToRad(0.0f), gfl2::math::ConvDegToRad(pWork->rotationWork.startRotationY), gfl2::math::ConvDegToRad(0.0f));
			pCharaModelInstance->SetRotationQuat(qua);

			// 待機モーションに移行
			// pWork->state = pWork->nextState;
			// pWork->nextState = 0;


			pCharaModelInstance->ChangeAnimation(pMoveCodeWork->pModel->GetDefaultIdleAnimationId());
			pWork->stateWork.one_rotation.state = 1;
			pWork->stateWork.one_rotation.frame = 0;



			// ピヨリで開始して、ピヨリループになっていなければ、うれしいを再生
			b32 isUreshii = false;
			if (pWork->stateWork.one_rotation.isPiyori)
			{
				GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
				Fieldmap* pFieldMap = pGameManager->GetFieldmap();

				switch (FieldMoveCodePlayer::GetPlayerMove(pFieldMap->GetPlayerCharacter()->GetMoveCodeWork()))
				{
				case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_LEFT_LOOP:
				case FieldMoveCodePlayer::PLAYER_MOVE_PIYORI_RIGHT_LOOP:
				{
					break;
				}
				default:
				{
					StartUreshii(pMoveCodeWork, PokemonFureai::FPF_YOROKOBI_REQ_RANK_A);
					continueFlag = true;
					isUreshii = true;
				}
				}
			}

			if (isUreshii == false)
			{
				PokemonFureai::CreateEffect(Effect::EFFECT_TYPE_FUREAI_ICON_SMILE1, pMoveCodeWork->pModel, true);
				PokemonFureai::AddPoint(pMoveCodeWork->pModel, PokemonFureai::FPF_ADD_POINT_FIELD_KURUKURU_02);    // ふれあいポイント加算
			}
			break;
		}

		// ちょっと待つ
		case 1:
		{
			pWork->stateWork.one_rotation.frame += 1;
			if (pWork->stateWork.one_rotation.frame <= 30 )
			{
				return continueFlag;
			}

			pWork->state = State::IDLE;
			break;
		}
	}

	return continueFlag;
}

//----------------------------------------------------------------------------
/**
*  @brief  アイコン処理　
*
*  @param  pMoveCodeWork  動作コードワーク
*
*  @retval true 今フレームにもう一度処理を実行する
*/
//-----------------------------------------------------------------------------
b32 FieldMoveCodeFureaiTypeA::SubIconProcess(FieldMoveCodeWork* pMoveCodeWork) const
{
	// 共有ワークを取得
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

	// キャラモデル描画用インスタンスを取得
	poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();

	switch (pWork->stateWork.icon.iconMode)
	{
	case ICON_MODE_FAST:
	{
		u32 iconCreateFrame = 1;
		pWork->stateWork.icon.frame += 1;
		if (pWork->stateWork.icon.frame == iconCreateFrame)
		{
			PokemonFureai::CreateEffect(Effect::Type(pWork->stateWork.icon.icon_type), pMoveCodeWork->pModel, true, true );
			PokemonFureai::AddPointByFieldIcon(pMoveCodeWork->pModel, pWork->stateWork.icon.icon_type);  // ポイント加算
		}

		if (pWork->stateWork.icon.frame > iconCreateFrame)
		{
			// アイコンの途中で切り替える
			switch (pWork->subState)
			{
			case SubState::DISCOVER:
			{
				// 主人公反応処理
				if (CheckAndStartPlayerReaction(pMoveCodeWork))
				{
					return false;
				}
			}
			}

			if (PokemonFureai::HaveEffect(pMoveCodeWork->pModel) == false)
			{
				pWork->state = State::IDLE;
				return true;
			}
		}

		break;
	}
	case ICON_MODE_NORMAL:
	{
		u32 iconCreateFrame = 15;
		pWork->stateWork.icon.frame += 1;
		if (pWork->stateWork.icon.frame == iconCreateFrame)
		{
			PokemonFureai::CreateEffect(Effect::Type(pWork->stateWork.icon.icon_type), pMoveCodeWork->pModel, true, true );
			PokemonFureai::AddPointByFieldIcon(pMoveCodeWork->pModel, pWork->stateWork.icon.icon_type);  // ポイント加算
		}

		if (pWork->stateWork.icon.frame > iconCreateFrame)
		{
			// アイコンの途中で切り替える
			switch (pWork->subState)
			{
			case SubState::DISCOVER:
			{
				// 主人公反応処理
				if (CheckAndStartPlayerReaction(pMoveCodeWork))
				{
					return false;
				}
			}
			}

			if (PokemonFureai::HaveEffect(pMoveCodeWork->pModel) == false)
			{
				pWork->state = State::IDLE;
				return true;
			}
		}

		break;
	}

	case ICON_MODE_VERY_FAST:
	{
		u32 iconCreateFrame = 1;
		u32 iconEndFrame = 10;
		pWork->stateWork.icon.frame += 1;
		if (pWork->stateWork.icon.frame == iconCreateFrame)
		{
			PokemonFureai::CreateEffect(Effect::Type(pWork->stateWork.icon.icon_type), pMoveCodeWork->pModel, true, true );
			PokemonFureai::AddPointByFieldIcon(pMoveCodeWork->pModel, pWork->stateWork.icon.icon_type);  // ポイント加算
		}

		if (pWork->stateWork.icon.frame > iconCreateFrame)
		{
			// アイコンの途中で切り替える
			switch (pWork->subState)
			{
			case SubState::DISCOVER:
			{
				// 主人公反応処理
				if (CheckAndStartPlayerReaction(pMoveCodeWork))
				{
					return false;
				}
			}
			}

			if (pWork->stateWork.icon.frame >= iconEndFrame)
			{
				pWork->state = State::IDLE;
				return true;
			}
		}
  }
	}

	return false;
}

b32 FieldMoveCodeFureaiTypeA::SubWalkStopProcess(FieldMoveCodeWork* pMoveCodeWork)
{
  // キャラモデル描画用インスタンスを取得
  poke_3d::model::CharaModel* pCharaModelInstance = pMoveCodeWork->pModel->GetCharaDrawInstance();
  if (pCharaModelInstance->GetAnimationId() != GetRotationAfterMotionId(pMoveCodeWork))
  {
    pCharaModelInstance->ChangeAnimation(GetRotationAfterMotionId(pMoveCodeWork));
  }

  return true;
}

b32 FieldMoveCodeFureaiTypeA::GetMoveTargetPosition(FieldMoveCodeWork* pMoveCodeWork, gfl2::math::Vector3* pOut )
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
  gfl2::math::Vector3 pos_player(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);
  
#if 0
    // 厚みを取得
  f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
  f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);

  gfl2::math::Vector3 pos_player(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);

  pos_player.y = pos_mine.y = 0.0f;
  f32 length = (pos_player - pos_mine).Length();
  *pOut = pos_mine + (pos_player - pos_mine) * (length - playerBodyWidth - npcBodyWidth - 0.5f ) / length;
#endif

  f32 mineBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);
  f32 width = COL_GETA + mineBodyWidth;

  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
  FieldCollisionAccessor::CollisionData col;
  pFieldMoveModelActor->GetFreaiMoveAreaData(&col);
  
  pos_player.y = 0.0f;

  switch (col.type)
  {
  case FieldCollisionAccessor::CYLINDER:
  {
    gfl2::math::Vector3 pos(col.cylinder.center[0], col.cylinder.center[1], col.cylinder.center[2]);
    pos.y = 0.0f;

    if ((pos_player - pos).LengthSq() > col.cylinder.radius * col.cylinder.radius)
    {
      pos_player = pos + (pos_player - pos).Normalize() * col.cylinder.radius;
    }
    break;
  }
  case FieldCollisionAccessor::BOX:
  {
    gfl2::math::Vector3 pos(col.box.center[0], col.box.center[1], col.box.center[2]);
    gfl2::math::Quaternion q( col.box.quaternion[0], col.box.quaternion[1], col.box.quaternion[2], col.box.quaternion[3] );
    pos.y = 0.0f;

    // 頂点情報生成
    gfl2::math::Vector3 vertex[4];
    vertex[0] = gfl2::math::Vector3( -col.box.scale[ 0 ] + width, 0.0f, -col.box.scale[ 2 ] + width);
    vertex[1] = gfl2::math::Vector3( col.box.scale[ 0 ] - width, 0.0f, -col.box.scale[ 2 ] + width);
    vertex[2] = gfl2::math::Vector3( col.box.scale[ 0 ] - width, 0.0f, col.box.scale[ 2 ] - width);
    vertex[3] = gfl2::math::Vector3( -col.box.scale[ 0 ] + width, 0.0f, col.box.scale[ 2 ] - width);

    for (u32 i = 0; i < 4; i++)
    {
      vertex[i] = q.Transform(vertex[i]) + pos;
    }

    for (u32 i = 0; i < 4; i++)
    {
      u32 next = i + 1;
      if (next > 3)
      {
        next = 0;
      }


      gfl2::math::Vector2 vec1(vertex[next].x - vertex[i].x, vertex[next].z - vertex[i].z );
      gfl2::math::Vector2 vec2(pos_player.x - vertex[i].x, pos_player.z - vertex[i].z );

      f32 work = vec1.Cross(vec2);
      if (work < 0.0f)
      {
        // 射影
        f32 value = (vec1.Dot(vec2)) / (vec1.Length() * vec1.Length());
        gfl2::math::Vector2 vecWork(0.0f, 0.0f);
        vecWork.x = vec1.x * value;
        vecWork.y = vec1.y * value;

        pos_player.x = vertex[i].x + vecWork.x;
        pos_player.z = vertex[i].z + vecWork.y;
      }
    }
    break;
  }
  }

  *pOut = pos_player;
  return true;
}

b32 FieldMoveCodeFureaiTypeA::HitCheckReactionRangeToPlayer(FieldMoveCodeWork* pMoveCodeWork, f32 range )
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  // 厚みを取得
  f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
  f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);

  gfl2::math::Vector3 pos_player(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);

  f32 work = range + playerBodyWidth + npcBodyWidth;
  f32 distance = (pos_player - pos_mine).Length();
  if ((pos_player - pos_mine).LengthSq() <= (work * work))
  {
    return true;
  }

  return false;
}

f32 FieldMoveCodeFureaiTypeA::GetDirDiffToPlayer(FieldMoveCodeWork* pMoveCodeWork)
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  gfl2::math::Vector3 pos_player(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  return GetDirDiffToTarget( pMoveCodeWork, pos_player );
}

f32 FieldMoveCodeFureaiTypeA::GetDirToPlayer(FieldMoveCodeWork* pMoveCodeWork)
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  gfl2::math::Vector3 pos_player(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  return GetDirToTarget( pMoveCodeWork, pos_player );
}

f32 FieldMoveCodeFureaiTypeA::GetDistanceToPlayer(FieldMoveCodeWork* pMoveCodeWork)
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();

  // 関連座標を取得
  gfl2::math::Vector3 playerPos(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
  gfl2::math::Vector3 npcPos = pMoveCodeWork->pModel->GetLocalSRT().translate;
  gfl2::math::Vector3 npcToPlayerVec = (playerPos - npcPos);

  // 厚みを取得
  f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
  f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);

  // 自機との距離を取得
  return npcToPlayerVec.Length() - playerBodyWidth - npcBodyWidth;
}

f32 FieldMoveCodeFureaiTypeA::GetDirDiffToTarget(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget)
{
  gfl2::math::Vector3 pos_player( rTarget );
  gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);
  gfl2::math::Vector3 npcToPlayerVec = (pos_player - pos_mine);

  // 自機との距離を取得
  f32 distanceSqForPlayer(npcToPlayerVec.LengthSq());

  // @fix GFMcat[2273]:自機とNPCが重なっていた際に無限ループになる、。相対向きのロジックの結果がNANになるので、ありえないぐらい近い場合は処理を打ち切る。
  gfl2::math::Vector3 npcToPlayerVecXZ = npcToPlayerVec;
  npcToPlayerVecXZ.y = 0.0f;

  if (npcToPlayerVecXZ.LengthSq() <= (NpcTool::EFFECTIVE_DISTANCE * NpcTool::EFFECTIVE_DISTANCE))
  {
    return 0.0f;
  }

  // 自機との相対角度を取得
  f32 npcToPlayerRotationRad = NpcTool::ConvRotationVector3ToYRadian(npcToPlayerVec);
  gfl2::math::Quaternion npcRotation = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetRotationQuat();
  f32 npcNowRotationRad = NpcTool::ConvRotationQuaternionToYRadian(npcRotation);
  f32 rotationDiffDegForPlayer = gfl2::math::FAbs(gfl2::math::ConvRadToDeg(NpcTool::DiffAngle(npcToPlayerRotationRad, npcNowRotationRad)));

  return rotationDiffDegForPlayer;
}

f32 FieldMoveCodeFureaiTypeA::GetDirToTarget(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rTarget)
{
  gfl2::math::Vector3 pos_player(rTarget);
  gfl2::math::Vector3 pos_mine(pMoveCodeWork->pModel->GetLocalSRT().translate);
  gfl2::math::Vector3 npcToPlayerVec = (pos_player - pos_mine);
  // 自機との相対角度を取得
  f32 npcToPlayerRotationRad = NpcTool::ConvRotationVector3ToYRadian(npcToPlayerVec);

  return gfl2::math::ConvRadToDeg(npcToPlayerRotationRad);
}

void FieldMoveCodeFureaiTypeA::PositionClampMoveArea(FieldMoveCodeWork* pMoveCodeWork)
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
  FieldCollisionAccessor::CollisionData col;
  pFieldMoveModelActor->GetFreaiMoveAreaData(&col);


  gfl2::math::Vector3 pos_work = pMoveCodeWork->pModel->GetCharaDrawInstance()->GetPosition();

  f32 mineBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);
  f32 width = COL_GETA + mineBodyWidth;

  switch (col.type)
  {
  case FieldCollisionAccessor::BOX:
  {
    gfl2::math::Vector3 pos(col.box.center[0], col.box.center[1], col.box.center[2]);
    gfl2::math::Quaternion q( col.box.quaternion[0], col.box.quaternion[1], col.box.quaternion[2], col.box.quaternion[3] );
    // pos.y = 0.0f;

    // 頂点情報生成
    gfl2::math::Vector3 vertex[4];
    vertex[0] = gfl2::math::Vector3( -col.box.scale[ 0 ] + width, 0.0f, -col.box.scale[ 2 ] + width);
    vertex[1] = gfl2::math::Vector3( col.box.scale[ 0 ] - width, 0.0f, -col.box.scale[ 2 ] + width);
    vertex[2] = gfl2::math::Vector3( col.box.scale[ 0 ] - width, 0.0f, col.box.scale[ 2 ] - width);
    vertex[3] = gfl2::math::Vector3( -col.box.scale[ 0 ] + width, 0.0f, col.box.scale[ 2 ] - width);

    for (u32 i = 0; i < 4; i++)
    {
      vertex[i] = q.Transform(vertex[i]) + pos;
    }

    for (u32 i = 0; i < 4; i++)
    {
      u32 next = i + 1;
      if (next > 3)
      {
        next = 0;
      }

      gfl2::math::Vector2 vec1(vertex[next].x - vertex[i].x, vertex[next].z - vertex[i].z );
      gfl2::math::Vector2 vec2(pos_work.x - vertex[i].x, pos_work.z - vertex[i].z );

      f32 work = vec1.Cross(vec2);
      if (work < 0.0f)
      {
        // 射影
        f32 value = (vec1.Dot(vec2)) / (vec1.Length() * vec1.Length());
        gfl2::math::Vector2 vecWork(0.0f, 0.0f);
        vecWork.x = vec1.x * value;
        vecWork.y = vec1.y * value;

        pos_work.x = vertex[i].x + vecWork.x;
        pos_work.z = vertex[i].z + vecWork.y;
      }
    }

    break;
  }
  }

  pMoveCodeWork->pModel->GetCharaDrawInstance()->SetPosition( pos_work );
}

void FieldMoveCodeFureaiTypeA::PositionClampMoveAreaForComeNear(FieldMoveCodeWork* pMoveCodeWork, const gfl2::math::Vector3& rStart, const gfl2::math::Vector3& rEnd, gfl2::math::Vector3* pOut )
{
  // 共有ワークを取得
  Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
  Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
  FieldCollisionAccessor::CollisionData col;
  pFieldMoveModelActor->GetFreaiMoveAreaData(&col);
  
	f32 mineBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);
  f32 width = COL_GETA + mineBodyWidth;

  gfl2::math::Vector3 result = rEnd;

  switch (col.type)
  {
  case FieldCollisionAccessor::BOX:
  {
    gfl2::math::Vector3 pos(col.box.center[0], col.box.center[1], col.box.center[2]);
    gfl2::math::Quaternion q(col.box.quaternion[0], col.box.quaternion[1], col.box.quaternion[2], col.box.quaternion[3]);
    pos.y = 0.0f;


    gfl2::math::Vector3 vertex[4];
    gfl2::math::Vector2 vertex2[4];
		vertex[0] = gfl2::math::Vector3(-col.box.scale[0] + width, 0.0f, -col.box.scale[2] + width);
		vertex[1] = gfl2::math::Vector3(col.box.scale[0] - width, 0.0f, -col.box.scale[2] + width);
		vertex[2] = gfl2::math::Vector3(col.box.scale[0] - width, 0.0f, col.box.scale[2] - width);
		vertex[3] = gfl2::math::Vector3(-col.box.scale[0] + width, 0.0f, col.box.scale[2] - width);

    for (u32 i = 0; i < 4; i++)
    {
      vertex[i] = q.Transform(vertex[i]) + pos;
      vertex2[i].x = vertex[i].x;
      vertex2[i].y = vertex[i].z;
    }

    for (u32 i = 0; i < 4; i++)
    {
      u32 next = i + 1;
      if (next > 3)
      {
        next = 0;
      }


      gfl2::math::Vector2 vec1(vertex[next].x - vertex[i].x, vertex[next].z - vertex[i].z);
      gfl2::math::Vector2 vec2(result.x - vertex[i].x, result.z - vertex[i].z);

      f32 work = vec1.Cross(vec2);
      if (work < 0.0f)
      {
        gfl2::math::Vector2 pos_mine_2;
        gfl2::math::Vector2 pos_player_2;
        pos_mine_2.x = result.x;
        pos_mine_2.y = result.z;
        pos_player_2.x = rStart.x;
        pos_player_2.y = rStart.z;

        gfl2::math::Vector2 col;
        if (NpcTool::ColSegments(vertex2[i], vertex2[next], pos_mine_2, pos_player_2, &col))
        {
          result.x = col.x;
          result.z = col.y;
        }
        else
        {
          // 実質直線上
          result = rStart;
        }
      }
    }
  }
  }

  *pOut = result;
}

void FieldMoveCodeFureaiTypeA::PlayerPush(FieldMoveCodeWork* pMoveCodeWork)
{
  // 自機取得のため、グローバルアクセス
  GameSys::GameManager* pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
  Fieldmap* pFieldMap = pGameManager->GetFieldmap();
	
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));
	Field::FieldMoveModelActor* pFieldMoveModelActor = static_cast<Field::FieldMoveModelActor*>(pMoveCodeWork->pModel->GetActorUserWork());
	FieldCollisionAccessor::CollisionData col;
	pFieldMoveModelActor->GetFreaiMoveAreaData(&col);

	// 関連座標を取得
	gfl2::math::Vector3 playerPos(pFieldMap->GetPlayerCharacter()->GetLocalSRT().translate);
	gfl2::math::Vector3 npcPos = pMoveCodeWork->pModel->GetLocalSRT().translate;
	playerPos.y = npcPos.y;
	gfl2::math::Vector3 npcToPlayerVec = (playerPos - npcPos);
	gfl2::math::Vector3 playerToNpcVec = (npcPos - playerPos);

	// 厚みを取得
	f32 playerBodyWidth = GetBodyWidth(pFieldMap->GetPlayerCharacter(), true);
	f32 npcBodyWidth = GetBodyWidth(pMoveCodeWork->pModel, false);

	if (npcToPlayerVec.Length() - playerBodyWidth - npcBodyWidth < 5)
	{
		gfl2::math::Vector3 pos_work(npcPos + playerToNpcVec.Normalize() * 1.0f );
		// gfl2::math::Vector3 pos_work(playerPos + playerToNpcVec.Normalize() * (playerBodyWidth + npcBodyWidth + 5.0f ));
		// pos_work = 

		pMoveCodeWork->pModel->GetCharaDrawInstance()->SetPosition(pos_work);

	}
}

// 各種カウンター制御
void FieldMoveCodeFureaiTypeA::CountResetMove(FieldMoveCodeWork* pMoveCodeWork)
{

}
void FieldMoveCodeFureaiTypeA::CountResetRndIdle(FieldMoveCodeWork* pMoveCodeWork, b32 resetActionCount )
{
	// 共有ワークを取得
	Work* pWork = (Work*)FieldMoveCodeManager::GetMoveCodeWork(pMoveCodeWork, sizeof(Work));

	f32 waitTable[4] = { 7.0f, 7.0f, 9.0f, 12.0f };

	if (!pWork->isIconEndFlag && pWork->isFirstMoveEndFlag)
	{
		pWork->randActionWaitCount = 5;
	}
	else
	{
		pWork->randActionWaitCount = waitTable[System::GflUse::GetPublicRand() % GFL_NELEMS(waitTable)] * 30;
	}
	
	if (resetActionCount)
	{
		pWork->randActionCount = 0;
	}
}

}; //end of namespace MoveModel
}; //end of namespace Field
