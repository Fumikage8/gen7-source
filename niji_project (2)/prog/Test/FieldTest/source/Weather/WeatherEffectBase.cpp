//===================================================================
/**
 * @file    WeatherEffectBase.cpp
 * @brief   天候エフェクト基底クラス
 * @author  saita_kazuki
 * @date    2015.03.06
 */
//===================================================================

#include "Test/FieldTest/include/Weather/WeatherEffectBase.h"
#include "Test/FieldTest/include/FieldEffectManager.h"
#include "Test/FieldTest/include/FieldEffectModel.h"

#include "Test/FieldTest/include/FieldTestUtility.h"

// デバッグ
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

// メモリ
#include <gfx/include/gfl2_GLMemory.h>

// effect
#include "Test/FieldTest/include/FieldEffectEmitterControl.h"

// modelpackリソース
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <Animation/include/gfl2_AnimationController.h>

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
WeatherEffectBase::WeatherEffectBase( gfl2::heap::HeapBase* pSystemHeap, gfl2::gfx::IGLAllocator* pGLAllocator, Field::Effect::Manager* pEffectManager) :
  m_pSystemHeap( pSystemHeap),
  m_pGLAllocator( pGLAllocator),
  m_pEffectManager( pEffectManager),
  m_eftHandleFall(),
  m_eftHandleUnder(),
  m_pEffectModel(),
  m_seq( 0),
  m_timeCnt( 0.0f),
  m_playerPos()
{
}

/**
 * @brief   デストラクタ
 */
WeatherEffectBase::~WeatherEffectBase()
{
}

/**
 * @brief   初期化
 */
void WeatherEffectBase::Initialize()
{
}

/**
 * @brief   読み込み
 */
b8 WeatherEffectBase::Load()
{
  // @todo sangoでは
  //  ・ファイル読み込み
  //  ・解凍、アタッチ、パーティクル作成、接地エフェクト作成、
  //    スクリーンエフェクト作成などをしていた
  return true;
}

/**
 * @brief   開始演出
 * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
 * @param   changeTime 切り替え時間(秒)
 */
b8 WeatherEffectBase::UpdateIn( f32 intervalFrameTime, f32 changeTime)
{
  // @todo sangoでは切り替え時間に合わせて
  //  ・SEを鳴らし始める
  //  ・アルファ値を1.0に上げ始める
  //  ・放出量を設定された値に徐々に上げ始める
  //  ・接地エフェクトの生成レートを徐々に設定値まで上げる
  //  といったことをしていた
  return true;
}

/**
 * @brief   演出メイン処理
 */
void WeatherEffectBase::UpdateMain()
{
  // sangoではマテリアルの更新や、接地マテリアルの生成などをおこなっていた
  // ※ これらはUpdateIn、UpdateOutでも行う
}

/**
 * @brief   終了演出
 * @param   intervalFrameTime 1Syncで進行する時間(1.0/frame)
 * @param   changeTime 切り替え時間(秒)
 */
b8 WeatherEffectBase::UpdateOut( f32 intervalFrameTime, f32 changeTime)
{
  // @todo sangoでは切り替え時間に合わせて
  //  ・SEを停止させ始める
  //  ・アルファ値を0に落とし始める
  //  ・放出量を徐々に0まで落とす(最後は無限放出設定も切る)
  //  ・接地エフェクトの生成レートを徐々に0まで落とす
  // といったことをしていた
  return true;
}

/**
 * @brief   破棄
 * @retval  true  破棄完了
 * @retval  false 破棄実行中
 */
b8 WeatherEffectBase::Terminate()
{
  enum
  {
    TERMINATE_FALL_EFFECT,
    TERMINATE_UNDER_EFFECT,
    TERMINATE_UNDER_EFFECT_AFETER,
    FINISH,
  };

  switch( m_seq)
  {
  case TERMINATE_FALL_EFFECT:
    if( m_eftHandleFall.IsValid())
    {
      m_eftHandleFall.GetEmitterSet()->Kill();
    }
    m_seq++;
    //break;

  case TERMINATE_UNDER_EFFECT:
    for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
    {
      if( m_eftHandleUnder[i].IsValid())
      {
        m_eftHandleUnder[i].GetEmitterSet()->Kill();
      }
    }

    for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
    {
      if( m_pEffectModel[i] )
      {
        m_pEffectModel[i]->Terminate();
      }
      GFL_SAFE_DELETE( m_pEffectModel[i]);
    }
    m_seq++;

    //break;

  case TERMINATE_UNDER_EFFECT_AFETER:
    {
      m_seq = FINISH;
    }
    //break;

  case FINISH:
    return true;
    //break;
  }
  return false;
}

/**
 * @brief   再開処理
 */
void WeatherEffectBase::Restart()
{
  // sangoでは
  //  ・パーティクルの更新を進める
  //  ・SEを再開する
  //  ・エミッタ、接地エフェクトを最大にする
  //  ・アルファ値を1.0にする
  // といったことをしていた
}

