//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   hangul.h
 *  @brief  ÉnÉìÉOÉãèàóù
 *  @author ichibayashi_hironori@Nintendo
 *  @data   2010.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( HANGUL_H_INCLUDED )
#define HANGUL_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>

GFL_NAMESPACE_BEGIN(hangul)

  bool IsSyllable(gfl2::str::STRCODE code);
  int GetPatchim(gfl2::str::STRCODE code);
  gfl2::str::STRCODE GetParticle(gfl2::str::STRCODE last_char, uint particle_id);

  bool IsHangul(gfl2::str::STRCODE code);
  bool IsKSJamo(gfl2::str::STRCODE code);
  bool IsKSCons(gfl2::str::STRCODE code);
  bool IsKSVowel(gfl2::str::STRCODE code);

  int KSConsToLeadConsIndex(gfl2::str::STRCODE kscons);
  int KSVowelToVowelIndex(gfl2::str::STRCODE ksvowel);
  int KSConsToPatchimIndex(gfl2::str::STRCODE kscons);

  int CompoundVowel(int vowel1, int vowel2);
  int ClusterPatchim(int patchim, int cons);

  gfl2::str::STRCODE Compose(int leadcons_index, int vowel_index, int patchim_index);
  bool Decompose(gfl2::str::STRCODE hangul, int *p_leadcons_index, int *p_vowel_index, int *p_patchim_index);

  gfl2::str::STRCODE ComposeKSConsAndKSVowel(gfl2::str::STRCODE kscons, gfl2::str::STRCODE ksvowel);
  gfl2::str::STRCODE AddPatchim(gfl2::str::STRCODE cons_vowel, int kscons);

GFL_NAMESPACE_END(hangul)

#endif  // HANGUL_H_INCLUDED
