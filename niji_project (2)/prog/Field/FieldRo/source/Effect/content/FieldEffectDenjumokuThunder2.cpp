//======================================================================
/**
 * @file FieldEffectDenjumokuThunder2.cpp
 * @date 2017/01/26 17:00:39
 * @author kawazoe_shinichi
 * @brief デンジュモクマップの雷2
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectDenjumokuThunder2.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @class  EffectDenjumokuThunder2
*  @brief  デンジュモクマップの雷2
*/
/**
*  @brief  コンストラクタ
*/
EffectDenjumokuThunder2::EffectDenjumokuThunder2(void)
: IEffectBase()
, m_pEffectHeap         (NULL)
, m_pEffectSystem       (NULL)
, m_pEffectHandle       (NULL)
, m_pEffectHandleRoot   (NULL)
, m_fScaleCoeff         (1.0f)
{
}

/**
*  @brief  デストラクタ
*/
EffectDenjumokuThunder2::~EffectDenjumokuThunder2(void)
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  初期化
*/
void EffectDenjumokuThunder2::Initialize(SetupData &setupData)
{
  m_vPosition         = setupData.position;
  m_vRotation         = setupData.rotation;
  m_fScaleCoeff       = setupData.fScaleCoeff;
  m_pEffectHeap       = setupData.pEffectHeap;
  m_pEffectSystem     = setupData.pEffectSystem;
  m_pEffectHandle     = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle;
  m_pEffectHandleRoot = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle;

  m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_THUNDER2);
  m_pEffectSystem->CreateEmitterSetID(m_pEffectHandleRoot, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_THUNDER2_ROOT);

  if (m_pEffectHandle && m_pEffectHandle->GetEmitterSet() &&
    m_pEffectHandleRoot && m_pEffectHandleRoot->GetEmitterSet())
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
    m_pEffectHandleRoot->GetEmitterSet()->SetMtx(srt);
  }
}

/**
*  @brief  破棄
*/
bool EffectDenjumokuThunder2::Terminate(void)
{
  End();
  GFL_SAFE_DELETE(m_pEffectHandle);
  GFL_SAFE_DELETE(m_pEffectHandleRoot);
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
void EffectDenjumokuThunder2::Start(void)
{
  if (!IsAlive())
  {
    m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_THUNDER2);
    m_pEffectSystem->CreateEmitterSetID(m_pEffectHandleRoot, m_vPosition, 0, EFFECT_RESOURCE_DENJUMOKU_THUNDER2_ROOT);
  }
}

/**
*  @brief  停止
*/
void EffectDenjumokuThunder2::End(void)
{
  if (m_pEffectHandle->GetEmitterSet())
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
  if (m_pEffectHandleRoot->GetEmitterSet())
  {
    m_pEffectHandleRoot->GetEmitterSet()->Kill();
  }
}

/**
*  @brief  再生確認
*/
bool EffectDenjumokuThunder2::IsAlive(void)
{
  if (!m_pEffectHandle->GetEmitterSet() && !m_pEffectHandleRoot->GetEmitterSet())             { return false; }
  if (!m_pEffectHandle->GetEmitterSet()->IsAlive() && !m_pEffectHandleRoot->GetEmitterSet()->IsAlive())  { return false; }

  return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  更新処理
*/
void EffectDenjumokuThunder2::updateNormal(void)
{

}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)