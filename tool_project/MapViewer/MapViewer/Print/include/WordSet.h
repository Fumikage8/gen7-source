//============================================================================================
/**
 * @file    WordSet.h
 * @brief   �����񒆂̒P�ꖄ�ߍ��ݏ����N���X
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.03.03
 *
 * @li sango��wordset.h���ڐA
 */
//============================================================================================
#if !defined( WORDSET_H_INCLUDED )
#define WORDSET_H_INCLUDED
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include <pml/include/pmlib.h>

#include <System/include/PokemonVersion.h>
#include <PrintSystem.h>
#include <WordSetLoader.h>
#include <GrammaticalAttributeProvider.h>
///#include <Savedata/include/MyStatus.h>


// Register �֐��́A�ȉ��̂����ԍ����Ⴂ���̂�D�悵�Ďg�p����B
// 1��2���g�p���Ă����΁AEFIGS���@�Ή�OK�B�������A�A�C�e���Ȃǌ���������̂́A�P������������n���K�v������B
//
// 1) ��p�� Register* �֐��iRegisterPokeNickName �Ȃǁj
//
// 2) void  RegisterWord( u32 bufID, gfl::str::MsgData* man, u32 strID, Form form = FORM_DEFAULT );
//      ��p�� Register* �֐������܂ł��Ȃ��ꍇ�Ȃǂ͂���B
//
// 3) void  RegisterWord( u32 bufID, const gfl::str::StrBuf& word, print::GrammaticalAttribute grammaticalAttribute, Form form = FORM_DEFAULT );
//      MsgData ����擾�����������ۑ����Ďg���܂킷�Ƃ��ȂǁB
//      EFIGS���@�Ή�����ɂ́A print::GrammaticalAttribute ���Z�b�g�ŕۑ����Ă����K�v������i������̎�ނɂ���ẮA�Ή����Ȃ��Ƃ������f���\�j�B
//
// 4) void  RegisterWord( u32 bufID, const gfl::str::StrBuf& word, Grammer grm = GRAMMER_DEFAULT, Form form = FORM_DEFAULT );
//      �C�ӕ�����i�t���[���[�h���܂ށj�B�ʂɃ��[�J���C�Y�Ή����l������K�v������B

GFL_NAMESPACE_BEGIN(print)

class WordSet {
  GFL_FORBID_COPY_AND_ASSIGN( WordSet );

public:
  enum {
    DEFAULT_WORD_MAX  = 8,    ///< �o�^�ł���P�ꐔ�̃f�t�H���g�l
    DEFAULT_BUF_LEN   = 64,   ///< �P��P������m�ۂ����o�b�t�@���i�������j�̃f�t�H���g�l
  };

  /*----------------------------------*/
  /** ���@��                          */
  /*----------------------------------*/
  typedef enum {
    GRAMMER_MALE,     ///< �j��
    GRAMMER_FEMALE,   ///< ����
    GRAMMER_NEUTRAL,  ///< ����
    GRAMMER_DEFAULT = GRAMMER_MALE,
  }Grammer;

  /*----------------------------------*/
  /** �P��                            */
  /*----------------------------------*/
  typedef enum {
    FORM_SINGLE,      ///< �P��
    FORM_PLURAL,      ///< ����
    FORM_UNCOUNTABLE, ///< ��Z�A�g�p�֎~�iWordSet�����ł̂ݎg�p���܂��j
    FORM_DEFAULT = FORM_SINGLE,
  }Form;

  //=============================================================================================
  /**
   * �R���X�g���N�^�F�o�^�ł���P�ꐔ���P��̍ő啶�������f�t�H���g�ō쐬
   *
   * @param   pHeap   �g�p�q�[�v�n���h��
   */
  //=============================================================================================
  WordSet( gfl2::heap::HeapBase* pHeap );

  //=============================================================================================
  /**
   * �R���X�g���N�^�F�o�^�ł���P�ꐔ���P��̍ő啶�������w�肵�č쐬
   *
   * @param   wordMax
   * @param   bufLen
   * @param   pHeap       �g�p�q�[�v�n���h��
   */
  //=============================================================================================
  WordSet( u32 wordMax, u32 bufLen, gfl2::heap::HeapBase* pHeap );

  /** �f�X�g���N�^ */
  ~WordSet();

