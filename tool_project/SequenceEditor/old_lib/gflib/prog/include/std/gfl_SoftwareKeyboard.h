//======================================================================
/**
 * @file  gfl_SoftwareKeyboard
 * @brief �\�t�g�E�F�A�L�[�{�[�h
 * @author  ariizumi
 * @data
 */
//======================================================================

#ifndef __GFL_SOFTWARE_KEYBOARD_H__
#define __GFL_SOFTWARE_KEYBOARD_H__

#pragma once

#include <nn/swkbd.h>

#include <gfl_Heap.h>

namespace gfl
{
namespace std
{

//���[�U�[�����̌^
typedef nn::swkbd::UserWord UserWord;


/*!
    @brief  �L�[�{�[�h�̃^�C�v�񋓑�
*/
enum KeyboardType
{
    KEYBOARD_TYPE_FULL    = nn::swkbd::KEYBOARD_TYPE_FULL,             //!< �t���L�[�{�[�h
    KEYBOARD_TYPE_QWERTY  = nn::swkbd::KEYBOARD_TYPE_QWERTY,               //!< QWERTY�L�[�{�[�h
    KEYBOARD_TYPE_TENKEY  = nn::swkbd::KEYBOARD_TYPE_TENKEY,               //!< �e���L�[�{�[�h
    KEYBOARD_TYPE_MAX     = nn::swkbd::KEYBOARD_TYPE_MAX,

    KEYBOARD_TYPE_MAX_BIT = nn::swkbd::KEYBOARD_TYPE_MAX_BIT
};

/*!
    @brief  ���у{�^���̃^�C�v�񋓑�
*/
enum BottomButtonType
{
    BOTTOM_BUTTON_TYPE_1BUTTON = nn::swkbd::BOTTOM_BUTTON_TYPE_1BUTTON,     //!< 1�{�^��
    BOTTOM_BUTTON_TYPE_2BUTTON = nn::swkbd::BOTTOM_BUTTON_TYPE_2BUTTON,         //!< 2�{�^��
    BOTTOM_BUTTON_TYPE_3BUTTON = nn::swkbd::BOTTOM_BUTTON_TYPE_3BUTTON,         //!< 3�{�^��
    BOTTOM_BUTTON_TYPE_MAX     = nn::swkbd::BOTTOM_BUTTON_TYPE_MAX,

    BOTTOM_BUTTON_TYPE_MAX_BIT = nn::swkbd::BOTTOM_BUTTON_TYPE_MAX_BIT
};

/*!
    @brief  ���у{�^���̃C���f�b�N�X�񋓑�
*/
enum BottomButton
{
    BOTTOM_BUTTON_0   = nn::swkbd::BOTTOM_BUTTON_0,
    BOTTOM_BUTTON_1   = nn::swkbd::BOTTOM_BUTTON_1,
    BOTTOM_BUTTON_2   = nn::swkbd::BOTTOM_BUTTON_2,
    BOTTOM_BUTTON_MAX = nn::swkbd::BOTTOM_BUTTON_MAX,

    BOTTOM_BUTTON_1BUTTON           = nn::swkbd::BOTTOM_BUTTON_1BUTTON,         //!< 1�{�^���̃{�^��
    BOTTOM_BUTTON_2BUTTON_LEFT      = nn::swkbd::BOTTOM_BUTTON_2BUTTON_LEFT,    //!< 2�{�^���̍��{�^��
    BOTTOM_BUTTON_2BUTTON_RIGHT     = nn::swkbd::BOTTOM_BUTTON_2BUTTON_RIGHT,   //!< 2�{�^���̉E�{�^��
    BOTTOM_BUTTON_3BUTTON_LEFT      = nn::swkbd::BOTTOM_BUTTON_3BUTTON_LEFT,    //!< 3�{�^���̍��{�^��
    BOTTOM_BUTTON_3BUTTON_MIDDLE    = nn::swkbd::BOTTOM_BUTTON_3BUTTON_MIDDLE,  //!< 3�{�^���̒��{�^��
    BOTTOM_BUTTON_3BUTTON_RIGHT     = nn::swkbd::BOTTOM_BUTTON_3BUTTON_RIGHT,   //!< 3�{�^���̉E�{�^��

