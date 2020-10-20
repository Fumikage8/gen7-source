//============================================================================================
/**
 * @file  wordset.cpp
 * @brief 文字列中の単語埋め込み処理クラス
 * @author  taya
 * @date  2011.03.03
 */
//============================================================================================
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>

#include "System/include/PokemonVersion.h"
#include "Print/include/WordSet.h"
#include "Print/include/hangul.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "Savedata/include/MyItemSaveDef.h"

#include "arc_index/message.gaix"
#include "niji_conv_header/app/grammar/grammar_wordform.h"
#include "niji_conv_header/message/msg_tokusei.h"
#include "niji_conv_header/message/msg_wazaname.h"
#include "niji_conv_header/message/msg_itemname_wordset.h"
#include "niji_conv_header/message/msg_ribbon.h"
#include "niji_conv_header/message/msg_seikaku.h"
#include "niji_conv_header/message/msg_bag_pocket.h"
#include "niji_conv_header/message/msg_trname.h"
#include "niji_conv_header/message/msg_trtype.h"
#include "niji_conv_header/message/msg_jf_attraction.h"
#include "niji_conv_header/message/msg_kisekae_item_name.h"


/*
 * @note
 *  警告[4245]を無視する
 *  warning C4245: '=' : 'int' から 'u8' に変換しました。signed/unsigned が一致しません。
 */
GFL_WARNING_PUSH
GFL_WARNING_WIN32_IGNORE(4245)


GFL_NAMESPACE_BEGIN(print)

// GrammerFromSex で PM_MALE PM_FEMALE PM_NEUTRAL から Grammer にキャストしてよいことを確認する
// STATIC_ASSERT をヘッダに書きたくない一方で STATIC_ASSERT を関数外に書けないため、ダミーの関数を用意している
namespace {
  inline void AssertEqualPMSexAndGrammer()
  {
    STATIC_ASSERT( PM_MALE == WordSet::GRAMMER_MALE );
    STATIC_ASSERT( PM_FEMALE == WordSet::GRAMMER_FEMALE );
    STATIC_ASSERT( PM_NEUTRAL == WordSet::GRAMMER_NEUTRAL );
  }
}

const f32 WordSet::DEFAULT_LIMIT_WIDTH_POKEMON = 6.5f;
const f32 WordSet::DEFAULT_LIMIT_WIDTH_PERSON  = 6.5f;
const u16 WordSet::REF_WORD_ID_HERO = 255;   // 主人公の文法特性を参照する
const u16 WordSet::REF_WORD_ID_EXPANDING_WORD = 254; // 現在展開中の単語の文法特性を参照する
const u16 WordSet::REF_WORD_ID_NUM = 253;   // 単複の文法特性を参照する


//--------------------------------------------------------------
// Constructor / Destructor
//--------------------------------------------------------------
WordSet::WordSet( gfl2::heap::HeapBase* pHeap )
 : m_tmpBuf(NULL)
 , m_word(NULL)
 , m_bAreaWidthTagEnable(false)
 , m_wordSetLoader(NULL)
{
  setupMembers( DEFAULT_WORD_MAX, DEFAULT_BUF_LEN, pHeap );
}

WordSet::WordSet( u32 wordMax, u32 bufLen, gfl2::heap::HeapBase* pHeap )
 : m_tmpBuf(NULL)
 , m_word(NULL)
 , m_bAreaWidthTagEnable(false)
 , m_wordSetLoader(NULL)
{
  setupMembers( wordMax, bufLen, pHeap );
}

WordSet::~WordSet( )
{
  if( m_tmpBuf )
  {
    GFL_DELETE  m_tmpBuf;
  }

  if( m_word )
  {
    for(u32 i=0; i<m_wordMax; ++i)
    {
      if( m_word[i].str ){
        GFL_DELETE  m_word[i].str;
      }
    }
    GflHeapFreeMemory( m_word );
  }

#if 0
  if ( m_wordSetLoader )
  {
    GFL_DELETE m_wordSetLoader;
  }
#endif
}

void WordSet::setupMembers( u32 wordMax, u32 bufLen, gfl2::heap::HeapBase* pHeap )
{
  GFL_ASSERT( wordMax );  // @check
  GFL_ASSERT( bufLen );   // @check

  m_wordMax = wordMax;
  m_heapHandle = pHeap;

  m_tmpBuf = GFL_NEW(pHeap) gfl2::str::StrBuf( bufLen, pHeap );
  m_word = reinterpret_cast<WORD*>( GflHeapAllocMemory( pHeap, sizeof(WORD) * wordMax ) );

  if( m_word )
  {
    for(u32 i=0; i<wordMax; ++i)
    {
      m_word[i].param = defaultWordParam();
      m_word[i].str = GFL_NEW(pHeap) gfl2::str::StrBuf( bufLen, pHeap );
      if( m_word[i].str == NULL )
      {
        m_wordMax = i;
        break;
      }
    }
  }
  else
  {
    m_wordMax = 0;
  }
}

void WordSet::clearWord( u32 wordIdx )
{
  if( wordIdx < m_wordMax )
  {
    m_word[wordIdx].param = defaultWordParam();
    m_word[wordIdx].str->Clear();
  }
}

// pml::Sex から Grammer に変換。ポケモンの性別にはこれを使用する。
WordSet::Grammer WordSet::GrammerFromPmlSex(pml::Sex sex)
{
  switch (sex)
  {
  case pml::SEX_MALE:
    return GRAMMER_MALE;
  case pml::SEX_FEMALE:
    return GRAMMER_FEMALE;
  case pml::SEX_UNKNOWN:
  default:
    // 性別不明の場合、FIS: 男性、G: 中性として扱う
    if (System::GetLang() == POKEMON_LANG_GERMANY)
      return GRAMMER_NEUTRAL;
    else
      return GRAMMER_MALE;
  }
}

// （アイテム相当のものの）個数から Form に変換。
WordSet::Form WordSet::FormFromCount(u32 count)
{
  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
  return provider->IsPluralNumber(count) ? FORM_PLURAL : FORM_SINGLE;
}

// 初期値0でWORD_PARAMを生成
inline WordSet::WORD_PARAM WordSet::defaultWordParam()
{
  WORD_PARAM param;
  param.raw = 0;
  return param;
}

// GrammaticalAttribute がある場合（MsgDataから文字列を取得した場合）の WORD_PARAM 生成
inline WordSet::WORD_PARAM WordSet::createWordParam(print::GrammaticalAttribute attribute, WordSet::Form form)
{
  WORD_PARAM param = defaultWordParam();
  param.attribute = attribute;
  param.is_initialsound_calculated = 1; // GrammaticalAttribute がある⇒ユーザー入力名ではないので、文法タグで参照したい場合は翻訳者が入力してある
  param.form = form;
  return param;
}

