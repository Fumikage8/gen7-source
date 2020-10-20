//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   hangul.cpp
 *  @brief  ハングル処理
 *  @author ichibayashi_hironori@Nintendo
 *  @data   2010.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "../include/hangul.h"

// ※Unicode字母を使用していないのが前提となっている

GFL_NAMESPACE_BEGIN(hangul)

  // KS X1001 字母の定数。ただし、基本の子音と母音のみ
const gfl2::str::STRCODE KS_JAMO_BASE   = 0x3131; // 最初の字母のコード
const gfl2::str::STRCODE KS_CONS_BASE   = 0x3131; // 最初の子音のコード
const gfl2::str::STRCODE KS_VOWEL_BASE  = 0x314f; // 最初の母音のコード
const int KS_JAMO_COUNT  = 0x33;   // 字母の数
const int KS_CONS_COUNT  = 0x1e;   // 子音の数
const int KS_VOWEL_COUNT = 0x15;   // 母音の数

const gfl2::str::STRCODE SYLLABLE_BASE  = 0xac00; // 最初のハングル音節のコード
const int SYLLABLE_COUNT    = 11172; // ハングル音節の数
const int LEADCONS_COUNT    = 19;    // 子音（最初）の種類数
const int VOWEL_COUNT       = 21;    // 母音の種類数
const int PATCHIM_COUNT     = 28;    // パッチムの種類数

const int PARTICLE_ID_COUNT = 8;     // 助詞の数
const int PARTICLE_ID_NI    = 5;     // 助詞「に」のID

// 数字がパッチムを持つかどうか（LSB=数字の0）
// （助詞が「に」かつパッチムが「己」の場合は、パッチムなしと同じ扱いにする）
const int PATCHIM_BITS_NUMBER    = 0x1CB;    // 「に」以外   0,1,3,6,7,8 … パッチムあり  2,4,5,9 … パッチムなし
const int PATCHIM_BITS_NUMBER_NI = 0x049;    // 「に」の場合 1,7,8 … パッチムが「己」なのでパッチムなし扱い

// 助詞の変化表
static const gfl2::str::STRCODE ParticleTable[2][PARTICLE_ID_COUNT] =
{
  // なし,        は,        を,        が,        と,        に,        い,     さん
  { L'\0', L'\xb294', L'\xb97c', L'\xac00', L'\xc640',     L'\0',     L'\0', L'\xc57c' },  // パッチムなし
  { L'\0', L'\xc740', L'\xc744', L'\xc774', L'\xacfc', L'\xc73c', L'\xc774', L'\xc544' },  // パッチムあり
};

//----------------------------------------------------------------------------
/**
 *  @brief  文字がハングル音節（i.e. 字母でないハングル）であるか判定
 *
 *  @param  gfl::str::STRCODE code  文字
 *
 *  @return ハングル音節であればtrue
 */
