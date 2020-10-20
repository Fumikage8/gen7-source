#if !defined(JOINFESTA_POKETYPEATTRACTION_INFO_H_INCLUDED)
#define JOINFESTA_POKETYPEATTRACTION_INFO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaPokeTypeAttractionInfo.h
 * @brief  ポケモンタイプ相性アトラクションに出てくるポケモンデータ構造
 * @author ichiraku_katsuhiko
 * @date   2016/02/10　20:14
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
  * @brief  ポケモンデータ
  */
//------------------------------------------------------------------
struct  PokeTypeAttractionInfo
{
  s32         pokeType;                 /**<  タイプ  */
  u32         monsNo;                   /**<  種族番号  */
  u32         formNo;                   /**<  フォルム番号  */


  void  Dump(void) const
#if PM_DEBUG
  ;
#else
  {}
#endif
};


} /*  namespace NetApp    */
} /*  namespace JoinFesta */
#endif  /*  #if !defined(JOINFESTA_POKETYPEATTRACTION_INFO_H_INCLUDED)  */
