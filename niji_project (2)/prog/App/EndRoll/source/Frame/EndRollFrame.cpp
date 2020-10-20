// ============================================================================
/*
 * @file EndRollFrame.cpp
 * @brief エンドロールアプリのプロセスです。
 * @date 2016.03.16
 * @author uchida_yuto
 */
// ============================================================================
#include "App/EndRoll/source/Frame/EndRollFrame.h"
#include "App/EndRoll/source/EndRollFrameResult.h"

#include <Fade/include/gfl2_FadeManager.h>

//  nijiのインクルード
#include "System/include/ArcIdGetter.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "AppLib/include/Tool/AppToolTimeLimit.h"

//  Viewのインクルード
#include "App/EndRoll/source/System/EndRollViewList.h"
#include "App/EndRoll/source/System/EndRollViewObjects.h"
#include "App/EndRoll/source/View/EndRollTheEndView.h"

#include <Sound/include/Sound.h>

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(EndRoll)
GFL_NAMESPACE_BEGIN(Frame)

EndRollFrame::EndRollFrame(
  App::EndRoll::System::EndRollWork* pEndRollWork ) :
  NetAppLib::System::NetAppFrameBase( pEndRollWork )
{
  _Clear();

  m_pEndRollWork  = pEndRollWork;
}


EndRollFrame::~EndRollFrame()
{
  GFL_SAFE_DELETE( m_pEnterInputTimeLimit );
}


//-----------------------------------------------------------------------------
// NetAppFrameBase
//-----------------------------------------------------------------------------
bool EndRollFrame::startup()
{
  App::EndRoll::System::EndRollViewList*  pViewList       = m_pEndRollWork->GetViewSystemAccessor()->GetViewList();
  pViewList->RemoveAllView();

  App::EndRoll::System::EndRollViewObjects* pViewObjects  = m_pEndRollWork->GetViewSystemAccessor()->GetViewObjects();

  {
    App::EndRoll::View::EndRollTheEndView*  pTheEndView =  pViewObjects->GetTheEndUpperView();
    pViewList->AddUpperView( pTheEndView );

    //  一枚絵を設定
    Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
    pTheEndView->SetImg( pMyStatus->GetDressup() );
  }

  //  入力制限時間管理作成
  {
    m_pEnterInputTimeLimit  = GFL_NEW( m_pEndRollWork->GetAppHeap()->GetDeviceHeap() ) App::Tool::TimeLimit();
  }

  //  フェードをリセット
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  pFadeManager->Reset( gfl2::Fade::DISP_DOUBLE );

  return true;
}


bool EndRollFrame::cleanup()
{
  return true;
}


void EndRollFrame::setListener()
{
}


void EndRollFrame::removeListener()
{
}


