/**
* @brief  竹部テスト用プロック：着せ替え
* @file   TakebeTestProc_Kisekae.cpp
* @author takebe_makoto
* @data   2015.04.14
*/
#if PM_DEBUG

#include "../include/TakebeTestProc_Kisekae.h"

#include <GameSys/include/GameProcManager.h>

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// garc
#include <arc_def_index/arc_def.h>

#include <System/include/DressUp.h>

#define KISEKAE_TEST
//#define KISEKAE_DYNAMIC_MOTION

#if defined(KISEKAE_TEST)

// テスト
#define KISEKAE_ARCID_NORMAL ARCID_CHARA_MODEL_FIELD_NORMAL
#define KISEKAE_ARCID_PARTS_LIST System::DressUp::GetDressUpArcIdListField()

// モデルのインデックス
#include <niji_conv_header/chara_test/chara_model_id.h>
#define KISEKAE_MODEL_ID 1//CHARA_MODEL_ID_P2_BASE_FI

// モーションのインデックス
#include <niji_conv_header/chara_test/p2_base_fi.h>
#define KISEKAE_ANIMID P2_BASE_FI_WAIT01

#elif defined(KISEKAE_BATTLE)

// バトル
#define KISEKAE_ARCID_NORMAL ARCID_CHARA_MODEL_BATTLE

// モデルのインデックス
#include <niji_conv_header/battle/chara/chara_model_id.h>
#define KISEKAE_MODEL_ID CHARA_MODEL_ID_P2_BASE

// モーションのインデックス
#include <niji_conv_header/battle/chara/p2_base.h>
#define KISEKAE_ANIMID P2_BASE_BA005_WAIT01

#else

// フィールド
#define KISEKAE_ARCID_NORMAL ARCID_CHARA_MODEL_FIELD

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>
#define KISEKAE_MODEL_ID CHARA_MODEL_ID_P2_BASE_FI

// モーションのインデックス
#include <niji_conv_header/field/chara/p2_base_fi.h>
#define KISEKAE_ANIMID P2_BASE_FI010_WALK01
#endif

#if defined(KISEKAE_DYNAMIC_MOTION)
#include <niji_conv_header/field/chara/chara_model_dynamic_motion.h>
#define KISEKAE_DYNAMIC_ID P2_BASE_DYNAMIC0

#include <niji_conv_header/field/chara/p2_base_dynamic0.h>
#define KISEKAE_DYNAMIC_ANIMID P2_BASE_FI041_SEEK01_START
#endif


namespace Test { namespace TakebeTest {

using namespace poke_3d::model;

/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TakebeTestProc_Kisekae::TakebeTestProc_Kisekae( void ) :
  m_pHeap(NULL),
  m_pNijiAllocator(NULL),
  m_StartFreeSize(0),
  m_InitSeq(0),
  m_UpdateSeq(0),
  m_EndSeq(0),
  m_pRenderingPipeLine(NULL),
  m_pDrawEnvNode(NULL)
{
}

/**
* @brief デストラクタ
*
* @return 無し
*/
TakebeTestProc_Kisekae::~TakebeTestProc_Kisekae( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result TakebeTestProc_Kisekae::InitFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch (m_InitSeq)
  {
  case 0:
    {
      m_pHeap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_APP_DEVICE );
      GFL_ASSERT( m_pHeap != NULL );
      m_StartFreeSize = m_pHeap->GetTotalFreeSize();

      m_pNijiAllocator = GFL_NEW(m_pHeap) System::nijiAllocator(m_pHeap);

      m_pRenderingPipeLine = GFL_NEW(m_pHeap) gfl2::renderingengine::renderer::DefaultRenderingPipeLine(m_pNijiAllocator);

      m_pDrawEnvNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateDrawEnvNode(m_pNijiAllocator);
      // ライト2
      {
        gfl2::renderingengine::scenegraph::instance::LightSetNode		*pLightSetNode;
        gfl2::renderingengine::scenegraph::instance::LightNode			*pDirectionalLightNode;
        gfl2::renderingengine::scenegraph::instance::LightNode			*pAmbientLightNode;

        pLightSetNode			= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightSetNode(m_pNijiAllocator);
        pDirectionalLightNode = gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);
        pAmbientLightNode		= gfl2::renderingengine::scenegraph::instance::InstanceCreator::CreateLightNode(m_pNijiAllocator);

        {
          pDirectionalLightNode->SetNodeName( "DirectionalLight02" );
          gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pDirectionalLightNode->GetNodeName()) );
          pDirectionalLightNode->SetNodeNameHash( nameHash.value );
          pDirectionalLightNode->SetLightType( gfl2::renderingengine::LightType::Directional );
          gfl2::math::Vector3 position = gfl2::math::Vector3(0.0f, 10000000.0f, 10000000.0f);
          pDirectionalLightNode->SetPosition( position );
          gfl2::math::Vector3 direction = -position.Normalize();
          pDirectionalLightNode->SetDirection( direction );
          pDirectionalLightNode->SetColor( gfl2::math::Vector3( 0.7f, 0.7f, 0.7f ) );
        }

