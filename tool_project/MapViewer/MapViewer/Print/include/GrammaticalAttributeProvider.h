//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
//  @file   GrammaticalAttributeProvider.h
//  @brief  �P��̕��@�I�ȑ����i���A�Z�A�ꓪ�j���擾����
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
// @brief ���@�����i���A���A�ŏ��̉��A�Z�j������킷�B
 */
union GrammaticalAttribute {
  struct {
    u8  Grammer       : 2;   ///< ���@���i0 = �j���A1 = �����A2 = �����jWordSet::Grammer �Ɠ���
    u8  InitialSound  : 2;   ///< �ŏ��̉��^�C�v�i������L�j
    u8  IsUncountable : 1;   ///< ��Z���ǂ��� (0 = �Z�A1 = ��Z�j
    u8  IsAlwaysPlural: 1;   ///< ��ɕ������ۂ� (�u��ɕ����v���u��Z�v�͋֎~)
    u8  ExtraAttribute: 2;   ///< ���̑��ǉ��̑����B
                             ///   ���݁A�t�����X��X�y�C����Ńo�g���̃g���[�i�[�^�C�v+�g���[�i�[���̕ω��p�^�[���Ɏg�p�B
  };
  u8 Raw;

  // ���b�Z�[�W�o�C�i����userParam����GrammaticalAttribute�𐶐�����
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

  // @brief ������
  // @param pHeap ���@�����f�[�^�ǂݍ��ݐ�̃q�[�v
  void Initialize(gfl2::heap::HeapBase* pHeap);

  // @brief �w�肵������̕��@�����f�[�^��ǂݍ���
  // @param language �ύX��̌���
  void ChangeLanguage(uint language);

  // @brief �I������
  void Finalize();

  // @brief ��`�ω�����P��̌�`�𓾂�
  // @param word_id     �P��ID
  // @param wordform_id ��`ID
  const gfl2::str::STRCODE* GetWordForm(uint word_id, uint wordform_id) const;

  // @brief �w�肵��������i���[�U�[���͕������z��j�̍ŏ��̉��^�C�v�𓾂�
  // @param sp   EOM�I�[�̕�����
  uint GetInitialSoundType(const gfl2::str::STRCODE* sp) const;

  // @brief �w�肵�������������ǂ����𓾂�
  // @param number ��
  bool IsPluralNumber(int number) const; 

private:
  // ���@���� dat �ւ̃A�N�Z�X
  const GrammarDatHeader* getHeader() const;
  const u8* getWordFormOffsets() const;

  // �ǂݍ��݊֘A
  static void OpenArcFile();
  static void CloseArcFile();
  void LoadData(uint datId);
  static u32 getMaxDataSize();
  static uint getDatId(uint lang);

  // �ŏ��̉��^�C�v����
  static uint getInitialSoundTypeOther(const gfl2::str::STRCODE* sp);
  static uint getInitialSoundTypeFrench(const gfl2::str::STRCODE* sp);
  static uint getInitialSoundTypeItalian(const gfl2::str::STRCODE* sp);
  static bool isVowelAlphabet(gfl2::str::STRCODE c);

  u8* mpData; // ���@���� dat ���i�[���郁����
  u32 mpDataSize; // �m�ۂ���mpData�̃T�C�Y
  uint m_currentLanguage;  // ���[�h���Ă��錾��
};

} // namespace print

SINGLETON_INSTANCE_DECLARATION(print::GrammaticalAttributeProvider);

#endif // GRAMMATICALATTRIBUTEPROVIDER_H_INCLUDED
