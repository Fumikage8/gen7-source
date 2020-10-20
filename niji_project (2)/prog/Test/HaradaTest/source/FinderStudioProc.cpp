#if PM_DEBUG
//======================================================================
/**
 * @file FinderStudioProc.cpp
 * @date 2016/08/25
 * @author harada_takumi
 * @brief ファインダースタジオプロック
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#include "../include/FinderStudioProc.h"

// FinderStudio
#include "App/FinderStudioViewer/include/FinderStudioViewer.h"
#include "App/FinderStudioViewer/source/FinderStudioPokemon.h"
#include "App/FinderStudioViewer/source/FinderStudioDressUpChara.h"
#include "App/FinderStudioViewer/source/FinderStudioBackGround.h"

#if PM_DEBUG
// Debug
#include <debug/include/gfl2_DebugWinSystem.h>
#endif

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(HaradaTest)

const u32 FinderStudioTestProc::HEAP_SIZE = 0x1400000;

//コンストラクタ
FinderStudioTestProc::FinderStudioTestProc():
  m_InitSeq(0)
  ,m_EndSeq(0)
  ,m_ChangeMode(0)
  ,m_MainSeq(0)
  ,m_SubSeq(0)
  ,m_RealFormNo(0)
  ,m_pAsingBattleMotionList(NULL)
  ,m_pAsingKawaigariMotionList(NULL)
  ,m_WaitCnt(0)
  ,m_MotionListIdx(0)
  ,m_pHeap(NULL)
  ,m_pMainSystem(NULL)
{
}
//デストラクタ
FinderStudioTestProc::~FinderStudioTestProc(void)
{
}

//============================================================================
//GameProc
//============================================================================
//------------------------------------------------------------------
/**
  * @brief   PROCの初期化を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でMainFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result FinderStudioTestProc::InitFunc(gfl2::proc::Manager* pManager)
{
  switch(m_InitSeq)
  {
  case 0:
    {
      // ヒープ確保
      gfl2::heap::HeapBase* heap = gfl2::heap::Manager::GetHeapByHeapId(HEAPID_APP_DEVICE);
      m_pHeap = GFL_CREATE_LOCAL_HEAP_NAME(heap,HEAP_SIZE,gfl2::heap::HEAP_TYPE_EXP,false,"FinderStudioMainHeap");

      m_pPersonalData = GFL_NEW(m_pHeap) pml::personal::PersonalData(m_pHeap);

      // メインシステム初期化
      App::FinderStudio::Viewer::PhotoParam photoParam = 
      {
        103,
        1,
        0,
        1,
        1,
        1,
        1,
        1,
        1,

        8,
        1,
        1,
        1,
        34,

        1,
        1,
        1,
        1,
        0,

        0,
        0,
        0,
        0,

        0,
        0,

        3,
        0,
        1,
        0,
        8,
        40,
        30,
        0,

        1,
        1,
        1,
        1,
        30,
        30,

        30,
        30,
        1,
        1,
        2,

        10,
        180,
        10,

        1,
        90,
        90,
        100,
        10,

        24000,
        12000,

        0,
        0,
        0,
        0
      };

      m_PhotoParam = photoParam;


      //m_pMainSystem = GFL_NEW(m_pHeap) App::FinderStudio::Viewer::FinderStudioViewer(m_pHeap,photoParam,App::FinderStudio::Viewer::VIEWER_MODE_NORMAL);

      // 通常起動
      PokeTool::SimpleParam pokeSimpleParam(static_cast<MonsNo>(25),static_cast<pml::FormNo>(0),static_cast<pml::Sex>(1),0,0,0);
      poke_3d::model::DressUpParam dressUpParam;
      System::DressUp::GetDressUpParamDefaultHeroine(&dressUpParam);
      m_pMainSystem = GFL_NEW(m_pHeap) App::FinderStudio::Viewer::FinderStudioViewer(m_pHeap,pokeSimpleParam,dressUpParam);
      //m_pMainSystem = GFL_NEW(m_pHeap) App::FinderStudio::Viewer::FinderStudioViewer(m_pHeap,m_PhotoParam,App::FinderStudio::Viewer::VIEWER_MODE_PHOTO);

      m_InitSeq++;
    }
    // fall through
  case 1:
    if(m_pMainSystem->InitFunc())
    {
      m_InitSeq++;
    }
    break;
  case 2:
    // 初期化終了

    m_pMainSystem->ChangeTimeZone(App::FinderStudio::Viewer::TIME_ZONE_EVENING);

    return gfl2::proc::RES_FINISH;
  }
  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCの終了を行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
  */
