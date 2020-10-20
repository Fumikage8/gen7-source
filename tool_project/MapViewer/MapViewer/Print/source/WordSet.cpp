//============================================================================================
/**
 * @file  wordset.cpp
 * @brief �����񒆂̒P�ꖄ�ߍ��ݏ����N���X
 * @author  taya
 * @date  2011.03.03
 */
//============================================================================================
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <util/include/gfl2_std_string.h>

#include "System/include/PokemonVersion.h"
#include "../include/WordSet.h"
#include "../include/hangul.h"
#include "GameSystem/include/GameManager.h"
#include "GameSystem/include/GameData.h"

#include "arc_index/message.gaix"
#include "niji_conv_header/app/grammar/grammar_wordform.h"
#include "niji_conv_header/message/msg_tokusei.h"
#include "niji_conv_header/message/msg_wazaname.h"
#include "niji_conv_header/message/msg_itemname_wordset.h"
#include "niji_conv_header/message/msg_ribbon.h"
#include "niji_conv_header/message/msg_seikaku.h"
#include "niji_conv_header/message/msg_bag_pocket.h"


GFL_NAMESPACE_BEGIN(print)

// GrammerFromSex �� PM_MALE PM_FEMALE PM_NEUTRAL ���� Grammer �ɃL���X�g���Ă悢���Ƃ��m�F����
// STATIC_ASSERT ���w�b�_�ɏ��������Ȃ������ STATIC_ASSERT ���֐��O�ɏ����Ȃ����߁A�_�~�[�̊֐���p�ӂ��Ă���
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
const u16 WordSet::REF_WORD_ID_HERO = 255;   // ��l���̕��@�������Q�Ƃ���
const u16 WordSet::REF_WORD_ID_EXPANDING_WORD = 254; // ���ݓW�J���̒P��̕��@�������Q�Ƃ���


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

// pml::Sex ���� Grammer �ɕϊ��B�|�P�����̐��ʂɂ͂�����g�p����B
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
    // ���ʕs���̏ꍇ�AFIS: �j���AG: �����Ƃ��Ĉ���
    if (System::GetLang() == POKEMON_LANG_GERMANY)
      return GRAMMER_NEUTRAL;
    else
      return GRAMMER_MALE;
  }
}

// �i�A�C�e�������̂��̂́j������ Form �ɕϊ��B
WordSet::Form WordSet::FormFromCount(u32 count)
{
  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
  return provider->IsPluralNumber(count) ? FORM_PLURAL : FORM_SINGLE;
}

// �����l0��WORD_PARAM�𐶐�
inline WordSet::WORD_PARAM WordSet::defaultWordParam()
{
  WORD_PARAM param;
  param.raw = 0;
  return param;
}

// GrammaticalAttribute ������ꍇ�iMsgData���當������擾�����ꍇ�j�� WORD_PARAM ����
inline WordSet::WORD_PARAM WordSet::createWordParam(print::GrammaticalAttribute attribute, WordSet::Form form)
{
  WORD_PARAM param = defaultWordParam();
  param.attribute = attribute;
  param.is_initialsound_calculated = 1; // GrammaticalAttribute ������˃��[�U�[���͖��ł͂Ȃ��̂ŁA���@�^�O�ŎQ�Ƃ������ꍇ�͖|��҂����͂��Ă���
  param.form = form;
  return param;
}

// GrammaticalAttribute ���Ȃ��ꍇ�i���[�U�[���͖��̏ꍇ�j�� WORD_PARAM ����
inline WordSet::WORD_PARAM WordSet::createWordParam(WordSet::Grammer grm, WordSet::Form form)
{
  WORD_PARAM param = defaultWordParam();
  param.attribute.Grammer = grm;
  param.form = form;
  return param;
}

//======================================================================================================
// ������W�J
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
 * 1�񂾂��P��im_word�j���ċA�I�ȓW�J���s���W�J
 *
 * @param[out]   dstbuf   ������̕�������i�[����o�b�t�@
 * @param[in]    srcbuf   �������ƂȂ�i�^�O�R�[�h�����ߍ��܂ꂽ�j������
 * @param[in]    recursed �ċA�������ǂ����B
 */
//------------------------------------------------------------------
void WordSet::expandCore( gfl2::str::StrBuf* dstbuf, const gfl2::str::StrBuf* srcbuf, bool recursed)
{
  const gfl2::str::STRCODE* src = srcbuf->GetPtr();

  while( *src != gfl2::str::EOM_CODE )
  {
    if( *src == gfl2::str::TAG_START_CODE )
    {
      if( print::IsWordSetTagGroup(src) )
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
 * �P�ꖄ�ߍ��ݑΏۃ^�O�̏���
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

  // �̈�v�Z�J�n�̃^�O���p�����[�^�𖄂ߍ���
  if( m_word[ word_id ].areaWidthChars )
  {
    dstbuf->AddCode( gfl2::str::TAG_START_CODE );
    dstbuf->AddCode( 2 );
    dstbuf->AddCode( print::CreateTagCode(print::TAGGROUP_GENERAL_CTRL, print::TAGIDX_GENERAL_FIX_WIDTH_START) );
    dstbuf->AddCode( m_word[ word_id ].areaWidthChars );
  }

  if( print::IsWordSetNumberTagGroup(src) )
  {
    // �����^�O������؂�����Ȃ���W�J����
    // 2�Ԗڂ̃^�O�p�����[�^������΁A���ꂪ����؂�̕���
    gfl2::str::STRCODE separator;
    if ( gfl2::str::GetTagParamCount(src) > 1)
    {
      separator = gfl2::str::GetTagParam( src, 1 );
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
      // �ċA�͈�x����
      dstbuf->AddStr( *(m_word[ word_id ].str) );
    }
    else
    {
      // m_word �ɓo�^���ꂽ�P�ꒆ�̃^�O��Expand����i������I���^�O��z��j
      m_expandingWordId = word_id;
      expandCore( dstbuf, m_word[ word_id ].str, true );
    }
  }

  // �Ō�ɓW�J�����P��̍Ō�̕�����ۑ��i�؍��̏����ω��p
  // ���؍��łŎ��ۂɋN���邱�Ƃ͂Ȃ����Am_word���^�O�ŏI�����Ă���Ƃ��܂������Ȃ��B
  m_lastWordLastChar = getLastChar( m_word[ word_id ].str );

  // �̈�v�Z�I���̃^�O���p�����[�^�𖄂ߍ���
  if( m_word[ word_id ].areaWidthChars )
  {
    dstbuf->AddCode( gfl2::str::TAG_START_CODE );
    dstbuf->AddCode( 1 );
    dstbuf->AddCode( print::CreateTagCode(print::TAGGROUP_GENERAL_CTRL, print::TAGIDX_GENERAL_FIX_WIDTH_END) );
  }

  // �P���W�J������A�u��ɕ����`�v�ȂǕ��@��������������t���O�𗎂Ƃ�
  m_forceWordParam.raw = 0;
  m_forceWordParamMask.raw = 0;

  return gfl2::str::SkipTagCode( src );
}


//------------------------------------------------------------------
/**
 * ����؂�����Ȃ��琔�l��W�J����
 *
 * @param[out]   dstbuf    ���l��ǉ�����o�b�t�@
 * @param[in]    word_id   ���l�̓����Ă���P��ԍ�
 * @param[in]    separator ����؂�̕����B0�ł���Ό���؂肵�Ȃ��B
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
  // ����؂���s���ŏ��̌����B�C�^���A��ƃX�y�C����́A5������i4���̂Ƃ��Ɍ���؂�����Ȃ��j�A���̑���4������B
  const int minSeparatedKetaCount = (lang == POKEMON_LANG_ITALY || lang == POKEMON_LANG_SPAIN) ? 5 : 4;

  // �ŏ��̐����̈ʒu�ƁA�����̌��������߂�i�ŏ��̐����̌�͂��ׂĐ����Ɖ��肷��j
  // ���ŏ��̐�������A�����鐔���������Ώۂɂ���΁A�����ɂ��Ή��ł���i�����_�Ŏ~�܂�j
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

  // �ŏ��̐����̑O�܂ł�dstbuf�ɒǉ�
  const gfl2::str::STRCODE* src = srcbuf->GetPtr();
  while( src < pFirstDigit )
  {
    // �ʏ킠�肦�Ȃ����A�^�O���Ɍ���؂�����Ă��܂����Ƃ����
    if (*src == gfl2::str::TAG_START_CODE)
    {
      GFL_ASSERT_MSG(false, "TAG_START_CODE contained in a digit word");
      return;
    }
    dstbuf->AddCode( *src );
    src ++;
  }

  // ����؂�����Ȃ��琔����dstbuf�ɒǉ�
  for( int currentKeta = ketaCount - 1; currentKeta >= 0; currentKeta-- ) // currentKeta : 0�I���W���̌��݂̌�
  {
    // �ʏ킠�肦�Ȃ����A�^�O���Ɍ���؂�����Ă��܂����Ƃ����
    if (*src == gfl2::str::TAG_START_CODE)
    {
      GFL_ASSERT_MSG(false, "TAG_START_CODE contained in a digit word");
      return;
    }

    dstbuf->AddCode( *src );
    src ++;
    // 4���ڂ̌�icurrentKeta=3�j�A7���ڂ̌�icurrentKeta=6�j�A�c�c�Ɍ���؂������
    if( (currentKeta % 3 == 0) && (currentKeta != 0) )
    {
      dstbuf->AddCode( separator );
    }
  }
}

//------------------------------------------------------------------
/**
 * �؍���̏����^�O�̏���
 */
//------------------------------------------------------------------
const gfl2::str::STRCODE* WordSet::processKoreanParticleTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src)
{
  const uint particle_id = gfl2::str::GetTagParam( src, 0 );  // �^�O���̏���ID
  const gfl2::str::STRCODE particle = hangul::GetParticle( m_lastWordLastChar, particle_id );   // �ǉ����ׂ�����

  if (particle != 0)
  {
    dstbuf->AddCode( particle );
  }

  return gfl2::str::SkipTagCode( src );
}

//------------------------------------------------------------------
/**
 * ������I���^�O�̏���
 */
//------------------------------------------------------------------
const gfl2::str::STRCODE* WordSet::processStringSelectTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src)
{
  const uint MAX_SELSTR_COUNT = 4;  // �I�����镶����̍ő吔

  const uint param_count = gfl2::str::GetTagParamCount(src);
  const uint str_count = (param_count - 1) * 2;

  if (param_count > 1 + MAX_SELSTR_COUNT/2)
  {
    GFL_ASSERT_MSG(false, "TagParamCount is too large");
    return gfl2::str::SkipTagCode( src );
  }

  // �I�����镶���������
  const uint ref_word_id = gfl2::str::GetTagParam(src, 0);
  int select_index = getStringSelectIndex(print::GetTagIndex(src), ref_word_id);
  if (select_index >= str_count)
  {
    GFL_ASSERT_MSG(false, "select_index is too large");
    select_index = 0;
  }

  // �^�O�p�����[�^����e������̒������擾
  u8 str_length[MAX_SELSTR_COUNT];
  gfl2::std::MemClear( str_length, sizeof(str_length) );

  for (int i = 0; i < param_count-1; i ++)
  {
    uint param = gfl2::str::GetTagParam(src, i+1);
    str_length[i*2    ] = param & 0xff;
    str_length[i*2 + 1] = (param >> 8);
  }

  // �I�����ꂽ�������dstbuf�ɒǉ�
  src = gfl2::str::SkipTagCode(src);
  for (int i = 0; i < str_count; i ++)
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
 *  tag_index �Ŏw�肵��������I���^�O�ɑ΂��āA���݂̌���(xy::GetLang) �ɂ�����
 *  ref_word_id �Ŏw�肵���P��̐��A���ɉ����đI�����ׂ�������̃C���f�b�N�X�𓾂�
 */
//------------------------------------------------------------------
uint WordSet::getStringSelectIndex(uint tag_index, uint ref_word_id) const
{
  const WORD_PARAM param = getRefWordParam(ref_word_id);
  const uint grammer = param.attribute.Grammer;
  // ������I���^�O�ɂ����ẮA��Z�͒P���Ƃ��Ĉ���
  const uint form = (param.form == FORM_UNCOUNTABLE) ? FORM_SINGLE : param.form;

  switch (tag_index)
  {
  case print::TAGIDX_STRING_SELECT_BY_GENDER:
    return grammer;
  case print::TAGIDX_STRING_SELECT_BY_QUANTITY:
    return form;
  case print::TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY:
    // �h�C�c��ȊO�́A�������Ȃ��B
    // �P���j���A�P�������A�����j���A����������4��ނ̕ω��ƂȂ�B
    return form*2 + grammer;
  case print::TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY_GERMAN:
    // �h�C�c��́A����������A�܂��A�����̏ꍇ�͐��ɂ��ω����Ȃ��B
    // �P���j���A�P�������A�P�������A������4��ނ̕ω��ƂȂ�B
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
 * ��`�ω��^�O�̏���
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
 *  ���݂̌���(xy::GetLang)�ɂ��āA
 *  ref_word_id �Ŏw�肵���P��̐��A���A�ŏ��̉��ɉ����đ�����ׂ���`�̃C���f�b�N�X�𓾂�
 */
//------------------------------------------------------------------
uint WordSet::getWordFormIndex(uint ref_word_id) const
{
  const WORD_PARAM param = getRefWordParam(ref_word_id);
  const uint form = param.form;
  const uint grammer = param.attribute.Grammer;
  const uint initialsound = param.attribute.InitialSound;

  // ��`�̕��я��́A��`�ω��\�iwordforms.csv�j���Q��
  switch (System::GetLang())
  {
  case POKEMON_LANG_ENGLISH: // ���̎��*3�A���̎��*1�A�ŏ��̉��̎��*2
    return form*1*2 + initialsound;
  case POKEMON_LANG_FRANCE:  // ���̎��*3�A���̎��*2�A�ŏ��̉��̎��*2
    return form*2*2 + grammer*2 + initialsound;
  case POKEMON_LANG_SPAIN:   // ���̎��*3�A���̎��*2�A�ŏ��̉��̎��*3
  case POKEMON_LANG_ITALY:   // ���̎��*3�A���̎��*2�A�ŏ��̉��̎��*3
    return form*2*3 + grammer*3 + initialsound;
  case POKEMON_LANG_GERMANY:  // ���̎��*3�A���̎��*3�A�ŏ��̉��̎��*1
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
 * ���@���������^�O�̏���
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
 *  ref_word_id �Ŏw�肵���P��� WORD_PARAM ���A����ȎQ�Ɣԍ��i��l���A�W�J���̒P��j
 *  ����ѕ��@�����̋������l�����ē���
 */
//------------------------------------------------------------------
WordSet::WORD_PARAM WordSet::getRefWordParam(uint ref_word_id) const
{
  WORD_PARAM param = defaultWordParam();

  if (ref_word_id == REF_WORD_ID_HERO)
  {
    // REF_WORD_ID_HERO ���w�肳�ꂽ��A���͎�l���̐��ʁA���͒P���ɂ���
    u8 hero_sex = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus()->GetSex();
    param.attribute.Grammer = GrammerFromSex(hero_sex);
    // REF_WORD_ID_HERO ���g�p���Ă���Ȃ當���Ɏ�l�����͕\������Ȃ��̂ŁA�ŏ��̉��̓f�t�H���g��OK
    param.is_initialsound_calculated = true;
  }
  else
  {
    if (ref_word_id == REF_WORD_ID_EXPANDING_WORD)
    {
      // REF_WORD_ID_EXPANDING_WORD �Ȃ�A�W�J���i�ċA���j�̒P���WORD_PARAM���Q��
      ref_word_id = m_expandingWordId;
    }

    if (ref_word_id >= m_wordMax)
    {
      GFL_ASSERT_MSG(false, "ref_word_id %u is too large", ref_word_id);
      return defaultWordParam();
    }
    param = m_word[ref_word_id].param;
  }

  // ���@�����̋���
  param.raw = (param.raw & ~m_forceWordParamMask.raw) | (m_forceWordParam.raw & m_forceWordParamMask.raw);

  // �Z�E��ɕ������l�����āA����␳
  if (param.attribute.IsUncountable)
  {
    param.form = FORM_UNCOUNTABLE;
  }
  else if (param.attribute.IsAlwaysPlural)
  {
    param.form = FORM_PLURAL;
  }

  // �ŏ��̉������肳��Ă��Ȃ��Ȃ画�肷��
  if (!param.is_initialsound_calculated)
  {
    print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);
    param.attribute.InitialSound = provider->GetInitialSoundType(m_word[ref_word_id].str->GetPtr());
  }

  return param;
}

//------------------------------------------------------------------
/**
 * strbuf����A�^�O���̂������Ō�̕������擾����
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
 * dstbuf �ɁAsrcptr �̎w��EOM�I�[�̕������ǉ�����
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
// �S������N���A
//======================================================================================================
void WordSet::ClearAllBuffer( void )
{
  for(u32 i=0; i<m_wordMax; ++i)
  {
    clearWord( i );
  }
}

//======================================================================================================
// �P��Z�b�g���W���[���Ɋe��P���o�^����
//======================================================================================================

//------------------------------------------------------------------
/**
 * �P�ꕶ���񁕃p�����[�^���w��o�b�t�@�ɓo�^
 * ���ڎg�p����ꍇ�́A���[�J���C�Y�Ή����l�����邱�ƁB
 *
 * @param   wordset   �P��Z�b�g���W���[��
 * @param   bufID   �o�b�t�@�h�c
 * @param   str     ������
 * @param   param   ������ɕt������p�����[�^
 * @param   areaWidthChars   �����̕`��̈�����肷��^�O�𖄂ߍ��ގ��̌v�Z�p�B0�Ȃ�Ȃɂ�����
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
    //������L���ɂ���΃^�O���Ƃ̏k���������悤�ɂȂ�B
    //�����Akujira�ł�MsgWin�S�̂����Ă̏k���������Ă���̂ŕs�v�B
    //m_word[bufID].areaWidthChars = (m_bAreaWidthTagEnable)? areaWidthChars : 0;
    m_word[bufID].areaWidthChars = 0;
  }
}

//------------------------------------------------------------------
/**
 * ���b�Z�[�W�A�[�J�C�u���̎w��ID��������w��o�b�t�@�Ƀ��[�h
 *
 * @param   dstbuf  �ǂݍ��ݐ�o�b�t�@
 * @param   dat_id  �A�[�J�C�u���f�[�^ID
 * @param   msg_id  ������ID
 *
 * @return  u16     ���b�Z�[�W�̃��[�U�[�ݒ�p�����[�^

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
 * ���b�Z�[�W�A�[�J�C�u���́A�w��ID��������o�b�t�@�W�J
 *
 * @param   bufID   �o�b�t�@�h�c
 * @param   dat_id  �A�[�J�C�u���f�[�^ID
 * @param   msg_id  ������ID
 * @param   form    �P��
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
 * MsgData���當������擾���ēo�^�B������g���ƁAMsgData �� userParam ���當�@�p�����[�^���Z�b�g���܂��B
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
 * �C�ӕ�������p�����[�^�t���œo�^�BMsgData����擾�����ꍇ��������g���B
 */
//------------------------------------------------------------------
void WordSet::RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, print::GrammaticalAttribute grammaticalAttribute, Form form)
{
  this->registerWord( bufID, word, createWordParam(grammaticalAttribute, form) );
}

//------------------------------------------------------------------
/**
 * �C�ӕ�������p�����[�^�t���œo�^�B���[�J���C�Y�Ή����l�����邱�ƁB
 */
//------------------------------------------------------------------
void WordSet::RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, Grammer grm, Form form )
{
  this->registerWord( bufID, word, createWordParam(grm, form) );
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̃j�b�N�l�[����o�^ (CoreParam)
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
 * �w��o�b�t�@�Ƀ|�P�����̃j�b�N�l�[����o�^ (������A����)
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeNickName( u32 bufID, const gfl2::str::StrBuf& str, pml::Sex sex )
{
  Grammer grm = GrammerFromPmlSex(sex);
  this->registerWord( bufID, str, createWordParam(grm), (u16)(6.25f*256) );
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����푰����o�^ (CoreParam)
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
 * �w��o�b�t�@�Ƀ|�P�����푰����o�^ (MonsNo)
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeMonsNameNo( u32 bufID, MonsNo monsno )
{
  print::GrammaticalAttribute grammaticalAttribute = print::GrammaticalAttribute::FromMsgDataUserParam( pml::personal::GetMonsName( m_tmpBuf, monsno ) );
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grammaticalAttribute, FORM_SINGLE) );
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̐e����o�^
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
 * �w��o�b�t�@�Ƀ|�P�����̉ߋ��e����o�^
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
 * �w��o�b�t�@�Ƀg���C����o�^
 */
//------------------------------------------------------------------
#if 0 // @todo savedata�҂�
void WordSet::RegisterBoxTrayName( u32 bufID, const savedata::BOX* p_box, const u32 tray_no )
{
  const gfl2::str::STRCODE* p_code = p_box->GetTrayName( tray_no );
  m_tmpBuf->SetStr( p_code );

  this->registerWord( bufID, *m_tmpBuf, defaultWordParam() );
}
#endif

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀg���[�i�[��ʂ�o�^
 *
 * @param bufID   ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param trID    �g���[�i�[�^�C�v ( TRTYPE_xxxx )
 *
 */
//------------------------------------------------------------------
void WordSet::RegisterTrTypeName( u32 bufID, u16 trType )
{
#if 0 // @todo Arc�����҂�
  GFL_ASSERT(bufID < m_wordMax);        //@check
  GFL_ASSERT(trType < msg_trtype_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_trtype_DAT, trType );
#endif
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀg���[�i�[����o�^
 *
 * @param bufID   ���Ԃ̃o�b�t�@�ɓo�^���邩
 * @param trID    �g���[�i�[ID
 */
//------------------------------------------------------------------
void WordSet::RegisterTrainerName( u32 bufID, u32 trID )
{
#if 0 // @todo Arc�����҂�
  GFL_ASSERT(bufID < m_wordMax);      //@check
  GFL_ASSERT(trID < msg_trname_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_trname_DAT, trID );
#endif
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀg���[�i�[�^�C�v���ƃg���[�i�[���������������̂�o�^�i�t�����X��A�X�y�C����̃o�g�����g���[�i�[�^�C�v�\���ɑΉ��j
 * �����p�^�[���ԍ��́A�g���[�i�[�^�C�v���� GrammaticalAttribute::ExtraAttribute �ɓ����Ă���B
 *
 * @param   dstBufID           �ݒ��o�b�t�@ID�BtrTypeNameBufID�AtrainerNameBufID�Ɠ����ł��悢�B
 * @param   trTypeNameBufID    �g���[�i�[�^�C�v�����Z�b�g�����o�b�t�@ID
 * @param   trainerNameBufID   �g���[�i�[�����Z�b�g�����o�b�t�@ID
 *
 * @note �t�����X��A�X�y�C����ɂ����錋���p�^�[�����n�[�h�R�[�h���Ă���B
 *       ��������������𐶐����邽�߂�WordSet�𐶐����郁���������A�����R�X�g���l�����āA�n�[�h�R�[�h��I������
 */
//------------------------------------------------------------------
void WordSet::RegisterTrTypeNameAndTrainerName(u32 dstBufID, u32 trTypeNameBufID, u32 trainerNameBufID)
{
  // �͈̓`�F�b�N
  if ( dstBufID >= m_wordMax || trTypeNameBufID >= m_wordMax || trainerNameBufID >= m_wordMax)
  {
    GFL_ASSERT_MSG( false, "dstBufID=%d, trTypeNameBufID=%d, trainerNameBufID = %d, wordmax=%d", dstBufID, trTypeNameBufID, trainerNameBufID, m_wordMax );
    // �󕶎�����Z�b�g���Ă���
    m_tmpBuf->Clear();
    this->registerWord( dstBufID, *m_tmpBuf, defaultWordParam() );
    return;
  }

  const WORD* trTypeNameWord = &m_word[trTypeNameBufID];
  const WORD* trainerNameWord = &m_word[trainerNameBufID];
  const gfl2::str::StrBuf* trTypeName = trTypeNameWord->str;
  const gfl2::str::StrBuf* trainerName = m_word[trainerNameBufID].str;

  print::GrammaticalAttributeProvider *provider = GFL_SINGLETON_INSTANCE(print::GrammaticalAttributeProvider);

  // �����p�^�[���ԍ�
  const uint pattern = trTypeNameWord->param.attribute.ExtraAttribute;
  // ����������� WORD_PARAM�B�u�ŏ��̉��v�̒������s���A�܂��A�X�y�C����Ŗ������ɂ������Ȃ��Z���Z�b�g����
  WORD_PARAM dstParam = trTypeNameWord->param;

  m_tmpBuf->Clear();
  switch ( System::GetLang() )
  {
  case POKEMON_LANG_FRANCE:
    {
      // �t�����X��́A[Name:TrainerName ] [�O�u��/����] [Name:TrainerType ]
      // pattern �ɑΉ�����A�g���[�i�[�^�C�v�ɕt����O�u��/�����B-1�Ȃ�t���Ȃ��B
      static s8 patternToFrenchPrepositionID[4] = {
        // [Name:TrainerName ] le [Name:TrainerType ]
        WORDFORM_WORD_FRENCH_DEFART,
        // [Name:TrainerName ] de la [Name:TrainerType ]
        WORDFORM_WORD_FRENCH_DE_DEFART,
        // [Name:TrainerName ] de [Name:TrainerType ]
        WORDFORM_WORD_FRENCH_DE,
        // ���g�p
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

      // �g���[�i�[�����ŏ��ɗ���
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

      // �g���[�i�[�����ŏ��ɗ���
      dstParam = trainerNameWord->param;
      // ��Z�ɂ��邱�Ƃɂ�薳�����ɂ���
      // @note �g���[�i�[�^�C�v+�g���[�i�[���^�O�p�̑O�u���^�O��p�ӂ��A��Z�̕����͖������ɂ��Ă���
      dstParam.attribute.IsAlwaysPlural = 0;
      dstParam.attribute.IsUncountable = 1;
      break;
    }
    break;

  default:  // ���̑��̌���
    GFL_ASSERT_MSG(false, "[Name:TrainerTypeAndName ] can only be used in French or Spanish.");
    // ����g�p���ꂽ�ꍇ�ɔ����āA"[Name:TrainerType ] [Name:TrainerName ]" ���Z�b�g���Ă���
    m_tmpBuf->AddStr ( *trTypeName );
    m_tmpBuf->AddCode( L' ');
    m_tmpBuf->AddStr ( *trainerName );
    break;
  }
  this->registerWord(dstBufID, *m_tmpBuf, dstParam);
}

//------------------------------------------------------------------
/**
 * �v���C���[����o�^
 *
 * @param   bufID   �o�b�t�@ID
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
 * �v���C���[�j�b�N�l�[���ړ��q��o�^�i�v���C���[�l�[���̐擪�Q�����j
 *
 * @param   bufID   �o�b�t�@ID
 * @param   status  �v���C���[�X�e�[�^�X
 */
//------------------------------------------------------------------
#if 0   // @todo  �g�p���邱�ƂɂȂ�����L���ɂ��Ă������� 2015/03/11
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
    // �C�O�͑O���P����
    code2[0] = code[0];
    code2[1] = gfl2::str::EOM_CODE;
  }

  m_tmpBuf->SetStr( code2 );

  Grammer grm = GrammerFromSex(status->GetSex());
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}
#endif

//------------------------------------------------------------------
/**
 * ���C�o������o�^
 *
 * @param   bufID   �o�b�t�@ID
 * @param   misc  ���C�o�������ݒ肳��Ă��� misc
 *
 */
//------------------------------------------------------------------
#if 0 // @todo savedata�҂�
void WordSet::RegisterRivalName( u32 bufID, const Savedata::Misc* misc )
{
  // ���C�o���̐��ʂ��擾
#if (PM_VERSION == VERSION_X) || (PM_VERSION == VERSION_Y) || (PM_VERSION == VERSION_CTR_SAPPHIRE) || (PM_VERSION == VERSION_CTR_RUBY)
  uint hero_sex = GFL_SINGLETON_INSTANCE(gamesystem::GameManager)->GetGameData()->GetPlayerStatus()->GetSex();
  Grammer grm = (hero_sex == PM_MALE) ? GRAMMER_FEMALE : GRAMMER_MALE;  // ��l���Ƌt�̐���
#else
  // �����̃o�[�W�����ł́A���C�o���̐��ʂ𒲐߂���K�v������B
#error Please adjust the rival's sex for this version.
#endif

  misc->GetRivalName( m_tmpBuf );
  this->registerWord( bufID, *m_tmpBuf, createWordParam(grm) );
}
#endif

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����i���U�j�^�C�v����o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   typeID      �^�C�vID
 *
 */
//------------------------------------------------------------------
void WordSet::RegisterPokeTypeName( u32 bufID, u32 typeID )
{
  this->registerWordFromMsgArc( bufID, GARC_message_typename_DAT, typeID );
}

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ|�P�����̂Ƃ���������o�^
 *
 * @param   bufID       �o�b�t�@ID
 * @param   tokuseiID   �Ƃ�����ID
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
 * �w��o�b�t�@�Ƀ|�P�����̂�����������o�^
 *
 * @param   bufID       �o�b�t�@ID
 * @param   seikakuID   ��������ID
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
 * �w��o�b�t�@�Ƀ��U����o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   wazaID      ���UID
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
 * �w��o�b�t�@�ɃA�C�e������o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   itemID    �A�C�e��ID
 * @param   itemCount �A�C�e���̌��i�P���`�ƕ����`��؂�ւ��邽�߁j
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
 * �w��o�b�t�@�Ƀ|�P�b�g����o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   pocketNo    �|�P�b�gNO
 */
//------------------------------------------------------------------
void WordSet::RegisterFieldPocketName( u32 bufID, u16 pocketNo )
{
  GFL_ASSERT(bufID < m_wordMax);              //@check
  GFL_ASSERT(pocketNo < msg_bag_pocket_max);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_bag_pocket_DAT, pocketNo );
}

/* ------------------------------------------------------------------------- */
/**
 * �w��o�b�t�@�ɒn����o�^
 *
 * @param bufID �o�b�t�@ID
 * @param placeNameID �n��ID�i�]�[������Ƃ�܂��j
 */
/* ------------------------------------------------------------------------- */
void WordSet::RegisterPlaceName( u32 bufID, u32 placeNameID )
{
#if 0 // @todo Arc�����҂�
  GFL_ASSERT(bufID < m_wordMax);  //@check

  this->registerWordFromMsgArc( bufID, GARC_message_place_name_DAT, placeNameID );
#endif
}

/* ------------------------------------------------------------------------- */
/**
 * @brief �w��o�b�t�@�ɓV�C����o�^
 *
 * @param bufID �o�b�t�@ID
 * @param weather_type �V��^�C�v
 */
/* ------------------------------------------------------------------------- */
#if 0 // @todo �V������҂�
void WordSet::RegisterWeatherName( u32 bufID, xy_system::weather::Type weather_type )
{
  GFL_ASSERT( weather_type < xy_system::weather::TYPE_MAX );  //@check

  u32 msg_id = weather_name_sunny;

  switch ( weather_type )
  {
  case xy_system::weather::TYPE_SHINE:
    // �͂�
    msg_id = weather_name_sunny;
    break;

  case xy_system::weather::TYPE_CLOUD:
    // ������
    msg_id = weather_name_cloudy;
    break;

  case xy_system::weather::TYPE_RAIN:
    // ����
    msg_id = weather_name_rain;
    break;


  default : GFL_ASSERT(0);
  }

  this->registerWordFromMsgArc( bufID, GARC_message_weather_name_DAT, msg_id );
}
#endif

//------------------------------------------------------------------
/**
 * �w��o�b�t�@�Ƀ��{������o�^
 *
 * @param   bufID     �o�b�t�@ID
 * @param   ribbonID  ���{��ID
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
  param.attribute.InitialSound = 0;  // ���肪����AEFIGS�ł͕s�v
  param.is_initialsound_calculated = 1;
  param.form = provider->IsPluralNumber(number) ? FORM_PLURAL : FORM_SINGLE;

  this->registerWord( bufID, *m_tmpBuf, param );
}


//======================================================================================================
//======================================================================================================
void WordSet::SetWordSetLoader( WordSetLoader * pLoader )
{
//  GFL_ASSERT( m_wordSetLoader == NULL );  //@note �t�B�[���h����ʂŕ�����WordSetLoader���g�p���Ă��邽�߃R�����g�A�E�g 2014/02/25 by nakahiro
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
