//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
//  @file   GrammaticalAttributeProvider.h
//  @brief  単語の文法的な属性（性、可算、語頭）を取得する
//  @author ichibayashi_hironori@Nintendo
//  @data   2012.09.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( GRAMMATICALATTRIBUTEPROVIDER_H_INCLUDED )
#define GRAMMATICALATTRIBUTEPROVIDER_H_INCLUDED
#pragma once

#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>
#include <ro/include/gfl2_RoManager.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(fs)
class ArcFile;
GFL_NAMESPACE_END(fs)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)

/**
// @brief 文法特性（性、数、最初の音、可算）をあらわす。
 */
union GrammaticalAttribute {
  struct {
    u8  Grammer       : 2;   ///< 文法性（0 = 男性、1 = 女性、2 = 中性）WordSet::Grammer と同じ
    u8  InitialSound  : 2;   ///< 最初の音タイプ（言語特有）
    u8  IsUncountable : 1;   ///< 非可算かどうか (0 = 可算、1 = 非可算）
    u8  IsAlwaysPlural: 1;   ///< 常に複数か否か (「常に複数」かつ「非可算」は禁止)
    u8  ExtraAttribute: 2;   ///< その他追加の属性。
                             ///   現在、フランス語スペイン語でバトルのトレーナータイプ+トレーナー名の変化パターンに使用。
  };
  u8 Raw;

  // メッセージバイナリのuserParamからGrammaticalAttributeを生成する
  inline static GrammaticalAttribute FromMsgDataUserParam(u16 userParam)
  {
    GrammaticalAttribute g;
    g.Raw = static_cast<u8>(userParam);
    return g;
  }
};

struct GrammarDatHeader;

class GrammaticalAttributeProvider : public gfl2::base::Singleton<GrammaticalAttributeProvider>
{
  GFL_FORBID_COPY_AND_ASSIGN(GrammaticalAttributeProvider);
public:
  GrammaticalAttributeProvider();
  ~GrammaticalAttributeProvider();

  // @brief 初期化
  // @param pHeap 文法特性データ読み込み先のヒープ
  void Initialize(gfl2::heap::HeapBase* pHeap);

  // @brief 指定した言語の文法特性データを読み込む
  // @param language 変更後の言語
  void ChangeLanguage(uint language);

  // @brief 終了処理
  void Finalize();

  // @brief 語形変化する単語の語形を得る
  // @param word_id     単語ID
  // @param wordform_id 語形ID
  const gfl2::str::STRCODE* GetWordForm(uint word_id, uint wordform_id) const;

  // @brief 指定した文字列（ユーザー入力文字列を想定）の最初の音タイプを得る
  // @param sp   EOM終端の文字列
  uint GetInitialSoundType(const gfl2::str::STRCODE* sp) const;

  // @brief 指定した数が複数かどうかを得る
  // @param number 数
  bool IsPluralNumber(int number) const; 

private:
  // 文法特性 dat へのアクセス
  const GrammarDatHeader* getHeader() const;
  const u8* getWordFormOffsets() const;

  // 読み込み関連
  static void OpenArcFile();
  static void CloseArcFile();
  void LoadData(uint datId);
  static u32 getMaxDataSize();
  static uint getDatId(uint lang);

  // 最初の音タイプ判定
  static uint getInitialSoundTypeOther(const gfl2::str::STRCODE* sp);
  static uint getInitialSoundTypeFrench(const gfl2::str::STRCODE* sp);
  static uint getInitialSoundTypeItalian(const gfl2::str::STRCODE* sp);
  static bool isVowelAlphabet(gfl2::str::STRCODE c);

  u8* mpData; // 文法特性 dat を格納するメモリ
  u32 mpDataSize; // 確保したmpDataのサイズ
  uint m_currentLanguage;  // ロードしてある言語
};

} // namespace print

SINGLETON_INSTANCE_DECLARATION(print::GrammaticalAttributeProvider);

#endif // GRAMMATICALATTRIBUTEPROVIDER_H_INCLUDED
