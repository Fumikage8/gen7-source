//======================================================================
/**
 * @file  gfl_SoftwareKeyboard
 * @brief �\�t�g�E�F�A�L�[�{�[�h
 * @author  ariizumi
 * @data
 */
//======================================================================

#include <std/gfl_SoftwareKeyboard.h>

#include "core/gfl_Dummy.h"

namespace gfl
{
namespace std
{

SoftwareKeyboard::SoftwareKeyboard( heap::HeapBase *heap , const u32 strLen )
  :mHeap(heap)
  ,mUserWord(NULL)
  ,mInitialStatusData(NULL)
{
  mInputStrLen = 0;
  mInputStr   = static_cast<wchar_t*>(GflHeapAllocMemory( mHeap , sizeof(wchar_t)*(strLen+1) ));
  mDefaultStr = NULL;

  nn::swkbd::InitializeConfig( &mKbdParam.config );
  mKbdParam.config.textLengthMax = strLen;
}

SoftwareKeyboard::~SoftwareKeyboard()
{
  GflHeapFreeMemory( mInputStr );

  if( mDefaultStr )
  {
    GflHeapFreeMemory( mDefaultStr );
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  �\�t�g�E�F�A�L�[�{�[�h�N��
 *
 */
//-----------------------------------------------------------------------------
AppletWakeupState SoftwareKeyboard::BootKeyboard( void )
{
  // ���L�������̈�T�C�Y�v�Z
  s32  memSize  = nn::swkbd::GetSharedMemorySize( &mKbdParam.config, NULL, NULL );
  void *swkBuf  = NULL;

  // �K�v�ȋ��L�������T�C�Y���O�̏ꍇ�̓��������m�ۂ��Ȃ�
  if( memSize > 0 )
  {
    swkBuf = GflHeapAllocMemoryAlign4096(mHeap , memSize);
  }

  //------------------------
  //SDK4.0����R�}���h���X�g�����������邱�Ƃ��K�{�ɂȂ�܂����I
  //------------------------
  nngxWaitCmdlistDone();
  //------------------------

  // �L�[�{�[�h�Ăяo��
  mKbdParam.config.inputTextLength = 0;
  mKbdParam.config.extOutputDataFlag |= EXT_OUTPUT_DATA_STATUS;
  AppletWakeupState wstate;
  bool ret = nn::swkbd::StartKeyboardApplet( &wstate,
                                  &mKbdParam,
                                  swkBuf,   // 4096�o�C�g��������Ă���K�v������܂��B�f�o�C�X�������͎w��ł��܂���
                                  memSize,  // 4096�̔{���ł���K�v������܂�
                                  mDefaultStr,
                                  mUserWord,
                                  mInitialStatusData );
  if( swkBuf != NULL )
  {
    // ���͂��ꂽ��������R�s�[
    mInputStrLen = mKbdParam.config.inputTextLength;
    gfl::std::MemCopy( swkBuf, mInputStr, mKbdParam.config.inputTextLength * sizeof(wchar_t) );

    // ������I�[�R�[�h���Z�b�g
    mInputStr[mInputStrLen] = '\0';

    // �����ԃf�[�^��ۑ����Ă���
    void* statusDataPtr = static_cast<u8*>(swkBuf) + mKbdParam.config.statusData;
    gfl::std::MemCopy( statusDataPtr, mStatusData, sizeof(mStatusData) );

    GflHeapFreeMemory(swkBuf);
  }

  //�󋵂ɉ����ďI���Ή��B���C�����[�v��applet�������E���Ă���邩���B
  GFL_PRINT("Swkbd return[%d]\n",wstate);
  return wstate;
}

//----------------------------------------------------------------------------
/*
 *  @brief  ���^�[���R�[�h�̎擾
 */
//-----------------------------------------------------------------------------
ReturnCode SoftwareKeyboard::GetReturnCode(void)
{
  return static_cast<ReturnCode>(mKbdParam.config.returnCode);
}


//----------------------------------------------------------------------------
/*
 *  @brief  �L�[�{�[�h�^�C�v�ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetKeyboardType( const KeyboardType kbType )
{
  mKbdParam.config.keyboardType = static_cast<nn::swkbd::KeyboardType>(kbType);
}

//----------------------------------------------------------------------------
/*
 *  @brief  ���у{�^���̃^�C�v�ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetBottomButtonType( const BottomButtonType buttonType )
{
  mKbdParam.config.bottomButtonType = static_cast<nn::swkbd::BottomButtonType>(buttonType);;
}


//----------------------------------------------------------------------------
/*
 *  @brief  ���у{�^���̃e�L�X�g�ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetBottomButtonText( const BottomButton buttonPos , const wchar_t *text )
{
  gfl::str::StrCopy( mKbdParam.config.bottomButtonText[buttonPos] , text );
}


//----------------------------------------------------------------------------
/*
 *  @brief  �e���L�[�̊g���{�^���̃e�L�X�g�ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetTenkeyExtText( const TenkeyExtKey buttonPos , const wchar_t text )
{
  mKbdParam.config.tenkeyExtKeyChar[buttonPos] = text;
}

//----------------------------------------------------------------------------
/*
 *  @brief  ���͊m��\�����ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetFinishableCondition( const FinishableCondition con )
{
  mKbdParam.config.finishableCondition = static_cast<nn::swkbd::FinishableCondition>(con);
}

//----------------------------------------------------------------------------
/*
 *  @brief  ���͕����̉B�����[�h�ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetMaskMode( const MaskMode mode )
{
  mKbdParam.config.maskMode = static_cast<nn::swkbd::MaskMode>(mode);
}

//----------------------------------------------------------------------------
/*
 *  @brief  �ی�҂ɂ��g�p�����ꎞ�������[�h�ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetParentalControlsMode( const ParentalControlsMode mode )
{
  mKbdParam.config.parentalControlsMode = static_cast<nn::swkbd::ParentalControlsMode>(mode);
}

//----------------------------------------------------------------------------
/*
 *  @brief  ���ʂ̕\�����@�t���O�̐ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetUpperScreenFlag( const u32 flg )
{
  mKbdParam.config.upperScreenFlag = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  �e�L�X�g�`�F�b�N�̎�ރt���O�̐ݒ�
 *
 *  @param  const u32 flg TextCheckFlag�̘_���a
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetTextCheckFlag( const u32 flg )
{
  mKbdParam.config.textCheckFlag = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  �����̍ő���͉\��
 *          SetTextCheckFlag��TEXT_CHECK_NUMBER ���Z�b�g����K�v����
 *
 *  @param  const u16 num ������
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetNumbersMax( const u16 num )
{
  mKbdParam.config.numNumbersMax = num;
}

//----------------------------------------------------------------------------
/*
 *  @brief  �K�C�h�e�L�X�g�̐ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetGuideText( const wchar_t *text )
{
  gfl::str::StrCopy( mKbdParam.config.guideText , text );
}

//----------------------------------------------------------------------------
/*
 *  @brief  �\���ϊ��̐ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnablePrediction( const bool flg )
{
  mKbdParam.config.prediction = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  ���s�̐ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableLinefeed( const bool flg )
{
  mKbdParam.config.lineFeed = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  �����\���̐ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableFixedWidthMode( const bool flg )
{
  mKbdParam.config.fixedWidthMode = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  �z�[���{�^���I���\��
 *          ��HOME�{�^������SetTextCheckFlag�̃`�F�b�N�������ɂȂ�܂�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableHomeButton( const bool flg )
{
  mKbdParam.config.homeButton = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  �\�t�g�E�F�A���Z�b�g���\���H
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetEnableSoftwareReset( const bool flg )
{
  mKbdParam.config.softwareReset = flg;
}

//----------------------------------------------------------------------------
/*
 *  @brief  �����e�L�X�g�̐ݒ�
 *
 *  @param  const wchar_t *text ����������(NULL�I�[)
 *                              ������������Ȃ��������ꍇNULL��n���Ă��������B
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetDefaultText( const wchar_t *text )
{
  if( text )
  {
    if( mDefaultStr == NULL )
    {
      mDefaultStr = static_cast<wchar_t*>(GflHeapAllocMemory( mHeap , sizeof(wchar_t)*(mKbdParam.config.textLengthMax+1) ));
    }
    gfl::str::StrCopy( mDefaultStr , text );
  }
  else
  {
    if( mDefaultStr )
    {
      GflHeapFreeMemory( mDefaultStr );
    }
    mDefaultStr = NULL;
  }
}

//----------------------------------------------------------------------------
/*
 *  @brief  ���[�U�[�����̐ݒ�
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetUserWord( const UserWord *userWord , const u32 wordNum )
{
  mUserWord = userWord;
  mKbdParam.config.numUserWords = wordNum;
}




//-----------------------------------------------------------------------------
/**
 * @brief ���͂�����������擾����
 */
//-----------------------------------------------------------------------------
const wchar_t* SoftwareKeyboard::GetInputString( void ) const
{
  return mInputStr;
}

//-----------------------------------------------------------------------------
/**
 * @brief ���͂���������̒������擾����
 */
//-----------------------------------------------------------------------------
u16 SoftwareKeyboard::GetInputStringLength( void ) const
{
  return mInputStrLen;
}

//-----------------------------------------------------------------------------
/**
 * �����ԃf�[�^�̐擪�A�h���X���擾����
 *
 * @return �����ԃf�[�^�擪�A�h���X
 */
//-----------------------------------------------------------------------------
const void* SoftwareKeyboard::GetStatusData( void )
{
  return mStatusData;
}
//-----------------------------------------------------------------------------
/**
 * @brief �����ԃf�[�^���Z�b�g����
 * @param statusData    �����ԃf�[�^
 */
//-----------------------------------------------------------------------------
void SoftwareKeyboard::SetStatusData( const void* statusData )
{
  mInitialStatusData = statusData;
}


} //namespace std
} //namespace gfl


