//==============================================================================
/**
 * @file   ResourceProvider.cpp
 * @date   2016/11/14 Mon. 16:10:59
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <thread/include/gfl2_ThreadStatic.h>
#include  <AppLib/include/Tool/GeneralView/Module/ResourceProvider.h>


namespace app   {
namespace tool  {
// =============================================================================
/**/
//==============================================================================


// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
ResourceProvider::ResourceProvider(app::util::Heap* pUtilHeap)
  : m_pUtilHeap(pUtilHeap)
  , m_pLoaders(NULL)
  , m_Loaders(0)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
ResourceProvider::~ResourceProvider()
{
  ReleaseData();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  ResourceProvider::LoadRequest(const ArcTable* pTable, const u32 elems)
{
  GFL_ASSERT(m_pUtilHeap);
  GFL_ASSERT(m_Loaders == 0);

  m_Loaders = elems;
  {
    gfl2::heap::HeapBase* pDevHeapBase = m_pUtilHeap->GetDeviceHeap();
    
    m_pLoaders  = GFL_NEW(pDevHeapBase)  app::tool::ResourceLoader[elems];
    for(u32 index = 0; index < elems; ++index)
    {
      m_pLoaders[index].RequestLoad(m_pUtilHeap, pTable[index].arcID, pTable[index].arcDatID, pTable[index].isCompressed);
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
bool  ResourceProvider::ObserveLoad(void)
{
  bool  isEnd = true;

  for(u32 index = 0; index < m_Loaders; ++index)
  {
    isEnd &= m_pLoaders[index].ObserveLoad();
  }

  return isEnd;
}



//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void    ResourceProvider::ReleaseData(void)
{
  while(!ObserveLoad())
  {
    GFL_ASSERT(false);  /*  読み込み中のメモリ解放抑制 */
    gfl2::thread::ThreadStatic::CurrentSleep(1);
  }

  GFL_SAFE_DELETE_ARRAY(m_pLoaders);
  m_Loaders = 0;
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void*  ResourceProvider::IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID)
{
  if(m_pLoaders)
  {
    for (u32 index = 0; index < m_Loaders; ++index)
    {
      app::tool::ResourceLoader*  pLoader = &m_pLoaders[index];
      if((arcID == pLoader->GetArcID()) && (arcDatID == pLoader->GetArcDatID()))
      {
        return pLoader->GetData();
      }
    }
  }
  
  return NULL;
}





} /*  namespace tool  */
} /*  namespace app   */