//-----------------------------------------------------------------------------
bool IsSyllable(gfl2::str::STRCODE code)
{
  return SYLLABLE_BASE <= code && code < SYLLABLE_BASE+SYLLABLE_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ハングル音節の文字から、パッチムを取得する
 *
 *  @param  gfl::str::STRCODE code  ハングル音節の文字
 *
 *  @return パッチムの番号（0:なし～28）
 */
//-----------------------------------------------------------------------------
int GetPatchim(gfl2::str::STRCODE code)
{
  GFL_ASSERT(IsSyllable(code));
  return (code - SYLLABLE_BASE) % PATCHIM_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  助詞IDと、助詞の直前の文字から、助詞の変化形を得る
 *
 *  @param  gfl::str::STRCODE last_char 助詞の直前の文字
 *  @param  int particle_id   助詞ID
 *
 *  @return 助詞の文字（0であれば文字を入れない）
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE GetParticle(gfl2::str::STRCODE last_char, uint particle_id)
{
  GFL_ASSERT(particle_id < PARTICLE_ID_COUNT);
  if (PARTICLE_ID_COUNT <= particle_id)
  {
    return 0;
  }

  int has_patchim;
  if (IsSyllable(last_char))
  {
    int patchim_id = GetPatchim(last_char);
    if (particle_id == PARTICLE_ID_NI)
    {
      has_patchim = (patchim_id != 0 && patchim_id != 8) ? 1 : 0;
    }
    else
    {
      has_patchim = (patchim_id != 0) ? 1 : 0;
    }
  }
  else if (L'0' <= last_char && last_char <= L'9')
  {
    int number = last_char - L'0';
    if (particle_id == PARTICLE_ID_NI)
    {
      has_patchim = (PATCHIM_BITS_NUMBER_NI >> number) & 1;
    }
    else
    {
      has_patchim = (PATCHIM_BITS_NUMBER    >> number) & 1;
    }
  }
  else {
    has_patchim = 0;
  }

  return ParticleTable[has_patchim][particle_id];
}



//
// これより下はハングル入力でしか使用しない（動的モジュールに追い出せる可能性がある）
//


//----------------------------------------------------------------------------
/**
 *  @brief  文字がハングル（字母を含む）であるか判定
 *
 *  @param  gfl::str::STRCODE code  文字
 *
 *  @return ハングルであればtrue
 */
//-----------------------------------------------------------------------------
bool IsHangul(gfl2::str::STRCODE code)
{
  return IsKSJamo(code) || IsSyllable(code);
}

//----------------------------------------------------------------------------
/**
 *  @brief  文字がKS X1001字母であるか判定
 *
 *  @param  gfl::str::STRCODE code  文字
 *
 *  @return 字母であればtrue
 */
//-----------------------------------------------------------------------------
bool IsKSJamo(gfl2::str::STRCODE code)
{
  return KS_JAMO_BASE <= code && code < KS_JAMO_BASE+KS_JAMO_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  文字がKS X1001字母の子音であるか判定
 *
 *  @param  gfl::str::STRCODE code  文字
 *
 *  @return 子音であればtrue
 */
//-----------------------------------------------------------------------------
bool IsKSCons(gfl2::str::STRCODE code)
{
  return KS_CONS_BASE <= code && code < KS_CONS_BASE+KS_CONS_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  文字がKS X1001字母の母音であるか判定
 *
 *  @param  gfl::str::STRCODE code  文字
 *
 *  @return 母音であればtrue
 */
//-----------------------------------------------------------------------------
bool IsKSVowel(gfl2::str::STRCODE code)
{
  return KS_VOWEL_BASE <= code && code < KS_VOWEL_BASE+KS_VOWEL_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  KS X1001子音を、Unicode子音のインデックスに変換
 *
 *  @param  gfl::str::STRCODE kscons  KS X1001子音
 *
 *  @return Unicode子音のインデックス。対応するUnicode子音が存在しなければ-1
 */
//-----------------------------------------------------------------------------
int KSConsToLeadConsIndex(gfl2::str::STRCODE kscons)
{
  // ※古代ハングルの分は定義していない
  static s8 table[KS_CONS_COUNT] = {
     0,  1, -1,  2, -1, -1,  3,  4,  // U+3131
     5, -1, -1, -1, -1, -1, -1, -1,  // U+3139
     6,  7,  8, -1,  9, 10, 11, 12,  // U+3141
    13, 14, 15, 16, 17, 18           // U+3149
  };
  if (!IsKSCons(kscons))
  {
    return -1;
  }
  return table[kscons - KS_CONS_BASE];
}

//----------------------------------------------------------------------------
/**
 *  @brief  KS X1001母音を、Unicode母音のインデックスに変換
 *
 *  @param  gfl::str::STRCODE ksvowel  KS X1001母音
 *
 *  @return Unicode母音のインデックス。対応するUnicode母音が存在しなければ-1
 */
//-----------------------------------------------------------------------------
int KSVowelToVowelIndex(gfl2::str::STRCODE ksvowel)
{
  if (!IsKSVowel(ksvowel))
  {
    return -1;
  }
  return ksvowel - KS_VOWEL_BASE;
}

//----------------------------------------------------------------------------
/**
 *  @brief  KS X1001子音を、Unicodeパッチムのインデックスに変換
 *
 *  @param  gfl::str::STRCODE kscons  KS X1001子音
 *
 *  @return Unicodeパッチムのインデックス。対応するUnicodeパッチムが存在しなければ-1
 */
//-----------------------------------------------------------------------------
int KSConsToPatchimIndex(gfl2::str::STRCODE kscons)
{
  // ※古代ハングルの分は定義していない
  static s8 table[KS_CONS_COUNT] = {
     1,  2, -1,  4, -1, -1,  7, -1,  // U+3131
     8, -1, -1, -1, -1, -1, -1, -1,  // U+3139
    16, 17, -1, -1, 19, 20, 21, 22,  // U+3141
    -1, 23, 24, 25, 26, 27           // U+3149
  };
  if (!IsKSCons(kscons))
  {
    return -1;
  }
  return table[kscons - KS_CONS_BASE];
}

//----------------------------------------------------------------------------
/**
 *  @brief  母音2つを、合成母音字母に変換
 *          キーボードから直接入力できない合成母音字母の生成に使用
 *
 *  @param  int vowel1  Unicode母音インデックス
 *  @param  int vowel2  Unicode母音インデックス
 *
 *  @return Unicode母音のインデックス。合成できなければ-1
 */
//-----------------------------------------------------------------------------
int CompoundVowel(int vowel1, int vowel2)
{
  static struct
  {
    s8 vowel1;
    s8 vowel2;
    s8 compounded_vowel;
  } table[] = 
  {
    {  8,  0,  9}, // o + a  = wa
    {  8,  1, 10}, // o + ae = wae
    {  8, 20, 11}, // o + i  = oe
    { 13,  4, 14}, // u + eo = wo
    { 13,  5, 15}, // u + e  = we
    { 13, 20, 16}, // u + i  = wi
    { 18, 20, 19}, // eu+ i  = ui
  };
  int i;

  for (i = 0; i < GFL_NELEMS(table); i ++)
  {
    if (table[i].vowel1 == vowel1 && table[i].vowel2 == vowel2)
    {
      return table[i].compounded_vowel;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  パッチムに子音を付加して、子音2つからなるパッチムを生成
 *          キーボードから直接入力できない複合パッチムの生成に使用
 *
 *  @param  int patchim Unicodeパッチムインデックス
 *  @param  int cons    Unicode子音インデックス
 *
 *  @return Unicodeパッチムのインデックス。合成できなければ-1
 */
//-----------------------------------------------------------------------------
int ClusterPatchim(int patchim, int cons)
{
  static struct
  {
    s8 patchim;
    s8 cons;
    s8 clustered_patchim;
  } table[] = 
  {
    {  1,  9,  3}, // k + s
    {  4, 12,  5}, // n + j
    {  4, 18,  6}, // n + h
    {  8,  0,  9}, // r + k
    {  8,  6, 10}, // r + m
    {  8,  7, 11}, // r + b
    {  8,  9, 12}, // r + s
    {  8, 16, 13}, // r + t
    {  8, 17, 14}, // r + p
    {  8, 18, 15}, // r + h
    { 17,  9, 18}, // p + s
  };
  int i;

  for (i = 0; i < GFL_NELEMS(table); i ++)
  {
    if (table[i].patchim == patchim && table[i].cons == cons)
    {
      return table[i].clustered_patchim;
    }
  }

  return -1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ハングルを、先頭の子音、母音、パッチムのインデックスに分解する
 *          キーボードから直接入力できない複合パッチムの生成に使用
 *
 *  @param  gfl::str::STRCODE hangul    ハングル
 *  @param  int *p_leadcons_index [out] 子音インデックス
 *  @param  int *p_vowel_index    [out] 母音インデックス
 *  @param  int *p_patchim_index  [out] パッチムインデックス
 *
 *  @return 分解できたらtrue、分解できなかったら（字母の場合）false
 */
//-----------------------------------------------------------------------------
bool Decompose(gfl2::str::STRCODE hangul, int *p_leadcons_index, int *p_vowel_index, int *p_patchim_index)
{
  if (!IsSyllable(hangul))
  {
    return false;
  }
  else
  {
    int hangul_index = hangul - SYLLABLE_BASE;

    if (p_leadcons_index != NULL)
    {
      *p_leadcons_index = hangul_index / (PATCHIM_COUNT*VOWEL_COUNT);
    }
    if (p_vowel_index != NULL)
    {
      *p_vowel_index = (hangul_index / PATCHIM_COUNT) % VOWEL_COUNT;
    }
    if (p_patchim_index != NULL) 
    {
      *p_patchim_index = hangul_index % PATCHIM_COUNT;
    }
    return true;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  先頭の子音、母音、パッチムのインデックスから、ハングルを合成する
 *
 *  @param  int leadcons_index 子音インデックス
 *  @param  int vowel_index    母音インデックス
 *  @param  int patchim_index  パッチムインデックス
 *
 *  @return 合成したハングル
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE Compose(int leadcons_index, int vowel_index, int patchim_index)
{
  return SYLLABLE_BASE +
    (leadcons_index * VOWEL_COUNT + vowel_index) * PATCHIM_COUNT + patchim_index;
}

//----------------------------------------------------------------------------
/**
 *  @brief  KS X1001子音と母音から、ハングルを合成
 *
 *  @param  gfl::str::STRCODE kscons   KS X1001子音
 *  @param  gfl::str::STRCODE ksvowel  KS X1001母音
 *
 *  @return 合成したハングル。合成できなければ0
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE ComposeKSConsAndKSVowel(gfl2::str::STRCODE kscons, gfl2::str::STRCODE ksvowel)
{
  int cons_index, vowel_index;

  if (!IsKSCons(kscons) || !IsKSVowel(ksvowel))
  {
    return 0;
  }

  cons_index = KSConsToLeadConsIndex(kscons);
  if (cons_index == -1)
  {
    return 0;
  }
  
  vowel_index = KSVowelToVowelIndex(ksvowel);
  if (vowel_index == -1)
  {
    return 0;
  }

  return SYLLABLE_BASE + (cons_index * VOWEL_COUNT + vowel_index) * PATCHIM_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  パッチムのないハングルにパッチムを追加
 *
 *  @param  gfl::str::STRCODE hangul     ハングル
 *  @param  int patchim_index  パッチムインデックス
 *
 *  @return パッチムを追加したハングル。追加できなければ0
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE AddPatchim(gfl2::str::STRCODE hangul, int patchim_index)
{
  if (!IsSyllable(hangul) || (GetPatchim(hangul) != 0))
  {
    return 0;
  }

  return hangul + patchim_index;
}

GFL_NAMESPACE_END(hangul)
