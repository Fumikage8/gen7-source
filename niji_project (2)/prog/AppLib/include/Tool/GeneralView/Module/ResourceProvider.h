#if !defined(RESOURCEPROVIDER_H_INCLUDED)
#define RESOURCEPROVIDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   ResourceProvider.h
 * @date   2016/11/14 Mon. 16:07:41
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
#include  <fs/include/gfl2_Fs.h>
#include  <heap/include/gfl2_Heap.h>

#include  <AppLib/include/Util/app_util_heap.h>
#include  <AppLib/include/Tool/GeneralView/Module/IResourceProvider.h>
#include  <AppLib/source/Tool/GeneralView/Module/ResourceLoader.h>


namespace app   {
namespace tool  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/11/14 Mon. 15:13:38
  */
//==============================================================================
class ResourceProvider
  : public app::tool::IResourceProvider
{
  GFL_FORBID_COPY_AND_ASSIGN(ResourceProvider);
public:
  struct  ArcTable
  {
    gfl2::fs::ArcFile::ARCID    arcID;
    gfl2::fs::ArcFile::ARCDATID arcDatID;
    bool                        isCompressed;
  };


  explicit ResourceProvider(app::util::Heap* pUtilHeap=NULL);
  virtual ~ResourceProvider();

  void  SetHeap(app::util::Heap* pUtilHeap) {m_pUtilHeap = pUtilHeap;}    /**<  ctorで渡せない場合  */
  void  LoadRequest(const ArcTable* pTable, const u32 elems);
  bool  ObserveLoad(void);
  void  ReleaseData(void);

  virtual void*  IResourceProvider_OnResourceRequest(const gfl2::fs::ArcFile::ARCID arcID, const gfl2::fs::ArcFile::ARCDATID arcDatID);

protected:
  ResourceProvider(gfl2::heap::HeapBase* pHeapBase){}

  app::util::Heap*            m_pUtilHeap;
  app::tool::ResourceLoader*  m_pLoaders;
  u32                         m_Loaders;
};




} /*  namespace tool  */
} /*  namespace app   */
#endif  /*  #if !defined(RESOURCEPROVIDER_H_INCLUDED)  */
