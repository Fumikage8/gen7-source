//==============================================================================
/**
 @file    GameOverView.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のビュー
 */
//==============================================================================

// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header\app\gameover\GameOver_res2d.h>
#include <niji_conv_header\app\gameover\GameOver_res2d_anim_list.h>
#include <niji_conv_header\app\gameover\GameOver_res2d_pane.h>


// 図鑑のインクルード
#include "App/GameOver/include/GameOverProcParam.h"
#include "GameOverDefine.h"
#include "GameOverDataManager.h"
#include "GameOverFrame.h"

#include "GameOverView.h"

#include "arc_index/message.gaix"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


//==============================================================================
//
//  GameOverViewListener
//
//==============================================================================
GameOverViewListener::GameOverViewListener(GameOverView* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
GameOverViewListener::~GameOverViewListener()
{}

app::ui::UIInputListener::ListenerResult GameOverViewListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( pTouchPanel->IsTouchTrigger() )
  {
    Sound::PlaySE( SEQ_SE_RELEASE );
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult GameOverViewListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  if( pButton->IsTrigger(gfl2::ui::BUTTON_A) || pButton->IsTrigger(gfl2::ui::BUTTON_B) )
  {
    if(pButton->IsTrigger(gfl2::ui::BUTTON_A))
    {
      Sound::PlaySE( SEQ_SE_DECIDE1 );
    }
    else
    {
      Sound::PlaySE( SEQ_SE_CANCEL1 );
    }
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}


//==============================================================================
//
//  GameOverView
//
//==============================================================================
GameOverView::GameOverView(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL)
{
  // GameOverViewListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) GameOverViewListener(this);
    this->SetInputListener(m_viewListener);  // GameOverViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  }
  
  s32 iMsgId = m_param.param->m_iMsgId;
  
  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ関数
    this->CreateGraphicsSystem( m_param.appHeap, false );  // 親であるapp::ui::UIViewのメンバ関数
 
    void* res2dDataBuf = m_param.dataManager->GetDataBuf(GameOverDataManager::DATA_ID_GAMEOVER_RES2D);
    
    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res_0
      {
        res2dDataBuf,
        0,
        app::util::G2DUtil::ATTACH_ROOT
      },
    };

    const app::util::G2DUtil::LytwkData wkTbl[] =
    {
      // wk_0
      {
        // 上画面
        0,  // res_0
        LYTRES_GAMEOVER_RES2D_GAMEOVER_MIN_UPP_000_BFLYT,
        LA_GAMEOVER_RES2D_GAMEOVER_MIN_UPP_000___NUM,
        res2dDataBuf,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
    };

    this->Create2D(  // 親であるapp::ui::UIViewのメンバ関数
        m_param.appHeap,
        NULL,
        1, 
        resTbl,
        GFL_NELEMS(resTbl), 
        wkTbl,
        GFL_NELEMS(wkTbl), 
        GARC_message_gameover_DAT    // app::ui::UIView::UNUSE_MESSAGE_DATA
    );

#if 0 // アニメーションがない場合アサートする.( 0 < max(0) )
    u32 upperAnim = 0;
    {
      upperAnim = LA_GAMEOVER_RES2D_GAMEOVER_MIN_UPP_000___NUM;
    }
    app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
    pG2DUtil->StartAnime(
        0,  // wk_0
        upperAnim
    );
#endif
  }
  
  static const s32  MSGTAGNUM = 1;   ///< msg_all_dead_01～3 に指定されているタグが 1 
  
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  pG2DUtil->SetRegisterPlayerName( MSGTAGNUM, m_param.param->m_pStatus );
  
  SetTextboxPaneMessage( 0, PANENAME_GAMEOVER_MIN_UPP_000_PANE_TEXTBOX_00, iMsgId );
}

GameOverView::~GameOverView()
{
  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ関数
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ関数

  // GameOverViewListener
  GFL_SAFE_DELETE(m_viewListener);
}

void GameOverView::Update(void)
{
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ関数
}
void GameOverView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0 );  // wk_0
//  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 1 );  // wk_1
}

void GameOverView::ReqEnd(void)
{
  m_param.frameCell->ReqEnd();
}


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

