//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *  @file   GrammaticalAttributeProvider.cpp
 *  @brief  �P��̕��@�I�ȑ����i���A�Z�A�ŏ��̉��j���擾����
 *  @author ichibayashi_hironori@Nintendo
 *  @date   2012.09.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

// "GrammaticalAttribute"Provider �Ƃ��������ł͂Ȃ��Ȃ��Ă��܂���

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
  u8  WordCount;                      // ��`�ω�����P��̐�
  u8  WordFormCount;                  // ��`�ω��̕ω��p�^�[����
  u16 WordOffsets[1];                 // ��`�ω�������ւ̃I�t�Z�b�g
  // WordOffsets �ȍ~ (WordOffsets�܂�) ��
  // u16 WordOffsets[WordCount];
  // u8[WordCount*WordFormCount]        // ��`�ω���������ł̊e��`�̃I�t�Z�b�g
  // wordFormString[WordCount] ALIGN(2) // ��`�ω�������
};

//-----------------------------------------------------------------------------
// �������E�I������
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

// @brief ������
// @param pHeap ���@�����f�[�^�ǂݍ��ݐ�̃q�[�v
void GrammaticalAttributeProvider::Initialize(gfl2::heap::HeapBase* heap)
{
  GFL_ASSERT_MSG(mpData == 0, "already initialized");

  // ���݂̌���̕��@�����f�[�^�ԍ����擾
  uint language = System::GetLang();
  uint datId = getDatId(language);

  m_currentLanguage = language;

  OpenArcFile();
  // �ő�dat�̃T�C�Y�Ń��������m��
  mpDataSize = getMaxDataSize();
  mpData = static_cast<u8*>(GflHeapAllocMemory(heap, mpDataSize));
  // ���@�����f�[�^�����[�h
  LoadData( datId );
  CloseArcFile();
}

// @brief �w�肵������̕��@�����f�[�^��ǂݍ���
// @param language �ύX��̌���
void GrammaticalAttributeProvider::ChangeLanguage(uint language)
{
  if (language == m_currentLanguage)
    return;
  m_currentLanguage = language;

  // �w�肵������̕��@�����f�[�^�ԍ����擾
  uint datId = getDatId( language );

  // ���@�����f�[�^�����[�h
  OpenArcFile();
  LoadData( datId );
  CloseArcFile();
}

// @brief �I������
void GrammaticalAttributeProvider::Finalize()
{
  GFL_SAFE_DELETE(mpData);
}

// ARCID_GRAMMAR ���J��
void GrammaticalAttributeProvider::OpenArcFile()
{
  gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_FILEREAD);

  gfl2::fs::AsyncFileManager::ArcFileOpenReq req;
  req.fileName    = NULL;                                         // [in]  �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B  // ****�L�[****
  req.arcId       = ARCID_GRAMMAR;                                // [in]  ARCID�BARCID_NULL�̂Ƃ���fileName���g����B  // ****�L�[****
  req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
  req.heapForFile = fileread_heap;                                // [in]  �q�[�v�B�t�@�C���C���X�^���X�̍쐬�ƍ����e�[�u���̍쐬�Ɏg�p�BClose����܂ō��L�̂��̂��ێ������B
  req.heapForReq  = fileread_heap;                                // [in]  ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�����e�[�u���̍쐬�̂��߂ɂ��g�p����BArcSrc�̃C���X�^���X��\�[�X�e�[�u���̍쐬�̂��߂ɂ��g�p����B

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  man->SyncArcFileOpen( req );
}

// ARCID_GRAMMAR �����
void GrammaticalAttributeProvider::CloseArcFile()
{
  gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_FILEREAD);

  gfl2::fs::AsyncFileManager::ArcFileCloseReq req;
  req.fileName    = NULL;                                         // [in]  �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B  // ****�L�[****
  req.arcId       = ARCID_GRAMMAR;                                // [in]  ARCID�BARCID_NULL�̂Ƃ���fileName���g����B  // ****�L�[****
  req.prio        = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
  req.heapForReq  = NULL;                                         // [in]  ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�����łł�NULL�ł悢�B

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  man->SyncArcFileClose( req );
}

