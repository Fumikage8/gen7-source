
#if PM_DEBUG

#if !defined(NETAPPPOKEMODELUTILITYDEBUG_H_INCLUDED)
#define NETAPPPOKEMODELUTILITYDEBUG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   NetAppPokeModelUtilityDebug.h
 * @date   2016/06/01 Wed. 12:35:26
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include <macro/include/gfl2_macros.h>
#include <Layout/include/gfl2_Layout.h>
#include <heap/include/gfl2_Heap.h>
#include <Debug/DebugWin/include/DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinSystem.h>
#include <Debug/include/gfl2_DebugWinItemTypes.h>

#include <NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h>
#include <NetStatic/NetAppLib/include/Debug/NetAppDebugPokeToolSimpleParam.h>

namespace NetAppLib {
namespace Util      {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @note   NetAppPokeModelUtilityのinnerClassであり、単体の使用は想定しない
  *  @date   2016/06/01 Wed. 12:36:13
  */
//==============================================================================
class NetAppPokeModelUtility::NetAppPokeModelUtilityDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppPokeModelUtilityDebug);
public:
  NetAppPokeModelUtilityDebug(NetAppPokeModelUtility* pOwner);
  virtual~  NetAppPokeModelUtilityDebug();

  void  OnUpdate(void);
  void  OnDraw(const gfl2::gfx::CtrDisplayNo displayNo);

protected:

  /*
    funcs
  */
  static void First(  void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void Next(   void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void Set(    void* _pThis, gfl2::debug::DebugWinItem* pItem);

  void  ToInfo(void);
  void  FromInfo(void);



  /*
  */
  NetAppPokeModelUtility*       m_pOwner;
  gfl2::debug::DebugWinGroup*   m_pGroup_Root;


  /*  items  */
  enum  {
    VALITEM_slot,

    VALITEM_monsNo,
    VALITEM_formNo,
    VALITEM_gender,
    VALITEM_rare,
    VALITEM_isEgg,

    VALITEM_isDrawFrame,
    VALITEM_ofsX,
    VALITEM_ofsY,
    VALITEM_ofsSize,


    
    VALITEM_MAX
  };

  struct  ItemInfo
  {
    NetAppPokeModelUtilityDebug*            pThis;
    gfl2::debug::DebugWinItemValueInt<s32>* pViewItem;
    s32                                     value;
  };
  ItemInfo  m_ItemInfos[VALITEM_MAX];


  PokeTool::SimpleParam         m_SimpleParam;
  pml::personal::PersonalData   m_PersonalData;
};



} /*  namespace Util      */
} /*  namespace NetAppLib */
#endif  /*  #if !defined(NETAPPPOKEMODELUTILITYDEBUG_H_INCLUDED)  */

#endif  /*  #if PM_DEMUG  */
