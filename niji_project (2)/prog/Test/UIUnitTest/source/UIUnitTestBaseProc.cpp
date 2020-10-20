#if PM_DEBUG

#include "Test/UiUnitTest/include/UIUnitTestBaseProc.h"

#include <debug/include/gfl2_DebugPrint.h>

#include <GameSys/include/GameProcManager.h>

#include "System/include/nijiAllocator.h"
#include <System/include/HeapDefine.h>

GFL_NAMESPACE_BEGIN(Test)

UIUnitTestBaseProc::UIUnitTestBaseProc(void)
{
  m_bInit = false;
  m_bEnd  = false;
  mpAppHeap     = NULL;
  mpManagerHeap = NULL;
}

UIUnitTestBaseProc::~UIUnitTestBaseProc(void)
{
  GFL_SAFE_DELETE( mpAppHeap );

  gfl2::heap::Manager::DeleteHeap( mpManagerHeap );
}

//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UIUnitTestBaseProc::InitFunc(gfl2::proc::Manager* pManager)
{
  if( m_bInit == true )
  {
    return gfl2::proc::RES_FINISH;
  }

  s32 seq = GameProc::GetSubSeq();

  if( seq == 0 )
  {
    //HEAPID_APP  本体に関するものはAPPめもりーから確保します
    gfl2::heap::HeapBase *appHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
    gfl2::heap::HeapBase *devHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );

    // このマネージャーに必要なメモリを作成  これはヒープが穴になるのでここから
    mpManagerHeap = GFL_CREATE_LOCAL_HEAP( appHeap, 4096, gfl2::heap::HEAP_TYPE_EXP, false );

    // このアプリ全体に必要なメモリを作成
    mpAppHeap = GFL_NEW(mpManagerHeap) app::util::Heap();

    // ローカルヒープを作る
    mpAppHeap->LocalHeapCreate(appHeap,devHeap, 1024 * 1024, 1024 * 1024 * 34 );

    //マネージャーを作成
    //並列の場合には２本作成
    mpFrameManager = GFL_NEW(mpManagerHeap) applib::frame::Manager(mpManagerHeap);

    //自分のパイプラインを作成
    mpAppRenderingManager  = GFL_NEW(mpAppHeap->GetSystemHeap()) app::util::AppRenderingManager();

    {
      m_pNijiAllocatorForEdgeMapTexture = GFL_NEW( mpAppHeap->GetDeviceHeap() ) System::nijiAllocator( mpAppHeap->GetDeviceHeap() );

      app::util::AppRenderingManager::Description appRenderingManagerDesc;

      //  下画面はレイアウトタイプ設定(派生先で内容を変えることができる)
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = _OnGetDispLowAppRenderOrderType();

      //  上画面はレイアウトタイプ設定(派生先で内容を変えることができる)
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = _OnGetDispUppAppRenderOrderType();

      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN].renderOrder  = _OnGetDispOffScreenAppRenderOrderType();

      mpAppRenderingManager->SyncCreate(
          appRenderingManagerDesc,
          mpAppHeap,
          m_pNijiAllocatorForEdgeMapTexture,
          GameSys::GameProc::GetGameManager()->GetAsyncFileManager()
      );
    }

    //起動フレーム作成
    {
      mpMenuCellFrame = OnCreateFrame( mpFrameManager, GFL_SINGLETON_INSTANCE( GameSys::GameManager ) );
    }

    ++seq;
  }

  if( seq == 1 )
  {
    if( mpAppRenderingManager->IsCreated() == true )
    {
      ++seq;
    }
  }

  if( seq == 2 )
  {
    BaseProcess::SetSubSeq( 0 );
    m_bInit = true;
  }

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief	  PROCのメイン更新を行う関数
  * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UIUnitTestBaseProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );
  
  mpAppRenderingManager->Update();

  if(mpFrameManager->Main()==applib::frame::MAINSTATUS_NULL){

    return gfl2::proc::RES_FINISH;
	}

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void UIUnitTestBaseProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  GFL_UNUSED( pManager );

  mpAppRenderingManager->PreDraw();
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void UIUnitTestBaseProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  GFL_UNUSED( pManager );

  mpFrameManager->Draw( displayNo );
  mpAppRenderingManager->Draw( displayNo );
}

//------------------------------------------------------------------
/**
  * @brief   PROCのメインを行う関数
  *  1/30フレームの②フレーム目の処理です。
  *  1/60フレーム動作時には、使われません。
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result UIUnitTestBaseProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );
  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result UIUnitTestBaseProc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_UNUSED( pManager );

  if( m_bEnd == true )
  {
    return gfl2::proc::RES_FINISH;
  }

  // 終了処理が完了するまで待つ
  if( mpFrameManager->End() == true )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  if( mpAppRenderingManager->CanDestroy() == false )
  {
    return gfl2::proc::RES_CONTINUE;
  }

  GFL_SAFE_DELETE( mpFrameManager );

  GFL_SAFE_DELETE( mpAppRenderingManager );
  GFL_SAFE_DELETE( m_pNijiAllocatorForEdgeMapTexture );

  m_bEnd  = true;

  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCのイベント起動検知処理
  *
  *  以下のタイミングで行うべき、イベント起動処理
  *  　GameProc->EventCheckFunc
  *  　EventManager->Main
  *  　GameProc->Main
  */
//------------------------------------------------------------------
void UIUnitTestBaseProc::EventCheckFunc(GameSys::GameManager* pManager)
{
  GFL_UNUSED( pManager );
}

GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
