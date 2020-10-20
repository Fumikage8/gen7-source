//=============================================================================================
/**
 * @file   MenuTestProc.cpp
 * @brief  呼び出しのProcから   ここは作ってもらう
 * @author k.ohno
 * @date   2015.2.16
 */
//=============================================================================================
#if PM_DEBUG

#include "Test/MenuFrameTest/include/MenuTestProc.h"
#include "System/include/HeapDefine.h"

#include "MenuSampleFrame.h"
#include <system/include/GflUse.h>



GFL_NAMESPACE_BEGIN( Test )
GFL_NAMESPACE_BEGIN( MenuFrameTest )



Proc::Proc(void)  //コンストラクタ
{
}


Proc::~Proc(void)  //デストラクタ
{
}




//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::InitFunc(gfl2::proc::Manager* pManager)
{
  
  //HEAPID_APP  本体に関するものはAPPめもりーから確保します
  gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
	gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

  // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
  mpManagerHeap = GFL_CREATE_LOCAL_HEAP( devHeap, SIZE_PROC_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false );

  // このアプリ全体に必要なメモリを作成
  mpAppHeap = GFL_NEW(mpManagerHeap) app::util::Heap();

  // ローカルヒープを作る
	mpAppHeap->LocalHeapCreate(appHeap,devHeap,SIZE_MY_APP,SIZE_MY_APPDEV);

	//マネージャーを作成
  //並列の場合には２本作成
  mpFrameManager = GFL_NEW(mpManagerHeap) applib::frame::Manager(mpManagerHeap);

  // リスナー設定
  mpFrameManager->SetAppFrameListener(this);
  
  //自分のパイプラインを作成
//  mpRenderingPipeLine  = GFL_NEW(mpAppHeap->GetSystemHeap()) MyRenderingPipeLine( 512, 256 );
  mpAppRenderingManager  = GFL_NEW(mpAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();

  app::util::AppRenderingManager::Description desc;
  {
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
    desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
  }

	mpAppRenderingManager->StartAsyncCreate( desc, mpAppHeap, NULL, mpAsyncFileManager );


//  mpAppRenderingManager->Init(mpAppHeap);

  //起動フレーム作成
//  mpMenuSampleFrame = MENU_FRAME_CALL_MODAL<applib::frame::Manager, MenuSampleFrame>( mpFrameManager );

//  mpMenuSampleFrame->Setup( mpAppHeap,  mpAppRenderingManager, mpAppParam);

  return gfl2::proc::RES_FINISH;

}




//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::UpdateFunc(gfl2::proc::Manager* pManager)
{
   mpAppRenderingManager->Update();
  if(!mpAppRenderingManager->IsCreated()){
    return gfl2::proc::RES_CONTINUE;
  }
 
  if(mpFrameManager->Main()==applib::frame::MAINSTATUS_NULL){

    return gfl2::proc::RES_FINISH;

	}
  return gfl2::proc::RES_CONTINUE;

}

//-----------------------------------------------------------------------------
/**
 * @func    PreDrawFunc
 * @brief   Proc描画下準備
 * @date    2015.07.09
 *
 * @param   pManager    プロセス管理マネージャー
 */
//-----------------------------------------------------------------------------
void Proc::PreDrawFunc( gfl2::proc::Manager * pManager )
{
  mpAppRenderingManager->PreDraw();
}

//------------------------------------------------------------------
/**
 * @brief	  PROCのメイン描画を行う関数
 *          純粋仮想ではありません。 順番を変更しました
 */
//------------------------------------------------------------------
void Proc::DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo no)
{
  mpFrameManager->Draw(no);
  mpAppRenderingManager->Draw(no);

}


//------------------------------------------------------------------
/**
 * @brief   PROCのメインを行う関数
 *  1/30フレームの②フレーム目の処理です。
 *  1/60フレーム動作時には、使われません。
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}


//------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
gfl2::proc::Result Proc::EndFunc(gfl2::proc::Manager* pManager)
{
  // 終了処理が完了するまで待つ
  if( mpFrameManager->End() == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  if( !mpAppRenderingManager->CanDestroy() )
  {
    return gfl2::proc::RES_CONTINUE;
  }
  GFL_DELETE mpFrameManager;
  GFL_DELETE mpAppRenderingManager;
  GFL_DELETE mpAppHeap;
  gfl2::heap::Manager::DeleteHeap( mpManagerHeap );
  return gfl2::proc::RES_FINISH;
}


//------------------------------------------------------------------
/**
 * @brief	  EndFuncが完了した直後に呼び出される関数
 */
//------------------------------------------------------------------
void Proc::EndFuncFinish( applib::frame::CellSuper* pEndCell )
{
  GFL_PRINT("EndFuncFinish呼び出し\n");
}



GFL_NAMESPACE_END( MenuFrameTest )
GFL_NAMESPACE_END( Test )

#endif // PM_DEBUG
