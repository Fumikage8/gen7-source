#if !defined(EASYMODEL_H_INCLUDED)
#define EASYMODEL_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyModel.h
 * @date   2016/09/07 Wed. 15:50:49
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

#include  "./EasyMotion.h"


namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/07 Wed. 15:52:04
  */
//==============================================================================
class EasyModel
{
  GFL_FORBID_COPY_AND_ASSIGN(EasyModel);
public:
  EasyModel(void);
  virtual ~EasyModel();

  void  Create(const void* pResourceData, app::util::Heap* pAppUtilHeap, const bool bResourceDeleter = false);
  void  Release(void);
  void  Update(void);

  poke_3d::model::BaseModel*  GetBaseModel(void)  {return &m_BaseModel;}

  /*  motion  */
  void  SetMotion(const u32 slot, app::util::Heap* pAppUtilHeap, const void* pData, const u32 motionHash);
  void  UnsetMotion(const u32 motionHash);


  /*  debug  */
  void  AddDebug(gfl2::debug::DebugWinGroup* pGroup, gfl2::heap::HeapBase* pHeapBase);
  void  RemoveDebug(void);

  /*  hash  */
  void  SetHash(const u32 hash = 0u)                                {m_Hash = hash;}
  void  SetHash(const u8* pHashSource, const u32 sourceDataBytes)   {SetHash(gfl2::util::FnvHash::FnvHash(pHashSource, sourceDataBytes).value);}
  u32   GetHash(void) const                                         {return m_Hash;}


protected:
  poke_3d::model::BaseModel                                       m_BaseModel;
  const void*                                                     m_pData;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*      m_pResourceNode;
  /*  motion  */
  EasyMotion                                                      m_Motions[1];

  /*  debug  */
  class EasyModelDebug;
  EasyModelDebug*                                                 m_pEasyModelDebug;
  gfl2::debug::DebugWinGroup*                                     m_pDebugGroup;
  u32                                                             m_Hash;
};


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/08 Thu. 12:06:46
  */
//==============================================================================
class EasyModelPool
{
  GFL_FORBID_COPY_AND_ASSIGN(EasyModelPool);
public:
  EasyModelPool(gfl2::heap::HeapBase* pHeapBase, const u32 capacity);
  virtual ~EasyModelPool();

  void        Update(void);

  EasyModel*  Get(void);
  void        Dispose(EasyModel* pTarget);
  void        DisposeAll(void);

  EasyModel*  FindBusyModel(const u32 hash = 0u);

protected:
  struct SGarbageInfo
  {
    EasyModel*  pTarget;
    u32         life;
    
    SGarbageInfo(void)
      : pTarget(NULL)
      , life(0)
    {}
    SGarbageInfo(EasyModel* _pTarget, u32 _life)
      : pTarget(_pTarget)
      , life(_life)
    {}
  };

  EasyModel*                                m_pEasyModels;
  gfl2::util::Map<EasyModel*, EasyModel*>   m_BusyMap;
  gfl2::util::List<EasyModel*>              m_FreeList;
  gfl2::util::List<SGarbageInfo>            m_GarbageList;
};




} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(EASYMODEL_H_INCLUDED)  */