  //=============================================================================================
  /**
   * �����o�b�t�@��S�ċ󕶎���ŃN���A����
   */
  //=============================================================================================
  void ClearAllBuffer( void );


  //=============================================================================================
  /**
   * �o�^���ꂽ�P����g���ĕ����񐶐�����
   *
   * @param[out]   dstbuf   ������̕�������i�[����o�b�t�@
   * @param[in]    srcbuf   �������ƂȂ�i�^�O�R�[�h�����ߍ��܂ꂽ�j������
   */
  //=============================================================================================
  void Expand( gfl2::str::StrBuf* dstbuf, const gfl2::str::StrBuf* srcbuf );

  void SetWordSetLoader( WordSetLoader * pLoader );

#if 0
  //=============================================================================================
  /**
   * @param[in] p_readman
   * @param[in] pHeap
   * @param[in] read_id_bits
   */
  //=============================================================================================
  void PreLoad( gfl::heap::HeapBase* pHeap, gfl::fs::FileReadManager* p_readman, u32 read_id_bits );

  //=============================================================================================
  /**
   * @brief   ���O�ǂݍ��ݏ����I���҂�
   * @return  bool  true�̂Ƃ��A�Z�b�g�A�b�v����
   */
  //=============================================================================================
  bool WaitLoad( void );
#endif

  //------------------------------------------------------------------
  /**
   * PM_MALE PM_FEMALE PM_NEUTRAL ���� Grammer �ɕϊ�
   */
  //------------------------------------------------------------------
  static Grammer GrammerFromSex(u8 sex)
  {
    // Grammer �̒l���͈͊O�ł�������I���őI������镶����A��`�I���őI��������`�������_���ɂȂ邾���Ȃ̂�ASSERT�̂݁B
    GFL_ASSERT( sex <= PM_NEUTRAL );
    return static_cast<Grammer>( sex );
  }

  //------------------------------------------------------------------
  /**
   * pml::Sex ���� Grammer �ɕϊ��B�|�P�����̐��ʂɂ͂�����g�p����B
   */
  //------------------------------------------------------------------
  static Grammer GrammerFromPmlSex(pml::Sex sex);

  //------------------------------------------------------------------
  /**
   * �i�A�C�e�������̂��̂́j������ Form �ɕϊ��B
   */
  //------------------------------------------------------------------
  static Form FormFromCount(u32 count);

  //------------------------------------------------------------------
  /**
   * MsgData���當������擾���ēo�^�B������g���ƁAMsgData �� userParam ���當�@�p�����[�^���Z�b�g���܂��B
   */
  //------------------------------------------------------------------
  void  RegisterWord( u32 bufID, gfl2::str::MsgData* man, u32 strID, Form form = FORM_DEFAULT );

