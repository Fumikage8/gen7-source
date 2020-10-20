//=============================================================================
/**
 * @file   gfl_AutoStackManager.h
 * @brief  StackMemoryAutoStackManager �����N���X
 * @author suzuki_hisanao
 * @date   2012.6.18
 */
//=============================================================================
#ifndef __GFL_AUTO_STACK_MANAGER_H__
#define __GFL_AUTO_STACK_MANAGER_H__
#pragma once

#include <nn.h>
#include <gfl_Heap.h>
#include <gfl_Macros.h>

namespace gfl {
namespace base {

/**
* @brief StackMemoryAutoStackManager �����N���X<br>
*        <br>
*        ���<br>
*        <br>
*        �@�@nn::os::Thread::StartUsingAutoStack()<br>
*        �@�@nn::os::Thread::TryStartUsingAutoStack()<br>
*        <br>
*        �R�[�����Ɏg�p�����X�^�b�N�G���A���m�ۂ��邽�߂̃X�^�b�N�}�l�[�W���B<br>
*        <br>
*        �Ȃ��Ann::nex::Core::SetThreadMode()�̈�����<br>
*        <br>
*        �@�@nn::nex::ThreadModeUnsafeTransportBuffer<br>
*        �@�@nn::nex::ThreadModeInternal<br>
*        <br>
*        �����w�肵���ꍇ�ɂ́A�����X���b�h�ɂĖ{�}�l�[�W���o�R�ŃX�^�b�N�G���A���m�ۂ����B<br>
*        �m�ۂ����X�^�b�N�G���A��4096�o�C�g�A���C�����g�Ŋm�ۂ����B<br>
*        �܂��A�X�^�b�N�j�󎞂ɂ́A�X�^�b�N�O�G���A�ւ̃A�h���X�Q�Ƃ��s��ꂽ�|��Assert�����s�����B<br>
*/
class AutoStackManager : public nn::os::StackMemoryAutoStackManager
{
  GFL_FORBID_COPY_AND_ASSIGN( AutoStackManager );

public:
  /***************************************************************************
   * �R���X�g���N�^
   * @param heap �X�^�b�N�̈�Ƃ��Ďg�p������p�q�[�v�G���A�B<br>
   *             Nex�Ŏg�p���邱�Ƃ��l�����A�f�o�C�X�q�[�v���w�肵�Ă͂Ȃ�Ȃ��B<br>
   *             �܂��A�{�������𑼂̃��W���[�����g�p���Ă͂Ȃ�Ȃ��B
   *
   * @return none
   ***************************************************************************/
  AutoStackManager( gfl::heap::HeapBase* heap );

  /***************************************************************************
   * �f�X�g���N�^
   * @return none
   ***************************************************************************/
  virtual ~AutoStackManager( void );


public:
  /***************************************************************************
   * @brief �X�^�b�N�p�̃��������m�ۂ���
   *
   * @param stackSize �m�ۂ��ׂ��X�^�b�N�̃T�C�Y
   *                  �T�C�Y��4096�̔{���łȂ��ꍇ�A�����I��4096�̔{�����m�ۂ����
   *
   * @return �m�ۂ����X�^�b�N�p�������̍Ō���ւ̃|�C���^
   ***************************************************************************/
  virtual void* Construct( size_t stackSize );

  /***************************************************************************
   * @brief �X�^�b�N�p�̃��������������
   *
   * @param pStackBottom ������ׂ��X�^�b�N�B
   * @param isError �G���[�ɔ���������ۂ���\��
   *
   * @return none
   ***************************************************************************/
  virtual void Destruct( void * pStackBottom, bool isError );

private:
  static const u32                STACK_SIZE = 0x1000;      // �f�X�g���N�^�p���[�N�T�C�Y
  //
  gfl::heap::FndAllocator*        m_allocator;              // �X�^�b�N�p�A���P�[�^
  nn::os::StackBuffer<STACK_SIZE> m_stackForDestruct;       // �f�X�g���N�^�p���[�N�G���A
};



} // namespace base
} // namespace gfl



#endif // __GFL_AUTO_STACK_MANAGER_H__