// �w�肵�� dat �����[�h����
void GrammaticalAttributeProvider::LoadData( uint datId )
{
  gfl2::heap::HeapBase* fileread_heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_FILEREAD);

  gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
  req.fileName          = NULL;                                         // [in]  �t�@�C�����B�I�[��'\0'�ɂȂ��Ă��邱�ƁBarcId��ARCID_NULL�̂Ƃ��̂ݎg����B
  req.arcId             = ARCID_GRAMMAR;                                // [in]  ARCID�BARCID_NULL�̂Ƃ���fileName���g����B
  req.datId             = datId;                                        // [in]  �A�[�J�C�u�t�@�C�����f�[�^ID
  req.prio              = gfl2::fs::AsyncFileManager::REQ_PRIO_NORMAL;  // [in]  ���N�G�X�g�̎��s�D��x(�l�������������D��x������)
  req.pBuf              = mpData;                                       // [out] �Ăяo�����ł��炩���߃��������m�ۂ��Ă������o�b�t�@�B�����Ƀt�@�C������ǂݍ��񂾓��e������B  // ****�L�[****
  req.bufSize           = mpDataSize;                                   // [in]  pBuf�̃T�C�Y�B�f�[�^�T�C�Y(�p�f�B���O���܂�)�ȏ�ɂ��ĉ������B
  req.pRealReadSize     = NULL;                                         // [out] ���ۂɓǂݍ��񂾃T�C�Y�Bcompressed��true�̂Ƃ��𓀌�̃T�C�Y�BNULL�ł��悢�B
  req.heapForReq        = fileread_heap;                                // [in]  ���N�G�X�g�I���܂ňꎞ�I�Ɏg�p����q�[�v�B�w�u�����Łv���uArcSrc���g��Ȃ��Ƃ��v�x��NULL�ł悢�BArcSrc���g���Ƃ��̓t�@�C���I�[�v���ɂ��g�p����B
  req.heapForCompressed = NULL;                                         // [in]  �𓀂���ꍇ�Ɉꎞ�I�Ɏg�p����q�[�v�B�𓀂��Ȃ��ꍇ��NULL�ł悢�B

  gfl2::fs::AsyncFileManager * man = gfl2::fs::System::GetAsyncFileManagerForRomDevice();
  man->SyncArcFileLoadDataBuf( req );
}

// grammar.garc ���� dat �̍ő�T�C�Y�𓾂�
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

