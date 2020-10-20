#ifndef __GFL_PROCBASEPROCESS_H__
#define __GFL_PROCBASEPROCESS_H__
//=============================================================================
/**
 * @file	 gfl_ProcBaseProcess.h
 * @brief	 �v���Z�X���N���X
 * @author  k.ohno
 * @date	 2010.10.18
 */
//=============================================================================
#pragma once

#include <nn.h>
#include <ro/gfl_RoManager.h>
#include <gfl_Macros.h>
#include <gfl_Heap.h>

namespace gfl {
namespace proc {

class Manager;


//------------------------------------------------------------------
/**
 * @brief	�v���Z�X����֐��̌���
 */
//------------------------------------------------------------------
typedef enum
{
  RES_CONTINUE,		///<����p����
  RES_FINISH,				///<����I��

  ENUM_FORCE_DWORD(Result)  ///< �傫�������낦��  ????_FORCE_DWORD
}Result;


typedef enum
{
  RES_FF,		///<����p����
  ENUM_FORCE_DWORD(Result2)  ///< �傫�������낦��
}Result2;



class BaseProcess
{
  GFL_FORBID_COPY_AND_ASSIGN(BaseProcess);

protected:
  int m_subseq;					      ///< �v���Z�X�֐��̓���V�[�P���X
  bool mbEnd;  //End�t���O

private:
  BaseProcess* parent;			///< ��ʃv���Z�X�ւ̃|�C���^

  #if GFL_ENABLE_RO
  nn::ro::Module*      m_pModule;         //!< ���I���W���[��(���g�j
  nn::ro::Module**     m_pChildModule;    //!< �q�̓��I���W���[��
  u32                  m_pChildModuleNum; //!< �q�̓��I���W���[����
  #endif

public:
  //------------------------------------------------------------------
  /**
   * @brief	  �R���X�g���N�^
   */
  //------------------------------------------------------------------
  BaseProcess(void) : m_subseq(0), mbEnd(false), parent(NULL)
  #if GFL_ENABLE_RO
  , m_pModule(NULL), m_pChildModule(NULL), m_pChildModuleNum(0)
  #endif
  {};

  //------------------------------------------------------------------
  /**
   * @brief   �f�X�g���N�^
   */
  //------------------------------------------------------------------
  virtual ~BaseProcess()
  {
    #if GFL_ENABLE_RO
    if( m_pChildModule != NULL )
    {
      GflHeapFreeMemory(m_pChildModule);
      m_pChildModule = NULL;
    }
    #endif
  };

  //------------------------------------------------------------------
  /**
   * @brief	  �v���Z�X�̉ߒ��i���݂̏�ԁj��Ԃ�
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline int GetSubSeq(void) const { return m_subseq; };
  //------------------------------------------------------------------
  /**
   * @brief	  �v���Z�X�̉ߒ��i���݂̏�ԁj���X�V����
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void SetSubSeq(int seq){  m_subseq = seq; };
  //------------------------------------------------------------------
  /**
   * @brief	  �v���Z�X�̉ߒ��i���݂̏�ԁj�����Z�b�g����
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void ResetSubSeq(void){  m_subseq = 0; };
  //------------------------------------------------------------------
  /**
   * @brief	  �v���Z�X�̉ߒ��i���݂̏�ԁj���P��ɐi�߂�
   * @return	ProcSeqENUM
   */
  //------------------------------------------------------------------
  inline void AddSubSeq(void){  m_subseq++; };


public:


  //------------------------------------------------------------------
  /**
   * @brief	  PROC�̏��������s���֐�
   * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����UpdateFunc�ɐi��
   */
  //------------------------------------------------------------------
  virtual Result InitFunc(Manager* pManager) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROC�̃��C���X�V���s���֐�
   * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤����EndFunc�ɐi��
   */
  //------------------------------------------------------------------
  virtual Result UpdateFunc(Manager* pManager) = 0;
  //------------------------------------------------------------------
  /**
   * @brief	  PROC�̃��C���`����s���֐�
   *          �������z�ł͂���܂���B
   */
  //------------------------------------------------------------------
  virtual void   DrawFunc(Manager* pManager){GFL_UNUSED(pManager);};
  //------------------------------------------------------------------
  /**
   * @brief	  PROC�̏I�����s���֐�
   * @return	Result ���삪�I������� RES_FINISH��Ԃ��Ă��炤���ŏI������Ɖ��߂���
   */
  //------------------------------------------------------------------
  virtual Result EndFunc(Manager* pManager) = 0;
  //---------------------------------------------------------------------------------
  /**
   * @brief   ����Proc�ɐ��䂪��������I������悤�Ɏw��
   */
  //---------------------------------------------------------------------------------
  void SetProcEnd(void){ mbEnd = true; };


  #if GFL_ENABLE_RO
public:
  //------------------------------------------------------------------
  /**
   * @brief   ���I���W���[���|�C���^�̓o�^
   */
  //------------------------------------------------------------------
  void SetModulePtr( nn::ro::Module* pModule ){ m_pModule = pModule; }

  //------------------------------------------------------------------
  /**
   * @brief   �o�^����Ă��铮�I���W���[���|�C���^�̎擾
   */
  //------------------------------------------------------------------
  nn::ro::Module* GetModulePtr( void ){ return m_pModule; }

public:

  //------------------------------------------------------------------
  /**
   * @brief   �q�ƂȂ铮�I���W���[���̃o�b�t�@�T�C�Y���擾
   */
  //------------------------------------------------------------------
  u32 GetChildModulePtrBufferNum( void ){ return m_pChildModuleNum; }

  //------------------------------------------------------------------
  /**
   * @brief   �q�ƂȂ铮�I���W���[�����i�[���邽�߂̃o�b�t�@���m��
   */
  //------------------------------------------------------------------
  void CreateChildModulePtrBuffer( gfl::heap::HeapBase* heap, u32 num )
  {
    if( m_pChildModule == NULL )
    {
      m_pChildModule = (nn::ro::Module**)GflHeapAllocMemory(heap, sizeof(nn::ro::Module*)*num );
      m_pChildModuleNum = num;
    }
  }

  //------------------------------------------------------------------
  /**
   * @brief   �q�ƂȂ铮�I���W���[���|�C���^�̓o�^
   */
  //------------------------------------------------------------------
  void SetChildModulePtr( u32 index, nn::ro::Module* pModule )
  {
    GFL_ASSERT_STOP( index < m_pChildModuleNum );  //@fix
    m_pChildModule[index] = pModule;
  }

  //------------------------------------------------------------------
  /**
   * @brief   �q���ēo�^����Ă��铮�I���W���[���|�C���^�̎擾
   */
  //------------------------------------------------------------------
  nn::ro::Module* GetChildModulePtr( u32 index ){ return m_pChildModule[index]; }



  #endif

};

}  //proc
} //gfl

#endif // __GFL_PROCBASEPROCESS_H__
