//=======================================================================================
/**
 * @file    PrintSystem.h
 * @brief   ������`��V�X�e��
 * @author  taya
 * @author  Hiroyuki Nakamura
 * @date    2011.03.08
 *
 * @li sango��printsys.h���ڐA
 */
//=======================================================================================
#if !defined( PRINTSYSTEM_H_INCLUDED )
#define PRINTSYSTEM_H_INCLUDED
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_Str.h>

#include "../../System/include/PokemonLanguage.h"


GFL_NAMESPACE_BEGIN( print )

/**
 *  �^�O�O���[�vID
 */
enum TagGroup {
  TAGGROUP_NULL           = 0x00,
  TAGGROUP_WORD           = 0x01,
  TAGGROUP_NUMBER         = 0x02,  ///< ���l�}��

  TAGGROUP_GRAMMAR_FORCE  = 0x10,  ///< ���@���������^�O
  TAGGROUP_STRING_SELECT  = 0x11,  ///< ������I���^�O
  TAGGROUP_WORDFORM_START = 0x11,  ///< ��`�I���^�O�O���[�v�̍ŏ�
  TAGGROUP_WORDFORM_MAX   = 0x19,  ///< ��`�I���^�O�O���[�v�̍Ō�i�܂܂��j
  TAGGROUP_KOREAN         = 0x19,  ///< �؍��ꏕ��

  TAGGROUP_GENERAL_CTRL   = 0xbd,  ///< �ėp�R���g���[������
  TAGGROUP_STREAM_CTRL    = 0xbe,  ///< ����郁�b�Z�[�W�p�R���g���[������
  TAGGROUP_SYSTEM         = 0xff,  ///< �G�f�B�^�V�X�e���^�O�p�R���g���[������
};

/**
 *  TAGGROUP_WORD �� Index
 */
enum WordTagIdx {
  TAGIDX_WORD_TRAINER_NAME          = 0,
  TAGIDX_WORD_POKE_NAME             = 1,
  TAGIDX_WORD_POKE_NICKNAME         = 2,
  TAGIDX_WORD_POKE_TYPE             = 3,
  TAGIDX_WORD_TOKUSEI_NAME          = 6,
  TAGIDX_WORD_WAZA_NAME             = 7,
  TAGIDX_WORD_ITEM_NAME             = 9,
  TAGIDX_WORD_POKE_NICKNAME_TRUTH   = 12,
  TAGIDX_WORD_TRAINER_TYPE          = 14,
  TAGIDX_WORD_POKETYPE_NAME         = 15,
  TAGIDX_WORD_APOWER_NAME           = 16,
  TAGIDX_WORD_TRAINER_NAME_FIELD    = 20,
  TAGIDX_WORD_APOWER_USER_NAME      = 141,
  TAGIDX_WORD_TRAINER_TYPE_AND_NAME = 158,
};

/**
 *  ���@���������^�O�� Index
 */
enum GrammarForceTagIndex {
  TAGIDX_GRAMMAR_FORCE_SINGULAR   = 0,  ///< �����P��
  TAGIDX_GRAMMAR_FORCE_PLURAL     = 1,  ///< ��������
  TAGIDX_GRAMMAR_FORCE_MASCULINE  = 2,  ///< �����j��
  //TAGIDX_GRAMMAR_FORCE_FEMININE   = 3,  ///< ���������i���̂Ƃ���s�v�Ȃ̂Œ�`���Ȃ��j
  //TAGIDX_GRAMMAR_FORCE_NEUTER     = 4,  ///< ���������i���̂Ƃ���s�v�Ȃ̂Œ�`���Ȃ��j
};

/**
 *  ������I���^�O�� Index
 */
enum StringSelectTagIndex {
  TAGIDX_STRING_SELECT_BY_GENDER                 = 0,  ///< ���ɂ��I��
  TAGIDX_STRING_SELECT_BY_QUANTITY               = 1,  ///< ���ɂ��I��
  TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY        = 2,  ///< ���Ɛ��ɂ��I��
  TAGIDX_STRING_SELECT_BY_GENDER_QUANTITY_GERMAN = 3,  ///< ���Ɛ��ɂ��I���i�h�C�c��j
};

