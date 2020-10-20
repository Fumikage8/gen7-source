#ifndef __TASKSYSTEM_H__
#define __TASKSYSTEM_H__
//======================================================================
/**
 * @file	TaskSystem.h
 * @brief	タスク管理システム(gflにあったがバトルでしか使ってなかったので個別に移植)
 * @author	ariizumi
 * @data	15/04/23
 */
//======================================================================

#pragma once

#include <macro/include/gfl2_macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_math.h>

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class TaskSystem;
class Task;

//タスクプライオリティ
static const u32 TASK_PRI_HIGHEST = 8;
static const u32 TASK_PRI_HIGH    = 16;
static const u32 TASK_PRI_DEFAULT = 32;
static const u32 TASK_PRI_LOW     = 40;
static const u32 TASK_PRI_LOWEST  = 48; //自動破棄系


//------------------------------------------------------------------
/**
 * @brief	タスク動作関数型定義
 *
 * TaskSystemに登録する実行関数の型定義。
 * 引数としてTask*と、ワークへのポインタをとるようになっている
 */
//------------------------------------------------------------------
typedef void (TASKFUNC)(Task *task, void *userWork,int step);

class Task
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(Task);

  friend class TaskSystem;
public:
  //------------------------------------------------------------------
  /**
   *  @brief	自身の削除
   */
  //------------------------------------------------------------------
  void DeleteSelf(void);

  //------------------------------------------------------------------
  /**
   *  @brief	ワークの取得
   */
  //------------------------------------------------------------------
  inline void* GetWork(void)const{return mWork;}

  //------------------------------------------------------------------
  /**
   *  @brief	タスク関数の切り替え
   *
   * @param func  新しいタスク関数
   */
  //------------------------------------------------------------------
  inline void SetFunc(TASKFUNC func){mFunc = func;}

  //------------------------------------------------------------------
  /**
   *  @brief	プライオリティの取得
   *          プライオリティの変更はTaskSystemから行ってください
   */
  //------------------------------------------------------------------
  inline u32 GetPriority(void)const{return mPri;}

  //------------------------------------------------------------------
  /**
   *  @brief	TaskSystemの取得
   */
  //------------------------------------------------------------------
  inline TaskSystem* GetSystem(void)const{return mTaskSys;}

  void Update(int step=1);
  
private:
  Task(){};
  ~Task(){};

  void  Clear( TaskSystem *taskSys , Task *dummyTask );
  void  Delete(void);

  TaskSystem *mTaskSys;   ///<所属するタスクシステムへのポインタ
  Task *mPrev;            ///<前のタスクへのポインタ
  Task *mNext;            ///<次のタスクへのポインタ
  u32 mPri;               ///<タスクのプライオリティ指定
  void *mWork;            ///<タスクワークへのポインタ（NULLのとき、確保済み領域を使用）
  TASKFUNC *mFunc;        ///<タスク関数へのポインタ
  bool  mIsCreateBuff;
};

class TaskSystem
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(TaskSystem);
public:
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
  TaskSystem( gfl2::heap::HeapBase* sysHeap , gfl2::heap::HeapBase* userHeap , const u32 taskMax , const u32 workSize );

  //------------------------------------------------------------------
  /**
   *  @brief	タスクシステム終了
   */
  //------------------------------------------------------------------
  ~TaskSystem(void);


  //------------------------------------------------------------------
  /**
   *	@brief	タスク更新
   *
   *	タスクシステムに登録されている各タスクを優先順に呼び出す
   *
   */
  //------------------------------------------------------------------
  void Update(int step=1);

  //------------------------------------------------------------------
  /**
   * @brief タスクシステム上で今生きているタスクを全て殺す
   */
  //------------------------------------------------------------------
  void DeleteAll();

  //------------------------------------------------------------------------------
  /**
  	@brief	タスクを追加する
  	@param	func      GFL_TASKFUNC:関連付ける実行関数ポインタ
  	@param	workSize  使用するワークのサイズ
  	@param	pri       u32:タスクプライオリティ

  	@return	Task *	追加したタスクを示すポインタ

  	必要なワークは内部で自動的に確保される(0クリア済み)
  */
  //------------------------------------------------------------------------------
  Task* Create( TASKFUNC * func, u32 workSize, u32 pri);

  //------------------------------------------------------------------------------
  /**
  	@brief	タスクを消去する
  	@param	task		タスクポインタ

  	保持しているワークの開放処理も同時に行われる
  */
  //------------------------------------------------------------------------------
  void Delete( Task* task );

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
  void ChangePriority( Task* task, u32 pri);

  int GetActiveTaskNum(void){return mTaskNum;}
  
  // Sync Mode:
  bool IsSyncMode(void) { return(isSyncMode); }
  void SetSyncMode(bool b) { isSyncMode = b; }

  // Last Created Task:
  Task* GetLastCreatedTask(void) { return(mLastCreatedTask); }

private:
  
  //Taskのポインタ取得
  Task* GetTaskPtr( const u32 offset );
  //未使用タスクリストからタスクを取得
  Task* PopTask(void);
  //未使用タスクリストへのタスクの登録
  void PushTask(Task* task);
  //タスクの削除
  void DeleteTaskFunc( Task *task );

  u32 mTaskMax;        ///<保持できるタスクの最大数
  u32 mWorkSize;        ///<標準で確保しているワークのサイズ
  gfl2::heap::HeapBase* mHeap;       ///<使用するヒープ
  bool mAddFlag;        ///<
  bool mSuicideFlag;     ///<自殺したかどうかのフラグ
  void *mTcbArray;      ///<タスク用に保持したメモリへのポインタ
  Task mTtcbFirst;     ///<双方向リスト先頭になるダミータスク
  Task *mNowChain;   ///<現在実行中のタスクへのポインタ
  Task *mStackTop;   ///<非使用状態のタスクリストへのポインタ
  int  mTaskNum;
  bool isSyncMode;
  Task *mLastCreatedTask;

};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)
#endif // __TASKSYSTEM_H__
