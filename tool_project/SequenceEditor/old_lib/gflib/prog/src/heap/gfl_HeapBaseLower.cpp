//======================================================================
/**
 * @file	gfl_HeapBaseLower.cpp
 * @brief	GFL�q�[�v���N���X
 * @author	ariizumi
 * @data	10/10/05
 */
//======================================================================
#include <string.h>
#include <gflib.h>
#include "gfl_HeapLocal.h"
#include "heap/gfl_HeapBaseLower.h"

namespace gfl
{
namespace heap
{

GflHeapBaseLower::GflHeapBaseLower(GflHeapBase *parent)
:m_parentHeap(parent)
{
}

//----------------------------------------------------------------------------
/**
 *  @brief  �m�ۂ����������̃��T�C�Y
 *
 *  @param  void* buf ���T�C�Y���郁����  
 *  @param  int       �ύX��̃T�C�Y
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::ResizeMemory( void* buf , const int newSize)
{
  m_parentHeap->ResizeMemory(buf,newSize);
}

//----------------------------------------------------------------------------
/**
 *  @brief  �������󋵂̕\��
 *          �Ǝ��ŗp�ӂ���MemoryHeader�����Ƀ_���v���܂�
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::Dump(void)
{
  m_parentHeap->Dump();
}


//----------------------------------------------------------------------------
/**
 *  @brief  �������󋵂̕\��
 *          ���C�u�����Ȃǂŗp�ӂ��ꂽ�֐����g���_���v���܂�
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::SystemDump(void)
{
  m_parentHeap->SystemDump();
}


//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�j��`�F�b�N
 *
 *  @param  none
 *  @return  bool true���Ă��Ȃ� false���Ă���
 */
//-----------------------------------------------------------------------------
bool GflHeapBaseLower::Check(void)
{
  return m_parentHeap->Check();
}


//----------------------------------------------------------------------------
/**
 *  @brief  �f�t�H���g�A���C�����g�l�̎擾
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetDefaultAlign(void)
{
  return m_parentHeap->GetDefaultAlign();
}

//----------------------------------------------------------------------------
/**
 *  @brief  HEAPID�̎擾
 *
 *  @return  HEAPID
 */
//-----------------------------------------------------------------------------
HEAPID GflHeapBaseLower::GetHeapId(void) const
{
  return m_parentHeap->GetHeapId();
}

//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�̃T�C�Y���擾���܂�
 *
 *  @return  int �T�C�Y
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetTotalSize(void)
{
  return m_parentHeap->GetTotalSize();
}


//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�̍��v�󂫃T�C�Y���擾���܂�
 *
 *  @return  int �T�C�Y
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetTotalFreeSize(void)
{
  return m_parentHeap->GetTotalFreeSize();
}


//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�Ŋm�ۂł���ő�̃T�C�Y���擾���܂�
 *
 *  @return  int �T�C�Y
 */
//-----------------------------------------------------------------------------
int GflHeapBaseLower::GetTotalAllocatableSize(void)
{
  return m_parentHeap->GetTotalAllocatableSize();
}

//AllocCount�̑���(HeapBaseLower���瑀�삪����̂ł��������g���I)
int GflHeapBaseLower::GetAllocCount(void)const
{
  return m_parentHeap->GetAllocCount();
}
void GflHeapBaseLower::IncAllocCount(void)
{
  m_parentHeap->IncAllocCount();
}
void GflHeapBaseLower::DecAllocCount(void)
{
  m_parentHeap->DecAllocCount();
}

//�ȉ��������z�֐�
//�p����Ŏ������Ă��������B
//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�̍쐬
 *          friend��gfl::heap::Manager����̂݌Ăяo��
 *
 *  @param  void *buf �m�ۗp�o�b�t�@
 *  @param  const int size �o�b�t�@�T�C�Y
 *  @param  void* param ���[�U�[�p�����[�^�[
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::CreateHeap( void *buf , const int size , void *param )
{
  GFL_UNUSED(buf);
  GFL_UNUSED(size);
  GFL_UNUSED(param);
  GFL_ASSERT_STOP_MSG(0,"GlfHeap HeapBaseLower is can't create!!\n"); //@check �V�X�e���œ���ȍ������邽�߁B���[�U�[�̌Ăяo���֎~
}


//----------------------------------------------------------------------------
/**
 *  @brief  �q�[�v�̍폜
 *          friend��gfl::heap::Manager����̂݌Ăяo��
 *
 *  @param  none
 */
//-----------------------------------------------------------------------------
void GflHeapBaseLower::DeleteHeap(void)
{
  GFL_ASSERT_STOP_MSG(0,"GlfHeap HeapBaseLower is can't delete!!\n"); //@check �V�X�e���œ���ȍ������邽�߁B���[�U�[�̌Ăяo���֎~
}


//���ۂ̃������m�ہE�J������
void* GflHeapBaseLower::AllocAlign( const int size , const int align )
{
  return m_parentHeap->AllocAlign(size,(align>0?-align:align));
}


void GflHeapBaseLower::Free( void* buf )
{
  m_parentHeap->Free(buf);
}

//�����܂ŏ������z�֐�

#if GFL_HEAP_DEBUG
#if GFL_HIO_ENABLE
void GflHeapBaseLower::SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum, const int align )
{
  m_parentHeap->SetMemoryHeader_UserData(userHeader,filename,linenum,align);
}
#else
void GflHeapBaseLower::SetMemoryHeader_UserData( void *userHeader , const char *filename , const u16 linenum )
{
  m_parentHeap->SetMemoryHeader_UserData(userHeader,filename,linenum);
}
#endif

#else
//���[�U�[�w�b�_�[�Z�b�g(�f�t�H���g)
void GflHeapBaseLower::SetMemoryHeader_UserData( void *userHeader )
{
  m_parentHeap->SetMemoryHeader_UserData(userHeader);
}
#endif

} //namespace heap
} //namespace gfl