/**
 *  TAGGROUP_KOREAN �� Index
 */
enum KoreanTagIdx {
  TAGIDX_KOREAN_PARTICLE = 0, // ����
};

/**
 *  TAGGROUP_GENERAL_CTRL �� Index
 */
enum GenTagIdx {
  TAGIDX_GENERAL_CHANGE_COLOR = 0,  ///< �F��ύX
  TAGIDX_GENERAL_BACK_COLOR,        ///< �F��߂�
  TAGIDX_GENERAL_X_CENTERING,       ///< �����`��w���W �Z���^�����O
  TAGIDX_GENERAL_X_FIT_RIGHT,       ///< �����`��w���W �E��
  TAGIDX_GENERAL_X_ADD,             ///< �����`��w���W ���Z
  TAGIDX_GENERAL_X_MOVE,            ///< �����`��w���W �w��
  TAGIDX_GENERAL_FIX_WIDTH_START,   ///< �����`��w�T�C�Y�Œ艻�����̊J�n
  TAGIDX_GENERAL_FIX_WIDTH_DIRECT,  ///< �����`��w�T�C�Y�Œ艻�����̊J�n�i�v�Z�ς݁j
  TAGIDX_GENERAL_FIX_WIDTH_END,     ///< �����`��w�T�C�Y�Œ艻�����̏I��

  TAGIDX_GENERAL_NOTUSED_MESSAGE = 255, ///< �f�o�b�O�p�B���g�p���b�Z�[�W�ł��邱�Ƃ������B
};

/**
 *  TAGGROUP_STREAM_CTRL �� Index
 */
enum StreamTagIdx {
  TAGIDX_STREAM_LINE_FEED = 0,  ///< �y�[�W�؂�ւ�(�s����
  TAGIDX_STREAM_PAGE_CLEAR,     ///< �y�[�W�؂�ւ�(�N���A
  TAGIDX_STREAM_WAIT_ONE,       ///< �E�F�C�g�ύX(�P��
  TAGIDX_STREAM_WAIT_CONT,      ///< �E�F�C�g�ύX(�p��
  TAGIDX_STREAM_WAIT_RESET,     ///< �E�F�C�g���Z�b�g
  TAGIDX_STREAM_CALLBACK_ONE,   ///< �R�[���o�b�N�����ύX(�P��
  TAGIDX_STREAM_CALLBACK_CONT,  ///< �R�[���o�b�N�����ύX(�p��
  TAGIDX_STREAM_CALLBACK_RESET, ///< �R�[���o�b�N�������Z�b�g
  TAGIDX_STREAM_CLEAR_WIN,      ///< �`��̈�N���A
  TAGIDX_STREAM_CTRL_SPEED,     ///< ���x�R���g���[��
};

/**
 *  TAGGROUP_SYSTEM �� Index
 */
enum SystemTagIdx {
  TAGIDX_SYSTEML_CHANGE_COLOR = 0,  //MessageStudio�ɂ��F�ύX
};

//EFIGS���ɉ������ɂԂ��{��
//�f�t�H���g��0.64�{�B�Œ�l��0.5�{�B
extern const f32 PRINT_EFIGS_SCALE;

// �e�L�X�g�{�b�N�X�y�C���Ɏw�肳��Ă��镶���̃X�P�[���l
// DrawUtil�ŕ�����`�悷��ۂɎg�p���Ă�������
extern const f32 LAYOUT_STRING_SCALE_X;    ///< ������X�X�P�[���l
extern const f32 LAYOUT_STRING_SCALE_Y;    ///< ������Y�X�P�[���l
extern const f32 LAYOUT_NUMBER_SCALE_X;    ///< ������X�X�P�[���l�i�����݂̂̏ꍇ�͂�����j
extern const f32 LAYOUT_NUMBER_SCALE_Y;    ///< ������Y�X�P�[���l�i�����݂̂̏ꍇ�͂�����j


