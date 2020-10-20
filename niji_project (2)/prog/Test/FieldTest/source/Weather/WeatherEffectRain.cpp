﻿//===================================================================
/**
 * @file    WeatherEffectRain.cpp
 * @brief   天候エフェクト雨
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#include "Test/FieldTest/include/Weather/WeatherEffectRain.h"

#include "Test/FieldTest/include/FieldEffectManager.h"
#include "Test/FieldTest/include/FieldEffectEmitterControl.h"
#include "Test/FieldTest/include/FieldEffectModel.h"

#include "Test/FieldTest/include/FieldTestUtility.h"

#include "System/include/GflUse.h" // for PublicRand

// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

// effectリソース
#include <niji_conv_header/field/effect/effect_weather.h>

// resource
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

// FieldTest ( Debug )
#include "test/FieldTest/include/Debug/FieldDebugMenu.h"
#include "test/FieldTest/include/Debug/FieldDebugTypes.h"


GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( Weather )

/**
 * @brief   コンストラクタ
 * @param   pSystemHeap インスタンス生成などに使用するヒープ
 * @param   pGLAllocator リソース確保用アロケータ
 * @param   pEffectManager フィールドエフェクト管理クラス
 */
WeatherEffectRain::WeatherEffectRain( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager) :
  WeatherEffectBase( pSystemHeap, pGLAllocator, pEffectManager)
{
}

/**
 * @brief   デストラクタ
 */
WeatherEffectRain::~WeatherEffectRain()
{
}

/**
 * @brief   初期化
 */
void WeatherEffectRain::Initialize()
{
}

/**
 * @brief   読み込み
 * @retval  true 完了
 * @retval  false 実行中
 */
b8 WeatherEffectRain::Load()
{
  gfl2::Effect::System* pEffectSystem = m_pEffectManager->GetSystem();

  // 降るエフェクト生成
  pEffectSystem->CreateEmitterSetID(
    &m_eftHandleFall, gfl2::math::Vector3( 0.0f, 0.0f, 0.0f),
    EmitterSetFieldWeather::we_rain, Field::Effect::Manager::RESOURCE_ID_WEATHER, Field::Effect::Manager::GROUP_ID_0
    );

  this->SetEmissionRatioEmitterAll( m_eftHandleFall, 0.0f);

  // 地面のエフェクトを生成
  // 一度生成しておく
  for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
  {
    pEffectSystem->CreateEmitterSetID(
      &m_eftHandleUnder[i], gfl2::math::Vector3( 0.0f, 0.0f, 0.0f),
      EmitterSetFieldWeather::we_rain_under, Field::Effect::Manager::RESOURCE_ID_WEATHER, Field::Effect::Manager::GROUP_ID_0
      );
  }

  gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResourceRootNode;
  pModelResourceRootNode = m_pEffectManager->GetModelResourceRootNode( Field::Effect::Manager::RESOURCE_ID_WEATHER);

  gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionResourceRootNode;
  pMotionResourceRootNode = m_pEffectManager->GetMotionResourceRootNode( Field::Effect::Manager::RESOURCE_ID_WEATHER);

  gfl2::animation::AnimationController::InitDescription desc;
  desc.materialSlotCount   = 1;
  desc.visibilitySlotCount = 1;

  for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
  {
    m_pEffectModel[i] = GFL_NEW( m_pSystemHeap) Field::Effect::Model(); 
    m_pEffectModel[i]->Initialize( m_pSystemHeap, pModelResourceRootNode, pMotionResourceRootNode, desc);
  }

  m_resourceSwitch = RESOURCE_SWITCH_PARTICLE;
#if PM_DEBUG
  m_resourceSwitch = -1; // 絶対更新させるため
  this->UpdateResourceSwitch();
#endif // PM_DEBUG

  return true;
}

/**
 * @brief   開始演出
 * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
 * @param   changeTime 切り替え時間(秒)
 */
b8 WeatherEffectRain::UpdateIn( f32 intervalFrameTime, f32 changeTime)
{
  this->UpdateCore();

  f32 ratio = 1.0f;
  if( changeTime > 0.0f)
  {
    m_timeCnt += intervalFrameTime;
    m_timeCnt = gfl2::math::Clamp( m_timeCnt, 0.0f, changeTime);
    ratio = (m_timeCnt / changeTime);
  }

  this->SetEmissionRatioEmitterAll( m_eftHandleFall, ratio);

  if( m_timeCnt >= changeTime)
  {
    m_timeCnt = 0;
    return true;
  }
  return false;
}

/**
 * @brief   演出メイン処理
 */
