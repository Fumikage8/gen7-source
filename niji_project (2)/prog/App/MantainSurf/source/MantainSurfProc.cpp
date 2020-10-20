//======================================================================
/**
 * @file MantainSurfProc.cpp
 * @date 2016/09/12
 * @author fang_yicheng
 * @brief マンタインサーフプロセス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include <debug/include/gfl2_DebugPrint.h>
#include <util/include/gfl2_DrawUtilText.h>
#include <gfx/include/gfl2_DrawUtil.h>
#include <util/include/gfl2_UtilTextRenderingPipeLine.h>
#include <fade/include/gfl2_FadeManager.h>
#include <Layout/include/gfl2_LytSys.h>

#include <System/include/HeapDefine.h>
#include <System/include/GflUse.h>
#include <GameSys/include/GameProcManager.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/DllProc.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>
#include <AppLib/include/Util/app_util_AppLytAccessor.h>

#include <Print/include/wordset.h>
#include <arc_def_index/arc_def.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include "../include/MantainSurfProc.h"

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(MantainSurf)

static const s32 MANTAIN_SURF_APP_DEVICE_HEAP_MEM_SIZE        = 0x02000000;
static const s32 MANTAIN_SURF_APP_LOCAL_HEAP_MEM_SIZE         = 0x00500000;
static const s32 MANTAIN_SURF_APP_LOCAL_DEVICE_HEAP_MEM_SIZE  = 0x01500000;

// @fix GFMMCat[168] (上松さん要望)アプリ開始終了時に画面がパッと切り替わる違和感を解消するためにフェードイン/フェードアウトフレーム数を長めに調整
static const u32 APP_FADE_IN_FRAME  = 10;   // アプリ開始時のフェードインフレーム数
static const u32 APP_FADE_OUT_FRAME = 10;   // アプリ終了時のフェードアウトフレーム数

//-----------------------------------------------------------------------------
/**
 * @brief コンストラクタ
 */
//-----------------------------------------------------------------------------
MantainSurfProc::MantainSurfProc(void) :
  m_Seq(0),
  m_SeqSub(0),
  m_seWaitTimer(0),
  m_courseIdNow(0),
  m_courseIdNext(-1),
  m_pNijiAllocator(NULL),
  m_AppDevHeap(NULL),
  m_AppLocalHeap(NULL),
  m_pAppParam(NULL),
  m_pDrawManager(NULL),
  m_pSurfUI(NULL),
  m_pAppManager(NULL),
  m_StartFreeSize(0),
  m_isDrawEnable(false)
{
#if PM_DEBUG
  m_debugIsRestartTest = false;
  m_debugRestartTestInterval = 300;
#endif
}

//-----------------------------------------------------------------------------
/**
 * @brief デストラクタ
 */
//-----------------------------------------------------------------------------
MantainSurfProc::~MantainSurfProc()
{
}

//-----------------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @param   プロセス管理マネージャー
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
  */
