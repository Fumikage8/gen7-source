#if !defined(JOINFESTAEFFECTSYSTEMDEBUG_H_INCLUDED)
#define JOINFESTAEFFECTSYSTEMDEBUG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaEffectSystemDebug.h
 * @date   2015/10/22 Thu. 12:22:13
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
/*  gfl2  */
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
//#include  <heap/include/gfl2_Heap.h>
//#include  <fs/include/gfl2_Fs.h>
//#include  <str/include/gfl2_Str.h>
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>


/*  JoinFesta  */
#include  "../JoinFestaDefine.h"


// =============================================================================
/**/
//==============================================================================
namespace Field {
  namespace Effect  {
    class EffectFesMessage;
  }
}



namespace NetApp    {
namespace JoinFesta {
  class JoinFestaEffectSystem;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/22 Thu. 12:24:08
  */
//==============================================================================
#ifdef JF_DEBUG_MENU
class JoinFestaEffectSystemDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaEffectSystemDebug);
public:
  JoinFestaEffectSystemDebug(gfl2::heap::HeapBase* pHeapBase, JoinFestaEffectSystem* pEffectSystem, gfl2::debug::DebugWinGroup* pParentGroup = NULL);
  virtual ~JoinFestaEffectSystemDebug();


protected:
  class DebugFuncs;

  gfl2::heap::HeapBase*             m_pHeapBase;
  JoinFestaEffectSystem*            m_pEffectSystem;
  gfl2::debug::DebugWinGroup*       m_pMyGroup;


  Field::Effect::EffectFesMessage*  m_pFesMessage;
};
#endif  /*  JF_DEBUG_MENU */



} /*  namespace JoinFesta */
} /*  namespace NetApp    */
#endif  /*  #if !defined(JOINFESTAEFFECTSYSTEMDEBUG_H_INCLUDED)  */