//------------------------------------------------------------------
/**
 * ���l�𕶎��񉻂��鎞�̐��`�^�C�v
 */
//------------------------------------------------------------------
enum NumberDispType {
  NUM_DISP_LEFT,    ///< ���l��
  NUM_DISP_SPACE,   ///< �E�l�߁i�X�y�[�X���߁j
  NUM_DISP_ZERO,    ///< �E�l�߁i�[�����߁j
};


//------------------------------------------------------------------
/**
 * ���l�𕶎��񉻂��鎞�̕����R�[�h�w��
 */
//------------------------------------------------------------------
enum NumberCodeType {
  NUM_CODE_ZENKAKU,   ///< �S�p
  NUM_CODE_HANKAKU,   ///< ���p
  // ���[�J���C�Y��
  NUM_CODE_DEFAULT = NUM_CODE_HANKAKU,
};

//���C�A�E�g�ɉE�񂹁E�Z���^�����O���g�����̓��ꃏ�[�N
class FontTagProc;
struct LYT_TEXT_EX_WORK
{
  FontTagProc *tagProc;
};


//=============================================================================================
/**
 * �^�O�J�n�R�[�h���`�F�b�N���A������|�C���^WordSet�̒P�ꖄ�ߍ��ݑΏۃ^�O���ǂ�������
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  bool    �P�ꖄ�ߍ��ݑΏۃ^�O�ł����true
 */
