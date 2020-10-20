//======================================================================
/**
 * @file OteireTestProc.cpp
 * @date 2015/10/29 8:42:40
 * @author araki_syo
 * @brief お手入れ用テスト
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if (PM_DEBUG)

#include "../include/OteireTestProc.h"
#include "System/include/Screenshot.h"

#include <arc_def_index/arc_def.h>
#include <niji_conv_header/battle/chara/chara_model_id.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <debug/include/gfl2_DebugWinItemTypes.h>

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(OteireTest)

/**
  * @brief constructor
  *
  */
OteireTestProc::OteireTestProc() : m_InitSeq(0), m_UpdateSeq(0), m_EndSeq(0), m_MonsNo(1), m_FormNo(0), m_Sex(0), m_Rare(false), m_pHeap(NULL), m_pRenderingPipeLine(NULL)
{
}

/**
 * @brief destructor
 *
 */
OteireTestProc::~OteireTestProc()
{
}

//------------------------------------------------------------------
/**
 * @brief   PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
 */
//------------------------------------------------------------------

gfl2::proc::Result OteireTestProc::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch(m_InitSeq)
  {
  case 0:
    {
      // Heap, Allocator初期化
      gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME(pDevHeap, 0x1000000, gfl2::heap::HEAP_TYPE_EXP, false, "OteireTest"); 
      m_pAllocator = GFL_NEW(m_pHeap) System::nijiAllocator(m_pHeap);

      // RenderingPipeLine初期化
      {
        m_pRenderingPipeLine = GFL_NEW(m_pHeap) Test::OteireTest::OteireTestRenderingPipeline();
        gfl2::fs::AsyncFileManager* pFileManager = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager();
        m_pRenderingPipeLine->Initialize(pFileManager, m_pHeap, m_pAllocator);
      }

      ++m_InitSeq;
    }
    break;

  case 1:
    {
      //PokeModelSystem作成
      m_pPokeModelSys = GFL_NEW(m_pHeap)PokeTool::PokeModelSystem();
      m_pPokeModelSys->InitSystemAsync(m_pHeap,m_pHeap,GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(),POKE_NUM);

      // CharaModelFactory初期化
      m_CharaModelFactory.Initialize(GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager(), m_pHeap, ARCID_CHARA_MODEL_BATTLE_NORMAL);
      m_CharaModelFactory.LoadModelAsync(CHARA_MODEL_ID_BTOB0217_00);

      ++m_InitSeq;
    }
    // break; // スルー

  case 2:
    {
      if( !m_pPokeModelSys->IsFinishInitSystemAsync() ||
          !m_CharaModelFactory.IsModelLoaded(CHARA_MODEL_ID_BTOB0217_00))
      {
        return gfl2::proc::RES_CONTINUE;
      }

      //PokeModelSystem初期化
      m_pPokeModelSys->CreateSystem(false, NULL);

      //管理ヒープ作成
      {
        PokeTool::PokeModelSystem::HeapOption opt;
        opt.animeType = PokeTool::MODEL_ANIMETYPE_KAWAIGARI;
        opt.useIdModel = false;
        opt.useShadow = true;
        opt.useColorShader = true;
        m_pPokeModelSys->CreatePokeModelHeap(m_pHeap,&opt);
      }

      // ボール作成
      m_CharaModelFactory.SetupModel(m_pAllocator, CHARA_MODEL_ID_BTOB0217_00);
      m_CharaModelFactory.CreateModel(m_pAllocator, m_pHeap, &m_CharaModel, CHARA_MODEL_ID_BTOB0217_00);

      // 深度テストを常時パスするよう設定
      for (u32 i = 0; i < m_CharaModel.GetMaterialCount(); ++i)
      {
        m_CharaModel.SetMaterialDepthTestEnable(i, false);
        m_CharaModel.SetMaterialDepthTestFunction(i, poke_3d::model::BaseModel::MATERIAL_TEST_FUNCTION_ALWAYS);
      }

      // ボールの描画順を最後に設定
      m_CharaModel.GetModelInstanceNode()->SetSortPriority(0x00);

      // ボールを描画
      m_pRenderingPipeLine->AddNode(m_CharaModel.GetModelInstanceNode());
      AddDefaultDrawEnv();

      // デバッグメニュー初期化
      InitDebug();

      result = gfl2::proc::RES_FINISH;
    }
    break;
  }

  return result;
}

