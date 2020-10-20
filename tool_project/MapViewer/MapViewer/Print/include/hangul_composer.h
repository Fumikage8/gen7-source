//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   hangul_composer.h
 *  @brief  �n���O�����͎��̎��ꌋ������
 *  @author ichibayashi_hironori@Nintendo
 *  @data   2010.09.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( HANGUL_COMPOSER_H_INCLUDED )
#define HANGUL_COMPOSER_H_INCLUDED
#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <str/include/gfl2_Str.h>

GFL_NAMESPACE_BEGIN(print)

class HangulComposer
{
  GFL_FORBID_COPY_AND_ASSIGN(HangulComposer);
public:
  HangulComposer(const nw::font::Font* font);

  void SetFont(const nw::font::Font* font);

  // ����
  bool InputCode(gfl2::str::STRCODE ksjamo, gfl2::str::STRCODE *composed_code, bool is_last_code);
  bool BackSpace();

  // �������n���O���i���m�蕶���j�̑���
  gfl2::str::STRCODE FixComposition();
  void ClearComposition();

  // �������n���O���i���m�蕶���j�̎擾
  bool              HasCompositionChar() const;
  gfl2::str::STRCODE GetCompositionChar() const;
  int               GetCompositionJamoCount() const;

private:
  void AddCompositionLog(gfl2::str::STRCODE hangul, gfl2::str::STRCODE ksjamo);
  void CombineHangul(
    gfl2::str::STRCODE hangul, gfl2::str::STRCODE ksjamo,
    gfl2::str::STRCODE *out_hangul1, gfl2::str::STRCODE *out_hangul2, gfl2::str::STRCODE *deprived_ksjamo);

  static const int HANGUL_LOG_LEN = 6;
  // �O���t�����݂��邩���肷�邽�߂̃t�H���g
  const nw::font::Font* font;
  // �n���O�����O�B������͂��ƂɁA������̃n���O�����L�^
  gfl2::str::STRCODE hangul_log[HANGUL_LOG_LEN];  
  // ���ꃍ�O�B������͂��ƂɋL�^
  gfl2::str::STRCODE jamo_log[HANGUL_LOG_LEN];
  //�n���O�����O������
  u16               hangul_log_idx;   
};

GFL_NAMESPACE_END(print)

#endif // HANGUL_COMPOSER_H_INCLUDED
