//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   hangul_composer.cpp
 *  @brief  �n���O�����͎��̎��ꌋ������
 *  @author ichibayashi_hironori@Nintendo
 *  @data   2010.09.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <../include/hangul.h>
#include <../include/hangul_composer.h>

GFL_NAMESPACE_BEGIN(print)

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  ������
 *          �t�H���g�͏��������ɃZ�b�g����̂������ł����A�s�\�ȏꍇ��NULL���w�肵�A
 *          �g�p�O�� HangulComposer::SetFont �Őݒ肵�Ă��������B
 *
 *  @param  font    �O���t�̎擾�Ɏg�p����t�H���g
 */
//-----------------------------------------------------------------------------
HangulComposer::HangulComposer(const nw::font::Font* font)
{
  this->font = font;
}


//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �t�H���g�ݒ�BCreate���Ƀt�H���g��NULL���w�肵���ꍇ�A�������̂��߂�1�񂾂��ĂԂ��Ƃ��ł���B
 *
 *  @param  font    �O���t�̎擾�Ɏg�p����t�H���g
 */
//-----------------------------------------------------------------------------
void HangulComposer::SetFont(const nw::font::Font* font)
{
  GFL_ASSERT(this->font == NULL);
  GFL_ASSERT(font != NULL);
  this->font = font;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �����1��������
 *
 *  @param  ksjamo        ����
 *  @param  composed_code [out]���͂̌��ʁA�m�肵���n���O���B�m�肵�Ȃ����0�B
 *  @param  is_last_code  �o�b�t�@�̍Ōォ�A���Ȃ킿�����ł��Ȃ�����ɂ���Ċm�肵�ė~�����Ȃ����ǂ���
 *
 *  @return is_last_code�̎w��ɂ���ē��͂ł��Ȃ�������false�B����ȊO�̏ꍇ�͓��͂ł���true�B
 */
//-----------------------------------------------------------------------------
bool HangulComposer::InputCode(gfl2::str::STRCODE ksjamo, gfl2::str::STRCODE *composed_code, bool is_last_code)
{
  GFL_ASSERT(this->font);
  GFL_ASSERT(hangul::IsKSJamo(ksjamo));

  // �������̃n���O�����Ȃ��Ƃ��́Aksjamo���������͂��ꂽ��Ԃɂ���
  if (this->hangul_log_idx == 0)
  {
    AddCompositionLog(ksjamo, ksjamo);
    *composed_code = 0;
    return true;
  }
  // �������̃n���O��������Ƃ��́A�������s��
  else
  {
    gfl2::str::STRCODE new_hangul1, new_hangul2, deprived_ksjamo;
    
    CombineHangul(
      this->hangul_log[this->hangul_log_idx-1], ksjamo,
      &new_hangul1, &new_hangul2, &deprived_ksjamo);
    
    // �P������
    if (new_hangul2 == 0)
    {
      AddCompositionLog(new_hangul1, ksjamo);
      *composed_code = 0;
      return true;
    }
    else
    {
      // �ϊ�����������悤�ȓ��͕͂s�B2�����󂫂��Ȃ���΂Ȃ�Ȃ�
      if (is_last_code)
      {
        *composed_code = 0;
        return false;
      }
      // ���O�̃n���O�����玚�����苎���Č��������ꍇ�A����backspace�ł͂��̎���ɖ߂�
      if (deprived_ksjamo != 0)
      {
        HangulComposer::ClearComposition();
        AddCompositionLog(deprived_ksjamo, deprived_ksjamo);
        AddCompositionLog(new_hangul2, ksjamo);
      }
      else 
      {
        HangulComposer::ClearComposition();
        AddCompositionLog(new_hangul2, ksjamo);
      }
      
      // �m��
      *composed_code = new_hangul1;
      return true;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �������̃n���O�����玚����폜
 *
 *  @return ���ꂪ�폜�ł����Ȃ�true�B���ꂪ�폜�ł��Ȃ���΁i�������̃n���O�������݂��Ȃ���΁jfalse�B
 */
//-----------------------------------------------------------------------------
bool HangulComposer::BackSpace()
{
  if(this->hangul_log_idx > 0)
  { 
    // �n���O�����O�������̂ڂ�
    this->hangul_log_idx --;

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �������̃n���O�����m�肵�ĕԂ�
 *
 *  @return �������������n���O���B���݂��Ȃ����0�B
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE HangulComposer::FixComposition()
{
  gfl2::str::STRCODE code = HangulComposer::GetCompositionChar();
  HangulComposer::ClearComposition();
  return code;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �������̃n���O�����N���A
 */
//-----------------------------------------------------------------------------
void HangulComposer::ClearComposition()
{
  this->hangul_log_idx = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �������̃n���O�������݂��邩
 *
 *  @return �������̃n���O�������݂��邩
 */
//-----------------------------------------------------------------------------
bool HangulComposer::HasCompositionChar() const
{
  return this->hangul_log_idx > 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �������̃n���O�����擾
 *
 *  @return �������̃n���O���B���݂��Ȃ����0�B
 */
//-----------------------------------------------------------------------------
gfl2::str::STRCODE HangulComposer::GetCompositionChar() const
{
  if (HangulComposer::HasCompositionChar())
  {
    return this->hangul_log[this->hangul_log_idx-1];
  }
  else {
    return 0;
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �������̃n���O���Ɋ܂܂�鎚��̐�
 *
 *  @return �������̃n���O���Ɋ܂܂�鎚��̐��B���݂��Ȃ����0�B
 */
//-----------------------------------------------------------------------------
int HangulComposer::GetCompositionJamoCount() const
{
  return this->hangul_log_idx;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �n���O���ϊ����O��ǉ�
 *
 *  @param  hangul              ������̃n���O��
 *  @param  ksjamo              ������͂��� KS X1001 ����
 */
//-----------------------------------------------------------------------------
void HangulComposer::AddCompositionLog(gfl2::str::STRCODE hangul, gfl2::str::STRCODE ksjamo)
{
  int index = this->hangul_log_idx;
  GFL_ASSERT(this->hangul_log_idx < HANGUL_LOG_LEN);
  this->hangul_log[index] = hangul;
  this->jamo_log[index] = ksjamo;
  this->hangul_log_idx = index + 1;
}

//----------------------------------------------------------------------------
/**
 *  @brief  �n���O������  �n���O��������
 *          �o�͂́A3�p�^�[���F
 *          �Ehangul���玚�����菜���Č��� (*deprived_ksjamo != 0)
 *          �E�P������ (*out_hangul2 == 0)
 *          �E�����Ȃ� (*out_hangul1 == hangul, *out_hangul2 == ksjamo)
 *
 *  @param  hangul               �����O�̃n���O��
 *  @param  *out_hangul1         �o��
 *  @param  *out_hangul2         �o��
 *  @param  *out_deprived_ksjamo �o��
 */
//-----------------------------------------------------------------------------
void HangulComposer::CombineHangul(
  const gfl2::str::STRCODE hangul, const gfl2::str::STRCODE ksjamo,
  gfl2::str::STRCODE *out_hangul1, gfl2::str::STRCODE *out_hangul2, gfl2::str::STRCODE *out_deprived_ksjamo)
{
  int leadcons, vowel, patchim; // hangul�̕���
  gfl2::str::STRCODE deprived_ksjamo = 0;
  gfl2::str::STRCODE combined_code = 0;  // ������̃n���O��

  GFL_ASSERT(out_hangul1 != NULL);
  GFL_ASSERT(out_hangul2 != NULL);
  GFL_ASSERT(out_deprived_ksjamo != NULL);

  GFL_ASSERT(hangul::IsHangul(hangul)); // ����ł�����
  GFL_ASSERT(hangul::IsKSJamo(ksjamo)); // ���� i.e. KS X1001�q�����ꉹ

  // ���O�̃n���O��������P�Ƃł���ꍇ
  if (!hangul::Decompose(hangul, &leadcons, &vowel, &patchim))
  {
    // hangul���q������ksjamo���ꉹ�Ȃ猋��
    if (hangul::IsKSCons(hangul) && hangul::IsKSVowel(ksjamo))
    {
      combined_code = hangul::ComposeKSConsAndKSVowel(hangul, ksjamo);
    }
    // �����Ȃ��Ό����s��
    else
    {
      combined_code = 0;
    }
  }
  // �Ōオ�p�b�`���̏ꍇ
  else if (patchim != 0)
  {
    // ksjamo���ꉹ�Ȃ炻��ƌ�������
    if (hangul::IsKSVowel(ksjamo))
    {
      GFL_ASSERT(this->hangul_log_idx >= 3); // �p�b�`�������݂����3����ȏ�
      deprived_ksjamo = this->jamo_log[this->hangul_log_idx-1];
      combined_code = hangul::ComposeKSConsAndKSVowel(deprived_ksjamo, ksjamo);
    }
    // ksjamo���q���Ȃ�p�b�`���ւ̒ǉ������݂�
    else
    {
      GFL_ASSERT(hangul::IsKSCons(ksjamo));
      patchim = hangul::ClusterPatchim(patchim, hangul::KSConsToLeadConsIndex(ksjamo));

      // ����������A����
      if (patchim != -1)
      {
        combined_code = hangul::Compose(leadcons, vowel, patchim);
      }
      // ���s������A�����s��
      else
      {
        combined_code = 0;
      }
    }

  }
  // �Ōオ�ꉹ�̏ꍇ
  else
  {
    // ksjamo���q���Ȃ�p�b�`���Ƃ���
    if (hangul::IsKSCons(ksjamo))
    {
      int add_patchim = hangul::KSConsToPatchimIndex(ksjamo);
      if (add_patchim != -1)
      {
        combined_code = hangul::AddPatchim(hangul, add_patchim);
      }
      else
      {
        combined_code = 0;
      }
    }
    // �����Ȃ��΁A�����ꉹ�ɂ��邱�Ƃ����݂�
    else
    {
      GFL_ASSERT(hangul::IsKSVowel(ksjamo));
      vowel = hangul::CompoundVowel(vowel, hangul::KSVowelToVowelIndex(ksjamo));

      // ����������A����
      if (vowel != -1)
      {
        combined_code = hangul::Compose(leadcons, vowel, patchim);
      }
      // ���s������A�����s��
      else
      {
        combined_code = 0;
      }
    }
  }

  // ���������n���O���̃O���t�������Ă��Ȃ������猋�����s�ɂ���
  if (combined_code != 0)
  {
    if (!this->font->HasGlyph(combined_code)) 
    {
      deprived_ksjamo = 0;
      combined_code = 0;
    }
  }

  if (deprived_ksjamo != 0)
  {
    *out_hangul1 = this->hangul_log[this->hangul_log_idx-2];
    *out_hangul2 = combined_code;
    *out_deprived_ksjamo = deprived_ksjamo;
  }
  else if (combined_code != 0)
  {
    *out_hangul1 = combined_code;
    *out_hangul2 = 0;
    *out_deprived_ksjamo = 0;
  }
  else
  {
    *out_hangul1 = hangul;
    *out_hangul2 = ksjamo;
    *out_deprived_ksjamo = 0;
  }
}

GFL_NAMESPACE_END(print)
