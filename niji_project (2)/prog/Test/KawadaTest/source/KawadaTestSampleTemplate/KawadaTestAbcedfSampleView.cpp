//==============================================================================
/**
 @file    KawadaTestAbcdefSampleView.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    0000.00.00
 @brief   KawadaTest
 */
//==============================================================================


// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <Sound/include/Sound.h>


// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test_pane.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test_anim_list.h>


// KawadaTestのインクルード
#include "KawadaTestAbcdefSampleDataManager.h"
#include "KawadaTestAbcdefSampleFrame.h"

#include "KawadaTestAbcdefSampleView.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
//
//  KawadaTestAbcdefSampleViewListener
//
//==============================================================================
KawadaTestAbcdefSampleViewListener::KawadaTestAbcdefSampleViewListener(KawadaTestAbcdefSampleView* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
KawadaTestAbcdefSampleViewListener::~KawadaTestAbcdefSampleViewListener()
{}

app::ui::UIInputListener::ListenerResult KawadaTestAbcdefSampleViewListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( pTouchPanel->IsTouchTrigger() )
  {
    Sound::PlaySE( SEQ_SE_RELEASE );
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult KawadaTestAbcdefSampleViewListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
//  KawadaTestAbcdefSampleView
//
//==============================================================================
KawadaTestAbcdefSampleView::KawadaTestAbcdefSampleView(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL)
{
  // KawadaTestAbcdefSampleViewListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestAbcdefSampleViewListener(this);
    this->SetInputListener(m_viewListener);  // KawadaTestAbcdefSampleViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  }

  void* appLytBuf = m_param.dataManager->GetDataBuf(KawadaTestAbcdefSampleDataManager::DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT);

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ関数
    this->CreateGraphicsSystem( m_param.appHeap, false );  // 親であるapp::ui::UIViewのメンバ関数
    
    const app::util::G2DUtil::LytResData resTbl[] =
    {
      // res_0
      {
        appLytBuf,
        0,
        static_cast<u32>(app::util::G2DUtil::ATTACH_ROOT)
      },
    };

    const app::util::G2DUtil::LytwkData wkTbl[] =
    {
      // wk_0
      {
        // 上画面
        0,  // res_0
        LYTRES_SIMPLE_LAYOUT_TEST_MIRACLETRADE_MIN_LOW_BFLYT,
        LA_SIMPLE_LAYOUT_TEST_MIRACLETRADE_MIN_LOW___NUM,
        appLytBuf,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
 
      // wk_1
      {
        // 下画面
        0,  // res_0
        LYTRES_SIMPLE_LAYOUT_TEST_MIRACLETRADE_MIN_LOW_BFLYT,
        LA_SIMPLE_LAYOUT_TEST_MIRACLETRADE_MIN_LOW___NUM,
        appLytBuf,
        true,
        app::util::G2DUtil::SETUP_LOWER,
        true
      },
    };

    this->Create2D(  // 親であるapp::ui::UIViewのメンバ関数
        m_param.appHeap,
        NULL,
        2, 
        resTbl,
        GFL_NELEMS(resTbl), 
        wkTbl,
        GFL_NELEMS(wkTbl), 
        app::ui::UIView::USE_APPLYT_MESSAGE_DATA
    );
  }
}
KawadaTestAbcdefSampleView::~KawadaTestAbcdefSampleView()
{
  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ関数
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ関数

  // KawadaTestAbcdefSampleViewListener
  GFL_SAFE_DELETE(m_viewListener);
}

void KawadaTestAbcdefSampleView::Update(void)
{
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ関数
}
void KawadaTestAbcdefSampleView::Draw(void)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  pG2DUtil->AddDrawableLytWk( m_param.appRenderingManager, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWk( m_param.appRenderingManager, 1 );  // wk_1
}

void KawadaTestAbcdefSampleView::ReqEnd(void)
{
  m_param.frameCell->ReqEnd();
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)

