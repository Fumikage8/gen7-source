#ifndef __GFL_TASKSYSTEM_H__
#define __GFL_TASKSYSTEM_H__
//======================================================================
/**
 * @file	gfl_TaskSystem.h
 * @brief	�^�X�N�Ǘ��V�X�e��
 * @author	ariizumi
 * @data	10/12/03
 */
//======================================================================
//gflib��tcbl���ڐA

#pragma once

namespace gfl
{
namespace base
{

class TaskSystem;
class Task;

//------------------------------------------------------------------
/**
 * @brief	�^�X�N����֐��^��`
 *
 * TaskSystem�ɓo�^������s�֐��̌^��`�B
 * �����Ƃ���Task*�ƁA���[�N�ւ̃|�C���^���Ƃ�悤�ɂȂ��Ă���
 */
//------------------------------------------------------------------
typedef void (GFL_TASKFUNC)(Task *task, void *userWork);

class Task
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(Task);

  friend class TaskSystem;
public:
  //------------------------------------------------------------------
  /**
   *  @brief	���g�̍폜
   */
  //------------------------------------------------------------------
  void DeleteSelf(void);

  //------------------------------------------------------------------
  /**
   *  @brief	���[�N�̎擾
   */
  //------------------------------------------------------------------
  inline void* GetWork(void)const{return mWork;}

  //------------------------------------------------------------------
  /**
   *  @brief	�^�X�N�֐��̐؂�ւ�
   *
   * @param func  �V�����^�X�N�֐�
   */
  //------------------------------------------------------------------
  inline void SetFunc(GFL_TASKFUNC func){mFunc = func;}

  //------------------------------------------------------------------
  /**
   *  @brief	�v���C�I���e�B�̎擾
   *          �v���C�I���e�B�̕ύX��TaskSystem����s���Ă�������
   */
  //------------------------------------------------------------------
  inline u32 GetPriority(void)const{return mPri;}

  //------------------------------------------------------------------
  /**
   *  @brief	TaskSystem�̎擾
   */
  //------------------------------------------------------------------
  inline TaskSystem* GetSystem(void)const{return mTaskSys;}
  
private:
  Task(){};
  ~Task(){};

  void  Clear( TaskSystem *taskSys , Task *dummyTask );
  void  Delete(void);

  TaskSystem *mTaskSys;   ///<��������^�X�N�V�X�e���ւ̃|�C���^
  Task *mPrev;      ///<�O�̃^�X�N�ւ̃|�C���^
  Task *mNext;      ///<���̃^�X�N�ւ̃|�C���^
  u32 mPri;          ///<�^�X�N�̃v���C�I���e�B�w��
  void *mWork;        ///<�^�X�N���[�N�ւ̃|�C���^�iNULL�̂Ƃ��A�m�ۍςݗ̈���g�p�j
  GFL_TASKFUNC *mFunc;    ///<�^�X�N�֐��ւ̃|�C���^
  bool  mIsCreateBuff;
};

class TaskSystem
{
  // �R�s�[�R���X�g���N�^�{������Z�q�֎~
  GFL_FORBID_COPY_AND_ASSIGN(TaskSystem);
public:
  //------------------------------------------------------------------
  /**
   * �R���X�g���N�^
   *
   * @param sysHeap  �^�X�N�V�X�e���Ɏg�p����q�[�v�̎w��
   * @param userHeap �^�X�N���[�N�Ɏg�p����q�[�v�̎w��
   * @param taskMax  �ғ��ł���ő�^�X�N��
   * @param workSize �e�^�X�N���ێ����郏�[�N�̃T�C�Y
   *
   * workSize�Ŏw�肵�����[�N���ő�^�X�N�����̃��[�N�G���A��sysHeap����擾����B
   * �^�X�N��������workSize�𒴂���T�C�Y�̃��[�N��K�v�Ƃ���Ƃ��A�����I��userHeap
   * �Ŏw�肷��q�[�v���烏�[�N��alloc����B
   *
   */
  //------------------------------------------------------------------
  TaskSystem( gfl::heap::HeapBase* sysHeap , gfl::heap::HeapBase* userHeap , const u32 taskMax , const u32 workSize );

  //------------------------------------------------------------------
  /**
   *  @brief	�^�X�N�V�X�e���I��
   */
  //------------------------------------------------------------------
  ~TaskSystem(void);


  //------------------------------------------------------------------
  /**
   *	@brief	�^�X�N�X�V
   *
   *	�^�X�N�V�X�e���ɓo�^����Ă���e�^�X�N��D�揇�ɌĂяo��
   *
   */
  //------------------------------------------------------------------
  void Update(void);

  //------------------------------------------------------------------
  /**
   * @brief �^�X�N�V�X�e����ō������Ă���^�X�N��S�ĎE��
   */
  //------------------------------------------------------------------
  void DeleteAll();

  //------------------------------------------------------------------------------
  /**
  	@brief	�^�X�N��ǉ�����
  	@param	func      GFL_TASKFUNC:�֘A�t������s�֐��|�C���^
  	@param	workSize  �g�p���郏�[�N�̃T�C�Y
  	@param	pri       u32:�^�X�N�v���C�I���e�B

  	@return	Task *	�ǉ������^�X�N�������|�C���^

  	�K�v�ȃ��[�N�͓����Ŏ����I�Ɋm�ۂ����(0�N���A�ς�)
  */
  //------------------------------------------------------------------------------
  Task* Create( GFL_TASKFUNC * func, u32 workSize, u32 pri);

  //------------------------------------------------------------------------------
  /**
  	@brief	�^�X�N����������
  	@param	task		�^�X�N�|�C���^

  	�ێ����Ă��郏�[�N�̊J�������������ɍs����
  */
  //------------------------------------------------------------------------------
  void Delete( Task* task );

  //------------------------------------------------------------------
  /**
   * @brief	�^�X�N�v���C�I���e�B�̕ύX
   *
   * �^�X�N�̓���D�揇�ʂ��擾����
   *
   * @param	task	�^�X�N�ւ̃|�C���^
   * @param	pri	�ύX��̃v���C�I���e�B
   */
  //------------------------------------------------------------------
  void ChangePriority( Task* task, u32 pri);

private:
  
  //Task�̃|�C���^�擾
  Task* GetTaskPtr( const u32 offset );
  //���g�p�^�X�N���X�g����^�X�N���擾
  Task* PopTask(void);
  //���g�p�^�X�N���X�g�ւ̃^�X�N�̓o�^
  void PushTask(Task* task);
  //�^�X�N�̍폜
  void DeleteTaskFunc( Task *task );

  u32 mTaskMax;        ///<�ێ��ł���^�X�N�̍ő吔
  u32 mWorkSize;        ///<�W���Ŋm�ۂ��Ă��郏�[�N�̃T�C�Y
  gfl::heap::HeapBase* mHeap;       ///<�g�p����q�[�vID
  bool mAddFlag;        ///<
  bool mSuicideFlag;     ///<���E�������ǂ����̃t���O
  void *mTcbArray;      ///<�^�X�N�p�ɕێ������������ւ̃|�C���^
  Task mTtcbFirst;     ///<�o�������X�g�擪�ɂȂ�_�~�[�^�X�N
  Task *mNowChain;   ///<���ݎ��s���̃^�X�N�ւ̃|�C���^
  Task *mStackTop;   ///<��g�p��Ԃ̃^�X�N���X�g�ւ̃|�C���^

};

}//namespace base
}//namespace gfl
#endif // __GFL_TASKSYSTEM_H__
