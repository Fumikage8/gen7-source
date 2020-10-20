//=================================================================================================
/**
*  @file   FieldMoveModelShadowManager.cpp
*  @brief  フィールド用 動作モデルの影管理マネージャー
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <heap/include/gfl2_Heap.h>
#include <arc_def_index/arc_def.h>
#include "../../include/MoveModel/FieldMoveModelShadowManager.h"
#include "GameSys/include/GameManager.h"
#include "Field/FieldRo/include/Fieldmap.h"
#include "Field/FieldRo/include/MyRenderingPipeLine.h" 

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectShadow.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectShadowFoot.h"

namespace Field{ namespace MoveModel {

const f32 FieldMoveModelShadowManager::SHADOW_OFFSET_FROM_GROUND = 3.0f;

//-----------------------------------------------------------------------------
/**
*  @brief  コンストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelShadowManager::FieldMoveModelShadowManager(Field::MoveModel::FieldMoveModelManager* pFieldCharaModelManager,Field::Effect::EffectManager* pEffectManager)
{
  m_pFieldCharaModelManager     = pFieldCharaModelManager;
  m_pEffectManager               = pEffectManager;
  m_pEffectShadow[SHADOW_CIRCLE] = static_cast<Effect::EffectShadow*    >(m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_SHADOW_CIRCLE , NULL )); 
  m_pEffectShadow[SHADOW_DEPTH ] = static_cast<Effect::EffectShadow*    >(m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_SHADOW_DEPTH  , NULL )); 
  m_pEffectShadow[SHADOW_WIDTH ] = static_cast<Effect::EffectShadow*    >(m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_SHADOW_WIDTH  , NULL ));
  m_pEffectShadowFoot            = static_cast<Effect::EffectShadowFoot*>(m_pEffectManager->CreateEffect( Effect::EFFECT_TYPE_SHADOW_FOOT   , m_pFieldCharaModelManager->GetFieldMoveModel(FIELD_MOVE_MODEL_PLAYER ) ));
}

//-----------------------------------------------------------------------------
/**
*  @brief  デストラクタ
*/
//-----------------------------------------------------------------------------
FieldMoveModelShadowManager::~FieldMoveModelShadowManager()
{
  // エフェクトマネージャーの破棄に任せる
  for( s32 i=0 ; i<SHADOW_MAX ; i++ )
  {
    if(m_pEffectShadow[i])
    {
      m_pEffectShadow[i]->Delete();
      m_pEffectShadow[i] = NULL;
    }
  }

  if(m_pEffectShadowFoot)
  {
    m_pEffectShadowFoot->Delete();
    m_pEffectShadowFoot = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  影エフェクトの更新
*/
//-----------------------------------------------------------------------------
void FieldMoveModelShadowManager::UpdateShadow(void)
{
  FieldMoveModelPlayer* pPlayer = static_cast<FieldMoveModelPlayer*>( m_pFieldCharaModelManager->GetFieldMoveModel(FIELD_MOVE_MODEL_PLAYER ) );

  //-----------------------------------------------------------
  // 丸影を初期化
  //-----------------------------------------------------------

  s32 shadowIndex[SHADOW_MAX] = {1,1,1};
  if(m_pEffectShadow[SHADOW_CIRCLE])
  {
    m_pEffectShadow[SHADOW_CIRCLE]->setSRT( 0.0f, gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f) );
  }
  if(m_pEffectShadow[SHADOW_DEPTH])
  {
    m_pEffectShadow[SHADOW_DEPTH ]->setSRT( 0.0f, gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f) );
  }
  if(m_pEffectShadow[SHADOW_WIDTH])
  {
    m_pEffectShadow[SHADOW_WIDTH ]->setSRT( 0.0f, gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f),gfl2::math::Vector3(0.0f,0.0f,0.0f) );
  }

  //-----------------------------------------------------------
  // 丸影を更新 
  //-----------------------------------------------------------
  gfl2::math::Quaternion playerGroundQua(0.0f,0.0f,0.0f,1.0f);
  for( u32 i=0 ; i<FIELD_MOVE_MODEL_MAX ; i++ )
  {
    //---------------------------------------------------------
    // 丸影が利用できる状態かどうか
    //---------------------------------------------------------

    // 未セットアップの動作モデルは影は無効
    FieldMoveModel* pFieldMoveModel = m_pFieldCharaModelManager->GetFieldMoveModel(i);
    if( !pFieldMoveModel->IsSetup() )
    {
      continue;
    }

    // カリング中なら無効
    if( pFieldMoveModel->IsEnableCulilng() )
    {
      continue;
    }

    // 描画設定がオフなら影は無効
    if( !pFieldMoveModel->GetCharaDrawInstance()->IsVisible() )
    {
      continue;
    }

    // プレイヤーはライド時になら影は無効
    if( i==FIELD_MOVE_MODEL_PLAYER && pPlayer->IsRideMoveModel() )
    {
      continue;
    }

    // @fix MMcat[409]:ホクラニ天文台にて、自機が入口付近に居るとマーレインの影が正しく表示されない、対処。表示数オーバーが発生していたので、明示的に切る機能を追加
    if (pFieldMoveModel->CheckStateBit( Field::MoveModel::FIELD_MOVE_MODEL_STATEBIT_SHADOW_OFF ))
    {
      continue;
    }

    // アプリケーションデータをコンバートリストから取得
    const Field::MoveModel::ApplicationDataForCommon* pAppData         = m_pFieldCharaModelManager->GetApplicationData( pFieldMoveModel->GetCharacterId() );
    poke_3d::model::CharaModel*                       pFieldCharaModel = pFieldMoveModel->GetCharaDrawInstance();

    // 影を出すオブジェクトかどうか
    SHADOW_TYPE shadowType = SHADOW_MAX;
    // player用にカスタマイズ
    if     ( pAppData->type == Field::MoveModel::TYPE_PLAYER  ){ shadowType = (SHADOW_TYPE)((ApplicationDataForPlayer* )pAppData)->shadowType; }
    // npc用にカスタマイズ
    else if( pAppData->type == Field::MoveModel::TYPE_NPC     ){ shadowType = (SHADOW_TYPE)((ApplicationDataForNPC*    )pAppData)->shadowType; }
    // pokemon用にカスタマイズ
    else if( pAppData->type == Field::MoveModel::TYPE_POKEMON ){ shadowType = (SHADOW_TYPE)((ApplicationDataForPokemon*)pAppData)->shadowType; }

    // 影タイプが不正
    if( shadowType >= SHADOW_MAX )
    {
      continue;
    }

    //--------------------------------------------------------
    // 丸影のSRT更新
    //--------------------------------------------------------

    // 地面の高さ・法線を取得
    const f32           GROUND_OFFSET( 3.0f );
    f32                 groundHeight ( pFieldMoveModel->GetGroundHeight() );
    gfl2::math::Vector3 groundNormal ( pFieldMoveModel->GetGroundNormal() );

    // @fix GFNMCat[519] 空手王だけは待機アニメーション時に動作モデルの座標を動かさず、その場で演技をするので影がずれないよう対処
    u32 jointIndex = pFieldCharaModel->GetJointIndex("Origin");
    if( !pFieldMoveModel->IsAttachShadowToOrigin()
      || pFieldMoveModel->GetCharacterId() == CHARA_MODEL_ID_TR0091_00 )
    {
      jointIndex = pFieldCharaModel->GetJointIndex("Waist");
    }

    // 影をアタッチするジョイントの座標と姿勢を取得
    gfl2::math::Vector3    jointPos( pFieldCharaModel->GetJointWorldMatrix(jointIndex).GetElemPosition() );
    gfl2::math::Quaternion jointQua( gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f) );
    jointQua.MatrixToQuaternion( pFieldCharaModel->GetJointWorldMatrix(jointIndex) );

    // 影パラメータのデフォルト値を取得
    f32                 shadowScale    ( 1.0f );
    gfl2::math::Vector3 shadowPos      ( gfl2::math::Vector3( jointPos.x, groundHeight+GROUND_OFFSET, jointPos.z ) );
    gfl2::math::Vector3 shadowPosOffset( gfl2::math::Vector3( 0.0f , 0.0f , 0.0f ) );

    // player用にカスタマイズ
    if     ( pAppData->type == Field::MoveModel::TYPE_PLAYER )
    {
      shadowPosOffset = jointQua.Transform( gfl2::math::Vector3( ((ApplicationDataForPlayer*)pAppData)->shadowOffsetX , 0.0f , ((ApplicationDataForPlayer*)pAppData)->shadowOffsetZ ) );
      shadowScale     = (f32)((ApplicationDataForPlayer*)pAppData)->shadowScale / 100.0f;

      // ジョインフェスタのゲストはプレイヤータイプなので弾く
      if( i==FIELD_MOVE_MODEL_PLAYER || i==FIELD_MOVE_MODEL_RIDE_POKEMON )
      {
        groundNormal = pPlayer->GetGroundNormal();
        shadowPos.y  = pPlayer->GetGroundHeight()+GROUND_OFFSET;
      }
    }
    // npc用にカスタマイズ
    else if( pAppData->type == Field::MoveModel::TYPE_NPC )
    {
      shadowPosOffset = jointQua.Transform( gfl2::math::Vector3( ((ApplicationDataForNPC*)pAppData)->shadowOffsetX , 0.0f , ((ApplicationDataForNPC*)pAppData)->shadowOffsetZ ) );
      shadowScale     = (f32)((ApplicationDataForNPC*)pAppData)->shadowScale/100.0f;
    }
    // pokemon用にカスタマイズ
    else if( pAppData->type == Field::MoveModel::TYPE_POKEMON )
    {
      shadowPosOffset = jointQua.Transform( gfl2::math::Vector3( ((ApplicationDataForPokemon*)pAppData)->shadowOffsetX , 0.0f , ((ApplicationDataForPokemon*)pAppData)->shadowOffsetZ ) );
      shadowScale     = (f32)((ApplicationDataForPokemon*)pAppData)->shadowScale/100.0f;
    }
    shadowPosOffset.y = 0.0f;
    shadowPos += shadowPosOffset;

    // UnitYから地面法線に遷移するクォータニオンを作成
    gfl2::math::Vector3    up        ( 0.0f,1.0f,0.0f );
    gfl2::math::Vector3    axis      ( up.Cross( groundNormal ).Normalize() );
    f32                    dot       ( gfl2::math::Clamp( groundNormal.Dot(up), -1.0f , 1.0f ) );
    f32                    angle     ( acos( dot ) );
    f32                    cosAngle  ( cos(angle * 0.5f) );
    f32                    sinAngle  ( sin(angle * 0.5f) );
    gfl2::math::Quaternion groundQua ( sinAngle * axis.x, sinAngle * axis.y, sinAngle * axis.z, cosAngle );
    groundQua.Normalize();
    if (dot >= 0.99f )
    {
      groundQua = gfl2::math::Quaternion(0.0f,0.0f,0.0f,1.0f);
    }

    // 親と地面を両方考慮したクォータニオンを設定
    // note:Waistジョイントの座標に影をだすものでも姿勢はOriginジョイントを利用する
    gfl2::math::Quaternion parentQua( 0.0f,0.0f,0.0f,1.0f );
    gfl2::math::Quaternion yQUa     ( 0.0f,0.0f,0.0f,1.0f );
    parentQua.MatrixToQuaternion( pFieldCharaModel->GetJointWorldMatrix( pFieldCharaModel->GetJointIndex("Origin") ) );
    yQUa.RadianYToQuaternion( PI/2.0f );
    gfl2::math::Quaternion resutQua ( groundQua * parentQua * yQUa );

    // SRTを設定
    if( m_pEffectShadow[ shadowType ] )
    {
      static_cast<Effect::EffectShadow*>(m_pEffectShadow[ shadowType ])->setSRT( (u32)shadowIndex[ shadowType ] , shadowScale , resutQua , shadowPos );
    }
    shadowIndex[ shadowType ] += 1;

    // プレイヤー専用の足跡影用に地面クォータニオンの保存
    if( i == FIELD_MOVE_MODEL_PLAYER )
    {
      playerGroundQua = groundQua;
    }
  }

  //-----------------------------------------------------------
  // プレイヤー専用の足跡影のSRT更新
  //-----------------------------------------------------------

  // プレイヤーの影描画設定を取得
  const Field::MoveModel::ApplicationDataForPlayer* pPlayerData         = (ApplicationDataForPlayer*)m_pFieldCharaModelManager->GetApplicationData( pPlayer->GetCharacterId() );
  Field::MoveModel::ApplicationDataForPlayer*       pRideData           = NULL;
  b32                                               isVisibleFootShadow = (b32)pPlayerData->shadowFootEnable ? true : false;

  // ライド対象の影描画設定を取得
  if( pPlayer->IsRideMoveModel() )
  {
    pRideData = (ApplicationDataForPlayer*)m_pFieldCharaModelManager->GetApplicationData( pPlayer->GetOnMoveModel()->GetCharacterId() );
    isVisibleFootShadow = (b32)pRideData->shadowFootEnable ? true : false;
  }

  // 足跡影を無効
  if( !isVisibleFootShadow
   ||  pPlayer->IsEnableCulilng()
   || !pPlayer->GetCharaDrawInstance() 
   || !pPlayer->GetCharaDrawInstance()->IsVisible() )
  {
    if( m_pEffectShadowFoot )
    {
      m_pEffectShadowFoot->SetVisibleShadow( false );
    }
  }
  // 足跡影を有効
  else
  {
    if(m_pEffectShadowFoot)
    {
      m_pEffectShadowFoot->SetVisibleShadow( true );
    }

    poke_3d::model::CharaModel* pPlayerCharaModel = pPlayer->GetCharaDrawInstance();

    for( u32 i=0 ; i<2 ; i++ )
    {
      // 影を出す対象足を取得
      s32 jointToeIndex  = -1;
      s32 jointFootIndex = -1;
      if( i==0 )
      {
        jointToeIndex  = pPlayerCharaModel->GetJointIndex("LToe");
        jointFootIndex = pPlayerCharaModel->GetJointIndex("LFoot");
      }
      if( i==1 )
      { 
        jointToeIndex  = pPlayerCharaModel->GetJointIndex("RToe");
        jointFootIndex = pPlayerCharaModel->GetJointIndex("RFoot");
      }

      // 影の座標を取得
      gfl2::math::Vector3 newFootShadowPos ( pPlayerCharaModel->GetJointWorldPosition(jointToeIndex) );
      newFootShadowPos.y = pPlayer->GetGroundHeight() + SHADOW_OFFSET_FROM_GROUND;
      if( !pPlayer->IsSetCollisionSceneContainerForGround() || !pPlayer->IsOnGround() )
      {
        newFootShadowPos.y = pPlayer->GetCharaDrawInstance()->GetPosition().y;
      }

      // 影の姿勢を取得
      gfl2::math::Vector3 UnitX    (1.0f,0.0f,0.0f);
      gfl2::math::Vector3 UnitZ    (0.0f,0.0f,1.0f);
      gfl2::math::Vector3 FootFront(pPlayerCharaModel->GetJointWorldMatrix(jointFootIndex).GetElemAxisX());  // X軸伝播
      FootFront.y = 0.0f;
      FootFront   = FootFront.Normalize();
      f32 defRad  = gfl2::math::AcosRad( UnitZ.Dot( FootFront ) );
      if( UnitX.Dot( FootFront ) < 0.0f ){ defRad *= -1.0f; }

      gfl2::math::Quaternion  newFootShadowQua      (0.0f,0.0f,0.0f,1.0f);
      gfl2::math::Quaternion  newFootShadowQuaOffset(0.0f,0.0f,0.0f,1.0f);
      newFootShadowQua.RadianYToQuaternion( defRad );
      newFootShadowQuaOffset.RadianYToQuaternion( -PI/2.0f );
      newFootShadowQua = newFootShadowQua * newFootShadowQuaOffset;

      // 影のスケールを取得
      f32 newFootShadowScale = 1.0f;
      if( pRideData ){ newFootShadowScale = (f32)pRideData->shadowFootScale  /100.0f; }
      else           { newFootShadowScale = (f32)pPlayerData->shadowFootScale/100.0f; }

      // 影の半透明を設定
      f32 GROUND_OFFSET = 4.0f;
      f32 MAX_DISTANCE  = 30.0f;
      if( pPlayerCharaModel->GetAnimationId() == P2_BASE_FI010_WALK01 )
      {
        MAX_DISTANCE = 25.0f;
      }
      f32 defLocalY   = pPlayerCharaModel->GetJointWorldPosition(jointFootIndex).y - pPlayerCharaModel->GetJointWorldPosition(1).y;
      f32 defGlobalY  = pPlayerCharaModel->GetPosition().y - pPlayer->GetGroundHeight() - GROUND_OFFSET;
      f32 shadowAlpha = gfl2::math::Clamp( (1.0f - defLocalY / MAX_DISTANCE) , 0.0f , 1.0f ) *
                        gfl2::math::Clamp( (1.0f - defGlobalY/ 100.0f      ) , 0.0f , 1.0f );

      // つま先が反転した際に影も反転するので表示をオフ
      gfl2::math::Matrix34 localFootMat  ( pPlayerCharaModel->GetWorldMatrix().Inverse() * pPlayerCharaModel->GetJointWorldMatrix(jointFootIndex) );
      gfl2::math::Vector3  localFootFront( localFootMat.GetElemAxisX() );
      if( localFootFront.z < 0.0f )
      {
        shadowAlpha = 0.0f;
      }

      // 影のSRT設定
      if(m_pEffectShadowFoot)
      {
        m_pEffectShadowFoot->setSRT( (i==0) ? true:false,
                                     newFootShadowScale,
                                     newFootShadowQua,
                                     newFootShadowPos,
                                     shadowAlpha);
      }
    }
  }
}

//-----------------------------------------------------------------------------
/**
*  @brief  影エフェクトの取得
*/
//-----------------------------------------------------------------------------
Field::Effect::EffectShadow* FieldMoveModelShadowManager::GetEffectShadow( u32 index) const
{
  if( index < SHADOW_MAX)
  {
    return m_pEffectShadow[index];
  }
  return NULL;
}

//-----------------------------------------------------------------------------
/**
*  @brief  足跡影エフェクトの取得
*/
//-----------------------------------------------------------------------------
Field::Effect::EffectShadowFoot* FieldMoveModelShadowManager::GetEffectShadowFoot() const
{
  return m_pEffectShadowFoot;
}

}; //end of namespace MoveModel
}; //end of namespace Field
