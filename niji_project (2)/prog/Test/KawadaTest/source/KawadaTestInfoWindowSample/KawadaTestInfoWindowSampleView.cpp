//==============================================================================
/**
 @file    KawadaTestInfoWindowSampleView.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.01
 @brief   KawadaTest
 */
//==============================================================================
#if PM_DEBUG

// gfl2のインクルード
#include <str/include/gfl2_MsgData.h>
#include <str/include/gfl2_StrBuf.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_AppLytObject.h>

#include <Sound/include/Sound.h>

#include <Print/include/WordSet.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test_pane.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test_anim_list.h>


// KawadaTestのインクルード
#include "KawadaTestInfoWindowSampleDataManager.h"
#include "KawadaTestInfoWindowSampleFrame.h"

#include "KawadaTestInfoWindowSampleView.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
//
//  KawadaTestInfoWindowSampleViewListener
//
//==============================================================================
KawadaTestInfoWindowSampleViewListener::KawadaTestInfoWindowSampleViewListener(KawadaTestInfoWindowSampleFrameCell* frameCell)
  : app::ui::UIInputListener(),
    m_frameCell(frameCell)
{}
KawadaTestInfoWindowSampleViewListener::~KawadaTestInfoWindowSampleViewListener()
{}

app::ui::UIInputListener::ListenerResult KawadaTestInfoWindowSampleViewListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  GFL_PRINT("KawadaTestInfoWindowSampleViewListener::OnTouchEvent\n");

  if( pTouchPanel->IsTouchTrigger() )
  {
    m_frameCell->ReqInfoWindowBookTypeOpen( gfl2::math::Vector2(pTouchPanel->GetX(), pTouchPanel->GetY()) );
    
    //Sound::PlaySE( SEQ_SE_RELEASE );
    //m_frameCell->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult KawadaTestInfoWindowSampleViewListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  GFL_PRINT("KawadaTestInfoWindowSampleViewListener::OnKeyAction\n");
 
  if( pButton->IsTrigger(gfl2::ui::BUTTON_L) )
  {
    m_frameCell->ReqInfoWindowSheetTypeOpen( gfl2::math::Vector2(160,120) );
  }
  else if( pButton->IsTrigger(gfl2::ui::BUTTON_R) )
  {
    m_frameCell->ReqInfoWindowBookTypeOpen( gfl2::math::Vector2(160,120) );
  }
  else if( pButton->IsTrigger(gfl2::ui::BUTTON_Y) )
  {
    m_frameCell->ReqMenuWindowOpen();
  }
  else if(pButton->IsTrigger(gfl2::ui::BUTTON_X))
  {
    //Sound::PlaySE( SEQ_SE_DECIDE1 );
    Sound::PlaySE( SEQ_SE_CANCEL1 );
    m_frameCell->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}




//==============================================================================
//
//  KawadaTestInfoWindowSampleView
//
//==============================================================================
KawadaTestInfoWindowSampleView::KawadaTestInfoWindowSampleView(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param)
{
  void* appLytBuf = m_param.dataManager->GetDataBuf(KawadaTestInfoWindowSampleDataManager::DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT);

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
KawadaTestInfoWindowSampleView::~KawadaTestInfoWindowSampleView()
{
  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ関数
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ関数
}

void KawadaTestInfoWindowSampleView::Update(void)
{
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ関数
}
void KawadaTestInfoWindowSampleView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 1 );  // wk_1
}

gfl2::str::MsgData* KawadaTestInfoWindowSampleView::GetMsgData(gfl2::fs::ArcFile::ARCID arcIdLang, gfl2::fs::ArcFile::ARCDATID datId) const
{
  if(m_appLytObjArray)  // 親であるapp::ui::UIViewのメンバ変数
  {
    return m_appLytObjArray[0].GetMsgData(arcIdLang, datId);  // res_0
  }
  return NULL;
}
void KawadaTestInfoWindowSampleView::GetMsgWorkObject(print::WordSet** wordSet, gfl2::str::StrBuf** strBuf0, gfl2::str::StrBuf** strBuf1) const
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  *wordSet = m_wordSetForAppLytObj;  // 親であるapp::ui::UIViewのメンバ変数
  *strBuf0 = pG2DUtil->GetTempStrBuf(0);
  *strBuf1 = pG2DUtil->GetTempStrBuf(1);
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
