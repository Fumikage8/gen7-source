
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyMotion.cpp
 * @date   2016/09/23 Fri. 15:07:06
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <PokeTool/include/PokeModelSystem.h>
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include  <model/include/gfl2_BaseModel.h>

//#include  "./Debug/EasyMotionDebug.h"
#include  "./EasyMotion.h"


namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyMotion::EasyMotion(void)
  : m_pData(NULL)
  , m_pResourceNode(NULL)
  /*  debug  */
//  , m_pEasyMotionDebug(NULL)
//  , m_pDebugGroup(NULL)
  , m_Hash(0u)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyMotion::~EasyMotion()
{
  Release();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyMotion::Create(const void* pResourceData, app::util::Heap* pAppUtilHeap, const bool bResourceDeleter)
{
  gfl2::heap::HeapBase*         pHeapBase         = pAppUtilHeap->GetDeviceHeap();
  gfl2::util::GLHeapAllocator*  pGLHeapAllocator  = pAppUtilHeap->GetDevGLHeapAllocator();


  Release();
  m_pData = bResourceDeleter ? pResourceData : NULL;
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMotData      binMotData;

    binMotData.SetMotionData((c8*)pResourceData);

    m_pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(pGLHeapAllocator, &binMotData);
  }


  /*  debug  */
/*
  {
    m_pEasyMotionDebug = GFL_NEW(pHeapBase) EasyMotionDebug();
    m_pEasyMotionDebug->SetOwner(this);
    SetHash();
  }
*/
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyMotion::Release(void)
{
  /*  debug */
  {
//    GFL_SAFE_DELETE(m_pEasyMotionDebug);
  }
  
  GFL_SAFE_DELETE(m_pResourceNode);

  if(m_pData)
  {
    void* pData = const_cast<void*>(m_pData);
    GflHeapFreeMemory(pData);
    m_pData = NULL;
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*
void  EasyMotion::AddDebug(gfl2::debug::DebugWinGroup* pGroup, gfl2::heap::HeapBase* pHeapBase)
{
  m_pEasyMotionDebug->AddTo(pGroup, pHeapBase);
  m_pDebugGroup = pGroup;
}
*/

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
/*
void  EasyMotion::RemoveDebug(void)
{
  if(m_pDebugGroup)
  {
    gfl2::debug::DebugWin_RemoveGroup(m_pDebugGroup);
    m_pDebugGroup = NULL;
  }
}
*/

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
EasyMotionPool::EasyMotionPool(gfl2::heap::HeapBase* pHeapBase, const u32 capacity)
  : m_pEasyMotions(NULL)
  , m_BusyMap(pHeapBase, capacity)
  , m_FreeList(pHeapBase, capacity)
  , m_GarbageList(pHeapBase, capacity)
{
  m_pEasyMotions = GFL_NEW(pHeapBase) EasyMotion[capacity];

  for(u32 index = 0; index < capacity; ++index)
  {
    m_FreeList.PushBack(&(m_pEasyMotions[index]));
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
EasyMotionPool::~EasyMotionPool()
{
  GFL_SAFE_DELETE_ARRAY(m_pEasyMotions);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void EasyMotionPool::Update(void)
{
  gfl2::util::List<SGarbageInfo>::Iterator        itr = m_GarbageList.Begin();
  const gfl2::util::List<SGarbageInfo>::Iterator  end = m_GarbageList.End();

  while(itr != end)
  {
    if(itr->life) --(itr->life);
    if(!(itr->life))
    {
      m_FreeList.PushBack(itr->pTarget);
      itr = m_GarbageList.Erase(itr);
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyMotion*  EasyMotionPool::Get(void)
{
  EasyMotion*  pEasyMotion  = NULL;

  if(m_FreeList.Size())
  {
    pEasyMotion  = *(m_FreeList.Begin());
    m_FreeList.PopFront();
    pEasyMotion->SetHash();
    m_BusyMap.Insert(gfl2::util::Pair<EasyMotion*, EasyMotion*>(pEasyMotion, pEasyMotion));
  }

  return pEasyMotion;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyMotionPool::Dispose(EasyMotion* pTarget)
{
  gfl2::util::Map<EasyMotion*, EasyMotion*>::Iterator itrBusy = m_BusyMap.Find(pTarget);
  
  if(itrBusy != m_BusyMap.End())
  {
    m_BusyMap.Erase(itrBusy);
    m_GarbageList.PushBack(SGarbageInfo(itrBusy->first, 2));
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  EasyMotionPool::DisposeAll(void)
{
  const u32                                               capacity  = m_FreeList.MaxSize();
  const gfl2::util::Map<EasyMotion*, EasyMotion*>::Iterator itrEnd    = m_BusyMap.End();

  for(u32 index = 0; index < capacity; ++index)
  {
    Dispose(&m_pEasyMotions[index]);
  }
}




//------------------------------------------------------------------
/**
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
EasyMotion*  EasyMotionPool::FindBusyMotion(const u32 hash)
{
  const gfl2::util::Map<EasyMotion*, EasyMotion*>::Iterator itrEnd    = m_BusyMap.End();
  const u32   capacity      = m_FreeList.MaxSize();
  EasyMotion*  pCurrentMotion = &m_pEasyMotions[0];
  EasyMotion*  pFoundMotion   = NULL;
  
  
  for(u32 index = 0; index < capacity; ++index)
  {
    if(pCurrentMotion->GetHash() == hash)
    {
      if(m_BusyMap.Find(pCurrentMotion) != itrEnd)
      {
        pFoundMotion = pCurrentMotion;
        break;
      }
    }
    pCurrentMotion++;
  }
  

  return pFoundMotion;
}



} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
