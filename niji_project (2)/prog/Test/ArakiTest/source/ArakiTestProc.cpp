//======================================================================
/**
* @file  ArakiTestProc.cpp
* @date 15/05/15
* @author  araki_syo
* @brief  描画テスト
*/
//======================================================================

#if (PM_DEBUG)

// specify test target
#define MALE
#define FIELD

#include "../include/ArakiTestProc.h"

#include <GameSys/include/GameProcManager.h>

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include <arc_def_index/arc_def.h>

#include <System/include/Screenshot.h>
#include <gfx/include/gfl2_PCDebugUtil.h>

#include "System/include/DressUp.h"

// set arc id and model id
#if defined FEMALE
#define INITIAL_ITEM DRESSUP_ITEM_ID_P2_FACE01_OFF
#if defined FIELD
#include <niji_conv_header/field/chara/chara_model_id.h>
#define CHARA_ARCID_NORMAL ARCID_CHARA_MODEL_FIELD_NORMAL
#define CHARA_ARCID_DRESSUP System::DressUp::GetDressUpArcIdListField()
#define CHARA_MODEL_ID CHARA_MODEL_ID_PC0002_00
#else
#include <niji_conv_header/battle/chara/chara_model_id.h>
#define CHARA_ARCID_NORMAL ARCID_CHARA_MODEL_BATTLE_NORMAL
#define CHARA_ARCID_DRESSUP System::DressUp::GetDressUpArcIdListBattle()
#define CHARA_MODEL_ID CHARA_MODEL_ID_P2_BASE
#endif
#else
#define INITIAL_ITEM DRESSUP_ITEM_ID_P1_FACE01
#if defined FIELD
#include <niji_conv_header/field/chara/chara_model_id.h>
#define CHARA_ARCID_NORMAL ARCID_CHARA_MODEL_FIELD_NORMAL
#define CHARA_ARCID_DRESSUP System::DressUp::GetDressUpArcIdListField()
#define CHARA_MODEL_ID CHARA_MODEL_ID_PC0001_00
#else
#include <niji_conv_header/battle/chara/chara_model_id.h>
#define CHARA_ARCID_NORMAL ARCID_CHARA_MODEL_BATTLE_NORMAL
#define CHARA_ARCID_DRESSUP System::DressUp::GetDressUpArcIdListBattle()
#define CHARA_MODEL_ID CHARA_MODEL_ID_P1_BASE
#endif
#endif

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(ArakiTest)

  using namespace poke_3d::model;

/**
* @brief constructor
*
*/
ArakiTestProc::ArakiTestProc() :
  m_pHeap(NULL),
  m_pAllocator(NULL),
  m_InitSeq(0),
  m_UpdateSeq(0),
  m_EndSeq(0),
  m_TestFlag(false),
  m_ItemParamSeq(INITIAL_ITEM),
  m_BodyParamSeq(0),
  m_StartFreeSize(0),
  m_pRenderingPipeLine(NULL),
  m_pDrawEnvNode(NULL),
  buf(NULL)
{
}

/**
* @brief destructor
*
*/
ArakiTestProc::~ArakiTestProc()
{
}

//------------------------------------------------------------------
/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
*/
//------------------------------------------------------------------

gfl2::proc::Result ArakiTestProc::InitFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch (m_InitSeq)
  {
  case 0:
    {
      gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_DEBUG_DEBUGGER);
      m_pHeap = GFL_CREATE_LOCAL_HEAP(pDevHeap, 0x1000000, gfl2::heap::HEAP_TYPE_EXP, false);

      GFL_ASSERT(m_pHeap != NULL);
      m_StartFreeSize = m_pHeap->GetTotalFreeSize();

      m_pAllocator = GFL_NEW(m_pHeap) System::nijiAllocator(m_pHeap);

      m_pRenderingPipeLine = GFL_NEW(m_pHeap) gfl2::renderingengine::renderer::DefaultRenderingPipeLine(m_pAllocator);

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

      // キャラモデルファクトリー初期化、フィールド用かバトル用のARCIDを指定する
      // arc_def_index/arc_def.hをインクルードしておいてください
      GFL_PRINT("Initialize CharaModelFactory\n");
      m_CharaModelFactory.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pHeap, CHARA_ARCID_NORMAL);

      m_DressUpModelResourceManager.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pHeap, CHARA_ARCID_DRESSUP);

      // リソースを非同期読み込みする
      // niji_conv_header/chara/chara_model_id.hをインクルードしておいてください
      GFL_PRINT("Load Model\n");
      m_CharaModelFactory.LoadModelAsync(CHARA_MODEL_ID);

      // 着せ替えモデル用のパーツを非同期読み込みする
      // 適当な着せ替え設定