void WeatherEffectRain::UpdateMain()
{
  this->UpdateCore();
}

/**
 * @brief   終了演出
 * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
 * @param   changeTime 切り替え時間(秒)
 */
b8 WeatherEffectRain::UpdateOut( f32 intervalFrameTime, f32 changeTime)
{
  this->UpdateCore();

  f32 ratio = 0.0f;
  if( changeTime > 0.0f)
  {
    m_timeCnt += intervalFrameTime;
    m_timeCnt = gfl2::math::Clamp( m_timeCnt, 0.0f, changeTime);
    ratio = 1.0f - (m_timeCnt / changeTime);
  }

  this->SetEmissionRatioEmitterAll( m_eftHandleFall, ratio);

  if( m_timeCnt >= changeTime)
  {
    m_timeCnt = 0;
    return true;
  }
  return false;
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 WeatherEffectRain::Terminate()
{
  return WeatherEffectBase::Terminate();
}

/**
 * @brief   再開処理
 */
void WeatherEffectRain::Restart()
{
}

/**
 * @brief   描画
 */
void WeatherEffectRain::Draw()
{
}

/**
 * @brief   アルファ値設定
 * @param   alpha アルファ値(0.0〜1.0)
 * @param   changeFrame 遷移にかけるフレーム数
 */
void WeatherEffectRain::SetAlpha( f32 alpha, f32 changeFrame)
{
}

/**
 * @brief   カメラ更新後に行う更新
 * @param   cameraPos カメラ位置
 */
void WeatherEffectRain::UpdateAfterCameraUpdate( gfl2::math::Vector3& cameraPos)
{
  WeatherEffectBase::UpdateAfterCameraUpdate( cameraPos);
}

/**
 * @brief   更新コア処理
 */
void WeatherEffectRain::UpdateCore()
{
#if PM_DEBUG
  this->UpdateResourceSwitch();
#endif // PM_DEBUG

  this->UpdateGroundEffect();
}

void WeatherEffectRain::UpdateGroundEffect()
{
  gfl2::Effect::System* pEffectSystem = m_pEffectManager->GetSystem();

  const f32 RANGE_X = 600.0f;
  const f32 RANGE_Z = 800.0f;

  // 基準点をずらして、そこからのオフセットで位置を求める
  gfl2::math::Vector3 groundPosBase = m_playerPos;
  groundPosBase.x -= ( RANGE_X * 0.5f );
  groundPosBase.z -= ( RANGE_Z * 0.5f );

  if( m_resourceSwitch == RESOURCE_SWITCH_PARTICLE)
  {
    for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
    {
      // エミッターが無くなったら初期化
      if( m_eftHandleUnder[i].IsValid())
      {
        if( m_eftHandleUnder[i].GetEmitterSet()->GetNumEmitter() == 0)
        {
          m_eftHandleUnder[i].GetEmitterSet()->Kill();

          gfl2::math::Vector3 groundPos;
          this->CalculateGroundEffectPosition( &groundPos, groundPosBase, RANGE_X, RANGE_Z);
          pEffectSystem->CreateEmitterSetID(
            &m_eftHandleUnder[i], groundPos,
            EmitterSetFieldWeather::we_rain_under, Field::Effect::Manager::RESOURCE_ID_WEATHER, Field::Effect::Manager::GROUP_ID_0);

          // ばらつきを出すため1度初期化したらbreak
          break;
        }
      }
    }
  }

  if( m_resourceSwitch == RESOURCE_SWITCH_MODEL)
  {
    for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
    {
      m_pEffectModel[i]->Update();
      if( m_pEffectModel[i]->GetAnimationController()->GetMaterialSlot()->IsLastFrame())
      {
        gfl2::math::Vector3 groundPos;
        this->CalculateGroundEffectPosition( &groundPos, groundPosBase, RANGE_X, RANGE_Z);
        m_pEffectModel[i]->GetModelInstanceNode()->SetLocalTranslate( groundPos);

        // ばらつきを出すため1度初期化したらbreak
        break;
      }
    }
  }
}

void WeatherEffectRain::CalculateGroundEffectPosition( gfl2::math::Vector3* pOutPos, const gfl2::math::Vector3& basePos, f32 rangeX, f32 rangeZ)
{
  f32 rx = static_cast<f32>( System::GflUse::GetPublicRand( rangeX));
  f32 rz = static_cast<f32>( System::GflUse::GetPublicRand( rangeZ));

  pOutPos->x = basePos.x + rx;
  pOutPos->y = basePos.y;
  pOutPos->z = basePos.z + rz;
}

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

