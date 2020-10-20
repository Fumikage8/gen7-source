//==============================================================================
/**
 @file    KawadaTestKiraCardSampleMain.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.23
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Frame/AppFrameManager.h>

// KawadaTestのインクルード
#include "KawadaTestKiraCardSampleDataManager.h"
#include "KawadaTestKiraCardSampleFrame.h"

#include "KawadaTestKiraCardSampleMain.h"

#include "KawadaTestKiraCardSampleDefine.h"

/*
// test_t0
#include <str/include/gfl2_MsgData.h>
#include <Print/include/PrintSystem.h>
#include <arc_index/message.gaix>
*/




#if KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO
#ifdef GFL2_UI_USE_GYROSCOPE

#include <ui/include/gfl2_UI_Gyroscope.h>

void ControlGyro(GameSys::GameManager* gameManager)
{
  const gfl2::ui::Button*  button  = gameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  gfl2::ui::Gyroscope*     gyro    = gameManager->GetUiDeviceManager()->GetGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);

  if(button->IsTrigger(gfl2::ui::BUTTON_R))
  {
    if(gyro->IsValid())
    {
      gyro->SetAxisRotationMatrixFromCurrentDirection();
      GFL_PRINT("Gyro: SetAxisRotationMatrixFromCurrentDirection\n");
    }
  }
}

void PrintGyro(GameSys::GameManager* gameManager)
{
  /*
  gfl2::ui::Gyroscope* gyro = gameManager->GetUiDeviceManager()->GetGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
  if(gyro->IsValid())
  {
    gfl2::math::VEC3 dirX = gyro->GetDirectionOfX();
    gfl2::math::VEC3 dirY = gyro->GetDirectionOfY();
    gfl2::math::VEC3 dirZ = gyro->GetDirectionOfZ();
    GFL_PRINT("Gyro: dirX(%f,%f,%f), dirY(%f,%f,%f), dirZ(%f,%f,%f)\n", dirX.x,dirX.y,dirX.z, dirY.x,dirY.y,dirY.z, dirZ.x,dirZ.y,dirZ.z);

    gfl2::math::VEC3 angle = gyro->GetAngle();
    GFL_PRINT("Gyro: angle[degree] pitch%f, yaw=%f, roll=%f\n", angle.x*360.0f, angle.y*360.0f, angle.z*360.0f);
  }
  */
}

#endif
#endif  // KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


KawadaTestKiraCardSampleMain::KawadaTestKiraCardSampleMain(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
  : KawadaTestSampleBase(gameManager, parentHeapMem, parentDevMem),
    m_step(STEP_NONE),
    m_stepCount(0),
    m_appHeap(NULL),
    m_frameManagerUniMem(NULL),
    m_dataManager(NULL),
    m_frameManager(NULL),
    m_frameListener(NULL),
    m_frameCell(NULL),
    m_frameCreated(false),
    m_frameReqDestroy(false),
    m_appRenderingManager(NULL)
{
  this->initialize(gameManager, parentHeapMem, parentDevMem);
}
KawadaTestKiraCardSampleMain::~KawadaTestKiraCardSampleMain()
{
  this->terminate();
}

void KawadaTestKiraCardSampleMain::Update(void)
{
  // データマネージャ
  if(m_dataManager) m_dataManager->Update();

  // フレーム
  applib::frame::MainStatus frameStatus = applib::frame::MAINSTATUS_NULL;
  if(m_frameManager) frameStatus = m_frameManager->Main();
  if(  m_frameCreated  &&  frameStatus == applib::frame::MAINSTATUS_NULL  ) m_frameReqDestroy = true;

  // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->Update();  // 非同期生成処理を進めるために描画していないときもUpdateを毎フレーム呼ぶ。

  switch(m_step)
  {
  case STEP_NONE:    this->updateFunc_None();    break;
  case STEP_CREATE:  this->updateFunc_Create();  break;
  case STEP_WORKING: this->updateFunc_Working(); break;
  case STEP_DESTROY: this->updateFunc_Destroy(); break;
  }

#if KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO
#ifdef GFL2_UI_USE_GYROSCOPE
  ControlGyro(m_gameManager);
  
  PrintGyro(m_gameManager);
#endif
#endif  // KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO
}

void KawadaTestKiraCardSampleMain::PreDraw(void)
{
  if(m_step == STEP_WORKING)  // このテストでは、ワーキングステップのときだけ描画する
  {
    // レンダリングマネージャ
    m_appRenderingManager->PreDraw();
  }
}

void KawadaTestKiraCardSampleMain::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_step == STEP_WORKING)  // このテストでは、ワーキングステップのときだけ描画する
  {
    // フレーム
    m_frameManager->Draw(displayNo);
    
    // レンダリングマネージャ
    m_appRenderingManager->Draw(displayNo);
  }
}

