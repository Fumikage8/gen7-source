//==============================================================================
/**
 @file    KawadaTestPokeModelSampleMain.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.15
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <fs/include/gfl2_Fs.h>
#include <gfx/include/gfl2_DrawUtil.h>


#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>


// poke_3d_libのインクルード
#include <model/include/gfl2_BaseCamera.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <System/include/nijiAllocator.h>
#include <System/include/Camera/ModelViewerInFrame.h>
#include <System/include/Camera/InFrameCamera.h>

#include <System/source/Camera/field/CameraDebugPrimitive.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/AppCamera.h>
#include <AppLib/include/Tool/AppToolPokeSimpleModelInFrame.h>

#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeModel.h>


// KawadaTestのインクルード
#include "KawadaTestPokeModelSampleMain.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


#define TEST_KAWADATEST_POKEMODELSAMPLE__TEST_MODEL (0)
static const u32 TEST_MODEL_NUM_MAX = 4;


KawadaTestPokeModelSampleMain::KawadaTestPokeModelSampleMain(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
  : KawadaTestSampleBase(gameManager, parentHeapMem, parentDevMem),
    m_step(STEP_NONE),
    m_stepCount(0),
    m_appHeap(NULL),
    m_nijiAllocatorForEdgeMapTexture(NULL),
    m_appRenderingManager(NULL),
    m_asyncCreateDestroyPokeModelStep(0),
    m_pokeModelSys(NULL),
    m_drawEnvNodeOfCamera(NULL),
    m_camera(NULL),
    m_drawEnvNode(NULL),
    m_inFrameSys(NULL),
    m_inFrameCamera(NULL),
    m_appCamera(NULL),
    m_testModelDataPackBuf(NULL),
    m_testMotDataBuf(NULL),
    m_testModelDataPack(NULL),
    m_testMotData(NULL),
    m_testModelResourceNode(NULL),
    m_testMotResourceNode(NULL),
    m_testBaseModel(NULL),
    m_testModelCount(0)
{
  for(u32 i=0; i<POKE_MODEL_NUM; ++i)
  {
    m_pokeModel[i] = NULL;
  }

  this->initialize(gameManager, parentHeapMem, parentDevMem);
}
KawadaTestPokeModelSampleMain::~KawadaTestPokeModelSampleMain()
{
  this->terminate();
}

void KawadaTestPokeModelSampleMain::Update(void)
{
  if(m_pokeModelSys)
  {
    m_pokeModelSys->Update();
  }
  if(m_inFrameSys)
  {
    m_inFrameSys->Update(m_inFrameCamera);
  }
  if(m_appRenderingManager)  // 非同期生成処理を進めるために描画していないときもUpdateを毎フレーム呼ぶ。
  {
    m_appRenderingManager->Update();
  }

  switch(m_step)
  {
  case STEP_NONE:    this->updateFunc_None();    break;
  case STEP_CREATE:  this->updateFunc_Create();  break;
  case STEP_WORKING: this->updateFunc_Working(); break;
  case STEP_DESTROY: this->updateFunc_Destroy(); break;
  }
}

void KawadaTestPokeModelSampleMain::PreDraw(void)
{
  if(m_step == STEP_WORKING)  // このテストでは、ワーキングステップのときだけ描画する
  {
    m_appRenderingManager->PreDraw();
  }
}

void KawadaTestPokeModelSampleMain::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_step == STEP_WORKING)  // このテストでは、ワーキングステップのときだけ描画する
  {
    m_appRenderingManager->Draw(displayNo);

#if 0
    // DrawUtilのテスト
    this->drawDrawUtil(displayNo);
#elif 0
    // インフレームのデバッグ表示がうまくできるかのテスト
    testInFrameDebugDraw(gfl2::gfx::CtrDisplayNo displayNo)
#endif

  }
}

void KawadaTestPokeModelSampleMain::initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
{
  static const s32 HEAP_MEM_SIZE  =  0x100000 - 0x1000;
  static const s32 DEV_MEM_SIZE   = 0x1000000 - 0x1000;
 
  m_enableToEnd = false;
  m_gameManager = gameManager;

  m_heapMem     = GFL_CREATE_LOCAL_HEAP(parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_devMem      = GFL_CREATE_LOCAL_HEAP(parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_uniMem      = m_devMem;

  // 他はUpdateでつくることにする
  this->changeStep(STEP_CREATE);
}

void KawadaTestPokeModelSampleMain::terminate(void)
{
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

void KawadaTestPokeModelSampleMain::changeStep(Step step)
{
  m_step      = step;
  m_stepCount = 0;
}

void KawadaTestPokeModelSampleMain::updateFunc_None(void)
{}
void KawadaTestPokeModelSampleMain::updateFunc_Create(void)
{
  switch(m_stepCount)
  {
  case 0:
    {
      static const s32 HEAP_SIZE_APP_HEAP           =  0xF0000;  //!< このアプリのapp::util::Heap
      static const s32 HEAP_SIZE_APP_DEV            = 0x400000;//= 0xF00000;  //!< このアプリのapp::util::Heap
  
      // メモリ
      m_appHeap = GFL_NEW(m_uniMem) app::util::Heap();
      m_appHeap->LocalHeapCreate(m_heapMem, m_devMem, HEAP_SIZE_APP_HEAP, HEAP_SIZE_APP_DEV);

#if GFL_DEBUG  // @note この定義で括られたgflib2の関数を使用している
#if PM_DEBUG
      m_appHeap->GetSystemHeap()->PrintHeapInfo("app::util::Heap SystemHeap 0");
      m_appHeap->GetDeviceHeap()->PrintHeapInfo("app::util::Heap DeviceHeap 0");
#endif
#endif

      m_nijiAllocatorForEdgeMapTexture = GFL_NEW(m_uniMem) System::nijiAllocator(m_devMem); 

      ++m_stepCount;
      //breakしない
    }
  case 1:
    {
      // レンダリングマネージャ
      app::util::AppRenderingManager::Description appRenderingManagerDesc;
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
      m_appRenderingManager = GFL_NEW(m_uniMem) app::util::AppRenderingManager();
      m_appRenderingManager->StartAsyncCreate(
          appRenderingManagerDesc,
          m_appHeap,
          m_nijiAllocatorForEdgeMapTexture,
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

#if GFL_DEBUG  // @note この定義で括られたgflib2の関数を使用している
#if PM_DEBUG
      m_appHeap->GetSystemHeap()->PrintHeapInfo("app::util::Heap SystemHeap 2");
      m_appHeap->GetDeviceHeap()->PrintHeapInfo("app::util::Heap DeviceHeap 2");
#endif
#endif

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
      this->initDrawUtil();
      ++m_stepCount;
      //breakしない
    }
  case 4:
    {
      // ポケモンモデル
      m_asyncCreateDestroyPokeModelStep = 0;

      ++m_stepCount;
      //breakしない
    }
  case 5:
    {
      // ポケモンモデル
      if( this->asyncCreatePokeModel() )
      {
        this->addPokeModel();

        // 初期値設定
        {

          /*
          m_pokeModel[0]->SetPosition(50, 0, 0);
          m_pokeModel[1]->SetPosition(-50, 0, 0);
          m_pokeModel[0]->ChangeAnimation(PokeTool::MODEL_ANIME_BUTURI1, true);
          m_pokeModel[1]->ChangeAnimation(PokeTool::MODEL_ANIME_TOKUSHU1, true);
          m_pokeModel[0]->SetAnimationIsLoop(true);
          m_pokeModel[1]->SetAnimationIsLoop(true);
          */

          m_pokeModel[0]->SetVisible(true);
          m_pokeModel[0]->SetPosition(0, 0, 0);

          m_pokeModel[1]->ChangeAnimation(PokeTool::MODEL_ANIME_BT_WAIT_A, true);
          m_pokeModel[1]->SetAnimationIsLoop(true);

        }

        ++m_stepCount;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 6:
    {
      // カメラ
      app::util::AppRenderingManager::CreateDefaultCamera(m_appHeap, &m_drawEnvNodeOfCamera, &m_camera);
      m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_camera);

      // ライト
      m_drawEnvNode = app::util::AppRenderingManager::CreateDefaultDrawEnv(m_appHeap);
      app::tool::PokeSimpleModelInFrame::SetLightParamForInFrame(m_drawEnvNode->GetLightSet(0)->GetLightNode(0), System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT);
      m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_drawEnvNode);

      // テストモデル
      { 
        m_testModelDataPackBuf  = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) void*[TEST_MODEL_NUM_MAX]                                                            ;
        m_testMotDataBuf        = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) void*[TEST_MODEL_NUM_MAX]                                                            ;
        m_testModelDataPack     = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack*[TEST_MODEL_NUM_MAX];
        m_testMotData           = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) gfl2::renderingengine::scenegraph::resource::GfBinaryMotData*[TEST_MODEL_NUM_MAX]    ;
        m_testModelResourceNode = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) gfl2::renderingengine::scenegraph::resource::ResourceNode*[TEST_MODEL_NUM_MAX]       ;
        m_testMotResourceNode   = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) gfl2::renderingengine::scenegraph::resource::ResourceNode*[TEST_MODEL_NUM_MAX]       ;
        m_testBaseModel         = GFL_NEW_ARRAY(m_appHeap->GetDeviceHeap()) poke_3d::model::BaseModel*[TEST_MODEL_NUM_MAX]                                       ;        