//------------------------------------------------------------------
gfl2::proc::Result FinderStudioTestProc::EndFunc(gfl2::proc::Manager* pManager)
{
  switch(m_EndSeq)
  {
  case 0:
    if(m_pMainSystem->EndFunc())
    {
      // 終了処理終了
      GFL_SAFE_DELETE(m_pMainSystem);
      GFL_SAFE_DELETE(m_pPersonalData);
      m_EndSeq++;
    }
    break;
  case 1:
    GFL_DELETE_HEAP(m_pHeap);
    return gfl2::proc::RES_FINISH;
  }
  
  return gfl2::proc::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
  * @brief   PROCのメインを行う関数
  * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
  */
//------------------------------------------------------------------
gfl2::proc::Result FinderStudioTestProc::UpdateFunc(gfl2::proc::Manager* pManager)
{
  // 仮処理 Startボタンで前Procに戻る
  // ボタン入力状況を取得
  gfl2::ui::Button* pButton = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetButton(gfl2::ui::DeviceManager::BUTTON_STANDARD);
  if(pButton->IsTrigger(gfl2::ui::BUTTON_START))
  {
    return gfl2::proc::RES_FINISH;
  }

  // メイン更新
  m_pMainSystem->Update();


  switch(m_MainSeq)
  {
  case 0:
  {
    if(!gfl2::debug::DebugWin_IsOpen())
    {
      if(pButton->IsTrigger(gfl2::ui::BUTTON_R))
      {
        m_MainSeq++;
      }
      //m_pMainSystem->IsFinishPhotoReproduct();

      if(pButton->IsTrigger(gfl2::ui::BUTTON_L))
      {

      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
      {
        m_pMainSystem->GetChara()->DebugPlayZMotion();
      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_X))
      {
        m_pMainSystem->StopZMotion();
      }

      if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
      {
        m_pMainSystem->PlayPokeMotion();
      }

      if(pButton->IsHold(gfl2::ui::BUTTON_ZL) && pButton->IsTrigger(gfl2::ui::BUTTON_Y))
      {
        m_pMainSystem->ChangePokeMotionType(App::FinderStudio::Viewer::POKE_MOTION_TYPE_KAWAIGARI,true);
      }

      if(pButton->IsHold(gfl2::ui::BUTTON_ZR) && pButton->IsTrigger(gfl2::ui::BUTTON_Y))
      {
        m_pMainSystem->ChangePokeMotionType(App::FinderStudio::Viewer::POKE_MOTION_TYPE_BATTLE,true);
      }

      gfl2::ui::TouchPanel* pTouch = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetTouchPanel(0);
      if(!pTouch->IsTouch())
      {
        gfl2::ui::VectorDevice* pStick = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetStick(0);
        f32 sx = pStick->GetX();
        f32 sy = pStick->GetY();

        if(sx > 0.5f)
        {
          m_pMainSystem->CameraRotateYLeft();
        }
        if(sx < -0.5f)
        {
          m_pMainSystem->CameraRotateYRight();
        }
        if(sy > 0.5f)
        {
          m_pMainSystem->CameraRotateXUp();
        }
        if(sy < -0.5f)
        {
          m_pMainSystem->CameraRotateXDown();
        }

        // モード切替
        if(pButton->IsTrigger(gfl2::ui::BUTTON_Y))
        {
          const int MODE_MAX = 2;

          m_ChangeMode++;
          if(m_ChangeMode > MODE_MAX-1) m_ChangeMode = 0;
        }

        if(m_ChangeMode == 0)
        {
          // ズームイン/アウト
          if(pButton->IsHold(gfl2::ui::BUTTON_UP)) m_pMainSystem->CameraZoomIn();
          if(pButton->IsHold(gfl2::ui::BUTTON_DOWN)) m_pMainSystem->CameraZoomOut();
          // トレーナー回転
          if(pButton->IsHold(gfl2::ui::BUTTON_LEFT)) m_pMainSystem->TrainerRotateLeft();
          if(pButton->IsHold(gfl2::ui::BUTTON_RIGHT)) m_pMainSystem->TrainerRotateRight();
        }
        else if(m_ChangeMode == 1)
        {
          // 高さ変更
          if(pButton->IsHold(gfl2::ui::BUTTON_UP)) m_pMainSystem->CameraHeightUp();
          if(pButton->IsHold(gfl2::ui::BUTTON_DOWN)) m_pMainSystem->CameraHeightDown();
          // フォーカス変更
          if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT)) m_pMainSystem->CameraFocusToPlayer();
          if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT)) m_pMainSystem->CameraFocusToPokemon();
        }
      }
      else
      {
        // ポケモン座標変更
        gfl2::math::Vector2 touchPos(pTouch->GetX(),pTouch->GetY());
        m_pMainSystem->SetPokePosition(touchPos,true);
        // ポケモン角度変更
        gfl2::ui::VectorDevice* pStick = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager()->GetStick(0);
        f32 sx = pStick->GetX();
        f32 sy = pStick->GetY();

        if(sx > 0.5f)
        {
          m_pMainSystem->PokemonRotateRight();
        }
        if(sx < -0.5f)
        {
          m_pMainSystem->PokemonRotateLeft();
        }
      }
    }
    break;
  }
  case 1:
  {
    enum{
      INIT,
      CHANGE_POKE,
      CHANGE_POKE_WAIT,
      WAIT_BATTLE_MOTION_WAIT,
      PLAY_BATTLE_MOTION,
      WAIT_KAWAIGARI_MOTION_WAIT,
      PLAY_KAWAIGARI_MOTION,
      WAIT_SLEEP,
      WAIT_WAKE,
      NEXT_POKE
    };

    // 自動エージングテスト
    switch(m_SubSeq)
    {
    case INIT:
      m_PokeSimpleParam.monsNo = MONSNO_HUSIGIDANE;
      m_PokeSimpleParam.formNo = 0;
      m_RealFormNo = 0;
      m_PokeSimpleParam.sex = pml::SEX_MALE;
      m_PokeSimpleParam.isEgg = false;
      m_PokeSimpleParam.isRare = false;
      m_PokeSimpleParam.perRand = 0;

      m_pAsingBattleMotionList = m_pMainSystem->DebugGetPokeCanPlayBattleMotionList();
      m_pAsingKawaigariMotionList = m_pMainSystem->DebugGetPokeCanPlayKawaigariMotionList();

      m_pMainSystem->DebugSetupAsing();

      m_SubSeq++;
      // fall through

    case CHANGE_POKE:
      m_pMainSystem->StartChangePokeModel(m_PokeSimpleParam);

      m_SubSeq++;
      // fall through

    case CHANGE_POKE_WAIT:
      if(m_pMainSystem->IsFinishChangePokeModel())
      {
        m_pMainSystem->ChangePokeMotionType(App::FinderStudio::Viewer::POKE_MOTION_TYPE_BATTLE,true);
        m_WaitCnt = 0;

        m_pMainSystem->SetPokePosition(gfl2::math::Vector2(50,120));

        m_SubSeq++;
      }
      break;

    case WAIT_BATTLE_MOTION_WAIT:
      m_WaitCnt++;

      if(m_WaitCnt > 90)
      {
        m_MotionListIdx = 0;
        m_pMainSystem->DebugPlayPokeBattleMotion(m_pAsingBattleMotionList[m_MotionListIdx]);

        m_SubSeq++;
      }
      break;

    case PLAY_BATTLE_MOTION:
      if(m_pMainSystem->IsFinishPokeMotion())
      {
        m_MotionListIdx++;
        if(m_pAsingBattleMotionList[m_MotionListIdx] == PokeTool::MODEL_ANIME_ERROR || 
          m_MotionListIdx >= PokeTool::MODEL_ANIME_BATTLE_NUM)
        {
          m_pMainSystem->ChangePokeMotionType(App::FinderStudio::Viewer::POKE_MOTION_TYPE_KAWAIGARI,true);
          m_pMainSystem->SetPokePosition(gfl2::math::Vector2(50,120));
          m_WaitCnt = 0;

          m_SubSeq++;
        }
        else
        {
          m_pMainSystem->DebugPlayPokeBattleMotion(m_pAsingBattleMotionList[m_MotionListIdx]);
        }
      }
      break;

    case WAIT_KAWAIGARI_MOTION_WAIT:
      m_WaitCnt++;

      if(m_WaitCnt > 90)
      {
        m_MotionListIdx = 0;
        m_pMainSystem->DebugPlayPokeKawaigariMotion(m_pAsingKawaigariMotionList[m_MotionListIdx]);

        m_SubSeq++;
      }
      break;

    case PLAY_KAWAIGARI_MOTION:
      if(m_pMainSystem->IsFinishPokeMotion())
      {
        m_MotionListIdx++;
        if(m_pAsingKawaigariMotionList[m_MotionListIdx] == PokeTool::MODEL_ANIME_ERROR ||
          m_MotionListIdx >= PokeTool::MODEL_ANIME_KAWAIGARI_NUM)
        {
          if(m_pMainSystem->DebugIsPokeCanSleep() && m_pMainSystem->DebugSleepPoke())
          {
            m_WaitCnt = 0;
            m_SubSeq++;
            break;
          }
          else
          {
            m_SubSeq = NEXT_POKE;
          }
        }
        else
        {
          m_pMainSystem->DebugPlayPokeKawaigariMotion(m_pAsingKawaigariMotionList[m_MotionListIdx]);
        }
      }
      break;

    case WAIT_SLEEP:
      m_WaitCnt++;

      if(m_WaitCnt > 120 && m_pMainSystem->DebugIsPokeCanWake())
      {
        m_pMainSystem->DebugWakePoke();

        m_SubSeq++;
      }
      break;

    case WAIT_WAKE:
      if(m_pMainSystem->IsFinishPokeMotion())
      {
        m_SubSeq++;
      }
      break;

    case NEXT_POKE:
      m_pPersonalData->LoadData(m_PokeSimpleParam.monsNo,m_PokeSimpleParam.formNo);
      int formMax = m_pPersonalData->GetParam(pml::personal::PARAM_ID_form_max);
      
      m_PokeSimpleParam.formNo++;
      m_RealFormNo++;
      if(m_RealFormNo >= formMax)
      {
        m_PokeSimpleParam.formNo = 0;
        m_RealFormNo = 0;
        m_PokeSimpleParam.monsNo = static_cast<MonsNo>(m_PokeSimpleParam.monsNo + 1);

        if(m_PokeSimpleParam.monsNo > MONSNO_END)
        {
          m_SubSeq = 0;
          m_MainSeq = 0;
        }
      }

      m_pMainSystem->DebugCheckMonsNoFormNoCombination(m_PokeSimpleParam.monsNo,&m_PokeSimpleParam.formNo);
      m_SubSeq = CHANGE_POKE;
      break;
    }

    if(pButton->IsTrigger(gfl2::ui::BUTTON_R))
    {
      m_MainSeq--;
      m_SubSeq = 0;
    }

    break;
  }
    
  }

