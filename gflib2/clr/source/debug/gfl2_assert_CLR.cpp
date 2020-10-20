//===========================================================================
/**
 * @file    gfl2_assert_CLR.cpp
 * @brief   �A�T�[�g����
 */
//===========================================================================
#include <clr/include/debug/gfl2_assert_CLR.h>

namespace gfl2 { namespace clr { namespace debug {

/**
 * @brief ASSERT���[�v���R�[���o�b�N�֐��ݒ�
 * @param GFL_ASSERT�œ�����~�����ہA����ΏۂƂȂ�E�C���h�E�n���h��
 */
void Assert::SetUpAssertLoopFunc(System::IntPtr handle)
{
  NativeAssert::SetHWND(reinterpret_cast<HWND>(handle.ToPointer()));
  gfl2::debug::GFL_ASSERT_SetLoopFunc(NativeAssert::AssertLoopFunc);
}

HWND NativeAssert::m_Hwnd;  //!< @brief GFL_ASSERT�œ�����~�����ہA����ΏۂƂȂ�E�C���h�E�n���h��

/**
 * @brief C#����GFL_ASSERT�Œ�~�����ہA����ΏۂƂȂ�E�C���h�E�n���h���̐ݒ�
 * @param HWND hwnd �E�C���h�E�n���h��
 */
void NativeAssert::SetHWND(HWND hwnd)
{
  m_Hwnd = hwnd;
}

/**
 * @brief GFL_ASSERT�Œ�~�����یĂяo�����֐�
 */
void NativeAssert::AssertLoopFunc(const c8* pbuff, b32 bStop)
{
  MessageBoxA(NULL, pbuff, "GFL_ASSERT", MB_ICONERROR);
  // �A�v���P�[�V�����������I��
  System::Environment::Exit(0);
}

}}}
