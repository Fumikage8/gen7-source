//======================================================================
/**
 * @file	gfl_TaskSystem.cpp
 * @brief	�^�X�N�Ǘ��V�X�e��
 * @author	ariizumi
 * @data	10/12/03
 */
//======================================================================
#include <gflib.h>

namespace gfl
{
namespace base
{
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
TaskSystem::TaskSystem( gfl::heap::HeapBase* sysHeap , gfl::heap::HeapBase* userHeap , const u32 taskMax , const u32 workSize )
:mTaskMax(taskMax)
,mWorkSize(workSize)
,mHeap(userHeap)
,mAddFlag(false)
,mSuicideFlag(false)
,mNowChain(NULL)
,mStackTop(NULL)
{
  Task *prev = NULL;
  Task *now;

  //4�o�C�g���E�ɓK������悤�ɐ��K������
  mWorkSize = math::RoundUp(mWorkSize,4);
  mTcbArray = GflHeapAllocMemory( sysHeap , (sizeof(Task)+mWorkSize)*mTaskMax );
  
  for (int i = 0; i < mTaskMax; i++)
  {
    now = GetTaskPtr(i);
    now->Clear(this,&mTtcbFirst);
    PushTask(now);
  }
  
  mTtcbFirst.Clear(this,&mTtcbFirst);
  mNowChain = NULL;
}

//------------------------------------------------------------------
/**
 *  @brief	�^�X�N�V�X�e���I��
 */
//------------------------------------------------------------------
TaskSystem::~TaskSystem(void)
{
  //���s����ׂ��^�X�N���c���Ă�����G���[�ɂ���
  if(mTtcbFirst.mNext != &(mTtcbFirst)) 
  {
    GFL_PRINT("Error:Task Left. function = %08X\n",mTtcbFirst.mNext->mFunc);
    GFL_ASSERT(0);
  }
  GflHeapFreeMemory(mTcbArray);
}


//------------------------------------------------------------------
/**
 *	@brief	�^�X�N�X�V
 *
 *	�^�X�N�V�X�e���ɓo�^����Ă���e�^�X�N��D�揇�ɌĂяo��
 */
//------------------------------------------------------------------
void TaskSystem::Update(void)
{
  Task *now;
  Task *head = &(mTtcbFirst);

  mNowChain = head->mNext;
  while (mNowChain != head)
  {
    mSuicideFlag = false;
    mNowChain->mFunc(mNowChain, mNowChain->mWork);
    if (mSuicideFlag == true)
    {
      //���E�����ꍇ
      Task *next = mNowChain->mNext;
      DeleteTaskFunc(mNowChain);
      mNowChain = next;
    }
    else
     {
      mNowChain = mNowChain->mNext;
    }
  }
  mNowChain = NULL;
}

//------------------------------------------------------------------
/**
 * @brief �^�X�N�V�X�e����ō������Ă���^�X�N��S�ĎE��
 */
//------------------------------------------------------------------
void TaskSystem::DeleteAll()
{
  Task *now;
  Task *head = &(mTtcbFirst);

  mNowChain = head->mNext;
  while (mNowChain != head) 
  {
    Task *next = mNowChain->mNext;
    DeleteTaskFunc(mNowChain);
    mNowChain = next;
  }
  mNowChain = NULL;
}

//------------------------------------------------------------------------------
/**
	@brief	�^�X�N��ǉ�����
	@param	func      TCB_FUNC:�֘A�t������s�֐��|�C���^
	@param	workSize  �g�p���郏�[�N�̃T�C�Y
	@param	pri       u32:�^�X�N�v���C�I���e�B

	@return	Task *	�ǉ������^�X�N�������|�C���^

	�K�v�ȃ��[�N�͓����Ŏ����I�Ɋm�ۂ����(0�N���A�ς�)
*/
//------------------------------------------------------------------------------
Task* TaskSystem::Create( GFL_TASKFUNC * func, u32 workSize, u32 pri)
{
  Task* get;
  Task* find;
  Task* head = &(mTtcbFirst);

  get = PopTask();
  if (get == NULL)
  {
    //TCB������Ȃ��Ȃ����ꍇ
    return NULL;
  }
  get->mPri = pri;
  get->mFunc = func;
  if (workSize <= mWorkSize)
  {
    get->mWork = reinterpret_cast<void*>( reinterpret_cast<uptr>(get) + sizeof(Task));
    std::MemClear(get->mWork,workSize);
    get->mIsCreateBuff = false;
  }
  else 
  {
    get->mWork = GflHeapAllocMemory(mHeap, workSize);
    get->mIsCreateBuff = true;
  }

  //���s���X�g�ɐڑ�(�����v���C�I���e�B�̎��ɂ́A��ɓo�^�����ق�����)
  for (find = head->mNext; find != head; find = find->mNext)
  {
    if (find->mPri > get->mPri)
    {
      get->mPrev = find->mPrev;
      get->mNext = find;
      find->mPrev->mNext = get;
      find->mPrev = get;
      return get;
    }
  }
  //�Ō�܂Ōq���Ƃ��낪������Ȃ���΍Ō�Ɍq��
  get->mPrev = mTtcbFirst.mPrev; //���܂ł̍Ō�
  get->mNext = &(mTtcbFirst); //�������Ō�
  mTtcbFirst.mPrev->mNext = get; //���܂ōŌゾ�����u���b�N�̎��͎���
  mTtcbFirst.mPrev = get;   //���X�g�̍Ō���X�V
  return get;
}

//------------------------------------------------------------------------------
/**
	@brief	�^�X�N����������
	@param	TCBL		�^�X�N�|�C���^

	�ێ����Ă��郏�[�N�̊J�������������ɍs����
*/
//------------------------------------------------------------------------------
void TaskSystem::Delete( Task* task )
{
  GFL_ASSERT(task->mFunc != NULL);
  if (mNowChain == task)
  {
    //���E�̏ꍇ�̓����N�̂Ȃ��ւ����V�X�e���ɔC����
    mSuicideFlag = true;
    return;
  }
  else
  {
    //���E�̏ꍇ�͂��̏�ŏ���
    DeleteTaskFunc(task);
  }  
}


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
void TaskSystem::ChangePriority( Task* task, u32 pri)
{
  Task *find;
  Task *head = &(mTtcbFirst);

  //TCB���s���ɌĂяo���ꂽ�ꍇ�AASSERT�Œ�~������
  GFL_ASSERT_MSG(mNowChain == NULL, "TCB���쒆��priority�𑀍�\n");

  if (task->mPri > pri)
  {
    find = task->mPrev;
    while (find != head && find->mPri > pri) {
      find = find->mPrev;
    }
    //�Ώۂ����X�g����͂���
    task->mPrev->mNext = task->mNext;
    task->mNext->mPrev = task->mPrev;
    
    task->mNext = find->mNext;
    task->mNext->mPrev = task;
    task->mPrev = find;
    find->mNext = task;

  }
  else
  {

    find = task->mNext;
    while (find != head && find->mPri < pri)
    {
      find = find->mNext;
    }
    //�Ώۂ����X�g����͂���
    task->mPrev->mNext = task->mNext;
    task->mNext->mPrev = task->mPrev;

    task->mPrev = find->mPrev;
    task->mPrev->mNext = task;
    task->mNext = find;
    find->mPrev = task;
  }

  task->mPri = pri;
}
  
//------------------------------------------------------------------
/**
 * @brief Task�̃|�C���^�擾
 */
//------------------------------------------------------------------
Task* TaskSystem::GetTaskPtr( const u32 offset )
{
  return reinterpret_cast<Task*>( reinterpret_cast<uptr>(mTcbArray) + offset * (sizeof(Task) + mWorkSize) );
}
//------------------------------------------------------------------
/**
 * @brief ���g�p�^�X�N���X�g����^�X�N���擾
 */
//------------------------------------------------------------------
Task* TaskSystem::PopTask(void)
{
  Task *task;
  task = mStackTop;
  if (mStackTop == NULL) {
    //Pop�ł��Ȃ�
    return NULL;
  }
  mStackTop = task->mNext;
  if (task->mNext)
  {
    //�擾�^�X�N�����g�p���X�g�̍Ō�̗v�f�łȂ��ꍇ�́A
    //�V�����擪�ɂȂ����^�X�N�̑O��NULL�ɐݒ肷��
    task->mNext->mPrev = NULL;
  }

  return task;
}

//------------------------------------------------------------------
/**
 * @brief ���g�p�^�X�N���X�g�ւ̃^�X�N�̓o�^
 */
//------------------------------------------------------------------
void TaskSystem::PushTask(Task* task)
{
  Task *taskTop;
  taskTop = mStackTop;
  task->mPrev = NULL;
  task->mNext = taskTop;
  if (taskTop)
  {
    taskTop->mPrev = task;
  }
  mStackTop = task;
#ifdef TCBL_DEBUG 
  GFL_PRINT("top:%08x\n", mStackTop);
#endif
}

void TaskSystem::DeleteTaskFunc( Task *task )
{
  task->Delete();
  PushTask( task );
}

//���g�̍폜
void Task::DeleteSelf(void)
{
  mTaskSys->Delete(this);
}


void  Task::Clear( TaskSystem *taskSys , Task *dummyTask )
{
  mTaskSys = taskSys;
  mPrev = dummyTask;
  mNext = dummyTask;
  mPri = 0;
  mWork = NULL;
  mFunc = NULL;
  mIsCreateBuff = false;
}

void  Task::Delete(void)
{
  mFunc = NULL;
  if( mIsCreateBuff == true )
  {
    GflHeapFreeMemory( mWork );
  }
  mPrev->mNext = mNext;
  mNext->mPrev = mPrev;
 
}

}//namespace base
}//namespace gfl