    BOTTOM_BUTTON_MAX_BIT           = nn::swkbd::BOTTOM_BUTTON_MAX_BIT
};

/*!
    @brief  �e���L�[�̊g���L�[�̃C���f�b�N�X�񋓑�
*/
enum TenkeyExtKey
{
    TENKEY_EXT_KEY_LEFT  = nn::swkbd::TENKEY_EXT_KEY_LEFT,            //!< �u0�v�L�[�̍��ׂ̃L�[
    TENKEY_EXT_KEY_RIGHT = nn::swkbd::TENKEY_EXT_KEY_RIGHT,               //!< �u0�v�L�[�̉E�ׂ̃L�[
    TENKEY_EXT_KEY_MAX   = nn::swkbd::TENKEY_EXT_KEY_MAX,

    TENKEY_EXT_KEY_MAX_BIT = nn::swkbd::TENKEY_EXT_KEY_MAX_BIT
};

/*!
    @brief  ���͊m��\�����񋓑�
*/
enum FinishableCondition
{
    //!< ��Ɋm��\
    FINISHABLE_CONDITION_NONE     = nn::swkbd::FINISHABLE_CONDITION_NONE,

    //!< 1�����ȏ�̓���
    FINISHABLE_CONDITION_ANY      = nn::swkbd::FINISHABLE_CONDITION_ANY,

    //!< �󔒋L���i�S�p�E���p�j�ȊO��1�����ȏ�̓���
    FINISHABLE_CONDITION_NON_SPACE = nn::swkbd::FINISHABLE_CONDITION_NON_SPACE,

    //!< �󔒋L���i�S�p�E���p�j�ȊO��1�����ȏ�̓��́A�܂��͖�����
    FINISHABLE_CONDITION_EXCEPT_SP = nn::swkbd::FINISHABLE_CONDITION_EXCEPT_SPACE_ONLY,

    //!< ���͉\�ȍő啶�����ȏ�̓���
    FINISHABLE_CONDITION_FULL     = nn::swkbd::FINISHABLE_CONDITION_FULL,

    FINISHABLE_CONDITION_MAX      = nn::swkbd::FINISHABLE_CONDITION_MAX,

    FINISHABLE_CONDITION_MAX_BIT  = nn::swkbd::FINISHABLE_CONDITION_MAX_BIT
};

/*!
    @brief  ���͕����̉B�����[�h
*/
enum MaskMode
{
    //!< �B�����Ȃ�
    MASK_MODE_DISABLE = nn::swkbd::MASK_MODE_DISABLE,

    //!< ���͌�ɑ����ɉB������
    MASK_MODE_IMMEDIATE = nn::swkbd::MASK_MODE_IMMEDIATE,

    //!< ���͂���1�b�҂��A���̕��������͂��ꂽ��B������
    MASK_MODE_WAIT_1SEC = nn::swkbd::MASK_MODE_WAIT_1SEC,

    MASK_MODE_MAX = nn::swkbd::MASK_MODE_MAX,

    MASK_MODE_MAX_BIT = nn::swkbd::MASK_MODE_MAX_BIT
};

/*!
    @brief  �ی�҂ɂ��g�p�����ꎞ�������[�h
*/
enum ParentalControlsMode
{
    //!< �ی�҂ɂ��g�p�����̈ꎞ�������s��Ȃ�
    PARENTAL_CONTROLS_MODE_DISABLE = nn::swkbd::PARENTAL_CONTROLS_MODE_DISABLE,