#if defined FEMALE
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = DRESSUP_COLOR_ID_SKIN_SWH;    // 体の色      
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = DRESSUP_COLOR_ID_EYE_EGR;     // 目の色
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = DRESSUP_COLOR_ID_HAIR_HLB;    // 髪の色
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = DRESSUP_COLOR_ID_LIP_LOF;    // リップの色
#else
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_SEX] = DressUpParam::SEX_MALE;
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = DRESSUP_COLOR_ID_SKIN_SWH;    // 体の色      
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = DRESSUP_COLOR_ID_EYE_EGR;     // 目の色
      m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = DRESSUP_COLOR_ID_HAIR_HLB;    // 髪の色
#endif

#if defined FEMALE
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_FACE] = DRESSUP_ITEM_ID_P2_FACE01_OFF;           // 顔
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = DRESSUP_ITEM_ID_P2_HAIR_BOB_ON;    // 髪型
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;           // バッジアクセサリ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;             // アイアクセサリ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;            // ヘアアクセサリ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BAG] = DRESSUP_ITEM_ID_P2_BAG_BAG01_BE;                // バッグ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01_BK;               // バングル
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BODY] = -1;               // ボディ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = DRESSUP_ITEM_ID_P2_BOTTOMS_7DENIM01_BL;            // ボトムス
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAT] = DRESSUP_ITEM_ID_P2_HAT_CAP_CAP01;                // 帽子
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = DRESSUP_ITEM_ID_P2_LEGS_BAREFOOT01_LONG;               // レッグ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = DRESSUP_ITEM_ID_P2_SHOES_DSHOES01;              // シューズ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = DRESSUP_ITEM_ID_P2_TOPS_DCUT01_BK;               // トップス
#else
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_FACE] = DRESSUP_ITEM_ID_P1_FACE01;           // 顔
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = DRESSUP_ITEM_ID_P1_HAIR_MIDIUM;    // 髪型
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;           // バッジアクセサリ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = DRESSUP_ITEM_ID_P1_ACCEYE_METAL01;             // アイアクセサリ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;            // ヘアアクセサリ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BAG] = DRESSUP_ITEM_ID_P1_BAG_DRUCK01;                // バッグ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P1_BNGL_ZENBRESS01;               // バングル
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BODY] = -1;               // ボディ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = DRESSUP_ITEM_ID_P1_BOTTOMS_DPAN01_WH;            // ボトムス
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAT] = DRESSUP_ITEM_ID_P1_HAT_DCAP01;                // 帽子
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = DRESSUP_ITEM_ID_P1_LEGS_BAREFOOT01;               // レッグ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = DRESSUP_ITEM_ID_P1_SHOES_DSHOES01;              // シューズ
      m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = DRESSUP_ITEM_ID_P1_TOPS_STRIPE01_BK;               // トップス
