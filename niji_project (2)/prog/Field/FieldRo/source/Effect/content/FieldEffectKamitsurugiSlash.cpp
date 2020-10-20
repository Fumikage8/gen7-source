//======================================================================
/**
 * @file FieldEffectKamitsurugiSlash.cpp
 * @date 2017/02/02 16:36:45
 * @author kawazoe_shinichi
 * @brief カミツルギイベントの斬撃エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

// effect
#include "Field/FieldRo/include/Effect/content/FieldEffectKamitsurugiSlash.h"
#include "Field/FieldRo/include/Effect/FieldEffectResource_define.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @class  EffectKamitsurugiSlash
*  @brief  カミツルギイベントの斬撃エフェクト
*/
/**
*  @brief  コンストラクタ
*/
EffectKamitsurugiSlash::EffectKamitsurugiSlash(void)
: IEffectBase()
, m_pEffectHeap       (NULL)
, m_pEffectSystem     (NULL)
, m_pEffectHandle     (NULL)
, m_fScaleCoeff       (1.0f)
, m_sResorceID        (-1)
{
}

/**
*  @brief  デストラクタ
*/
EffectKamitsurugiSlash::~EffectKamitsurugiSlash(void)
{
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Public Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  初期化
*/
void EffectKamitsurugiSlash::Initialize(SetupData &setupData)
{
  m_vPosition         = setupData.position;
  m_vRotation         = setupData.rotation;
  m_fScaleCoeff       = setupData.fScaleCoeff;
  m_pEffectHeap       = setupData.pEffectHeap;
  m_pEffectSystem     = setupData.pEffectSystem;
  m_sResorceID        = setupData.resorceID;
  m_pEffectHandle     = GFL_NEW(m_pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle;
  
  m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, m_sResorceID);

  if (m_pEffectHandle && m_pEffectHandle->GetEmitterSet())
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
  }

}

/**
*  @brief  破棄
*/
bool EffectKamitsurugiSlash::Terminate(void)
{
  End();
  GFL_SAFE_DELETE(m_pEffectHandle);
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
void EffectKamitsurugiSlash::Start(void)
{
  if (!IsAlive())
  {
    m_pEffectSystem->CreateEmitterSetID(m_pEffectHandle, m_vPosition, 0, m_sResorceID);
  }
}

/**
*  @brief  停止
*/
void EffectKamitsurugiSlash::End(void)
{
  if (m_pEffectHandle->GetEmitterSet())
  {
    m_pEffectHandle->GetEmitterSet()->Kill();
  }
}

/**
*  @brief  再生確認
*/
bool EffectKamitsurugiSlash::IsAlive(void)
{
  if (!m_pEffectHandle->GetEmitterSet())             { return false; }
  if (!m_pEffectHandle->GetEmitterSet()->IsAlive())  { return false; }

  return true;
}

//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
// Protected Functions
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
/**
*  @brief  更新処理
*/
void EffectKamitsurugiSlash::updateNormal(void)
{

}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)