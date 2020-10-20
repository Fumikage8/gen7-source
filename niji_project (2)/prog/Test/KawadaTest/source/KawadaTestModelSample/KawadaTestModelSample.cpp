//==============================================================================
/**
 @file    KawadaTestModelSample.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.23
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


#define KAWADA_TEST_MODEL_SAMPLE_BLOOM_ASYNC_CREATE (1)  // 1のときブルームを非同期生成する


// gfl2のインクルード
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfEnvData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceNode.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryShdData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryTexData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>
#include <Animation/include/gfl2_DrawEnvAnimationController.h>
#include <gfx/include/gfl2_GFGL.h>

#include <math/include/gfl2_math.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <fs/include/gfl2_Fs.h>

// poke_3dのインクルード
#include <model/include/gfl2_BaseModel.h>
#include <renderer/include/gfl2_BloomRenderPath.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/kawada_test_data_misc.gaix>

// nijiのインクルード
#include <System/include/nijiAllocator.h>
#include <System/include/GflUse.h>
#include <System/include/ArcIdGetter.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

// KawadaTestのインクルード
#include "KawadaTestModelSample.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


#if defined(GF_PLATFORM_CTR)
  static const u32 A_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_btl_G_kusa_GFBMDLP;  // パックはテクスチャとシェーダが込みのデータ。パックでないのはモデルのデータのみ。どちらにもアニメは入っていない。
  static const u32 B_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_btl_G_kusa_GFBMDLP;
  //static const u32 A_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_niji_btl_kusa01_GFBMDLP;
  //static const u32 B_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_niji_btl_kusa01_GFBMDLP;
  static const u32 ENV_RESOURCE_FILE_NAME     = GARC_kawada_test_data_misc_DrawEnvTest_v1_0_GFBENV;
  static const u32 BLOOM_ALPHA_MASK_TEX_RES_FILE_NAME = GARC_kawada_test_data_misc_bloom_alpha_mask_test_tex_ctr_BTEX;
  static const u32 ANIM_ENV_RESOURCE_FILE_NAME     = GARC_kawada_test_data_misc_DrawEnv1_v1_0_GFBENV;
  static const u32 ANIM_ENV_MOT_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_DrawEnv1_GFBMOT;
#elif defined(GF_PLATFORM_WIN32)
  static const u32 A_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_btl_G_kusa_GFBMDLP;
  static const u32 B_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_btl_G_kusa_GFBMDLP;
  //static const u32 A_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_niji_btl_kusa01_GFBMDLP;
  //static const u32 B_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_niji_btl_kusa01_GFBMDLP;
#if WIN32_IKEUCHI_FOG_TEST
  static const u32 C_MODEL_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_map_wingl_GFBMDLP;
#endif  // WIN32_IKEUCHI_FOG_TEST
  static const u32 ENV_RESOURCE_FILE_NAME     = GARC_kawada_test_data_misc_DrawEnvTest_v1_0_GFBENV;
  static const u32 ANIM_ENV_RESOURCE_FILE_NAME     = GARC_kawada_test_data_misc_DrawEnv1_v1_0_GFBENV;
  static const u32 ANIM_ENV_MOT_RESOURCE_FILE_NAME = GARC_kawada_test_data_misc_DrawEnv1_GFBMOT;
#endif  // GF_PLATFORM_CTR, GF_PLATFORM_WIN32


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//! @brief  テスト用のレンダリングパイプライン
class TestRenderingPipeLine : public gfl2::renderingengine::renderer::RenderingPipeLine
{
  GFL_FORBID_COPY_AND_ASSIGN(TestRenderingPipeLine);

public:
  //! @brief  コンストラクタ
  TestRenderingPipeLine(gfl2::util::GLHeapAllocator* glAllocator)
    : gfl2::renderingengine::renderer::RenderingPipeLine(),
      m_RenderPathCnt(0)
  {
    SceneRenderPath::InitDescription desc;
    m_SceneRenderPath.Initialize( glAllocator, desc );
  }

  virtual ~TestRenderingPipeLine()
  {}

private:
  virtual b32 StartRenderPath()
  {
    m_RenderPathCnt = 0;
    return true;
  }

  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath()
  {
    switch( m_RenderPathCnt ){
    case 0:
      return &m_SceneRenderPath;
      //returnするのでbreak要らない
    case 1:
      return &m_BloomRenderPath;
      //returnするのでbreak要らない
    case 2:
      //return &m_PresentPath;
      break;
    }
    return NULL;
  }

  virtual b32 NextRenderPath()
  {
    m_RenderPathCnt++;
    if ( m_RenderPathCnt == 3 )
      return false;
    return true;
  }

public:
  void AddDrawEnv(gfl2::renderingengine::scenegraph::instance::DrawEnvNode* pNode)
  {
    m_SceneRenderPath.AddDrawEnv(pNode);
  }

public:
  poke_3d::renderer::BloomRenderPath* GetBloomRenderPath(void)
  {
    return &m_BloomRenderPath;
  }

private:
  gfl2::renderingengine::renderer::RenderingPipeLine::SceneRenderPath   m_SceneRenderPath;
  poke_3d::renderer::BloomRenderPath                                    m_BloomRenderPath;
  gfl2::renderingengine::renderer::RenderingPipeLine::PresentPath       m_PresentPath;
  u32                                                                   m_RenderPathCnt;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


KawadaTestModelSample::KawadaTestModelSample(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
  : KawadaTestSampleBase(gameManager, parentHeapMem, parentDevMem),
    m_step(STEP_NONE),
    m_stepCount(0),
    m_glAllocator(NULL),
    m_nijiAllocator(NULL),
    m_envNode(NULL),
    m_envResourceBuf(NULL),
    m_envResourceNode(NULL),
    m_renderingPipeLine(NULL),
    m_bloomAlphaMaskTexResBuf(NULL),
    m_bloomAlphaMaskTexResNode(NULL),
    m_anEnvResourceBuf(NULL),
    m_anEnvResourceNode(NULL),
    m_anEnvMotResourceBuf(NULL),
    m_anEnvMotResourceNode(NULL),
    m_anEnvAnimCtrl(NULL)
{
  for(u32 i=0; i<MODEL_NUM; ++i)
  {
    m_model[i] = NULL;
  }
  for(u32 i=0; i<MODEL_RESOURCE_NUM; ++i)
  {
    m_modelResourceBuf[i]  = NULL;
    m_modelResourceNode[i] = NULL;
  }
  this->initialize(gameManager, parentHeapMem, parentDevMem);
}
KawadaTestModelSample::~KawadaTestModelSample()
{
  this->terminate();
}

void KawadaTestModelSample::Update(void)
{
  // ブルーム描画パス
  {
    if(m_renderingPipeLine)
    {
      poke_3d::renderer::BloomRenderPath* bloomRenderPath = m_renderingPipeLine->GetBloomRenderPath();
      if(bloomRenderPath)
      {
        bloomRenderPath->Update();
      }
    }
  }

  switch(m_step)
  {
  case STEP_NONE:    this->updateFunc_None();     break;
  case STEP_CREATE0: this->updateFunc_Create0();  break;
  case STEP_CREATE1: this->updateFunc_Create1();  break;
  case STEP_WORKING: this->updateFunc_Working();  break;
  case STEP_DESTROY: this->updateFunc_Destroy();  break;
  }
}

void KawadaTestModelSample::PreDraw(void)
{
  if(m_step == STEP_WORKING)
  {
    // 何もしない
  }
}

void KawadaTestModelSample::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  if(m_step == STEP_WORKING)
  {
    gfl2::gfx::DisplayDeviceHandle displayDeviceHandle = System::GflUse::m_HandleList[displayNo];
    
    gfl2::gfx::Surface* backBuffer         = gfl2::gfx::GFGL::GetBackBuffer(displayDeviceHandle);
    gfl2::gfx::Surface* depthStencilBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer(displayDeviceHandle);
    u32                 displayWidth       = gfl2::gfx::GFGL::GetDisplayWidth(displayDeviceHandle);
    u32                 displayHeight      = gfl2::gfx::GFGL::GetDisplayHeight(displayDeviceHandle);

    gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetPerspective(
      gfl2::math::ConvDegToRad(37.0f),
      static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight),
      1.1f,
      1000.0f,
      true
    );
    m_renderingPipeLine->SetProjectionMatrix(projMatrix);
  
#if WIN32_IKEUCHI_FOG_TEST
    gfl2::math::Vector3 eyePos( 100.0f, 100.0f, 1.0f );   // 視点(カメラの位置)
    gfl2::math::Vector3 aimPos( 100.0f, 100.0f, 0.0f );   // 注視点
#else
    gfl2::math::Vector3 eyePos( 80.0f, 60.0f, 150.0f );   // 視点(カメラの位置)
    gfl2::math::Vector3 aimPos( 0.0f, 40.0f, 0.0f );      // 注視点
#endif
    gfl2::math::Vector3 upDir( 0.0f, 1.0f, 0.0f );        // カメラの上方向
    gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetLookAt(eyePos, aimPos, upDir);
    m_renderingPipeLine->SetViewMatrix(viewMatrix);

    m_renderingPipeLine->SetRenderTarget(backBuffer);
    m_renderingPipeLine->SetDepthStencil(depthStencilBuffer);
    m_renderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
    
    m_renderingPipeLine->Execute();
  }
}

void KawadaTestModelSample::initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem)
{
  static const s32 HEAP_MEM_SIZE  =  0x100000 - 0x1000;
  static const s32 DEV_MEM_SIZE   = 0x1000000 - 0x1000;
 
  m_enableToEnd = false;
  m_gameManager = gameManager;

  m_heapMem     = GFL_CREATE_LOCAL_HEAP(parentHeapMem, HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_devMem      = GFL_CREATE_LOCAL_HEAP(parentDevMem, DEV_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_uniMem      = m_devMem;

  // 他はUpdateでつくることにする
  this->changeStep(STEP_CREATE0);
}

void KawadaTestModelSample::terminate(void)
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

void KawadaTestModelSample::changeStep(Step step)
{
  m_step      = step;
  m_stepCount = 0;
}

void KawadaTestModelSample::updateFunc_None(void)
{}

void KawadaTestModelSample::updateFunc_Create0(void)
{
  gfl2::fs::AsyncFileManager* romAsyncFileManager = m_gameManager->GetAsyncFileManager();

  // ファイルオープン
  app::util::FileAccessor::FileOpenSync(NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA_MISC), m_devMem, true);

  // Allocator
  {
    m_glAllocator   = GFL_NEW(m_uniMem) gfl2::util::GLHeapAllocator(m_devMem);
    m_nijiAllocator = GFL_NEW(m_uniMem) System::nijiAllocator(m_devMem);
  }
  
  // resource
  {
    // modelResource
    {
      static const u32 modelResourceFileName[MODEL_RESOURCE_NUM] =
      {
#if WIN32_IKEUCHI_FOG_TEST
        C_MODEL_RESOURCE_FILE_NAME,
        C_MODEL_RESOURCE_FILE_NAME,
        C_MODEL_RESOURCE_FILE_NAME,
#else
        A_MODEL_RESOURCE_FILE_NAME,
        B_MODEL_RESOURCE_FILE_NAME,
#endif
      };
      
      for(u32 i=0; i<MODEL_RESOURCE_NUM; ++i)
      {
        // ファイルを読み込みバッファに格納
        app::util::FileAccessor::FileLoadSync(
            NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA_MISC),
            modelResourceFileName[i],
            &m_modelResourceBuf[i],
            m_devMem,
            false,
            128
        );
        // リソースノード
        {
          gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack modelDataPack;
          modelDataPack.SetModelData(static_cast<c8*>(m_modelResourceBuf[i]));
          m_modelResourceNode[i] = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &modelDataPack);
        }
      }
    }

    // envResource
    {
      // ファイルを読み込みバッファに格納
      app::util::FileAccessor::FileLoadSync(
          NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA_MISC),
          ENV_RESOURCE_FILE_NAME,
          &m_envResourceBuf,
          m_devMem,
          false,
          128
      );
      // リソースノード
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData envData;
        envData.SetBinaryEnvData(static_cast<c8*>(m_envResourceBuf));
        m_envResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &envData);
      }
    }
  }

  // BaseModel
  {
#if 1
    // 違うリソースを使う
    gfl2::renderingengine::scenegraph::resource::ResourceNode* modelResourceNode[MODEL_NUM] =
    {
#if WIN32_IKEUCHI_FOG_TEST
      m_modelResourceNode[0],
      m_modelResourceNode[1],
      m_modelResourceNode[2],
#else
      m_modelResourceNode[0],
      m_modelResourceNode[1],
#endif
    };
#else
    // 同じリソースを使う
    gfl2::renderingengine::scenegraph::resource::ResourceNode* modelResourceNode[MODEL_NUM] =
    {
#if WIN32_IKEUCHI_FOG_TEST
      m_modelResourceNode[0],
      m_modelResourceNode[0],
      m_modelResourceNode[0],
#else
      m_modelResourceNode[0],
      m_modelResourceNode[0],
#endif
    };
#endif

    class SRT
    {
    public:
      gfl2::math::Vector3 scale;
      gfl2::math::Vector3 rotation;  // [radian]
      gfl2::math::Vector3 position;
    public:
      SRT(void)
      {
        scale    = gfl2::math::Vector3(1, 1, 1);
        rotation = gfl2::math::Vector3(0, 0, 0);
        position = gfl2::math::Vector3(0, 0, 0);
      }
      SRT(
          f32 scaleX   , f32 scaleY   , f32 scaleZ   ,
          f32 rotationX, f32 rotationY, f32 rotationZ,
          f32 positionX, f32 positionY, f32 positionZ
      )
      {
        scale    = gfl2::math::Vector3(scaleX   , scaleY   , scaleZ   );
        rotation = gfl2::math::Vector3(rotationX, rotationY, rotationZ);
        position = gfl2::math::Vector3(positionX, positionY, positionZ);
      }
    };

    static const SRT modelSRT[MODEL_NUM] = 
    {
#if WIN32_IKEUCHI_FOG_TEST
      SRT(   1,    1,    1, 0, 0,                  0, 0,     0, 0),
      SRT(0.2f, 0.2f, 0.2f, 0, 0, 90.0f/180.0f*3.14f, 0, 10.0f, 0),
      SRT(   1,    1,    1, 0, 0,                  0, 0,     0, 0),
#else
      SRT(   1,    1,    1, 0, 0,                  0, 0,     0, 0),
      SRT(0.2f, 0.2f, 0.2f, 0, 0, 90.0f/180.0f*3.14f, 0, 10.0f, 0),
#endif
    };

    for(u32 i=0; i<MODEL_NUM; ++i)
    {
      poke_3d::model::BaseModel* model = GFL_NEW(m_devMem) poke_3d::model::BaseModel;
      model->Create(m_glAllocator, m_devMem, modelResourceNode[i]);

      model->SetPosition(modelSRT[i].position);
      model->SetRotation(modelSRT[i].rotation);
      model->SetScale(modelSRT[i].scale);

      u32 materialCount = model->GetMaterialCount();
      for(u32 materialIndex=0; materialIndex<materialCount; ++materialIndex)
      {
        /*
        GFL_PRINT("materialIndex=%d: w=%d, e=%d, f=%d\n", materialIndex, model->IsMaterialDepthWriteEnable(materialIndex), model->IsMaterialDepthTestEnable(materialIndex), model->GetMaterialDepthTestFunction(materialIndex));
        // CTR版に関するコメント。
        //     デプステストはマテリアルにStateObjectのインスタンスをつくるようにしたからいいが、
        //     それ以外はStateObjectのインスタンスがないのでNULLアクセスになる。2015.04.30現在。
        //     デプステストの初期値をStateObjectのインスタンスに反映しているわけではないので、
        //     値をゲットしてもグラフィッカーさん設定値がいくつだったかを知ることはできない。2015.04.30現在。
        */

        /*
        // BaseModelの関数を使ったデプステスト
        model->SetMaterialDepthTestFunction(materialIndex, poke_3d::model::BaseModel::MATERIAL_TEST_FUNCTION_NEVER);
        // UpdateStateを呼んでいないので反映されない。2015.04.30現在。
        */

        gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*    modelInstanceNode    = model->GetModelInstanceNode();
        gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* materialInstanceNode = modelInstanceNode->GetMaterialInstanceNode(materialIndex);

        if(i==0)
        {
          // デプステスト
          gfl2::gfx::DepthStencilStateObject* depthStencilStateObject = materialInstanceNode->GetDepthStencilStateObject();
          depthStencilStateObject->SetDepthWriteEnable(true);
          depthStencilStateObject->SetDepthTestEnable(true);
          depthStencilStateObject->SetDepthFunc(gfl2::gfx::CompareFunc::Never);
          depthStencilStateObject->UpdateState();  // UpdateStateを呼んで反映される
        }

        /*
        // ブレンド
        gfl2::gfx::BlendStateObject* blendStateObject = materialInstanceNode->GetBlendStateObject();
        blendStateObject->SetBlendEnable(true);
        blendStateObject->SetBlendOp(gfl2::gfx::BlendOp::Add, gfl2::gfx::BlendOp::Add);
        blendStateObject->SetBlendFunc(gfl2::gfx::BlendFunc::Zero, gfl2::gfx::BlendFunc::Zero, gfl2::gfx::BlendFunc::Zero, gfl2::gfx::BlendFunc::Zero);
        blendStateObject->UpdateState();  // UpdateStateを呼んで反映される
        // CTR版に関するコメント。
        //     変更した分のコマンドを発行していないので反映されない。2015.04.30現在。
        */

        // ライトセット番号
        //GFL_PRINT("%d %d\n", materialIndex, materialInstanceNode->GetMaterialResourceNode()->GetAttributeParam().m_LightSetNo);
      }

      m_model[i] = model;
    }
  }

  // RenderingPipeLine
  {
    m_renderingPipeLine = GFL_NEW(m_uniMem) TestRenderingPipeLine(m_glAllocator);
  }

  // ブルーム描画パス
  {
    poke_3d::renderer::BloomRenderPath::Description desc;
    {
      desc.sourceType                               = poke_3d::renderer::BloomRenderPath::SOURCE_TYPE_FRAME_BUFFER;
      desc.bloomType                                = poke_3d::renderer::BloomRenderPath::BLOOM_TYPE_GAUSSIAN_FILTER_3X3;
      desc.highLuminanceRenderTargetReciprocalScale = 2;
      desc.highLuminanceCopyNumMax                  = 1;
      desc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::LEFT ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::LEFT ] );
      desc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::RIGHT] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::RIGHT] );
      desc.sourceFrameBuffer[gfl2::gfx::CtrDisplayNo::DOWN ] = gfl2::gfx::GFGL::GetBackBuffer( System::GflUse::m_HandleList[gfl2::gfx::CtrDisplayNo::DOWN ] );
    }

    poke_3d::renderer::BloomRenderPath* bloomRenderPath = m_renderingPipeLine->GetBloomRenderPath();