// GrammaticalAttribute がない場合（ユーザー入力名の場合）の WORD_PARAM 生成
inline WordSet::WORD_PARAM WordSet::createWordParam(WordSet::Grammer grm, WordSet::Form form)
{
  WORD_PARAM param = defaultWordParam();
  param.attribute.Grammer = grm;
  param.form = form;
  return param;
}

//======================================================================================================
// 文字列展開
//======================================================================================================

void WordSet::Expand( gfl2::str::StrBuf* dstbuf, const gfl2::str::StrBuf* srcbuf )
{
  m_expandingWordId = 0;
  m_lastWordLastChar = 0;
  m_forceWordParam.raw = 0;
  m_forceWordParamMask.raw = 0;

  dstbuf->Clear();
  expandCore(dstbuf, srcbuf, false);
}

//------------------------------------------------------------------
/**
 * 1回だけ単語（m_word）も再帰的な展開を行う展開
 *
 * @param[out]   dstbuf   生成後の文字列を格納するバッファ
 * @param[in]    srcbuf   生成元となる（タグコードが埋め込まれた）文字列
 * @param[in]    recursed 再帰したかどうか。
 */
//------------------------------------------------------------------
void WordSet::expandCore( gfl2::str::StrBuf* dstbuf, const gfl2::str::StrBuf* srcbuf, bool recursed)
{
  const gfl2::str::STRCODE* src = srcbuf->GetPtr();

  while( *src != gfl2::str::EOM_CODE )
  {
    if( *src == gfl2::str::TAG_START_CODE )
    {
      if(print::IsNameAreaTagGroup(src))
      {
        const gfl2::str::STRCODE* p = src;
        src = gfl2::str::SkipTagCode( src );
        while(p < src)
        {
          dstbuf->AddCode( *p++ );
        }
			}
      else if( print::IsWordSetTagGroup(src) )
      {
        src = processWordSetTag( dstbuf, src, recursed );
      }
      else if( print::IsKoreanParticleTag(src))
      {
        src = processKoreanParticleTag( dstbuf, src );
      }
      else if ( print::IsStringSelectTag(src) )
      {
        src = processStringSelectTag( dstbuf, src );
      }
      else if ( print::IsWordFormTag(src) )
      {
        src = processWordFormTag( dstbuf, src );
      }
      else if ( print::IsGrammarForceTag(src) )
      {
        src = processGrammarForceTag( dstbuf, src );
      }
      else
      {
        const gfl2::str::STRCODE* p = src;

        src = gfl2::str::SkipTagCode( src );
        while(p < src)
        {
          dstbuf->AddCode( *p++ );
        }
      }
    }
    else
    {
      dstbuf->AddCode( *src++ );
    }
  }
}

//------------------------------------------------------------------
/**
 * 単語埋め込み対象タグの処理
 */
//------------------------------------------------------------------
const gfl2::str::STRCODE* WordSet::processWordSetTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src, bool recursed)
{
  u32 word_id = gfl2::str::GetTagParam( src, 0 );

  if( word_id >= m_wordMax )
  {
    GFL_ASSERT_MSG(0, "word_id:%d, numWordMax:%d", word_id, m_wordMax );
    return gfl2::str::SkipTagCode( src );
  }

  // 領域計算開始のタグ＆パラメータを埋め込む
  if( m_word[ word_id ].areaWidthChars )
  {
    dstbuf->AddCode( gfl2::str::TAG_START_CODE );
    dstbuf->AddCode( 2 );
    dstbuf->AddCode( print::CreateTagCode(print::TAGGROUP_GENERAL_CTRL, print::TAGIDX_GENERAL_FIX_WIDTH_START) );
    dstbuf->AddCode( m_word[ word_id ].areaWidthChars );
  }

  if( print::IsWordSetNumberTagGroup(src) )
  {
    // 数字タグを桁区切りを入れながら展開する
    // 2番目のタグパラメータがあれば、それが桁区切りの文字
    gfl2::str::STRCODE separator;
    if ( gfl2::str::GetTagParamCount(src) > 1)
    {
      separator = gfl2::str::GetTagParam( src, 1 );
//      if((separator == 0) && (System::GetLang() == POKEMON_LANG_ITALY))  //イタリア語だけデフォルトでカンマに変更 k.ohno 2016.01.21
      {
//        separator = 0x002E;//ピリオドの事
      }
    }
    else
    {
      separator = 0;
    }
    expandNumber(dstbuf, word_id, separator);
  }
  else
  {
    if( recursed )
    {
      // 再帰は一度だけ
      dstbuf->AddStr( *(m_word[ word_id ].str) );
    }
    else
    {
      // m_word に登録された単語中のタグもExpandする（文字列選択タグを想定）
      m_expandingWordId = word_id;
      expandCore( dstbuf, m_word[ word_id ].str, true );
    }
  }

  // 最後に展開した単語の最後の文字を保存（韓国の助詞変化用
  // ※韓国版で実際に起こることはないが、m_wordがタグで終了しているとうまく動かない。
  m_lastWordLastChar = getLastChar( m_word[ word_id ].str );

  // 領域計算終了のタグ＆パラメータを埋め込む
  if( m_word[ word_id ].areaWidthChars )
  {
    dstbuf->AddCode( gfl2::str::TAG_START_CODE );
    dstbuf->AddCode( 1 );
    dstbuf->AddCode( print::CreateTagCode(print::TAGGROUP_GENERAL_CTRL, print::TAGIDX_GENERAL_FIX_WIDTH_END) );
  }

  // 単語を展開したら、「常に複数形」など文法特性を強制するフラグを落とす
  m_forceWordParam.raw = 0;
  m_forceWordParamMask.raw = 0;

  return gfl2::str::SkipTagCode( src );
}


//------------------------------------------------------------------
/**
 * 桁区切りを入れながら数値を展開する
 *
 * @param[out]   dstbuf    数値を追加するバッファ
 * @param[in]    word_id   数値の入っている単語番号
 * @param[in]    separator 桁区切りの文字。0であれば桁区切りしない。
 */
