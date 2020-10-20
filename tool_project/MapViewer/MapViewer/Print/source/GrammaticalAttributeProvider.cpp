//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   GrammaticalAttributeProvider.cpp
 *  @brief  単語の文法的な属性（性、可算、最初の音）を取得する
 *  @author ichibayashi_hironori@Nintendo
 *  @date   2012.09.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// "GrammaticalAttribute"Provider という感じではなくなってしまった

#include <../include/GrammaticalAttributeProvider.h>
#include <../../System/include/PokemonVersion.h>
#include <../../System/include/HeapDefine.h>

#include <arc_def_index/arc_def.h>
#include <arc_index/grammar.gaix>


#if GFL_RO_ENABLE
SINGLETON_INSTANCE_DEFINITION(print::GrammaticalAttributeProvider);
template class gfl2::base::SingletonAccessor<print::GrammaticalAttributeProvider>;
#endif 

GFL_NAMESPACE_BEGIN(print)

struct GrammarDatHeader
{
  u8  WordCount;                      // 語形変化する単語の数
  u8  WordFormCount;                  // 語形変化の変化パターン数
  u16 WordOffsets[1];                 // 語形変化文字列へのオフセット
  // WordOffsets 以降 (WordOffsets含む) は
  // u16 WordOffsets[WordCount];
  // u8[WordCount*WordFormCount]        // 語形変化文字列内での各語形のオフセット
  // wordFormString[WordCount] ALIGN(2) // 語形変化文字列
};

//-----------------------------------------------------------------------------
// 初期化・終了処理
//-----------------------------------------------------------------------------

GrammaticalAttributeProvider::GrammaticalAttributeProvider()
  : mpData(0)
  , mpDataSize(0)
{
}


GrammaticalAttributeProvider::~GrammaticalAttributeProvider()
{
  GFL_SAFE_DELETE(mpData);
}

// @brief 初期化
// @param pHeap 文法特性データ読み込み先のヒープ
void GrammaticalAttributeProvider::Initialize(gfl2::heap::HeapBase* heap)
{
  GFL_ASSERT_MSG(mpData == 0, "already initialized");

  // 現在の言語の文法特性データ番号を取得
  uint language = System::GetLang();
  uint datId = getDatId(language);

  m_currentLanguage = language;

  OpenArcFile();
  // 最大datのサイズでメモリを確保
  mpDataSize = getMaxDataSize();
  mpData = static_cast<u8*>(GflHeapAllocMemory(heap, mpDataSize));
  // 文法特性データをロード
  LoadData( datId );
  CloseArcFile();
}

// @brief 指定した言語の文法特性データを読み込む
// @param language 変更後の言語
void GrammaticalAttributeProvider::ChangeLanguage(uint language)
{
  if (language == m_currentLanguage)
    return;
  m_currentLanguage = language;

  // 指定した言語の文法特性データ番号を取得
  uint datId = getDatId( language );

  // 文法特性データをロード
  OpenArcFile();
  LoadData( datId );
  CloseArcFile();
}

// @brief 終了処理
void GrammaticalAttributeProvider::Finalize()
{
  GFL_SAFE_DELETE(mpData);
}

// ARCID_GRAMMAR を開く
void GrammaticalAttributeProvider::OpenArcFile()
{
  gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_FILEREAD);

  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.fileName    = NULL;                                         // [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
  req.arcId       = ARCID_GRAMMAR;                                // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
  req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
  req.heapForFile = fileread_heap;                                // [in]  ヒープ。ファイルインスタンスの作成と索引テーブルの作成に使用。Closeするまで左記のものが保持される。
  req.heapForReq  = fileread_heap;                                // [in]  リクエスト終了まで一時的に使用するヒープ。索引テーブルの作成のためにも使用する。ArcSrcのインスタンスやソーステーブルの作成のためにも使用する。

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  man->SyncArcFileOpen( req );
}