#if KAWADA_TEST_MODEL_SAMPLE_BLOOM_ASYNC_CREATE==0
    // 同期
    bloomRenderPath->SyncCreate(
        m_devMem,
        m_devMem->GetLowerHandle(),
        m_nijiAllocator,
        m_nijiAllocator,
        romAsyncFileManager,
        desc
    );
#else
    // 非同期
    bloomRenderPath->StartAsyncCreate(
        m_devMem,
        m_devMem->GetLowerHandle(),
        m_nijiAllocator,
        m_nijiAllocator,
        romAsyncFileManager,
        desc
    );
#endif
  }

  // 次へ
  this->changeStep(STEP_CREATE1);
}

void KawadaTestModelSample::updateFunc_Create1(void)
{
  // ブルーム描画パス
  {
    poke_3d::renderer::BloomRenderPath* bloomRenderPath = m_renderingPipeLine->GetBloomRenderPath();
#if KAWADA_TEST_MODEL_SAMPLE_BLOOM_ASYNC_CREATE==0
    // 同期
    // 生成済み
#else
    // 非同期
    if(bloomRenderPath->IsCreated())
    {
      // 生成済み
    }
    else
    {
      // 生成中
      return;
    }
#endif

    bloomRenderPath->SetStrength(1.0f);
    bloomRenderPath->SetIntensity(0.66f);
    bloomRenderPath->SetWeight( gfl2::math::Vector4(0.2989f, 0.5866f, 0.1144f) );
    bloomRenderPath->SetEnable(true);

#if defined(GF_PLATFORM_CTR)
    // bloomAlphaMaskTexRes
    {
      // ファイルを読み込みバッファに格納
      app::util::FileAccessor::FileLoadSync(
          NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA_MISC),
          BLOOM_ALPHA_MASK_TEX_RES_FILE_NAME,
          &m_bloomAlphaMaskTexResBuf,
          m_devMem,
          false,
          128
      );
      // リソースノード
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryTexData texData;
        texData.SetTextureData(static_cast<c8*>(m_bloomAlphaMaskTexResBuf));
        m_bloomAlphaMaskTexResNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &texData);
      }
      // ブルーム描画パスに渡す
      gfl2::renderingengine::scenegraph::resource::TextureResourceNode* texResNode = poke_3d::renderer::BloomRenderPath::GetTextureResourceNode(m_bloomAlphaMaskTexResNode);
      bloomRenderPath->SetAlphaMaskTexture(texResNode->GetTexture());
    }
