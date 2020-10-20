#if !defined(JOINFESTA_KISEKAEPATTERN_INFO_H_INCLUDED)
#define JOINFESTA_KISEKAEPATTERN_INFO_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   JoinFestaKisekaePatternInfo.h
 * @brief  アトラクションの着せ替えダミーパターン
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
  * @brief  着せ替えデータ
  */
//------------------------------------------------------------------
struct  KisekaePatternInfo
{
  s32       sex;
  s32       body_color;
  s32       eye_color;
  s32       hair_color;
  s32       female_lip_color;
  s32       face;
  s32       hair_style;
  s32       accbadge;
  s32       acceye;
  s32       acchair;
  s32       bag;
  s32       bngl;
  s32       body;
  s32       bottoms;
  s32       hat;
  s32       legs;
  s32       shoes;
  s32       tops;  


  void  Dump(void) const
#if PM_DEBUG
  ;
#else
  {}
#endif
};


} /*  namespace NetApp    */
} /*  namespace JoinFesta */
#endif  /*  #if !defined(JOINFESTA_KISEKAEPATTERN_INFO_H_INCLUDED)  */
