//======================================================================
/**
 * @file EffectPartical.cpp
 * @date 2016/09/12 16:40:17
 * @author fang_yicheng
 * @brief パーティクル型エフェクト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "EffectPartical.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)
GFL_NAMESPACE_BEGIN(Effect)

EffectPartical::EffectPartical() :
  m_pEffectSystem(NULL),
  m_pHandle(NULL),
  m_resID(-1)
{
}

EffectPartical::~EffectPartical()
{
}

void EffectPartical::Initialize(u32 resId, gfl2::Effect::System* pEffectSystem, gfl2::Effect::GFHeap* pEffectHeap)
{
  m_resID = resId;
  m_pEffectSystem = pEffectSystem;
  m_pHandle = GFL_NEW(pEffectHeap->GetGFHeapBase()) gfl2::Effect::Handle();
}

void EffectPartical::Terminate()
{
  Kill();
  GFL_SAFE_DELETE(m_pHandle);
}

bool EffectPartical::Start()
{
  nw::eft::EmitterSet* pEmitterSet = m_pHandle->GetEmitterSet();
  bool bCreate = true;
  if(pEmitterSet != NULL)
  {
    if(!pEmitterSet->IsAlive() || pEmitterSet->IsFadeRequest())
    {
      pEmitterSet->Kill();
    }
    else
    {
      bCreate = false;
    }
  }
  if(bCreate)
  {
    m_pEffectSystem->CreateEmitterSetID(m_pHandle, gfl2::math::Vector3(), 0, m_resID);
  }
  return bCreate;
}

void EffectPartical::End()
{
  nw::eft::EmitterSet* pEmitterSet = m_pHandle->GetEmitterSet();
  if(pEmitterSet != NULL && pEmitterSet->IsAlive() && !pEmitterSet->IsFadeRequest())
  {
    pEmitterSet->Fade();
  }
}

void EffectPartical::Kill()
{
  if(m_pHandle && m_pHandle->GetEmitterSet())
  {
    m_pHandle->GetEmitterSet()->Kill();
  }
}

void EffectPartical::SetVisible(bool visible)
{
  if(m_pHandle && m_pHandle->GetEmitterSet()) m_pHandle->GetEmitterSet()->SetStopDraw(!visible);
}

bool EffectPartical::IsAlive()
{
  return m_pHandle && m_pHandle->GetEmitterSet() && m_pHandle->GetEmitterSet()->IsAlive();
}

void EffectPartical::Stop()
{
  if(m_pHandle && m_pHandle->GetEmitterSet())
  {
    m_pHandle->GetEmitterSet()->SetStopCalc(true);
  }
}

void EffectPartical::Resume()
{
  if(m_pHandle && m_pHandle->GetEmitterSet())
  {
    m_pHandle->GetEmitterSet()->SetStopCalc(false);
  }
}

void EffectPartical::UpdatePosition(const gfl2::math::SRT &srt)
{
  if(m_pHandle && m_pHandle->GetEmitterSet())
  {
    // trans
    nw::math::VEC3 nwPos(srt.translate.x, srt.translate.y, srt.translate.z);
    // rot
    nw::math::VEC3 nwRot(srt.rotation.x, srt.rotation.y, srt.rotation.z);
    // scale
    nw::math::VEC3 nwScale(srt.scale.x, srt.scale.y, srt.scale.z);
    // 設定
    nw::math::MTX34 srtMtx;
#if defined(GF_PLATFORM_CTR)
    nw::eft::MTX34MakeSRT(&srtMtx, nwScale, nwRot, nwPos);
#elif defined(GF_PLATFORM_WIN32)
    nw::math::MTX34MakeSRT(&srtMtx, nwScale, nwRot, nwPos);
#endif
    m_pHandle->GetEmitterSet()->SetMtx(srtMtx);
  }
}

gfl2::Effect::Handle* EffectPartical::GetHandle()
{
  return m_pHandle;
}

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