void EndRollFrame::updateSequence()
{
  enum
  {
    UPDATE_SEQ_ANIM_EVENT_START = 0,
    UPDATE_SEQ_ANIM_EVENT_WAIT,
    UPDATE_SEQ_INPUT_ENTRY_TIME_LIMIT_START,
    UPDATE_SEQ_INPUT_ENTRY_TIME_LIMIT_WAIT,
    UPDATE_SEQ_INPUT_ENTRY_IDLE,
    CLEANUP_SEQ_FADE_OUT_START,
    CLEANUP_SEQ_FADE_OUT_WAIT,
  };

  switch( m_updateSeq )
  {
    case  UPDATE_SEQ_ANIM_EVENT_START:
    {
      App::EndRoll::System::EndRollViewObjects* pViewObjects  = m_pEndRollWork->GetViewSystemAccessor()->GetViewObjects();

      App::EndRoll::View::EndRollTheEndView*  pTheEndView =  pViewObjects->GetTheEndUpperView();
      pTheEndView->PlayAnime( App::EndRoll::View::EndRollTheEndView::ANIME_VIEW_IN );

      //  BGM再生
      Sound::StartBGMReq( STRM_BGM_EPILOGUE, Sound::BGM_FADE_VERY_FAST );

      // フェードイン
      GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn( gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 0 );
      
      ++m_updateSeq;
      //  すぐに待機チェックに入りたいのでスルー
      //  break;
    }

    case UPDATE_SEQ_ANIM_EVENT_WAIT:
    {
      App::EndRoll::System::EndRollViewObjects* pViewObjects  = m_pEndRollWork->GetViewSystemAccessor()->GetViewObjects();

      App::EndRoll::View::EndRollTheEndView*  pTheEndView =  pViewObjects->GetTheEndUpperView();
      if( pTheEndView->IsPlayAnime( App::EndRoll::View::EndRollTheEndView::ANIME_VIEW_IN ) == false )
      {
        //  終り
        pTheEndView->PlayAnime( App::EndRoll::View::EndRollTheEndView::ANIME_VIEW_LOOP );
        ++m_updateSeq;

        //  すぐに次の処理に入りたいのでスルー
        //  break;
      }
      else
      {
        break;
      }
    }

    case UPDATE_SEQ_INPUT_ENTRY_TIME_LIMIT_START:
    {
      //  指定秒待機
      //  前提直値
      m_pEnterInputTimeLimit->Setup( 5 );
      ++m_updateSeq;

      //  すぐにチェックに入るのでスルー
      //break;
    }

    case UPDATE_SEQ_INPUT_ENTRY_TIME_LIMIT_WAIT:
    {
      m_pEnterInputTimeLimit->Update();
      if( 1.0f <= m_pEnterInputTimeLimit->GetProgression() )
      {
        //  待機終了ですぐに処理に入りたいのでスルー
        ++m_updateSeq;
      }
      else
      {
        break;
      }
    }

    case UPDATE_SEQ_INPUT_ENTRY_IDLE:
    {
      gfl2::ui::DeviceManager* pUIDevManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetUiDeviceManager();
      const gfl2::ui::Button*       button   = pUIDevManager->GetButton( gfl2::ui::DeviceManager::BUTTON_STANDARD );

      //  強制終了
      if( button->IsTrigger( gfl2::ui::BUTTON_A ) )
      {
        ++m_updateSeq;
        //  すぐに処理に入れるようにスルー
      }
      else
      {
        break;
      }
    }

    case CLEANUP_SEQ_FADE_OUT_START:
    {
      //  フェードアウト

      App::EndRoll::System::EndRollViewObjects* pViewObjects  = m_pEndRollWork->GetViewSystemAccessor()->GetViewObjects();

      App::EndRoll::View::EndRollTheEndView*  pTheEndView =  pViewObjects->GetTheEndUpperView();
      pTheEndView->PlayAnime( App::EndRoll::View::EndRollTheEndView::ANIME_VIEW_OUT );

      u32 fadeOutFrameNum = pTheEndView->GetPlayAnimeLastFrame( View::EndRollTheEndView::ANIME_VIEW_OUT ) * 2;
      Sound::FadeOutBGMReq( static_cast<Sound::FadeFrame>( fadeOutFrameNum ) );

      ++m_updateSeq;

      //  すぐに次の処理に入れるのでスルー
      //  break;
    }

    case CLEANUP_SEQ_FADE_OUT_WAIT:
    {
      //  フェードアウト
      App::EndRoll::System::EndRollViewObjects* pViewObjects  = m_pEndRollWork->GetViewSystemAccessor()->GetViewObjects();

      App::EndRoll::View::EndRollTheEndView*  pTheEndView =  pViewObjects->GetTheEndUpperView();
      if( pTheEndView->IsPlayAnime( App::EndRoll::View::EndRollTheEndView::ANIME_VIEW_OUT ) == false )
      {
        //  全体を黒画面で閉じておく
        gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
        pFadeManager->ForceDefaultOut( gfl2::Fade::DISP_DOUBLE );

        exitFrame( App::EndRoll::FRAME_RESULT_BACK );
      }
    }
  }
  
}

GFL_NAMESPACE_END(Frame)
GFL_NAMESPACE_END(EndRoll)
GFL_NAMESPACE_END(App)