#endif  // GF_PLATFORM_CTR
  }



  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  // DrawEnv
#if 0
  ////////////////////////////////////////////////////////////////////////////////
  // ファイルから作成したものを使う
  // DrawEnv
  {
    m_envNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_glAllocator, m_envResourceNode);
    m_renderingPipeLine->AddDrawEnv(m_envNode);
  }

  /*
  // gfenvが0.8から1.0に変わったときのテスト
#if defined(GF_PLATFORM_WIN32)
  {
    //std::string ENV_FILE_NAME = "R:/home/momiji/resource/app_resource/resource/test/kawada_test/kawada_test_data/model_sample/win32/DrawEnv1_v1_0.gfenv";     // version=1.0
    std::string ENV_FILE_NAME = "R:/home/momiji/resource/app_resource/resource/test/kawada_test/kawada_test_data/model_sample/win32/DrawEnvTest_v0_8.gfenv";  // version=0.8
    gfl2::renderingengine::scenegraph::resource::GfEnvData     envData(ENV_FILE_NAME);
    gfl2::renderingengine::scenegraph::resource::ResourceNode* envResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &envData);
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode*  drawEnvNode     = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_glAllocator, envResourceNode);
    // 解放処理をきちんと書いていないので実際は使わないようにして下さい。読み込んで生成できるかどうかの確認にしか使っていない。
  }
#endif  // GF_PLATFORM_WIN32
  */