//------------------------------------------------------------------
void WordSet::expandNumber( gfl2::str::StrBuf* dstbuf, uint word_id, gfl2::str::STRCODE separator )
{
  const gfl2::str::StrBuf* srcbuf = m_word[ word_id ].str;

  if( (separator == 0) || (srcbuf->GetLength() == 0) )
  {
    dstbuf->AddStr( *srcbuf );
    return;
  }

  const uint lang = System::GetLang();
  // 桁区切りを行う最小の桁数。スペイン語は、5桁から（4桁のときに桁区切りを入れない）、その他は4桁から。2016.2.9 k.ohno
  const int minSeparatedKetaCount = (lang == POKEMON_LANG_SPAIN) ? 5 : 4;

  // 最初の数字の位置と、数字の桁数を求める（最初の数字の後はすべて数字と仮定する）
  // ※最初の数字から連続する数字を処理対象にすれば、小数にも対応できる（小数点で止まる）
  const gfl2::str::STRCODE* pFirstDigit;
  for( pFirstDigit = srcbuf->GetPtr(); *pFirstDigit != gfl2::str::EOM_CODE; pFirstDigit++)
  {
    if ('0' <= *pFirstDigit && *pFirstDigit <= '9')
      break;
  }

  const int ketaCount = srcbuf->GetLength() - (pFirstDigit - srcbuf->GetPtr());
  if (ketaCount < minSeparatedKetaCount)
  {
    dstbuf->AddStr( *srcbuf );
    return;
  }

  // 最初の数字の前までをdstbufに追加
  const gfl2::str::STRCODE* src = srcbuf->GetPtr();
  while( src < pFirstDigit )
  {
    // 通常ありえないが、タグ内に桁区切りを入れてしまうことを回避
    if (*src == gfl2::str::TAG_START_CODE)
    {
      GFL_ASSERT_MSG(false, "TAG_START_CODE contained in a digit word");
      return;
    }
    dstbuf->AddCode( *src );
    src ++;
  }

  // 桁区切りを入れながら数字をdstbufに追加
  for( int currentKeta = ketaCount - 1; currentKeta >= 0; currentKeta-- ) // currentKeta : 0オリジンの現在の桁
  {
    // 通常ありえないが、タグ内に桁区切りを入れてしまうことを回避
    if (*src == gfl2::str::TAG_START_CODE)
    {
      GFL_ASSERT_MSG(false, "TAG_START_CODE contained in a digit word");
      return;
    }

    dstbuf->AddCode( *src );
    src ++;
    // 4桁目の後（currentKeta=3）、7桁目の後（currentKeta=6）、……に桁区切りを入れる
    if( (currentKeta % 3 == 0) && (currentKeta != 0) )
    {
      dstbuf->AddCode( separator );
    }
  }
}

//------------------------------------------------------------------
/**
 * 韓国語の助詞タグの処理
 */
//------------------------------------------------------------------
const gfl2::str::STRCODE* WordSet::processKoreanParticleTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src)
{
  const uint particle_id = gfl2::str::GetTagParam( src, 0 );  // タグ中の助詞ID
  const gfl2::str::STRCODE particle = hangul::GetParticle( m_lastWordLastChar, particle_id );   // 追加すべき助詞

  if (particle != 0)
  {
    dstbuf->AddCode( particle );
  }

  return gfl2::str::SkipTagCode( src );
}

//------------------------------------------------------------------
/**
 * 文字列選択タグの処理
 */
//------------------------------------------------------------------
const gfl2::str::STRCODE* WordSet::processStringSelectTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src)
{
  const uint MAX_SELSTR_COUNT = 4;  // 選択する文字列の最大数

  const uint param_count = gfl2::str::GetTagParamCount(src);
  if( param_count == 0 )
  {
    GFL_ASSERT( 0 );
    return src;
  }
  const uint str_count = (param_count - 1) * 2;

  if (param_count > 1 + MAX_SELSTR_COUNT/2)
  {
    GFL_ASSERT_MSG(false, "TagParamCount is too large");
    return gfl2::str::SkipTagCode( src );
  }

  // 選択する文字列を決定
  const uint ref_word_id = gfl2::str::GetTagParam(src, 0);
  uint select_index = getStringSelectIndex(print::GetTagIndex(src), ref_word_id);
  if (select_index >= str_count)
  {
    GFL_ASSERT_MSG(false, "select_index is too large");
    select_index = 0;
  }

  // タグパラメータから各文字列の長さを取得
  u8 str_length[MAX_SELSTR_COUNT];
  gfl2::std::MemClear( str_length, sizeof(str_length) );

  for (uint i = 0; i < param_count-1; i ++)
  {
    uint param = gfl2::str::GetTagParam(src, i+1);
    str_length[i*2    ] = param & 0xff;
    str_length[i*2 + 1] = (param >> 8);
  }

  // 選択された文字列をdstbufに追加
  src = gfl2::str::SkipTagCode(src);
  for (uint i = 0; i < str_count; i ++)
  {
    if (i == select_index)
    {
      for (int k = 0; k < str_length[select_index]; k ++)
      {
        dstbuf->AddCode(*src);
        src ++;
      }
    }
    else
    {
      src += str_length[i];
    }
  }
  return src;
}

//------------------------------------------------------------------
/**
 *  tag_index で指定した文字列選択タグに対して、現在の言語(xy::GetLang) において
 *  ref_word_id で指定した単語の性、数に応じて選択すべき文字列のインデックスを得る
 */
//------------------------------------------------------------------
uint WordSet::getStringSelectIndex(uint tag_index, uint ref_word_id) const
{
  const WORD_PARAM param = getRefWordParam(ref_word_id);
  const uint grammer = param.attribute.Grammer;
  // 文字列選択タグにおいては、非可算は単数として扱う
  const uint form = (param.form == FORM_UNCOUNTABLE) ? FORM_SINGLE : param.form;

  switch (tag_index)
  {
  case print::TAGIDX_STRING_SELECT_BY_GENDER:
    return grammer;
  case print::TAGIDX_STRING_SELECT_BY_QUANTITY:
    return form;
  case print::TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY:
    // ドイツ語以外は、中性がない。
    // 単数男性、単数女性、複数男性、複数女性の4種類の変化となる。
    return form*2 + grammer;
  case print::TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY_GERMAN:
    // ドイツ語は、中性があり、また、複数の場合は性による変化がない。
    // 単数男性、単数女性、単数中性、複数の4種類の変化となる。
    if (form == FORM_PLURAL)
    {
      return 3;
    }
    else
    {
      return grammer;
    }
  default:
    GFL_ASSERT_MSG(false, "tag_index %u is not a select-tag index", tag_index);
    return 0;
  }
}

//------------------------------------------------------------------
/**
 * 語形変化タグの処理
 */
//------------------------------------------------------------------
const gfl2::str::STRCODE* WordSet::processWordFormTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src)
{
  const uint word_id = print::GetWordFormTagWordIndex(src);

  const uint ref_id = gfl2::str::GetTagParam(src, 0);
  const uint wordform_id = getWordFormIndex(ref_id);

  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
  const gfl2::str::STRCODE* wordform = provider->GetWordForm(word_id, wordform_id);
  for (const gfl2::str::STRCODE* wp = wordform; *wp != gfl2::str::EOM_CODE; wp ++)
  {
    dstbuf->AddCode(*wp);
  }

  return gfl2::str::SkipTagCode(src);
}

//------------------------------------------------------------------
/**
 *  現在の言語(xy::GetLang)について、
 *  ref_word_id で指定した単語の性、数、最初の音に応じて代入すべき語形のインデックスを得る
 */
