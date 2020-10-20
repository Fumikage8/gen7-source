//======================================================================
/**
 * @file CharaViewerFrame.cpp
 * @date 2015/07/21 20:32:23
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

// gfl2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

// 図鑑のインクルード
#include <Test/CharaViewer/include/CharaViewerProcParam.h>
#include "../include/CharaViewerDataManager.h"
#include "../include/CharaViewerView.h"

#include "../include/CharaViewerFrame.h"

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)


//==============================================================================
//
//  CharaViewerFrameListener
//
//==============================================================================
CharaViewerFrameListener::CharaViewerFrameListener(void)
  : applib::frame::AppFrameListener()
{}

CharaViewerFrameListener::~CharaViewerFrameListener()
{}

void CharaViewerFrameListener::EndFuncFinish( applib::frame::CellSuper* /*pEndCell*/ )
{}




//==============================================================================
//
//  CharaViewerFrameCell
//
//==============================================================================

//
// メモ
//
// CharaViewerViewの入力可不可制御はCharaViewerFrameCellで行う。
// フェードはCharaViewerFrameCellで行う。
//

CharaViewerFrameCell::CharaViewerFrameCell(void)
  : applib::frame::CellSuper(),
    m_param(),
    m_fadeColor(0.0f, 0.0f, 0.0f, 255.0f),
    m_seqCount(0),
    m_view(NULL),
    m_endReq(false)
{}

CharaViewerFrameCell::~CharaViewerFrameCell()
{}

applib::frame::Result CharaViewerFrameCell::InitFunc(void)
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;

  // 最初に画面を真っ黒にしておく
  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut(gfl2::Fade::DISP_DOUBLE, &m_fadeColor);

  // データの読み込み待ち
  void* res2dDataBuf = m_param.dataManager->GetDataBuf(CharaViewerDataManager::DATA_ID_CHARA_VIEWER_RES2D);

  if(res2dDataBuf)
  {
    CharaViewerView::Param param;
    param.param               = m_param.param;
    param.appHeap             = m_param.appHeap;
    param.gameManager         = m_param.gameManager;
    param.dataManager         = m_param.dataManager;
    param.frameManager        = m_param.frameManager;
    param.appRenderingManager = m_param.appRenderingManager;
    param.charaModelManager   = m_param.charaModelManager;
    param.frameCell           = this;
    m_view = GFL_NEW(m_param.appHeap->GetDeviceHeap()) CharaViewerView(param);
    m_view->SetInputEnabledAllView(false);  // CharaViewerViewの親であるapp::ui::UIViewのメンバ関数
    res = applib::frame::RES_FINISH;

  }

  return res;
}
applib::frame::Result CharaViewerFrameCell::UpdateFunc(void)
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
      if(m_seqCount >= 2)  // CharaViewerViewのアニメの初期値が反映されるまで待つ
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
        m_view->SetInputEnabledAllView(true);  // CharaViewerViewの親であるapp::ui::UIViewのメンバ関数
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
void CharaViewerFrameCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  m_view->DrawTree(no);
}
applib::frame::Result CharaViewerFrameCell::EndFunc(void)
{
  GFL_SAFE_DELETE(m_view);
  return applib::frame::RES_FINISH;
}

void CharaViewerFrameCell::Setup(const Param& param)
{
  m_param = param;
}

void CharaViewerFrameCell::ReqEnd(void)
{
  m_view->SetInputEnabledAllView(false);  // CharaViewerViewの親であるapp::ui::UIViewのメンバ関数
  m_endReq = true;
}

void CharaViewerFrameCell::changeSeq(int seq)
{
  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ関数
  m_seqCount = 0;
}


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
