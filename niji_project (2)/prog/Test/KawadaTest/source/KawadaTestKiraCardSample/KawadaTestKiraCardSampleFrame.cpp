//==============================================================================
/**
 @file    KawadaTestKiraCardSampleFrame.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.12.23
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// KawadaTestのインクルード
#include "KawadaTestKiraCardSampleDataManager.h"
#include "KawadaTestKiraCardSampleView.h"

#include "KawadaTestKiraCardSampleFrame.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
//
//  KawadaTestKiraCardSampleFrameListener
//
//==============================================================================
KawadaTestKiraCardSampleFrameListener::KawadaTestKiraCardSampleFrameListener(void)
  : applib::frame::AppFrameListener()
{}
KawadaTestKiraCardSampleFrameListener::~KawadaTestKiraCardSampleFrameListener()
{}

void KawadaTestKiraCardSampleFrameListener::EndFuncFinish( applib::frame::CellSuper* /*pEndCell*/ )
{}




//==============================================================================
//
//  KawadaTestKiraCardSampleFrameCell
//
//==============================================================================

//
// メモ
//
// KawadaTestKiraCardSampleViewの入力可不可制御はKawadaTestKiraCardSampleFrameCellで行う。
// フェードはKawadaTestKiraCardSampleFrameCellで行う。
//

KawadaTestKiraCardSampleFrameCell::KawadaTestKiraCardSampleFrameCell(void)
  : applib::frame::CellSuper(),
    m_param(),
    m_fadeColor(0.0f, 0.0f, 0.0f, 255.0f),
    m_seqCount(0),
    m_view(NULL),
    m_endReq(false)
{}
KawadaTestKiraCardSampleFrameCell::~KawadaTestKiraCardSampleFrameCell()
{}

applib::frame::Result KawadaTestKiraCardSampleFrameCell::InitFunc(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;

  // 最初に画面を真っ黒にしておく
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut(gfl2::Fade::DISP_DOUBLE, &m_fadeColor);

  // データの読み込み待ち
  void* res2dDataBuf = m_param.dataManager->GetDataBuf(KawadaTestKiraCardSampleDataManager::DATA_ID_KIRA_CARD_TEST_APPLYT);
  if(res2dDataBuf)
  {
    KawadaTestKiraCardSampleView::Param param;
    param.appHeap             = m_param.appHeap;
    param.gameManager         = m_param.gameManager;
    param.dataManager         = m_param.dataManager;
    param.frameManager        = m_param.frameManager;
    param.appRenderingManager = m_param.appRenderingManager;
    param.frameCell           = this;
    param.texPhysicalAddr     = m_param.texPhysicalAddr;
    param.renderTargetTexture = m_param.renderTargetTexture;
    m_view = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestKiraCardSampleView(param);
    m_view->SetInputEnabledAllView(false);  // KawadaTestKiraCardSampleViewの親であるapp::ui::UIViewのメンバ関数
    res = applib::frame::RES_FINISH;
  }

  return res;
}
applib::frame::Result KawadaTestKiraCardSampleFrameCell::UpdateFunc(void)
{
  enum
  {
    SEQ_WAIT               = 0,
    SEQ_START_FADE_IN         ,
    SEQ_WAIT_FADE_IN_END      ,
    SEQ_WAIT_END_REQ          ,
    SEQ_START_FADE_OUT        ,
    SEQ_WAIT_FADE_OUT_END     ,
    SEQ_END                   ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;

  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  m_view->UpdateTree();

  switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ関数
  {
  case SEQ_WAIT:
    {
      ++m_seqCount;
      if(m_seqCount >= 2)  // KawadaTestKiraCardSampleViewのアニメの初期値が反映されるまで待つ
      {
        this->changeSeq(SEQ_START_FADE_IN);
      }
      break;
    }
  case SEQ_START_FADE_IN:
    {
      pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
      this->changeSeq(SEQ_WAIT_FADE_IN_END);
      //breakしない
    }
  case SEQ_WAIT_FADE_IN_END:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        m_view->SetInputEnabledAllView(true);  // KawadaTestKiraCardSampleViewの親であるapp::ui::UIViewのメンバ関数
        this->changeSeq(SEQ_WAIT_END_REQ);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_END_REQ:
    {
      if(m_endReq)
      {
        this->changeSeq(SEQ_START_FADE_OUT);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_START_FADE_OUT:
    {
      pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
      this->changeSeq(SEQ_WAIT_FADE_OUT_END);
      //breakしない
    }
  case SEQ_WAIT_FADE_OUT_END:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        this->changeSeq(SEQ_END);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_END:
  default:
    {
      res = applib::frame::RES_FINISH;
      break;
    }
  }

  return res;
}
void KawadaTestKiraCardSampleFrameCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  m_view->DrawTree(no);
}
applib::frame::Result KawadaTestKiraCardSampleFrameCell::EndFunc(void)
{
  GFL_SAFE_DELETE(m_view);

  // このサンプルでは最後に画面を出しておく
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA, 0);

  return applib::frame::RES_FINISH;
}

void KawadaTestKiraCardSampleFrameCell::Setup(const Param& param)
{
  m_param = param;
}

void KawadaTestKiraCardSampleFrameCell::ReqEnd(void)
{
  m_view->SetInputEnabledAllView(false);  // KawadaTestKiraCardSampleViewの親であるapp::ui::UIViewのメンバ関数
  m_endReq = true;
}

void KawadaTestKiraCardSampleFrameCell::changeSeq(int seq)
{
  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ関数
  m_seqCount = 0;
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

