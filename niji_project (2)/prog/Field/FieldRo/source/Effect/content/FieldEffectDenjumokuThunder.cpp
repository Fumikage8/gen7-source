//======================================================================
/**
 * @file FieldEffectDenjumokuThunder.cpp
 * @date 2016/10/21 12:14:26
 * @author kawazoe_shinichi
 * @brief デンジュモクマップの雷
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectDenjumokuThunder.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @class  EffectDenjumokuThunder
*  @brief  デンジュモクマップの雷
*/
/**
*  @brief  コンストラクタ
*/
EffectDenjumokuThunder::EffectDenjumokuThunder(void)
: IEffectBase()
, m_pEffectHeap       (NULL)
, m_pEffectSystem     (NULL)
, m_pEffectHandle     (NULL)
, m_pEffectHandleExp  (NULL)
, m_fScaleCoeff       (1.0f)
{
}

/**
*  @brief  デストラクタ
*/
EffectDenjumokuThunder::~EffectDenjumokuThunder(void)
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  初期化
*/
void EffectDenjumokuThunder::Initialize(SetupData &setupData)
{
  m_vPosition         = setupData.position;
  m_pEffectHeap       = setupData.pEffectHeap;
  m_pEffectSystem     = setupData.pEffectSystem;
  m_fScaleCoeff       = setupData.fScaleCoeff;
  m_pEffectHandle     = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle;
  m_pEffectHandleExp  = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle;

  m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_THUNDER);
  m_pEffectSystem->CreateEmitterSetID(m_pEffectHandleExp, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_EXP);

  if (m_pEffectHandle && m_pEffectHandle->GetEmitterSet() &&
    m_pEffectHandleExp && m_pEffectHandleExp->GetEmitterSet())
  {
    // trans
    nw::math::VEC3 nwPos(m_vPosition.x, m_vPosition.y, m_vPosition.z);
    // rot
    nw::math::VEC3 nwRot(setupData.rotation.x, setupData.rotation.y, setupData.rotation.z);
    // scale
    nw::math::VEC3 nwScale(m_fScaleCoeff, m_fScaleCoeff, m_fScaleCoeff);
    // 設定
    nw::math::MTX34 srt;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srt, nwScale, nwRot, nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srt, nwScale, nwRot, nwPos);
#endif
    m_pEffectHandle->GetEmitterSet()->SetMtx(srt);
    m_pEffectHandleExp->GetEmitterSet()->SetMtx(srt);
  }
}

/**
*  @brief  破棄
*/
bool EffectDenjumokuThunder::Terminate(void)
{
  End();
  GFL_SAFE_DELETE(m_pEffectHandle);
  GFL_SAFE_DELETE(m_pEffectHandleExp);
  if (m_pEffectSystem && m_pEffectHeap)
  {
    m_pEffectSystem = NULL;
    m_pEffectHeap = NULL;
  }
  return true;
}

/**
*  @brief  再生
*/
void EffectDenjumokuThunder::Start(void)
{
  if (!IsAlive())
  {
    m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_THUNDER);
    m_pEffectSystem->CreateEmitterSetID(m_pEffectHandleExp, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_EXP);
  }
}

/**
*  @brief  停止
*/
void EffectDenjumokuThunder::End(void)
{
  if (m_pEffectHandle->GetEmitterSet())
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
  if (m_pEffectHandleExp->GetEmitterSet())
  {
    m_pEffectHandleExp->GetEmitterSet()->Kill();
  }
}

/**
*  @brief  再生確認
*/
bool EffectDenjumokuThunder::IsAlive(void)
{
  if (!m_pEffectHandle->GetEmitterSet() && !m_pEffectHandleExp->GetEmitterSet())             { return false; }
  if (!m_pEffectHandle->GetEmitterSet()->IsAlive() && !m_pEffectHandleExp->GetEmitterSet()->IsAlive())  { return false; }

  return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  更新処理
*/
void EffectDenjumokuThunder::updateNormal(void)
{
  if (IsAnimationLastFrame())
  {
    // 再生終了したので自殺する
    RequestDelete();
  }
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)