#endif

      GFL_PRINT("Load Dress Up Parts\n");
      m_DressUpModelResourceManager.LoadDressUpPartsAsync(m_DressUpParam);

      ++m_InitSeq;
    }
    break;

  case 1:
    {
      if(!m_CharaModelFactory.IsModelLoaded(CHARA_MODEL_ID))
      {
        GFL_PRINT("Model Loaded\n");
        break;
      }

      if(!m_DressUpModelResourceManager.IsDressUpPartsLoaded(m_DressUpParam))
      {
        GFL_PRINT("Dress Up Parts Loaded\n");
        break;
      }

      // 読み込んだモデルをセットアップ
      GFL_PRINT("Set Up Model\n");
      m_CharaModelFactory.SetupModel(m_pAllocator, CHARA_MODEL_ID);

      // 読み込んだ着せ替えパーツをセットアップ
      GFL_PRINT("Set Up Dress Up Parts\n");
      m_DressUpModelResourceManager.SetupDressUpParts(m_pAllocator, m_DressUpParam);

      // 読み込んだリソースから着せ替えモデルを作成
      GFL_PRINT("Create Dress Up Model\n");
      m_CharaModelFactory.CreateDressUpModel(m_pAllocator, m_pHeap, &m_DressUpModel, &m_DressUpModelResourceManager, CHARA_MODEL_ID);

      // アニメーションを変更する
      //GFL_PRINT("Change Animeation\n");
      //m_DressUpModel.ChangeAnimation(P2_BASE_BA002_BALL01);

      // アニメーションデータのループフラグが設定されていなかったので強制的にループさせます。
      // 本来はデータに含まれているはずなので必要ありません
      m_DressUpModel.SetAnimationLoop(true);

      // 初期化完了！
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
gfl2::proc::Result ArakiTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  gfl2::ui::DeviceManager* pDeviceManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
  gfl2::ui::Button* pButton = pDeviceManager->GetButton(0);

  switch (m_UpdateSeq)
  {

  case 0:
    {
      if(pButton->IsHold(gfl2::ui::BUTTON_B))
      {
        return gfl2::proc::RES_FINISH;
      }

      // 画面のスクリーンショットを保存(L:左画面、R:右画面、A：下画面)
#if (PM_DEBUG)
      if(pButton->IsTrigger(gfl2::ui::BUTTON_L))
      {
        System::Screenshot::PrintScreen("screenshot/left.bmp", gfl2::gfx::DisplayMode::CTR_LEFT);
      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_R))
      {
        System::Screenshot::PrintScreen("screenshot/right.bmp", gfl2::gfx::DisplayMode::CTR_RIGHT);
      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        System::Screenshot::PrintScreen("screenshot/down.bmp", gfl2::gfx::DisplayMode::CTR_DOWN);
      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_X))
      {
        System::Screenshot::PrintDepthMap("screenshot/reldepth.bmp", gfl2::gfx::DisplayMode::CTR_LEFT);
        System::Screenshot::PrintAbsDepthMap("screenshot/absdepth.bmp", gfl2::gfx::DisplayMode::CTR_LEFT);
        System::Screenshot::PrintStencil("screenshot/stencil.bmp", gfl2::gfx::DisplayMode::CTR_LEFT);
      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
      {
        buf = GflHeapAllocMemoryAlign(m_pHeap, 3 * 512 * 256, 16);
        gfl2::gfx::GFGL::AddTransferLinearColorBufferCommand(buf);
        m_UpdateSeq += 7;
      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
      {
        buf = GflHeapAllocMemoryAlign(m_pHeap, 4 * 512 * 256, 16);
        gfl2::gfx::GFGL::AddTransferColorBufferCommand(buf);
        m_UpdateSeq += 8;
      }

#endif // PM_DEBUG

      // 着せ替えモデルを再読み込み
      if(pButton->IsTrigger(gfl2::ui::BUTTON_START))
      {
        m_TestFlag = true;
      }

      if(m_TestFlag)
      {
        m_DressUpModel.SetVisible(false);
        ++m_UpdateSeq;
        break;
      }

      break;
    }

  case 1:
    {
        // インスタンスを先に殺す
        GFL_PRINT("Destroy Dress Up Model\n");
        if ( !m_DressUpModel.CanDestroy() )
        {
          break;
        }

        m_DressUpModel.Destroy();
        ++m_UpdateSeq;
    }

  case 2:
    {
      if ( !m_DressUpModelResourceManager.CanUnloadDressUpParts(m_DressUpParam) )
      {
        break;
      }

      // 読み込んだ着せ替えリソースの開放
      GFL_PRINT("Unload Dress Up Parts\n");
      m_DressUpModelResourceManager.UnloadDressUpParts(m_DressUpParam); 
      ++m_UpdateSeq;
    }

  case 3:
    {
      // 読み込んだリソースの開放
      GFL_PRINT("Unload Model\n");
      m_CharaModelFactory.UnloadModel(CHARA_ARCID_NORMAL);

      // キャラモデルファクトリの終了処理
      GFL_PRINT("Finalize CharaModelFactory\n");
      m_CharaModelFactory.Finalize();
      m_DressUpModelResourceManager.Finalize();

      // キャラモデルファクトリー初期化、フィールド用かバトル用のARCIDを指定する
      // arc_def_index/arc_def.hをインクルードしておいてください
      GFL_PRINT("Initialize CharaModelFactory\n");
      m_CharaModelFactory.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pHeap, CHARA_ARCID_NORMAL);
      m_DressUpModelResourceManager.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pHeap, CHARA_ARCID_DRESSUP);

      // リソースを非同期読み込みする
      // niji_conv_header/chara/chara_model_id.hをインクルードしておいてください
      GFL_PRINT("Load Model\n");
      m_CharaModelFactory.LoadModelAsync(CHARA_MODEL_ID);

      // 着せ替えモデル用のパーツを非同期読み込みする
      // 適当な着せ替え設定
      SetItemParams();

      GFL_PRINT("Load Dress Up Parts\n");
      m_DressUpModelResourceManager.LoadDressUpPartsAsync(m_DressUpParam);

      ++m_UpdateSeq;
    }

  case 4:
    {
      // リソースの非同期読み込み終了確認待ち
      if (!m_CharaModelFactory.IsModelLoaded(CHARA_MODEL_ID))
      {
        break;
      }

      // 着せ替えパーツの非同期読み込み終了確認待ち
      if (!m_DressUpModelResourceManager.IsDressUpPartsLoaded(m_DressUpParam))
      {
        break;
      }

      // 読み込んだモデルをセットアップ
      m_CharaModelFactory.SetupModel(m_pAllocator, CHARA_MODEL_ID);

      // 読み込んだ着せ替えパーツをセットアップ
      GFL_PRINT("Set Up Dress Up Parts\n");
      m_DressUpModelResourceManager.SetupDressUpParts(m_pAllocator, m_DressUpParam);

      // 読み込んだリソースから着せ替えモデルを作成
      GFL_PRINT("Create Dress Up Model\n");
      m_CharaModelFactory.CreateDressUpModel(m_pAllocator, m_pHeap, &m_DressUpModel, &m_DressUpModelResourceManager, CHARA_MODEL_ID);

      // アニメーションを変更する
      //GFL_PRINT("Change Animeation\n");
      //m_DressUpModel.ChangeAnimation(P2_BASE_BA002_BALL01);

      // アニメーションデータのループフラグが設定されていなかったので強制的にループさせます。
      // 本来はデータに含まれているはずなので必要ありません
      m_DressUpModel.SetAnimationLoop(true);

      ++m_UpdateSeq;

      break;
    }

  case 5:
    {
      //GFL_PRINT("Update Animation\n");
      m_DressUpModel.UpdateAnimation();
      //GFL_PRINT("Traverse Model\n");
      gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();

      ++m_UpdateSeq;
      break;
    }

  case 6:
    {
      char buf[128];
#if defined(GF_PLATFORM_WIN32)
      sprintf_s(buf, sizeof(buf), "screenshot/%d.bmp", m_ItemParamSeq);
#else
      sprintf(buf, "screenshot/%d.bmp", m_ItemParamSeq);
#endif
      //System::Screenshot::PrintScreen(buf, gfl2::gfx::DisplayMode::CTR_LEFT);
      ++m_ItemParamSeq;
      m_UpdateSeq = 0;

      break;
    }

  case 7:
    {
      gfl2::gfx::PCDebugUtil::PrintImage(buf, "screenshot/colorbuffer_linear.bmp", 512, 256, 3, m_pHeap);
      GflHeapFreeMemory(buf);
      m_UpdateSeq = 0;

      break;
    }

  case 8:
    {
      gfl2::gfx::PCDebugUtil::PrintBlockImage(buf, "screenshot/colorbuffer_block.bmp", 512, 256, 4, m_pHeap);
      GflHeapFreeMemory(buf);
      m_UpdateSeq = 0;

      break;
    }

  }

  if(m_UpdateSeq == 0)
  {
    //GFL_PRINT("Update Animation\n");
    m_DressUpModel.UpdateAnimation();
    //GFL_PRINT("Traverse Model\n");
    gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();
  }

  return result;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void ArakiTestProc::PreDrawFunc (gfl2::proc::Manager* pManager)
{
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void ArakiTestProc::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
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
  gfl2::math::Vector3 eyePosition(80.0f, 100.0f, 250.0f);
  gfl2::math::Vector3 focusPosition(0.0f, 80.0f, 0.0f);
  gfl2::math::Vector3 upDirection(0.0f, 1.0f, 0.0f);

  gfl2::math::Matrix34 viewMatrix = gfl2::math::Matrix34::GetLookAt(eyePosition, focusPosition, upDirection);

  m_pRenderingPipeLine->SetViewMatrix(viewMatrix);

  // 描画
  m_pRenderingPipeLine->SetDisplaySize(displayWidth, displayHeight);
  m_pRenderingPipeLine->Execute();
}

