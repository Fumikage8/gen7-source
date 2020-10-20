
#if PM_DEBUG

#if !defined(NETAPPDEBUGPOKETOOLSIMPLEPARAM_H_INCLUDED)
#define NETAPPDEBUGPOKETOOLSIMPLEPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppDebugPokeToolSimpleParam.h
 * @date   2016/05/31 Tue. 17:03:22
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

#include  <pml/include/personal/pml_PersonalData.h>

#include  <PokeTool/include/PokeTool.h>
#include  <PokeTool/include/PokeModelSystem.h>



namespace NetAppLib   {
namespace Debug       {
// =============================================================================
/**/
//==============================================================================

//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/05/31 Tue. 17:04:35
  */
//==============================================================================
class PokeToolSimpleParamDebugger
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeToolSimpleParamDebugger);
public:
  PokeToolSimpleParamDebugger(gfl2::heap::HeapBase* pHeapBase, PokeTool::PokeModelSystem* pPokeModelSystem = NULL)  {}
  virtual ~PokeToolSimpleParamDebugger()  {}


  /*
    pml::personal::LoadPersonalData(MonsNo monsno, FormNo formno, PersonalData* buffer);
    を使用する版
  */
  static void   SetFirst(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData = NULL);
  static bool   SetNext(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData = NULL, PokeTool::PokeModelSystem* pPokeModelSystem = NULL);
  static void   Set(PokeTool::SimpleParam& rSimpleParam, pml::personal::PersonalData* pPersonalData = NULL, PokeTool::PokeModelSystem* pPokeModelSystem = NULL);


protected:
  PokeTool::SimpleParam   m_SimpleParam;
  
  /*    */
  PokeTool::SimpleParam*  m_pSimpleParams;
  u32                     m_SimpleParamNum;
  u32                     m_SimpleParamIndex;
};




} /*  namespace Debug       */
} /*  namespace NetAppLib   */
#endif  /*  #if !defined(NETAPPDEBUGPOKETOOLSIMPLEPARAM_H_INCLUDED)  */

#endif /* PM_DEBUG  */