#elif 1
  ////////////////////////////////////////////////////////////////////////////////
  // プログラムで作成したものを使う
  // DrawEnv
  {
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* drawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_glAllocator);
    gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f));
    drawEnvNode->SetAABB(envSpace);

    // ライト
    {
      // ライトセット
      gfl2::renderingengine::scenegraph::instance::LightSetNode* lightSetNode;
      lightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(m_glAllocator);
      lightSetNode->SetSetNo( 0 );
     
      // ライト
      gfl2::renderingengine::scenegraph::instance::LightNode* directionalLightNode;
      directionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_glAllocator);
      directionalLightNode->SetNodeName( "DirectionalLight" );
      gfl2::util::FnvHash nameHash( reinterpret_cast<const u8*>(directionalLightNode->GetNodeName()) );
      directionalLightNode->SetNodeNameHash( nameHash.value );
      directionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
      gfl2::math::Vector3 pos = gfl2::math::Vector3(-10000000.0f, 10000000.0f, 10000000.0f);
      directionalLightNode->SetPosition( pos );
      gfl2::math::Vector3 direction = -pos.Normalize();
      directionalLightNode->SetDirection( direction );
      directionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) );
     
      // 追加
      lightSetNode->AddLight( directionalLightNode );
      drawEnvNode->AddLightSet( lightSetNode );
    }

    // フォグ
    {
      drawEnvNode->SetFogEnable( true );
      drawEnvNode->SetFogStrength( 1.0f );
      drawEnvNode->SetFogColor( gfl2::math::Vector4(1.0f, 1.0f, 1.0f) );
      drawEnvNode->SetFogNearLength( 1.0f );
      drawEnvNode->SetFogFarLength( 1000.0f );
    }

    gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild( drawEnvNode );
    m_renderingPipeLine->AddDrawEnv(drawEnvNode);

    m_envNode = drawEnvNode;
  }