/**
* @brief   PROCのメインを行う関数
*  1/30フレームの②フレーム目の処理です。
*  1/60フレーム動作時には、使われません。
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
*/
gfl2::proc::Result ArakiTestProc::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result ArakiTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch(m_EndSeq)
  {
  case 0:

    // @todo システムかファクトリで１フレ遅れ開放の対処がしたい
    m_DressUpModel.SetVisible(false);

    // 次のシーケンスへ
    ++m_EndSeq;
    break;

  case 1:

    // インスタンスを先に殺す
    GFL_PRINT("Destroy Dress Up Model\n");
    m_DressUpModel.Destroy();

    // 読み込んだ着せ替えリソースの開放
    GFL_PRINT("Unload Dress Up Parts\n");
    m_DressUpModelResourceManager.UnloadDressUpPartsAll();

    // 読み込んだリソースの開放
    GFL_PRINT("Unload Model\n");
    m_CharaModelFactory.UnloadModelAll();

    // キャラモデルファクトリの終了処理
    GFL_PRINT("Finalize CharaModelFactory\n");
    m_CharaModelFactory.Finalize();

    m_DressUpModelResourceManager.Finalize();

    // レンダリングパイプライン削除
    GFL_PRINT("Delete RenderingPipeLine\n");
    GFL_SAFE_DELETE(m_pRenderingPipeLine);

    // DrawEnvNode削除
    // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも殺されるらしい
    GFL_PRINT("Delete DrawEnvNode\n");
    GFL_SAFE_DELETE(m_pDrawEnvNode);

    GFL_SAFE_DELETE(m_pAllocator);

    // メモリリークチェック
    {
      s32 endFreeSize = m_pHeap->GetTotalFreeSize();
      GFL_ASSERT(m_StartFreeSize == endFreeSize);
    }

    GFL_DELETE_HEAP(m_pHeap);

    result = gfl2::proc::RES_FINISH;
    break;
  }

  return result;
}

