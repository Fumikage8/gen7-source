#if PM_DEBUG


//==============================================================================
/**
 @file    SimpleAppTestView.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.22
 @brief   シンプルなアプリのビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/performance_test_lyt_a.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/performance_test_lyt_a_pane.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/performance_test_lyt_a_anim_list.h>

// 本アプリのインクルード
#include "SimpleAppTestFrame.h"
#include "SimpleAppTestView.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)


//==============================================================================
//
//  SimpleAppTestViewListener
//
//==============================================================================
SimpleAppTestViewListener::SimpleAppTestViewListener(SimpleAppTestView* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
SimpleAppTestViewListener::~SimpleAppTestViewListener()
{}

app::ui::UIInputListener::ListenerResult SimpleAppTestViewListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( pTouchPanel->IsTouchTrigger() )
  {
    Sound::PlaySE( SEQ_SE_RELEASE );
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult SimpleAppTestViewListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
//  SimpleAppTestView
//
//==============================================================================
SimpleAppTestView::SimpleAppTestView(SimpleAppTestViewParam* param)
  : app::ui::UIView( param->appHeap, param->appHeap->GetDeviceAllocator(), param->appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL)
{
  // SimpleAppTestViewListener
  {
    m_viewListener = GFL_NEW(m_param->appHeap->GetDeviceHeap()) SimpleAppTestViewListener(this);
    this->SetInputListener(m_viewListener);  // SimpleAppTestViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
    this->CreateGraphicsSystem( m_param->appHeap, false );  // 親であるapp::ui::UIViewのメンバ
 
    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res_0
      {
        m_param->appLytDataBuf,
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
        LYTRES_PERFORMANCE_TEST_LYT_A_A_BFLYT,
        LA_PERFORMANCE_TEST_LYT_A_A___NUM,
        m_param->appLytDataBuf,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
 
      // wk_1
      {
        // 下画面
        0,  // res_0
        LYTRES_PERFORMANCE_TEST_LYT_A_A_BFLYT,
        LA_PERFORMANCE_TEST_LYT_A_A___NUM,
        m_param->appLytDataBuf,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };

    this->Create2D(  // 親であるapp::ui::UIViewのメンバ
        m_param->appHeap,
        NULL,
        2, 
        resTbl,
        GFL_NELEMS(resTbl), 
        wkTbl,
        GFL_NELEMS(wkTbl), 
        app::ui::UIView::UNUSE_MESSAGE_DATA
    );
  }
}
SimpleAppTestView::~SimpleAppTestView()
{
  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ

  // SimpleAppTestViewListener
  GFL_SAFE_DELETE(m_viewListener);
}

void SimpleAppTestView::Update(void)
{
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
}
void SimpleAppTestView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param->appRenderingManager, displayNo, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param->appRenderingManager, displayNo, 1 );  // wk_1
}

void SimpleAppTestView::ReqEnd(void)
{
  m_param->frameCell->ReqEnd();
}

  
GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG
