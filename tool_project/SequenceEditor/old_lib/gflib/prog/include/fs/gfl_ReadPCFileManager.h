#ifndef __GFL_READPCFILEMANAGER_H__
#define __GFL_READPCFILEMANAGER_H__
//===========================================================================
/**
 * @file    gfl_ReadPCFileManager.h
 * @brief   GAMEFREAK Library PC�t�@�C����ǂݍ��ނ��߂ɕK�v�ȏ������Ɖ���̃N���X
 * @author  k.ohno
 * @date    2010.11.18
 */
//===========================================================================
#pragma once

#if NN_SWITCH_ENABLE_HOST_IO

#include <nw/io/io_HioFileStream.h>
#include <gfl_Heap.h>

namespace gfl {
namespace fs {

class ReadPCFileManager
{
  GFL_FORBID_COPY_AND_ASSIGN(ReadPCFileManager);

private:
  static void* deviceMemory;
  
public:

  //----------------------------------------------------------------------
  /**
   * @brief   �R���X�g���N�^
   */
  //----------------------------------------------------------------------
  
  ReadPCFileManager(void){};

  //----------------------------------------------------------------------
  /**
   * @brief   HIO�̊J�n �f�o�C�X����������g�p���郁�����̊m�ۂ��s����
   * @param[in]   heapId  �f�o�C�X�������������Ă���ID�����Ȃ���΂����Ȃ�
   */
  //----------------------------------------------------------------------
  static void Initialize(const gfl::heap::HEAPID heapId);

  //----------------------------------------------------------------------
  /**
   * @brief   HIO�̊J�� �f�o�C�X�������̊J��
   */
  //----------------------------------------------------------------------
  static void Finalize(void);
};


}  //fs
}  //gfl

#endif //GFL_DEBUG
#endif // __GFL_READPCFILEMANAGER_H__