#elif 0
  ////////////////////////////////////////////////////////////////////////////////
  // Envのアニメーション
  {
    // Env
    {
      // ファイルを読み込みバッファに格納
      app::util::FileAccessor::FileLoadSync(
          NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA_MISC),
          ANIM_ENV_RESOURCE_FILE_NAME,
          &m_anEnvResourceBuf,
          m_devMem,
          false,
          128
      );
      // リソースノード
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryEnvData anEnvData;
        anEnvData.SetBinaryEnvData(static_cast<c8*>(m_anEnvResourceBuf));
        m_anEnvResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &anEnvData);
      }
      // DrawEnv
      m_envNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_glAllocator, m_anEnvResourceNode);
    }

    // Envのモーション
    {
      // ファイルを読み込みバッファに格納
      app::util::FileAccessor::FileLoadSync(
          NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA_MISC),
          ANIM_ENV_MOT_RESOURCE_FILE_NAME,
          &m_anEnvMotResourceBuf,
          m_devMem,
          false,
          128
      );
      // リソースノード
      {
        gfl2::renderingengine::scenegraph::resource::GfBinaryMotData anEnvMotData;
        anEnvMotData.SetMotionData(static_cast<c8*>(m_anEnvMotResourceBuf));
        m_anEnvMotResourceNode = gfl2::renderingengine::scenegraph::resource::ResourceManager::CreateResourceNode(m_glAllocator, &anEnvMotData);
      }
    }
 
    // Envのアニメーションコントローラー
    {
      m_anEnvAnimCtrl = GFL_NEW(m_uniMem) gfl2::animation::DrawEnvAnimationController();
      m_anEnvAnimCtrl->Initialize(m_devMem, m_envNode);
 
      // アニメーションは30FPSで作られているが実行環境は60FPS  // ←サンプルをコピーしてきたので書いておいたが、これnijiでは当てはまらないかも？
      static const f32 SC_STEP_FRAME = 30.0f / 60.0f;

      // フォグアニメーション
      gfl2::animation::FogAnimationSlot* pFogSlot = m_anEnvAnimCtrl->GetFogSlot();
      pFogSlot->SetAnimationResource(m_anEnvMotResourceNode);
      pFogSlot->SetStepFrame(SC_STEP_FRAME);
      pFogSlot->SetLoop(true);
 
      // グレア(ブルーム)アニメーション
      gfl2::animation::GlareAnimationSlot* pGlareSlot = m_anEnvAnimCtrl->GetGlareSlot();
      pGlareSlot->SetAnimationResource(m_anEnvMotResourceNode);
      pGlareSlot->SetStepFrame(SC_STEP_FRAME);
      pGlareSlot->SetLoop(true);
    }
 
    // 設定
    {
      // レンダリングパイプラインに追加
      m_renderingPipeLine->AddDrawEnv(m_envNode);
 
      // ブルーム描画パスにアニメーションするEnvをセットする
      poke_3d::renderer::BloomRenderPath* bloomRenderPath = m_renderingPipeLine->GetBloomRenderPath();
      bloomRenderPath->SetDrawEnvNode(m_envNode);
    }
  }
#endif
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////



  // フォグパラメータをマテリアルに反映する
  this->setFogParamToMaterial();

  // ファイルクローズ
  app::util::FileAccessor::FileCloseSync(NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA_MISC));

  // 描画時は必ず行う更新
  this->updateForDraw();

  // 次へ
  this->changeStep(STEP_WORKING);
}

void KawadaTestModelSample::updateFunc_Working(void)
{
  const gfl2::ui::Button*       button   = m_gameManager->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  const gfl2::ui::VectorDevice* crossKey = m_gameManager->GetUiDeviceManager()->GetCrossKey(gfl2::ui::DeviceManager::CROSSKEY_STANDARD);
 
  // マテリアル更新
  {
    bool setNeverFlag  = false;
    bool setAlwaysFlag = false;
    bool resetFlag     = false;
    if( button->IsTrigger(gfl2::ui::BUTTON_A) )
    {
      setNeverFlag  = true;
    }
    else if( button->IsTrigger(gfl2::ui::BUTTON_Y) )
    {
      setAlwaysFlag = true;
    }
    else if( button->IsTrigger(gfl2::ui::BUTTON_X) )
    {
      resetFlag     = true;
    }

    if(setNeverFlag || setAlwaysFlag || resetFlag)
    {
      poke_3d::model::BaseModel* model = m_model[0];
      u32 materialCount = model->GetMaterialCount();
      for(u32 materialIndex=0; materialIndex<materialCount; ++materialIndex)
      {
        gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*    modelInstanceNode    = model->GetModelInstanceNode();
        gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* materialInstanceNode = modelInstanceNode->GetMaterialInstanceNode(materialIndex);

        // デプステスト
        gfl2::gfx::DepthStencilStateObject* depthStencilStateObject = materialInstanceNode->GetDepthStencilStateObject();
        if(resetFlag)
        {
          depthStencilStateObject->ResetRenderStates();
        }
        else
        {
          depthStencilStateObject->SetDepthWriteEnable(true);
          depthStencilStateObject->SetDepthTestEnable(true);
          if(setNeverFlag)
          {
            depthStencilStateObject->SetDepthFunc(gfl2::gfx::CompareFunc::Never);
          }
          else  // setAlwaysFlag
          {
            depthStencilStateObject->SetDepthFunc(gfl2::gfx::CompareFunc::Always);
          }
        }
        depthStencilStateObject->UpdateState();  // UpdateStateを呼んで反映される
      } 
    }
  }

  // フォグ更新
  {
#if 0
    bool printFlag = false;
    f32 fogStrength = m_envNode->GetFogStrength();
    if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
    {
      fogStrength += 0.05f;
      printFlag = true;
    }
    else if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      fogStrength -= 0.05f;
      printFlag = true;
    }
    m_envNode->SetFogStrength(fogStrength);
    if(printFlag)
    {
      //GFL_PRINT("fogStrength=%f\n", m_envNode->GetFogStrength());
    }
#endif

#if 1
    bool printFlag = false;
    
    f32 nearDiff = 0.0f;
    f32 farDiff  = 0.0f;
    
    f32 fogNear = m_envNode->GetFogNearLength();
    f32 fogFar  = m_envNode->GetFogFarLength();

    if     (fogNear < 100.0f)    nearDiff = 1.0f;
    else if(fogNear < 1000.0f)   nearDiff = 10.0f;
    else if(fogNear < 10000.0f)  nearDiff = 100.0f;
    else if(fogNear < 100000.0f) nearDiff = 1000.0f;
    else                         nearDiff = 10000.0f;

    if     (fogFar < 100.0f)    farDiff = 1.0f;
    else if(fogFar < 1000.0f)   farDiff = 10.0f;
    else if(fogFar < 10000.0f)  farDiff = 100.0f;
    else if(fogFar < 100000.0f) farDiff = 1000.0f;
    else                        farDiff = 10000.0f;
 
    if( crossKey->IsRepeat(gfl2::ui::DIR_UP) )
    {
      fogNear += nearDiff;
      printFlag = true;
    }
    else if( crossKey->IsRepeat(gfl2::ui::DIR_DOWN) )
    {
      fogNear -= nearDiff;
      printFlag = true;
    }

    if( crossKey->IsRepeat(gfl2::ui::DIR_LEFT) )
    {
      fogFar -= farDiff;
      printFlag = true;
    }
    else if( crossKey->IsRepeat(gfl2::ui::DIR_RIGHT) )
    {
      fogFar += farDiff;
      printFlag = true;
    }

    if(fogNear >= fogFar) fogFar = fogNear + 1.0f;

    m_envNode->SetFogNearLength(fogNear);
    m_envNode->SetFogFarLength(fogFar);
    if(printFlag)
    {
      GFL_PRINT("fog near=%f, far=%f\n", m_envNode->GetFogNearLength(), m_envNode->GetFogFarLength());
    }
#endif
  }

  // 描画時は必ず行う更新
  this->updateForDraw();

  if( button->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    // 次へ
    this->changeStep(STEP_DESTROY);
  }
}
void KawadaTestModelSample::updateFunc_Destroy(void)
{
  if(m_stepCount == 0)
  {
    // @todo 非表示にして、1フレ遅れ解放に対応させたい。
    ++m_stepCount;
  }
  else
  {
    // 破棄

    // Envのアニメーション
    GFL_SAFE_DELETE(m_anEnvAnimCtrl);
    GFL_SAFE_DELETE(m_anEnvMotResourceNode);
    GflHeapSafeFreeMemory(m_anEnvMotResourceBuf);
    GFL_SAFE_DELETE(m_anEnvResourceNode);
    GflHeapSafeFreeMemory(m_anEnvResourceBuf);

    // bloomAlphaMaskTexRes
    GFL_SAFE_DELETE(m_bloomAlphaMaskTexResNode);
    GflHeapSafeFreeMemory(m_bloomAlphaMaskTexResBuf);

    // RenderingPipeLine
    GFL_SAFE_DELETE(m_renderingPipeLine);

    // DrawEnv
    GFL_SAFE_DELETE(m_envNode);  // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも破棄されるらしい

    // BaseModel
    for(u32 i=0; i<MODEL_NUM; ++i)
    {
      m_model[i]->Destroy();
      GFL_SAFE_DELETE(m_model[i]);
    } 

    // resource
    GFL_SAFE_DELETE(m_envResourceNode);
    GflHeapSafeFreeMemory(m_envResourceBuf);
    for(u32 i=0; i<MODEL_RESOURCE_NUM; ++i)
    {
      GFL_SAFE_DELETE(m_modelResourceNode[i]);
      GflHeapSafeFreeMemory(m_modelResourceBuf[i]);
    }

    // Allocator
    GFL_SAFE_DELETE(m_nijiAllocator);
    GFL_SAFE_DELETE(m_glAllocator);

    // 次へ
    m_enableToEnd = true;
    this->changeStep(STEP_NONE);
  }
}