/**
 * @brief   描画
 */
void WeatherEffectBase::Draw()
{
}

/**
 * @brief   アルファ値設定
 * @param   alpha アルファ値(0.0〜1.0)
 * @param   changeFrame 遷移にかけるフレーム数
 */
void WeatherEffectBase::SetAlpha( f32 alpha, f32 changeFrame)
{
}

/**
 * @brief   カメラ更新後に行う更新
 * @param   cameraPos カメラ位置
 */
void WeatherEffectBase::UpdateAfterCameraUpdate( gfl2::math::Vector3& cameraPos)
{
  if( m_eftHandleFall.IsValid())
  {
    Field::Effect::EmitterControl::SetPos( m_eftHandleFall, cameraPos);
  }
}

/**
 * @brief   エフェクトモデルインスタンスの取得
 * @param   idx モデル番号。現状はEFFECT_UNDER_MAX
 */
gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* WeatherEffectBase::GetEffectModelInstanceNode( u32 idx)
{
  GFL_ASSERT( idx < EFFECT_UNDER_MAX);
  if( m_pEffectModel[idx])
  {
    return m_pEffectModel[idx]->GetModelInstanceNode();
  }
  return NULL;
}

/**
 * @brief   エミッタセット全てのエミッタの放出レートのスケール値を変更
 * @param   handle 変更したいエミッタセット
 * @param   value 放出レートのスケール値
 */
void WeatherEffectBase::SetEmissionRatioEmitterAll( gfl2::Effect::Handle& handle, f32 value)
{
  if( handle.IsValid())
  {
    for( u32 i = 0; i < handle.GetEmitterSet()->GetNumCreatedEmitter(); ++i)
    {
      Field::Effect::EmitterControl::SetEmissionRatio( handle, i, value);
    }
  }
}

/**
 * @brief   エミッタセット全てのエミッタの放出間隔のスケール値を変更
 * @param   handle 変更したいエミッタセット
 * @param   value 放出間隔のスケール値
 */
void WeatherEffectBase::SetEmissionIntervalEmitterAll( gfl2::Effect::Handle& handle, f32 value)
{
  if( handle.IsValid())
  {
    for( u32 i = 0; i < handle.GetEmitterSet()->GetNumCreatedEmitter(); ++i)
    {
      Field::Effect::EmitterControl::SetEmissionInterval( handle, i, value);
    }
  }
}




#if PM_DEBUG
void WeatherEffectBase::UpdateResourceSwitch()
{
  s32 oldResourceSwitch = m_resourceSwitch;
  m_resourceSwitch = Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::DRAW_SWITCH_WEATHER ];
  if( oldResourceSwitch != m_resourceSwitch)
  {
    if( m_resourceSwitch == RESOURCE_SWITCH_NONE)
    {
      this->SetStopCalcAndDraw_FallEmitterSet( true);
      this->SetStopCalcAndDraw_UnderEmitterSetAll( true);
      this->SetVisible_EffectModelAll( false);
    }
    else if( m_resourceSwitch == RESOURCE_SWITCH_PARTICLE)
    {
      this->SetStopCalcAndDraw_FallEmitterSet( false);
      this->SetStopCalcAndDraw_UnderEmitterSetAll( false);
      this->SetVisible_EffectModelAll( false);
    }
    else if( m_resourceSwitch == RESOURCE_SWITCH_MODEL)
    {
      this->SetStopCalcAndDraw_FallEmitterSet( false);
      this->SetStopCalcAndDraw_UnderEmitterSetAll( true);
      this->SetVisible_EffectModelAll( true);
    }
  }
}

void WeatherEffectBase::SetStopCalcAndDraw_FallEmitterSet( bool isStopCalcAndDraw)
{
  // エミッタセットの計算と描画を止める
  if( m_eftHandleFall.IsValid())
  {
    m_eftHandleFall.GetEmitterSet()->SetStopCalcAndDraw( isStopCalcAndDraw);
  }
}

void WeatherEffectBase::SetStopCalcAndDraw_UnderEmitterSetAll( bool isStopCalcAndDraw)
{
  for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
  {
    if( m_eftHandleUnder[i].IsValid())
    {
      m_eftHandleUnder[i].GetEmitterSet()->SetStopCalcAndDraw( isStopCalcAndDraw);
    }
  }
}

void WeatherEffectBase::SetVisible_EffectModelAll( bool isVisible)
{
  // モデルの描画フラグをOFF
  for( u32 i = 0; i < EFFECT_UNDER_MAX; ++i)
  {
    m_pEffectModel[i]->SetVisible( isVisible);
  }
}
#endif // PM_DEBUG

GFL_NAMESPACE_END( Weather )
GFL_NAMESPACE_END( Field )

