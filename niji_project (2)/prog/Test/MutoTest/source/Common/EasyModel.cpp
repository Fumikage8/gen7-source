
#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyModel.cpp
 * @date   2016/09/07 Wed. 15:57:45
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
#include  <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include  <model/include/gfl2_BaseModel.h>

#include  "./Debug/EasyModelDebug.h"
#include  "./EasyModel.h"


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
EasyModel::EasyModel(void)
  : m_BaseModel()
  , m_pData(NULL)
  , m_pResourceNode(NULL)
  /*  debug  */
  , m_pEasyModelDebug(NULL)
  , m_pDebugGroup(NULL)
  , m_Hash(0u)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyModel::~EasyModel()
{
  Release();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyModel::Create(const void* pResourceData, app::util::Heap* pAppUtilHeap, const bool bResourceDeleter)
{
  gfl2::heap::HeapBase*         pHeapBase         = pAppUtilHeap->GetDeviceHeap();
  gfl2::util::GLHeapAllocator*  pGLHeapAllocator  = pAppUtilHeap->GetDevGLHeapAllocator();


  Release();
  m_pData = bResourceDeleter ? pResourceData : NULL;
  {
    gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack  dataPack;

    dataPack.SetModelData((c8*)pResourceData);

    m_pResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(pGLHeapAllocator, &dataPack);
  }


  /**
   * @brief リソースノードから作成
   *
   * @param pGfxAllocator Gfxライブラリ用のアロケータ
   * @param pAnimationHeap Animationライブラリ用のヒープ
   * @param pModelResource ベースモデルリソース
   * @param pMotionConfigResource モーションコンフィグリソース
   *
   * @return 作成成功したらtrue, 失敗したらfalse
   */
  /*
  bool Create(
    gfl2::gfx::IGLAllocator* pGfxAllocator,
    gfl2::heap::HeapBase* pAnimationHeap,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pModelResource,
    gfl2::renderingengine::scenegraph::resource::ResourceNode* pMotionConfigResource = NULL);
  */
  m_BaseModel.Create(pGLHeapAllocator, pHeapBase, m_pResourceNode);
  m_BaseModel.SetVisible(true);
  m_BaseModel.GetModelInstanceNode()->SetDrawCheckListener(false);

  /*  debug  */
  {
    m_pEasyModelDebug = GFL_NEW(pHeapBase) EasyModelDebug();
    m_pEasyModelDebug->SetOwner(this);
    SetHash();
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyModel::Release(void)
{
  /*  debug */
  {
    GFL_SAFE_DELETE(m_pEasyModelDebug);
  }
  
  m_BaseModel.Destroy();
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
void  EasyModel::AddDebug(gfl2::debug::DebugWinGroup* pGroup, gfl2::heap::HeapBase* pHeapBase)
{
  m_pEasyModelDebug->AddTo(pGroup, pHeapBase);
  m_pDebugGroup = pGroup;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyModel::RemoveDebug(void)
{
  if(m_pDebugGroup)
  {
    gfl2::debug::DebugWin_RemoveGroup(m_pDebugGroup);
    m_pDebugGroup = NULL;
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
void  EasyModel::Update(void)
{
  m_BaseModel.UpdateAnimation();
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyModel::SetMotion(const u32 slot, app::util::Heap* pAppUtilHeap, const void* pData, const u32 motionHash)
{
  m_BaseModel.ChangeAnimationByResourceNode(NULL, 0);
  m_Motions[0].Release();
  m_Motions[0].Create(pData, pAppUtilHeap, true);
  m_BaseModel.ChangeAnimationByResourceNode(m_Motions[0].GetResourceNode(), 0);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyModel::UnsetMotion(const u32 motionHash)
{
  for(u32 index = 0; index < GFL_NELEMS(m_Motions); ++index)
  {
    if(m_Motions[index].GetHash() == motionHash)
    {
      m_BaseModel.ChangeAnimationByResourceNode(NULL, 0);
      m_Motions[index].Release();
      break;
    }
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
EasyModelPool::EasyModelPool(gfl2::heap::HeapBase* pHeapBase, const u32 capacity)
  : m_pEasyModels(NULL)
  , m_BusyMap(pHeapBase, capacity)
  , m_FreeList(pHeapBase, capacity)
  , m_GarbageList(pHeapBase, capacity)
{
  m_pEasyModels = GFL_NEW(pHeapBase) EasyModel[capacity];

  for(u32 index = 0; index < capacity; ++index)
  {
    m_FreeList.PushBack(&(m_pEasyModels[index]));
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
EasyModelPool::~EasyModelPool()
{
  GFL_SAFE_DELETE_ARRAY(m_pEasyModels);
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void EasyModelPool::Update(void)
{
  /*  garbage  */
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

  /*    */
  {
    gfl2::util::Map<EasyModel*, EasyModel*>::Iterator itr = m_BusyMap.Begin();
    gfl2::util::Map<EasyModel*, EasyModel*>::Iterator end = m_BusyMap.End();
    
    while(itr != end)
    {
      itr->second->Update();
      ++itr;
    }
  }
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
EasyModel*  EasyModelPool::Get(void)
{
  EasyModel*  pEasyModel  = NULL;

  if(m_FreeList.Size())
  {
    pEasyModel  = *(m_FreeList.Begin());
    m_FreeList.PopFront();
    pEasyModel->SetHash();
    m_BusyMap.Insert(gfl2::util::Pair<EasyModel*, EasyModel*>(pEasyModel, pEasyModel));
  }

  return pEasyModel;
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  EasyModelPool::Dispose(EasyModel* pTarget)
{
  gfl2::util::Map<EasyModel*, EasyModel*>::Iterator itrBusy = m_BusyMap.Find(pTarget);
  
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
void  EasyModelPool::DisposeAll(void)
{
  const u32                                               capacity  = m_FreeList.MaxSize();
  const gfl2::util::Map<EasyModel*, EasyModel*>::Iterator itrEnd    = m_BusyMap.End();

  for(u32 index = 0; index < capacity; ++index)
  {
    Dispose(&m_pEasyModels[index]);
  }
}




//------------------------------------------------------------------
/**
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
EasyModel*  EasyModelPool::FindBusyModel(const u32 hash)
{
  const gfl2::util::Map<EasyModel*, EasyModel*>::Iterator itrEnd    = m_BusyMap.End();
  const u32   capacity      = m_FreeList.MaxSize();
  EasyModel*  pCurrentModel = &m_pEasyModels[0];
  EasyModel*  pFoundModel   = NULL;
  
  
  for(u32 index = 0; index < capacity; ++index)
  {
    if(pCurrentModel->GetHash() == hash)
    {
      if(m_BusyMap.Find(pCurrentModel) != itrEnd)
      {
        pFoundModel = pCurrentModel;
        break;
      }
    }
    pCurrentModel++;
  }
  

  return pFoundModel;
}


} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
