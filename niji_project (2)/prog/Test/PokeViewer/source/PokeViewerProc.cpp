//======================================================================
/**
 * @file PokeViewerProc.cpp
 * @date 2015/11/24 12:36:25
 * @author araki_syo
 * @brief PokeViewerのProc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include "../include/PokeViewerProc.h"
#include "System/include/HeapDefine.h"

#include "../include/PokeViewerHeapDefine.h"

#include "./HIOInjection/HIOInjection.h"


GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(pokeviewer)

/**
 * @brief ctor
 */
PokeViewerProc::PokeViewerProc()
: m_Step(STEP_NONE),
  m_StepCounter(0),
  m_pDataManager(NULL),
  m_pFrameManager(NULL),
  m_pFrameManagerHeap(NULL),
  m_pFrameCell(NULL),
  m_frameCreated(false),
  m_frameReqDestroy(false),
  m_pAppRenderingManager(NULL),
  m_pPokeModelManager(NULL),
  /*  HIO  */
  m_pHIOInjection(NULL)
{
}

/**
 * @brief dtor
 */
PokeViewerProc::~PokeViewerProc()
{
}

/**
 * @brief   FrameModeの取得
 * @retval  FrameMode
 *
 * @detail フレームモードを切り替えるときは、戻り値を変えてください。
 */
GameSys::FrameMode PokeViewerProc::GetFrameMode( void ) const
{
  return GameSys::FRAMEMODE_DEFAULT;
}

/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
gfl2::proc::Result PokeViewerProc::InitFunc(gfl2::proc::Manager* pManager)
{
  // ヒープ初期化
  gfl2::heap::HeapBase* pParentHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  m_pHeap = GFL_CREATE_LOCAL_HEAP(pParentHeap, 0x1000000, gfl2::heap::HEAP_TYPE_EXP, false);

  m_pAppHeap = GFL_NEW(m_pHeap) app::util::Heap();
  m_pAppHeap->LocalHeapCreate(m_pHeap, m_pHeap, POKEVIEWER_MEM_IMPL_HEAP_MEM_SIZE, POKEVIEWER_MEM_IMPL_DEV_MEM_SIZE);

  ChangeStep(STEP_CREATE);

  return gfl2::proc::RES_FINISH;
}

/**
 * @brief   PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
gfl2::proc::Result PokeViewerProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  // データマネージャ
  if(m_pDataManager)
  {
    m_pDataManager->Update();
  }

  // フレーム
  applib::frame::MainStatus frameStatus = applib::frame::MAINSTATUS_NULL;
  if(m_pFrameManager)
  {
    frameStatus = m_pFrameManager->Main();
  }

  if(m_frameCreated && frameStatus == applib::frame::MAINSTATUS_NULL)
  {
    m_frameReqDestroy = true;
  }

  // HIO
  if(m_pHIOInjection) m_pHIOInjection->Update();

  // レンダリングマネージャ
  if(m_pAppRenderingManager)
  {
    m_pAppRenderingManager->Update();
  }

  // ポケモン描画
  if(m_pPokeModelManager)
  {
    m_pPokeModelManager->Update();
  }

  switch(m_Step)
  {
  case STEP_NONE:
    // なにもしない
    break;

  case STEP_CREATE:
    UpdateFunc_Create();
    break;

  case STEP_WORKING:
    UpdateFunc_Working();
    break;

  case STEP_DESTROY:
    UpdateFunc_Destroy();
    break;
  }

  if(m_Step == STEP_NONE)
  {
    result = gfl2::proc::RES_FINISH;
  }

  return result;
}

/**
 * @brief   PROCの描画処理(下準備)
 * @param   pManager   プロセス管理マネージャー
 * @detail  登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 *          例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
void PokeViewerProc::PreDrawFunc( gfl2::proc::Manager* pManager )
{
  if(m_Step == STEP_WORKING)
  {
    m_pAppRenderingManager->PreDraw();
  }
}

/**
 * @brief   PROCの描画処理
 * @param   pManager   プロセス管理マネージャー
 * @param   displayNo  描画対象画面
 * @detail  登録したPROCによる、フレームバッファへの描画を実行します。
 *          ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
void PokeViewerProc::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  if(m_Step == STEP_WORKING)
  {
    // フレームマネージャ
    m_pFrameManager->Draw(displayNo);

    // レンダリングマネージャ
    m_pAppRenderingManager->Draw(displayNo);

    // ポケモン描画
    m_pPokeModelManager->Draw(displayNo);
  }
}

/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
gfl2::proc::Result PokeViewerProc::EndFunc(gfl2::proc::Manager* pManager)
{
  GFL_SAFE_DELETE(m_pAppHeap);

  GFL_DELETE_HEAP(m_pHeap);
  m_pHeap = NULL;
  return gfl2::proc::RES_FINISH;
}

void PokeViewerProc::UpdateFunc_Create()
{
  switch(m_StepCounter)
  {
  case 0:
    {
      // データマネージャ初期化
      m_pDataManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) PokeViewerDataManager(m_pAppHeap, GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager());
      m_pDataManager->Start();
      ++m_StepCounter;
      // break; 
    }

  case 1:
    {
      // @todo pFrameManagerHeap解放
      // フレームマネージャ初期化
      m_pFrameManagerHeap = GFL_CREATE_LOCAL_HEAP(m_pAppHeap->GetDeviceHeap(), 0x10000, gfl2::heap::HEAP_TYPE_EXP, false);
      m_pFrameManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) applib::frame::Manager(m_pFrameManagerHeap);

      ++m_StepCounter;
      // break;
    }

  case 2:
    {
      // レンダリングマネージャ初期化
      m_pAppRenderingManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) app::util::AppRenderingManager();
      app::util::AppRenderingManager::Description desc;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      m_pAppRenderingManager->StartAsyncCreate(desc, m_pAppHeap, NULL, this->GetGameManager()->GetAsyncFileManager());

      ++m_StepCounter;
      // break;
    }

  case 3:
    {
      // レンダリングマネージャ初期化確認
      if(!m_pAppRenderingManager->IsCreated())
      {
        break;
      }

      ++m_StepCounter;
      // break;
    }

  case 4:
    {
      m_pPokeModelManager = GFL_NEW(m_pAppHeap->GetDeviceHeap()) PokeViewerPokeModelManager();
      m_pPokeModelManager->Initialize(GFL_SINGLETON_INSTANCE(GameSys::GameManager), m_pAppHeap->GetDeviceHeap());

      ++m_StepCounter;
      // break;
    }

  case 5:
    {
      // フレーム初期化
      m_pFrameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, PokeViewerFrameCell>(m_pFrameManager);
      PokeViewerFrameCell::Param param;
      param.pAppHeap = m_pAppHeap;
      param.pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
      param.pDataManager = m_pDataManager;
      param.pFrameManager = m_pFrameManager;
      param.pAppRenderingManager = m_pAppRenderingManager;
      param.pPokeModelManager = m_pPokeModelManager;

      m_pFrameCell->Setup(param);

      m_frameCreated = true;

      ++m_StepCounter;
      // break;
    }

  case 6:
    {
      /*  HIO  */
      gfl2::heap::HeapBase* pHeapBase = m_pAppHeap->GetDeviceHeap();
      
      m_pHIOInjection = GFL_NEW(pHeapBase) HIOInjection();
      m_pHIOInjection->Initialize(pHeapBase, m_pPokeModelManager);
      
      ++m_StepCounter;
      // break;
    }
    

  default:
    {
      ChangeStep(STEP_WORKING);
      break;
    }
  }
}