//-----------------------------------------------------------------------------
gfl2::proc::Result MantainSurfProc::InitFunc(gfl2::proc::Manager* pManager)
{
#if PM_DEBUG
  // 強制フェードアウト
  static const gfl2::math::Vector4 color(0.0f, 0.0f, 0.0f, 255.0f);
  // @fix GFMMCat[168] デバッグメニューからのアクセス以外は画面をブラックアウトしないように(デモ終了時のホワイトアウトからフェードイン要望対応)
  if(m_pAppParam == NULL)
  {
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut(gfl2::Fade::DISP_DOUBLE, &color);
  }
#endif

  gfl2::heap::HeapBase* pDevHeap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
  GFL_PRINT("[MantainSurfProc::InitFunc]AllocatableSize=%d\n", pDevHeap->GetTotalAllocatableSize());

  m_AppDevHeap = GFL_CREATE_LOCAL_HEAP(pDevHeap, MANTAIN_SURF_APP_DEVICE_HEAP_MEM_SIZE, gfl2::heap::HEAP_TYPE_EXP, false);
  m_StartFreeSize = m_AppDevHeap->GetTotalFreeSize();

  m_AppLocalHeap = GFL_NEW(m_AppDevHeap) app::util::Heap();
  m_AppLocalHeap->LocalHeapCreate(m_AppDevHeap, m_AppDevHeap, MANTAIN_SURF_APP_LOCAL_HEAP_MEM_SIZE, MANTAIN_SURF_APP_LOCAL_DEVICE_HEAP_MEM_SIZE);
  m_pNijiAllocator = GFL_NEW(m_AppLocalHeap->GetDeviceHeap()) System::nijiAllocator(m_AppLocalHeap->GetDeviceHeap());

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief   PROCの終了を行う関数
 * @param   プロセス管理マネージャー
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//---------------------------------------------------------------------------
gfl2::proc::Result MantainSurfProc::EndFunc(gfl2::proc::Manager* pManager)
{
  // アロケーターを破棄
  GFL_SAFE_DELETE(m_pNijiAllocator);

  // メモリリークチェック
  {
    m_AppLocalHeap->Delete();
    GFL_SAFE_DELETE(m_AppLocalHeap);

    s32 endFreeSize = m_AppDevHeap->GetTotalFreeSize();
    if(m_StartFreeSize != endFreeSize)
    {
      m_AppDevHeap->Dump();
      GFL_ASSERT(m_StartFreeSize == endFreeSize);
    }
    GFL_DELETE_HEAP(m_AppDevHeap);
  }

  return gfl2::proc::RES_FINISH;
}

//---------------------------------------------------------------------------
/**
 * @brief	  PROCのメイン更新を行う関数
 * @param   プロセス管理マネージャー
 * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//---------------------------------------------------------------------------
gfl2::proc::Result MantainSurfProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  switch(m_Seq)
  {
  case 0:
    // 初期化
    {
      if(!UpdateInitialize())
      {
        break;
      }
      m_isDrawEnable = true;
      m_SeqSub = 0;
      m_seWaitTimer = 0;
      m_Seq++;
    }
    break;
  case 1:
    // アップデート
    {
      if(!UpdateNormal())
      {
        break;
      }
      m_isDrawEnable = false;
      m_SeqSub = 0;
      m_Seq++;
    }
    break;
  case 2:
    // リソース開放
    {
#if PM_DEBUG
      if(m_pAppManager->GetDebugMenu()->m_dbvIsCourseRestart)
      {
        m_courseIdNext = m_pAppManager->GetDebugMenu()->m_dbvCourseIdNow;
        m_debugIsRestartTest = m_pAppManager->GetDebugMenu()->m_dbvIsRestartTest;
        m_debugRestartTestInterval = m_pAppManager->GetDebugMenu()->m_dbvRestartInterval;
      }
#endif
      if(!UpdateDeinitialize())
      {
        break;
      }
      if(m_courseIdNext >= 0)
      {
        // 再スタート
        m_courseIdNow = m_courseIdNext;
        m_courseIdNext = -1;
        m_Seq = 0;
        m_SeqSub = 0;
        return gfl2::proc::RES_CONTINUE;
      }
      return gfl2::proc::RES_FINISH;
    }
  }
  return gfl2::proc::RES_CONTINUE;
}

//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
//---------------------------------------------------------------------------
void MantainSurfProc::PreDrawFunc(gfl2::proc::Manager* pManager)
{
  if(!m_isDrawEnable)
  {
    return;
  }
  m_pDrawManager->PreDraw();
}

//---------------------------------------------------------------------------
/**
 * @brief	PROCの描画処理
 * @param pManager   プロセス管理マネージャー
 * @param displayNo  描画対象画面
 * 登録したPROCによる、フレームバッファへの描画を実行します。
 * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
 */
//---------------------------------------------------------------------------
void MantainSurfProc::DrawFunc(gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo)
{
  if(!m_isDrawEnable)
  {
    return;
  }

  // 描画
#if PM_DEBUG
  if(m_pAppManager->GetDebugMenu()->m_dbvIsUIDrawEnable)
  {
    switch(displayNo)
    {
    case gfl2::gfx::CtrDisplayNo::LEFT:
      if(m_pAppManager->GetDebugMenu()->m_dbvIsUIUppderDrawEnable) m_pSurfUI->Draw(displayNo);
      break;
    case gfl2::gfx::CtrDisplayNo::DOWN:
      if(m_pAppManager->GetDebugMenu()->m_dbvIsUILowerDrawEnable) m_pSurfUI->Draw(displayNo);
      break;
    }
  }
#else
  m_pSurfUI->Draw(displayNo);
#endif
  m_pDrawManager->Draw(displayNo);
}

//---------------------------------------------------------------------------
/**
 * @brief 起動パラメータを設定する
 * @param pParam                パラメータ
 */
//---------------------------------------------------------------------------
void MantainSurfProc::Setup(App::Event::MANTAIN_SURF_PARAM* pParam)
{
  GFL_ASSERT(pParam != NULL);
  m_pAppParam = pParam;
  m_courseIdNow = m_pAppParam->in_corse_id;
  if(m_pAppParam->in_tutorial && m_pAppParam->in_corse_id != Course::CourseDataManager::TUTORIAL_COURSE_ID)
  {
    // チュートリアル後再スタート
    m_courseIdNow = Course::CourseDataManager::TUTORIAL_COURSE_ID;
    m_courseIdNext = m_pAppParam->in_corse_id;
  }
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool MantainSurfProc::UpdateInitialize()
{
  switch(m_SeqSub)
  {
  case 0:
    {
      // 描画マネージャーの初期化
      m_pDrawManager = GFL_NEW(m_AppLocalHeap->GetDeviceHeap()) MantainSurfDrawManager();
      m_pDrawManager->Initialize(m_pNijiAllocator, m_AppLocalHeap, GetGameManager());
      m_SeqSub++;
    }
    /* Fall Through*/
  case 1:
    {
      if(!m_pDrawManager->IsInitializeFinished())
      {
        break;
      }
      // UIの初期化
      m_pSurfUI = GFL_NEW(m_AppLocalHeap->GetDeviceHeap()) UI::SurfUI(m_AppLocalHeap->GetDeviceHeap(), m_pDrawManager->GetLayoutRenderingManager());
      m_SeqSub++;
    }
    /* Fall Through*/
  case 2:
    {
      if(!m_pSurfUI->ObserveSetup())
      {
        break;
      }
      // Appマネージャーの初期化
      m_pAppManager = GFL_NEW(m_AppLocalHeap->GetDeviceHeap()) MantainSurfAppManager();
#if PM_DEBUG
      GameSys::GameManager *pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
      gfl2::ui::Button* pButton = pGameManager->GetUiDeviceManager()->GetButton(0);
      if(pButton->IsHold(gfl2::ui::BUTTON_L))
      {
        m_courseIdNow = Course::CourseDataManager::TUTORIAL_COURSE_ID;
      }
#endif
      m_pAppManager->SetCourseId(m_courseIdNow);
      m_pAppManager->Initialize(m_pNijiAllocator, m_AppLocalHeap->GetDeviceHeap(), m_pDrawManager, m_pSurfUI, m_pAppParam);
      m_SeqSub++;
    }
    /* Fall Through*/
  case 3:
    {
      if(!m_pAppManager->IsInitializeFinished())
      {
        break;
      }
      m_pDrawManager->SetCamera(m_pAppManager->GetCamera());
      m_SeqSub++;
    }
    /* Fall Through*/
  case 4:
    {
      // 初期化完了
#if PM_DEBUG
      m_pAppManager->GetDebugMenu()->m_dbvIsRestartTest = m_debugIsRestartTest;
      m_pAppManager->GetDebugMenu()->m_dbvRestartInterval = m_debugRestartTestInterval;
#endif
      s32 startSize = m_AppLocalHeap->GetDeviceHeap()->GetTotalSize();
      s32 nowSize = m_AppLocalHeap->GetDeviceHeap()->GetTotalFreeSize();
      s32 usedSize = startSize - nowSize;
      f32 useRate = static_cast<f32>(usedSize) / static_cast<f32>(startSize)* 100.0f;
      GFL_PRINT("[MantainSurfProc::InitFunc]StartSize=%d NowSize=%d UsedSize=%d UseRate=%f02\n", startSize, nowSize, usedSize, useRate);
      return true;
    }
  }
  return false;
}

bool MantainSurfProc::UpdateNormal()
{
  // トラバース前更新
  m_pAppManager->Update();
  // トラバース
  m_pDrawManager->Update();
  // UI更新
  m_pSurfUI->Update();
  // ライティング更新
  m_pDrawManager->UpdateLighting(m_pAppManager->GetSkyBoxManager()->GetCurrentLightingColor());
  // トラバース後更新
  m_pAppManager->UpdateAfterTraverse();

  switch(m_SeqSub)
  {
  case 0:
    // フェイドインリクエスト
    GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, APP_FADE_IN_FRAME);
    m_SeqSub++;
    /* Fall Throught */
  case 1:
    // フェイドイン完了待ち
    if(!GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE))
    {
      break;
    }
    m_SeqSub++;
    /* Fall Throught */
  case 2:
    // メインループ
    if(m_pAppManager->GetGameState() == MantainSurfAppManager::ST_QUIT)
    {
      // APP終了
      if(!m_pAppManager->IsTutorialMode() || !m_pAppManager->GetTutorialManager()->IsTutorialFinished())
      {
        static const gfl2::math::Vector4 col_start(0.0f, 0.0f, 0.0f, 0.0f);
        static const gfl2::math::Vector4 col_end(0.0f, 0.0f, 0.0f, 255.0f);
        GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestOut(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, &col_start, &col_end, APP_FADE_OUT_FRAME);
      }
      m_SeqSub++;
    }
    break;
  case 3:
    // フェイドアウト完了待ち
    if(!GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->IsEnd(gfl2::Fade::DISP_DOUBLE))
    {
      break;
    }
    // BGM/SEを終了する
    SurfSound::SurfSoundManager::StopAllSE();
    SurfSound::SurfSoundManager::EndBGM();
    m_SeqSub++;
    /* Fall Through */

  case 4:
    // SE終了待ち
    if(!SurfSound::SurfSoundManager::IsAllSEStopped())
    {
      m_seWaitTimer++;
      if(m_seWaitTimer < 30 * 3)
      {
        break;
      }
      // 3秒過ぎても終了していない場合は強制離脱
      GFL_ASSERT(0);
      SurfSound::SurfSoundManager::StopAllSE();
    }
    m_SeqSub++;
    /* Fall Through */

  case 5:
    // シーケンス終了
    return true;
  }
  return false;
}

bool MantainSurfProc::UpdateDeinitialize()
{
  // Appマネージャーを破棄
  if(m_pAppManager)
  {
    m_pAppManager->Finalize();
  }
  GFL_SAFE_DELETE(m_pAppManager);

  // UIを破棄
  if(m_pSurfUI && m_pSurfUI->IsDrawing())
  {
    return false;
  }
  GFL_SAFE_DELETE(m_pSurfUI);

  // 描画マネージャーを破棄
  if(m_pDrawManager)
  {
    m_pDrawManager->Finalize();
  }
  GFL_SAFE_DELETE(m_pDrawManager);
  return true;
}

GFL_NAMESPACE_END(MantainSurf)
GFL_NAMESPACE_END(App)