// ARCID_GRAMMAR を閉じる
void GrammaticalAttributeProvider::CloseArcFile()
{
  gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_FILEREAD);

  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.fileName    = NULL;                                         // [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。  // ****キー****
  req.arcId       = ARCID_GRAMMAR;                                // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。  // ****キー****
  req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
  req.heapForReq  = NULL;                                         // [in]  リクエスト終了まで一時的に使用するヒープ。同期版ではNULLでよい。

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  man->SyncArcFileClose( req );
}

// 指定した dat をロードする
void GrammaticalAttributeProvider::LoadData( uint datId )
{
  gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_FILEREAD);

  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
  req.fileName          = NULL;                                         // [in]  ファイル名。終端は'\0'になっていること。arcIdがARCID_NULLのときのみ使われる。
  req.arcId             = ARCID_GRAMMAR;                                // [in]  ARCID。ARCID_NULLのときはfileNameが使われる。
  req.datId             = datId;                                        // [in]  アーカイブファイル内データID
  req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  リクエストの実行優先度(値が小さい方が優先度が高い)
  req.pBuf              = mpData;                                       // [out] 呼び出し元であらかじめメモリを確保しておいたバッファ。ここにファイルから読み込んだ内容を入れる。  // ****キー****
  req.bufSize           = mpDataSize;                                   // [in]  pBufのサイズ。データサイズ(パディングを含む)以上にして下さい。
  req.pRealReadSize     = NULL;                                         // [out] 実際に読み込んだサイズ。compressedがtrueのとき解凍後のサイズ。NULLでもよい。
  req.heapForReq        = fileread_heap;                                // [in]  リクエスト終了まで一時的に使用するヒープ。『「同期版」かつ「ArcSrcを使わないとき」』はNULLでよい。ArcSrcを使うときはファイルオープンにも使用する。
  req.heapForCompressed = NULL;                                         // [in]  解凍する場合に一時的に使用するヒープ。解凍しない場合はNULLでよい。

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  man->SyncArcFileLoadDataBuf( req );
}

// grammar.garc 中の dat の最大サイズを得る
u32 GrammaticalAttributeProvider::getMaxDataSize()
{
  gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_FILEREAD);
  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  const gfl2::fs::ArcFile * arcFile = man->GetArcFile( ARCID_GRAMMAR );
  size_t maxDataSize = 0; 
  for (int i = 0; i < GARC_grammar_DATA_NUM; i ++)
  {
    size_t dataSize;
    gfl2::fs::Result result = arcFile->GetDataSize( &dataSize, i, fileread_heap );
    if( result.IsSuccess() == false )
    {
      GFL_ASSERT(0);
    }
    if (maxDataSize < dataSize)
    {
      maxDataSize = dataSize;
    }
  }
  return maxDataSize;
}

// 指定した言語の文法特性データ番号を取得
uint GrammaticalAttributeProvider::getDatId(uint lang)
{
  // PM_LANG から読み込み対象のdatへの表
  static const u16 lang_to_grammar_garc_id[] = {
    GARC_grammar_DATA_NUM,    // POKEMON_LANG_NULL
    GARC_grammar_JPN_DAT,
    GARC_grammar_English_DAT,
    GARC_grammar_French_DAT,
    GARC_grammar_Italian_DAT,
    GARC_grammar_German_DAT,
    GARC_grammar_DATA_NUM,    // 欠番
    GARC_grammar_Spanish_DAT,
    GARC_grammar_Korean_DAT,
  };

  if (lang >= GFL_NELEMS(lang_to_grammar_garc_id))
  {
    GFL_ASSERT_MSG(false, "lang %u is out of range", lang);
    return GARC_grammar_English_DAT;
  }

  uint datId = lang_to_grammar_garc_id[lang];
  if (datId >= GARC_grammar_DATA_NUM)
  {
    GFL_ASSERT_MSG(false, "lang %d does not exist");
    return GARC_grammar_English_DAT;
  }
  else
  {
    return datId;
  }
}

//-----------------------------------------------------------------------------
// 文法特性の取得
//-----------------------------------------------------------------------------