/**
 * @brief	  PROCのメイン更新を行う関数
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
gfl2::proc::Result OteireTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  m_pPokeModelSys->Update();

  m_pRenderingPipeLine->Update();
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModelFast();

  char filename[255];
  gfl2::math::Vector3 waistposition;

  if(m_pPokeModel && m_pPokeModel->IsCreated())
  {
    // Waistジョイントの位置を取得
    waistposition = m_pPokeModel->GetJointWorldPosition(m_pPokeModel->GetJointIndex("Waist"));

    // ボールをWaist位置に移動
    m_CharaModel.SetPosition(waistposition);
  }

  switch(m_UpdateSeq)
  {
  case 0:

    if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
    {
      return gfl2::proc::RES_FINISH;
    }

    if( pButton->IsTrigger( gfl2::ui::BUTTON_A ) )
    {
      //ポケモデル作成開始
      PokeTool::PokeModel::SetupOption opt;
      opt.dataHeap = NULL;
      opt.workHeap = m_pHeap;
      opt.useShadow = false;
      opt.animeType = PokeTool::MODEL_ANIMETYPE_KAWAIGARI;

      PokeTool::SimpleParam pokeParam;
      pokeParam.monsNo = (MonsNo)m_MonsNo;
      pokeParam.formNo = m_FormNo;
      pokeParam.sex    = (pml::Sex)m_Sex;
      pokeParam.isEgg  = false;
      pokeParam.isRare = m_Rare;
      pokeParam.perRand = 0;

      m_pPokeModel = m_pPokeModelSys->CreateModelSimple(&pokeParam,opt);

      m_UpdateSeq++;
    }

    break;

  case 1:
    if( m_pPokeModel->IsFinishLoad() == true )
    {
      //ポケモデル作成
      m_pPokeModel->Create();
      
      // ポケモンの描画順を先に設定
      m_pPokeModel->GetModelInstanceNode()->SetSortPriority(0xFF);

      // ポケモデル描画
      m_pRenderingPipeLine->AddNode(m_pPokeModel->GetModelInstanceNode());

      if( m_pPokeModel->GetShadowModel() )
      {
        m_pRenderingPipeLine->AddNode(m_pPokeModel->GetShadowModel()->GetModelInstanceNode());
      }

      m_pRenderingPipeLine->AddDrawEnv(m_pPokeModel->GetDrawEnvNode());
      m_pRenderingPipeLine->RemoveDrawEnv(m_pDrawEnvNode);

      m_pPokeModel->ChangeAnimationDirect(0);
      m_pPokeModel->SetAnimationIsLoop(true);

      ++m_UpdateSeq;
    }

    break;

  case 2:
    {
      ++m_UpdateSeq;
    }

    break;

  case 3:
    // 描画待ち
    {
      ++m_UpdateSeq;
    }

    break;

  case 4:
    // ボールの移動待ち
    {
      ++m_UpdateSeq;
    }

    break;

  case 5:
    // よくわかんないけどもう1フレーム待ち
    {
      ++m_UpdateSeq;
    }

    break;

  case 6:
    // スクリーンショット撮影
    {
      // Visual Studioはsprintf_s
#if defined(GF_PLATFORM_WIN32)
      sprintf_s(filename, "screenshot/%d.bmp", m_MonsNo);
#else
      sprintf(filename, "screenshot/%d.bmp", m_MonsNo);
#endif

#if DEBUG_ONLY_FOR_araki_syo
      System::Screenshot::PrintScreen(filename, gfl2::gfx::DisplayMode::CTR_DOWN);
#endif

      ++m_UpdateSeq;
    }

    break;

  case 7:
    // ポケモデル描画停止
    if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      m_pRenderingPipeLine->RemoveDrawEnv(m_pPokeModel->GetDrawEnvNode());
      m_pRenderingPipeLine->AddDrawEnv(m_pDrawEnvNode);

      m_pPokeModel->SetVisible(false);
      m_pRenderingPipeLine->RemoveNode(m_pPokeModel->GetModelInstanceNode());

      if(m_pPokeModel->GetShadowModel())
      {
        m_pPokeModel->GetShadowModel()->SetVisible(false);
        m_pRenderingPipeLine->RemoveNode(m_pPokeModel->GetShadowModel()->GetModelInstanceNode());
      }

      ++m_UpdateSeq;
    }

    break;

  case 8:
    // ポケモデル解放
    if(m_pPokeModel->CanDelete())
    {
      m_pPokeModelSys->DeleteModel(m_pPokeModel);
      m_pPokeModel = NULL;

      ++m_UpdateSeq;
    }

    break;

  case 9:

    {
      // Visual Studioはsprintf_s
#if defined(GF_PLATFORM_WIN32)
      sprintf_s(filename, "screenshot/%d_waist.bmp", m_MonsNo);
#else
      sprintf(filename, "screenshot/%d_waist.bmp", m_MonsNo);
#endif

#if DEBUG_ONLY_FOR_araki_syo
      System::Screenshot::PrintScreen(filename, gfl2::gfx::DisplayMode::CTR_DOWN);
#endif

      m_UpdateSeq = 0;
    }

    break;
  }

  return result;
}

/**
 * @brief	PROCの描画処理(下準備)
 * @param pManager   プロセス管理マネージャー
 * 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
 * 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
 */