    //!< �ی�҂ɂ��g�p�����̈ꎞ�������s��
    PARENTAL_CONTROLS_MODE_ENABLE = nn::swkbd::PARENTAL_CONTROLS_MODE_ENABLE,

    // �{�̐ݒ�p�i�g�p�֎~�j
    PARENTAL_CONTROLS_MODE_SYSTEM_SETTINGS = nn::swkbd::PARENTAL_CONTROLS_MODE_SYSTEM_SETTINGS,
    PARENTAL_CONTROLS_MODE_MAX = nn::swkbd::PARENTAL_CONTROLS_MODE_MAX,

    PARENTAL_CONTROLS_MODE_MAX_BIT = nn::swkbd::PARENTAL_CONTROLS_MODE_MAX_BIT
};

/*!
    @brief  ���ʂ̕\�����@�������t���O
*/
enum UpperScreenFlag
{
    //��bit�Ǘ�

    //!< �A�v���P�[�V�����̉摜�����̂܂ܕ\�����܂��B
    UPPER_SCREEN_NORMAL = nn::swkbd::UPPER_SCREEN_NORMAL,

    //!< �A�v���P�[�V�����̉摜���Â����ĕ\�����܂��B
    UPPER_SCREEN_DARK = nn::swkbd::UPPER_SCREEN_DARK,

    UPPER_SCREEN_MAX_BIT = nn::swkbd::UPPER_SCREEN_MAX_BIT
};

/*!
    @brief  �e�L�X�g�`�F�b�N�̎�ނ������t���O
*/
enum TextCheckFlag
{
    //!< �e�L�X�g�`�F�b�N���s��Ȃ�
    TEXT_CHECK_DISABLE      = nn::swkbd::TEXT_CHECK_DISABLE,// = 0x0,

    //!< �����̓��͐��𐧌�����
    TEXT_CHECK_NUMBER       = nn::swkbd::TEXT_CHECK_NUMBER,// = 0x1,

    //!< �A�b�g�}�[�N�̓��͂��֎~����
    TEXT_CHECK_AT_MARK      = nn::swkbd::TEXT_CHECK_AT_MARK,// = 0x2,

    //!< �p�[�Z���g�L���̓��͂��֎~����
    TEXT_CHECK_PERCENT_SIGN = nn::swkbd::TEXT_CHECK_PERCENT_SIGN,// = 0x4,

    //!< �o�b�N�X���b�V���̓��͂��֎~����
    TEXT_CHECK_BACKSLASH    = nn::swkbd::TEXT_CHECK_BACKSLASH,// = 0x8,

    //!< �X�N���[����ɕ\������镶����Ƃ��Ė��̂�����̓��͂��֎~����
    TEXT_CHECK_BANNED_WORDS = nn::swkbd::TEXT_CHECK_BANNED_WORDS,// = 0x10,

    //!< �A�v���P�[�V�������e�L�X�g�`�F�b�N���s��
    TEXT_CHECK_APP          = nn::swkbd::TEXT_CHECK_APP,// = 0x20,

    TEXT_CHECK_MAX_BIT      = nn::swkbd::TEXT_CHECK_MAX_BIT,// = (1u << 31)
};

/*!
    @brief  �g���o�̓f�[�^�̎�ނ������t���O
*/
enum ExtOutputDataFlag
{
    //!< �g���o�̓f�[�^�Ȃ�
    EXT_OUTPUT_DATA_NONE = nn::swkbd::EXT_OUTPUT_DATA_NONE,// = 0x0,

    //!< �ŏI�����ԃf�[�^
    EXT_OUTPUT_DATA_STATUS = nn::swkbd::EXT_OUTPUT_DATA_STATUS,// = 0x1,

    //!< �\���ϊ��w�K�f�[�^
    EXT_OUTPUT_DATA_LEARNING = nn::swkbd::EXT_OUTPUT_DATA_LEARNING,// = 0x2,