// ヘッダへのポインタを取得
inline const GrammarDatHeader* GrammaticalAttributeProvider::getHeader() const
{
  return reinterpret_cast<const GrammarDatHeader*>(mpData);
}

// 各単語の語形変化文字列内での各語形へのオフセット
inline const u8* GrammaticalAttributeProvider::getWordFormOffsets() const
{
  // WordOffsets の直後に存在する
  const GrammarDatHeader* header = getHeader();
  return reinterpret_cast<const u8*>(&header->WordOffsets[header->WordCount]); 
}

// @brief 語形変化する単語の語形を得る
// @param word_id     単語ID
// @param wordform_id 語形ID
const gfl2::str::STRCODE* GrammaticalAttributeProvider::GetWordForm(uint word_id, uint wordform_id) const
{
  static const gfl2::str::STRCODE* const empty_str = L"";
  const GrammarDatHeader* header = getHeader();

  if (word_id > header->WordCount || wordform_id > header->WordFormCount)
  {
    GFL_ASSERT_MSG(false, "word_id(%u) or wordform_id(%u) is out of range", word_id, wordform_id);
    return empty_str;
  }

  // 語形変化文字列へのオフセット
  uint wordOffset = header->WordOffsets[word_id];
  // wordFormOffsets には、「各単語の語形変化文字列内での各語形へのオフセット（WordFormCount個）」が、
  // 単語の種類数（WordCount）だけ並んでいる
  const u8* wordFormOffsets = getWordFormOffsets();
  uint wordFormOffset = wordFormOffsets[word_id * header->WordFormCount + wordform_id];

  return reinterpret_cast<gfl2::str::STRCODE*>(this->mpData + wordOffset + wordFormOffset);
}

//その他の言語の最初の音タイプ判定
//    最初の文字が母音字⇒母音
//    それ以外⇒子音
//
//最初の音タイプの判定は
//・（フランス語）前置詞 de のエリジオン
//・（イタリア語）バトルのニックネーム（デフォルト名）の冠詞変化
//にのみ使用される想定ですが、間違って使用された場合に備えて、
//ほかの言語でも母音字かどうか位は判定しておきます。
uint GrammaticalAttributeProvider::getInitialSoundTypeOther(const gfl2::str::STRCODE* sp)
{
  const uint INITIALSOUNDTYPE_CONS = 0;
  const uint INITIALSOUNDTYPE_VOWEL = 1;

  gfl2::str::STRCODE c = sp[0];
  if (isVowelAlphabet(c))
  {
    return INITIALSOUNDTYPE_VOWEL;
  }
  else
  {
    return INITIALSOUNDTYPE_CONS;
  }
}

//フランス語の最初の音タイプ判定
//    最初の文字が母音字⇒母音（エリジオンする）
//    最初の文字が h で、かつ、次の文字が存在してそれが母音字⇒母音（エリジオンする）
//    それ以外⇒子音（エリジオンしない）
uint GrammaticalAttributeProvider::getInitialSoundTypeFrench(const gfl2::str::STRCODE* sp)
{
  const uint FRENCH_INITIALSOUNDTYPE_CONS = 0;
  const uint FRENCH_INITIALSOUNDTYPE_VOWEL = 1;

  gfl2::str::STRCODE c = sp[0];
  if (c == 'h' || c == 'H')     // 最初の文字が h / H なら次の文字を見る
  {
    c = sp[1]; // sp[0] が EOM ではないので、sp[1] を読んでよい
  }

  if (isVowelAlphabet(c))
  {
    return FRENCH_INITIALSOUNDTYPE_VOWEL;
  }
  else
  {
    return FRENCH_INITIALSOUNDTYPE_CONS;
  }
}