void OteireTestProc::PreDrawFunc (gfl2::proc::Manager* pManager)
{
}

/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
void OteireTestProc::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
  gfl2::gfx::DisplayDeviceHandle displayHandle = System::GflUse::m_HandleList[displayNo];
  u32 displayWidth = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
  u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);

  // 射影変換行列（パースペクティブ（透視法）射影）
  gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetPerspective(
    gfl2::math::ConvDegToRad(37.0f),
    static_cast<f32>(displayWidth) / static_cast<f32>(displayHeight),
    1.1f,
    1000.0f,
    true);

  m_pRenderingPipeLine->SetProjectionMatrix(projMatrix);

  // ビュー変換行列
  gfl2::math::Vector3 eyePosition(0.0f, 100.0f, 250.0f);
  gfl2::math::Vector3 focusPosition(0.0f, 80.0f, 0.0f);
  gfl2::math::Vector3 upDirection(0.0f, 1.0f, 0.0f);

  gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

  m_pRenderingPipeLine->SetViewMatrix(viewMatrix);

  gfl2::gfx::Surface* pBackBuffer = gfl2::gfx::GFGL::GetBackBuffer(displayHandle);
  gfl2::gfx::Surface* pDepthBuffer = gfl2::gfx::GFGL::GetDepthStencilBuffer(displayHandle);

  // 描画
  m_pRenderingPipeLine->SetRenderTarget(pBackBuffer);
  m_pRenderingPipeLine->SetDepthStencil(pDepthBuffer);
  m_pRenderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
  m_pRenderingPipeLine->Execute();
}