void KawadaTestModelSample::updateForDraw(void)
{
  // Envのアニメーションコントローラー更新
  if(m_anEnvAnimCtrl) m_anEnvAnimCtrl->Update();


  // フォグ
  {
    gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetPerspective(
      gfl2::math::ConvDegToRad(37.0f),
      static_cast<f32>(400) / static_cast<f32>(240),
      1.1f,
#if WIN32_IKEUCHI_FOG_TEST
      50000.0f,
#else
      1000.0f,
#endif
      true
    );

    gfl2::renderingengine::scenegraph::instance::DrawEnvNode::FogOuterParam param;
    param.projMatrix = projMatrix;

    // 他は変更していないので
    //   ・Zバッファ
    //   ・Zバッファなのでカメラのニアクリップ距離、ファークリップ距離は使わない
    //   ・深度バッファの範囲はデフォルトから変更しない
    // となっている。

    //Wバッファのテスト
    //param.cameraNear     = 1.0f;
    //param.cameraFar      = 1000.0f;
    //param.depthRangeNear = 0.0f;
    //param.depthRangeFar  = 1.0f;
    //param.wScale         = 1.0f/param.cameraFar;
    
    m_envNode->UpdateFog(param);
    for(u32 i=0; i<MODEL_NUM; ++i)
    {
      poke_3d::model::BaseModel* model = m_model[i];
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* modelInstanceNode = model->GetModelInstanceNode();
      /*bool isUpdated =*/ m_envNode->UpdateMaterialFog(modelInstanceNode);  // envが複数ある場合は、envでループを回してtrueが返ってきたら抜ける。
    }
  }


  {
    // ワールド行列更新、この中でシーングラフを辿ってローカル行列からワールド行列を生成しています
    // シーン全体で一回呼ぶだけで大丈夫です
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
  }
}

