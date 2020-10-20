/**
* @brief  竹部テスト用プロック：キャラモデル
* @file   TakebeTestProc_CharaModel.h
* @author takebe_makoto
* @data   2015.04.14
*/
#if PM_DEBUG

#include "../include/TakebeTestProc_CharaModel.h"

#include <GameSys/include/GameProcManager.h>

#include <model/include/gfl2_CharaModel.h>
#include <model/include/gfl2_CharaModelFactory.h>

#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

// garc
#include <arc_def_index/arc_def.h>

// モデルのインデックス
#include <niji_conv_header/field/chara/chara_model_id.h>

// モーションのインデックス
#include <niji_conv_header/field/chara/tr0004_00.h>

#define TEST_ARC_ID ARCID_CHARA_MODEL_FIELD_NORMAL
#define TEST_MODEL_ID CHARA_MODEL_ID_TR0004_00
#define TEST_ANIM_ID TR0004_00_FI001_WAIT01

#define TEST_DYNAMIC_MOTION
#if defined(TEST_DYNAMIC_MOTION)
// 非常中モーションのインデックス
#include <niji_conv_header/field/chara/chara_model_dynamic_motion.h>
#include <niji_conv_header/field/chara/tr0004_00_dynamic0.h>

#define TEST_DY_ANIM_PACK_ID TR0004_00_DYNAMIC0
#define TEST_DY_ANIM_ID TR0004_00_EV0040_APPEAL
#endif

#include <system/include/GflUse.h>

namespace Test { namespace TakebeTest {


/**
* @brief コンストラクタ
*
* @return 無し
*/ 
TakebeTestProc_CharaModel::TakebeTestProc_CharaModel( void ) :
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
TakebeTestProc_CharaModel::~TakebeTestProc_CharaModel( void )
{
}

/**
* @brief   PROCの初期化を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
*/
gfl2::proc::Result TakebeTestProc_CharaModel::InitFunc( gfl2::proc::Manager* pManager )
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
      m_CharaModelFactory.Initialize(this->GetGameManager()->GetAsyncFileManager(), m_pHeap, TEST_ARC_ID);

      // リソースを非同期読み込みする
      // niji_conv_header/chara/chara_model_id.hをインクルードしておいてください
      m_CharaModelFactory.LoadModelAsync(TEST_MODEL_ID);

      // 次のシーケンスへ
      ++m_InitSeq;
    }
    break;

