//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   hangul.cpp
 *  @brief  �n���O������
 *  @author ichibayashi_hironori@Nintendo
 *  @data   2010.04.22
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include "../include/hangul.h"

// ��Unicode������g�p���Ă��Ȃ��̂��O��ƂȂ��Ă���

GFL_NAMESPACE_BEGIN(hangul)

  // KS X1001 ����̒萔�B�������A��{�̎q���ƕꉹ�̂�
const gfl2::str::STRCODE KS_JAMO_BASE   = 0x3131; // �ŏ��̎���̃R�[�h
const gfl2::str::STRCODE KS_CONS_BASE   = 0x3131; // �ŏ��̎q���̃R�[�h
const gfl2::str::STRCODE KS_VOWEL_BASE  = 0x314f; // �ŏ��̕ꉹ�̃R�[�h
const int KS_JAMO_COUNT  = 0x33;   // ����̐�
const int KS_CONS_COUNT  = 0x1e;   // �q���̐�
const int KS_VOWEL_COUNT = 0x15;   // �ꉹ�̐�

const gfl2::str::STRCODE SYLLABLE_BASE  = 0xac00; // �ŏ��̃n���O�����߂̃R�[�h
const int SYLLABLE_COUNT    = 11172; // �n���O�����߂̐�
const int LEADCONS_COUNT    = 19;    // �q���i�ŏ��j�̎�ސ�
const int VOWEL_COUNT       = 21;    // �ꉹ�̎�ސ�
const int PATCHIM_COUNT     = 28;    // �p�b�`���̎�ސ�

const int PARTICLE_ID_COUNT = 8;     // �����̐�
const int PARTICLE_ID_NI    = 5;     // �����u�Ɂv��ID

// �������p�b�`���������ǂ����iLSB=������0�j
// �i�������u�Ɂv���p�b�`�����u�ȁv�̏ꍇ�́A�p�b�`���Ȃ��Ɠ��������ɂ���j
const int PATCHIM_BITS_NUMBER    = 0x1CB;    // �u�Ɂv�ȊO   0,1,3,6,7,8 �c �p�b�`������  2,4,5,9 �c �p�b�`���Ȃ�
const int PATCHIM_BITS_NUMBER_NI = 0x049;    // �u�Ɂv�̏ꍇ 1,7,8 �c �p�b�`�����u�ȁv�Ȃ̂Ńp�b�`���Ȃ�����

// �����̕ω��\
static const gfl2::str::STRCODE ParticleTable[2][PARTICLE_ID_COUNT] =
{
  // �Ȃ�,        ��,        ��,        ��,        ��,        ��,        ��,     ����
  { L'\0', L'\xb294', L'\xb97c', L'\xac00', L'\xc640',     L'\0',     L'\0', L'\xc57c' },  // �p�b�`���Ȃ�
  { L'\0', L'\xc740', L'\xc744', L'\xc774', L'\xacfc', L'\xc73c', L'\xc774', L'\xc544' },  // �p�b�`������
};

//----------------------------------------------------------------------------
/**
 *  @brief  �������n���O�����߁ii.e. ����łȂ��n���O���j�ł��邩����
 *
 *  @param  gfl::str::STRCODE code  ����
 *
 *  @return �n���O�����߂ł����true
 */
