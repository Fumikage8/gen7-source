//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   hangul_composer.cpp
 *  @brief  ハングル入力時の字母結合処理
 *  @author ichibayashi_hironori@Nintendo
 *  @data   2010.09.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <Print/include/hangul.h>
#include <Print/include/hangul_composer.h>

GFL_NAMESPACE_BEGIN(print)

//----------------------------------------------------------------------------
/**
 *  @brief  ハングル入力  初期化
 *          フォントは初期化時にセットするのが原則ですが、不可能な場合はNULLを指定し、
 *          使用前に HangulComposer::SetFont で設定してください。
 *
 *  @param  font    グリフの取得に使用するフォント
 */
//-----------------------------------------------------------------------------
HangulComposer::HangulComposer(const nw::font::Font* font)
{
  this->font = font;
}


//----------------------------------------------------------------------------
/**
 *  @brief  ハングル入力  フォント設定。Create時にフォントにNULLを指定した場合、初期化のために1回だけ呼ぶことができる。
 *
 *  @param  font    グリフの取得に使用するフォント
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
 *  @brief  ハングル入力  字母を1文字入力
 *
 *  @param  ksjamo        字母
 *  @param  composed_code [out]入力の結果、確定したハングル。確定しなければ0。
 *  @param  is_last_code  バッファの最後か、すなわち結合できない字母によって確定して欲しくないかどうか
 *
 *  @return is_last_codeの指定によって入力できなかったらfalse。それ以外の場合は入力できてtrue。
 */
