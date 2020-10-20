#if !defined(HIOINJECTION_H_INCLUDED)
#define HIOINJECTION_H_INCLUDED
#pragma once

#if PM_DEBUG
//==============================================================================
/**
 * @file   HIOInjection.h
 * @date   2016/10/04 Tue. 15:56:06
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
#include  <heap/include/gfl2_Heap.h>

#include  <NetStatic/NetAppLib/include/Debug/NetAppHIOUtil.h>

#include  "../../include/PokeViewerPokeModelManager.h"


namespace test  {
namespace pokeviewer  {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/10/04 Tue. 16:00:41
  */
//==============================================================================
class HIOInjection
{
  GFL_FORBID_COPY_AND_ASSIGN(HIOInjection);
public:
  HIOInjection(void);
  virtual ~HIOInjection();

  void  Initialize(gfl2::heap::HeapBase* pHeapBase, PokeViewerPokeModelManager* pModelManager = NULL);
  void  Finalize(void);
  void  Update(void);

  void  SetModelManager(PokeViewerPokeModelManager* pModelManager)  {m_pModelManager = pModelManager;}

protected:




private:
  PokeViewerPokeModelManager* m_pModelManager;

  class HIOInjection_Impl;
  HIOInjection_Impl*   m_pImpl;
};




} /*  namespace pokevierwer  */
} /*  namespace test  */

#endif // PMM_DEBUG
#endif  /*  #if !defined(HIOINJECTION_H_INCLUDED)  */