//-----------------------------------------------------------------------------
bool IsSyllable(gfl2::str::STRCODE code)
{
  return SYLLABLE_BASE <= code && code < SYLLABLE_BASE+SYLLABLE_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O�����߂̕�������A�p�b�`�����擾����
 *
 *  @param  gfl::str::STRCODE code  �n���O�����߂̕���
 *
 *  @return �p�b�`���̔ԍ��i0:�Ȃ��`28�j
 */
//-----------------------------------------------------------------------------
int GetPatchim(gfl2::str::STRCODE code)
{
  GFL_ASSERT(IsSyllable(code));
  return (code - SYLLABLE_BASE) % PATCHIM_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ����ID�ƁA�����̒��O�̕�������A�����̕ω��`�𓾂�
 *
 *  @param  gfl::str::STRCODE last_char �����̒��O�̕���
 *  @param  int particle_id   ����ID
 *
 *  @return �����̕����i0�ł���Ε��������Ȃ��j
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
// �����艺�̓n���O�����͂ł����g�p���Ȃ��i���I���W���[���ɒǂ��o����\��������j
//


//----------------------------------------------------------------------------
/**
 *  @brief  �������n���O���i������܂ށj�ł��邩����
 *
 *  @param  gfl::str::STRCODE code  ����
 *
 *  @return �n���O���ł����true
 */
//-----------------------------------------------------------------------------
bool IsHangul(gfl2::str::STRCODE code)
{
  return IsKSJamo(code) || IsSyllable(code);
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������KS X1001����ł��邩����
 *
 *  @param  gfl::str::STRCODE code  ����
 *
 *  @return ����ł����true
 */
//-----------------------------------------------------------------------------
bool IsKSJamo(gfl2::str::STRCODE code)
{
  return KS_JAMO_BASE <= code && code < KS_JAMO_BASE+KS_JAMO_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������KS X1001����̎q���ł��邩����
 *
 *  @param  gfl::str::STRCODE code  ����
 *
 *  @return �q���ł����true
 */
//-----------------------------------------------------------------------------
bool IsKSCons(gfl2::str::STRCODE code)
{
  return KS_CONS_BASE <= code && code < KS_CONS_BASE+KS_CONS_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ������KS X1001����̕ꉹ�ł��邩����
 *
 *  @param  gfl::str::STRCODE code  ����
 *
 *  @return �ꉹ�ł����true
 */
//-----------------------------------------------------------------------------
bool IsKSVowel(gfl2::str::STRCODE code)
{
  return KS_VOWEL_BASE <= code && code < KS_VOWEL_BASE+KS_VOWEL_COUNT;
}

//----------------------------------------------------------------------------
/**
 *  @brief  KS X1001�q�����AUnicode�q���̃C���f�b�N�X�ɕϊ�
 *
 *  @param  gfl::str::STRCODE kscons  KS X1001�q��
 *
 *  @return Unicode�q���̃C���f�b�N�X�B�Ή�����Unicode�q�������݂��Ȃ����-1
 */
//-----------------------------------------------------------------------------
int KSConsToLeadConsIndex(gfl2::str::STRCODE kscons)
{
  // ���Ñ�n���O���̕��͒�`���Ă��Ȃ�
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
 *  @brief  KS X1001�ꉹ���AUnicode�ꉹ�̃C���f�b�N�X�ɕϊ�
 *
 *  @param  gfl::str::STRCODE ksvowel  KS X1001�ꉹ
 *
 *  @return Unicode�ꉹ�̃C���f�b�N�X�B�Ή�����Unicode�ꉹ�����݂��Ȃ����-1
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
 *  @brief  KS X1001�q�����AUnicode�p�b�`���̃C���f�b�N�X�ɕϊ�
 *
 *  @param  gfl::str::STRCODE kscons  KS X1001�q��
 *
 *  @return Unicode�p�b�`���̃C���f�b�N�X�B�Ή�����Unicode�p�b�`�������݂��Ȃ����-1
 */
//-----------------------------------------------------------------------------
int KSConsToPatchimIndex(gfl2::str::STRCODE kscons)
{
  // ���Ñ�n���O���̕��͒�`���Ă��Ȃ�
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
 *  @brief  �ꉹ2���A�����ꉹ����ɕϊ�
 *          �L�[�{�[�h���璼�ړ��͂ł��Ȃ������ꉹ����̐����Ɏg�p
 *
 *  @param  int vowel1  Unicode�ꉹ�C���f�b�N�X
 *  @param  int vowel2  Unicode�ꉹ�C���f�b�N�X
 *
 *  @return Unicode�ꉹ�̃C���f�b�N�X�B�����ł��Ȃ����-1
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
 *  @brief  �p�b�`���Ɏq����t�����āA�q��2����Ȃ�p�b�`���𐶐�
 *          �L�[�{�[�h���璼�ړ��͂ł��Ȃ������p�b�`���̐����Ɏg�p
 *
 *  @param  int patchim Unicode�p�b�`���C���f�b�N�X
 *  @param  int cons    Unicode�q���C���f�b�N�X
 *
 *  @return Unicode�p�b�`���̃C���f�b�N�X�B�����ł��Ȃ����-1
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
 *  @brief  �n���O�����A�擪�̎q���A�ꉹ�A�p�b�`���̃C���f�b�N�X�ɕ�������
 *          �L�[�{�[�h���璼�ړ��͂ł��Ȃ������p�b�`���̐����Ɏg�p
 *
 *  @param  gfl::str::STRCODE hangul    �n���O��
 *  @param  int *p_leadcons_index [out] �q���C���f�b�N�X
 *  @param  int *p_vowel_index    [out] �ꉹ�C���f�b�N�X
 *  @param  int *p_patchim_index  [out] �p�b�`���C���f�b�N�X
 *
 *  @return �����ł�����true�A�����ł��Ȃ�������i����̏ꍇ�jfalse
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
 *  @brief  �擪�̎q���A�ꉹ�A�p�b�`���̃C���f�b�N�X����A�n���O������������
 *
 *  @param  int leadcons_index �q���C���f�b�N�X
 *  @param  int vowel_index    �ꉹ�C���f�b�N�X
 *  @param  int patchim_index  �p�b�`���C���f�b�N�X
 *
 *  @return ���������n���O��
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE Compose(int leadcons_index, int vowel_index, int patchim_index)
{
  return SYLLABLE_BASE +
    (leadcons_index * VOWEL_COUNT + vowel_index) * PATCHIM_COUNT + patchim_index;
}

//----------------------------------------------------------------------------
/**
 *  @brief  KS X1001�q���ƕꉹ����A�n���O��������
 *
 *  @param  gfl::str::STRCODE kscons   KS X1001�q��
 *  @param  gfl::str::STRCODE ksvowel  KS X1001�ꉹ
 *
 *  @return ���������n���O���B�����ł��Ȃ����0
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
 *  @brief  �p�b�`���̂Ȃ��n���O���Ƀp�b�`����ǉ�
 *
 *  @param  gfl::str::STRCODE hangul     �n���O��
 *  @param  int patchim_index  �p�b�`���C���f�b�N�X
 *
 *  @return �p�b�`����ǉ������n���O���B�ǉ��ł��Ȃ����0
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