        {
          pAmbientLightNode->SetNodeName( "AmbientLight02" );
          gfl2::util::FnvHash				nameHash( reinterpret_cast<const u8*>(pAmbientLightNode->GetNodeName()) );
          pAmbientLightNode->SetNodeNameHash( nameHash.value );
          pAmbientLightNode->SetLightType( gfl2::renderingengine::LightType::Ambient );
          pAmbientLightNode->SetColor( gfl2::math::Vector3( 1.0f, 1.0f, 1.0f ) );
        }

        pLightSetNode->SetSetNo( 2 );
        pLightSetNode->AddLight( pDirectionalLightNode );//LightSetNodeの子となる
        pLightSetNode->AddLight( pAmbientLightNode );//LightSetNodeの子となる
        m_pDrawEnvNode->AddLightSet( pLightSetNode );//DrawEnvNodeの子となる
      }

      gfl2::math::AABB envSpace(gfl2::math::Vector4(-500000.0f, -500000.0f, -500000.0f), gfl2::math::Vector4(500000.0f, 500000.0f, 500000.0f));
      m_pDrawEnvNode->SetAABB(envSpace);

      gfl2::renderingengine::scenegraph::SceneGraphManager::AddChild(m_pDrawEnvNode);
      m_pRenderingPipeLine->AddDrawEnv(m_pDrawEnvNode);

      // キャラモデルファクトリー初期化、フィールド用かバトル用のARCIDを指定する
      // arc_def_index/arc_def.hをインクルードしておいてください
      m_CharaModelFactory.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pHeap, KISEKAE_ARCID_NORMAL);

      m_DressUpModelResourceManager.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pHeap, KISEKAE_ARCID_PARTS_LIST);

      // リソースを非同期読み込みする
      // niji_conv_header/chara/chara_model_id.hをインクルードしておいてください
      m_CharaModelFactory.LoadModelAsync(KISEKAE_MODEL_ID);

      // 着せ替えモデル用のパーツを非同期読み込みする
#if 0
      System::DressUp::GetDressUpParamDefaultHero(&m_DressUpParam);
#else
      System::DressUp::GetDressUpParamDefaultHeroine(&m_DressUpParam);