    EXT_OUTPUT_DATA_MAX_BIT = nn::swkbd::EXT_OUTPUT_DATA_MAX_BIT,// = (1u << 31)
};

/*!
    @brief  �A�v���P�[�V�����ɂ��e�L�X�g�`�F�b�N����
*/
enum AppTextCheckResult
{
    //!< ���Ȃ��B���̂܂܃L�[�{�[�h�I���B
    APP_TEXT_CHECK_RESULT_OK = nn::swkbd::APP_TEXT_CHECK_RESULT_OK,// = 0,

    //!< ��肠��B�_�C�A���O��\�����ăL�[�{�[�h�I���B
    APP_TEXT_CHECK_RESULT_NG_FINISH = nn::swkbd::APP_TEXT_CHECK_RESULT_NG_FINISH,

    //!< ��肠��B�_�C�A���O��\�����ăL�[�{�[�h���s�B
    APP_TEXT_CHECK_RESULT_NG_CONTINUE = nn::swkbd::APP_TEXT_CHECK_RESULT_NG_CONTINUE,

    APP_TEXT_CHECK_RESULT_MAX = nn::swkbd::APP_TEXT_CHECK_RESULT_MAX,

    APP_TEXT_CHECK_RESULT_MAX_BIT = nn::swkbd::APP_TEXT_CHECK_RESULT_MAX_BIT,// = (1u << 31)
};

/*!
    @brief  ���^�[���R�[�h

            ����ɏI�������ꍇ�� 0 �ȏ�A�ُ�I�������ꍇ�͕��l�ƂȂ�܂��B
*/
enum ReturnCode
{
    //!< �s��
    RETURN_CODE_UNKNOWN = nn::swkbd::RETURN_CODE_UNKNOWN,// = -1,

    //!< �R���t�B�O�̒l���s��
    RETURN_CODE_INVALID_CONFIG = nn::swkbd::RETURN_CODE_INVALID_CONFIG,// = -2,

    //!< �������s���ɂ��ُ�I��
    RETURN_CODE_OUT_OF_MEMORY = nn::swkbd::RETURN_CODE_OUT_OF_MEMORY,// = -3,

    //!< �o�[�W�����ԍ��s��v
    //RETURN_CODE_INVALID_VERSION                 = -4,

    //!< 1�{�^���^�C�v�̉��у{�^���������ꂽ
    RETURN_CODE_BOTTOM_BUTTON_1BUTTON = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_1BUTTON,// = 0,

    //!< 2�{�^���^�C�v�̉��у{�^���̓��A���̃{�^���������ꂽ
    RETURN_CODE_BOTTOM_BUTTON_2BUTTON_LEFT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_2BUTTON_LEFT,// = 1,

    //!< 2�{�^���^�C�v�̉��у{�^���̓��A�E�̃{�^���������ꂽ
    RETURN_CODE_BOTTOM_BUTTON_2BUTTON_RIGHT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_2BUTTON_RIGHT,// = 2,

    //!< 3�{�^���^�C�v�̉��у{�^���̓��A���̃{�^���������ꂽ
    RETURN_CODE_BOTTOM_BUTTON_3BUTTON_LEFT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_3BUTTON_LEFT,// = 3,

    //!< 3�{�^���^�C�v�̉��у{�^���̓��A�����̃{�^���������ꂽ
    RETURN_CODE_BOTTOM_BUTTON_3BUTTON_MIDDLE = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_3BUTTON_MIDDLE,// = 4,

    //!< 3�{�^���^�C�v�̉��у{�^���̓��A�E�̃{�^���������ꂽ
    RETURN_CODE_BOTTOM_BUTTON_3BUTTON_RIGHT = nn::swkbd::RETURN_CODE_BOTTOM_BUTTON_3BUTTON_RIGHT,// = 5,

    //!< HOME �{�^���������ꂽ
    RETURN_CODE_HOME_BUTTON = nn::swkbd::RETURN_CODE_HOME_BUTTON,// = 10,