#if TEST_KAWADATEST_POKEMODELSAMPLE__TEST_MODEL
        void*  buf;
        size_t bufSize;
        size_t fileSize;

        gfl2::fs::PcUtil::ReadFile(&buf, &bufSize, &fileSize, "R:/home/momiji/resource/app_resource/resource/island/res3d/beans_bin/binary_wingl/bean_bin.wingl.gfbmdlp", m_appHeap->GetDeviceHeap(), 128);
        m_testModelDataPackBuf[0] = buf;
        
        gfl2::fs::PcUtil::ReadFile(&buf, &bufSize, &fileSize, "R:/home/momiji/resource/app_resource/resource/island/res3d/beans_bin/bean_bin.gfbmot", m_appHeap->GetDeviceHeap(), 128);
        m_testMotDataBuf[0] = buf;
        
        gfl2::fs::PcUtil::ReadFile(&buf, &bufSize, &fileSize, "R:/home/momiji/resource/app_resource/resource/island/res3d/beans_bin/bean_bin_touch.gfbmot", m_appHeap->GetDeviceHeap(), 128);
        m_testMotDataBuf[1] = buf;

        m_testModelDataPack[0] = GFL_NEW(m_appHeap->GetDeviceHeap()) gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack;
        m_testMotData[0]       = GFL_NEW(m_appHeap->GetDeviceHeap()) gfl2::renderingengine::scenegraph::resource::GfBinaryMotData;
        m_testMotData[1]       = GFL_NEW(m_appHeap->GetDeviceHeap()) gfl2::renderingengine::scenegraph::resource::GfBinaryMotData;

        m_testModelDataPack[0]->SetModelData(static_cast<c8*>(m_testModelDataPackBuf[0]));
        m_testMotData[0]->SetMotionData(static_cast<c8*>(m_testMotDataBuf[0]));
        m_testMotData[1]->SetMotionData(static_cast<c8*>(m_testMotDataBuf[1]));

        m_testModelResourceNode[0] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_appHeap->GetDevGLHeapAllocator(), m_testModelDataPack[0]);
        m_testMotResourceNode[0]   = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_appHeap->GetDevGLHeapAllocator(), m_testMotData[0]);
        m_testMotResourceNode[1]   = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_appHeap->GetDevGLHeapAllocator(), m_testMotData[1]);

        m_testBaseModel[0] = GFL_NEW(m_appHeap->GetDeviceHeap()) poke_3d::model::BaseModel;
        m_testBaseModel[0]->Create(m_appHeap->GetDevGLHeapAllocator(), m_appHeap->GetDeviceHeap(), m_testModelResourceNode[0]);
        
        m_testBaseModel[0]->AllocateAnimationByResourceNode(m_appHeap->GetDeviceHeap(), false, m_testMotResourceNode[0]);
        m_testBaseModel[0]->AllocateAnimationByResourceNode(m_appHeap->GetDeviceHeap(), false, m_testMotResourceNode[1]);

        m_testBaseModel[0]->ChangeAnimationByResourceNode(m_testMotResourceNode[0]);

        m_testBaseModel[0]->SetAnimationStepFrame(1.0f);
        m_testBaseModel[0]->SetAnimationLoop(true);
        m_testBaseModel[0]->SetAnimationFrame(0.0f);

        m_appRenderingManager->AddModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_testBaseModel[0]);