//------------------------------------------------------------------
uint WordSet::getWordFormIndex(uint ref_word_id) const
{
  const WORD_PARAM param = getRefWordParam(ref_word_id);
  const uint form = param.form;
  const uint grammer = param.attribute.Grammer;
  const uint initialsound = param.attribute.InitialSound;

  // 語形の並び順は、語形変化表（wordforms.csv）を参照
  switch (System::GetLang())
  {
  case POKEMON_LANG_ENGLISH: // 数の種類*3、性の種類*1、最初の音の種類*2
    return form*1*2 + initialsound;
  case POKEMON_LANG_FRANCE:  // 数の種類*3、性の種類*2、最初の音の種類*2
    return form*2*2 + grammer*2 + initialsound;
  case POKEMON_LANG_SPAIN:   // 数の種類*3、性の種類*2、最初の音の種類*3
  case POKEMON_LANG_ITALY:   // 数の種類*3、性の種類*2、最初の音の種類*3
    return form*2*3 + grammer*3 + initialsound;
  case POKEMON_LANG_GERMANY:  // 数の種類*3、性の種類*3、最初の音の種類*1
    return form*1*3 + grammer;
  case POKEMON_LANG_JAPAN:
  case POKEMON_LANG_KOREA:
    return 0;
  default:
    return 0;
  }
}

//------------------------------------------------------------------
/**
 * 文法特性強制タグの処理
 */
//------------------------------------------------------------------
const gfl2::str::STRCODE* WordSet::processGrammarForceTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src)
{
  uint tag_index = print::GetTagIndex(src);
  switch (tag_index)
  {
  case print::TAGIDX_GRAMMAR_FORCE_SINGULAR:
    m_forceWordParam.form = FORM_SINGLE;
    m_forceWordParamMask.form = ~0;
    break;
  case print::TAGIDX_GRAMMAR_FORCE_PLURAL:
    m_forceWordParam.form = FORM_PLURAL;
    m_forceWordParamMask.form = ~0;
    break;
  case print::TAGIDX_GRAMMAR_FORCE_MASCULINE:
    m_forceWordParam.attribute.Grammer = GRAMMER_MALE;
    m_forceWordParamMask.attribute.Grammer = ~0;
    break;
  default:
    GFL_ASSERT_MSG(false, "tag_index %u is not a grammar-force-tag index", tag_index);
    break;
  }

  return gfl2::str::SkipTagCode(src);
}

//------------------------------------------------------------------
/**
 *  ref_word_id で指定した単語の WORD_PARAM を、特殊な参照番号（主人公、展開中の単語）
 *  および文法特性の強制を考慮して得る
 */
//------------------------------------------------------------------
WordSet::WORD_PARAM WordSet::getRefWordParam(uint ref_word_id) const
{
  WORD_PARAM param = defaultWordParam();

  if (ref_word_id == REF_WORD_ID_HERO)
  {
    // REF_WORD_ID_HERO を指定されたら、性は主人公の性別、数は単数にする
    u8 hero_sex = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetSex();
    param.attribute.Grammer = GrammerFromSex(hero_sex);
    // REF_WORD_ID_HERO を使用しているなら文中に主人公名は表示されないので、最初の音はデフォルトでOK
    param.is_initialsound_calculated = true;
  }
  else if (ref_word_id == REF_WORD_ID_NUM){
    //REF_WORD_ID_NUMを指定されたら ポケモンの数を取得し
    u8 num = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerPartyConst()->GetMemberCount();
    if(num>1){
      param.attribute.IsAlwaysPlural=1;
    }
    else{
      param.attribute.IsAlwaysPlural=0;
    }
    param.is_initialsound_calculated = true;
  }
  else
  {
    if (ref_word_id == REF_WORD_ID_EXPANDING_WORD)
    {
      // REF_WORD_ID_EXPANDING_WORD なら、展開中（再帰元）の単語のWORD_PARAMを参照
      ref_word_id = m_expandingWordId;
    }

    if (ref_word_id >= m_wordMax)
    {
      GFL_ASSERT_MSG(false, "ref_word_id %u is too large", ref_word_id);
      return defaultWordParam();
    }
    param = m_word[ref_word_id].param;
  }

  // 文法特性の強制
  param.raw = (param.raw & ~m_forceWordParamMask.raw) | (m_forceWordParam.raw & m_forceWordParamMask.raw);

  // 可算・常に複数を考慮して、数を補正
  if (param.attribute.IsUncountable)
  {
    param.form = FORM_UNCOUNTABLE;
  }
  else if (param.attribute.IsAlwaysPlural)
  {
    param.form = FORM_PLURAL;
  }

  // 最初の音が判定されていないなら判定する
  if (!param.is_initialsound_calculated)
  {
    print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
    param.attribute.InitialSound = provider->GetInitialSoundType(m_word[ref_word_id].str->GetPtr());
  }

  return param;
}

//------------------------------------------------------------------
/**
 * strbufから、タグをのぞいた最後の文字を取得する
 */
//------------------------------------------------------------------
gfl2::str::STRCODE WordSet::getLastChar(const gfl2::str::StrBuf* strbuf)
{
  const gfl2::str::STRCODE* sp = strbuf->GetPtr();
  const gfl2::str::STRCODE* sp_end = sp + strbuf->GetLength();
  gfl2::str::STRCODE last_char = gfl2::str::EOM_CODE;

  while (sp < sp_end)
  {
    gfl2::str::STRCODE code = *sp;
    if (code == gfl2::str::TAG_START_CODE)
    {
      sp = gfl2::str::SkipTagCode(sp);
    }
    else {
      last_char = code;
      sp ++;
    }
  }

  return last_char;
}


//------------------------------------------------------------------
/**
 * dstbuf に、srcptr の指すEOM終端の文字列を追加する
 */
//------------------------------------------------------------------
void WordSet::addStr(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* srcptr)
{
  for (const gfl2::str::STRCODE* sp = srcptr; *sp != gfl2::str::EOM_CODE; sp ++)
  {
    dstbuf->AddCode(*sp);
  }
}

//======================================================================================================
// 全文字列クリア
//======================================================================================================
void WordSet::ClearAllBuffer( void )
{
  for(u32 i=0; i<m_wordMax; ++i)
  {
    clearWord( i );
  }
}

//======================================================================================================
// 単語セットモジュールに各種単語を登録する
//======================================================================================================

//------------------------------------------------------------------
/**
 * 単語文字列＆パラメータを指定バッファに登録
 * 直接使用する場合は、ローカライズ対応を考慮すること。
 *
 * @param   wordset   単語セットモジュール
 * @param   bufID   バッファＩＤ
 * @param   str     文字列
 * @param   param   文字列に付随するパラメータ
 * @param   areaWidthChars   文字の描画領域を限定するタグを埋め込む時の計算用。0ならなにもせず
 *
 */