void PokeViewerProc::UpdateFunc_Working()
{
  if(m_frameReqDestroy)
  {
    ChangeStep(STEP_DESTROY);
  }
}

void PokeViewerProc::UpdateFunc_Destroy()
{
  switch(m_StepCounter)
  {
  case 0:
    {
      ++m_StepCounter;
      break;
    }
  case 1:
    {
      // レンダリングマネージャ解放
      if(m_pAppRenderingManager->CanDestroy())
      {
        GFL_SAFE_DELETE(m_pAppRenderingManager);
        ++m_StepCounter;
        // break;
      }
      else
      {
        break;
      }
    }
  case 2:
    {
      // フレーム解放
      if(!m_pFrameManager->End())
      {
        GFL_SAFE_DELETE(m_pFrameManager);
        GFL_DELETE_HEAP(m_pFrameManagerHeap);
        m_pFrameManagerHeap = NULL;

        ++m_StepCounter;
        // break;
      }
      else
      {
        break;
      }
    }
  case 3:
    {
      // データマネージャ停止
      m_pDataManager->Stop();
      ++m_StepCounter;
      // break;
    }
  case 4:
    {
      // データマネージャ破棄
      if(m_pDataManager->IsDisposable())
      {
        GFL_SAFE_DELETE(m_pDataManager);
        ++m_StepCounter;
        // break;
      }
      else
      {
        break;
      }
    }

  case 5:
    {
      if(m_pHIOInjection) m_pHIOInjection->SetModelManager(NULL);

      // ポケモン破棄
      m_pPokeModelManager->UnloadPokeModel();
      
      ++m_StepCounter;
      // break;
    }

  case 6:
    {
      if(!m_pPokeModelManager->IsDisposable())
      {
        break;
      }

      m_pPokeModelManager->Finalize();
      GFL_SAFE_DELETE(m_pPokeModelManager);

      ++m_StepCounter;
      // break;
    }

  case 7:
    {
      /*  HIO  */
      // Finalize()はデストラクタでしているため呼ばなくてもよい
      if(m_pHIOInjection) GFL_SAFE_DELETE(m_pHIOInjection);
      
      ++m_StepCounter;
      // break;
    }



  default:
    {
      ChangeStep(STEP_NONE);
      break;
    }
  }
}

void PokeViewerProc::ChangeStep(Step step)
{
  m_Step = step;
  m_StepCounter = 0;
}

  GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