#endif
      m_DressUpModelResourceManager.LoadDressUpPartsAsync(m_DressUpParam);

      // 次のシーケンスへ
      ++m_InitSeq;
    }
    break;

  case 1:
    {
      // リソースの非同期読み込み終了確認待ち
      if (!m_CharaModelFactory.IsModelLoaded(KISEKAE_MODEL_ID))
      {
        break;
      }

      // 着せ替えパーツの非同期読み込み終了確認待ち
      if (!m_DressUpModelResourceManager.IsDressUpPartsLoaded(m_DressUpParam))
      {
        break;
      }

      // 読み込んだモデルをセットアップ
      m_CharaModelFactory.SetupModel(m_pNijiAllocator, KISEKAE_MODEL_ID);

      // 読み込んだ着せ替えパーツをセットアップ
      m_DressUpModelResourceManager.SetupDressUpParts(m_pNijiAllocator, m_DressUpParam);

      // 読み込んだリソースから着せ替えモデルを作成
      m_CharaModelFactory.CreateDressUpModel(m_pNijiAllocator, m_pHeap, &m_DressUpModel, &m_DressUpModelResourceManager, KISEKAE_MODEL_ID);

      // アニメーションを変更する。アニメーションのIDは
      // niji_conv_header/chara/以下に出力されるヘッダーに定義してあるのでインクルードしておいてください
      m_DressUpModel.ChangeAnimation(KISEKAE_ANIMID);

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
gfl2::proc::Result TakebeTestProc_Kisekae::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  gfl2::ui::Button* pButton = this->GetGameManager()->GetUiDeviceManager()->GetButton(0);

  // Bボタンでサンプルを抜ける
  if (pButton->IsHold(gfl2::ui::BUTTON_B))
  {
    result = gfl2::proc::RES_FINISH;
    return result;
  }

#if defined(KISEKAE_DYNAMIC_MOTION)
  switch (m_UpdateSeq)
  {
  case 0:
    {

      // Yボタンで非常駐アニメーションのテスト
      if (pButton->IsHold(gfl2::ui::BUTTON_Y))
      {
        ++m_UpdateSeq;
        break;
      }
    }
    break;

  case 1:
    {
      // 非常駐アニメーションパックの読み込み
      m_CharaModelFactory.LoadDynamicAnimationAsync(KISEKAE_DYNAMIC_ID);
      m_DressUpModelResourceManager.LoadDressUpDynamicAnimationAsync(m_DressUpParam, KISEKAE_DYNAMIC_ID);

      ++m_UpdateSeq;
    }
    break;

  case 2:
    {
      // 非常駐アニメーションパックの読み込み完了待ち
      if (!m_CharaModelFactory.IsDynamicAnimationLoaded(KISEKAE_DYNAMIC_ID))
      {
        break;
      }
      if (!m_DressUpModelResourceManager.IsDressUpDynamicAnimationLoaded(m_DressUpParam, KISEKAE_DYNAMIC_ID))
      {
        break;
      }

      // 読み込んだ非常駐アニメーションパックのセットアップ
      m_CharaModelFactory.SetupDynamicAnimation(m_pNijiAllocator, m_pHeap, KISEKAE_DYNAMIC_ID);
      m_DressUpModelResourceManager.SetupDressUpDynamicAnimation(m_pNijiAllocator, m_pHeap, m_DressUpParam, KISEKAE_DYNAMIC_ID);

      // 非常駐アニメーションパック内のアニメーションを再生してみる
      m_DressUpModel.ChangeAnimationSmooth(KISEKAE_DYNAMIC_ANIMID, 8);
      m_DressUpModel.SetAnimationLoop(true);

      m_Counter = 60;
      ++m_UpdateSeq;
    }
    break;

  case 3:
    {
      // カウンタが０になるまで待ち
      if (--m_Counter > 0)
      {
        break;
      }

      // 再生中のアニメーションIDを取得してみる
      u32 animationId = m_DressUpModel.GetAnimationId();
      if (animationId != KISEKAE_DYNAMIC_ANIMID)
      {
        GFL_ASSERT(0);
      }

      // 常駐アニメーションに戻す
      m_DressUpModel.ChangeAnimationSmooth(KISEKAE_ANIMID, 8);
      m_DressUpModel.SetAnimationLoop(true);

      ++m_UpdateSeq;
    }
    break;

  case 4:
    {
      // アニメーション補間の終了待ち
      if (m_DressUpModel.IsAnimationInterp())
      {
        break;
      }

      // 非常駐アニメーションの開放
      m_CharaModelFactory.UnloadDynamicAnimation(KISEKAE_DYNAMIC_ID);
      m_DressUpModelResourceManager.UnloadDressUpDynamicAnimation(m_DressUpParam, KISEKAE_DYNAMIC_ID);

      m_UpdateSeq = 0;
    }
    break;
  }
#endif

  gfl2::ui::VectorDevice* pStick = this->GetGameManager()->GetUiDeviceManager()->GetStick(0);

  // 左右で移動
  gfl2::math::Vector3 pos = m_DressUpModel.GetPosition();
  pos.x += pStick->GetX();
  pos.y += pStick->GetY();
  m_DressUpModel.SetPosition(pos);

  // アニメーション更新
  m_DressUpModel.UpdateAnimation();

  // ワールド行列更新、この中でシーングラフを辿ってローカル行列からワールド行列を生成しています
  // シーン全体で一回呼ぶだけで大丈夫です
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();

  return result;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void TakebeTestProc_Kisekae::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void TakebeTestProc_Kisekae::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
{
  gfl2::gfx::DisplayDeviceHandle displayHandle = System::GflUse::m_HandleList[displayNo];
  u32 displayWidth  = gfl2::gfx::GFGL::GetDisplayWidth(displayHandle);
  u32 displayHeight = gfl2::gfx::GFGL::GetDisplayHeight(displayHandle);

  // 射影変換行列(パースペクティブ(透視法)射影)
  gfl2::math::Matrix44 projMatrix = gfl2::math::Matrix44::GetPerspective(
      gfl2::math::ConvDegToRad(37.0f),		// 視野角30°
      (float)displayWidth / (float)displayHeight,	// アスペクト比
      1.1f,							// 前方投影面までの距離
      1000.0f,							// 後方投影面までの距離
      true);                // 画面の向きに合わせる

  m_pRenderingPipeLine->SetProjectionMatrix(projMatrix);

  // ビュー変換行列
  gfl2::math::Vector3 eyePosition(80.0f, 100.0f, 250.0f);  // 視点(カメラの位置)
  gfl2::math::Vector3 focusPosition(0.0f, 80.0f,  0.0f);  // 注視点
  gfl2::math::Vector3 upDirection(0.0f, 1.0f,  0.0f);     // カメラの上方向

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
gfl2::proc::Result TakebeTestProc_Kisekae::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result TakebeTestProc_Kisekae::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch (m_EndSeq)
  {
  case 0:

    // インスタンスを解放できるか確認
    if (!m_DressUpModel.CanDestroy())
    {
      break;
    }

    // インスタンスを解放
    m_DressUpModel.Destroy();

    // 次のシーケンスへ
    ++m_EndSeq;
    break;

  case 1:

    // 読み込んだ着せ替えリソースの解放ができるか確認
    if (!m_DressUpModelResourceManager.CanUnloadDressUpParts(m_DressUpParam))
    {
      break;
    }

    // 読み込んだ着せ替えリソースの開放
    m_DressUpModelResourceManager.UnloadDressUpParts(m_DressUpParam);

    // 次のシーケンスへ
    ++m_EndSeq;
    break;

  case 2:

    // 読み込んだリソースの解放ができるか確認
    if (!m_CharaModelFactory.CanUnloadModel(KISEKAE_MODEL_ID))
    {
      break;
    }

    // 読み込んだリソースの開放
    m_CharaModelFactory.UnloadModel(KISEKAE_MODEL_ID);

    // キャラモデルファクトリの終了処理
    m_CharaModelFactory.Finalize();
    m_DressUpModelResourceManager.Finalize();

    // レンダリングパイプライン削除
    GFL_SAFE_DELETE(m_pRenderingPipeLine);

    // DrawEnvNode削除
    // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも殺されるらしい
    GFL_SAFE_DELETE(m_pDrawEnvNode);

    GFL_SAFE_DELETE(m_pNijiAllocator);

    // メモリリークチェック
    {
      s32 endFreeSize = m_pHeap->GetTotalFreeSize();
      GFL_ASSERT(m_StartFreeSize == endFreeSize);
    }

    // プロセス終了
    result = gfl2::proc::RES_FINISH;
    break;
  }

  return result;
}

}} // Test::TakebeTest

#endif // PM_DEBUG