//------------------------------------------------------------------
void WordSet::registerWord( u32 bufID, const gfl2::str::StrBuf& str, WORD_PARAM param, u16 areaWidthChars )
{
  GFL_ASSERT_STOP_MSG(bufID < m_wordMax, "bufID=%d, wordmax=%d", bufID, m_wordMax);
  GFL_ASSERT_STOP(m_word);

  if( bufID < m_wordMax )
  {
    m_word[bufID].param = param;
    m_word[bufID].str->SetStr( str );
    //ここを有効にすればタグごとの縮小が動くようになる。
    //ただ、kujiraではMsgWin全体を見ての縮小が入っているので不要。
    //m_word[bufID].areaWidthChars = (m_bAreaWidthTagEnable)? areaWidthChars : 0;
    m_word[bufID].areaWidthChars = 0;
  }
}

//------------------------------------------------------------------
/**
 * メッセージアーカイブ内の指定ID文字列を指定バッファにロード
 *
 * @param   dstbuf  読み込み先バッファ
 * @param   dat_id  アーカイブ内データID
 * @param   msg_id  文字列ID
 *
 * @return  u16     メッセージのユーザー設定パラメータ

 */
//------------------------------------------------------------------
u16 WordSet::loadStringFromMsgArc( gfl2::str::StrBuf& dstbuf, u32 dat_id, u32 msg_id )
{
  gfl2::str::MsgData *man = NULL;
  bool malloc = false;
  man = WordSetLoader::GetGlobalMsgData( dat_id );
  if ( man == NULL && m_wordSetLoader )
  {
    man = m_wordSetLoader->GetMsgData( dat_id );
  }
  if ( man == NULL )
  {
    man = GFL_NEW_LOW(m_heapHandle) gfl2::str::MsgData( print::GetMessageArcId(), dat_id, m_heapHandle->GetLowerHandle() );
    malloc = true;
  }
  u16 userParam = 0;
  if( man )
  {
    userParam = man->GetString( msg_id, dstbuf );
    if( malloc == true ){
      GFL_DELETE  man;
    }
  }
  return userParam;
}

//------------------------------------------------------------------
/**
 * メッセージアーカイブ内の、指定ID文字列をバッファ展開
 *
 * @param   bufID   バッファＩＤ
 * @param   dat_id  アーカイブ内データID
 * @param   msg_id  文字列ID
 * @param   form    単複
 *
 */
//------------------------------------------------------------------
inline void WordSet::registerWordFromMsgArc( u32 bufID, u32 arc_id, u32 msg_id, Form form)
{
  u16 userParam = loadStringFromMsgArc( *m_tmpBuf, arc_id, msg_id );
  print::GrammaticalAttribute grammaticalAttribute = print::GrammaticalAttribute::FromMsgDataUserParam(userParam);
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grammaticalAttribute, form) );
}

//------------------------------------------------------------------
/**
 * MsgDataから文字列を取得して登録。これを使うと、MsgData の userParam から文法パラメータをセットします。
 */
//------------------------------------------------------------------
void WordSet::RegisterWord( u32 bufID, gfl2::str::MsgData* man, u32 strID, Form form)
{
  u16 userParam = man->GetString(strID, *m_tmpBuf);
  print::GrammaticalAttribute grammaticalAttribute = print::GrammaticalAttribute::FromMsgDataUserParam(userParam);
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grammaticalAttribute, form) );
}

//------------------------------------------------------------------
/**
 * 任意文字列をパラメータ付きで登録。MsgDataから取得した場合こちらを使う。
 */
//------------------------------------------------------------------
void WordSet::RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, print::GrammaticalAttribute grammaticalAttribute, Form form)
{
  this->registerWord( bufID, word, createWordParam(grammaticalAttribute, form) );
}

//------------------------------------------------------------------
/**
 * 任意文字列をパラメータ付きで登録。ローカライズ対応を考慮すること。
 */
//------------------------------------------------------------------
void WordSet::RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, Grammer grm, Form form )
{
  this->registerWord( bufID, word, createWordParam(grm, form) );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのニックネームを登録 (CoreParam)
 *
 * @note ppのタマゴフラグが立っていてもニックネームを代入しますが、
 *       タマゴフラグ立っているときはニックネームにタマゴと入っているので
 *       そのまま代入すればOKです。
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeNickName( u32 bufID, const pml::pokepara::CoreParam* pp )
{
  pp->GetNickName( m_tmpBuf );

  Grammer grm = GrammerFromPmlSex(pp->GetSex());
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm), (u16)(6.25f*256) );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのニックネームを登録 (文字列、性別)
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeNickName( u32 bufID, const gfl2::str::StrBuf& str, pml::Sex sex )
{
  Grammer grm = GrammerFromPmlSex(sex);
  this->registerWord( bufID, str, createWordParam(grm), (u16)(6.25f*256) );
}


//------------------------------------------------------------------
/**
 * 指定バッファにポケモン種族名を登録 (CoreParam)
 *
 * @note ppのタマゴフラグが立っていてもポケモン種族名を代入します
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeMonsName( u32 bufID, const pml::pokepara::CoreParam* pp )
{
  MonsNo monsno;

  monsno = pp->GetMonsNo();
  this->RegisterPokeMonsNameNo( bufID, monsno );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのモンスター名を登録 (CoreParam)
 *
 * @note ppのタマゴフラグが立っているときはタマゴを代入し、立っていないときはポケモン種族名を代入します。
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeMonsNameOrTamago( u32 bufID, const pml::pokepara::CoreParam* pp )
{
  MonsNo monsno = MONSNO_TAMAGO;

  if(!(pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG)))
  {
    monsno = pp->GetMonsNo();
  }
  this->RegisterPokeMonsNameNo( bufID, monsno );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモン種族名を登録 (MonsNo)
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeMonsNameNo( u32 bufID, MonsNo monsno )
{
  u16 userParam = pml::personal::GetMonsName( m_tmpBuf, monsno );
  this->RegisterPokeMonsNameStrBuf( bufID, m_tmpBuf, userParam );
}

//------------------------------------------------------------------
/**
 * @brief  指定バッファにポケモン種族名を登録 (MsgData)
 */
//------------------------------------------------------------------
void   WordSet::RegisterPokeMonsNameMsgData( u32 bufID, gfl2::str::MsgData* msgData, u32 strID )
{
  u16 userParam = msgData->GetString( strID, *m_tmpBuf );
  this->RegisterPokeMonsNameStrBuf( bufID, m_tmpBuf, userParam );
}

//------------------------------------------------------------------
/**
 * @brief  指定バッファにポケモン種族名を登録 (StrBuf)
 */
