//==============================================================================
/**
 @file    KawadaTestKiraCardSampleView.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.23
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
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/kira_card_test.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/kira_card_test_pane.h>
#include <niji_conv_header/app/test/kawada_test/kawada_test_data/kira_card_test_anim_list.h>


// KawadaTestのインクルード
#include "KawadaTestKiraCardSampleDataManager.h"
#include "KawadaTestKiraCardSampleFrame.h"

#include "KawadaTestKiraCardSampleView.h"

#include "KawadaTestKiraCardSampleDefine.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
//
//  KawadaTestKiraCardSampleViewListener
//
//==============================================================================
KawadaTestKiraCardSampleViewListener::KawadaTestKiraCardSampleViewListener(KawadaTestKiraCardSampleView* view)
  : app::ui::UIInputListener(),
    m_view(view)
{}
KawadaTestKiraCardSampleViewListener::~KawadaTestKiraCardSampleViewListener()
{}

app::ui::UIInputListener::ListenerResult KawadaTestKiraCardSampleViewListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  if( pTouchPanel->IsTouchTrigger() )
  {
    Sound::PlaySE( SEQ_SE_RELEASE );
    m_view->ReqEnd();
  }
  return app::ui::UIInputListener::DISABLE_ACCESS;
}
app::ui::UIInputListener::ListenerResult KawadaTestKiraCardSampleViewListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
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
//  KawadaTestKiraCardSampleView
//
//==============================================================================
KawadaTestKiraCardSampleView::KawadaTestKiraCardSampleView(const Param& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_viewListener(NULL)
{
  // KawadaTestKiraCardSampleViewListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestKiraCardSampleViewListener(this);
    this->SetInputListener(m_viewListener);  // KawadaTestKiraCardSampleViewの親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ関数
  }

  void* appLytBuf = m_param.dataManager->GetDataBuf(KawadaTestKiraCardSampleDataManager::DATA_ID_KIRA_CARD_TEST_APPLYT);

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
        LYTRES_KIRA_CARD_TEST_PRISM_BFLYT,
        LA_KIRA_CARD_TEST_PRISM___NUM,
        appLytBuf,
        true,
        app::util::G2DUtil::SETUP_UPPER,
        true
      },
 
      // wk_1
      {
        // 下画面
        0,  // res_0
        LYTRES_KIRA_CARD_TEST_PRISM_BFLYT,
        LA_KIRA_CARD_TEST_PRISM___NUM,
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

  // ペイン増殖
  {
    app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
    gfl2::lyt::LytWk* lytWk_0 = pG2DUtil->GetLayoutWork(0);  // wk_0
   
    gfl2::lyt::LytPane* topPane = lytWk_0->GetPane(PANENAME_PRISM_PANE_TOP);
    
    PrismBlock originalPrismBlock;
    originalPrismBlock.blockPane                    = lytWk_0->GetPane(PANENAME_PRISM_PANE_PRISM_BLOCK);
    originalPrismBlock.piecePicPane[PRISM_PIECE_LT] = lytWk_0->GetPicturePane(PANENAME_PRISM_PANE_PRISM_PIECE_00);
    originalPrismBlock.piecePicPane[PRISM_PIECE_RT] = lytWk_0->GetPicturePane(PANENAME_PRISM_PANE_PRISM_PIECE_01);
    originalPrismBlock.piecePicPane[PRISM_PIECE_LB] = lytWk_0->GetPicturePane(PANENAME_PRISM_PANE_PRISM_PIECE_02);
    originalPrismBlock.piecePicPane[PRISM_PIECE_RB] = lytWk_0->GetPicturePane(PANENAME_PRISM_PANE_PRISM_PIECE_03);

    for(s32 row=0; row<PRISM_BLOCK_ROW_NUM; ++row)
    {
      for(s32 col=0; col<PRISM_BLOCK_COL_NUM; ++col)
      {
        PrismBlock* prismBlock = &(m_prismBlock[row][col]);
        prismBlock->blockPane = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::lyt::LytPane(*originalPrismBlock.blockPane);  // 破棄は親が一緒にやってくれる
        for(s32 piece=0; piece<PRISM_PIECE_NUM; ++piece)
        {
          prismBlock->piecePicPane[piece] = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::lyt::LytPicture(*originalPrismBlock.piecePicPane[piece]);  // 破棄は親が一緒にやってくれる
          prismBlock->blockPane->AppendChild(prismBlock->piecePicPane[piece]);
        }
        prismBlock->blockPane->SetTranslate(gfl2::math::VEC2(-192+32*col, 128-32*row));
        topPane->AppendChild(prismBlock->blockPane);
      }
    }

    originalPrismBlock.blockPane->SetVisible(false);
  }
}
KawadaTestKiraCardSampleView::~KawadaTestKiraCardSampleView()
{
  // レイアウト
  this->Delete2D();  // 親であるapp::ui::UIViewのメンバ関数
  this->DeleteGraphicsSystem();  // 親であるapp::ui::UIViewのメンバ関数

  // KawadaTestKiraCardSampleViewListener
  GFL_SAFE_DELETE(m_viewListener);
}

void KawadaTestKiraCardSampleView::Update(void)
{
#if KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO
#ifdef GFL2_UI_USE_GYROSCOPE
  // ジャイロ
  {
    gfl2::ui::Gyroscope* gyro = m_param.gameManager->GetUiDeviceManager()->GetGyroscope(gfl2::ui::DeviceManager::GYROSCOPE_STANDARD);
    if(gyro)
    {
      if(gyro->IsValid())
      {
        gfl2::math::VEC3 angle = gyro->GetAngle();
        gfl2::math::VEC3 normalizedAngle = angle;
        while(normalizedAngle.x <  0.0f) normalizedAngle.x += 1.0f;  while(normalizedAngle.x >= 1.0f) normalizedAngle.x -= 1.0f;
        while(normalizedAngle.y <  0.0f) normalizedAngle.y += 1.0f;  while(normalizedAngle.y >= 1.0f) normalizedAngle.y -= 1.0f;
        while(normalizedAngle.z <  0.0f) normalizedAngle.z += 1.0f;  while(normalizedAngle.z >= 1.0f) normalizedAngle.z -= 1.0f;

        for(s32 row=0; row<PRISM_BLOCK_ROW_NUM; ++row)
        {
          for(s32 col=0; col<PRISM_BLOCK_COL_NUM; ++col)
          {
            for(s32 piece=0; piece<PRISM_PIECE_NUM; ++piece)
            {
              for(u32 vtxIdx=0; vtxIdx<nw::lyt::VERTEXCOLOR_MAX; ++vtxIdx)
              {
                s32 redColorVal = 0;
                s32 blueColorVal = 0;
                s32 whiteColorVal = 0;
                s32 other0ColorVal = 0;
                s32 other1ColorVal = 0;
                s32 other2ColorVal = 0;
                s32 other3ColorVal = 0;

                f32 sheetVal = 0;
                if(piece <= 1)
                {
                  sheetVal = normalizedAngle.x * row * 4.0f;
                }
                else
                {
                  sheetVal = normalizedAngle.x * (row+1) * 4.0f;
                }
                if(piece==0 || piece==2)
                {
                  sheetVal += normalizedAngle.z * col * 3.0f;
                }
                else
                {
                  sheetVal += normalizedAngle.z * (col+1) * 3.0f;
                }
                sheetVal = 0;  // 汚くなるのでやめておく
               
                {
                  f32 val = normalizedAngle.x * 8.0f + sheetVal;
                  while(val <  0.0f) val += 1.0f;  while(val >= 1.0f) val -= 1.0f;
                  s32 colorVal = 509 * val;  if(colorVal >= 256) colorVal = 509 + 1 - colorVal;
                  redColorVal = colorVal;
                }
                {
                  f32 val = (normalizedAngle.x + normalizedAngle.z) * 7.0f + sheetVal;
                  while(val <  0.0f) val += 1.0f;  while(val >= 1.0f) val -= 1.0f;
                  s32 colorVal = 509 * val;  if(colorVal >= 256) colorVal = 509 + 1 - colorVal;
                  blueColorVal = colorVal;
                }
                {
                  f32 val = (normalizedAngle.x + normalizedAngle.z + normalizedAngle.y) * 9.0f + sheetVal;
                  while(val <  0.0f) val += 1.0f;  while(val >= 1.0f) val -= 1.0f;
                  s32 colorVal = 509 * val;  if(colorVal >= 256) colorVal = 509 + 1 - colorVal;
                  whiteColorVal = colorVal;
                }

                {
                  f32 val = (normalizedAngle.x * 1.5f + normalizedAngle.z * 2.0f + normalizedAngle.y) * 13.0f + sheetVal;
                  while(val <  0.0f) val += 1.0f;  while(val >= 1.0f) val -= 1.0f;
                  s32 colorVal = 509 * val;  if(colorVal >= 256) colorVal = 509 + 1 - colorVal;
                  other0ColorVal = colorVal;
                }
                {
                  f32 val = (normalizedAngle.x * 2.5f + normalizedAngle.z * 1.5f + normalizedAngle.y) * 13.0f + sheetVal;
                  while(val <  0.0f) val += 1.0f;  while(val >= 1.0f) val -= 1.0f;
                  s32 colorVal = 509 * val;  if(colorVal >= 256) colorVal = 509 + 1 - colorVal;
                  other1ColorVal = colorVal;
                }
                {
                  f32 val = (normalizedAngle.x * 0.5f + normalizedAngle.z * 1.5f + normalizedAngle.y) * 15.0f + sheetVal;
                  while(val <  0.0f) val += 1.0f;  while(val >= 1.0f) val -= 1.0f;
                  s32 colorVal = 509 * val;  if(colorVal >= 256) colorVal = 509 + 1 - colorVal;
                  other2ColorVal = colorVal;
                }
                {
                  f32 val = (normalizedAngle.x * 2.5f + normalizedAngle.z * 0.5f + normalizedAngle.y) * 15.0f + sheetVal;
                  while(val <  0.0f) val += 1.0f;  while(val >= 1.0f) val -= 1.0f;
                  s32 colorVal = 509 * val;  if(colorVal >= 256) colorVal = 509 + 1 - colorVal;
                  other3ColorVal = colorVal;
                }

                nw::ut::Color8 vtxColor[nw::lyt::VERTEXCOLOR_MAX] =
                {
                  nw::ut::Color8(redColorVal, other0ColorVal, other1ColorVal, 255),  // LT
                  nw::ut::Color8(0, 0, 0, 255),  // RT
                  nw::ut::Color8(whiteColorVal, whiteColorVal, whiteColorVal, 255),  // LB
                  nw::ut::Color8(other2ColorVal, other3ColorVal, blueColorVal, 255),  // RB
                };
                
                m_prismBlock[row][col].piecePicPane[piece]->SetVtxColor(vtxIdx, vtxColor[vtxIdx]);
              }
            }
          }
        }
      }
    }
  }
#endif
#endif  // KAWADA_TEST_KIRA_CARD_SAMPLE_USE_GYRO

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ関数
}
void KawadaTestKiraCardSampleView::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  app::util::G2DUtil* pG2DUtil = this->GetG2DUtil();  // 親であるapp::ui::UIViewのメンバ関数
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 0 );  // wk_0
  pG2DUtil->AddDrawableLytWkForOneFrame( m_param.appRenderingManager, displayNo, 1 );  // wk_1
}

void KawadaTestKiraCardSampleView::ReqEnd(void)
{
  m_param.frameCell->ReqEnd();
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)

#endif // PM_DEBUG
