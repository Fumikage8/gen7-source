//================================================================================
/**
 * @file   DllReadThread.cpp
 * @brief  DLL呼び出しスレッド
 * @author k.ohno
 * @date   2013.2.27

  2015.12.13 tamada バトル専用だったのを汎用化してnijiに移植開始
 */
//================================================================================

#include <ro/include/gfl2_RoManager.h>

#include <GameSys/include/DllReadThread.h>
#include <proc/include/gfl2_Proc.h>
#include <GameSys/include/GameProc.h>
#include <thread/include/gfl2_Thread.h>

GFL_NAMESPACE_BEGIN(GameSys)



//--------------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 * @param heap        スレッドのスタック領域に使用するヒープ
 * @param stack_size  スレッドのスタック領域のサイズ
 */
//--------------------------------------------------------------------------------
DllReadThread::DllReadThread( gfl2::heap::HeapBase* heap, u32 stack_size )
  : m_pHeap( NULL )
{
  m_pThread = GFL_NEW( heap ) gfl2::thread::Thread( this, heap, true, stack_size );

  for( int i = 0; i < _MAX; ++i)
  {
    mRoArr[i] = NULL;
    mCroFileNameArr[i] = NULL;
  }
  mRoNum = 0;
}

//--------------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//--------------------------------------------------------------------------------
DllReadThread::~DllReadThread()
{
  GFL_DELETE m_pThread;
}

//--------------------------------------------------------------------------------
/**
 * @brief DLL読み込みに必要なデータ設定
 * @param croFileNameArr[] 読み込む.croファイル名
 * @param num 個数
 * @param heap DLL読み込みに使用するヒープ
 * @note heapを設定しない場合はRoManagerのヒープを使用する
 */
//--------------------------------------------------------------------------------
void DllReadThread::SetData( const char* croFileNameArr[],int num,gfl2::heap::HeapBase* heap )
{
  GFL_ASSERT(num < _MAX);
  mRoNum = num;

  for(int i=0;i<num;i++){
    mCroFileNameArr[i] = croFileNameArr[i];
  }

  m_pHeap = heap;
}

void DllReadThread::Function( void )
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  for( int i=0;i<mRoNum;i++ ) {
    mRoArr[i] = roMgr->LoadModule(GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager(), mCroFileNameArr[i], m_pHeap);
    m_pThread->CurrentYield();
  }

  for( int i=0;i<mRoNum;i++ ){
    roMgr->StartModule(mRoArr[i]);

    m_pThread->CurrentYield();
  }

  m_pThread->Kill();
//  m_pThread->Wait();
}


void DllReadThread::Link( GameProc* proc , gfl2::heap::HeapBase* pTmpHeap)
{
  proc->CreateChildModulePtrBuffer( pTmpHeap , mRoNum );
  for( int i=0;i< mRoNum;i++ ){
    proc->SetChildModulePtr(i,mRoArr[i]);
  }
}

//--------------------------------------------------------------------------------
/**
 * @brief DLL破棄
 */
//--------------------------------------------------------------------------------
void DllReadThread::DisposeModule()
{
  for( int i=0;i<mRoNum;i++ ) {
    this->DisposeModule( i );
  }
}

void DllReadThread::DisposeModule(int index)
{
  gfl2::ro::RoManager *roMgr = GFL_SINGLETON_INSTANCE( gfl2::ro::RoManager );
  if( mRoArr[index] )
  {
    roMgr->DisposeModule( mRoArr[index] );
    mRoArr[index] = NULL;
  }
}

GFL_NAMESPACE_END(GameSys)
