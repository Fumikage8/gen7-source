#if !defined(ROTOMTONEGENERATORDEBUG_H_INCLUDED)
#define ROTOMTONEGENERATORDEBUG_H_INCLUDED
#pragma once
#if PM_DEBUG
//==============================================================================
/**
 * @file   RotomToneGeneratorDebug.h
 * @date   2017/04/27 Thu. 16:51:15
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
#include  <Layout/include/gfl2_Layout.h>
#include  <heap/include/gfl2_Heap.h>
#include  <Debug/DebugWin/include/DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>

#include  "../../../../include/Tool/RotomToneGenerator/app_tool_RotomToneGenerator.h"

namespace app {
namespace tool {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  
  *  @date   2017/04/27 Thu. 16:52:08
  */
//==============================================================================
class RotomToneGenerator::RotomToneGeneratorDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(RotomToneGeneratorDebug);
public:
  RotomToneGeneratorDebug(RotomToneGenerator* pOwner);
  virtual ~RotomToneGeneratorDebug();

  u32 GetRemainFrames(void) const { return m_ItemInfos[VALITEM_interval].value; }


  void  Tone(const u32 toneIndex);
  void  PrintInterval(const u32 seq);

protected:
  /*
  */
  RotomToneGenerator*           m_pOwner;
  gfl2::debug::DebugWinGroup*   m_pGroup_Root;
  u64                           m_IntervalBegin;


  /*  items  */
  enum  {
    VALITEM_tone0,
    VALITEM_tone1,
    VALITEM_tone2,
    VALITEM_tone3,
    VALITEM_interval,
    
    VALITEM_MAX
  };

  struct  ItemInfo
  {
    RotomToneGeneratorDebug*                pThis;
    gfl2::debug::DebugWinItemValueInt<s32>* pViewItem;
    s32                                     value;
  };
  ItemInfo  m_ItemInfos[VALITEM_MAX];

  /*
    funcs
  */
  static void Request(  void* _pThis, gfl2::debug::DebugWinItem* pItem);

  void  FixupTone(void);
};



}  /*  namespace tool  */
}  /*  namespace app  */
#endif  /*  #if PM_DEBUG  */
#endif  /*  #if !defined(ROTOMTONEGENERATORDEBUG_H_INCLUDED)  */