//イタリア語の最初の音タイプ判定
//    最初の文字が母音字⇒母音
//    最初の文字が z, x ,y ⇒ s+子音、ｚ 等 (子音2)
//      (以下3つは2文字以上の場合のみ)
//      最初の2文字が gn, pn, ps ⇒ s+子音、ｚ 等 (子音2)
//      最初の文字が h で、次の文字が存在してそれが母音字⇒母音
//      最初の文字が s で、次の文字が存在してそれが母音字ではない⇒ s+子音、ｚ (子音2)
//    それ以外⇒子音（その他）
uint GrammaticalAttributeProvider::getInitialSoundTypeItalian(const gfl2::str::STRCODE* sp)
{
  const uint ITALIAN_INITIALSOUNDTYPE_CONS = 0;
  const uint ITALIAN_INITIALSOUNDTYPE_VOWEL = 1;
  const uint ITALIAN_INITIALSOUNDTYPE_CONS2 = 2;

  gfl2::str::STRCODE c1 = sp[0];
  // 空文字列なら子音にしておく
  if (c1 == gfl2::str::EOM_CODE)
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS;
  }

  // sp[0] が EOM ではないので、sp[1] を読んでよい
  gfl2::str::STRCODE c2 = sp[1];

  if (isVowelAlphabet(c1))
  {
    return ITALIAN_INITIALSOUNDTYPE_VOWEL;
  }
  else if (c1 == 'z' || c1 == 'Z' || c1 == 'x' || c1 == 'X' || c1 == 'y' || c1 == 'Y')
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS2;
  }
  else if (c2 == gfl2::str::EOM_CODE)  // これより先は2文字以上の場合なので、1文字の場合は子音に確定
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS;
  }
  // 以下2文字以上の場合
  else if (((c1 == 'g' || c1 == 'G') && (c2 == 'n' || c2 == 'N')) ||
           ((c1 == 'p' || c1 == 'P') && (c2 == 'n' || c2 == 'N')) ||
           ((c1 == 'p' || c1 == 'P') && (c2 == 's' || c2 == 'S')))
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS2;
  }
  else if ((c1 == 'h' || c1 == 'H') && isVowelAlphabet(c2))
  {
    return ITALIAN_INITIALSOUNDTYPE_VOWEL;
  }
  else if ((c1 == 's' || c1 == 'S') && !isVowelAlphabet(c2))
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS2;
  }
  else
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS;
  }
}

// @brief 指定した文字列（ユーザー入力文字列を想定）の最初の音タイプを得る
// @param sp   EOM終端の文字列
uint GrammaticalAttributeProvider::GetInitialSoundType(const gfl2::str::STRCODE* sp) const
{
  switch (m_currentLanguage)
  {
  case POKEMON_LANG_FRANCE:
    return getInitialSoundTypeFrench(sp);
  case POKEMON_LANG_ITALY:
    return getInitialSoundTypeItalian(sp);
  default:
    return getInitialSoundTypeOther(sp);
  }
}

// @brief 指定した数が複数かどうかを得る
// @param number 数
bool GrammaticalAttributeProvider::IsPluralNumber(int number) const
{
  if (m_currentLanguage == POKEMON_LANG_FRANCE)
  {
    // F: 0と1だけが単数
    return (number != 0) && (number != 1);
  }
  else
  {
    // EIGS: 1だけが単数（0は複数）
    return number != 1;
  }
}

// @note 必要なら小数の単複も実装する
// F: 絶対値が2未満なら単数、他は複数
// EIGS: 常に複数

// 指定した文字が母音のアルファベットかどうかを得る
bool GrammaticalAttributeProvider::isVowelAlphabet(gfl2::str::STRCODE c)
{
  //// [0, 255] の文字のうち、母音のアルファベット
  //static const u32 IsVowelAlphabetTable[8] = {
  #include "IsVowelAlphabetTable.cdat"

  if (c >= sizeof(IsVowelAlphabetTable)*8)
  {
      return false;
  }

  int elem_index = c / (sizeof(u32) * 8);
  int bit_index = c % (sizeof(u32) * 8);
  return (IsVowelAlphabetTable[elem_index] & (1 << bit_index)) != 0;
}

GFL_NAMESPACE_END(print)