#endif  // TEST_KAWADATEST_POKEMODELSAMPLE__TEST_MODEL
      }

      ++m_stepCount;
      //breakしない
    }
  case 7:
    {
      // インフレーム
      m_inFrameSys = GFL_NEW(m_uniMem) System::Camera::CModelViewerInFrame();
      s32 left   =  50;
      s32 top    =  10;
      s32 right  = 270;
      s32 bottom = 230;
      m_inFrameSys->Init(System::Camera::CModelInFrame::SCREEN_LOWER, left, top, right, bottom, System::Camera::CModelViewerInFrame::TURN_DIRECTION_LEFT);
      m_inFrameSys->SetDrawPosition(System::Camera::CModelViewerInFrame::DRAW_POSITION_BOTTOM);

      m_inFrameCamera = GFL_NEW(m_uniMem) System::Camera::InFrameCamera();
      m_appRenderingManager->SetInFrameCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_inFrameCamera);
     
      m_inFrameSys->SetAndInitModel(m_pokeModel[1]);
      m_inFrameSys->ResetModel();
      m_inFrameSys->Update(m_inFrameCamera);  // 1フレーム変な位置にポケモンが出るのを防ぐために、すぐにインフレームのUpdateを呼んでおく。

      // AppCamera
      m_appCamera = GFL_NEW(m_uniMem) app::util::AppCamera();
      m_appCamera->SetupOrthoCamera(app::util::AppCamera::RenderPlaceType::UPPER_DISPLAY, 1.0f, 500.0f, -500.0f);
      m_appRenderingManager->SetAppCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_appCamera);

      ++m_stepCount;
      //breakしない
    }
  case 8:
  default:
    {
      // 描画時は必ず行う更新
      this->updateForDraw();
  
      // 次へ
      this->changeStep(STEP_WORKING);
      
#if GFL_DEBUG  // @note この定義で括られたgflib2の関数を使用している
#if PM_DEBUG
      m_appHeap->GetSystemHeap()->PrintHeapInfo("app::util::Heap SystemHeap 8");
      m_appHeap->GetDeviceHeap()->PrintHeapInfo("app::util::Heap DeviceHeap 8");
#endif
#endif

      break;
    }
  }
}
void KawadaTestPokeModelSampleMain::updateFunc_Working(void)
{
  bool finishFlag = false;
  
  const gfl2::ui::Button* button = m_gameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice* crossKey = m_gameManager->GetUiDeviceManager()->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);

  // カメラ
  {
    gfl2::math::Vector3 pos = m_camera->GetPosition();
    if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
    {
      pos.z -= 10.0f;
    }
    else if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      pos.z += 10.0f;
    }
    else if( crossKey->IsTrigger(gfl2::ui::DIR_LEFT) )
    {
      m_pokeModel[0]->SetPosition(0,0,0);
      m_pokeModel[1]->SetPosition(0,0,0);
    }
    else if( crossKey->IsTrigger(gfl2::ui::DIR_RIGHT) )
    {
      m_pokeModel[0]->SetPosition(50.0f,0,0);
      m_pokeModel[1]->SetPosition(50.0f,0,0);
    }
    m_camera->SetPosition(pos);
  }

  // 描画時は必ず行う更新
  this->updateForDraw();

  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    finishFlag = true;
  }

  if(finishFlag)
  {
    // 次へ
    this->changeStep(STEP_DESTROY);
  }
}
void KawadaTestPokeModelSampleMain::updateFunc_Destroy(void)
{
  switch(m_stepCount)
  {
  case 0:
    {
      // 非表示にして、1フレ遅れ解放に対応させたい。
      ++m_stepCount;
      break;  // breakする！
    }
  case 1:
    {
      // テストモデル
      {
        for(u32 i=0; i<TEST_MODEL_NUM_MAX; ++i)
        {
          if(m_testBaseModel[i]) m_appRenderingManager->RemoveModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_testBaseModel[i]);
          GFL_SAFE_DELETE(m_testBaseModel[i]);
          GFL_SAFE_DELETE(m_testMotResourceNode[i]);
          GFL_SAFE_DELETE(m_testModelResourceNode[i]);
          GFL_SAFE_DELETE(m_testMotData[i]);
          GFL_SAFE_DELETE(m_testModelDataPack[i]);
          GflHeapSafeFreeMemory(m_testMotDataBuf[i]);
          GflHeapSafeFreeMemory(m_testModelDataPackBuf[i]);
        }
        
        GFL_SAFE_DELETE_ARRAY(m_testBaseModel        );
        GFL_SAFE_DELETE_ARRAY(m_testMotResourceNode  );
        GFL_SAFE_DELETE_ARRAY(m_testModelResourceNode);
        GFL_SAFE_DELETE_ARRAY(m_testMotData          );
        GFL_SAFE_DELETE_ARRAY(m_testModelDataPack    );
        GFL_SAFE_DELETE_ARRAY(m_testMotDataBuf       );
        GFL_SAFE_DELETE_ARRAY(m_testModelDataPackBuf );
      }

      // AppCamera
      m_appRenderingManager->UnsetAppCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
      GFL_SAFE_DELETE(m_appCamera);

      // インフレーム
      m_inFrameSys->ResetModel();
      
      m_appRenderingManager->UnsetInFrameCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);

      GFL_SAFE_DELETE(m_inFrameCamera);
      GFL_SAFE_DELETE(m_inFrameSys);
      
      ++m_stepCount;
      //breakしない
    }
  case 2:
    {
      // ライト
      m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_drawEnvNode);
      
      // カメラ
      m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);

      // ポケモンモデル
      this->removePokeModel();
      m_asyncCreateDestroyPokeModelStep = 0;
      
      ++m_stepCount;
      break;  // breakする！
    }
  case 3:
    {
      // ライト
      GFL_SAFE_DELETE(m_drawEnvNode);

      // カメラ
      GFL_SAFE_DELETE(m_camera);
      GFL_SAFE_DELETE(m_drawEnvNodeOfCamera);

      // ポケモンモデル
      if( this->asyncDestroyPokeModel() )
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
      this->termDrawUtil();
      ++m_stepCount;
      //breakしない
    }
  case 5:
    {
      bool canDestroy = false;

      // レンダリングマネージャ
      if(m_appRenderingManager->CanDestroy())
      {
        GFL_SAFE_DELETE(m_appRenderingManager);
        canDestroy = true;
      }

      if(canDestroy)
      {
        // メモリ
        GFL_SAFE_DELETE(m_nijiAllocatorForEdgeMapTexture);
        GFL_SAFE_DELETE(m_appHeap);
        
        ++m_stepCount;
        //breakしない
      }
      else
      {
        break;
      }
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

void KawadaTestPokeModelSampleMain::updateForDraw(void)
{
  // テストモデル
  for(u32 i=0; i<TEST_MODEL_NUM_MAX; ++i)
  {
    if(m_testBaseModel[i])
    {
      if(m_testBaseModel[i]->IsCreated())
      {
        ++m_testModelCount;
        if(m_testModelCount == 120)
        {
          m_testBaseModel[0]->ChangeAnimationByResourceNode(m_testMotResourceNode[1]);
        }
        m_testBaseModel[i]->UpdateAnimation();
      }
    }
  }
}

bool KawadaTestPokeModelSampleMain::asyncCreatePokeModel(void)
{
  bool isCreated = false;

  switch(m_asyncCreateDestroyPokeModelStep)
  {
  case 0:
    {
      m_pokeModelSys = GFL_NEW(m_uniMem) PokeTool::PokeModelSystem();
      //システムの非同期初期化開始
      m_pokeModelSys->InitSystemAsync(m_uniMem, m_devMem, m_gameManager->GetAsyncFileManager(), POKE_MODEL_NUM);
      ++m_asyncCreateDestroyPokeModelStep;
      //breakしない
    }
  case 1:
    {
      //初期化待ち
      if( m_pokeModelSys->IsFinishInitSystemAsync() )
      {
        {
          //システム初期化
          m_pokeModelSys->CreateSystem(false, m_appHeap->GetDevGLHeapAllocator());
          //ポケモン用ヒープ作成
          PokeTool::PokeModelSystem::HeapOption opt;
          opt.animeType      = PokeTool::MODEL_ANIMETYPE_BATTLE;
          opt.useIdModel     = false;
          opt.useShadow      = true;
          opt.useColorShader = false;
          m_pokeModelSys->CreatePokeModelHeap(m_devMem, &opt);
        }

        {
          PokeTool::PokeModel::SetupOption opt;
          opt.dataHeap = m_devMem;
          opt.workHeap = m_uniMem->GetLowerHandle();
          opt.useShadow = true;
          for(u32 i=0; i<POKE_MODEL_NUM; ++i)
          {
            PokeTool::SimpleParam param;
            s32 no = 263;
            param.monsNo = (MonsNo)no;
            m_pokeModel[i] = m_pokeModelSys->CreateModelSimple(&param,opt);
          }
        }

        ++m_asyncCreateDestroyPokeModelStep;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 2:
    {
      bool allPokeModelIsCreated = true;
      for(u32 i=0; i<POKE_MODEL_NUM; ++i)
      {
        if( m_pokeModel[i]->IsFinishLoad() == false )
        {
          allPokeModelIsCreated = false;
          break;
        }
      }

      if(allPokeModelIsCreated)
      {
        for(u32 i=0; i<POKE_MODEL_NUM; ++i)
        {
          m_pokeModel[i]->Create();
        }
        ++m_asyncCreateDestroyPokeModelStep;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 3:
  default:
    {
      isCreated = true;
      break;
    }
  }

  return isCreated;
}

void KawadaTestPokeModelSampleMain::addPokeModel(void)
{
  for(u32 i=0; i<POKE_MODEL_NUM; ++i)
  {
    if(i==0) m_appRenderingManager->AddPokeModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pokeModel[i]);
    if(i==1) m_appRenderingManager->AddPokeModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pokeModel[i]);
  }
}
void KawadaTestPokeModelSampleMain::removePokeModel(void)
{
  for(u32 i=0; i<POKE_MODEL_NUM; ++i)
  {
    if(i==1) m_appRenderingManager->RemovePokeModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pokeModel[i]);
    if(i==0) m_appRenderingManager->RemovePokeModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pokeModel[i]);
  }
}

bool KawadaTestPokeModelSampleMain::asyncDestroyPokeModel(void)
{
  bool isDestroyed = false;
 
  switch(m_asyncCreateDestroyPokeModelStep)
  {
  case 0:
    {
      for(u32 i=0; i<POKE_MODEL_NUM; ++i)
      {
        m_pokeModelSys->DeleteModel(m_pokeModel[i]);
        m_pokeModel[i] = NULL;
      }

      //ポケモン用ヒープ削除
      m_pokeModelSys->DeletePokeModelHeap();
      //システム開放開始
      m_pokeModelSys->TermSystemAsync();
      
      ++m_asyncCreateDestroyPokeModelStep;
      //breakしない
    }
  case 1:
    {
      if( m_pokeModelSys->IsFinishTermSystemAsync() )
      {
        GFL_SAFE_DELETE(m_pokeModelSys);
        ++m_asyncCreateDestroyPokeModelStep;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case 2:
  default:
    {
      isDestroyed = true;
      break;
    }
  }

  return isDestroyed;
}

void KawadaTestPokeModelSampleMain::initDrawUtil(void)
{
  //u32 displayWidth = 400;
  //u32 displayHeight = 240;
  //gfl2::gfx::DrawUtil::Initialize(m_appHeap->GetDevGLHeapAllocator(), displayWidth, displayHeight, 64, 64);  外部でやってくれているようなのでコメントアウト。

  //gfl2::gfx::DrawUtil::SetTexture(texture);
}
void KawadaTestPokeModelSampleMain::termDrawUtil(void)
{
  //gfl2::gfx::DrawUtil::Terminate();  外部でやってくれているようなのでコメントアウト。
}
void KawadaTestPokeModelSampleMain::drawDrawUtil(gfl2::gfx::CtrDisplayNo displayNo)  // gflib2/gfx/Sample/DrawUtil/main.cppを参考にした
{
  gfl2::gfx::DisplayDeviceHandle handle = System::GflUse::m_HandleList[displayNo];
  
  gfl2::gfx::GFGL::SetActiveDisplay(handle);
 
  gfl2::gfx::Surface* pBackBuffer  = gfl2::gfx::GFGL::GetBackBuffer( handle );
  gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer( handle );
 
  gfl2::gfx::GFGL::SetRenderTarget( pBackBuffer );
  gfl2::gfx::GFGL::SetDepthStencil( pDepthBuffer );
 
  gfl2::gfx::GFGL::BeginScene();

  u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth( handle );
  u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight( handle );
 
  // 簡易描画のシザリング設定(描画前に行うこと)
  gfl2::gfx::DrawUtil::SetScissor(0, 0, displayWidth, displayHeight);  // シザリング領域設定
 
  // カリングの設定(描画前に行うこと)
  gfl2::gfx::DrawUtil::SetCullMode(gfl2::gfx::CullMode::None);  // 両面描画
 
  // ワイヤーフレームON/OFFの設定(描画前に行うこと)
  gfl2::gfx::DrawUtil::SetFillMode(gfl2::gfx::FillMode::Fill);  // 通常描画
 
  gfl2::gfx::GFGL::ClearRenderTarget( pBackBuffer, gfl2::gfx::Color::GetWhite() );
  gfl2::gfx::GFGL::ClearDepthStencil( pDepthBuffer, 1.0f, 255 );
 
  gfl2::gfx::GFGL::SetViewport(0, 0, displayWidth, displayHeight);
 
  // 簡易描画開始(簡易描画を始める前に必ず呼ぶ)
  gfl2::gfx::DrawUtil::BeginRender();
 
  gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
  gfl2::gfx::DrawUtil::SetProjectionMatrix(
    gfl2::math::Matrix44::GetPerspective(
        gfl2::math::ConvDegToRad(37.0f),    // 視野角30°
        (float)displayWidth / (float)displayHeight,  // アスペクト比
        1.1f,              // 前方投影面までの距離
        123.0f)              // 後方投影面までの距離
  );
 
  gfl2::math::Vector3 eyePosition( 5.0f, 5.0f, -5.0f );  // 視点(カメラの位置)
  gfl2::math::Vector3 focusPosition( 0.0f, 0.0f,  0.0f );  // 注視点
  gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
  gfl2::gfx::DrawUtil::SetViewMatrix(
    gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection)
  );
 
  gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetIdentity());
  // 黒の矩形を描画
  gfl2::gfx::DrawUtil::FillRect(0.0f, 300.0f, 100.0f, 100.0f, gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
  /////////////////////////////////////////////////////////////////////////////////////////
  // 線と三角形の描画を試してみる
  gfl2::gfx::DrawUtil::VertexFormat lineVertex[] =
  {
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(10,10,10,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(1,0,0,1)),
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(400,400,400,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(1,0,0,1)),
  };
  gfl2::gfx::DrawUtil::DrawLines(lineVertex, 2, false);
  gfl2::gfx::DrawUtil::VertexFormat triVertex[] =
  {
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-400,-400,-400,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-200,-200,-200,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-100,-100,-100,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(150,150,150,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
    gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(60,60,60,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
  };
  gfl2::gfx::DrawUtil::DrawTriangles(triVertex, 6, false);
  /////////////////////////////////////////////////////////////////////////////////////////
  // テクスチャ付きの矩形を描画
  //DrawUtil::DrawRect(200.0f, 360.0f, 100.0f, 50.0f);
  // ワールド行列を反映させて赤のAABBを描画
  gfl2::gfx::DrawUtil::SetModelMatrix(gfl2::math::Matrix44::GetTranslation(gfl2::math::Vector3(-1.25f, 0.0f, -2.5f)));
  gfl2::gfx::DrawUtil::DrawAABB(
    gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 0.0f),
    gfl2::math::Vector4(1.0f, 2.0f, 3.0f, 0.0f),
    gfl2::math::Vector4(1.0f, 0.0f, 0.0f, 0.0f));
  // AABBの開始点, 終了点を指定して描画
  gfl2::gfx::DrawUtil::DrawAABB(
    gfl2::math::AABB(
      gfl2::math::Vector4(-0.5f, -0.5f, -0.5f, 0.0f),
      gfl2::math::Vector4(0.5f, 0.5f, 0.5f, 0.0f)
      ));
  // 頂点フォーマットを用意して, 青い三角形を描画
  gfl2::math::Vector4 color = gfl2::math::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
  gfl2::gfx::DrawUtil::VertexFormat vertexFormat[] =
  {
    gfl2::gfx::DrawUtil::VertexFormat(  gfl2::math::Vector4( 0.0f, -1.0f, 0.0f, 0.0f),
                  gfl2::math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f),
                  gfl2::math::Vector4(color.x, color.y, color.z, color.w)),
    gfl2::gfx::DrawUtil::VertexFormat(  gfl2::math::Vector4( 1.0f, 0.0f, 0.0f, 0.0f),
                  gfl2::math::Vector4( 0.0f, 0.0f, 0.0f, 0.0f),
                  gfl2::math::Vector4(color.x, color.y, color.z, color.w)),
    gfl2::gfx::DrawUtil::VertexFormat(  gfl2::math::Vector4( 1.0f, -1.0f, 0.0f, 0.0f),
                  gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 0.0f),
                  gfl2::math::Vector4(color.x, color.y, color.z, color.w)),
  };
  gfl2::gfx::DrawUtil::DrawTriangles(vertexFormat, sizeof(vertexFormat) / sizeof(vertexFormat[0]), false);
 
  // アルファ値を設定して, 画面全体を緑で描画(フェードアウト)
  static int s_FrameCnt = 0;
  ++s_FrameCnt;
  f32 alpha = static_cast<f32>(s_FrameCnt % 300) / 300.0f;
  gfl2::gfx::DrawUtil::FillScreen(gfl2::math::Vector4(0.0f, 1.0f, 0.0f, alpha));
  
  // 簡易描画終了(簡易描画を終えたら必ず呼ぶ)
  gfl2::gfx::DrawUtil::EndRender();
  
  gfl2::gfx::GFGL::EndScene();
}

void KawadaTestPokeModelSampleMain::testInFrameDebugDraw(gfl2::gfx::CtrDisplayNo displayNo)
{
#ifdef GF_PLATFORM_WIN32
  /*
  // CModelViewerInFrameのDrawDebugのテスト
  {
    System::Camera::CModelViewerInFrame::m_nDispInfo = 1;
    if(m_inFrameSys)
    {
      if(displayNo == gfl2::gfx::CtrDisplayNo::DOWN)
      {
        m_inFrameSys->DrawDebug(m_inFrameCamera);
      }
    }
  }
  */



  // System::Camera::field::CDebugPrimを使ったDrawUtilのテスト
  {
    if(displayNo == gfl2::gfx::CtrDisplayNo::DOWN)
    {
      float fAspectRatio1;
      gfl2::math::VEC2 inPosiMax1, inPosiMin1;
   
      fAspectRatio1 = 320.0f/240.0f;
      inPosiMax1 = gfl2::math::VEC2(1.0f, 1.0f);
      inPosiMin1 = gfl2::math::VEC2(-1.0f, -1.0f);

      if( System::Camera::field::CDebugPrim::BeginProjSpace(displayNo) == true )
      {
        /*
        {
          //////////////////////////////////
          // テストその１  ビューマトリクスとプロジェクションマトリクスを設定してみる
          //////////////////////////////////
          
          gfl2::math::MTX44 projMtx;
          gfl2::math::MTX44Ortho(&projMtx, -160, 160, -120, 120, 0, 10000);
         
          gfl2::gfx::DrawUtil::SetProjectionMatrix( gfl2::math::ConvertNwMtx44ToGfMtx44(projMtx) );
         
         
         
          gfl2::gfx::DrawUtil::SetMaterialAlpha(1.0f);
          gfl2::gfx::DrawUtil::SetProjectionMatrix(
            gfl2::math::Matrix44::GetPerspective(
                gfl2::math::ConvDegToRad(37.0f),    // 視野角30°
                fAspectRatio1,  // アスペクト比
                1.1f,              // 前方投影面までの距離
                123.0f)              // 後方投影面までの距離
          );
         
          gfl2::math::Vector3 eyePosition( 5.0f, 5.0f, -5.0f );  // 視点(カメラの位置)
          gfl2::math::Vector3 focusPosition( 0.0f, 0.0f,  0.0f );  // 注視点
          gfl2::math::Vector3 upDirection( 0.0f, 1.0f,  0.0f );  // カメラの上方向
          gfl2::gfx::DrawUtil::SetViewMatrix(
            gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection)
          );
         
         
         
          System::Camera::field::CDebugPrim::DrawFrameProjSpace(inPosiMax1, inPosiMin1, 0.05f, gfl2::math::VEC4(0.0f, 0.0f, 0.0f, 0.45f), fAspectRatio1, -1.0f);
         
          // 黒の矩形を描画
          //gfl2::gfx::DrawUtil::FillRect(0.0f, 300.0f, 100.0f, 100.0f, gfl2::math::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
        }
        */

        {
          //////////////////////////////////
          // テストその２  ビューマトリクスとプロジェクションマトリクスが単位行列のときは、左上(-1,1), 右上(1,1), 左下(-1,-1), 右下(1,1), 中央(0,0)になる。トライアングルはCTRでも出るはずだが、何故か出ていない。
          //////////////////////////////////
 
          gfl2::gfx::DrawUtil::VertexFormat triVertex[] =
          {
            gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
            gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.9,0.9,0,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
            gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(-0.5,-0.1,0,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
          };
          gfl2::gfx::DrawUtil::DrawTriangles(triVertex, 3, false);
        }

        /*
        {
          //////////////////////////////////
          // テストその３  ライン描画。ラインはCTRでは出ない。
          //////////////////////////////////
 
          gfl2::gfx::DrawUtil::VertexFormat lineVertex[] =
          {
            gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
            gfl2::gfx::DrawUtil::VertexFormat(gfl2::math::Vector4(0.9,0.9,0,0), gfl2::math::Vector4(0,0,0,0), gfl2::math::Vector4(0,0,1,1)),
          };
          gfl2::gfx::DrawUtil::DrawLines(lineVertex, 2, false);
        } 
        */

        System::Camera::field::CDebugPrim::EndProjSpace();
      }
    }
  }
#endif  // GF_PLATFORM_WIN32
}



GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