    //!< �\�t�g�E�F�A���Z�b�g�ƂȂ�g�ݍ��킹�̃{�^���������ꂽ
    RETURN_CODE_SOFTWARE_RESET = nn::swkbd::RETURN_CODE_SOFTWARE_RESET,// = 11,

    //!< �d���{�^���������ꂽ
    RETURN_CODE_POWER_BUTTON = nn::swkbd::RETURN_CODE_POWER_BUTTON,// = 12,

    //!< �ی�҂ɂ��g�p�����̈ꎞ�����ɐ�������
    RETURN_CODE_PARENTAL_CONTROLS_SUCCESS = nn::swkbd::RETURN_CODE_PARENTAL_CONTROLS_SUCCESS,// = 20,

    //!< �ی�҂ɂ��g�p�����̈ꎞ�����Ɏ��s����
    RETURN_CODE_PARENTAL_CONTROLS_FAILURE = nn::swkbd::RETURN_CODE_PARENTAL_CONTROLS_FAILURE,// = 21,

    //!< �A�v���P�[�V�����ɂ��e�L�X�g�`�F�b�N�̌��ʂ�NG������
    RETURN_CODE_APP_TEXT_CHECK_NG = nn::swkbd::RETURN_CODE_APP_TEXT_CHECK_NG,// = 30,

    RETURN_CODE_MAX_BIT = nn::swkbd::RETURN_CODE_MAX_BIT,// = (s32)(1u << 31)
};


class SoftwareKeyboard
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(SoftwareKeyboard);

public:

  //----------------------------------------------------------------------------
  /*
   *  @brief  �R���X�g���N�^
   *
   *  @param  heap::HeapBase *heap ��Ɨp�q�[�v ���f�o�C�X�������֎~�I
   *  @param  const u32 strLen ���͕�����̃T�C�Y
   */
  //-----------------------------------------------------------------------------
  SoftwareKeyboard( gfl::heap::HeapBase *heap , const u32 strLen );

  ~SoftwareKeyboard();

  //----------------------------------------------------------------------------
  /*
   *  @brief  �\�t�g�E�F�A�L�[�{�[�h�N��
   */
  //-----------------------------------------------------------------------------
  AppletWakeupState BootKeyboard( void );

  //----------------------------------------------------------------------------
  /*
   *  @brief  ���^�[���R�[�h�̎擾
   */
  //-----------------------------------------------------------------------------
  ReturnCode GetReturnCode(void);