/**
 * @brief   PROCのメインを行う関数
 *  1/30フレームの②フレーム目の処理です。
 *  1/60フレーム動作時には、使われません。
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
gfl2::proc::Result OteireTestProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
 * @brief   PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
gfl2::proc::Result OteireTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch(m_EndSeq)
  {
  case 0:
    {
      // デバッグメニュー破棄
      TermDebug();

      // ボール描画停止
      m_CharaModel.SetVisible(false);
      m_pRenderingPipeLine->RemoveNode(m_CharaModel.GetModelInstanceNode());
      ++m_EndSeq;
    }

  case 1:
    {
      if(!m_CharaModel.CanDestroy())
      {
        return gfl2::proc::RES_CONTINUE;
      }

      // PokeModelSystem破棄
      m_pPokeModelSys->DeletePokeModelHeap();
      m_pPokeModelSys->TermSystemAsync();

      // CharaModel破棄
      m_CharaModel.Destroy();

      // CharaModelFactory破棄
      m_CharaModelFactory.UnloadModel(CHARA_MODEL_ID_BTOB0217_00);
      m_CharaModelFactory.Finalize();

      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = ARCID_RENDERPATH;
        closeReq.heapForReq = m_pHeap->GetLowerHandle();
        GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->AddArcFileCloseReq(closeReq);
      }

      ++m_EndSeq;
    }
    // break;

  case 2:
    if( m_pPokeModelSys->IsFinishTermSystemAsync()&&
      GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetAsyncFileManager()->IsArcFileCloseFinished(ARCID_RENDERPATH) )
    {
      GFL_SAFE_DELETE(m_pPokeModelSys);

      m_pRenderingPipeLine->Finalize();

      GFL_SAFE_DELETE(m_pRenderingPipeLine);
      GFL_SAFE_DELETE(m_pDrawEnvNode);

      GFL_SAFE_DELETE(m_pAllocator);
      GFL_DELETE_HEAP(m_pHeap);

      result = gfl2::proc::RES_FINISH;
    }
    break;
  }

  return result;
}

void OteireTestProc::AddDefaultDrawEnv()
{
  m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_pAllocator);

  {
    gfl2::renderingengine::scenegraph::instance::LightSetNode *pLightSetNode;
    gfl2::renderingengine::scenegraph::instance::LightNode *pDirectionalLightNode;
    gfl2::renderingengine::scenegraph::instance::LightNode *pAmbientLightNode;

    pLightSetNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(m_pAllocator);
    pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pAllocator);
    pAmbientLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pAllocator);

    {
      pDirectionalLightNode->SetNodeName("DirectionalLight02");
      gfl2::util::FnvHash nameHash(reinterpret_cast<const u8*>(pDirectionalLightNode->GetNodeName()) );
      pDirectionalLightNode->SetNodeNameHash(nameHash.value);
      pDirectionalLightNode->SetLightType(gfl2::renderingengine::LightType::Directional);
      gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
      pDirectionalLightNode->SetPosition(position);
      gfl2::math::Vector3 direction = -position.Normalize();
      pDirectionalLightNode->SetDirection(direction);
      pDirectionalLightNode->SetColor(gfl2::math::Vector3(0.7f, 0.7f, 0.7f));
    }

    {
      pAmbientLightNode->SetNodeName("AmbientLight02");
      gfl2::util::FnvHash nameHash(reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName()) );
      pAmbientLightNode->SetNodeNameHash(nameHash.value);
      pAmbientLightNode->SetLightType(gfl2::renderingengine::LightType::Ambient);
      pAmbientLightNode->SetColor(gfl2::math::Vector3(1.0f, 1.0f, 1.0f));
    }

    pLightSetNode->SetSetNo(2);
    pLightSetNode->AddLight(pDirectionalLightNode);
    pLightSetNode->AddLight(pAmbientLightNode);
    m_pDrawEnvNode->AddLightSet(pLightSetNode);
  }

  gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f));
  m_pDrawEnvNode->SetAABB(envSpace);

  gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(m_pDrawEnvNode);
  m_pRenderingPipeLine->AddDrawEnv(m_pDrawEnvNode);
}

void OteireTestProc::InitDebug(void)
{
  gfl2::debug::DebugWinGroup *topGroup = gfl2::debug::DebugWin_AddGroup(m_pHeap, L"お手入れテスト",NULL);
  gfl2::debug::DebugWinItem* item;

  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"MonsNo", &m_MonsNo, 1, MONSNO_MAX);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"FormNo", &m_FormNo, 0, 27);
  item = gfl2::debug::DebugWin_AddItemValueInt(topGroup, m_pHeap, L"Sex", &m_Sex, 0, 2);
  item = gfl2::debug::DebugWin_AddItemValueBool(topGroup, m_pHeap, L"Rare", &m_Rare);

  gfl2::debug::DebugWin_SetCurrentItem(topGroup);
}

void OteireTestProc::TermDebug(void)
{
  gfl2::debug::DebugWin_RemoveGroupByName(L"お手入れテスト");
}

GFL_NAMESPACE_END(OteireTest)
  GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
