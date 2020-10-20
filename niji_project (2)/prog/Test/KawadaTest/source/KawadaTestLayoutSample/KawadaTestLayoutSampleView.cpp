//==============================================================================
/**
 @file    KawadaTestLayoutSampleView.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   KawadaTest
 */
//==============================================================================
#if PM_DEBUG

// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_RenderTargetTexture.h>
#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>

#include <Sound/include/Sound.h>


// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test_pane.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/simple_layout_test_anim_list.h>


// KawadaTestのインクルード
#include "KawadaTestLayoutSampleDataManager.h"
#include "KawadaTestLayoutSampleFrame.h"

#include "KawadaTestLayoutSampleView.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
//
//  KawadaTestLayoutSampleViewListener
//
//==============================================================================
KawadaTestLayoutSampleViewListener::KawadaTestLayoutSampleViewListener(KawadaTestLayoutSampleView* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
KawadaTestLayoutSampleViewListener::~KawadaTestLayoutSampleViewListener()
{}

app::ui::UIInputListener::ListenerResult KawadaTestLayoutSampleViewListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( pTouchPanel->IsTouchTrigger() )
  {
    Sound::PlaySE( SEQ_SE_RELEASE );
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult KawadaTestLayoutSampleViewListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
//  KawadaTestLayoutSampleView
//
//==============================================================================
KawadaTestLayoutSampleView::KawadaTestLayoutSampleView(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL)
{
  // KawadaTestLayoutSampleViewListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestLayoutSampleViewListener(this);
    this->SetInputListener(m_viewListener);  // KawadaTestLayoutSampleViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  }

  void* appLytBuf = m_param.dataManager->GetDataBuf(KawadaTestLayoutSampleDataManager::DATA_ID_SIMPLE_LAYOUT_TEST_APPLYT);

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
        app::ui::UIView::UNUSE_MESSAGE_DATA//app::ui::UIView::USE_APPLYT_MESSAGE_DATA
    );
  }

  // テクスチャ差し替え
  {
    if(m_param.texPhysicalAddr)
    {
#if defined(GF_PLATFORM_CTR)
      app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
      gfl2::lyt::LytWk* lytWk_0 = pG2DUtil->GetLayoutWork(0);  // wk_0
      gfl2::lyt::LytPicture* picturePane = lytWk_0->GetPicturePane(PANENAME_MIRACLETRADE_MIN_LOW_PANE_BG_00);
      nw::lyt::Material* mat = picturePane->GetMaterial();
      const nw::lyt::TexMap& texMap = mat->GetTexMap(0);
      nw::lyt::TextureInfo texInfoSrc;
      texMap.GetTextureInfo(&texInfoSrc);
      nw::lyt::TexSize texSize(128, 128);
      nw::lyt::TexSize realTexSize(128, 128);
      nw::lyt::TextureInfo texInfoDst(
          texInfoSrc.GetTextureObject(),
          reinterpret_cast<uptr>(m_param.texPhysicalAddr),
          texSize,
          realTexSize,
          nw::lyt::TEXFORMAT_RGBA8,
          nw::lyt::TEXELROTATION_NONE
      );
      mat->SetTexMap(0, &texInfoDst);
#endif  // GF_PLATFORM_CTR
    }

    if(m_param.renderTargetTexture)
    {
      app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
      gfl2::lyt::LytWk* lytWk_0 = pG2DUtil->GetLayoutWork(0);  // wk_0
      gfl2::lyt::LytPicture* picturePane = lytWk_0->GetPicturePane(PANENAME_MIRACLETRADE_MIN_LOW_PANE_BG_01);
      //nw::lyt::Material* mat = picturePane->GetMaterial();
      //nw::lyt::TextureInfo texInfo;
      gfl2::lyt::LytResTextureInfo texInfo;
    #if 1
      m_param.renderTargetTexture->GetColorTextureInfo(&texInfo);
    #else
      const gfl2::gfx::Texture* gfxTexture = m_param.renderTargetTexture->GetColorTexture();
      app::util::G2DUtil::MakeTextureInfo(&texInfo, gfxTexture);
    #endif
      //mat->SetTexMap(0, &texInfo);
      app::tool::LytTexReplaceSystem::ReplaceTextureStatic(&texInfo, picturePane, 0, 0);
      nw::lyt::Size nwLytSize(m_param.renderTargetTexture->GetRealWidth(), m_param.renderTargetTexture->GetRealHeight());
      picturePane->SetSize(nwLytSize);
    }
  }
}
KawadaTestLayoutSampleView::~KawadaTestLayoutSampleView()
{
  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ関数
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ関数

  // KawadaTestLayoutSampleViewListener
  GFL_SAFE_DELETE(m_viewListener);
}

void KawadaTestLayoutSampleView::Update(void)
{
  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ関数
}
void KawadaTestLayoutSampleView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 1 );  // wk_1
}

void KawadaTestLayoutSampleView::ReqEnd(void)
{
  m_param.frameCell->ReqEnd();
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