//------------------------------------------------------------------
void   WordSet::RegisterPokeMonsNameStrBuf( u32 bufID, const gfl2::str::StrBuf* strBuf, u16 userParam )
{
  print::GrammaticalAttribute grammaticalAttribute = print::GrammaticalAttribute::FromMsgDataUserParam( userParam );
  this->registerWord( bufID, *strBuf, createWordParam(grammaticalAttribute, FORM_SINGLE) );
}


//------------------------------------------------------------------
/**
 * 指定バッファにポケモンの親名を登録
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeOyaName( u32 bufID, const pml::pokepara::CoreParam* pp )
{
  pp->GetParentName( m_tmpBuf );
  Grammer grm = (pp->GetParentSex() == pml::SEX_FEMALE) ? GRAMMER_FEMALE : GRAMMER_MALE;
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモンの過去親名を登録
 */
//------------------------------------------------------------------
void WordSet::RegisterPokePastOyaName( u32 bufID, const pml::pokepara::CoreParam* pp )
{
  pp->GetPastParentsName( m_tmpBuf );
  Grammer grm = (pp->GetPastParentsSex() == pml::SEX_FEMALE) ? GRAMMER_FEMALE : GRAMMER_MALE;
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}

//------------------------------------------------------------------
/**
 * 指定バッファにトレイ名を登録
 */
//------------------------------------------------------------------
void WordSet::RegisterBoxTrayName( u32 bufID, const Savedata::BOX* p_box, const u32 tray_no )
{
  const gfl2::str::STRCODE* p_code = p_box->GetTrayName( tray_no );
  m_tmpBuf->SetStr( p_code );

  this->registerWord( bufID, *m_tmpBuf, defaultWordParam() );
}

//------------------------------------------------------------------
/**
 * 指定バッファにトレーナー種別を登録
 *
 * @param bufID   何番のバッファに登録するか
 * @param trID    トレーナータイプ ( TRTYPE_xxxx )
 *
 */
//------------------------------------------------------------------
void WordSet::RegisterTrTypeName( u32 bufID, u16 trType )
{
  GFL_ASSERT(bufID < m_wordMax);        //@check
  GFL_ASSERT(trType < msg_trtype_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_trtype_DAT, trType );
}

//------------------------------------------------------------------
/**
 * 指定バッファにトレーナー名を登録
 *
 * @param bufID   何番のバッファに登録するか
 * @param trID    トレーナーID
 */