//=============================================================================================
bool IsWordSetTagGroup( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * �^�O�J�n�R�[�h���`�F�b�N���A������|�C���^WordSet�̐��l�}���^�O���ǂ�������
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  bool    ���l�}���^�O�ł����true
 */
//=============================================================================================
bool IsWordSetNumberTagGroup( const gfl2::str::STRCODE* sp );


//=============================================================================================
/**
 * �^�O�J�n�R�[�h���`�F�b�N���A������|�C���^���؍���̏����^�O���ǂ�������
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  bool    �����^�O�ł����true
 */
//=============================================================================================
bool IsKoreanParticleTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * �^�O�J�n�R�[�h���`�F�b�N���A������|�C���^�����@���������^�O���ǂ�������
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  bool  ���@���������^�O�ł����true
 */
//=============================================================================================
bool IsGrammarForceTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * �^�O�J�n�R�[�h���`�F�b�N���A������|�C���^��������I���^�O���ǂ�������
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  bool  ������I���^�O�ł����true
 */
//=============================================================================================
bool IsStringSelectTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * �^�O�J�n�R�[�h���`�F�b�N���A������|�C���^����`�ω��^�O���ǂ�������
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă��邱�Ɓj
 *
 * @retval  bool  ��`�ω��^�O�ł����true
 */
//=============================================================================================
bool IsWordFormTag( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * ��`�ω��^�O���w���Ă��镶���񂩂�A�P��ԍ����擾
 *
 * @param   sp    ������|�C���^�i��`�ω��^�O���w���Ă���K�v������j
 *
 * @retval  u8    ��`�ԍ�
 */
//=============================================================================================
u8 GetWordFormTagWordIndex( const gfl2::str::STRCODE* sp );


//=============================================================================================
/**
 * �^�O�R�[�h����^�O�O���[�v���擾
 *
 * @param   tagCode   �^�O�R�[�h
 *
 * @retval  TagGroup    �^�O�O���[�v
 */
//=============================================================================================
TagGroup TagCodeToGroup( const gfl2::str::STRCODE tagCode );

//=============================================================================================
/**
 * �^�O�R�[�h����^�OIndex���擾
 *
 * @param   tagCode   �^�O�R�[�h
 *
 * @retval  u8        �^�OIndex�i�O���[�v���̔ԍ��j
 */
//=============================================================================================
u8 TagCodeToIndex( const gfl2::str::STRCODE tagCode );


//=============================================================================================
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃O���[�v���擾
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  TagGroup    �^�O�O���[�v
 */
//=============================================================================================
extern TagGroup GetTagGroup( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃O���[�v���C���f�b�N�X��Ԃ�
 *
 * @param   sp    ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 *
 * @retval  u8    �O���[�v���C���f�b�N�X
 */
//=============================================================================================
extern u8 GetTagIndex( const gfl2::str::STRCODE* sp );

//=============================================================================================
/**
 * �^�O�O���[�vID�A�O���[�v��Index ���������ă^�O�R�[�h������
 *
 * @param   groupID
 * @param   index
 *
 * @retval  gfl2::str::STRCODE
 */
//=============================================================================================
extern gfl2::str::STRCODE CreateTagCode( TagGroup groupID, u8 index );

//=============================================================================================
/**
 * �^�O�J�n�R�[�h���w���Ă��镶���񂩂�A���̃^�O�̃^�C�v���擾
 *
 * @param[in]   sp            ������|�C���^�i�^�O�J�n�R�[�h���w���Ă���K�v������j
 * @param[in]   paramIndex    �p�����[�^�C���f�b�N�X
 *
 * @retval  u16   �^�O�^�C�v
 */
//=============================================================================================
u16 GetTagParam( const gfl2::str::STRCODE* sp, u32 paramIndex );

//=============================================================================================
/**
 * ���l�̕����񉻁i�����t��32bit�j
 *
 * @param[out]   dst
 * @param[in]    number   ���l
 * @param[in]    keta     �ő�P�^��
 * @param[in]    dispType ���`�^�C�v
 * @param[in]    codeType �����R�[�h�^�C�v
 */
//=============================================================================================
void StrNumber( gfl2::str::StrBuf* dst, s32 number, u32 keta, NumberDispType dispType, NumberCodeType codeType );

//----------------------------------------------------------------------------------
/**
 * ���l�̕����񉻁i�����t�� double �j
 *
 * @param[out]   dst
 * @param[in]    number    ���l
 * @param[in]    keta_int  �������ő�P�^��
 * @param[in]    keta_dec  �������P�^��
 * @param[in]    dispType  ���`�^�C�v
 * @param[in]    codeType  �����R�[�h�^�C�v
 */
//----------------------------------------------------------------------------------
void StrRealNumber( gfl2::str::StrBuf* dst, double number, u32 keta_int, u32 keta_dec, NumberDispType dispType, NumberCodeType codeType );

//----------------------------------------------------------------------------------
/**
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷��i�^�O�l���j
 * ���E�񂹁E�Z���^�����O���g���ꍇ�͉��ɂ��郏�[�N���������[�N�n���ł��g����
 *
 * @param   target
 * @param   str
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g������MsgData���玩���擾
 */
//----------------------------------------------------------------------------------
void SetStringForLytTextBox( gfl2::lyt::LytTextBox * target, const gfl2::str::StrBuf* str , System::MSGLANGID langId = System::MSGLANGID_MAX );
void SetStringForLytTextBox( gfl2::lyt::LytTextBox * target, const wchar_t* str , System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷�邽�߂̃��[�N�쐬
 * ����^�O�p(�E�񂹃^�O�A�Z���^�����O�^�O�ɕK�v)
 *
 * @param   gfl::heap::HeapBase *heap �q�[�v
 *
 * @retval  LYT_TEXT_EX_WORK*   ���[�N
 */
//----------------------------------------------------------------------------------
LYT_TEXT_EX_WORK* CreateLytTextExWork( gfl2::heap::HeapBase *heap );

//----------------------------------------------------------------------------------
/**
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷�邽�߂̃��[�N�폜
 * ����^�O�p(�E�񂹃^�O�A�Z���^�����O�^�O�ɕK�v)
 *
 * @param   LYT_TEXT_EX_WORK*   ���[�N
 */
//----------------------------------------------------------------------------------
void DeleteLytTextExWork( LYT_TEXT_EX_WORK* work );

//----------------------------------------------------------------------------------
/**
 * ���C�A�E�g�̃e�L�X�g�{�b�N�X�ɁA�������ݒ肷��i����^�O�l���j�E�񂹃^�O�A�Z���^�����O�^�O�ɕK�v
 *
 * @param   target
 * @param   str
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g������MsgData���玩���擾
 */
//----------------------------------------------------------------------------------
void SetStringForLytTextBoxEx( gfl2::lyt::LytTextBox * target, const gfl2::str::StrBuf* str , LYT_TEXT_EX_WORK *work , System::MSGLANGID langId = System::MSGLANGID_MAX );
void SetStringForLytTextBoxEx( gfl2::lyt::LytTextBox * target, const wchar_t* str , LYT_TEXT_EX_WORK *work , System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * ������̒������擾����B(���s���ߍς�)
 *
 * @param   str       ������
 * @param   pane      �e�L�X�g�{�b�N�X�y�C��
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g�����Ŏ����擾
 */
//----------------------------------------------------------------------------------
int GetStringWidth( const gfl2::str::StrBuf* str, gfl2::lyt::LytTextBox * pane, System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * ������̒������擾����B(���s���ߍς�)
 *  print�̓����p�̊֐��ł��B��{���̊֐����g���Ă��������I�I�I
 *
 * @param   str       ������
 * @param   font      �t�H���g�t�@�C��
 * @param   fontWidth �t�H���g��(�X�P�[���ς݂̃T�C�Y�����Ă�������)
 */
//----------------------------------------------------------------------------------
int GetStringWidth( const gfl2::str::StrBuf* str, const nw::font::Font *font , f32 fontWidth , int *retLine = NULL );
int GetStringWidth( const wchar_t* str, const nw::font::Font *font , f32 fontWidth , int *retLine = NULL );

//----------------------------------------------------------------------------------
/**
 * ������̃X�P�[���l���擾����(DrawUtil���Ǝ��̕����`������鎞�p!)
 *
 * @param   langId ����ID(EFIGS�̏k���Ή�)sv_config���̌���ID������
 *                 �f�t�H���g�����Ŏ����擾
 */
//----------------------------------------------------------------------------------
f32 GetStringScale( System::MSGLANGID langId = System::MSGLANGID_MAX );


//Msg�t�@�C���֌W
//----------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�̌���ID��ݒ肷��B
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
void SetMessageLangId( System::MSGLANGID langId );

//----------------------------------------------------------------------------------
/**
 * ���b�Z�[�W�̌���ID���擾����B
 *
 * @return  ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
System::MSGLANGID GetMessageLangId(void);

//----------------------------------------------------------------------------------
/**
 * �ʏ�̃��b�Z�[�W��ARCID���擾����
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
u32 GetMessageArcId( System::MSGLANGID langId = System::MSGLANGID_MAX );

//----------------------------------------------------------------------------------
/**
 * �f�o�b�O�p�̃��b�Z�[�W��ARCID���擾����
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
#if PM_DEBUG
u32 GetMessageArcId_Debug( System::MSGLANGID langId = System::MSGLANGID_MAX );
#endif
//----------------------------------------------------------------------------------
/**
 * �X�N���v�g�p�̃��b�Z�[�W��ARCID���擾����
 *
 * @param   langId       ����ID(ROM�̖{�̂̌���ID�Ƃ͈Ⴂ�܂��I)
 */
//----------------------------------------------------------------------------------
u32 GetMessageArcId_Script( System::MSGLANGID langId = System::MSGLANGID_MAX );

//�����p
//----------------------------------------------------------------------------------
/**
 * ��������k�߂邩�𔻒肷��
 *
 * @param   target      �e�L�X�g�{�b�N�X�y�C��
 * @param   langId      ����ID(sv_config)
 */
//----------------------------------------------------------------------------------
bool CheckEnableShortStr( gfl2::lyt::LytTextBox * target, System::MSGLANGID langIdTemp );
bool CheckEnableShortStrLang( System::MSGLANGID langId );


GFL_NAMESPACE_END( print )

#endif  // PRINTSYSTEM_H_INCLUDED