  case 1:
    {
      // リソースの非同期読み込み終了確認待ち
      if (!m_CharaModelFactory.IsModelLoaded(TEST_MODEL_ID))
      {
        break;
      }


      // 読み込んだモデルをセットアップ
      m_CharaModelFactory.SetupModel(m_pNijiAllocator, TEST_MODEL_ID);

      // 読み込んだリソースからモデルを作成
      m_CharaModelFactory.CreateModel(m_pNijiAllocator, m_pHeap, &m_CharaModel, TEST_MODEL_ID);

      // アニメーションを変更する。アニメーションのIDは
      // niji_conv_header/chara/以下に出力されるヘッダーに定義してあるのでインクルードしておいてください
      m_CharaModel.ChangeAnimation(TEST_ANIM_ID);

      // アニメーションデータのループフラグが設定されていなかったので強制的にループさせます。
      // 本来はデータに含まれているはずなので必要ありません
      m_CharaModel.SetAnimationLoop(true);


      // アプリ用のデータを取得してみる
      //const void* pAppData = m_CharaModelFactory.GetApplicationData(CHARA_MODEL_ID_TR0001_00);
      //(void)pAppData;

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
gfl2::proc::Result TakebeTestProc_CharaModel::UpdateFunc( gfl2::proc::Manager* pManager )
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch (m_UpdateSeq)
  {
  case 0:
    {
      gfl2::ui::Button* pButton = this->GetGameManager()->GetUiDeviceManager()->GetButton(0);

      // Bボタンでサンプルを抜ける
      if (pButton->IsHold(gfl2::ui::BUTTON_B))
      {
        result = gfl2::proc::RES_FINISH;
        break;
      }

      // 自動目パチ
      if (pButton->IsTrigger(gfl2::ui::BUTTON_L))
      {
        poke_3d::model::CharaModel::AutoBlinkMode autoBlinkMode = m_CharaModel.GetAutoBlinkMode();
        autoBlinkMode = (autoBlinkMode != poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE)
          ? poke_3d::model::CharaModel::AUTO_BLINK_MODE_NONE
          : poke_3d::model::CharaModel::AUTO_BLINK_MODE_FIELD;

        m_CharaModel.SetAutoBlinkMode(autoBlinkMode);
        m_CharaModel.SetupRandomFuc(System::GflUse::GetPublicRand);
        break;
      }

      // 口パク
      if (pButton->IsTrigger(gfl2::ui::BUTTON_R))
      {
        poke_3d::model::CharaModel::MouthIndex mouthIndex = m_CharaModel.GetMouthIndex();
        mouthIndex = (mouthIndex != poke_3d::model::CharaModel::MOUTH_INDEX_OPEN)
          ? poke_3d::model::CharaModel::MOUTH_INDEX_OPEN
          : poke_3d::model::CharaModel::MOUTH_INDEX_CLOSE;

        m_CharaModel.SetMouthIndex(mouthIndex);
        break;
      }

      // 首制御
      if (pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
      {
        m_CharaModel.SetNeckAngle(-0.7f, 0.0f, 10);
      }
      else if (pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
      {
        m_CharaModel.SetNeckAngle(0.7f, 0.0f, 10);
      }
      else if (pButton->IsTrigger(gfl2::ui::BUTTON_DOWN))
      {
        m_CharaModel.SetNeckAngle(0.0f, -0.7f, 10);
      }
      else if (pButton->IsTrigger(gfl2::ui::BUTTON_UP))
      {
        m_CharaModel.SetNeckAngle(0.0f, 0.7f, 10);
      }
      else if (pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        m_CharaModel.ResetNeckAngle(10);
      }


#if defined(TEST_DYNAMIC_MOTION)
      // Yボタンで非常駐アニメーションのテスト
      if (pButton->IsHold(gfl2::ui::BUTTON_Y))
      {
        ++m_UpdateSeq;
        break;
      }
#endif
    }
    break;

#if defined(TEST_DYNAMIC_MOTION)
  case 1:
    {
      // 非常駐アニメーションパックの読み込み
      m_CharaModelFactory.LoadDynamicAnimationAsync(TEST_DY_ANIM_PACK_ID);

      ++m_UpdateSeq;
    }
    break;

  case 2:
    {
      // 非常駐アニメーションパックの読み込み完了待ち
      if (!m_CharaModelFactory.IsDynamicAnimationLoaded(TEST_DY_ANIM_PACK_ID))
      {
        break;
      }

      // 読み込んだ非常駐アニメーションパックのセットアップ
      m_CharaModelFactory.SetupDynamicAnimation(m_pNijiAllocator, m_pHeap, TEST_DY_ANIM_PACK_ID);

      // 非常駐アニメーションパック内のアニメーションを再生してみる
      m_CharaModel.ChangeAnimationSmooth(TEST_DY_ANIM_ID, 8);

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
      u32 animationId = m_CharaModel.GetAnimationId();
      if (animationId != TEST_DY_ANIM_ID)
      {
        GFL_ASSERT(0);
      }

      // 常駐アニメーションに戻す
      m_CharaModel.ChangeAnimationSmooth(TEST_ANIM_ID, 8);

      ++m_UpdateSeq;
    }
    break;

  case 4:
    {
      // アニメーション補間の終了待ち
      if (m_CharaModel.IsAnimationInterp())
      {
        break;
      }

      // 非常駐アニメーションの開放
      m_CharaModelFactory.UnloadDynamicAnimation(TEST_DY_ANIM_PACK_ID);

      m_UpdateSeq = 0;
    }
    break;
#endif
  }

  // 目パチ口パクの更新
  m_CharaModel.UpdateEye();
  m_CharaModel.UpdateMouth();

  // アニメーション更新、この中でアニメーションのフレーム更新とローカル行列を生成しています
  // モデル毎に呼んでください
  m_CharaModel.UpdateAnimation();

  // ワールド行列更新、この中でシーングラフを辿ってローカル行列からワールド行列を生成しています
  // シーン全体で一回呼ぶだけで大丈夫です
  gfl2::renderingengine::scenegraph::SceneGraphManager::TraverseModel();

  // 首制御：ワールド行列を触るのでシーンのトラバース後
  m_CharaModel.UpdateNeck();

  return result;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void TakebeTestProc_CharaModel::PreDrawFunc( gfl2::proc::Manager* pManager )
{
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void TakebeTestProc_CharaModel::DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo )
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
  gfl2::math::Vector3 eyePosition(0.0f, 120.0f, 250.0f);  // 視点(カメラの位置)
  gfl2::math::Vector3 focusPosition(0.0f, 100.0f,  0.0f);  // 注視点
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
gfl2::proc::Result TakebeTestProc_CharaModel::NextFrameMainFunc(gfl2::proc::Manager* pManager)
{
  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief   PROCの終了を行う関数
* @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
*/
gfl2::proc::Result TakebeTestProc_CharaModel::EndFunc(gfl2::proc::Manager* pManager)
{
  gfl2::proc::Result result = gfl2::proc::RES_CONTINUE;

  switch (m_EndSeq)
  {
  case 0:

    // インスタンスを解放できるか確認
    if (!m_CharaModel.CanDestroy())
    {
      break;
    }

    // インスタンスを解放
    m_CharaModel.Destroy();

    // 次のシーケンスへ
    ++m_EndSeq;
    break;

  case 1:

    // 読み込んだリソースの解放ができるか確認
    if (!m_CharaModelFactory.CanUnloadModel(TEST_MODEL_ID))
    {
      break;
    }

    // 読み込んだリソースの開放
    m_CharaModelFactory.UnloadModel(TEST_MODEL_ID);

    // キャラモデルファクトリの終了処理
    m_CharaModelFactory.Finalize();

    // レンダリングパイプライン削除
    GFL_SAFE_DELETE(m_pRenderingPipeLine);

    // DrawEnvNode削除
    // この中でLightNodeなどDrawEnvNodeにぶら下がっているものも殺されるらしい
    GFL_SAFE_DELETE(m_pDrawEnvNode);

    
    GFL_SAFE_DELETE(m_pNijiAllocator);
    // メモリリークチェック
    {
      s32 endFreeSize = m_pHeap->GetTotalFreeSize();
      m_pHeap->Dump();
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
