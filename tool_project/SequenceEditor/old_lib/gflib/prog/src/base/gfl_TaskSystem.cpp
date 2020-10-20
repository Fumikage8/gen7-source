//======================================================================
/**
 * @file	gfl_TaskSystem.cpp
 * @brief	タスク管理システム
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
 * コンストラクタ
 *
 * @param sysHeap  タスクシステムに使用するヒープの指定
 * @param userHeap タスクワークに使用するヒープの指定
 * @param taskMax  稼働できる最大タスク数
 * @param workSize 各タスクが保持するワークのサイズ
 *
 * workSizeで指定したワークｘ最大タスク数分のワークエリアはsysHeapから取得する。
 * タスク生成時にworkSizeを超えるサイズのワークを必要とするとき、自動的にuserHeap
 * で指定するヒープからワークをallocする。
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

  //4バイト境界に適合するように正規化する
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
 *  @brief	タスクシステム終了
 */
//------------------------------------------------------------------
TaskSystem::~TaskSystem(void)
{
  //実行するべきタスクが残っていたらエラーにする
  if(mTtcbFirst.mNext != &(mTtcbFirst)) 
  {
    GFL_PRINT("Error:Task Left. function = %08X\n",mTtcbFirst.mNext->mFunc);
    GFL_ASSERT(0);
  }
  GflHeapFreeMemory(mTcbArray);
}


//------------------------------------------------------------------
/**
 *	@brief	タスク更新
 *
 *	タスクシステムに登録されている各タスクを優先順に呼び出す
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
      //自殺した場合
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
 * @brief タスクシステム上で今生きているタスクを全て殺す
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
	@brief	タスクを追加する
	@param	func      TCB_FUNC:関連付ける実行関数ポインタ
	@param	workSize  使用するワークのサイズ
	@param	pri       u32:タスクプライオリティ

	@return	Task *	追加したタスクを示すポインタ

	必要なワークは内部で自動的に確保される(0クリア済み)
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
    //TCBが足りなくなった場合
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

  //実行リストに接続(同じプライオリティの時には、先に登録したほうが先)
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
  //最後まで繋ぐところが見つからなければ最後に繋ぐ
  get->mPrev = mTtcbFirst.mPrev; //今までの最後
  get->mNext = &(mTtcbFirst); //自分が最後
  mTtcbFirst.mPrev->mNext = get; //今まで最後だったブロックの次は自分
  mTtcbFirst.mPrev = get;   //リストの最後を更新
  return get;
}

//------------------------------------------------------------------------------
/**
	@brief	タスクを消去する
	@param	TCBL		タスクポインタ

	保持しているワークの開放処理も同時に行われる
*/
//------------------------------------------------------------------------------
void TaskSystem::Delete( Task* task )
{
  GFL_ASSERT(task->mFunc != NULL);
  if (mNowChain == task)
  {
    //自殺の場合はリンクのつなぎ替えをシステムに任せる
    mSuicideFlag = true;
    return;
  }
  else
  {
    //他殺の場合はその場で処理
    DeleteTaskFunc(task);
  }  
}


//------------------------------------------------------------------
/**
 * @brief	タスクプライオリティの変更
 *
 * タスクの動作優先順位を取得する
 *
 * @param	task	タスクへのポインタ
 * @param	pri	変更後のプライオリティ
 */
//------------------------------------------------------------------
void TaskSystem::ChangePriority( Task* task, u32 pri)
{
  Task *find;
  Task *head = &(mTtcbFirst);

  //TCB実行中に呼び出された場合、ASSERTで停止させる
  GFL_ASSERT_MSG(mNowChain == NULL, "TCB動作中にpriorityを操作\n");

  if (task->mPri > pri)
  {
    find = task->mPrev;
    while (find != head && find->mPri > pri) {
      find = find->mPrev;
    }
    //対象をリストからはずす
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
    //対象をリストからはずす
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
 * @brief Taskのポインタ取得
 */
//------------------------------------------------------------------
Task* TaskSystem::GetTaskPtr( const u32 offset )
{
  return reinterpret_cast<Task*>( reinterpret_cast<uptr>(mTcbArray) + offset * (sizeof(Task) + mWorkSize) );
}
//------------------------------------------------------------------
/**
 * @brief 未使用タスクリストからタスクを取得
 */
//------------------------------------------------------------------
Task* TaskSystem::PopTask(void)
{
  Task *task;
  task = mStackTop;
  if (mStackTop == NULL) {
    //Popできない
    return NULL;
  }
  mStackTop = task->mNext;
  if (task->mNext)
  {
    //取得タスクが未使用リストの最後の要素でない場合は、
    //新しく先頭になったタスクの前をNULLに設定する
    task->mNext->mPrev = NULL;
  }

  return task;
}

//------------------------------------------------------------------
/**
 * @brief 未使用タスクリストへのタスクの登録
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

//自身の削除
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