// �w�肵������̕��@�����f�[�^�ԍ����擾
uint GrammaticalAttributeProvider::getDatId(uint lang)
{
  // PM_LANG ����ǂݍ��ݑΏۂ�dat�ւ̕\
  static const u16 lang_to_grammar_garc_id[] = {
    GARC_grammar_DATA_NUM,    // POKEMON_LANG_NULL
    GARC_grammar_JPN_DAT,
    GARC_grammar_English_DAT,
    GARC_grammar_French_DAT,
    GARC_grammar_Italian_DAT,
    GARC_grammar_German_DAT,
    GARC_grammar_DATA_NUM,    // ����
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
// ���@�����̎擾
//-----------------------------------------------------------------------------

// �w�b�_�ւ̃|�C���^���擾
inline const GrammarDatHeader* GrammaticalAttributeProvider::getHeader() const
{
  return reinterpret_cast<const GrammarDatHeader*>(mpData);
}

// �e�P��̌�`�ω���������ł̊e��`�ւ̃I�t�Z�b�g
inline const u8* GrammaticalAttributeProvider::getWordFormOffsets() const
{
  // WordOffsets �̒���ɑ��݂���
  const GrammarDatHeader* header = getHeader();
  return reinterpret_cast<const u8*>(&header->WordOffsets[header->WordCount]); 
}

// @brief ��`�ω�����P��̌�`�𓾂�
// @param word_id     �P��ID
// @param wordform_id ��`ID
const gfl2::str::STRCODE* GrammaticalAttributeProvider::GetWordForm(uint word_id, uint wordform_id) const
{
  static const gfl2::str::STRCODE* const empty_str = L"";
  const GrammarDatHeader* header = getHeader();

  if (word_id > header->WordCount || wordform_id > header->WordFormCount)
  {
    GFL_ASSERT_MSG(false, "word_id(%u) or wordform_id(%u) is out of range", word_id, wordform_id);
    return empty_str;
  }

  // ��`�ω�������ւ̃I�t�Z�b�g
  uint wordOffset = header->WordOffsets[word_id];
  // wordFormOffsets �ɂ́A�u�e�P��̌�`�ω���������ł̊e��`�ւ̃I�t�Z�b�g�iWordFormCount�j�v���A
  // �P��̎�ސ��iWordCount�j��������ł���
  const u8* wordFormOffsets = getWordFormOffsets();
  uint wordFormOffset = wordFormOffsets[word_id * header->WordFormCount + wordform_id];

  return reinterpret_cast<gfl2::str::STRCODE*>(this->mpData + wordOffset + wordFormOffset);
}

//���̑��̌���̍ŏ��̉��^�C�v����
//    �ŏ��̕������ꉹ���˕ꉹ
//    ����ȊO�ˎq��
//
//�ŏ��̉��^�C�v�̔����
//�E�i�t�����X��j�O�u�� de �̃G���W�I��
//�E�i�C�^���A��j�o�g���̃j�b�N�l�[���i�f�t�H���g���j�̊����ω�
//�ɂ̂ݎg�p�����z��ł����A�Ԉ���Ďg�p���ꂽ�ꍇ�ɔ����āA
//�ق��̌���ł��ꉹ�����ǂ����ʂ͔��肵�Ă����܂��B
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

//�t�����X��̍ŏ��̉��^�C�v����
//    �ŏ��̕������ꉹ���˕ꉹ�i�G���W�I������j
//    �ŏ��̕����� h �ŁA���A���̕��������݂��Ă��ꂪ�ꉹ���˕ꉹ�i�G���W�I������j
//    ����ȊO�ˎq���i�G���W�I�����Ȃ��j
uint GrammaticalAttributeProvider::getInitialSoundTypeFrench(const gfl2::str::STRCODE* sp)
{
  const uint FRENCH_INITIALSOUNDTYPE_CONS = 0;
  const uint FRENCH_INITIALSOUNDTYPE_VOWEL = 1;

  gfl2::str::STRCODE c = sp[0];
  if (c == 'h' || c == 'H')     // �ŏ��̕����� h / H �Ȃ玟�̕���������
  {
    c = sp[1]; // sp[0] �� EOM �ł͂Ȃ��̂ŁAsp[1] ��ǂ�ł悢
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

//�C�^���A��̍ŏ��̉��^�C�v����
//    �ŏ��̕������ꉹ���˕ꉹ
//    �ŏ��̕����� z, x ,y �� s+�q���A�� �� (�q��2)
//      (�ȉ�3��2�����ȏ�̏ꍇ�̂�)
//      �ŏ���2������ gn, pn, ps �� s+�q���A�� �� (�q��2)
//      �ŏ��̕����� h �ŁA���̕��������݂��Ă��ꂪ�ꉹ���˕ꉹ
//      �ŏ��̕����� s �ŁA���̕��������݂��Ă��ꂪ�ꉹ���ł͂Ȃ��� s+�q���A�� (�q��2)
//    ����ȊO�ˎq���i���̑��j
uint GrammaticalAttributeProvider::getInitialSoundTypeItalian(const gfl2::str::STRCODE* sp)
{
  const uint ITALIAN_INITIALSOUNDTYPE_CONS = 0;
  const uint ITALIAN_INITIALSOUNDTYPE_VOWEL = 1;
  const uint ITALIAN_INITIALSOUNDTYPE_CONS2 = 2;

  gfl2::str::STRCODE c1 = sp[0];
  // �󕶎���Ȃ�q���ɂ��Ă���
  if (c1 == gfl2::str::EOM_CODE)
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS;
  }

  // sp[0] �� EOM �ł͂Ȃ��̂ŁAsp[1] ��ǂ�ł悢
  gfl2::str::STRCODE c2 = sp[1];

  if (isVowelAlphabet(c1))
  {
    return ITALIAN_INITIALSOUNDTYPE_VOWEL;
  }
  else if (c1 == 'z' || c1 == 'Z' || c1 == 'x' || c1 == 'X' || c1 == 'y' || c1 == 'Y')
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS2;
  }
  else if (c2 == gfl2::str::EOM_CODE)  // ��������2�����ȏ�̏ꍇ�Ȃ̂ŁA1�����̏ꍇ�͎q���Ɋm��
  {
    return ITALIAN_INITIALSOUNDTYPE_CONS;
  }
  // �ȉ�2�����ȏ�̏ꍇ
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

// @brief �w�肵��������i���[�U�[���͕������z��j�̍ŏ��̉��^�C�v�𓾂�
// @param sp   EOM�I�[�̕�����
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

// @brief �w�肵�������������ǂ����𓾂�
// @param number ��
bool GrammaticalAttributeProvider::IsPluralNumber(int number) const
{
  if (m_currentLanguage == POKEMON_LANG_FRANCE)
  {
    // F: 0��1�������P��
    return (number != 0) && (number != 1);
  }
  else
  {
    // EIGS: 1�������P���i0�͕����j
    return number != 1;
  }
}

// @note �K�v�Ȃ珬���̒P������������
// F: ��Βl��2�����Ȃ�P���A���͕���
// EIGS: ��ɕ���

// �w�肵���������ꉹ�̃A���t�@�x�b�g���ǂ����𓾂�
bool GrammaticalAttributeProvider::isVowelAlphabet(gfl2::str::STRCODE c)
{
  //// [0, 255] �̕����̂����A�ꉹ�̃A���t�@�x�b�g
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
