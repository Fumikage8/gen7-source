//======================================================================
/**
 * @file  BattleMeasure.cpp
 * @brief 15/03/13
 * @author  ariizumi
 * @data  バトル処理負荷測定
 */
//======================================================================
#if PM_DEBUG

#include <System/include/HeapDefine.h>

#include <Test/AriizumiTest/include/AriTestBtlv.h>
#include <Test/AriizumiTest/include/BattleMeasure.h>

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Ariizumi)

BattleMeasureProc::BattleMeasureProc()
{

}

BattleMeasureProc::~BattleMeasureProc(void)
{

}

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result BattleMeasureProc::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::heap::HeapBase *heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  gfl2::heap::HeapBase *debugHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_EVENT);

  //0x1C00000
  //mMemHeap = GFL_CREATE_LOCAL_HEAP_NAME(   heap, 0x400000,gfl2::heap::HEAP_TYPE_EXP,false,"BtlMeasureMem");
  //mDevHeap = GFL_CREATE_LOCAL_HEAP_NAME(devHeap,0x1b20000,gfl2::heap::HEAP_TYPE_EXP,false,"BtlMeasureDev");

  mBtlvSystem = GFL_NEW(heap)DebugBattleViewSystem();
  mBtlvSystem->Initialize( debugHeap,heap );

  mMainSeq = 0;

  return gfl2::proc::RES_FINISH;
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result BattleMeasureProc::EndFunc(gfl2::proc::Manager* pManager)
{
  mBtlvSystem->Finalize();
  GFL_SAFE_DELETE(mBtlvSystem);

  return gfl2::proc::RES_FINISH;
}


//------------------------------------------------------------------
/**
  * @brief   PROCのメインを行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result BattleMeasureProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  mBtlvSystem->Update();

  enum
  {
    SEQ_SETUP = 0,
    SEQ_SETUP_WAIT,
    SEQ_MAIN,
    SEQ_CLEANUP,
    SEQ_CLEANUP_WAIT,
    SEQ_END,
  };
  switch(mMainSeq)
  {
  case SEQ_SETUP:
    mBtlvSystem->CMD_StartSetup();
    mMainSeq++;
    //break;
  case SEQ_SETUP_WAIT:
   if( mBtlvSystem->CMD_WaitSetup() )
   {
     mMainSeq++;
   }
   break;
  case SEQ_MAIN:
    break;
  case SEQ_CLEANUP:
    mBtlvSystem->CMD_StartCleanUp();
    mMainSeq++;
    //break;
  case SEQ_CLEANUP_WAIT:
   if( mBtlvSystem->CMD_WaitCleanUp() )
   {
     mMainSeq++;
   }
   break;
  case SEQ_END:
    return gfl2::proc::RES_FINISH;
    //break;
  }
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void BattleMeasureProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  mBtlvSystem->PreDraw();
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void BattleMeasureProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  mBtlvSystem->Draw(displayNo);
}

GFL_NAMESPACE_END(Ariizumi)
GFL_NAMESPACE_END(Test)

#endif//PM_DEBUG