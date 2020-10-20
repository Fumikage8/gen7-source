//======================================================================
/**
 * @file CharaViewerImpl.cpp
 * @date 2015/07/21 21:28:38
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

// gfl2のインクルード
#include <debug/include/gfl2_DebugPrint.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Frame/AppFrameManager.h>

// 図鑑のインクルード
#include <Test/CharaViewer/include/CharaViewerProcParam.h>
#include "../include/CharaViewerDataManager.h"
#include "../include/CharaViewerFrame.h"
#include "../include/CharaViewerDefine.h"
#include "../include/CharaViewerCharaModelManager.h"

#include "../include/CharaViewerImpl.h"


GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)


CharaViewerImpl::CharaViewerImpl(CharaViewerProcParam* param, app::util::Heap* appHeap, GameSys::GameManager* gameManager)
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
    m_appRenderingManager(NULL),
    m_charaModelManager(NULL)
{}

CharaViewerImpl::~CharaViewerImpl()
{}

gfl2::proc::Result CharaViewerImpl::InitFunc(void)
{
  // Updateでつくることにする
  this->changeStep(STEP_CREATE);
  
  return gfl2::proc::RES_FINISH;
}

gfl2::proc::Result CharaViewerImpl::UpdateFunc(void)
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

  // キャラ描画
  if(m_charaModelManager) m_charaModelManager->Update();

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
void CharaViewerImpl::PreDrawFunc(void)
{
  // レンダリングマネージャ
  if(m_step == STEP_WORKING) m_appRenderingManager->PreDraw();
//  if(m_frameManager) m_frameManager->SetDispDebugNo(false);
}
void CharaViewerImpl::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_step == STEP_WORKING)
  {
    switch (displayNo)
    {
    case gfl2::gfx::CtrDisplayNo::LEFT:
      {
        // キャラ描画
        m_charaModelManager->Draw(displayNo);
      }
      break;

    case gfl2::gfx::CtrDisplayNo::DOWN:
      {
        // フレーム
        m_frameManager->Draw(displayNo);
      
        // レンダリングマネージャ
        m_appRenderingManager->Draw(displayNo);
      }
      break;
    }
  }
}
gfl2::proc::Result CharaViewerImpl::EndFunc(void)
{
  return gfl2::proc::RES_FINISH;
}

void CharaViewerImpl::changeStep(Step step)
{
  m_step      = step;
  m_stepCount = 0;
}

void CharaViewerImpl::updateFunc_None(void)
{}
void CharaViewerImpl::updateFunc_Create(void)
{
  switch(m_stepCount)
  {
  case 0:
    {
      // データマネージャ
      m_dataManager = GFL_NEW(m_appHeap->GetDeviceHeap()) CharaViewerDataManager(m_appHeap, m_gameManager->GetAsyncFileManager());
      m_dataManager->Start();
      ++m_stepCount;
      //breakしない
    }
  case 1:
    {
      // フレーム
      
      // フレームマネージャ生成
      m_frameManagerDevMem = GFL_CREATE_LOCAL_HEAP(m_appHeap->GetDeviceHeap(), CHARA_VIEWER_MEM_FRAME_MANAGER_DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
      m_frameManager = GFL_NEW(m_appHeap->GetDeviceHeap()) applib::frame::Manager(m_frameManagerDevMem);  // マネージャはヒープが荒れるので、専用のヒープを渡す。
      m_frameListener = GFL_NEW(m_appHeap->GetDeviceHeap()) CharaViewerFrameListener();
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
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
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
      // キャラ描画
      m_charaModelManager = GFL_NEW(m_appHeap->GetDeviceHeap()) CharaViewerCharaModelManager();
      m_charaModelManager->Initialize(m_gameManager, m_appHeap->GetDeviceHeap(), m_param->fieldDebugModeEnabled, false);

      ++m_stepCount;
      //breakしない
    } 
  case 5:
    {
      // フレーム

      // フレーム生成
      m_frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, CharaViewerFrameCell>(m_frameManager);
      CharaViewerFrameCell::Param param;
      param.param               = m_param;
      param.appHeap             = m_appHeap;
      param.gameManager         = m_gameManager;
      param.dataManager         = m_dataManager;
      param.frameManager        = m_frameManager;
      param.appRenderingManager = m_appRenderingManager;
      param.charaModelManager   = m_charaModelManager;
      m_frameCell->Setup(param);

      m_frameCreated = true;

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
void CharaViewerImpl::updateFunc_Working(void)
{
  // 描画時は必ず行う更新
  this->updateForDraw();

  if(m_frameReqDestroy)
  {
    // 次へ
    this->changeStep(STEP_DESTROY);
  }
}
void CharaViewerImpl::updateFunc_Destroy(void)
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
    {
      // キャラ管理
      m_charaModelManager->Finalize();
      GFL_SAFE_DELETE(m_charaModelManager);

      ++m_stepCount;
      //breakしない
    }
  case 6:
  default:
    {
      // 次へ
      this->changeStep(STEP_NONE);
      break;
    }
  }
}
void CharaViewerImpl::updateForDraw(void)
{
  // 特にやることはない
}


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
