//==============================================================================
/**
 @file    KawadaTestAbcdefSampleFrame.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    0000.00.00
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
#include "KawadaTestAbcdefSampleDataManager.h"
#include "KawadaTestAbcdefSampleView.h"

#include "KawadaTestAbcdefSampleFrame.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
//
//  KawadaTestAbcdefSampleFrameListener
//
//==============================================================================
KawadaTestAbcdefSampleFrameListener::KawadaTestAbcdefSampleFrameListener(void)
  : applib::frame::AppFrameListener()
{}
KawadaTestAbcdefSampleFrameListener::~KawadaTestAbcdefSampleFrameListener()
{}

void KawadaTestAbcdefSampleFrameListener::EndFuncFinish( applib::frame::CellSuper* /*pEndCell*/ )
{}




//==============================================================================
//
//  KawadaTestAbcdefSampleFrameCell
//
//==============================================================================

//
// メモ
//
// KawadaTestAbcdefSampleViewの入力可不可制御はKawadaTestAbcdefSampleFrameCellで行う。
// フェードはKawadaTestAbcdefSampleFrameCellで行う。
//

KawadaTestAbcdefSampleFrameCell::KawadaTestAbcdefSampleFrameCell(void)
  : applib::frame::CellSuper(),
    m_param(),
    m_fadeColor(0.0f, 0.0f, 0.0f, 255.0f),
    m_seqCount(0),
    m_view(NULL),
    m_viewDrawFlag(false),
    m_endReq(false)
{}
KawadaTestAbcdefSampleFrameCell::~KawadaTestAbcdefSampleFrameCell()
{}

applib::frame::Result KawadaTestAbcdefSampleFrameCell::InitFunc(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;

  // 最初に画面を真っ黒にしておく
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut(gfl2::Fade::DISP_DOUBLE, &m_fadeColor);

  // データの読み込み待ち
  void* res2dDataBuf = m_param.dataManager->GetDataBuf(KawadaTestAbcdefSampleDataManager::DATA_ID_GHIJKL);
  if(res2dDataBuf)
  {
    KawadaTestAbcdefSampleView::Param param;
    param.appHeap             = m_param.appHeap;
    param.gameManager         = m_param.gameManager;
    param.dataManager         = m_param.dataManager;
    param.frameManager        = m_param.frameManager;
    param.appRenderingManager = m_param.appRenderingManager;
    param.frameCell           = this;
    m_view = GFL_NEW(m_param.appHeap->GetDeviceHeap()) KawadaTestAbcdefSampleView(param);
    m_view->SetInputEnabledAllView(false);  // KawadaTestAbcdefSampleViewの親であるapp::ui::UIViewのメンバ関数
    res = applib::frame::RES_FINISH;
  }

  return res;
}
applib::frame::Result KawadaTestAbcdefSampleFrameCell::UpdateFunc(void)
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
      if(m_seqCount >= 2)  // KawadaTestAbcdefSampleViewのアニメの初期値が反映されるまで待つ
      {
        this->changeSeq(SEQ_START_FADE_IN);
      }
      break;
    }
  case SEQ_START_FADE_IN:
    {
      pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE);
      this->changeSeq(SEQ_WAIT_FADE_IN_END);
      //breakしない
    }
  case SEQ_WAIT_FADE_IN_END:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        m_view->SetInputEnabledAllView(true);  // KawadaTestAbcdefSampleViewの親であるapp::ui::UIViewのメンバ関数
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

  m_viewDrawFlag = true;

  return res;
}
void KawadaTestAbcdefSampleFrameCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  if(m_viewDrawFlag)
  {
    m_view->DrawTree();
    m_viewDrawFlag = false;
  }
}
applib::frame::Result KawadaTestAbcdefSampleFrameCell::EndFunc(void)
{
  GFL_SAFE_DELETE(m_view);

  // このサンプルでは最後に画面を出しておく
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->RequestIn(gfl2::Fade::DISP_DOUBLE, 0);

  return applib::frame::RES_FINISH;
}

void KawadaTestAbcdefSampleFrameCell::Setup(const Param& param)
{
  m_param = param;
}

void KawadaTestAbcdefSampleFrameCell::ReqEnd(void)
{
  m_view->SetInputEnabledAllView(false);  // KawadaTestAbcdefSampleViewの親であるapp::ui::UIViewのメンバ関数
  m_endReq = true;
}

void KawadaTestAbcdefSampleFrameCell::changeSeq(int seq)
{
  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ関数
  m_seqCount = 0;
}


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

