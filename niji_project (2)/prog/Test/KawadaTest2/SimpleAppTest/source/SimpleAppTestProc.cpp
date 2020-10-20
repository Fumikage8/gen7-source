#if PM_DEBUG


//==============================================================================
/**
 @file    SimpleAppTestProc.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.22
 @brief   シンプルなアプリのProc
 */
//==============================================================================


// nijiのインクルード
#include <System/include/HeapDefine.h>
#include <GameSys/include/GameManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>

// 本アプリのインクルード
#include <Test/KawadaTest2/SimpleAppTest/include/SimpleAppTestProc.h>
#include "SimpleAppTestFrame.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)


SimpleAppTestProc::SimpleAppTestProc(SimpleAppTestProcParam* param)
  : GameSys::GameProc(),
    m_param(param),
    m_devMem(NULL),
    m_frameManagerDevMem(NULL),
    m_appHeap(NULL),
    m_appRenderingManager(NULL),
    m_frameManager(NULL),
    m_frameListener(NULL),
    m_frameParam(NULL),
    m_frameCell(NULL)
{}
SimpleAppTestProc::~SimpleAppTestProc()
{}

gfl2::proc::Result SimpleAppTestProc::InitFunc( gfl2::proc::Manager* /*pManager*/ )
{
  enum
  {
    SEQ_CREATE_HEAP                                   ,
    SEQ_START_ASYNC_CREATE_APP_RENDERING_MANAGER      ,
    SEQ_WAIT_ASYNC_CREATE_APP_RENDERING_MANAGER_FINISH,
    SEQ_CREATE_FRAME                                  ,
    SEQ_END                                           ,
  };

  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;

  int seq = this->GetSubSeq();  // 親であるgfl2::proc::BaseProcessのメンバ

  GameSys::GameManager* gameManager = this->GetGameManager();  // 親であるGameSys::GameProcのメンバ

  switch(seq)
  {
  case SEQ_CREATE_HEAP:
    {
      // ヒープ
      gfl2::heap::HeapBase* parentDevMem = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_devMem = GFL_CREATE_LOCAL_HEAP(parentDevMem, 0x100000, gfl2::heap::HEAP_TYPE_EXP, false);
 
      m_appHeap = GFL_NEW(m_devMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_devMem, m_devMem, 0x1000, 0xF1000);  // @attention デバイスメモリを使ってはいけないところがあるときにはこれはダメ。
     
      m_frameManagerDevMem = GFL_CREATE_LOCAL_HEAP( m_devMem, 0x1000, gfl2::heap::HEAP_TYPE_EXP, false );  // フレームマネージャはヒープが荒れるので、専用のヒープをつくっておく。

      ++seq;
      //breakしない
    }
  case SEQ_START_ASYNC_CREATE_APP_RENDERING_MANAGER:
    {
      // レンダリングマネージャ
      m_appRenderingManager = GFL_NEW(m_appHeap->GetDeviceHeap()) app::util::AppRenderingManager();
      
      app::util::AppRenderingManager::Description desc;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      desc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT;
      m_appRenderingManager->StartAsyncCreate(
          desc,
          m_appHeap,
          NULL,
          gameManager->GetAsyncFileManager()
      );
      
      ++seq;
      //breakしない
    }
  case SEQ_WAIT_ASYNC_CREATE_APP_RENDERING_MANAGER_FINISH:
    {
      if(m_appRenderingManager->IsCreated())
      {
        ++seq;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_CREATE_FRAME:
    {
      // フレーム
      m_frameManager  = GFL_NEW(m_appHeap->GetDeviceHeap()) applib::frame::Manager(m_frameManagerDevMem);  // フレームマネージャはヒープが荒れるので、専用のヒープを渡す。
      m_frameListener = GFL_NEW(m_appHeap->GetDeviceHeap()) SimpleAppTestFrameListener();
      m_frameManager->SetAppFrameListener(m_frameListener);

      m_frameParam = GFL_NEW(m_appHeap->GetDeviceHeap()) SimpleAppTestFrameParam();
      m_frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, SimpleAppTestFrameCell>(m_frameManager);
      m_frameParam->appHeap             = m_appHeap;
      m_frameParam->gameManager         = gameManager;
      m_frameParam->appRenderingManager = m_appRenderingManager;
      m_frameCell->Setup(m_frameParam);
      
      ++seq;
      //breakしない
    }
  case SEQ_END:
  default:
    {
      res = gfl2::proc::RES_FINISH;
      break;
    }
  } 

  // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->Update();  // 非同期生成処理を進めるために描画していないときもUpdateを毎フレーム呼ぶ。

  this->SetSubSeq(seq);  // 親であるgfl2::proc::BaseProcessのメンバ

  return res;
}

gfl2::proc::Result SimpleAppTestProc::UpdateFunc( gfl2::proc::Manager* /*pManager*/ )
{
  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;
  
  // フレーム
  applib::frame::MainStatus frameStatus = m_frameManager->Main();
  if( frameStatus == applib::frame::MAINSTATUS_NULL )
  {
    res = gfl2::proc::RES_FINISH;
  }
  
  // レンダリングマネージャ
  m_appRenderingManager->Update();

  return res;
}

void SimpleAppTestProc::PreDrawFunc( gfl2::proc::Manager* /*pManager*/ )
{
  // レンダリングマネージャ
  m_appRenderingManager->PreDraw();
}

void SimpleAppTestProc::DrawFunc( gfl2::proc::Manager* /*pManager*/, gfl2::gfx::CtrDisplayNo displayNo )
{
  // フレームマネージャ
  m_frameManager->Draw(displayNo);
  
   // レンダリングマネージャ
  m_appRenderingManager->Draw(displayNo);
}

gfl2::proc::Result SimpleAppTestProc::EndFunc( gfl2::proc::Manager* /*pManager*/ )
{
  enum
  {
    SEQ_WAIT_NOT_DRAWING                         ,
    SEQ_WAIT_DESTROY_FRAME_FINISH                ,
    SEQ_WAIT_DESTROY_APP_RENDERING_MANAGER_FINISH,
    SEQ_DESTROY_HEAP                             ,
    SEQ_END                                      ,
  };

  gfl2::proc::Result res = gfl2::proc::RES_CONTINUE;

  int seq = this->GetSubSeq();  // 親であるgfl2::proc::BaseProcessのメンバ

  switch(seq)
  {
  case SEQ_WAIT_NOT_DRAWING:
    {
      // 1フレーム描画しない
      ++seq;
      break;
    }
  case SEQ_WAIT_DESTROY_FRAME_FINISH:
    {
      // フレーム
      if(m_frameManager->End() == false)
      {
        GFL_SAFE_DELETE(m_frameManager);
        GFL_SAFE_DELETE(m_frameListener);
        GFL_SAFE_DELETE(m_frameParam);
        ++seq;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_DESTROY_APP_RENDERING_MANAGER_FINISH:
    {
      // レンダリングマネージャ
      if(m_appRenderingManager->CanDestroy())
      {
        GFL_SAFE_DELETE(m_appRenderingManager);
        ++seq;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_DESTROY_HEAP:
    {
      // ヒープ
      GFL_DELETE_HEAP(m_frameManagerDevMem);
      m_frameManagerDevMem = NULL;
      GFL_SAFE_DELETE(m_appHeap);
      GFL_DELETE_HEAP(m_devMem);
      m_devMem = NULL;
      ++seq;
      //breakしない
    }
  case SEQ_END:
  default:
    {
      res = gfl2::proc::RES_FINISH;
      break;
    }
  }

  // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->Update();  // 非同期生成処理を進めるために描画していないときもUpdateを毎フレーム呼ぶ。

  this->SetSubSeq(seq);  // 親であるgfl2::proc::BaseProcessのメンバ

  return res;
}


GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

