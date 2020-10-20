#if !defined(EGGHATCHINGPROCPARAM_H_INCLUDED)
#define EGGHATCHINGPROCPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingProcParam.h
 * @date   2015/11/24 Tue. 12:49:09
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <pml/include/personal/pml_PersonalConst.h>
#include  <pml/include/pokepara/pml_PokemonParam.h>
#include  <pml/include/pokepara/pml_PokemonParamType.h>


namespace NetApp      {
namespace EggHatching {
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  APP_PARAM
  *  @brief  孵化Proc引数及び結果
  *  @note   関数の実装はNetStatic側
  */
//==============================================================================
typedef struct APP_PARAM
{
  /*  孵化関連  */
  pml::pokepara::PokemonParam*    m_pTargetEgg;    /**<  [in]  デモに使用するタマゴデータ          */
  pml::pokepara::PokemonParam*    m_pTargetMons;   /**<  [in]  デモに使用する孵化モンスターデータ  */


  /*  ctor  */
  APP_PARAM(void)
    : m_pTargetEgg(NULL)
    , m_pTargetMons(NULL)
  {}

  /** 対象タマゴがアローラ種であるか  */
  bool  TargetMonsIsAlolaSpecies(void) const
  {
    pml::personal::LoadPersonalData(m_pTargetMons->GetMonsNo(), m_pTargetMons->GetFormNo());

    return  (pml::personal::GetPersonalParam(pml::personal::PARAM_ID_region) == pml::personal::REGION_ALOLA);
  }

}APP_PARAM;



} /*  namespace EggHatching */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EGGHATCHINGPROCPARAM_H_INCLUDED)  */
