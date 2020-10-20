#if !defined(EASYMODELDEBUG_H_INCLUDED)
#define EASYMODELDEBUG_H_INCLUDED
#pragma once

#if defined(GF_PLATFORM_CTR)
#if defined(NN_SWITCH_ENABLE_HOST_IO)
#if PM_DEBUG
//==============================================================================
/**
 * @file   EasyModelDebug.h
 * @date   2016/09/15 Thu. 12:24:48
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

#include  <Debug/DebugWin/include/DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>

#include  "../EasyModel.h"

namespace Test  {
namespace MutoTest  {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/09/15 Thu. 12:57:29
  */
//==============================================================================
class EasyModel::EasyModelDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(EasyModelDebug);
public:
  EasyModelDebug(void);
  virtual ~EasyModelDebug();

  void  SetOwner(EasyModel* pOwner) {m_pOwner = pOwner;}
  void  AddTo(gfl2::debug::DebugWinGroup* pGroup, gfl2::heap::HeapBase* pHeapBase);

protected:
  EasyModel*                    m_pOwner;
  gfl2::debug::DebugWinGroup*   m_pGroup_Root;

};



} /*  namespace MutoTest  */
} /*  namespace Test      */
#endif  /*  #if defined(NN_SWITCH_ENABLE_HOST_IO)  */
#endif  /*  #if PM_DEBUG                  */
#endif  /*  #if defined(GF_PLATFORM_CTR)    */
#endif  /*  #if !defined(EASYMODELDEBUG_H_INCLUDED)  */