inline void ArakiTestProc::SetBodyParams(s32 bodyColorID, s32 eyeColorID, s32 hairColorID, s32 ripColorID)
{
  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = bodyColorID;
  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = eyeColorID;
  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = hairColorID;
  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = ripColorID;
}

inline void ArakiTestProc::SetItemParams(s32 faceID, s32 hairStyleID, s32 badgeAccessoryID, s32 eyeAccessoryID, s32 hairAccessoryID, s32 bagID, s32 bangleID, s32 bodyID, s32 bottomsID, s32 hatID, s32 legsID, s32 shoesID, s32 topsID)
{
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_FACE] = faceID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = hairStyleID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = badgeAccessoryID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = eyeAccessoryID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = hairAccessoryID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BAG] = bagID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = bangleID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BODY] = bodyID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = bottomsID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAT] = hatID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = legsID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = shoesID;
  m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = topsID;
}

inline void ArakiTestProc::SetBodyParams()
{
  s32 iter = m_BodyParamSeq;

  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_BODY_COLOR] = iter % 4;
  iter /= 4;

  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_EYE_COLOR] = iter % 9;
  iter /= 9;

  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_HAIR_COLOR] = iter % 9;
  iter /= 9;

  m_DressUpParam.bodyParams[DressUpParam::BODY_CATEGORY_FEMALE_LIP_COLOR] = iter % 7;
  iter /= 7;

  if(iter != 0)
  {
    m_BodyParamSeq = 0;
  }
}