void KawadaTestKiraCardSampleMain::initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
{
  static const s32 HEAP_MEM_SIZE  =  0x100000 - 0x1000;
  static const s32 DEV_MEM_SIZE   = 0x1A00000 - 0x1000;
 
  m_enableToEnd = false;
  m_gameManager = gameManager;

  m_heapMem     = GFL_CREATE_LOCAL_HEAP(parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_devMem      = GFL_CREATE_LOCAL_HEAP(parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_uniMem      = m_devMem;

  // 他はUpdateでつくることにする
  this->changeStep(STEP_CREATE);

#if KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO
#ifdef GFL2_UI_USE_GYROSCOPE
  m_gameManager->GetUiDeviceManager()->BootGyroscope(m_uniMem, gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);

  PrintGyro(m_gameManager);
#endif
#endif  // KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO
}

void KawadaTestKiraCardSampleMain::terminate(void)
{
#if KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO
#ifdef GFL2_UI_USE_GYROSCOPE
  PrintGyro(m_gameManager);
    
  m_gameManager->GetUiDeviceManager()->StopGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
#endif
#endif  // KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO

  if(m_enableToEnd)
  {
    m_uniMem = NULL;
    if(m_devMem)
    {
      GFL_DELETE_HEAP(m_devMem);
      m_devMem = NULL;
    }
    if(m_heapMem)
    {
      GFL_DELETE_HEAP(m_heapMem);
      m_heapMem = NULL;
    }

    m_gameManager = NULL;
  }
  else
  {
    GFL_ASSERT(0);
  }
}

void KawadaTestKiraCardSampleMain::changeStep(Step step)
{
  m_step      = step;
  m_stepCount = 0;
}

void KawadaTestKiraCardSampleMain::updateFunc_None(void)
{}
void KawadaTestKiraCardSampleMain::updateFunc_Create(void)
{
  switch(m_stepCount)
  {
  case 0:
    {
      static const s32 HEAP_SIZE_FRAME_MANAGER  =   0x1000;  //!< フレームマネージャのヒープ
      static const s32 HEAP_SIZE_APP_HEAP       =  0xF0000;  //!< このアプリのapp::util::Heap
      static const s32 HEAP_SIZE_APP_DEV        = 0xF00000;  //!< このアプリのapp::util::Heap
  
      // メモリ
      m_appHeap = GFL_NEW(m_uniMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_heapMem, m_devMem, HEAP_SIZE_APP_HEAP, HEAP_SIZE_APP_DEV);
      m_frameManagerUniMem = GFL_CREATE_LOCAL_HEAP( m_uniMem, HEAP_SIZE_FRAME_MANAGER, gfl2::heap::HEAP_TYPE_EXP, false );


      // データマネージャ
      m_dataManager = GFL_NEW(m_uniMem) KawadaTestKiraCardSampleDataManager(m_appHeap, m_gameManager->GetAsyncFileManager());
      m_dataManager->Start();


      // フレーム
      
      // フレームマネージャ生成
      m_frameManager  = GFL_NEW(m_uniMem) applib::frame::Manager(m_frameManagerUniMem);  // マネージャはヒープが荒れるので、専用のヒープを渡す。
      m_frameListener = GFL_NEW(m_uniMem) KawadaTestKiraCardSampleFrameListener();
      m_frameManager->SetAppFrameListener(m_frameListener);
 
      ++m_stepCount;
      //breakしない
    }
  case 1:
    {
      // レンダリングマネージャ
      app::util::AppRenderingManager::Description appRenderingManagerDesc;
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_EFFECT;
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::LAYOUT_EFFECT;
      m_appRenderingManager = GFL_NEW(m_uniMem) app::util::AppRenderingManager();
      m_appRenderingManager->StartAsyncCreate(
          appRenderingManagerDesc,
          m_appHeap,
          NULL,
          m_gameManager->GetAsyncFileManager()
      );
      
      ++m_stepCount;
      //breakしない
    }
  case 2:
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
  case 3:
    {
      // フレーム
       
      // フレーム生成
      {
        m_frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, KawadaTestKiraCardSampleFrameCell>(m_frameManager);
        KawadaTestKiraCardSampleFrameCell::Param param;
        param.appHeap             = m_appHeap;
        param.gameManager         = m_gameManager;
        param.dataManager         = m_dataManager;
        param.frameManager        = m_frameManager;
        param.appRenderingManager = m_appRenderingManager;
        m_frameCell->Setup(param);
       
        m_frameCreated = true;
      }

      ++m_stepCount;
      //breakしない
    }
  case 4:
    {
      /*
      // test_t0

      m_appHeap->GetDeviceHeap()->PrintHeapInfo("==== 0 ====");
     
      gfl2::str::MsgData* msgData = GFL_NEW(m_appHeap->GetDeviceHeap()) gfl2::str::MsgData( print::GetMessageArcId(), GARC_message_battlevideo_player_DAT, m_appHeap->GetDeviceHeap(), gfl2::str::MsgData::LOAD_FULL );

      m_appHeap->GetDeviceHeap()->PrintHeapInfo("==== 1 ====");
     
      GFL_SAFE_DELETE(msgData);
      
      m_appHeap->GetDeviceHeap()->PrintHeapInfo("==== 2 ====");
      */ 

      ++m_stepCount;
      //breakしない
    }
  case 5:
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
void KawadaTestKiraCardSampleMain::updateFunc_Working(void)
{
  // 描画時は必ず行う更新
  this->updateForDraw();

  if(m_frameReqDestroy)
  {
    // 次へ
    this->changeStep(STEP_DESTROY);
  }
}
void KawadaTestKiraCardSampleMain::updateFunc_Destroy(void)
{
  switch(m_stepCount)
  {
  case 0:
    {
      // 非表示にして、1フレ遅れ解放に対応させたい。
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
      }
      break;
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

        GFL_DELETE_HEAP(m_frameManagerUniMem);
        m_frameManagerUniMem = NULL;
       
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
      // メモリ
      if(m_frameManagerUniMem)
      {
        GFL_DELETE_HEAP(m_frameManagerUniMem);
      }
      GFL_SAFE_DELETE(m_appHeap);

      ++m_stepCount;
      //breakしない
    }
  case 6:
  default:
    {
      // 次へ
      m_enableToEnd = true;
      this->changeStep(STEP_NONE);
      break;
    }
  }
}

void KawadaTestKiraCardSampleMain::updateForDraw(void)
{
  // 特にやることはない
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

