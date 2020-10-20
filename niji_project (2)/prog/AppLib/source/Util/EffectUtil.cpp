//==============================================================================
/**
 @file    EffectUtil.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.14
 @brief   エフェクトを簡便に使うためのクラス
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Util/EffectUtil.h>


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)


EffectUtil::EffectUtil(void)
  : m_desc(),
    m_effectHeap(),
    m_effectSys(),
    m_effectResBufArray(NULL),
    m_dataArray(NULL),
    m_effectHandleArray(NULL)  // @fix cov_ctr[10544]: 未初期化をNULLで初期化
{}

EffectUtil::EffectUtil(gfl2::heap::HeapBase* heap, const Description& desc)
  : m_desc(),
    m_effectHeap(),
    m_effectSys(),
    m_effectResBufArray(NULL),
    m_dataArray(NULL),
    m_effectHandleArray(NULL)
{
  this->Create(heap, desc);
}

EffectUtil::~EffectUtil()
{
  this->Destroy();
}

void EffectUtil::Create(gfl2::heap::HeapBase* heap, const Description& desc)
{
  // Description
  m_desc.layerNum        = desc.layerNum;
  m_desc.effectResNumMax = desc.effectResNumMax;
  m_desc.mode            = desc.mode;
  if(m_desc.layerNum > 0)
  {
    m_desc.effectGroupIDNumMaxArray = GFL_NEW_ARRAY(heap) u32[m_desc.layerNum];
    for(u32 i=0; i<m_desc.layerNum; ++i)
    {
      m_desc.effectGroupIDNumMaxArray[i] = desc.effectGroupIDNumMaxArray[i];
    }
  }

  // GFHeap
  m_effectHeap = GFL_NEW(heap) gfl2::Effect::GFHeap(heap);

  // System
  {
    gfl2::Effect::Config effectConfig;
    {
      effectConfig.SetEffectHeap(m_effectHeap);
#ifdef GF_PLATFORM_CTR
      effectConfig.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);  // デフォルトがVRAMAになっているのでFCRAMに変える。
#endif //GF_PLATFORM_CTR
    }

    m_effectSys = GFL_NEW(heap) gfl2::Effect::System(effectConfig);
  }

  // ResBuf
  if(m_desc.effectResNumMax > 0)
  {
    m_effectResBufArray = GFL_NEW_ARRAY(heap) void*[m_desc.effectResNumMax];
    for(u32 i=0; i<m_desc.effectResNumMax; ++i)
    {
      m_effectResBufArray[i] = NULL;
    }
  }

  // Data
  {
    if(m_desc.layerNum > 0)
    {
      m_dataArray = GFL_NEW_ARRAY(heap) Data[m_desc.layerNum];
      
      for(u32 i=0; i<m_desc.layerNum; ++i)
      {
        m_dataArray[i].effectRenderPathConfig.m_pSystem = m_effectSys;
        if(m_desc.effectGroupIDNumMaxArray[i] > 0)
        {
          m_dataArray[i].effectGroupIDList                      = GFL_NEW(heap) gfl2::util::List<u32>(heap, m_desc.effectGroupIDNumMaxArray[i]);
          m_dataArray[i].effectRenderPathConfig.m_cpGroupIDList = m_dataArray[i].effectGroupIDList;
        }
      }
    }
  }

  // Handle
  if(m_desc.mode == MODE_1RES_1EMITTER_SET_1GROUP)
  {
    if(m_desc.effectResNumMax > 0)
    {
      m_effectHandleArray = GFL_NEW(heap) gfl2::Effect::Handle[m_desc.effectResNumMax];
    }
  }
}

void EffectUtil::Destroy(void)
{
  // Handle
  GFL_SAFE_DELETE_ARRAY(m_effectHandleArray);

  {
    for(u32 i=0; i<m_desc.layerNum; ++i)
    {
      GFL_SAFE_DELETE(m_dataArray[i].effectGroupIDList);
    }
    GFL_SAFE_DELETE_ARRAY(m_dataArray);
  }

  GFL_SAFE_DELETE_ARRAY(m_effectResBufArray);

  GFL_SAFE_DELETE(m_effectSys);

  GFL_SAFE_DELETE(m_effectHeap);

  {
    GFL_SAFE_DELETE_ARRAY(m_desc.effectGroupIDNumMaxArray);
    m_desc.layerNum        = 0;
    m_desc.effectResNumMax = 0;
  }
}

void EffectUtil::EffectSystemEntryResource(void* resBuf, u32 resID, gfl2::Effect::EnvInfo* envInfo)
{
  if(resID < m_desc.effectResNumMax)
  {
    if(m_effectResBufArray[resID] == NULL)
    {
      m_effectSys->EntryResource( m_effectHeap, resBuf, static_cast<s32>(resID), envInfo );
      m_effectResBufArray[resID] = resBuf;
    }
    else
    {
      GFL_ASSERT(0);  // 登録済みのリソースID  // 開発中に気付かせるためのASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
}

void EffectUtil::EffectSystemClearResource(u32 resID)
{
  if( this->GetEffectResBuf(resID) != NULL )
  {
    m_effectSys->ClearResource( m_effectHeap, resID );
    m_effectResBufArray[resID] = NULL;
  }
  else
  {
    GFL_ASSERT(0);  // 登録していないリソースID  // 開発中に気付かせるためのASSERT
  }
}

u32 EffectUtil::GetEffectResID(void* resBuf) const
{
  u32 resID = INVALID_ID;
  for(u32 i=0; i<m_desc.effectResNumMax; ++i)
  {
    if(m_effectResBufArray[i] == resBuf)
    {
      resID = i;
      break;
    }
  }
  return resID;
}

void* EffectUtil::GetEffectResBuf(u32 resID) const
{
  void* resBuf = NULL;
  if(resID < m_desc.effectResNumMax)
  {
    resBuf = m_effectResBufArray[resID];
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
  return resBuf;
}

void EffectUtil::CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group(void)
{
  if(m_desc.mode == MODE_1RES_1EMITTER_SET_1GROUP)
  {
    if(m_effectSys)
    {
      for(u32 i=0; i<m_desc.effectResNumMax; ++i)
      {
        if(m_effectResBufArray[i] != NULL)
        {
          u32 emitterSetID = 0;  // 1リソースに1エミッターセットなので、エミッターセットのIDは0しかない。
          u8  groupID      = i;
          m_effectSys->CreateEmitterSetID( &(m_effectHandleArray[i]), gfl2::math::Vector3(0,0,0), emitterSetID, i, groupID );
        }
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // モードが違う  // 開発中に気付かせるためのASSERT
  }
}

void EffectUtil::KillAllEmitterSetForMode1Res1EmitterSet1Group(void)
{
  if(m_desc.mode == MODE_1RES_1EMITTER_SET_1GROUP)
  {
    if(m_effectSys)
    {
      for(u32 i=0; i<m_desc.effectResNumMax; ++i)
      {
        if(m_effectHandleArray[i].IsValid())
        {
          if(m_effectHandleArray[i].GetEmitterSet() != NULL)  // EffectのHnadleが保有しているEmitterSetのポインタが別のものになる場合への対応
          {
            m_effectSys->GetNwObject()->KillEmitterSet( m_effectHandleArray[i].GetEmitterSet() );
            m_effectHandleArray[i].Invalidate();
          }
        }
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // モードが違う  // 開発中に気付かせるためのASSERT
  }
}

void EffectUtil::UpdateForMode1Res1EmitterSet1Group(void)
{
  if(m_desc.mode == MODE_1RES_1EMITTER_SET_1GROUP)
  {
    if(m_effectSys)
    {
      m_effectSys->BeginFrame();

      for(u32 i=0; i<m_desc.effectResNumMax; ++i)
      {
        if(m_effectResBufArray[i] != NULL)
        {
          u8 groupID = i;
          m_effectSys->Calc(groupID);
        }
      }
    }
  }
  else
  {
    GFL_ASSERT(0);  // モードが違う  // 開発中に気付かせるためのASSERT
  }
}

gfl2::Effect::Handle* EffectUtil::GetEffectHandleForMode1Res1EmitterSet1Group(u32 resID) const
{
  gfl2::Effect::Handle* handle = NULL;
  if(m_desc.mode == MODE_1RES_1EMITTER_SET_1GROUP)
  {
    if(resID < m_desc.effectResNumMax)
    {
      handle = &(m_effectHandleArray[resID]);
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
    }
  }
  else
  {
    GFL_ASSERT(0);  // モードが違う  // 開発中に気付かせるためのASSERT
  }
  return handle;
}

gfl2::Effect::System* EffectUtil::GetEffectSystem(void) const
{
  return m_effectSys;
}

EffectUtil::Data* EffectUtil::GetData(u32 layerIndex) const
{
  EffectUtil::Data* data = NULL;
  if(layerIndex < m_desc.layerNum)
  {
    data = &m_dataArray[layerIndex];
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT
  }
  return data;
}


GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