//-----------------------------------------------------------------------------
bool HangulComposer::InputCode(gfl2::str::STRCODE ksjamo, gfl2::str::STRCODE *composed_code, bool is_last_code)
{
  GFL_ASSERT(this->font);
  GFL_ASSERT(hangul::IsKSJamo(ksjamo));

  // 結合中のハングルがないときは、ksjamoだけが入力された状態にする
  if (this->hangul_log_idx == 0)
  {
    AddCompositionLog(ksjamo, ksjamo);
    *composed_code = 0;
    return true;
  }
  // 結合中のハングルがあるときは、結合を行う
  else
  {
    gfl2::str::STRCODE new_hangul1, new_hangul2, deprived_ksjamo;
    
    CombineHangul(
      this->hangul_log[this->hangul_log_idx-1], ksjamo,
      &new_hangul1, &new_hangul2, &deprived_ksjamo);
    
    // 単純結合
    if (new_hangul2 == 0)
    {
      AddCompositionLog(new_hangul1, ksjamo);
      *composed_code = 0;
      return true;
    }
    else
    {
      // 変換文字が溢れるような入力は不可。2文字空きがなければならない
      if (is_last_code)
      {
        *composed_code = 0;
        return false;
      }
      // 直前のハングルから字母を取り去って結合した場合、次のbackspaceではその字母に戻る
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
      
      // 確定
      *composed_code = new_hangul1;
      return true;
    }
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  ハングル入力  結合中のハングルから字母を削除
 *
 *  @return 字母が削除できたならtrue。字母が削除できなければ（結合中のハングルが存在しなければ）false。
 */
//-----------------------------------------------------------------------------
bool HangulComposer::BackSpace()
{
  if(this->hangul_log_idx > 0)
  { 
    // ハングルログをさかのぼる
    this->hangul_log_idx --;

    return true;
  }

  return false;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ハングル入力  結合中のハングルを確定して返す
 *
 *  @return 結合中だったハングル。存在しなければ0。
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
 *  @brief  ハングル入力  結合中のハングルをクリア
 */
//-----------------------------------------------------------------------------
void HangulComposer::ClearComposition()
{
  this->hangul_log_idx = 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ハングル入力  結合中のハングルが存在するか
 *
 *  @return 結合中のハングルが存在するか
 */
//-----------------------------------------------------------------------------
bool HangulComposer::HasCompositionChar() const
{
  return this->hangul_log_idx > 0;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ハングル入力  結合中のハングルを取得
 *
 *  @return 結合中のハングル。存在しなければ0。
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
 *  @brief  ハングル入力  結合中のハングルに含まれる字母の数
 *
 *  @return 結合中のハングルに含まれる字母の数。存在しなければ0。
 */
//-----------------------------------------------------------------------------
int HangulComposer::GetCompositionJamoCount() const
{
  return this->hangul_log_idx;
}

//----------------------------------------------------------------------------
/**
 *  @brief  ハングル入力  ハングル変換ログを追加
 *
 *  @param  hangul              結合後のハングル
 *  @param  ksjamo              今回入力した KS X1001 字母
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
 *  @brief  ハングル入力  ハングルを結合
 *          出力は、3パターン：
 *          ・hangulから字母を取り除いて結合 (*deprived_ksjamo != 0)
 *          ・単純結合 (*out_hangul2 == 0)
 *          ・結合なし (*out_hangul1 == hangul, *out_hangul2 == ksjamo)
 *
 *  @param  hangul               結合前のハングル
 *  @param  *out_hangul1         出力
 *  @param  *out_hangul2         出力
 *  @param  *out_deprived_ksjamo 出力
 */
//-----------------------------------------------------------------------------
void HangulComposer::CombineHangul(
  const gfl2::str::STRCODE hangul, const gfl2::str::STRCODE ksjamo,
  gfl2::str::STRCODE *out_hangul1, gfl2::str::STRCODE *out_hangul2, gfl2::str::STRCODE *out_deprived_ksjamo)
{
  int leadcons, vowel, patchim; // hangulの分解
  gfl2::str::STRCODE deprived_ksjamo = 0;
  gfl2::str::STRCODE combined_code = 0;  // 結合後のハングル

  GFL_ASSERT(out_hangul1 != NULL);
  GFL_ASSERT(out_hangul2 != NULL);
  GFL_ASSERT(out_deprived_ksjamo != NULL);

  GFL_ASSERT(hangul::IsHangul(hangul)); // 字母でもいい
  GFL_ASSERT(hangul::IsKSJamo(ksjamo)); // 字母 i.e. KS X1001子音か母音

  // 直前のハングルが字母単独である場合
  if (!hangul::Decompose(hangul, &leadcons, &vowel, &patchim))
  {
    // hangulが子音かつksjamoが母音なら結合
    if (hangul::IsKSCons(hangul) && hangul::IsKSVowel(ksjamo))
    {
      combined_code = hangul::ComposeKSConsAndKSVowel(hangul, ksjamo);
    }
    // さもなくば結合不可
    else
    {
      combined_code = 0;
    }
  }
  // 最後がパッチムの場合
  else if (patchim != 0)
  {
    // ksjamoが母音ならそれと結合する
    if (hangul::IsKSVowel(ksjamo))
    {
      GFL_ASSERT(this->hangul_log_idx >= 3); // パッチムが存在する⇒3字母以上
      deprived_ksjamo = this->jamo_log[this->hangul_log_idx-1];
      combined_code = hangul::ComposeKSConsAndKSVowel(deprived_ksjamo, ksjamo);
    }
    // ksjamoが子音ならパッチムへの追加を試みる
    else
    {
      GFL_ASSERT(hangul::IsKSCons(ksjamo));
      patchim = hangul::ClusterPatchim(patchim, hangul::KSConsToLeadConsIndex(ksjamo));

      // 成功したら、結合
      if (patchim != -1)
      {
        combined_code = hangul::Compose(leadcons, vowel, patchim);
      }
      // 失敗したら、結合不可
      else
      {
        combined_code = 0;
      }
    }

  }
  // 最後が母音の場合
  else
  {
    // ksjamoが子音ならパッチムとする
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
    // さもなくば、合成母音にすることを試みる
    else
    {
      GFL_ASSERT(hangul::IsKSVowel(ksjamo));
      vowel = hangul::CompoundVowel(vowel, hangul::KSVowelToVowelIndex(ksjamo));

      // 成功したら、結合
      if (vowel != -1)
      {
        combined_code = hangul::Compose(leadcons, vowel, patchim);
      }
      // 失敗したら、結合不可
      else
      {
        combined_code = 0;
      }
    }
  }

  // 結合したハングルのグリフを持っていなかったら結合失敗にする
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