inline void ArakiTestProc::SetItemParams()
{
  s32 iter = m_ItemParamSeq;

  if (iter == 0)
  {
#if defined FEMALE
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_FACE] = DRESSUP_ITEM_ID_P2_FACE01_OFF;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = DRESSUP_ITEM_ID_P2_HAIR_BOB_ON;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = DRESSUP_ITEM_ID_P2_ACCEYE_BUTTERFLY01_WH;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = DRESSUP_ITEM_ID_P2_ACCHAIR_SIMPLEPIN01_WH;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BAG] = DRESSUP_ITEM_ID_P2_BAG_DBAG01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01_WH;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BODY] = -1;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = DRESSUP_ITEM_ID_P2_BOTTOMS_DHOTPANTS01_WH;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAT] = DRESSUP_ITEM_ID_P2_HAT_DHAT01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = DRESSUP_ITEM_ID_P2_LEGS_BAREFOOT01_LONG;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = DRESSUP_ITEM_ID_P2_SHOES_DSHOES01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = DRESSUP_ITEM_ID_P2_TOPS_DCUT01_WH;
#else
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_FACE] = DRESSUP_ITEM_ID_P1_FACE01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = DRESSUP_ITEM_ID_P1_HAIR_MIDIUM;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = -1;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = -1;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCBADGE] = -1;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BAG] = DRESSUP_ITEM_ID_P1_BAG_DRUCK01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = -1;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BODY] = -1;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = DRESSUP_ITEM_ID_P1_BOTTOMS_DPAN01_WH;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAT] = DRESSUP_ITEM_ID_P1_HAT_DCAP01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = DRESSUP_ITEM_ID_P1_LEGS_BAREFOOT01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = DRESSUP_ITEM_ID_P1_SHOES_DSHOES01;
    m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = DRESSUP_ITEM_ID_P1_TOPS_STRIPE01_BK;
#endif

    return;
  }

#if defined FEMALE
  if (iter < DRESSUP_ITEM_ID_P2_HAIR_BOB_ON ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_FACE] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_ACCEYE_BUTTERFLY01_WH ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_ACCHAIR_SIMPLEPIN01_WH ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_BAG_DBAG01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCHAIR] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_BNGL_ZENBRESS01_WH ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BAG] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_BOTTOMS_DHOTPANTS01_WH ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_HAT_DHAT01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_LEGS_BAREFOOT01_LONG ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAT] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_SHOES_DSHOES01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P2_TOPS_DCUT01_WH ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_FACE01_ON ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = iter; return; }
#else
  if (iter < DRESSUP_ITEM_ID_P1_HAIR_MIDIUM ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_FACE] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_ACCEYE_METAL01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAIR_STYLE] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_BAG_DRUCK01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_ACCEYE] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_BNGL_ZENBRESS01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BAG] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_BOTTOMS_DPAN01_WH ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BNGL] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_HAT_DCAP01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_BOTTOMS] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_LEGS_BAREFOOT01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_HAT] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_SHOES_DSHOES01 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_LEGS] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_TOPS_STRIPE01_BK ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_SHOES] = iter; return; }
  if (iter < DRESSUP_ITEM_ID_P1_TOPS_POLO_SPOLO01_PN3 + 1 ) { m_DressUpParam.itemParams[DressUpParam::ITEM_CATEGORY_TOPS] = iter; return; }
#endif

  m_TestFlag = false;
  GFL_PRINT("最後まで到達しました\n");
}

GFL_NAMESPACE_END(ArakiTest)
  GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