void KawadaTestModelSample::setFogParamToMaterial(void)
{
  for(u32 i=0; i<MODEL_NUM; ++i)
  {
    poke_3d::model::BaseModel* model = m_model[i];
    
    u32 materialCount = model->GetMaterialCount();
    for(u32 materialIndex=0; materialIndex<materialCount; ++materialIndex)
    {
      gfl2::renderingengine::scenegraph::instance::ModelInstanceNode*    modelInstanceNode    = model->GetModelInstanceNode();
      gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* materialInstanceNode = modelInstanceNode->GetMaterialInstanceNode(materialIndex);

      //GFL_PRINT("%d %d\n", materialIndex, materialInstanceNode->GetMaterialResourceNode()->GetAttributeParam().m_FogEnable);

   //   gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*                 materialResourceNode =
    //      const_cast<gfl2::renderingengine::scenegraph::resource::MaterialResourceNode*>(materialInstanceNode->GetMaterialResourceNode());
      /*gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* materialResourceNodeAttributeParam =
          materialResourceNode->GetAttributeParam();*/
      //materialResourceNodeAttributeParam->m_FogEnable = true;  // @todo どちらを使えばいいのか？→こっちじゃないっぽい。  //kawa20150507

      gfl2::renderingengine::scenegraph::resource::MaterialResourceNode::AttributeParam* materialInstanceNodeAttributeParam =
          materialInstanceNode->GetAttributeParam();
      materialInstanceNodeAttributeParam->m_FogEnable = true;  // @todo どちらを使えばいいのか？→こっちっぽい。  //kawa20150507
    }
  }
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

