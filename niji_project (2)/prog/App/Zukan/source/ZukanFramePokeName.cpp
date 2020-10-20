//==============================================================================
/**
 @file    ZukanFramePokeName.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の名前入力画面のフレーム
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
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

#include <App/StrInput/include/StrInputFrame.h>

#include <System/include/PokemonVersion.h>

#include <Savedata/include/MyStatus.h>

// nijiのデータのヘッダーファイルのインクルード
#include <niji_conv_header/message/msg_zukan.h>

// 図鑑登録の呼び出しEventのインクルード
#include <App/AppEvent/include/ZukanRegister/ZukanRegisterEvent.h>

// 図鑑登録チェッカーのインクルード
#include <PokeTool/include/PokeToolPokeGet.h>

// 図鑑のインクルード
#include <App/Zukan/include/ZukanProcParam.h>
#include "ZukanDataManager.h"
#include "ZukanTool.h"
#include "ZukanContents.h"
#include "ZukanFrameSceneBase.h"
#include "ZukanViewDialogue.h"

#include "ZukanFramePokeName.h"

// レコード＆ポケモンダイアリー
#include <Savedata/include/Record.h>




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanFramePokeNameCell
//
//==============================================================================
ZukanFramePokeNameCell::ZukanFramePokeNameCell(void)
  : ZukanFrameSceneBaseCell(),
    m_frameStrInput(NULL),
    m_view(NULL),
    m_dialogueResultListener(NULL),
    m_dialogueContent(),
    m_strInputReq(false),
    m_endReq(false)
{
  for(u32 i=0; i<STR_BUF_NUM; ++i)
  {
    m_strBuf[i] = NULL;
  }
}
ZukanFramePokeNameCell::~ZukanFramePokeNameCell()
{}

applib::frame::Result ZukanFramePokeNameCell::InitFunc(void)
{
  if( !m_param.tool->CreateTwoButtonsView(m_param.appHeap, m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_MENU_WINDOW2)) ) return applib::frame::RES_CONTINUE;
  if( !m_param.tool->CreateMultiButtonsView(m_param.appHeap, m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_MENU_WINDOW2)) ) return applib::frame::RES_CONTINUE;


  for(u32 i=0; i<STR_BUF_NUM; ++i)
  {
    m_strBuf[i] = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap() );
  }

  
  m_dialogueResultListener = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanPokeDialogueResultListener(this);


  // 対話内容を作成する
  {
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_QUESTION];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_16_01;
      expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_MONS_NAME;
      expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }

    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER0];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_16_02;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }

    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER1];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_16_03;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }

    m_dialogueContent.pattern           = ZukanViewDialogue::PATTERN_QA;
    m_dialogueContent.resultListener    = m_dialogueResultListener;
    m_dialogueContent.strBuf            = m_strBuf[STR_BUF_QUESTION];
    m_dialogueContent.strBuf0           = m_strBuf[STR_BUF_ANSWER0];
    m_dialogueContent.strBuf1           = m_strBuf[STR_BUF_ANSWER1];
    m_dialogueContent.button1UseBButton = true;
    m_dialogueContent.reqId0            = REQ_ID_QA_ANSWER0;
    m_dialogueContent.reqId1            = REQ_ID_QA_ANSWER1;
  }


  // 名前入力に来るようなポケモンの親は自分。
  {
    bool fastMode = m_param.param->zukanRegisterEventParam->pp->StartFastMode();

    Savedata::MyStatus* myStatus = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerStatus();
    
    gfl2::str::StrBuf tmpBuf( 32, m_param.appHeap->GetDeviceHeap()->GetLowerHandle() );
    myStatus->GetNameString(&tmpBuf);
    m_param.param->zukanRegisterEventParam->pp->SetParentName(&tmpBuf);
    
    u8 myStatusSex = myStatus->GetSex();
    pml::Sex pmlSex = pml::SEX_UNKNOWN;
    if(myStatusSex == PM_MALE)
    {
      pmlSex = pml::SEX_MALE;
    }
    else if(myStatusSex == PM_FEMALE)
    {
      pmlSex = pml::SEX_FEMALE;
    }
    else
    {
      GFL_ASSERT(0);  // ケア済み
      pmlSex = pml::SEX_MALE;
    }
    m_param.param->zukanRegisterEventParam->pp->SetParentSex(pmlSex);
    
    m_param.param->zukanRegisterEventParam->pp->EndFastMode(fastMode);
  }


  void* zukanRes2dDataBuf = m_param.dataManager->GetDataBuf(ZukanDataManager::DATA_ID_ZUKAN_RES2D);
  if(zukanRes2dDataBuf)
  {
    m_param.frameCell = this;
    m_view = GFL_NEW(m_param.appHeap->GetDeviceHeap()) ZukanViewPokeDialogue(m_param);
    m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
   
    ZukanViewPokeDialogue::PokeDialogueParam pokeDialogueParam;
    pokeDialogueParam.fadeInFlag = true;  // フェードイン済みなら何もしないのでtrueで開始する
    m_view->SetPokeDialogueParam(pokeDialogueParam);
    
    m_view->ReqStartDialogue(&m_dialogueContent);
    m_view->Req(ZukanViewPokeDialogue::REQ_ID_IN); 
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。データDATA_ID_ZUKAN_RES2Dの読み込みが終わってからフレームをつくるように呼び出し元でしているので。
  }


  return applib::frame::RES_FINISH;
}
applib::frame::Result ZukanFramePokeNameCell::UpdateFunc(void)
{
  enum
  {
    SEQ_WAIT_START         = 0,

    SEQ_WAIT_REQ              ,

    SEQ_START_STR_INPUT       ,
    SEQ_WAIT_STR_INPUT_END    ,

    SEQ_WAIT_END              ,
    SEQ_END                   ,
  };
 
  applib::frame::Result res = applib::frame::RES_CONTINUE;
 
  switch( this->GetSubSeq() )  // 親であるapplib::frame::CellSuperのメンバ
  {
  case SEQ_WAIT_START:
    {
      ++m_seqCount;
      if(m_seqCount >= 2)  // T_VIEW_SCENEのアニメの初期値が反映されるまで待つ
      {
        m_param.rootView->SetInputEnabledAllView(true);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
        this->changeSeq(SEQ_WAIT_REQ);
      }
      break;
    }

  case SEQ_WAIT_REQ:
    {
      if(m_endReq)
      {
        this->changeSeq(SEQ_WAIT_END);
      }
      else if(m_strInputReq)
      {
        m_strInputReq = false;
        
        this->changeSeq(SEQ_START_STR_INPUT);
      }
      break;
    }

  case SEQ_START_STR_INPUT:
    {
      //m_strBuf[STR_BUF_POKE_NICK_NAME]->Clear();  // 初期設定は空白でいい→空白ダメ
      m_param.param->zukanRegisterEventParam->pp->GetNickName(m_strBuf[STR_BUF_POKE_NICK_NAME]);  // ポケモンのニックネームを入れておくことで、名前入力画面でそれを出せるようになる。
      {
        m_strInputParam.mpGameProcManager      = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetProcManager();
        m_strInputParam.pSaveMisc              = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMisc();
        m_strInputParam.type                   = App::StrInput::TYPE_POKEMON_NAME;
        m_strInputParam.param                  = static_cast<u32>(m_param.param->zukanRegisterEventParam->pp->GetMonsNo());
        m_strInputParam.cp                     = m_param.param->zukanRegisterEventParam->pp;
        m_strInputParam.isFieldSubProc         = false;
        m_strInputParam.language               = System::GetLang();
        m_strInputParam.strBuf                 = m_strBuf[STR_BUF_POKE_NICK_NAME];
        m_strInputParam.strCode                = NULL;
        m_strInputParam.changeFlag             = false;
        m_strInputParam.isInputCancel          = false;
        m_strInputParam.skipInitGraphicMemory  = true;
        m_strInputParam.mpHeap                 = m_param.appHeap;
        m_strInputParam.mpAppRenderingManager  = m_param.appRenderingManager;
      }
      m_frameStrInput = MENU_FRAME_CALL_MODAL<applib::frame::Manager, App::StrInput::StrInputFrame>(m_param.frameManager);
      m_frameStrInput->Setup(&m_strInputParam);
        
      this->changeSeq(SEQ_WAIT_STR_INPUT_END);
      break;
    }
  case SEQ_WAIT_STR_INPUT_END:
    {
      if(m_strInputParam.changeFlag)
      {
        m_param.param->zukanRegisterEventParam->pp->SetNickName(m_strBuf[STR_BUF_POKE_NICK_NAME]);  // ポケモンのニックネームを設定する
        Savedata::IncRecord(Savedata::Record::RECID_NICKNAME);  // 名前をつけた回数＋１
      }
      
      this->changeSeq(SEQ_WAIT_END);
      break;
    }

  case SEQ_WAIT_END:
    {
      if(m_seqCount == 0)
      {
        //this->reqEnd();  // 次のシーンはもう決めてあるのでコメントアウト

        m_view->SetVisible(false);
        ++m_seqCount;
        break;
      }
      else
      {
        this->changeSeq(SEQ_END);
        //breakしない
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
void                  ZukanFramePokeNameCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  m_param.rootView->DrawTree(no);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
}
applib::frame::Result ZukanFramePokeNameCell::EndFunc(void)
{
  if( !m_param.tool->DestroyMultiButtonsView() ) return applib::frame::RES_CONTINUE;
  if( !m_param.tool->DestroyTwoButtonsView() ) return applib::frame::RES_CONTINUE;
  
  GFL_SAFE_DELETE(m_view);

  GFL_SAFE_DELETE(m_dialogueResultListener);
 
  for(u32 i=0; i<STR_BUF_NUM; ++i)
  {
    GFL_SAFE_DELETE(m_strBuf[i]);
  }

  return applib::frame::RES_FINISH;
}

void ZukanFramePokeNameCell::Req(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_QA_ANSWER0:
    {
      this->reqEnd();  // 先に次のシーンを決めておく

      m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。

      ZukanViewPokeDialogue::PokeDialogueParam pokeDialogueParam;
      pokeDialogueParam.frameCellReqIdEnd = REQ_ID_STR_INPUT;
      pokeDialogueParam.fadeOutFlag       = true;  // 名前入力画面へ遷移するためにフェードアウトするのでtrue
      m_view->SetPokeDialogueParam(pokeDialogueParam);

      m_view->Req(ZukanViewPokeDialogue::REQ_ID_OUT); 
      break;
    }
  case REQ_ID_QA_ANSWER1:
    {
      this->reqEnd();  // 先に次のシーンを決めておく
      
      m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。

      ZukanViewPokeDialogue::PokeDialogueParam pokeDialogueParam;
      pokeDialogueParam.frameCellReqIdEnd = REQ_ID_END;
      pokeDialogueParam.fadeOutFlag       = ( *m_param.nextSceneType == ZukanType::SCENE_TYPE_END );  // ここで終わるならフェードアウトするのでtrue
      m_view->SetPokeDialogueParam(pokeDialogueParam);
      
      m_view->Req(ZukanViewPokeDialogue::REQ_ID_OUT); 
      break;
    }
 
  case REQ_ID_STR_INPUT:
    {
      m_strInputReq = true;
      break;
    }
  case REQ_ID_END:
    {
      m_endReq = true;
      break;
    }
  }
}

void ZukanFramePokeNameCell::reqEnd(void)
{
  if( PokeTool::PokeGetUtil::DoesGoToPokePutSelect(
               m_param.param->zukanRegisterEventParam->pp,
               m_param.param->zukanRegisterEventParam->callerType ) )
  {
    *m_param.nextSceneType = ZukanType::SCENE_TYPE_POKE_PUT;
  }
  else
  {
    if( PokeTool::PokeGetUtil::DoesPut_NotSetFlag(m_param.param->zukanRegisterEventParam->pp, m_param.param->zukanRegisterEventParam->callerType) )
    {
      *m_param.nextSceneType = ZukanType::SCENE_TYPE_POKE_PUT;
    }
    else
    {
      if( PokeTool::PokeGetUtil::DoesSetFlag(m_param.param->zukanRegisterEventParam->pp, m_param.param->zukanRegisterEventParam->callerType) )
      {
        //PokeTool::PokeGetUtil::SetFlag(m_param.param->zukanRegisterEventParam->pp);  // SetFlagはZukanContentsをつくるときに済ませてあるのでコメントアウト。
      }
      *m_param.nextSceneType = ZukanType::SCENE_TYPE_END;
    }
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

