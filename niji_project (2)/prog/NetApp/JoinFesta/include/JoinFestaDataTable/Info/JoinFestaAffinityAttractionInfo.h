#if !defined(JOINFESTA_AFFINITYATTRACTION_INFO_H_INCLUDED)
#define JOINFESTA_AFFINITYATTRACTION_INFO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaAffinityAttractionInfo.h
 * @brief  タイプ相性診断アトラクションに出てくるポケモンデータ構造
 * @author ichiraku_katsuhiko
 * @date   2016/03/02　20:14
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <limits.h>
/*
  gfl2
*/
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>
//#include  <heap/include/gfl2_Heap.h>
//#include  <fs/include/gfl2_Fs.h>
//#include  <str/include/gfl2_Str.h>


namespace NetApp    {
namespace JoinFesta {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  * @brief  タイプデータ
  */
//------------------------------------------------------------------
struct  AffinityAttractionInfo
{
  s32         level;                    /**<  難易度レベル  */
  u32         monsNo;                   /**<  種族番号  */


  void  Dump(void) const
#if PM_DEBUG
  ;
#else
  {}
#endif
};


} /*  namespace NetApp    */
} /*  namespace JoinFesta */
#endif  /*  #if !defined(JOINFESTA_AFFINITYATTRACTION_INFO_H_INCLUDED)  */
