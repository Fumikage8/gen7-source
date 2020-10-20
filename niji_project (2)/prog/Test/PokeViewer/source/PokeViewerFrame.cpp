//======================================================================
/**
 * @file PokeViewerFrame.cpp
 * @date 2015/11/30 18:01:44
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#include <Fade/include/gfl2_FadeManager.h>

#include "GameSys/include/GameManager.h"

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include "../include/PokeViewerFrame.h"

GFL_NAMESPACE_BEGIN(test)
  GFL_NAMESPACE_BEGIN(pokeviewer)

/**
 * @fn PokeViewerFrameCell()
 * @brief ctor
 */
PokeViewerFrameCell::PokeViewerFrameCell()
  : applib::frame::CellSuper(),
    m_FadeColor(0.0f, 0.0f, 0.0f, 255.0f),
    m_SeqCounter(0),
    m_EndReq(false)
{
}

/**
 * @fn ~PokeViewerFrameCell()
 * @brief dtor
 */
PokeViewerFrameCell::~PokeViewerFrameCell()
{
}

applib::frame::Result PokeViewerFrameCell::InitFunc()
{
  applib::frame::Result res = applib::frame::RES_CONTINUE;

  GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager)->ForceOut(gfl2::Fade::DISP_DOUBLE, &m_FadeColor);

  // データの読み込み待ち
  void* pRes2dDataBuf = m_Param.pDataManager->GetDataBuf(PokeViewerDataManager::DATA_ID_POKEVIEWER_RES2D);

  if(pRes2dDataBuf)
  {
    PokeViewerView::Param param;
    param.pHeap = m_Param.pAppHeap;
    param.pGameManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager);
    param.pDataManager = m_Param.pDataManager;
    param.pFrameManager = m_Param.pFrameManager;
    param.pAppRenderingManager = m_Param.pAppRenderingManager;
    param.pFrameCell = this;
    param.pPokeModelManager = m_Param.pPokeModelManager;

    m_pView = GFL_NEW(m_Param.pAppHeap->GetDeviceHeap()) PokeViewerView(param);
    m_pView->SetInputEnabledAllView(false);

    res = applib::frame::RES_FINISH;
  }

  return res;
}

applib::frame::Result PokeViewerFrameCell::UpdateFunc()
{
  enum
  {
    SEQ_WAIT,
    SEQ_START_FADE_IN,
    SEQ_WAIT_FADE_IN_END,
    SEQ_WAIT_END_REQ,
    SEQ_START_FADE_OUT,
    SEQ_WAIT_FADE_OUT_END,
    SEQ_END
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;

  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  m_pView->UpdateTree();

  switch(this->GetSubSeq())
  {
  case SEQ_WAIT:
    {
      ++m_SeqCounter;
      if(m_SeqCounter >= 2)
      {
        ChangeSeq(SEQ_START_FADE_IN);
      }
      break;
    }
  case SEQ_START_FADE_IN:
    {
      pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
      ChangeSeq(SEQ_WAIT_FADE_IN_END);
      // break;
    }
  case SEQ_WAIT_FADE_IN_END:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        m_pView->SetInputEnabledAllView(true);
        ChangeSeq(SEQ_WAIT_END_REQ);
        // break;
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_END_REQ:
    {
      if(m_EndReq)
      {
        ChangeSeq(SEQ_START_FADE_OUT);
        // break;
      }
      else
      {
        break;
      }
    }
  case SEQ_START_FADE_OUT:
    {
      pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
      ChangeSeq(SEQ_WAIT_FADE_OUT_END);
      //break;
    }
  case SEQ_WAIT_FADE_OUT_END:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        ChangeSeq(SEQ_END);
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

void PokeViewerFrameCell::DrawFunc(gfl2::gfx::CtrDisplayNo displayNo)
{
  m_pView->DrawTree(displayNo);
}

applib::frame::Result PokeViewerFrameCell::EndFunc()
{
  GFL_SAFE_DELETE(m_pView);
  return applib::frame::RES_FINISH;
}

void PokeViewerFrameCell::Setup(const Param& param)
{
  m_Param = param;
}

void PokeViewerFrameCell::ReqEnd()
{
  m_pView->SetInputEnabledAllView(false);
  m_EndReq = true;
}

void PokeViewerFrameCell::ChangeSeq(int seq)
{
  SetSubSeq(seq);
  m_SeqCounter = 0;
}

  GFL_NAMESPACE_END(pokeviewer)
GFL_NAMESPACE_END(test)

#endif // PM_DEBUG