//   if(pButton->IsTrigger(gfl2::ui::BUTTON_L))
//   {
//     m_pMainSystem->ChangeCaptureMode(CAPTURE_MODE_POKE);
//   }
// 
//   if(pButton->IsTrigger(gfl2::ui::BUTTON_R))
//   {
//     m_pMainSystem->ChangeCaptureMode(CAPTURE_MODE_BOTH);
//   }
// 
//   if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
//   {
//     if(m_pMainSystem->IsFinishChangePokeModel())
//     {
//       PokeTool::SimpleParam param((MonsNo)103,(pml::FormNo)1,(pml::Sex)0,0,0,0);
//       m_pMainSystem->StartChangePokeModel(param);
//     }
//   }
// 
//   if(pButton->IsTrigger(gfl2::ui::BUTTON_B))
//   {
//     if(m_pMainSystem->IsFinishChangePokeModel())
//     {
//       PokeTool::SimpleParam param((MonsNo)103,(pml::FormNo)0,(pml::Sex)0,0,0,0);
//       m_pMainSystem->StartChangePokeModel(param);
//     }
//   }


//   if(pButton->IsTrigger(gfl2::ui::BUTTON_LEFT))
//   {
//     if(m_pMainSystem->IsFinishChangeBackGround())//   if(pButton->IsTrigger(gfl2::ui::BUTTON_X))
//   {
//     if(m_pMainSystem->IsFinishChangeDressUp())
//     {
//       poke_3d::model::DressUpParam param;
//       System::DressUp::GetDressUpParamDefaultHeroine(&param);
//       m_pMainSystem->StartChangeDressUp(param);
//     }
//   }
// 
//   if(pButton->IsTrigger(gfl2::ui::BUTTON_Y))
//   {
//     if(m_pMainSystem->IsFinishChangeDressUp())
//     {
//       poke_3d::model::DressUpParam param;
//       System::DressUp::GetDressUpParamDefaultHero(&param);
//       m_pMainSystem->StartChangeDressUp(param);
//     }
//   }

