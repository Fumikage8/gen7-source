//==============================================================================
/**
 @file    ZukanViewPokeDialogue.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.23
 @brief   図鑑のポケモンモデルを表示した対話のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <str/include/gfl2_StrBuf.h>
#include <Layout/include/gfl2_Layout.h>
#include <Fade/include/gfl2_FadeManager.h>

// nijiのインクルード
#include <System/include/GflUse.h>
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/AppRenderingManager.h>
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Util/app_util_2d.h>

#include <Sound/include/Sound.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/message/msg_zukan.h>
#include <niji_conv_header/message/msg_zukan_hyouka.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"
#include "ZukanFrameSceneBase.h"

#include "ZukanViewPokeDialogue.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPokeDialogueResultListener
 @brief     対話結果のリスナー
 */
//==============================================================================
ZukanPokeDialogueResultListener::ZukanPokeDialogueResultListener(ZukanFrameSceneBaseCell* frameCell)
  : ZukanViewDialogue::ResultListener(),
    m_frameCell(frameCell)
{}
ZukanPokeDialogueResultListener::~ZukanPokeDialogueResultListener()
{}

void ZukanPokeDialogueResultListener::Req(s32 reqId)
{
  m_frameCell->Req(reqId);
}




//==============================================================================
/**
 @class     ZukanViewPokeDialogueListener
 @brief     図鑑のポケモンモデルを表示した対話のビュー
 */
//==============================================================================
class ZukanViewPokeDialogueListener : public app::ui::UIInputListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewPokeDialogueListener);

public:
  ZukanViewPokeDialogueListener(ZukanViewPokeDialogue* view);
  virtual ~ZukanViewPokeDialogueListener();

public:
  enum InputState
  {
    INPUT_STATE_RELEASE,  //!< 他のviewに解放中
    INPUT_STATE_OCCUPY,   //!< このviewが占有しているが、入力は受け付けていない
    INPUT_STATE_ENABLE,   //!< このviewが占有していて、入力を受け付けている
  };
  void SetInputState(InputState inputState);

public:
  ZukanViewPokeDialogue* m_view;
  InputState              m_inputState;
};


//==============================================================================
//
//  ZukanViewPokeDialogueListener
//
//==============================================================================
ZukanViewPokeDialogueListener::ZukanViewPokeDialogueListener(ZukanViewPokeDialogue* view)
  : app::ui::UIInputListener(),
    m_view(view),
    m_inputState(INPUT_STATE_RELEASE)
{}
ZukanViewPokeDialogueListener::~ZukanViewPokeDialogueListener()
{}

void ZukanViewPokeDialogueListener::SetInputState(InputState inputState)
{
  m_inputState = inputState;
}




//==============================================================================
//
//  ZukanViewPokeDialogue
//
//==============================================================================
ZukanViewPokeDialogue::ZukanViewPokeDialogue(const ZukanSceneParam& param)
  : app::ui::UIView( param.appHeap, param.appHeap->GetDeviceAllocator(), param.appHeap->GetDeviceAllocator() ),
    m_param(param),
    m_pokeDialogueParam(),
    m_dialogueContent(NULL),
    m_viewListener(NULL),
    m_viewDialogue(NULL),
    m_startDialogueReq(false),
    m_dispState(DISP_STATE_OFF),
    m_dispStateCount(0)
{
  // ZukanViewPokeDialogueListener
  {
    m_viewListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewPokeDialogueListener(this);
    this->SetInputListener(m_viewListener);  // 親であるapp::ui::UIViewの親であるapp::ui::UIResponderのメンバ

    // 占有するが入力受け付けなし
    m_viewListener->SetInputState(ZukanViewPokeDialogueListener::INPUT_STATE_OCCUPY);
  }

  // UIView
  {
    m_param.parentView->AddSubView(this);  // app::ui::UIViewのメンバ
  }

  // 対話のビュー
  {
    ZukanSceneParam dialogueParam = m_param;
    dialogueParam.parentView = this;
    m_viewDialogue = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewDialogue(dialogueParam);
  }

  // ポケモンモデル
  ZukanUtil::SetPokeModelVisible(m_param.tool, true);  // 必ずポケモンモデルを表示して開始する。前シーンで表示していたらそのまま、表示していなかったらフェードイン前に表示される。
  m_param.tool->SetPokeModelPosType(ZukanTool::POKE_MODEL_POS_TYPE_SMALL_CENTER);
  m_param.tool->ChangePokeModel(m_param.contents->GetCurrDispPokeSimpleParam());
}
ZukanViewPokeDialogue::~ZukanViewPokeDialogue()
{
  // 対話のビュー
  GFL_SAFE_DELETE(m_viewDialogue);

  // UIView
  this->RemoveFromSuperView();  // 親であるapp::ui::UIViewのメンバ

  // ZukanViewPokeDialogueListener
  GFL_SAFE_DELETE(m_viewListener);
}

