//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   hangul_composer.h
 *  @brief  ハングル入力時の字母結合処理
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

  // 入力
  bool InputCode(gfl2::str::STRCODE ksjamo, gfl2::str::STRCODE *composed_code, bool is_last_code);
  bool BackSpace();

  // 結合中ハングル（未確定文字）の操作
  gfl2::str::STRCODE FixComposition();
  void ClearComposition();

  // 結合中ハングル（未確定文字）の取得
  bool              HasCompositionChar() const;
  gfl2::str::STRCODE GetCompositionChar() const;
  int               GetCompositionJamoCount() const;

private:
  void AddCompositionLog(gfl2::str::STRCODE hangul, gfl2::str::STRCODE ksjamo);
  void CombineHangul(
    gfl2::str::STRCODE hangul, gfl2::str::STRCODE ksjamo,
    gfl2::str::STRCODE *out_hangul1, gfl2::str::STRCODE *out_hangul2, gfl2::str::STRCODE *deprived_ksjamo);

  static const int HANGUL_LOG_LEN = 6;
  // グリフが存在するか判定するためのフォント
  const nw::font::Font* font;
  // ハングルログ。字母入力ごとに、結合後のハングルを記録
  gfl2::str::STRCODE hangul_log[HANGUL_LOG_LEN];  
  // 字母ログ。字母入力ごとに記録
  gfl2::str::STRCODE jamo_log[HANGUL_LOG_LEN];
  //ハングルログ文字数
  u16               hangul_log_idx;   
};

GFL_NAMESPACE_END(print)

#endif // HANGUL_COMPOSER_H_INCLUDED
