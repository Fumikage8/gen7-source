//==============================================================================
/**
 @file    KawadaTestLayoutSampleMain.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.15
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/AppCamera.h>

#include <AppLib/include/Frame/AppFrameManager.h>

// KawadaTestのインクルード
#include "KawadaTestLayoutSampleDataManager.h"
#include "KawadaTestLayoutSampleFrame.h"

#include "KawadaTestLayoutSampleMain.h"


#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
#include <PokeTool/include/PokeModelSystem.h>
#include <PokeTool/include/PokeModel.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>
#include <gfx/include/gfl2_Texture.h>
#include <model/include/gfl2_BaseCamera.h>
#include <System/include/nijiAllocator.h>
#include <thread/include/gfl2_threadstatic.h>
#include <AppLib/include/Util/app_util_RenderTargetTexture.h>
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST


#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
#include <AppLib/include/Util/EffectUtil.h>

    #define USE_EFFECT_UTIL_MODE_1RES_1EMITTER_SET_1GROUP  // これが定義されているときMODE_1RES_1EMITTER_SET_1GROUP

#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


KawadaTestLayoutSampleMain::KawadaTestLayoutSampleMain(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
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
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
    ,
    m_vramAllocator(NULL),
    m_pokeModelSys(NULL),
    m_pokeModel(NULL),
    m_drawEnvNodeOfCamera(NULL),
    m_camera(NULL),
    m_drawEnvNodeOfLight(NULL),
    m_renderTargetTexture(NULL)
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
    , m_appCamera(NULL)
    , m_appCameraLower(NULL)
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
    ,    
    m_effectRenderPathConfig(),
    m_effectSys(NULL),
    m_effectHeap(NULL),
    m_effectGroupIDList(NULL),
    m_effectDrawEnvNodeOfCamera(NULL),
    m_effectCamera(NULL)
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
    ,
    m_effectUtil(NULL),
    m_effectDrawEnvNodeOfCamera(NULL),
    m_effectCamera(NULL),
    m_isFirst(true)
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
{
  this->initialize(gameManager, parentHeapMem, parentDevMem);
}
KawadaTestLayoutSampleMain::~KawadaTestLayoutSampleMain()
{
  this->terminate();
}

void KawadaTestLayoutSampleMain::Update(void)
{
  // データマネージャ
  if(m_dataManager) m_dataManager->Update();

  // フレーム
  applib::frame::MainStatus frameStatus = applib::frame::MAINSTATUS_NULL;
  if(m_frameManager) frameStatus = m_frameManager->Main();
  if(  m_frameCreated  &&  frameStatus == applib::frame::MAINSTATUS_NULL  ) m_frameReqDestroy = true;

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
  if(m_pokeModelSys) m_pokeModelSys->Update();
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
  
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
  // エフェクト
  {
    if(m_effectSys)
    {
      // エミッターが無くなったら初期化
      if( m_effectSys->GetNwObject()->GetNumEmitter( m_effectGroupID ) == 0 )
      {
        bool testFlag = true;
        if(testFlag)
        {
          gfl2::Effect::Handle  effectHandle;
          m_effectSys->CreateEmitterSetID( &effectHandle, gfl2::math::Vector3(0,0,0), m_effectEmitterSetID, m_effectResID, m_effectGroupID );
        }
      }

      // エフェクト更新処理
      m_effectSys->BeginFrame();
      m_effectSys->Calc( m_effectGroupID );
    }
  }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
  #ifdef USE_EFFECT_UTIL_MODE_1RES_1EMITTER_SET_1GROUP
  // エフェクト
  {
    if(m_effectUtil!=NULL)
    {
      if(m_isFirst)
      {
        bool testFlag = true;
        if(testFlag)
        {
          m_effectUtil->CreateAllEffectEmitterSetForMode1Res1EmitterSet1Group();
          gfl2::Effect::Handle* effectHandle = m_effectUtil->GetEffectHandleForMode1Res1EmitterSet1Group(m_effectResID);
          nw::eft::EmitterSet* emitterSet = effectHandle->GetEmitterSet();
          if(emitterSet != NULL)  // EffectのHnadleが保有しているEmitterSetのポインタが別のものになる場合への対応
          {
            gfl2::math::Vector3 pos(160.0f, 120.0f, 0.0f);
            gfl2::Effect::EmitterSet::SetGFPos(emitterSet, pos);
            m_isFirst = false;
          }
        }
      }
      
      // エフェクト更新処理
      m_effectUtil->UpdateForMode1Res1EmitterSet1Group();
    }
  }
  #else
  // エフェクト
  {
    if(m_effectUtil!=NULL && m_effectUtil->GetEffectSystem()!=NULL)
    {
      // エミッターが無くなったら初期化
      if( m_effectUtil->GetEffectSystem()->GetNwObject()->GetNumEmitter( m_effectGroupID ) == 0 )
      {
        bool testFlag = true;
        if(testFlag)
        {
          gfl2::Effect::Handle  effectHandle;
          m_effectUtil->GetEffectSystem()->CreateEmitterSetID( &effectHandle, gfl2::math::Vector3(0,0,0), m_effectEmitterSetID, m_effectResID, m_effectGroupID );
        }
      }

      // エフェクト更新処理
      m_effectUtil->GetEffectSystem()->BeginFrame();
      m_effectUtil->GetEffectSystem()->Calc( m_effectGroupID );
    }
  }
  #endif
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL

  // レンダリングマネージャ
  if(m_appRenderingManager) m_appRenderingManager->Update();  // 非同期生成処理を進めるために描画していないときもUpdateを毎フレーム呼ぶ。

  switch(m_step)
  {
  case STEP_NONE:    this->updateFunc_None();    break;
  case STEP_CREATE:  this->updateFunc_Create();  break;
  case STEP_WORKING: this->updateFunc_Working(); break;
  case STEP_DESTROY: this->updateFunc_Destroy(); break;
  }
}

void KawadaTestLayoutSampleMain::PreDraw(void)
{
  if(m_step == STEP_WORKING)  // このテストでは、ワーキングステップのときだけ描画する
  {
    // レンダリングマネージャ
    m_appRenderingManager->PreDraw();
  }
}

void KawadaTestLayoutSampleMain::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_step == STEP_WORKING)  // このテストでは、ワーキングステップのときだけ描画する
  {
    // フレーム
    m_frameManager->Draw(displayNo);
    
    // レンダリングマネージャ
    m_appRenderingManager->Draw(displayNo);
  }
}

void KawadaTestLayoutSampleMain::initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
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
}

void KawadaTestLayoutSampleMain::terminate(void)
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

void KawadaTestLayoutSampleMain::changeStep(Step step)
{
  m_step      = step;
  m_stepCount = 0;
}

void KawadaTestLayoutSampleMain::updateFunc_None(void)
{}
void KawadaTestLayoutSampleMain::updateFunc_Create(void)
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
      m_dataManager = GFL_NEW(m_uniMem) KawadaTestLayoutSampleDataManager(m_appHeap, m_gameManager->GetAsyncFileManager());
      m_dataManager->Start();


      // フレーム
      
      // フレームマネージャ生成
      m_frameManager  = GFL_NEW(m_uniMem) applib::frame::Manager(m_frameManagerUniMem);  // マネージャはヒープが荒れるので、専用のヒープを渡す。
      m_frameListener = GFL_NEW(m_uniMem) KawadaTestLayoutSampleFrameListener();
      m_frameManager->SetAppFrameListener(m_frameListener);

      {
        // MultiTextureGpuHangUpWorkaroundEnabledのテスト
#if defined(GF_PLATFORM_CTR)  // cafeではdrawerが存在しない
        nw::lyt::Drawer  drawer;

        bool preventHangUpFlag = drawer.IsMultiTextureGpuHangUpWorkaroundEnabled();  // DrawerのInitializeを通さないとtrueで初期化されない。
        GFL_PRINT("preventHangUpFlag=%d\n", preventHangUpFlag);

        drawer.SetMultiTextureGpuHangUpWorkaroundEnabled(false);
        preventHangUpFlag = drawer.IsMultiTextureGpuHangUpWorkaroundEnabled();
        GFL_PRINT("preventHangUpFlag=%d\n", preventHangUpFlag);

        drawer.SetMultiTextureGpuHangUpWorkaroundEnabled(true);
        preventHangUpFlag = drawer.IsMultiTextureGpuHangUpWorkaroundEnabled();
        GFL_PRINT("preventHangUpFlag=%d\n", preventHangUpFlag);
#endif
      }
      
      ++m_stepCount;
      //breakしない
    }
  case 1:
    {
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
      m_vramAllocator = GFL_NEW(m_uniMem) System::nijiAllocator(m_devMem);
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST


      // レンダリングマネージャ
      app::util::AppRenderingManager::Description appRenderingManagerDesc;
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1;
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL0_EFFECT_LAYOUT_POKEMODEL1;
      appRenderingManagerDesc.renderDesc[app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN].renderOrder = app::util::AppRenderingManager::RenderOrderType::POKEMODEL;
      m_appRenderingManager = GFL_NEW(m_uniMem) app::util::AppRenderingManager();
      m_appRenderingManager->StartAsyncCreate(
          appRenderingManagerDesc,
          m_appHeap,
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
          m_vramAllocator,
#else  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
          NULL,
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
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
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
      {
        // pokeModelSys
        m_pokeModelSys = GFL_NEW(m_uniMem) PokeTool::PokeModelSystem();
        //システムの非同期初期化開始
        m_pokeModelSys->InitSystemAsync(m_uniMem, m_devMem, m_gameManager->GetAsyncFileManager(), 1);
        //初期化待ち
        while( !m_pokeModelSys->IsFinishInitSystemAsync() )
        {
          gfl2::thread::ThreadStatic::CurrentSleep(1);
        }
   
        {
          //システム初期化
          m_pokeModelSys->CreateSystem(false, m_appHeap->GetDevGLHeapAllocator());
          //ポケモン用ヒープ作成
          PokeTool::PokeModelSystem::HeapOption opt;
          opt.animeType      = PokeTool::MODEL_ANIMETYPE_BATTLE;
          opt.useIdModel     = false;
          opt.useShadow      = false;
          opt.useColorShader = false;
          m_pokeModelSys->CreatePokeModelHeap(m_devMem, &opt);
        } 

        // pokeModel
        PokeTool::PokeModel::SetupOption opt;
        opt.dataHeap = m_devMem;
        opt.workHeap = m_uniMem->GetLowerHandle();
        PokeTool::SimpleParam param;
        u32 m = 751;//577;//751;//382;//263;  // 577ユニラン; 751シズクモ; 382カイオーガ; 263ジグザグマ;
        //
        // シズクモ
        // オフスクリーンサイズ256x256のとき、m_debugTexScaleVal(1.0,0.5)、m_debugTexTranslateVal(0.0,0.0)(Translateは(0,1),(0,-1)でもいい)でIDエッジが適切な位置に描画される。
        // オフスクリーンサイズ128x128のとき、m_debugTexScaleVal(0.5,0.25)、m_debugTexTranslateVal(0.0,0.0)(Translate他の値は試していない)でIDエッジが適切な位置に描画される。
        //
        // ユニラン
        // オフスクリーンサイズ256x256のとき、m_debugTexScaleVal(1.0,0.5)、m_debugTexTranslateVal(0.0,1.0)(Translateは(0,-1),(0,3)でもいいが、(0,0)はダメ)でIDエッジが適切な位置に描画される。
        // オフスクリーンサイズ128x128のとき、m_debugTexScaleVal(0.5,0.14くらい)、m_debugTexTranslateVal(0.0,-6.5くらい)でIDエッジがそれっぽい位置に描画される。
        //
        // m_debugTexScaleVal、m_debugTexTranslateValはAppOutLinePostRenderPath.cppのデバッグ調整用の変数。
        // AppOutLinePostRenderPath.cppのオフスクリーン部分が画像サイズ256x256前提でつくられてしまっている。
        //
        param.monsNo = (MonsNo)m;
        param.formNo = 1;
        m_pokeModel = m_pokeModelSys->CreateModelSimple(&param,opt);
        while( !m_pokeModel->IsFinishLoad() )
        {
          gfl2::thread::ThreadStatic::CurrentSleep(1);
        }
        m_pokeModel->Create();
       
        // camera
        app::util::AppRenderingManager::CreateDefaultCamera(m_appHeap, &m_drawEnvNodeOfCamera, &m_camera);
        gfl2::math::Vector3 pos(0,30,200);
        gfl2::math::Vector3 targetPos(0,30,0);
        gfl2::math::Vector3 upVec(0,1,0); 
        m_camera->SetupCameraLookAt(pos, targetPos, upVec);

        // light
        m_drawEnvNodeOfLight = app::util::AppRenderingManager::CreateDefaultDrawEnv(m_appHeap);
       
        // renderTargetTexture
        m_renderTargetTexture = GFL_NEW(m_uniMem) app::util::RenderTargetTexture();
        app::util::RenderTargetTexture::Description renderTargetTextureDesc;
        renderTargetTextureDesc.width  = 256;
        renderTargetTextureDesc.height = 256;
        //renderTargetTextureDesc.type   = app::util::RenderTargetTexture::Type::DEPTH_BUFFER_CREATE;
        m_renderTargetTexture->Create(
            m_vramAllocator,
            m_appHeap->GetDevGLHeapAllocator(),
            renderTargetTextureDesc
        );
        m_renderTargetTexture->SetClearColor(gfl2::gfx::Color(1,1,1,1));

        // set, add
        m_appRenderingManager->SetOffScreenRenderTargetTexture(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_renderTargetTexture);
        m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_camera);
        m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_camera);
        m_appRenderingManager->SetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_camera);
        m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_drawEnvNodeOfLight);
        m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_drawEnvNodeOfLight);
        m_appRenderingManager->AddDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_drawEnvNodeOfLight);
        m_appRenderingManager->AddPokeModel(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_pokeModel);
        m_appRenderingManager->AddPokeModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pokeModel);
        m_appRenderingManager->AddPokeModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pokeModel);
      }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST


      // フレーム
       
      // フレーム生成
      {
        m_frameCell = MENU_FRAME_CALL_MODAL<applib::frame::Manager, KawadaTestLayoutSampleFrameCell>(m_frameManager);
        KawadaTestLayoutSampleFrameCell::Param param;
        param.appHeap             = m_appHeap;
        param.gameManager         = m_gameManager;
        param.dataManager         = m_dataManager;
        param.frameManager        = m_frameManager;
        param.appRenderingManager = m_appRenderingManager;
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
        param.renderTargetTexture = m_renderTargetTexture;
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
        m_frameCell->Setup(param);
       
        m_frameCreated = true;
      }

      ++m_stepCount;
      //breakしない
    }
  case 4:
    {
      // AppCamera
      if(m_appCamera==NULL)
      {
        m_appCamera = GFL_NEW(m_uniMem) app::util::AppCamera();

        app::util::AppCamera::RenderPlaceType::Tag appCameraRenderPlaceType = app::util::AppCamera::RenderPlaceType::UPPER_DISPLAY;
        bool isPivot = false;  // GF_PLATFORM_CTRかつならtrue。
#if defined(GF_PLATFORM_CTR)
        isPivot = true;
#endif
        //m_appCamera->SetupOrthoCamera(appCameraRenderPlaceType, 40.0f, 500.0f, -500.0f);  // 40倍にするとエフェクトが大きく表示される
        //m_appCamera->SetupOrthoCamera(appCameraRenderPlaceType, 0.5f, 500.0f, -500.0f);  // 0.5倍にするとエフェクトが小さく表示される
        m_appCamera->SetViewMatrix(gfl2::math::Vector3(0,0,500), gfl2::math::Vector3(0,0,-500),0);
        m_appCamera->SetOrthoProjectionMatrix(400.0f/240.0f, 240, 0, 1000, isPivot);
        m_appRenderingManager->SetEffectAppCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_appCamera);
      }
      if(m_appCameraLower==NULL)
      {
        m_appCameraLower = GFL_NEW(m_uniMem) app::util::AppCamera();

        app::util::AppCamera::RenderPlaceType::Tag appCameraRenderPlaceType = app::util::AppCamera::RenderPlaceType::LOWER_DISPLAY;
        bool isPivot = false;  // GF_PLATFORM_CTRかつならtrue。
#if defined(GF_PLATFORM_CTR)
        isPivot = true;
#endif
        m_appCameraLower->SetupOrthoCamera(appCameraRenderPlaceType, 40.0f, 500.0f, -500.0f);  // 40倍にするとエフェクトが大きく表示される
        m_appCameraLower->SetupOrthoCamera(appCameraRenderPlaceType, 0.5f, 500.0f, -500.0f);  // 0.5倍にするとエフェクトが小さく表示される
        m_appCameraLower->SetViewMatrix(gfl2::math::Vector3(0,0,500), gfl2::math::Vector3(0,0,-500),0);
        m_appCameraLower->SetOrthoProjectionMatrix(320.0f/240.0f, 240, 0, 1000, isPivot);
        m_appRenderingManager->SetEffectAppCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_appCameraLower);
      }

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
      // エフェクト
      void* effectResBuf = m_dataManager->GetDataBuf(KawadaTestLayoutSampleDataManager::DATA_ID_SIMPLE_EFFECT_TEST);
      if(effectResBuf == NULL) return;
      
      m_effectHeap = GFL_NEW(m_uniMem) gfl2::Effect::GFHeap(m_devMem);

      gfl2::Effect::Config config;
      {
        config.SetEffectHeap( m_effectHeap );
#ifdef GF_PLATFORM_CTR
        config.SetParticleMemType(nw::eft::BUFFER_LOCATION_FCRAM);  // デフォルトがVRAMAになっているのでFCRAMに変える。
#endif //GF_PLATFORM_CTR
      }

      m_effectSys = GFL_NEW(m_uniMem) gfl2::Effect::System(config);

      m_effectGroupIDList = GFL_NEW(m_uniMem) gfl2::util::List<u32>( m_uniMem, 1 );
      m_effectGroupIDList->PushFront( m_effectGroupID );  // GroupID=0を追加

      {
        m_effectRenderPathConfig.m_pSystem       = m_effectSys;
        m_effectRenderPathConfig.m_cpGroupIDList = m_effectGroupIDList;
      }

      m_effectSys->EntryResource( m_effectHeap, effectResBuf, m_effectResID );

      {
        app::util::AppRenderingManager::CreateDefaultCamera(m_appHeap, &m_effectDrawEnvNodeOfCamera, &m_effectCamera);
        m_effectCamera->SetupCameraLookAt(gfl2::math::Vector3(0,5,10), gfl2::math::Vector3(0,0,0), gfl2::math::Vector3(0,1,0));
      }

      {
        m_appRenderingManager->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_effectRenderPathConfig);
        m_appRenderingManager->SetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_effectCamera);
        
        m_appRenderingManager->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_effectRenderPathConfig);
        m_appRenderingManager->SetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_effectCamera);
      }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
      // エフェクト
      void* effectResBuf = m_dataManager->GetDataBuf(KawadaTestLayoutSampleDataManager::DATA_ID_SIMPLE_EFFECT_TEST);
      if(effectResBuf == NULL) return;

      u32 effectGroupIDNumMaxArray[1] = { 1 };
      app::util::EffectUtil::Description desc;
      {
        desc.layerNum = 1;
        desc.effectResNumMax = 1;
  #ifdef USE_EFFECT_UTIL_MODE_1RES_1EMITTER_SET_1GROUP
        desc.mode = app::util::EffectUtil::MODE_1RES_1EMITTER_SET_1GROUP;
  #endif
        desc.effectGroupIDNumMaxArray = effectGroupIDNumMaxArray;
      }

      m_effectUtil = GFL_NEW(m_uniMem) app::util::EffectUtil(m_devMem, desc);

      m_effectUtil->EffectSystemEntryResource( effectResBuf, m_effectResID );

      m_effectUtil->GetData(0)->effectGroupIDList->PushFront( m_effectGroupID );  // GroupID=0を追加

      {
        app::util::AppRenderingManager::CreateDefaultCamera(m_appHeap, &m_effectDrawEnvNodeOfCamera, &m_effectCamera);
        m_effectCamera->SetupCameraLookAt(gfl2::math::Vector3(0,5,10), gfl2::math::Vector3(0,0,0), gfl2::math::Vector3(0,1,0));
      }

      {
        m_appRenderingManager->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_effectUtil->GetData(0)->effectRenderPathConfig);
        m_appRenderingManager->SetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_effectCamera);
        
        m_appRenderingManager->SetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_effectUtil->GetData(0)->effectRenderPathConfig);
        m_appRenderingManager->SetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_effectCamera);
      }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL

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
void KawadaTestLayoutSampleMain::updateFunc_Working(void)
{
  // 描画時は必ず行う更新
  this->updateForDraw();

  if(m_frameReqDestroy)
  {
    // 次へ
    this->changeStep(STEP_DESTROY);
  }
}
void KawadaTestLayoutSampleMain::updateFunc_Destroy(void)
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
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
        // エフェクト
        {
          m_appRenderingManager->UnsetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
          m_appRenderingManager->UnsetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
          
          m_appRenderingManager->UnsetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
          m_appRenderingManager->UnsetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
        }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
        // エフェクト
        {
          m_appRenderingManager->UnsetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
          m_appRenderingManager->UnsetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
          
          m_appRenderingManager->UnsetEffectRenderPathConfig(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
          m_appRenderingManager->UnsetEffectCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
        }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL

        // AppCamera
        if(m_appCameraLower)
        {
          m_appRenderingManager->UnsetEffectAppCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
          GFL_SAFE_DELETE(m_appCameraLower);
        }
        if(m_appCamera)
        {
          m_appRenderingManager->UnsetEffectAppCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
          GFL_SAFE_DELETE(m_appCamera);
        }

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
        // unset, remove
        m_appRenderingManager->RemovePokeModel(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_pokeModel);
        m_appRenderingManager->RemovePokeModel(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_pokeModel);
        m_appRenderingManager->RemovePokeModel(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_pokeModel);
        m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN, m_drawEnvNodeOfLight);
        m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY, m_drawEnvNodeOfLight);
        m_appRenderingManager->RemoveDrawEnv(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY, m_drawEnvNodeOfLight);
        m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::LOWER_DISPLAY);
        m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::UPPER_DISPLAY);
        m_appRenderingManager->UnsetCamera(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN);
        m_appRenderingManager->UnsetOffScreenRenderTargetTexture(app::util::AppRenderingManager::RenderPlaceType::OFF_SCREEN);
     
        GFL_SAFE_DELETE(m_renderTargetTexture);
        
        GFL_SAFE_DELETE(m_drawEnvNodeOfLight);
        
        GFL_SAFE_DELETE(m_camera);
        GFL_SAFE_DELETE(m_drawEnvNodeOfCamera);

        m_pokeModelSys->DeleteModel(m_pokeModel);
        m_pokeModel = NULL;

        //ポケモン用ヒープ削除
        m_pokeModelSys->DeletePokeModelHeap();
        //システム開放開始
        m_pokeModelSys->TermSystemAsync();
        while( !m_pokeModelSys->IsFinishTermSystemAsync() )
        {
          gfl2::thread::ThreadStatic::CurrentSleep(1);
        } 
        GFL_SAFE_DELETE(m_pokeModelSys);
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST


        GFL_SAFE_DELETE(m_appRenderingManager);


#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
        GFL_SAFE_DELETE(m_vramAllocator);
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST


        ++m_stepCount;
      }
      break;
    }
  case 2:
    {
#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
      // エフェクト
      {
        // 1フレ遅れ解放になっていること！
        m_effectSys->KillAllEmitter();
        m_effectSys->ClearResource(m_effectHeap, m_effectResID);

        GFL_SAFE_DELETE(m_effectCamera);
        GFL_SAFE_DELETE(m_effectDrawEnvNodeOfCamera);

        GFL_SAFE_DELETE(m_effectGroupIDList);
        GFL_SAFE_DELETE(m_effectSys);
        GFL_SAFE_DELETE(m_effectHeap);
      }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
      // エフェクト
      {
  #ifdef USE_EFFECT_UTIL_MODE_1RES_1EMITTER_SET_1GROUP
        // 1フレ遅れ解放になっていること！
        m_effectUtil->KillAllEmitterSetForMode1Res1EmitterSet1Group();
  #else
        // 1フレ遅れ解放になっていること！
        m_effectUtil->GetEffectSystem()->KillAllEmitter();
  #endif
        m_effectUtil->EffectSystemClearResource(m_effectResID);

        GFL_SAFE_DELETE(m_effectCamera);
        GFL_SAFE_DELETE(m_effectDrawEnvNodeOfCamera);

        GFL_SAFE_DELETE(m_effectUtil);
      }
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL

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

void KawadaTestLayoutSampleMain::updateForDraw(void)
{
  // 特にやることはない
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