  //-----------------------------------------------------------------------------
  //�e��ݒ�
  //-----------------------------------------------------------------------------
  //----------------------------------------------------------------------------
  /*
   *  @brief  �L�[�{�[�h�^�C�v�ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetKeyboardType( const KeyboardType kbType );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ���у{�^���̃^�C�v�ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetBottomButtonType( const BottomButtonType buttonType );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ���у{�^���̃e�L�X�g�ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetBottomButtonText( const BottomButton buttonPos , const wchar_t *text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �e���L�[�̊g���{�^���̃e�L�X�g�ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetTenkeyExtText( const TenkeyExtKey buttonPos , const wchar_t text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ���͊m��\�����ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetFinishableCondition( const FinishableCondition con );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ���͕����̉B�����[�h�ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetMaskMode( const MaskMode mode );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �ی�҂ɂ��g�p�����ꎞ�������[�h�ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetParentalControlsMode( const ParentalControlsMode mode );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ���ʂ̕\�����@�t���O�̐ݒ�
   *
   *  @param  const u32 flg UpperScreenFlag�̘_���a
   */
  //-----------------------------------------------------------------------------
  void SetUpperScreenFlag( const u32 flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �e�L�X�g�`�F�b�N�̎�ރt���O�̐ݒ�
   *
   *  @param  const u32 flg TextCheckFlag�̘_���a
   */
  //-----------------------------------------------------------------------------
  void SetTextCheckFlag( const u32 flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �����̍ő���͉\��
   *          SetTextCheckFlag��TEXT_CHECK_NUMBER ���Z�b�g����K�v����
   *
   *  @param  const u16 num ������
   */
  //-----------------------------------------------------------------------------
  void SetNumbersMax( const u16 num );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �K�C�h�e�L�X�g�̐ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetGuideText( const wchar_t *text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �\���ϊ��̐ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetEnablePrediction( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ���s�̐ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetEnableLinefeed( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �z�[���{�^���I���\��
   *          ��HOME�{�^������SetTextCheckFlag�̃`�F�b�N�������ɂȂ�܂�
   */
  //-----------------------------------------------------------------------------
  void SetEnableHomeButton( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �\�t�g�E�F�A���Z�b�g���\���H
   */
  //-----------------------------------------------------------------------------
  void SetEnableSoftwareReset( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �����\���̐ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetEnableFixedWidthMode( const bool flg );
  //----------------------------------------------------------------------------
  /*
   *  @brief  �����e�L�X�g�̐ݒ�
   *
   *  @param  const wchar_t *text ����������(NULL�I�[)
   *                              ������������Ȃ��������ꍇNULL��n���Ă��������B
   */
  //-----------------------------------------------------------------------------
  void SetDefaultText( const wchar_t *text );
  //----------------------------------------------------------------------------
  /*
   *  @brief  ���[�U�[�����̐ݒ�
   */
  //-----------------------------------------------------------------------------
  void SetUserWord( const UserWord *userWord , const u32 wordNum );



  //-----------------------------------------------------------------------------
  /**
   * @brief ���͂�����������擾����
   */
  //-----------------------------------------------------------------------------
  const wchar_t* GetInputString( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief ���͂���������̒������擾����
   */
  //-----------------------------------------------------------------------------
  u16 GetInputStringLength( void ) const;

  /** �����ԃf�[�^�̃T�C�Y��` */
  enum {
    STATUS_DATA_SIZE = nn::swkbd::CTR::STATUS_DATA_SIZE,
  };

  //-----------------------------------------------------------------------------
  /**
   * @brief  �����ԃf�[�^�̐擪�A�h���X���擾����
   *
   * @return �����ԃf�[�^�擪�A�h���X
   *
   * @note
   * �����ԃf�[�^�̃T�C�Y�� STATUS_DATA_SIZE �Œ�`����Ă��܂��B
   * �\�t�g�E�F�A�L�[�{�[�h�̓���I����A���̊֐��Ŏ擾�����f�[�^��ۑ����Ă����A
   * ���� BootKeyboard �̑O�� SetStatusData �Ɉ����Ƃ��Ĉ����n�����ƂŁA
   * �����ԁi���͕�����̎w��Ȃǁj�������p���ŃL�[�{�[�h���N���ł��܂��B
   */
  //-----------------------------------------------------------------------------
  const void* GetStatusData( void );

  //-----------------------------------------------------------------------------
  /**
   * @brief �����ԃf�[�^���Z�b�g����
   * @param statusData    �����ԃf�[�^
   */
  //-----------------------------------------------------------------------------
  void SetStatusData( const void* statusData );

private:

  gfl::heap::HeapBase *mHeap;
  u16            mInputStrLen;
  wchar_t*       mInputStr;
  wchar_t*       mDefaultStr;

  const UserWord *mUserWord;

  nn::swkbd::Parameter mKbdParam; // �L�[�{�[�h�̐ݒ�\���̂ƁA��ʃL���v�`����񂪓������\����

  u8             mStatusData[ STATUS_DATA_SIZE ];   ///< �����̏�Ԑݒ�f�[�^��ۑ����Ă���
  const void*    mInitialStatusData;                ///< ����J�n�O�̏�Ԑݒ�f�[�^���w�肷��

};



} //namespace std
} //namespace gfl


#endif //__GFL_SOFTWARE_KEYBOARD_H__
