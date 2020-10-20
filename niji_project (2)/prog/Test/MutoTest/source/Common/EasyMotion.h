
#if !defined(EASYMOTION_H_INCLUDED)
#define EASYMOTION_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyMotion.h
 * @date   2016/09/23 Fri. 11:55:39
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
#include  <util/include/gfl2_Map.h>
#include  <util/include/gfl2_List.h>
#include  <util/include/gfl2_FnvHash.h>
#include  <model/include/gfl2_BaseModel.h>

#include  <Debug/DebugWin/include/DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>

#include  <AppLib/include/Util/app_util_heap.h>


namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/23 Fri. 11:56:02
  */
//==============================================================================
class EasyMotion
{
  GFL_FORBID_COPY_AND_ASSIGN(EasyMotion);
public:
  EasyMotion(void);
  virtual ~EasyMotion();

  void  Create(const void* pResourceData, app::util::Heap* pAppUtilHeap, const bool bResourceDeleter = false);
  void  Release(void);

  gfl2::renderingengine::scenegraph::resource::ResourceNode*  GetResourceNode(void)  {return m_pResourceNode;}

  /*  debug  */
//  void  AddDebug(gfl2::debug::DebugWinGroup* pGroup, gfl2::heap::HeapBase* pHeapBase);
//  void  RemoveDebug(void);

  void  SetHash(const u32 hash = 0u)                                {m_Hash = hash;}
  void  SetHash(const u8* pHashSource, const u32 sourceDataBytes)   {SetHash(gfl2::util::FnvHash::FnvHash(pHashSource, sourceDataBytes).value);}
  u32   GetHash(void) const                                         {return m_Hash;}


protected:
//  poke_3d::model::BaseModel                                       m_BaseModel;
  const void*                                                     m_pData;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pResourceNode;
  /*  debug  */
//  class EasyModelDebug;
//  EasyModelDebug*                                                 m_pEasyModelDebug;
//  gfl2::debug::DebugWinGroup*                                     m_pDebugGroup;
  u32                                                             m_Hash;
};


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/08 Thu. 12:06:46
  */
//==============================================================================
class EasyMotionPool
{
  GFL_FORBID_COPY_AND_ASSIGN(EasyMotionPool);
public:
  EasyMotionPool(gfl2::heap::HeapBase* pHeapBase, const u32 capacity);
  virtual ~EasyMotionPool();

  void        Update(void);

  EasyMotion* Get(void);
  void        Dispose(EasyMotion* pTarget);
  void        DisposeAll(void);

  EasyMotion*  FindBusyMotion(const u32 hash = 0u);
  EasyMotion*  FindBusyMotion(const u8* pHashSource, const u32 sourceDataBytes) {return FindBusyMotion(gfl2::util::FnvHash::FnvHash(pHashSource, sourceDataBytes).value);}

protected:
  struct SGarbageInfo
  {
    EasyMotion*  pTarget;
    u32         life;
    
    SGarbageInfo(void)
      : pTarget(NULL)
      , life(0)
    {}
    SGarbageInfo(EasyMotion* _pTarget, u32 _life)
      : pTarget(_pTarget)
      , life(_life)
    {}
  };

  EasyMotion*                                 m_pEasyMotions;
  gfl2::util::Map<EasyMotion*, EasyMotion*>   m_BusyMap;
  gfl2::util::List<EasyMotion*>               m_FreeList;
  gfl2::util::List<SGarbageInfo>              m_GarbageList;
};




} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(EASYMOTION_H_INCLUDED)  */
