//==============================================================================
/**
 @file    ZukanFrameRegisterIntro.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録の際の導入画面のフレーム
 */
//==============================================================================

// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <str/include/gfl2_StrBuf.h>

#include <Fade/include/gfl2_FadeManager.h>

// poke_libのインクルード
#include <pml/include/pmlib.h>

// nijiのインクルード
#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/message/msg_zukan.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanType.h"
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanUtil.h"
#include "ZukanContents.h"

#include "ZukanFrameRegisterIntro.h"


#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_NONE  (0)
#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_CROSS (1)
#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_ALPHA (2)
#define NIJI_APP_ZUKAN_REGISTER_INTRO_FADE (NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_NONE)  // これが0のときフェードなし、1のときクロスフェード、2のときアルファフェード  // ZukanViewRegisterAlolaTop.cppにもある  // 1より2のほうがきれいだが0にする。


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)



//==============================================================================
//
//  ZukanFrameRegisterIntroCell
//
//==============================================================================
ZukanFrameRegisterIntroCell::ZukanFrameRegisterIntroCell(void)
  : ZukanFrameSceneTemplateCell<ZukanViewPokeDialogue>(),
    m_dialogueResultListener(NULL),
    m_dialogueContent(NULL),
    m_strBuf(NULL),
    m_registerFadeState(REGISTER_FADE_STATE_OFF)
{}
ZukanFrameRegisterIntroCell::~ZukanFrameRegisterIntroCell()
{}

applib::frame::Result ZukanFrameRegisterIntroCell::InitFunc(void)
{
  applib::frame::Result res = ZukanFrameSceneTemplateCell<ZukanViewPokeDialogue>::InitFunc();


  if(res == applib::frame::RES_FINISH)
  {
    m_dialogueResultListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPokeDialogueResultListener(this);
    m_dialogueContent        = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewDialogue::Content();
    m_strBuf                 = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap() );
 
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf;
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_15_00;  // あたらしくとうろくされます
      expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_MONS_NAME;
      expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
 
    {
      m_dialogueContent->pattern        = ZukanViewDialogue::PATTERN_MES;
      m_dialogueContent->resultListener = m_dialogueResultListener;
      m_dialogueContent->strBuf         = m_strBuf;
      m_dialogueContent->reqId0         = REQ_ID_DIALOGUE_END;
    } 
 
    ZukanViewPokeDialogue::PokeDialogueParam pokeDialogueParam;
    pokeDialogueParam.frameCellReqIdEnd = REQ_ID_END;
    pokeDialogueParam.fadeInFlag        = true;
#if NIJI_APP_ZUKAN_REGISTER_INTRO_FADE==NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_NONE || NIJI_APP_ZUKAN_REGISTER_INTRO_FADE==NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_CROSS
    pokeDialogueParam.fadeOutFlag       = false;
#else
    pokeDialogueParam.fadeOutFlag       = true;
#endif
    m_view->SetPokeDialogueParam(pokeDialogueParam);
    
    m_view->ReqStartDialogue(m_dialogueContent);
    m_view->Req(ZukanViewPokeDialogue::REQ_ID_IN); 
  }

  
  return res;
}

applib::frame::Result ZukanFrameRegisterIntroCell::EndFunc(void)
{
  applib::frame::Result res = ZukanFrameSceneTemplateCell<ZukanViewPokeDialogue>::EndFunc();

  if(res == applib::frame::RES_FINISH)
  {
    GFL_SAFE_DELETE(m_strBuf);
    GFL_SAFE_DELETE(m_dialogueContent);
    GFL_SAFE_DELETE(m_dialogueResultListener);
  }

  if( (*(m_param.setupState)) != ZukanType::SETUP_STATE_ALL_FINISHED )
  {
    res = applib::frame::RES_CONTINUE;  // 次の画面に行けるのは準備が完了しているときだけ。  // ViewRegisterFadeのところでチェックしているのでここでのチェックは要らないが、念のためチェックしておく。
  }
  
  return res;
}

applib::frame::Result ZukanFrameRegisterIntroCell::UpdateFunc(void)
{
  applib::frame::Result res = ZukanFrameSceneTemplateCell<ZukanViewPokeDialogue>::UpdateFunc();
  
#if NIJI_APP_ZUKAN_REGISTER_INTRO_FADE==NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_CROSS
  gfl2::Fade::FadeManager* pFadeManager = GFL_SINGLETON_INSTANCE(gfl2::Fade::FadeManager);
#endif

  switch(m_registerFadeState)
  {
  case REGISTER_FADE_STATE_OFF:
    {
      // 何もしない
      break;
    }
  case REGISTER_FADE_STATE_OFF_REQ_IN:
    {
      if( (*(m_param.setupState)) == ZukanType::SETUP_STATE_ALL_FINISHED )  // 次の画面に行けるのは準備が完了しているときだけ。
      {
#if NIJI_APP_ZUKAN_REGISTER_INTRO_FADE==NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_CROSS
        m_param.tool->ResetPokeModel();  // クロスフェードアウトするときだけポケモンモデルリセット
        ZukanUtil::SetPokeModelVisible(m_param.tool, false);  // クロスフェードアウトするときだけポケモンモデルを消して終了する。

        pFadeManager->RequestOut(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_CROSS, &ZukanType::CROSS_FADE_COLOR_START, &ZukanType::CROSS_FADE_COLOR_END, 1);  // ぱっと消す
#endif
        m_param.tool->GetViewRegisterFade()->Req(ZukanViewRegisterFade::REQ_ID_IN);
        m_registerFadeState = REGISTER_FADE_STATE_ON_IN;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case REGISTER_FADE_STATE_ON_IN:
    {
      if(
#if NIJI_APP_ZUKAN_REGISTER_INTRO_FADE==NIJI_APP_ZUKAN_REGISTER_INTRO_FADE_CROSS
          pFadeManager->IsEnd(gfl2::Fade::DISP_DOUBLE)        &&
#endif
          m_param.tool->GetViewRegisterFade()->IsInFinished()    )
      {
        m_view->Req(ZukanViewPokeDialogue::REQ_ID_OUT); 
        m_registerFadeState = REGISTER_FADE_STATE_ON_WORK;
        //breakしない
      }
      else
      {
        break;
      }
    }
  case REGISTER_FADE_STATE_ON_WORK:
    {
      // 何もしない
      break;
    }
  }

  return res;
}

void ZukanFrameRegisterIntroCell::Req(s32 reqId)
{
  ZukanFrameSceneTemplateCell<ZukanViewPokeDialogue>::Req(reqId);
  
  if(!m_endReq)
  {
    switch(reqId)
    {
    case REQ_ID_DIALOGUE_END:
      {
        //m_view->Req(ZukanViewPokeDialogue::REQ_ID_OUT);  ReqはUpdateFuncから出すことにしたのでコメントアウト。
        if(m_registerFadeState == REGISTER_FADE_STATE_OFF)
        {
          *m_param.nextSceneType = ZukanType::SCENE_TYPE_REGISTER_ALOLA_TOP;
          m_registerFadeState = REGISTER_FADE_STATE_OFF_REQ_IN;
        }
        break;
      }
    }
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