  //------------------------------------------------------------------
  /**
   * �C�ӕ�������p�����[�^�t���œo�^�BMsgData����擾�����ꍇ������g���B
   *
   * GrammaticalAttribute �́Agfl::MsgData::GetString �œ���ꂽ userParam ��
   * print::GrammaticalAttribute::FromMsgDataUserParam �ɓn���Đ�������B
   */
  //------------------------------------------------------------------
  void  RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, print::GrammaticalAttribute grammaticalAttribute, Form form = FORM_DEFAULT );

  //------------------------------------------------------------------
  /**
   * �C�ӕ�������p�����[�^�t���œo�^�B���[�J���C�Y�Ή����l�����邱�ƁB
   */
  //------------------------------------------------------------------
  void  RegisterWord( u32 bufID, const gfl2::str::StrBuf& word, Grammer grm = GRAMMER_DEFAULT, Form form = FORM_DEFAULT );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����̃j�b�N�l�[����o�^ (CoreParam)
   */
  //------------------------------------------------------------------
  void   RegisterPokeNickName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����̃j�b�N�l�[����o�^ (������A����)
   */
  //------------------------------------------------------------------
  void   RegisterPokeNickName( u32 bufID, const gfl2::str::StrBuf& str, pml::Sex sex );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����̃����X�^�[����o�^ (CoreParam)
   */
  //------------------------------------------------------------------
  void   RegisterPokeMonsName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����푰����o�^ (MonsNo)
   */
  //------------------------------------------------------------------
  void   RegisterPokeMonsNameNo( u32 bufID, MonsNo monsno );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����̐e����o�^
   */
  //------------------------------------------------------------------
  void   RegisterPokeOyaName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����̉ߋ��e����o�^
   */
  //------------------------------------------------------------------
  void   RegisterPokePastOyaName( u32 bufID, const pml::pokepara::CoreParam* pp );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀg���C����o�^
   */
  //------------------------------------------------------------------
#if 0 // @todo savedata�҂�
  void   RegisterBoxTrayName( u32 bufID, const savedata::BOX* p_box, const u32 tray_no );
#endif

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀg���[�i�[�^�C�v����o�^
   *
   * @param   bufID   �o�b�t�@ID
   * @param   trID    �g���[�i�[�^�C�v�w��
   */
  //------------------------------------------------------------------
  void   RegisterTrTypeName( u32 bufID, u16 trType );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀg���[�i�[����o�^
   *
   * @param   bufID   �o�b�t�@ID
   * @param   trID    �g���[�i�[ID
   */
  //------------------------------------------------------------------
  void   RegisterTrainerName( u32 bufID, u32 trID );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀg���[�i�[�^�C�v���ƃg���[�i�[���������������̂�o�^�i����A����̃o�g�����g���[�i�[�^�C�v�\���ɑΉ��j
   *
   * @param   dstBufID           �ݒ��o�b�t�@ID�BtrTypeNameBufID�AtrainerNameBufID�Ɠ����ł��悢�B
   * @param   trTypeNameBufID    �g���[�i�[�^�C�v�����Z�b�g�����o�b�t�@ID
   * @param   trainerNameBufID   �g���[�i�[�����Z�b�g�����o�b�t�@ID
   */
  //------------------------------------------------------------------
  void   RegisterTrTypeNameAndTrainerName(u32 dstBufID, u32 trTypeNameBufID, u32 trainerNameBufID);

  //------------------------------------------------------------------
  /**
   * �v���C���[����o�^
   *
   * @param   bufID   �o�b�t�@ID
   * @param   status  �v���C���[�X�e�[�^�X
   */
  //------------------------------------------------------------------
  void RegisterPlayerName( u32 bufID, const Savedata::MyStatus* status );

  //------------------------------------------------------------------
  /**
   * �v���C���[�j�b�N�l�[���ړ��q��o�^
   *
   * @param   bufID   �o�b�t�@ID
   * @param   status  �v���C���[�X�e�[�^�X
   */
  //------------------------------------------------------------------
#if 0   // @todo  �g�p���邱�ƂɂȂ�����L���ɂ��Ă������� 2015/03/11
  void RegisterPlayerNickNamePrefix( u32 bufID, const Savedata::MyStatus* status );
#endif

  //------------------------------------------------------------------
  /**
   * ���C�o������o�^
   *
   * @param   bufID   �o�b�t�@ID
   * @param   misc  ���C�o�������ݒ肳��Ă��� misc
   */
  //------------------------------------------------------------------
#if 0 // @todo savedata�҂�
  void RegisterRivalName( u32 bufID, const Savedata::Misc* misc );
#endif

  //------------------------------------------------------------------
  /**
   * �|�P�����i���U�j�^�C�v����o�^
   *
   * @param   bufID     �o�b�t�@ID
   * @param   typeID    �^�C�vID POKETYPE_NORMAL�Ȃ�
   */
  //------------------------------------------------------------------
  void RegisterPokeTypeName( u32 bufID, u32 typeID );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����̂Ƃ���������o�^
   *
   * @param   bufID     �o�b�t�@ID
   * @param   tokuseiID   �Ƃ�����ID
   */
  //------------------------------------------------------------------
  void RegisterTokuseiName( u32 bufID, u32 tokuseiID );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�����̂�����������o�^
   *
   * @param   bufID       �o�b�t�@ID
   * @param   seikakuID   ��������ID
   */
  //------------------------------------------------------------------
  void RegisterSeikakuName( u32 bufID, u32 seikakuID );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ��U����o�^
   *
   * @param   bufID     �o�b�t�@ID
   * @param   wazaID      ���UID
   */
  //------------------------------------------------------------------
  void RegisterWazaName( u32 bufID, u32 wazaID );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�ɃA�C�e������o�^
   *
   * @param   bufID     �o�b�t�@ID
   * @param   itemID    �A�C�e��ID
   * @param   itemCount �A�C�e���̌��i�P���`�ƕ����`��؂�ւ��邽�߁j
   */
  //------------------------------------------------------------------
  void RegisterItemName( u32 bufID, u32 itemID, u32 itemCount);

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�ɒP���`�̃A�C�e������o�^
   *
   * @param   bufID     �o�b�t�@ID
   * @param   itemID    �A�C�e��ID
   *
   * @note  ForceSingular ForcePlural �^�O�w��ŃA�C�e�����̒P����؂�ւ���d�g�݂𓱓��������߁A
   *        �v���O�������� RegisterItemNameSingular �� RegisterItemNamePlural ���Ăѕ�����K�v�͂Ȃ��Ȃ����B
   *        �u�i�V���b�v�ōw�����������˂�Ƃ��Ȃǁj�����w�肵�Ȃ��Ƃ��v�́A�Ƃ肠�����P���`�����Ă����悤�ɂ���B
   */
  //------------------------------------------------------------------
  //void RegisterItemNameSingular( u32 bufID, u32 itemID );

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ|�P�b�g����o�^
   *
   * @param   bufID     �o�b�t�@ID
   * @param   pocketNo    �|�P�b�gNO
   */
  //------------------------------------------------------------------
  void RegisterFieldPocketName( u32 bufID, u16 pocketNo );

  /* ------------------------------------------------------------------------- */
  /**
   * �w��o�b�t�@�ɒn����o�^
   *
   * @param bufID �o�b�t�@ID
   * @param placeNameID �n��ID�i�]�[������Ƃ�܂��j
   */
  /* ------------------------------------------------------------------------- */
  void RegisterPlaceName( u32 bufID, u32 placeNameID );

  /* ------------------------------------------------------------------------- */
  /**
   * @brief �w��o�b�t�@�ɓV�C����o�^
   *
   * @param bufID �o�b�t�@ID
   * @param weather_type �V��^�C�v
   */
  /* ------------------------------------------------------------------------- */
#if 0 // @todo �V������҂�
  void RegisterWeatherName( u32 bufID, xy_system::weather::Type weather_type );
#endif

  //------------------------------------------------------------------
  /**
   * �w��o�b�t�@�Ƀ��{������o�^
   *
   * @param   bufID     �o�b�t�@ID
   * @param   ribbonID  ���{��ID
   */
  //------------------------------------------------------------------
  void RegisterRibbonName( u32 bufID, u32 ribbonID );

  //------------------------------------------------------------------
  /**
   * ���l��o�^
   *
   * @param   bufID
   * @param   number      ���l
   * @param   keta        �ő�P�^��
   * @param   dispType    ���`�^�C�v
   * @param   codeType    �����R�[�h�^�C�v
   */
  //------------------------------------------------------------------
  void RegisterNumber( u32 bufID, s32 number, u32 keta, print::NumberDispType dispType, print::NumberCodeType codeType );


  //-------------------------------------------------------------------------------------------------
  /**
   * ����^�O�i�|�P�����j�b�N�l�[���A�g���[�i�[���j�̕\�����ɐ����������邽�߂̃t���O��ݒ肵�܂�
   *
   * @param   flag    true ���w�肷�邱�ƂŁAExpand���� �\�������^�O�𖄂ߍ��ނ悤�ɂȂ�܂��B
   *
   */
  //-------------------------------------------------------------------------------------------------
  void SetAreaWidthTagEnable( bool flag ){ m_bAreaWidthTagEnable = flag; }


private:

  /*----------------------------------*/
  /** �P��p�����[�^                  */
  /*----------------------------------*/
  union WORD_PARAM
  {
    struct {
      print::GrammaticalAttribute attribute;   ///< ���@����
      struct {
        u8   space_flag                   : 1; ///< �X�y�[�X�t���O
        u8   is_initialsound_calculated   : 1; ///< �ŏ��̉��̔���ς݃t���O�i�����x�������邽�߁j
        u8   form                         : 2; ///< ���@��
        u8                                : 4; ///< ���g�p
      };
      u16  width;                              ///< �X�y�[�X���i�X�y�[�X�t���O��1�̏ꍇ�j
    };
    u32   raw;
  };

  /*----------------------------------*/
  /** �P��                            */
  /*----------------------------------*/
  struct WORD
  {
    gfl2::str::StrBuf*    str;
    WORD_PARAM            param;
    u16                   areaWidthChars;  ///< �`��̈�v�Z�i0=�v�Z�������̂܂� / !0 = �`��̈敶�����i8bit�Œ菬���j
  };

  /*----------------------------------*/
  /** �萔                            */
  /*----------------------------------*/
  static const f32  DEFAULT_LIMIT_WIDTH_POKEMON;
  static const f32  DEFAULT_LIMIT_WIDTH_PERSON;
  static const u16  REF_WORD_ID_HERO;   // ��l���̕��@�������Q�Ƃ���
  static const u16  REF_WORD_ID_EXPANDING_WORD; // ���ݓW�J���̒P��̕��@�������Q�Ƃ���

  /*---------------------------------------------------------------------------*/
  /* �����o�[                                                                  */
  /*---------------------------------------------------------------------------*/
  gfl2::str::StrBuf*     m_tmpBuf;
  u32                    m_wordMax;
  WORD*                  m_word;
  gfl2::heap::HeapBase*  m_heapHandle;
  bool                   m_bAreaWidthTagEnable;
  WordSetLoader*         m_wordSetLoader;
  u32                    m_expandingWordId;   // �i�ċA�I�ȓW�J�ɂ��j���ݓW�J���̒P��
  gfl2::str::STRCODE     m_lastWordLastChar;  // �Ō�ɓW�J�����P��̍Ō�̕����i�؍��̏����ω��p�j
  WORD_PARAM             m_forceWordParam;    // m_forceWordParam �ŕ��@�������㏑������
  WORD_PARAM             m_forceWordParamMask;// m_forceWordParamMask �� 1 �̃r�b�g�����㏑������

  static inline WORD_PARAM defaultWordParam();
  static inline WORD_PARAM createWordParam(print::GrammaticalAttribute attribute, Form form = FORM_SINGLE);
  static inline WORD_PARAM createWordParam(Grammer grm, Form form = FORM_SINGLE);

  /** �����o�\�z�i�R���X�g���N�^����Ăяo���j */
  void       setupMembers( u32 wordMax, u32 bufLen, gfl2::heap::HeapBase* pHeap );

  /** �P�ꃏ�[�N������ */
  void       clearWord( u32 wordIdx );

  /** �P��P���o�^ */
  void        registerWord( u32 bufIdx, const gfl2::str::StrBuf& str, WORD_PARAM param, u16 areaWidthChars=0 );
  inline void registerWordFromMsgArc( u32 bufID, u32 arc_id, u32 msg_id, Form form = FORM_DEFAULT);
  u16         loadStringFromMsgArc( gfl2::str::StrBuf& dstbuf, u32 dat_id, u32 msg_id );

  /** �^�O���̂������Ō�̕������擾���� **/
  gfl2::str::STRCODE getLastChar(const gfl2::str::StrBuf* strbuf);
  /** dstbuf �ɁAsrcptr �̎w��EOM�I�[�̕������ǉ�����  **/
  static void addStr(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* srcptr);

  /** Expand �̃T�u���� **/
  void expandCore( gfl2::str::StrBuf* dstbuf, const gfl2::str::StrBuf* srcbuf, bool recursed );
  void expandNumber( gfl2::str::StrBuf* dstbuf, uint word_id, gfl2::str::STRCODE separator );
  const gfl2::str::STRCODE* processWordSetTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src, bool recursed);
  const gfl2::str::STRCODE* processKoreanParticleTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  const gfl2::str::STRCODE* processStringSelectTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  const gfl2::str::STRCODE* processWordFormTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  const gfl2::str::STRCODE* processGrammarForceTag(gfl2::str::StrBuf* dstbuf, const gfl2::str::STRCODE* src);
  uint getStringSelectIndex(uint tag_index, uint ref_word_id) const;
  uint getWordFormIndex(uint ref_word_id) const;
  WORD_PARAM getRefWordParam(uint ref_word_id) const;
};  // class WordSet


GFL_NAMESPACE_END(print)

#endif  // WORDSET_H_INCLUDED