void ZukanViewPokeDialogue::Update(void)
{
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);

  // 表示状態
  switch(m_dispState)
  {
  case DISP_STATE_OFF:
    {
      break;
    }
  case DISP_STATE_ON_FADE_IN:
    {
      // フェードインを待つ。フェードイン済みのときはすぐに次のステートへ移行する。
      bool isNext = true;
      
      if( !pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE) )
      {
        isNext = false;
      }

      if(isNext)
      {
        this->changeDispState(DISP_STATE_ON_IN);
        //breakしない
      }
      else
      {
        break;
      }
    }
  case DISP_STATE_ON_IN:
    {
      this->changeDispState(DISP_STATE_ON_WORK);
      m_viewListener->SetInputState(ZukanViewPokeDialogueListener::INPUT_STATE_ENABLE);
      break;
    }
  case DISP_STATE_ON_WORK:
    {
      if(m_startDialogueReq)
      {
        m_viewDialogue->StartDialogue(m_dialogueContent);
        m_startDialogueReq = false;
      }
      break;
    }
  case DISP_STATE_ON_FADE_OUT:
    {
      if(pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE))
      {
        m_param.tool->ResetPokeModel();  // フェードアウトするときだけポケモンモデルリセット
        ZukanUtil::SetPokeModelVisible(m_param.tool, false);  // フェードアウトするときだけポケモンモデルを消して終了する。
        this->changeDispState(DISP_STATE_ON_OUT);
      }
      break;
    }
  case DISP_STATE_ON_OUT:
    {
      this->changeDispState(DISP_STATE_WAIT_SAFETY);
      break;
    }
  case DISP_STATE_WAIT_SAFETY:
    {
      m_param.frameCell->Req(m_pokeDialogueParam.frameCellReqIdEnd);
      this->changeDispState(DISP_STATE_WAIT_END);
      //breakしない
    }
  case DISP_STATE_WAIT_END:
    {
      break;
    }
  }

  app::ui::UIView::Update();  // 親であるapp::ui::UIViewのメンバ
}
void ZukanViewPokeDialogue::Draw(gfl2::gfx::CtrDisplayNo displayNo)
{}

void ZukanViewPokeDialogue::SetPokeDialogueParam(const PokeDialogueParam& pokeDialogueParam)
{
  m_pokeDialogueParam = pokeDialogueParam;
}
void ZukanViewPokeDialogue::ReqStartDialogue(ZukanViewDialogue::Content* dialogueContent)
{
  m_dialogueContent = dialogueContent;
  m_startDialogueReq = true;
}

void ZukanViewPokeDialogue::Req(ReqId reqId, s32 reqParam)
{
  switch(reqId)
  {
  case REQ_ID_IN:
    {
      this->startInSeq();
      break;
    }
  case REQ_ID_OUT:
    {
      this->startOutSeq();
      break;
    }
  }
}

void ZukanViewPokeDialogue::startInSeq(void)
{
  // フェードインは必ずすることにした(既にフェードイン済みならすぐに次の状態に移行する)
  //if(m_pokeDialogueParam.fadeInFlag)
  {
    gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
    pFadeManager->RequestIn(gfl2::Fade::DISP_DOUBLE, gfl2::Fade::FADE_TYPE_ALPHA);
   
    this->changeDispState(DISP_STATE_ON_FADE_IN);
  }
  //else
  //{
  //  this->changeDispState(DISP_STATE_ON_IN);
  //}

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewPokeDialogueListener::INPUT_STATE_OCCUPY);

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
  m_param.debugInfo->SetCurrTime(ZukanDebugInfo::TIME_PHASE_ZUKAN_LAUNCH_FADE_IN_START);
  m_param.debugInfo->Print();
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE
}
void ZukanViewPokeDialogue::startOutSeq(void)
{
  if(m_pokeDialogueParam.fadeOutFlag)
  {
    gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
    
    pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_DOUBLE);
    this->changeDispState(DISP_STATE_ON_FADE_OUT);
  }
  else
  {
    this->changeDispState(DISP_STATE_ON_OUT);
  }

  // 占有するが入力受け付けなし
  m_viewListener->SetInputState(ZukanViewPokeDialogueListener::INPUT_STATE_OCCUPY);
}

void ZukanViewPokeDialogue::changeDispState(DispState dispState)
{
  m_dispState = dispState;
  m_dispStateCount = 0;
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

