#ifndef __GFL_FND_ALLOCATOR_H__
#define __GFL_FND_ALLOCATOR_H__
#pragma once
//======================================================================
/**
 * @file  gfl_FndAllocator.h
 * @brief nn::fnd::IAllocator::Allocate����
 * @author  suzuki_hisanao
 * @data  2012/6/18
 */
//======================================================================

#include <nw.h>
#include <nw/os.h>

namespace gfl{
namespace heap{

class FndAllocator: public nn::fnd::IAllocator
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(FndAllocator);
public:

  /**
   * @brief  �R���X�g���N�^
   * @param  heapBase �g�p����q�[�v�G���A
   * @return none
   */
  FndAllocator(HeapBase *heapBase);

  /**
   * @brief �f�X�g���N�^
   */
  virtual ~FndAllocator();

public:
  /**
   * @brief �������m��
   * @param size �m�ۃT�C�Y
   * @param alignment �A���C�����g�T�C�Y
   */
  virtual void* Allocate( size_t size , s32 alignment );

  /**
   * @brief ���������
   * @param memory ����Ώۃ������A�h���X
   */
  virtual void Free( void *memory );

public:
  /**
   * @brief �ݒ肳��Ă���q�[�v�x�[�X�̎擾
   * @return �ݒ�q�[�v�x�[�X
   */
  HeapBase*  GetHeapBase(void);

  /**
   * @brief �������m�ی���Bottom����擾����悤�ɐݒ�
   * @param flg top����擾����Ȃ�false,
   */
  void       SetIsAllocLow( bool flg );

private:
  HeapBase *m_heapBase; // �q�[�v�x�[�X
  bool     m_AllocLow;  // ������Bottom�m�ۃt���O
};




} //namespace gfl
} //namespace heap


#endif // __GFL_FND_ALLOCATOR_H__