//     {
//       m_pMainSystem->StartChangeBackGround(0,TIME_ZONE_EVENING);
//     }
//   }
// 
//   if(pButton->IsTrigger(gfl2::ui::BUTTON_RIGHT))
//   {
//     if(m_pMainSystem->IsFinishChangeBackGround())
//     {
//       m_pMainSystem->StartChangeBackGround(4,TIME_ZONE_NIGHT);
//     }
//   }*/

  return gfl2::proc::RES_CONTINUE;
}

/**
* @brief	PROCの描画処理(下準備)
* @param pManager   プロセス管理マネージャー
* 登録したPROCによる、ディスプレイバッファに依存しない描画処理を実行します。
* 例) 立体視時の描画コマンドのセーブ、オフスクリーンバッファへの描画
*/
void FinderStudioTestProc::PreDrawFunc(gfl2::proc::Manager* pManager)
{
}

/**
* @brief	PROCの描画処理
* @param pManager   プロセス管理マネージャー
* @param displayNo  描画対象画面
* 登録したPROCによる、フレームバッファへの描画を実行します。
* ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
*/
void FinderStudioTestProc::DrawFunc(gfl2::proc::Manager* pManager,gfl2::gfx::CtrDisplayNo displayNo)
{
  m_pMainSystem->Draw(displayNo);
}

GFL_NAMESPACE_END(HaradaTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG