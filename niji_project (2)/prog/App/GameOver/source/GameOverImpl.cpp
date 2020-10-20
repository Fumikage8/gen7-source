//==============================================================================
/**
 @file    GameOverImpl.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑の実装
 */
//==============================================================================


// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Frame/AppFrameManager.h>

// 図鑑のインクルード
#include <App/GameOver/include/GameOverProcParam.h>
#include "GameOverDefine.h"
#include "GameOverDataManager.h"
#include "GameOverFrame.h"

#include "GameOverImpl.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


GameOverImpl::GameOverImpl(GameOverProcParam* param, app::util::Heap* appHeap, GameSys::GameManager* gameManager)
  : m_param(param),
    m_appHeap(appHeap),
    m_gameManager(gameManager),
    m_step(STEP_NONE),
    m_stepCount(0),
    m_dataManager(NULL),
    m_frameManagerDevMem(NULL),
    m_frameManager(NULL),
    m_frameListener(NULL),
    m_frameCell(NULL),
    m_frameCreated(false),
    m_frameReqDestroy(false),
    m_appRenderingManager(NULL)
{}
GameOverImpl::~GameOverImpl()
{}

gfl2::proc::Result GameOverImpl::InitFunc(void)
{
  // Updateでつくることにする
  this->changeStep(STEP_CREATE);
  
  return gfl2::proc::RES_FINISH;
}

gfl2::proc::Result GameOverImpl::UpdateFunc(void)
{
  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;

  // データマネージャ
  if(m_dataManager) m_dataManager->Update();

  // フレーム
  applib::frame::MainStatus frameStatus = applib::frame::MAINSTATUS_NULL;
  if(m_frameManager) frameStatus = m_frameManager->Main();
  if(  m_frameCreated  &&  frameStatus == applib::frame::MAINSTATUS_NULL  ) m_frameReqDestroy = true;

  // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->Update();

  switch(m_step)
  {
  case STEP_NONE:    this->updateFunc_None();    break;
  case STEP_CREATE:  this->updateFunc_Create();  break;
  case STEP_WORKING: this->updateFunc_Working(); break;
  case STEP_DESTROY: this->updateFunc_Destroy(); break;
  }

  if(m_step == STEP_NONE) res = gfl2::proc::RES_FINISH;
  return res;
}
void GameOverImpl::PreDrawFunc(void)
{
  // レンダリングマネージャ
  if(m_step == STEP_WORKING) m_appRenderingManager->PreDraw();
}
void GameOverImpl::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_step == STEP_WORKING)
  {
    // フレーム
    m_frameManager->Draw(displayNo);
  
    // レンダリングマネージャ
    m_appRenderingManager->Draw(displayNo);
  }
}
gfl2::proc::Result GameOverImpl::EndFunc(void)
{
  return gfl2::proc::RES_FINISH;
}

void GameOverImpl::changeStep(Step step)
{
  m_step      = step;
  m_stepCount = 0;
}

void GameOverImpl::updateFunc_None(void)
{}
void GameOverImpl::updateFunc_Create(void)
{
  switch(m_stepCount)
  {
  case 0:
    {
      // データマネージャ
      m_dataManager = GFL_NEW(m_appHeap->GetDeviceHeap()) GameOverDataManager(m_appHeap, m_gameManager->GetAsyncFileManager());
      m_dataManager->Start();
      ++m_stepCount;
      //breakしない
    }
  case 1:
    {
      // フレーム
      
      // フレームマネージャ生成
      m_frameManagerDevMem = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), GAMEOVER_MEM_FRAME_MANAGER_DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
      m_frameManager = GFL_NEW(m_appHeap->GetDeviceHeap()) applib::frame::Manager(m_frameManagerDevMem);  // マネージャはヒープが荒れるので、専用のヒープを渡す。
      m_frameListener = GFL_NEW(m_appHeap->GetDeviceHeap()) GameOverFrameListener();
      m_frameManager->SetAppFrameListener(m_frameListener);
    
      ++m_stepCount;
      //breakしない
    }
  case 2:
    {
      // レンダリングマネージャ
      m_appRenderingManager = GFL_NEW(m_appHeap->GetDeviceHeap()) app::util::AppRenderingManager();
  
      app::util::AppRenderingManager::Description desc;
      //desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      m_appRenderingManager->StartAsyncCreate(
          desc,
          m_appHeap,
          NULL,
          m_gameManager->GetAsyncFileManager()
      );
      
      ++m_stepCount;
      //breakしない
    }
  case 3:
    {
      // レンダリングマネージャ
      if(m_appRenderingManager->IsCreated())
      {
        ++m_stepCount;
        //breakしない
      }
      else
      {
        break;
      }
    }

  case 4:
    {
      // フレーム

      // フレーム生成
      m_frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, GameOverFrameCell>(m_frameManager);
      GameOverFrameCell::Param param;
      param.param               = m_param;
      param.appHeap             = m_appHeap;
      param.gameManager         = m_gameManager;
      param.dataManager         = m_dataManager;
      param.frameManager        = m_frameManager;
      param.appRenderingManager = m_appRenderingManager;
      m_frameCell->Setup(param);

      m_frameCreated = true;

      ++m_stepCount;
      //breakしない
    } 
  case 5:
    {
      ++m_stepCount;
      //breakしない
    }
  case 6:
  default:
    {
      // 描画時は必ず行う更新
      this->updateForDraw();
      
      // 次へ
      this->changeStep(STEP_WORKING);
      break;
    }
  }
}
void GameOverImpl::updateFunc_Working(void)
{
  // 描画時は必ず行う更新
  this->updateForDraw();

  if(m_frameReqDestroy)
  {
    // 次へ
    this->changeStep(STEP_DESTROY);
  }
}
void GameOverImpl::updateFunc_Destroy(void)
{
  switch(m_stepCount)
  {
  case 0:
    {
      // 非表示にして、1フレ遅れ解放に対応。
      ++m_stepCount;
      break;
    }
  case 1:
    {
      // レンダリングマネージャ
      if(m_appRenderingManager->CanDestroy())
      {
        GFL_SAFE_DELETE(m_appRenderingManager);
        ++m_stepCount;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 2:
    {
      // フレーム
      
      // フレームマネージャ破棄
      if(m_frameManager->End() == false)
      {
        GFL_SAFE_DELETE(m_frameManager);

        // フレーム破棄
        GFL_SAFE_DELETE(m_frameListener);

        GFL_DELETE_HEAP(m_frameManagerDevMem);
        m_frameManagerDevMem = NULL;
       
        ++m_stepCount;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 3:
    {
      // データマネージャ
      m_dataManager->Stop();
      ++m_stepCount;
      //breakしない
    }
  case 4:
    {
      // データマネージャ
      if(m_dataManager->CanDestroy())
      {
        GFL_SAFE_DELETE(m_dataManager);
        ++m_stepCount;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 5:
  default:
    {
      // 次へ
      this->changeStep(STEP_NONE);
      break;
    }
  }
}
void GameOverImpl::updateForDraw(void)
{
  // 特にやることはない
}


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

