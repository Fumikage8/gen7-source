//======================================================================
/**
 * @file SandStorm.cpp
 * @date 2015/12/09 20:13:08
 * @author saito_nozomu
 * @brief 砂嵐
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include "Field/FieldRo/include/Weather/SandStorm.h"
#include "Field/FieldRo/include/Weather/BillBoardfade.h"
// system
#include "System/include/GflUse.h"

#include "Field/FieldRo/include/Effect/FieldEffectManager.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectSandStorm.h"
#include "Field/FieldRo/include/Camera/FieldCameraManager.h"
#include "Field/FieldRo/include/Camera/FieldCameraUnit.h"
#include "Sound/include/Sound.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(weather)

SandStorm::SandStorm(gfl2::heap::HeapBase* pHeap) :WeatherBase()
{
  m_pHeap = pHeap;
  m_pSandStormFade = GFL_NEW(m_pHeap) BillBoardFade();
  m_pEffectManager =NULL;
  m_pCameraManager = NULL;
  m_pSandStormEff = NULL;
}

SandStorm::~SandStorm()
{
  if (m_pSandStormFade != NULL ) GFL_SAFE_DELETE(m_pSandStormFade);
}

void SandStorm::Initialize(InitParam &param)
{
  m_pEffectManager = param.pEffectManager;
  m_pCameraManager = param.pCameraManager;
  m_pSandStormEff = NULL;

  m_Type = WEATHER_NONE;
}

/**
  * @brief セットアップ
  */
bool SandStorm::SetupFunc(void)
{
  m_FadeOutSeq = 0;
  //天候作成
  Camera::CameraUnit* pCameraUnit = m_pCameraManager->GetMainGamePlayCamera();
  gfl2::math::Vector3 pos = pCameraUnit->GetBaseCamera()->GetPosition();
  gfl2::math::Quaternion rot = pCameraUnit->GetBaseCamera()->GetRotationQuat();
  gfl2::math::Vector3 dir( 0.0f, 0.0f, -1.0f );
  dir = rot.Transform( dir );
  gfl2::math::Vector3 eff_pos(0,0,0);
  Effect::IEffectBase *pEffect = m_pEffectManager->CreateEffect(Effect::EFFECT_TYPE_SANDSTORM, eff_pos);
  Effect::EffectSandStorm *eff = static_cast<Effect::EffectSandStorm*>(pEffect);

  m_pSandStormEff = eff;
  if (m_pSandStormEff == NULL )
  {
    return true;
  }

  eff->CreateEffect();

  m_pSandStormFade->Initialize( m_pSandStormEff->GetSandStormModel(), 0 );

  switch(m_Type)
  {
  case WEATHER_SANDSTORM:
    m_pSandStormEff->GetSandStormModel()->SetVisible(true);
    m_pSandStormFade->SetVisible(false);
    break;
  }

  return true;
}

void SandStorm::StartFunc(void)
{
  if (m_pSandStormEff == NULL )
  {
    return;
  }

  m_pSandStormFade->SetVisible(false);
}

void SandStorm::MainFunc(void)
{
  if (m_pSandStormEff == NULL )
  {
    return;
  }

  //ビルボードのフェードインフェードアウト処理
  m_pSandStormFade->Update();

}

bool SandStorm::FadeOutFunc(void)
{
  switch(m_FadeOutSeq)
  {
  case 0:
    FadeoutWeatherRequest(m_Type);
    m_FadeOutSeq++;
    break;
  case 1:
    if ( IsFinishedFadeOut(m_Type) )
    {
      m_Type = WEATHER_NONE;
      return true;
    }
    break;
  }

  return false;
}

/**
 * @brief ＳＥ再生
 */
void SandStorm::StartSE(void)
{
  switch(m_Type)
  {
  case WEATHER_SANDSTORM:
    Sound::PlaySE(SEQ_SE_FLD_AMB_TENKOU_SUNAARASHI);
    break;
  }
}

/**
 * @brief 天候のフェードアウト
 * @param[in] type 天候種類
 */
void SandStorm::FadeoutWeatherRequest(WEATHER_TYPE type)
{
  if (m_pSandStormEff == NULL )
  {
    return;
  }

  //現在の天候のフェードアウト
  switch(type)
  {
  case WEATHER_SANDSTORM:
    //ビルボードをフェードアウト
    m_pSandStormFade->RequestFadeOut();
    break;
  }
}

/**
 * @brief 天候の変更要請
 * @param[in] type 天候種類
 */
void SandStorm::ChangeWeatherRequestCore(WEATHER_TYPE type)
{
  if (m_pSandStormEff == NULL )
  {
    m_Type = type;
    return;
  }

  FadeoutWeatherRequest(m_Type);

  //次の天候のフェードイン
  switch(type)
  {
  case WEATHER_SANDSTORM:
    //ビルボードフェードイン
    m_pSandStormFade->RequestFadeIn();
    break;
  }
  m_Type = type;
}

/**
 * @brief タイプ別フェードアウト終了チェック
 * @param[in] type
 * @return bool ture でフェードアウト終了
 */
bool SandStorm::IsFinishedFadeOut(WEATHER_TYPE type)
{
  if (m_pSandStormEff == NULL )
  {
    return true;
  }

  switch(type)
  {
  case WEATHER_SANDSTORM:
    //ビルボードがフェードアウトしているか
    if (m_pSandStormFade->IsVisible()) return false;
    break;
  }

  return true;
}

GFL_NAMESPACE_END(weather)
GFL_NAMESPACE_END(Field)