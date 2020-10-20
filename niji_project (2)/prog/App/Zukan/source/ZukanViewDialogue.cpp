//==============================================================================
/**
 @file    ZukanViewDialogue.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の対話のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <Layout/include/gfl2_Layout.h>
#include <str/include/gfl2_StrBuf.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <AppLib/include/Tool/TalkWindow.h>

#include <AppLib/include/Tool/app_tool_2ButtonsView.h>
#include <AppLib/include/Tool/GeneralView/GeneralLowerView.h>

// 図鑑登録チェッカーのインクルード
#include <Sound/include/Sound.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanSceneParam.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanFramePokeName.h"
#include "ZukanFramePokePut.h"

#include "ZukanViewDialogue.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewDialogueListener
 @brief     図鑑の対話のビューのリスナー
 */
//==============================================================================
class ZukanViewDialogueListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewDialogueListener);

public:
  ZukanViewDialogueListener(ZukanViewDialogue* view);
  virtual ~ZukanViewDialogueListener();

  virtual app::ui::UIInputListener::ListenerResult OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch );
  virtual app::ui::UIInputListener::ListenerResult OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick );

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewDialogue* m_view;
  InputState         m_inputState;
};


//==============================================================================
//
//  ZukanViewDialogueListener
//
//==============================================================================
ZukanViewDialogueListener::ZukanViewDialogueListener(ZukanViewDialogue* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewDialogueListener::~ZukanViewDialogueListener()
{}

app::ui::UIInputListener::ListenerResult ZukanViewDialogueListener::OnTouchEvent( gfl2::ui::TouchPanel* pTouchPanel, bool isTouch )
{
  app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
  switch(m_inputState)
  {
  case INPUT_STATE_OCCUPY:
    {
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  case INPUT_STATE_ENABLE:
    {
      if( pTouchPanel->IsTouchTrigger() )
      {
        m_view->Req(ZukanViewDialogue::REQ_ID_TALK_WINDOW_DECIDE);
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}

app::ui::UIInputListener::ListenerResult ZukanViewDialogueListener::OnKeyAction( gfl2::ui::Button* pButton, gfl2::ui::VectorDevice* pKey, gfl2::ui::VectorDevice* pStick )
{
  app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
  switch(m_inputState)
  {
  case INPUT_STATE_OCCUPY:
    {
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  case INPUT_STATE_ENABLE:
    {
      if( pButton->IsTrigger(gfl2::ui::BUTTON_A) || pButton->IsTrigger(gfl2::ui::BUTTON_B) )
      {
        m_view->Req(ZukanViewDialogue::REQ_ID_TALK_WINDOW_DECIDE);
      }
      res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  return res;
}

void ZukanViewDialogueListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
}




//==============================================================================
/**
 @class     ZukanDialogueTwoButtonsViewEventHandler
 @brief     図鑑で使う2ボタンのリスナー
 */
//==============================================================================
class ZukanDialogueTwoButtonsViewEventHandler : public app::tool::TwoButtonsView::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDialogueTwoButtonsViewEventHandler);

public:
  ZukanDialogueTwoButtonsViewEventHandler(ZukanViewDialogue* view);
  virtual ~ZukanDialogueTwoButtonsViewEventHandler();

  virtual void TwoButtonsView_OnEvent(const app::tool::TwoButtonsView::IEventHandler::EventCode eventCode);

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewDialogue* m_view;
  InputState         m_inputState;
};


//==============================================================================
//
//  ZukanDialogueTwoButtonsViewEventHandler
//
//==============================================================================
ZukanDialogueTwoButtonsViewEventHandler::ZukanDialogueTwoButtonsViewEventHandler(ZukanViewDialogue* view)
  : app::tool::TwoButtonsView::IEventHandler(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanDialogueTwoButtonsViewEventHandler::~ZukanDialogueTwoButtonsViewEventHandler()
{}

void ZukanDialogueTwoButtonsViewEventHandler::TwoButtonsView_OnEvent(const app::tool::TwoButtonsView::IEventHandler::EventCode eventCode)
{
  //app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
  switch(m_inputState)
  {
  case INPUT_STATE_OCCUPY:
    {
      //res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  case INPUT_STATE_ENABLE:
    {
      switch(eventCode)
      {
      case app::tool::TwoButtonsView::IEventHandler::EVENT_OnButton1Click: m_view->Req(ZukanViewDialogue::REQ_ID_TWO_BUTTONS_VIEW_DECIDE, ZukanViewDialogue::TWO_BUTTONS_VIEW_ID_YES); break;
      case app::tool::TwoButtonsView::IEventHandler::EVENT_OnButton2Click: m_view->Req(ZukanViewDialogue::REQ_ID_TWO_BUTTONS_VIEW_DECIDE, ZukanViewDialogue::TWO_BUTTONS_VIEW_ID_NO ); break;
      }
      //res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  //return res;
}

void ZukanDialogueTwoButtonsViewEventHandler::SetInputState(InputState inputState)
{
  m_inputState = inputState;
}




//==============================================================================
/**
 @class     ZukanDialogueMultiButtonsViewEventHandler
 @brief     図鑑で使うマルチボタンのリスナー
 */
//==============================================================================
class ZukanDialogueMultiButtonsViewEventHandler : public app::tool::GeneralLowerView::IEventHandler
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanDialogueMultiButtonsViewEventHandler);

public:
  ZukanDialogueMultiButtonsViewEventHandler(ZukanViewDialogue* view);
  virtual ~ZukanDialogueMultiButtonsViewEventHandler();

  virtual void GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode);

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewDialogue* m_view;
  InputState         m_inputState;
};


//==============================================================================
//
//  ZukanDialogueMultiButtonsViewEventHandler
//
//==============================================================================
ZukanDialogueMultiButtonsViewEventHandler::ZukanDialogueMultiButtonsViewEventHandler(ZukanViewDialogue* view)
  : app::tool::GeneralLowerView::IEventHandler(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanDialogueMultiButtonsViewEventHandler::~ZukanDialogueMultiButtonsViewEventHandler()
{}

void ZukanDialogueMultiButtonsViewEventHandler::GeneralLowerView_OnEvent(app::tool::GeneralLowerView* pSender, const app::tool::GeneralLowerView::IEventHandler::EventCode eventCode)
{
  //app::ui::UIInputListener::ListenerResult res = app::ui::UIInputListener::ENABLE_ACCESS;
  switch(m_inputState)
  {
  case INPUT_STATE_OCCUPY:
    {
      //res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  case INPUT_STATE_ENABLE:
    {
      switch(eventCode)
      {
      case app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton0: m_view->Req(ZukanViewDialogue::REQ_ID_MULTI_BUTTONS_VIEW_DECIDE, ZukanViewDialogue::MULTI_BUTTONS_VIEW_ID_BUTTON0); break;
      case app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton1: m_view->Req(ZukanViewDialogue::REQ_ID_MULTI_BUTTONS_VIEW_DECIDE, ZukanViewDialogue::MULTI_BUTTONS_VIEW_ID_BUTTON1); break;
      case app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton2: m_view->Req(ZukanViewDialogue::REQ_ID_MULTI_BUTTONS_VIEW_DECIDE, ZukanViewDialogue::MULTI_BUTTONS_VIEW_ID_BUTTON2); break;
      case app::tool::GeneralLowerView::IEventHandler::EVENT_OnButton3: m_view->Req(ZukanViewDialogue::REQ_ID_MULTI_BUTTONS_VIEW_DECIDE, ZukanViewDialogue::MULTI_BUTTONS_VIEW_ID_BUTTON3); break;
      }
      //res = app::ui::UIInputListener::DISABLE_ACCESS;
      break;
    }
  }
  //return res;
}

void ZukanDialogueMultiButtonsViewEventHandler::SetInputState(InputState inputState)
{
  m_inputState = inputState;
}




//==============================================================================
//
//  ZukanViewDialogue
//
//==============================================================================
ZukanViewDialogue::ZukanViewDialogue(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_content(NULL),
    m_state(STATE_NONE),
    m_viewListener(NULL),
    m_twoButtonsViewEventHandler(NULL),
    m_multiButtonsViewEventHandler(NULL),
    m_closeReq(false),
    m_reqIdAtClose(REQ_ID_NONE),
    m_reqParamAtClose(0)
{
  // ZukanViewDialogueListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewDialogueListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ
  }

  // ZukanDialogueTwoButtonsViewEventHandler
  {
    m_twoButtonsViewEventHandler = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanDialogueTwoButtonsViewEventHandler(this);
  }
  // ZukanDialogueMultiButtonsViewEventHandler
  {
    m_multiButtonsViewEventHandler = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanDialogueMultiButtonsViewEventHandler(this);
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  // レイアウト
  {
    this->SetLayoutTransferArea( gfl2::lyt::MEMORY_AREA_FCRAM );  // 親であるapp::ui::UIViewのメンバ
  }
}
ZukanViewDialogue::~ZukanViewDialogue()
{
  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // ZukanDialogueMultiButtonsViewEventHandler
  GFL_SAFE_DELETE(m_multiButtonsViewEventHandler);
  // ZukanDialogueTwoButtonsViewEventHandler
  GFL_SAFE_DELETE(m_twoButtonsViewEventHandler);
  
  // ZukanViewDialogueListener
  GFL_SAFE_DELETE(m_viewListener);
}

void ZukanViewDialogue::Update(void)
{
  // 状態を処理する
  if(m_state != STATE_NONE)
  {
    // m_stateがSTATE_NONEでないときしかm_contentやpatternにアクセスしてはならない。
    switch(m_content->pattern)
    {
    case PATTERN_QA : this->updateForPatternQA();   break;
    case PATTERN_MES: this->updateForPatternMes();  break;
    case PATTERN_QA4: this->updateForPatternQA4();  break;
    }
  }

  App::Tool::TalkWindow* talkWindow = m_param.tool->GetTalkWindowIfOpen();
  if( talkWindow )
  {
    talkWindow->Update();
  }

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
} 

void ZukanViewDialogue::updateForPatternQA(void)
{
  switch(m_state)
  {
  case STATE_CREATING:
    {
      if( m_param.tool->CreateTalkWindow(m_param.appHeap, m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_TALK_WINDOW_RES2D)) )
      {
        m_param.tool->OpenTalkWindow(
            this,
            m_content->strBuf,
            false
        );
        m_state = STATE_FLOATING;
      }
      break;
    }
  case STATE_FLOATING:
    {
      if( m_param.tool->IsTalkWindowWaiting() )
      {
        m_param.tool->OpenTwoButtonsView(
            this,
            m_content->strBuf0,
            m_content->strBuf1,
            m_content->button1UseBButton,
            m_twoButtonsViewEventHandler
        );
        m_viewListener->SetInputState(ZukanViewDialogueListener::INPUT_STATE_RELEASE);
        m_twoButtonsViewEventHandler->SetInputState(ZukanDialogueTwoButtonsViewEventHandler::INPUT_STATE_ENABLE);
        m_state = STATE_WAITING;
      }
      break;
    }
  case STATE_WAITING:
    {
      // リクエストを処理する
      if(m_closeReq)
      {
        m_closeReq = false;
        m_param.tool->CloseTwoButtonsView();
        m_param.tool->CloseTalkWindow();
        m_state = STATE_NOT_DRAWING; 
      }
      break;
    }
  case STATE_NOT_DRAWING:
    {
      m_state = STATE_DESTROYING; 
      break;
    }
  case STATE_DESTROYING:
    {
      // リクエストを伝える
      if( m_param.tool->DestroyTalkWindow() )  // 次行のフレームへのリクエストの中でフレームからStartDialogueを呼ばれてしまっているので、先にDestroyとm_state設定をする。このリクエストの肥大化はよくないので、リクエストの中でやっていいこととダメなことの整理し、重い処理はUpdateに移すようにしなければならない。
      {
        m_state = STATE_NONE;
        if(m_content->resultListener)
        {
          if(m_reqParamAtClose == TWO_BUTTONS_VIEW_ID_YES) m_content->resultListener->Req(m_content->reqId0);
          else                                             m_content->resultListener->Req(m_content->reqId1);
        }
        else 
        {
          if(m_reqParamAtClose == TWO_BUTTONS_VIEW_ID_YES) m_param.frameCell->Req(m_content->reqId0);
          else                                             m_param.frameCell->Req(m_content->reqId1);
        }
      }
      break;
    }
  }
}

void ZukanViewDialogue::updateForPatternMes(void)
{
  switch(m_state)
  {
  case STATE_CREATING:
    {
      if( m_param.tool->CreateTalkWindow(m_param.appHeap, m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_TALK_WINDOW_RES2D)) )
      {
        m_param.tool->OpenTalkWindow(
            this,
            m_content->strBuf,
            false
        );
        m_state = STATE_FLOATING;
      }
      break;
    }
  case STATE_FLOATING:
    {
      if( m_param.tool->IsTalkWindowWaiting() )
      {
        m_viewListener->SetInputState(ZukanViewDialogueListener::INPUT_STATE_ENABLE);
        m_state = STATE_WAITING;
      }
      break;
    }
  case STATE_WAITING:
    {
      // リクエストを処理する
      if(m_closeReq)
      {
        m_closeReq = false;
        m_param.tool->CloseTalkWindow();  // クローズアニメがあるなら待つようにすべき。
        m_state = STATE_NOT_DRAWING; 
      }
      break;
    }
  case STATE_NOT_DRAWING:
    {
      m_state = STATE_DESTROYING; 
      break;
    }
  case STATE_DESTROYING:
    {
      // リクエストを伝える
      if( m_param.tool->DestroyTalkWindow() )
      {
        m_state = STATE_NONE; 
        if(m_content->resultListener)
        {
          m_content->resultListener->Req(m_content->reqId0);
        }
        else
        {
          m_param.frameCell->Req(m_content->reqId0);
        }
      }
      break;
    }
  }
}

void ZukanViewDialogue::updateForPatternQA4(void)
{
  switch(m_state)
  {
  case STATE_CREATING:
    {
      if( m_param.tool->CreateTalkWindow(m_param.appHeap, m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_TALK_WINDOW_RES2D)) )
      {
        m_param.tool->OpenTalkWindow(
            this,
            m_content->strBuf,
            false
        );
        m_state = STATE_FLOATING;
      }
      break;
    }
  case STATE_FLOATING:
    {
      if( m_param.tool->IsTalkWindowWaiting() )
      {
        m_param.tool->OpenMultiButtonsView(
            this,
            m_content->strBuf0,
            m_content->strBuf1,
            m_content->strBuf2,
            m_content->strBuf3,
            m_content->button3UseBButton,
            m_content->button1UseBButtonDecideSE,
            m_multiButtonsViewEventHandler,
            m_content->cursorPos
        );
        m_viewListener->SetInputState(ZukanViewDialogueListener::INPUT_STATE_RELEASE);
        m_multiButtonsViewEventHandler->SetInputState(ZukanDialogueMultiButtonsViewEventHandler::INPUT_STATE_ENABLE);
        m_state = STATE_WAITING;
      }
      break;
    }
  case STATE_WAITING:
    {
      // リクエストを処理する
      if(m_closeReq)
      {
        m_closeReq = false;
        m_param.tool->CloseMultiButtonsView();
        m_param.tool->CloseTalkWindow();
        m_state = STATE_NOT_DRAWING; 
      }
      break;
    }
  case STATE_NOT_DRAWING:
    {
      m_state = STATE_DESTROYING; 
      break;
    }
  case STATE_DESTROYING:
    {
      // リクエストを伝える
      if( m_param.tool->DestroyTalkWindow() )  // 次行のフレームへのリクエストの中でフレームからStartDialogueを呼ばれてしまっているので、先にDestroyとm_state設定をする。このリクエストの肥大化はよくないので、リクエストの中でやっていいこととダメなことの整理し、重い処理はUpdateに移すようにしなければならない。
      {
        m_state = STATE_NONE;
        s32 reqIdArray[] =
        {
          m_content->reqId0,
          m_content->reqId1,
          m_content->reqId2,
          m_content->reqId3,
        };
        if(m_content->resultListener)
        {
          m_content->resultListener->Req(reqIdArray[m_reqParamAtClose]);
        }
        else 
        {
          m_param.frameCell->Req(reqIdArray[m_reqParamAtClose]);
        }
      }
      break;
    }
  }
}

void ZukanViewDialogue::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{
  App::Tool::TalkWindow* talkWindow = m_param.tool->GetTalkWindowIfOpen();
  if( talkWindow )
  {
    talkWindow->Draw( displayNo, gfl2::lyt::ORDER_NORMAL, 1 );
  }
}

void ZukanViewDialogue::StartDialogue(Content* content)
{
  m_content = content;

  switch(m_content->pattern)
  {
  case PATTERN_QA:
    {
      m_viewListener->SetInputState(ZukanViewDialogueListener::INPUT_STATE_OCCUPY);
      m_state = STATE_CREATING;
      break;
    }
  case PATTERN_MES:
    {
      m_viewListener->SetInputState(ZukanViewDialogueListener::INPUT_STATE_OCCUPY);
      m_state = STATE_CREATING;
      break;
    }
  case PATTERN_QA4:
    {
      m_viewListener->SetInputState(ZukanViewDialogueListener::INPUT_STATE_OCCUPY);
      m_state = STATE_CREATING;
      break;
    }
  }
}

void ZukanViewDialogue::Req(ReqId reqId, s32 reqParam)
{
  switch(m_content->pattern)
  {
  case PATTERN_QA:
    {
      if(reqId == REQ_ID_TWO_BUTTONS_VIEW_DECIDE)
      {
        m_twoButtonsViewEventHandler->SetInputState(ZukanDialogueTwoButtonsViewEventHandler::INPUT_STATE_OCCUPY);
        m_closeReq = true;
      }
      break;
    }
  case PATTERN_MES:
    {
      if(reqId == REQ_ID_TALK_WINDOW_DECIDE)
      {
        Sound::PlaySE(SEQ_SE_MESSAGE);
        m_viewListener->SetInputState(ZukanViewDialogueListener::INPUT_STATE_OCCUPY);
        m_closeReq = true;
      }
      break;
    }
  case PATTERN_QA4:
    {
      if(reqId == REQ_ID_MULTI_BUTTONS_VIEW_DECIDE)
      {
        m_multiButtonsViewEventHandler->SetInputState(ZukanDialogueMultiButtonsViewEventHandler::INPUT_STATE_OCCUPY);
        m_closeReq = true;
      }
      break;
    }
  }
  if(m_closeReq)
  {
    m_reqIdAtClose    = reqId;
    m_reqParamAtClose = reqParam;
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

