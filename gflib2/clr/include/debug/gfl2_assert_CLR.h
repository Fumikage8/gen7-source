#if !defined(GFLIB2_CLR_DEBUG_ASSERT_CLR_H_INCLUDED)
#define GFLIB2_CLR_DEBUG_ASSERT_CLR_H_INCLUDED
#pragma once
//===========================================================================
/**
 * @file    gfl2_assert_CLR.h
 * @brief   �A�T�[�g����
 */
//===========================================================================
#include <debug/include/gfl2_Assert.h>
#include <Windows.h>
#using <System.dll>
#using <System.Windows.Forms.dll>

namespace gfl2 { namespace clr { namespace debug {
public ref class Assert
{
public:
  /**
   * @brief ASSERT���[�v���R�[���o�b�N�֐��ݒ�
   */
  static void SetUpAssertLoopFunc(System::IntPtr handle);
  static System::Windows::Forms::Form^ m_ApplicationCore;
};

public class NativeAssert
{
public:
  /**
   * @brief C#����GFL_ASSERT�Œ�~�����ہA����ΏۂƂȂ�E�C���h�E�n���h���̐ݒ�
   * @param HWND hwnd �E�C���h�E�n���h��
   */
  static void SetHWND(HWND hwnd);

  /**
   * @brief GFL_ASSERT�Œ�~�����یĂяo�����֐�
   */
  static void AssertLoopFunc(const c8* pbuff, b32 bStop);

private:
  static HWND m_Hwnd;
};

}}}

#endif