//------------------------------------------------------------------
void WordSet::RegisterTrainerName( u32 bufID, u32 trID )
{
  GFL_ASSERT(bufID < m_wordMax);      //@check
  GFL_ASSERT(trID < msg_trname_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_trname_DAT, trID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにトレーナータイプ名とトレーナー名を結合したものを登録（フランス語、スペイン語のバトル中トレーナータイプ表示に対応）
 * 結合パターン番号は、トレーナータイプ名の GrammaticalAttribute::ExtraAttribute に入っている。
 *
 * @param   dstBufID           設定先バッファID。trTypeNameBufID、trainerNameBufIDと同じでもよい。
 * @param   trTypeNameBufID    トレーナータイプ名をセットしたバッファID
 * @param   trainerNameBufID   トレーナー名をセットしたバッファID
 *
 * @note フランス語、スペイン語における結合パターンをハードコードしてある。
 *       結合した文字列を生成するためのWordSetを生成するメモリ消費や、実装コストを考慮して、ハードコードを選択した
 */
//------------------------------------------------------------------
void WordSet::RegisterTrTypeNameAndTrainerName(u32 dstBufID, u32 trTypeNameBufID, u32 trainerNameBufID)
{
  // 範囲チェック
  if ( dstBufID >= m_wordMax || trTypeNameBufID >= m_wordMax || trainerNameBufID >= m_wordMax)
  {
    GFL_ASSERT_MSG( false, "dstBufID=%d, trTypeNameBufID=%d, trainerNameBufID = %d, wordmax=%d", dstBufID, trTypeNameBufID, trainerNameBufID, m_wordMax );
    // 空文字列をセットしておく
    m_tmpBuf->Clear();
    this->registerWord( dstBufID, *m_tmpBuf, defaultWordParam() );
    return;
  }

  const WORD* trTypeNameWord = &m_word[trTypeNameBufID];
  const WORD* trainerNameWord = &m_word[trainerNameBufID];
  const gfl2::str::StrBuf* trTypeName = trTypeNameWord->str;
  const gfl2::str::StrBuf* trainerName = m_word[trainerNameBufID].str;

  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);

  // 結合パターン番号
  const uint pattern = trTypeNameWord->param.attribute.ExtraAttribute;
  // 結合した後の WORD_PARAM。「最初の音」の調整を行う、また、スペイン語で無冠詞にしたいなら非可算をセットする
  WORD_PARAM dstParam = trTypeNameWord->param;

  m_tmpBuf->Clear();
  switch ( System::GetLang() )
  {
  case POKEMON_LANG_FRANCE:
    {
      // フランス語は、[Name:TrainerName ] [前置詞/冠詞] [Name:TrainerType ]
      // pattern に対応する、トレーナータイプに付ける前置詞/冠詞。-1なら付けない。
      static s8 patternToFrenchPrepositionID[4] = {
        // [Name:TrainerName ] le [Name:TrainerType ]
        WORDFORM_WORD_FRENCH_DEFART,
        // [Name:TrainerName ] de la [Name:TrainerType ]
        WORDFORM_WORD_FRENCH_DE_DEFART,
        // [Name:TrainerName ] de [Name:TrainerType ]
        WORDFORM_WORD_FRENCH_DE,
        // 未使用
        -1
      };
      s8 frenchPrepositionID = patternToFrenchPrepositionID[pattern];

      m_tmpBuf->AddStr( *trainerName );
      m_tmpBuf->AddCode( L' ' );
      if (frenchPrepositionID != -1)
      {
        addStr(m_tmpBuf, provider->GetWordForm( frenchPrepositionID, getWordFormIndex(trTypeNameBufID)) );
      }
      m_tmpBuf->AddStr( *trTypeName );

      // トレーナー名が最初に来る
      dstParam = trainerNameWord->param;
    }
    break;

  case POKEMON_LANG_SPAIN:
    switch ( pattern )
    {
    case 0:
    default:
      // "[Name:TrainerType ] [Name:TrainerName ]"
      m_tmpBuf->AddStr( *trTypeName );
      m_tmpBuf->AddCode( L' ');
      m_tmpBuf->AddStr( *trainerName );
      break;
    case 1:
    case 2:
      m_tmpBuf->AddStr( *trainerName );
      if ( pattern == 1 )
      {
        // "[Name:TrainerName ] del [Name:TrainerType ]"
        m_tmpBuf->AddCode( L' ');
        addStr( m_tmpBuf,  provider->GetWordForm(WORDFORM_WORD_SPANISH_DE_DEFART, getWordFormIndex(trTypeNameBufID)) );
      }
      else  // pattern == 2
      {
        // "[Name:TrainerName ] de [Name:TrainerType ]"
        addStr( m_tmpBuf,  L" de " );
      }
      m_tmpBuf->AddStr( *trTypeName );

      // トレーナー名が最初に来る
      dstParam = trainerNameWord->param;
      // 非可算にすることにより無冠詞にする
      // @note トレーナータイプ+トレーナー名タグ用の前置詞タグを用意し、非可算の部分は無冠詞にしておく
      dstParam.attribute.IsAlwaysPlural = 0;
      dstParam.attribute.IsUncountable = 1;
      break;
    }
    break;

  default:  // その他の言語
    GFL_ASSERT_MSG(false, "[Name:TrainerTypeAndName ] can only be used in French or Spanish.");
    // 万一使用された場合に備えて、"[Name:TrainerType ] [Name:TrainerName ]" をセットしておく
    m_tmpBuf->AddStr ( *trTypeName );
    m_tmpBuf->AddCode( L' ');
    m_tmpBuf->AddStr ( *trainerName );
    break;
  }
  this->registerWord(dstBufID, *m_tmpBuf, dstParam);
}

//------------------------------------------------------------------
/**
 * プレイヤー名を登録
 *
 * @param   bufID   バッファID
 * @param   status
 *
 */
//------------------------------------------------------------------
void WordSet::RegisterPlayerName( u32 bufID, const Savedata::MyStatus* status )
{
  status->GetNameString( m_tmpBuf );
  Grammer grm = GrammerFromSex(status->GetSex());
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}

//------------------------------------------------------------------
/**
 * プレイヤーニックネームを登録
 *
 * @param   bufID   バッファID
 * @param   status  プレイヤーステータス
 * @param   menu    フィールドメニューセーブデータ
 */
//------------------------------------------------------------------
void WordSet::RegisterPlayerNickName( u32 bufID, const Savedata::MyStatus* status, const Savedata::FieldMenu* menu )
{
  menu->GetNickNameString( m_tmpBuf );
  Grammer grm = GrammerFromSex(status->GetSex());
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}

//------------------------------------------------------------------
/**
 * プレイヤーニックネーム接頭子を登録（プレイヤーネームの先頭２文字）
 *
 * @param   bufID   バッファID
 * @param   status  プレイヤーステータス
 */
//------------------------------------------------------------------
void WordSet::RegisterPlayerNickNamePrefix( u32 bufID, const Savedata::MyStatus* status )
{
  status->GetNameString( m_tmpBuf );
  const gfl2::str::STRCODE* code = m_tmpBuf->GetPtr();
  gfl2::str::STRCODE code2[3];

  const uint lang = System::GetLang();

  if ( lang == POKEMON_LANG_JAPAN )
  {
    code2[0] = code[0];
    code2[1] = code[1];
    code2[2] = gfl2::str::EOM_CODE;
  }
  else
  {
    // 海外は前頭１文字
    code2[0] = code[0];
    code2[1] = gfl2::str::EOM_CODE;
  }

  m_tmpBuf->SetStr( code2 );

  Grammer grm = GrammerFromSex(status->GetSex());
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}

//------------------------------------------------------------------
/**
 * ライバル名を登録
 *
 * @param   bufID   バッファID
 * @param   misc  ライバル名が設定されている misc
 *
 */
//------------------------------------------------------------------
#if 0 // @note niji未実装
void WordSet::RegisterRivalName( u32 bufID, const Savedata::Misc* misc )
{
  // ライバルの性別を取得
#if (PM_VERSION == VERSION_X) || (PM_VERSION == VERSION_Y) || (PM_VERSION == VERSION_CTR_SAPPHIRE) || (PM_VERSION == VERSION_CTR_RUBY)
  uint hero_sex = GFL_SINGLETON_INSTANCE(gamesystem::GameManager)->GetGameData()->GetPlayerStatus()->GetSex();
  Grammer grm = (hero_sex == PM_MALE) ? GRAMMER_FEMALE : GRAMMER_MALE;  // 主人公と逆の性別
#else
  // 未来のバージョンでは、ライバルの性別を調節する必要がある。
#error Please adjust the rivals sex for this version.
#endif

  misc->GetRivalName( m_tmpBuf );
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}
#endif

//------------------------------------------------------------------
/**
 * 指定バッファにポケモン（ワザ）タイプ名を登録
 *
 * @param   bufID     バッファID
 * @param   typeID      タイプID
 *
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeTypeName( u32 bufID, u32 typeID )
{
  this->registerWordFromMsgArc( bufID, GARC_message_typename_DAT, typeID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのとくせい名を登録
 *
 * @param   bufID       バッファID
 * @param   tokuseiID   とくせいID
 */
//------------------------------------------------------------------
void WordSet::RegisterTokuseiName( u32 bufID, u32 tokuseiID )
{
  GFL_ASSERT(bufID < m_wordMax);           //@check
  GFL_ASSERT(tokuseiID < msg_tokusei_max); //@check

  this->registerWordFromMsgArc( bufID, GARC_message_tokusei_DAT, tokuseiID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケモンのせいかく名を登録
 *
 * @param   bufID       バッファID
 * @param   seikakuID   せいかくID
 */
//------------------------------------------------------------------
void WordSet::RegisterSeikakuName( u32 bufID, u32 seikakuID )
{
  GFL_ASSERT(bufID < m_wordMax);            //@check
  GFL_ASSERT(seikakuID < msg_seikaku_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_seikaku_DAT, seikakuID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにワザ名を登録
 *
 * @param   bufID     バッファID
 * @param   wazaID      ワザID
 *
 */
//------------------------------------------------------------------
void WordSet::RegisterWazaName( u32 bufID, u32 wazaID )
{
  GFL_ASSERT(bufID < m_wordMax);          //@check
  GFL_ASSERT(wazaID < msg_wazaname_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_wazaname_DAT, wazaID );
}

//------------------------------------------------------------------
/**
 * 指定バッファにアイテム名を登録
 *
 * @param   bufID     バッファID
 * @param   itemID    アイテムID
 * @param   itemCount アイテムの個数（単数形と複数形を切り替えるため）
 */
//------------------------------------------------------------------
void WordSet::RegisterItemName( u32 bufID, u32 itemID, u32 itemCount )
{
  GFL_ASSERT(bufID < m_wordMax);                  //@check
  GFL_ASSERT(itemID < msg_itemname_wordset_max);  //@check

  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
  Form form = provider->IsPluralNumber(itemCount) ? FORM_PLURAL : FORM_SINGLE;
  this->registerWordFromMsgArc( bufID, GARC_message_itemname_wordset_DAT, itemID, form);
}

//------------------------------------------------------------------
/**
 * 指定バッファにポケット名を登録
 *
 * @param   bufID     バッファID
 * @param   pocketNo    ポケットNO
 */
//------------------------------------------------------------------
void WordSet::RegisterFieldPocketName( u32 bufID, u16 pocketNo )
{
  GFL_ASSERT(bufID < m_wordMax);              //@check

  // ポケット番号がコンバータから自動生成されるため、テーブル参照にする
  static const u32 tbl[][2] =
  { // ポケット番号, メッセージID
    { BAG_POCKET_DRUG,    str_pocketname_001 },   // かいふく
    { BAG_POCKET_NORMAL,  str_pocketname_002 },   // どうぐ１
    { BAG_POCKET_BEADS,   str_pocketname_003 },   // ビーズ
    { BAG_POCKET_WAZA,    str_pocketname_004 },   // わざマシン
    { BAG_POCKET_NUTS,    str_pocketname_005 },   // きのみ
    { BAG_POCKET_EVENT,   str_pocketname_006 },   // たいせつなもの
    { BAG_POCKET_FREE,    str_pocketname_007 },   // フリースペース
    { BAG_POCKET_ROTOPON, str_pocketname_008 },   // ロトムパワー
  };
  for( u32 i=0; i<msg_bag_pocket_max; i++ )
  {
    if( tbl[i][0] == pocketNo )
    {
      this->registerWordFromMsgArc( bufID, GARC_message_bag_pocket_DAT, tbl[i][1] );
      return;
    }
  }
  GFL_ASSERT( 0 );
}

/* ------------------------------------------------------------------------- */
/**
 * 指定バッファに地名を登録
 *
 * @param bufID バッファID
 * @param placeNameID 地名ID（ゾーンからとれます）
 */
/* ------------------------------------------------------------------------- */
void WordSet::RegisterPlaceName( u32 bufID, u32 placeNameID )
{
  GFL_ASSERT(bufID < m_wordMax);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_place_name_DAT, placeNameID );
}

/* ------------------------------------------------------------------------- */
/**
 * @brief 指定バッファに天気名を登録
 *
 * @param bufID バッファID
 * @param weather_type 天候タイプ
 */
/* ------------------------------------------------------------------------- */
#if 0 // @note niji未実装
void WordSet::RegisterWeatherName( u32 bufID, xy_system::weather::Type weather_type )
{
  GFL_ASSERT( weather_type < xy_system::weather::TYPE_MAX );  //@check

  u32 msg_id = weather_name_sunny;

  switch ( weather_type )
  {
  case xy_system::weather::TYPE_SHINE:
    // はれ
    msg_id = weather_name_sunny;
    break;

  case xy_system::weather::TYPE_CLOUD:
    // くもり
    msg_id = weather_name_cloudy;
    break;

  case xy_system::weather::TYPE_RAIN:
    // あめ
    msg_id = weather_name_rain;
    break;


  default : GFL_ASSERT(0);
  }

  this->registerWordFromMsgArc( bufID, GARC_message_weather_name_DAT, msg_id );
}
#endif

//------------------------------------------------------------------
/**
 * 指定バッファにリボン名を登録
 *
 * @param   bufID     バッファID
 * @param   ribbonID  リボンID
 *
 */
//------------------------------------------------------------------
void WordSet::RegisterRibbonName( u32 bufID, u32 ribbonID )
{
  GFL_ASSERT(bufID    < m_wordMax     );  //@check
  GFL_ASSERT(ribbonID < msg_ribbon_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_ribbon_DAT, ribbonID );
}

//------------------------------------------------------------------
/**
 *
 *
 * @param   bufID
 * @param   number
 * @param   keta
 * @param   dispType
 * @param   codeType
 */
//------------------------------------------------------------------
void WordSet::RegisterNumber( u32 bufID, s32 number, u32 keta, print::NumberDispType dispType, print::NumberCodeType codeType )
{
  print::StrNumber( m_tmpBuf, number, keta, dispType, codeType );

  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);

  WORD_PARAM  param = defaultWordParam();
  param.attribute.Grammer = GRAMMER_MALE;
  param.attribute.InitialSound = 0;  // 判定が困難、EFIGSでは不要
  param.is_initialsound_calculated = 1;
  param.form = provider->IsPluralNumber(number) ? FORM_PLURAL : FORM_SINGLE;

  this->registerWord( bufID, *m_tmpBuf, param );
}

//------------------------------------------------------------------
/**
 * 指定バッファにアトラクション名を登録
 *
 * @param   bufID           バッファID
 * @param   attractionMsgId アトラクション名のMsgId
 */
//------------------------------------------------------------------
void WordSet::RegisterAttractionName( u32 bufID, u32 attractionMsgId )
{
  GFL_ASSERT(bufID           < m_wordMax     );  //@check
  GFL_ASSERT(attractionMsgId < msg_jf_attraction_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_jf_attraction_DAT, attractionMsgId );
}


//------------------------------------------------------------------
/**
 * 指定バッファに着せ替えアイテム名を登録
 *
 * @param   bufID     バッファID
 * @param   itemID    着せ替えアイテムID
 * @param   itemCount アイテムの個数（単数形と複数形を切り替えるため）
 */
//------------------------------------------------------------------
void WordSet::RegisterKisekaeItemName( u32 bufID, u32 itemID, u32 itemCount )
{
  GFL_ASSERT(bufID < m_wordMax);                  //@check
//  GFL_ASSERT(itemID < msg_itemname_wordset_max);  //@check

  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
  Form form = provider->IsPluralNumber(itemCount) ? FORM_PLURAL : FORM_SINGLE;
  this->registerWordFromMsgArc( bufID, GARC_message_kisekae_item_name_DAT, itemID, form);
}





//======================================================================================================
//======================================================================================================
void WordSet::SetWordSetLoader( WordSetLoader * pLoader )
{
//  GFL_ASSERT( m_wordSetLoader == NULL );  //@note フィールド下画面で複数のWordSetLoaderを使用しているためコメントアウト 2014/02/25 by nakahiro
  m_wordSetLoader = pLoader;
}


#if 0
//------------------------------------------------------------------
/**
 * @param p_readman
 */
//------------------------------------------------------------------
void WordSet::PreLoad( gfl2::heap::HeapBase* pHeap, gfl2::fs::FileReadManager* p_readman, u32 read_id_bits )
{
  GFL_ASSERT( m_wordSetLoader == NULL );  //@check
  m_wordSetLoader = GFL_NEW(pHeap) WordSetLoader( pHeap, p_readman, read_id_bits );
  m_wordSetLoader->StartLoad();
}

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
bool WordSet::WaitLoad( void )
{
  return m_wordSetLoader->WaitLoad();
}
#endif

GFL_NAMESPACE_END(print)

GFL_WARNING_POP   // 4245設定回復
