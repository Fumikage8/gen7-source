#if PM_DEBUG


//==============================================================================
/**
 @file    SimpleAppTestFrame.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.07.22
 @brief   シンプルなアプリのフレーム
 */
//==============================================================================


// gfl2のインクルード
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <System/include/ArcIdGetter.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Util/app_util_FileAccessor.h>

// nijiのデータのヘッダーファイルのインクルード
#include <arc_def_index/arc_def.h>
#include <arc_index/kawada_test_data.gaix>

// 本アプリのインクルード
#include "SimpleAppTestView.h"
#include "SimpleAppTestFrame.h"




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest2)
GFL_NAMESPACE_BEGIN(SimpleAppTest)


//==============================================================================
//
//  SimpleAppTestFrameListener
//
//==============================================================================
SimpleAppTestFrameListener::SimpleAppTestFrameListener(void)
  : applib::frame::AppFrameListener()
{}
SimpleAppTestFrameListener::~SimpleAppTestFrameListener()
{}

void SimpleAppTestFrameListener::EndFuncFinish( applib::frame::CellSuper* /*pEndCell*/ )
{}




//==============================================================================
//
//  SimpleAppTestFrameCell
//
//==============================================================================
SimpleAppTestFrameCell::SimpleAppTestFrameCell(void)
  : applib::frame::CellSuper(),
    m_param(NULL),
    m_appLytDataBuf(NULL),
    m_viewParam(NULL),
    m_view(NULL),
    m_endReq(false)
{}
SimpleAppTestFrameCell::~SimpleAppTestFrameCell()
{}

applib::frame::Result SimpleAppTestFrameCell::InitFunc(void)
{
  enum
  {
    SEQ_START_ASYNC_LOAD_DATA ,
    SEQ_IS_DATA_LOADED        ,
    SEQ_CREATE_VIEW           ,
    SEQ_END                   ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;

  int seq = this->GetSubSeq();  // 親であるapplib::frame::CellSuperのメンバ

  switch(seq)
  {
  case SEQ_START_ASYNC_LOAD_DATA :
    {
      GFL_ASSERT(m_param);  // 開発中に気付かせるためのASSERT。本インスタンス生成直後に必ずSetupを呼んで下さい。

      // データのロード開始
      this->startAsyncLoadData();
      ++seq;
      //breakしない
    }
  case SEQ_IS_DATA_LOADED        :
    {
      // データのロード終了待ち
      if( this->isDataLoaded() )
      {
        ++seq;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_CREATE_VIEW           :
    {
      // ビュー生成
      m_viewParam = GFL_NEW(m_param->appHeap->GetDeviceHeap()) SimpleAppTestViewParam();
      m_viewParam->appHeap             = m_param->appHeap;
      m_viewParam->gameManager         = m_param->gameManager;
      m_viewParam->appRenderingManager = m_param->appRenderingManager;
      m_viewParam->appLytDataBuf       = m_appLytDataBuf;
      m_viewParam->frameCell           = this;
      m_view = GFL_NEW(m_param->appHeap->GetDeviceHeap()) SimpleAppTestView(m_viewParam);
      m_view->SetInputEnabledAllView(false);  // SimpleAppTestViewの親であるapp::ui::UIViewのメンバ
      ++seq;
      //breakしない
    }
  case SEQ_END                   :
  default:
    {
      res = applib::frame::RES_FINISH;
      break;
    }
  }

  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ
  
  return res;
}

applib::frame::Result SimpleAppTestFrameCell::UpdateFunc(void)
{
  enum
  {
    SEQ_START_FADE_IN        ,
    SEQ_WAIT_FADE_IN_FINISH  ,
    SEQ_WAIT_END_REQ         ,
    SEQ_START_FADE_OUT       ,
    SEQ_WAIT_FADE_OUT_FINISH ,
    SEQ_END                  ,
  };

  applib::frame::Result res = applib::frame::RES_CONTINUE;

  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  m_view->UpdateTree();

  int seq = this->GetSubSeq();  // 親であるapplib::frame::CellSuperのメンバ

  switch(seq)
  {
  case SEQ_START_FADE_IN        :
    {
      pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
      ++seq;
      //breakしない
    }
  case SEQ_WAIT_FADE_IN_FINISH  :
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        m_view->SetInputEnabledAllView(true);  // SimpleAppTestViewの親であるapp::ui::UIViewのメンバ
        ++seq;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_WAIT_END_REQ         :
    {
      if(m_endReq)
      {
        ++seq;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_START_FADE_OUT       :
    {
      pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
      ++seq;
      //breakしない
    }
  case SEQ_WAIT_FADE_OUT_FINISH :
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        ++seq;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case SEQ_END                  :
  defualut:
    {
      res = applib::frame::RES_FINISH;
      break;
    }
  }

  this->SetSubSeq(seq);  // 親であるapplib::frame::CellSuperのメンバ
  
  return res;
}
void SimpleAppTestFrameCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  m_view->DrawTree(no);
}
applib::frame::Result SimpleAppTestFrameCell::EndFunc(void)
{
  GFL_SAFE_DELETE(m_view);
  GFL_SAFE_DELETE(m_viewParam);
  GflHeapSafeFreeMemory(m_appLytDataBuf);
  return applib::frame::RES_FINISH;
}

void SimpleAppTestFrameCell::Setup(SimpleAppTestFrameParam* param)
{
  m_param = param;
}

void SimpleAppTestFrameCell::ReqEnd(void)
{
  m_view->SetInputEnabledAllView(false);  // SimpleAppTestViewの親であるapp::ui::UIViewのメンバ
  m_endReq = true;
}

void SimpleAppTestFrameCell::startAsyncLoadData(void)
{
  app::util::FileAccessor::FileOpen( NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA), m_param->appHeap->GetDeviceHeap(), true );
  app::util::FileAccessor::FileLoad(
      NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA),
      GARC_kawada_test_data_performance_test_lyt_a_applyt_COMP,
      &m_appLytDataBuf,
      m_param->appHeap->GetDeviceHeap(),
      true,
      128
  );
  app::util::FileAccessor::FileClose( NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA), m_param->appHeap->GetDeviceHeap() );
}
bool SimpleAppTestFrameCell::isDataLoaded(void)
{
  return \
       app::util::FileAccessor::IsFileOpen( NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA) ) \
    && app::util::FileAccessor::IsFileLoad( &m_appLytDataBuf ) \
    && app::util::FileAccessor::IsFileClose( NIJI_ARCID_NUM(ARCID_KAWADA_TEST_DATA) );
}


GFL_NAMESPACE_END(SimpleAppTest)
GFL_NAMESPACE_END(KawadaTest2)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG

