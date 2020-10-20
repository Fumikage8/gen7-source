//==============================================================================
/**
 @file    ZukanFramePokePut.cpp
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の送り先選択画面のフレーム
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#include <Fade/include/gfl2_FadeManager.h>

// poke_libのインクルード
#include <pml/include/pmlib.h>
//#include <pml/include/item/item.h>                   // pmlib.hに含まれているのでコメントアウト
//#include <niji_conv_header/poke_lib/item/itemsym.h>  // pmlib.hに含まれているのでコメントアウト

// nijiのインクルード
#include <GameSys/include/GameManager.h>
#include <GameSys/include/GameData.h>

#include <AppLib/include/Util/app_util_heap.h>
#include <AppLib/include/Util/app_util_2d.h>
#include <AppLib/include/Util/AppRenderingManager.h>

#include <AppLib/include/Frame/AppFrameManager.h>
#include <AppLib/include/Frame/AppFrameListener.h>
#include <AppLib/include/Frame/AppFrameCellSuper.h>

#include <Savedata/include/MyItemSave.h>

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

#include "ZukanFramePokePut.h"




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
//
//  ZukanFramePokePutCell
//
//==============================================================================
ZukanFramePokePutCell::ZukanFramePokePutCell(void)
  : ZukanFrameSceneBaseCell(),
    m_view(NULL),
    m_dialogueResultListener(NULL),
    m_dialogueContent(),
    m_endReq(false),
    m_state(STATE_QA_POKE_SEND),
    m_pokeSendType(POKE_SEND_TYPE_TARGET_TO_PARTY),
    m_itemSendType(ITEM_SEND_TYPE_TARGET_TO_BAG)
{
  for(u32 i=0; i<STR_BUF_NUM; ++i)
  {
    m_strBuf[i] = NULL;
  }
}
ZukanFramePokePutCell::~ZukanFramePokePutCell()
{}

applib::frame::Result ZukanFramePokePutCell::InitFunc(void)
{
  if( !m_param.tool->CreateTwoButtonsView(m_param.appHeap, m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_MENU_WINDOW2)) ) return applib::frame::RES_CONTINUE;
  if( !m_param.tool->CreateMultiButtonsView(m_param.appHeap, m_param.dataManager->GetZukanRes2dPackElemBuf(ZukanDataManager::ZUKAN_RES2D_PACK_ELEM_ID_MENU_WINDOW2)) ) return applib::frame::RES_CONTINUE;
 

  for(u32 i=0; i<STR_BUF_NUM; ++i)
  {
    m_strBuf[i] = GFL_NEW(m_param.appHeap->GetDeviceHeap()) gfl2::str::StrBuf( STR_BUF_CHAR_NUM_MAX, m_param.appHeap->GetDeviceHeap() );
  }


  // どういう状況でここに来たか判定する
  {
    if(m_param.prevSceneType == ZukanType::SCENE_TYPE_START_ZUKAN_REGISTER_AFTER_POKE_LIST)
    {
      // ポケモンリストからここに来た←手持ちかボックスかの選択をした後
      if(m_param.param->pokeListSelectPos != pml::PokeParty::MEMBER_INDEX_ERROR)
      {
        // ボックスへ送るポケモンを決定した
        if( this->isPokeChangeOK() )
        {
          // 入れ替えてもOK
          m_pokeSendType = POKE_SEND_TYPE_TARGET_TO_PARTY_AND_OTHER_TO_BOX;
       
          if( this->getPokeForBox()->GetItem() == ITEM_DUMMY_DATA )
          {
            // 道具を持っていない
            this->setupForStateMesPokeSendResult();
          }
          else
          {
            // 道具を持っている
            this->setupForStateQAItemSend();
          }
        }
        else
        {
          // 入れ替えるとバトルできなくなるのでNG
          this->setupForStateMesPokeChangeNG();
        }
      }
      else
      {
        // キャンセルした
        this->setupForStateQAPokeSend();
      }
    }
    else if(m_param.prevSceneType == ZukanType::SCENE_TYPE_START_ZUKAN_REGISTER_AFTER_POKE_STATUS)
    {
      // ポケモンステータスからここに来た
      s32 cursorPos = 0;
      if(m_param.param->returnVal == ZukanProcParam::RETURN_VAL_NEXT_POKE_STATUS_NEW)
      {
        cursorPos = 2;  // @fix NMCat[3402] 手持ちポケモンがいっぱいで捕獲した後の画面で起きるカーソル位置の不具合  →ボタンの並び順を変えたときにカーソルの番号を変え忘れていたのを修正。
      }
      else  // (m_param.param->returnVal == ZukanProcParam::RETURN_VAL_NEXT_POKE_STATUS_PARTY)
      {
        cursorPos = 3;  // @fix NMCat[3402] 手持ちポケモンがいっぱいで捕獲した後の画面で起きるカーソル位置の不具合  →ボタンの並び順を変えたときにカーソルの番号を変え忘れていたのを修正。
      }
      this->setupForStateQAPokeSend(cursorPos);
    }
    else
    {
      if( PokeTool::PokeGetUtil::DoesGoToPokePutSelect(
                   m_param.param->zukanRegisterEventParam->pp,
                   m_param.param->zukanRegisterEventParam->callerType ) )
      {
        // 手持ちかボックスかの選択をする
        this->setupForStateQAPokeSend();
      }
      else
      {
        // 配置するか配置済みか
        if( PokeTool::PokeGetUtil::DoesPut_NotSetFlag(
                     m_param.param->zukanRegisterEventParam->pp,
                     m_param.param->zukanRegisterEventParam->callerType ) )
        {
          // 配置する
          // 新たに加わるポケモンは手持ちの空きかボックスの空きに配置する(手持ちかボックスかの選択をしなくていいのに　配置しなければならないなら、空きに配置することになる)
          PokeTool::PokeGetUtil::PutPlace::Tag putPlace = PokeTool::PokeGetUtil::PutPlace::NONE;
          PokeTool::PokeGetUtil::PutSpace::Tag putSpace = PokeTool::PokeGetUtil::PutSpace::NONE;
          if( PokeTool::PokeGetUtil::DoesDecidePlaceSpaceIfPut(
                       m_param.param->zukanRegisterEventParam->pp,
                       m_param.param->zukanRegisterEventParam->callerType,
                       &putPlace,
                       &putSpace ) )
          {
            if( putSpace == PokeTool::PokeGetUtil::PutSpace::FREE )
            {
              if( putPlace == PokeTool::PokeGetUtil::PutPlace::PARTY )
              {
                // 手持ちに入れる
                m_pokeSendType = POKE_SEND_TYPE_TARGET_TO_PARTY;
              }
              else  // putPlace == PokeTool::PokeGetUtil::PutPlace::BOX
              {
                // ボックスに送る
                m_pokeSendType = POKE_SEND_TYPE_TARGET_TO_BOX;
              }
            }
            else
            {
              GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。DoesDecidePlaceSpaceIfPutはFREEしか返さないはず。
              m_pokeSendType = POKE_SEND_TYPE_TARGET_STAY;  // ケアしておく。配置済み扱いにする。
            }
          }
          else
          {
            GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。DoesPut_NotSetFlagがtrueでDoesGoToPokePutSelectがfalseなら、DoesDecidePlaceSpaceIfPutはtrueになるはず。
            m_pokeSendType = POKE_SEND_TYPE_TARGET_STAY;  // ケアしておく。配置済み扱いにする。
          }
        }
        else
        {
          // 配置済み
          GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。DoesPut_NotSetFlagがfalseでDoesGoToPokePutSelectがfalseなら、ZukanFramePokePutに来る前にZukanProcが終わっているはずなので、ここには到達することはないはず。
          // 新たに加わるポケモンは手持ちかボックスに配置済み(交換、進化、タマゴ孵化)
          m_pokeSendType = POKE_SEND_TYPE_TARGET_STAY;
        }

        this->setupForStateMesPokeSendResult();
      }
    }
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
applib::frame::Result ZukanFramePokePutCell::UpdateFunc(void)
{
  enum
  {
    SEQ_WAIT_START         = 0,
    SEQ_WAIT_END_REQ          ,
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
        this->changeSeq(SEQ_WAIT_END_REQ);
      }
      break;
    }
  case SEQ_WAIT_END_REQ:
    {
      if(m_endReq)
      {
        this->changeSeq(SEQ_WAIT_END);
      }
      break;
    }
  case SEQ_WAIT_END:
    {
      if(m_seqCount == 0)
      {
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
void                  ZukanFramePokePutCell::DrawFunc(gfl2::gfx::CtrDisplayNo no)
{
  m_param.rootView->DrawTree(no);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
}
applib::frame::Result ZukanFramePokePutCell::EndFunc(void)
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

void ZukanFramePokePutCell::Req(s32 reqId)
{
  if(reqId == REQ_ID_END)
  {
    m_endReq = true;
  }
  else
  {
    switch(m_state)
    {
    case STATE_QA_POKE_SEND:         this->reqForStateQAPokeSend(reqId);        break;
    case STATE_MES_POKE_SEND_SELECT: this->reqForStateMesPokeSendSelect(reqId); break;
    case STATE_MES_POKE_CHANGE_NG:   this->reqForStateMesPokeChangeNG(reqId);   break;
    case STATE_QA_ITEM_SEND:         this->reqForStateQAItemSend(reqId);        break;
    case STATE_MES_ITEM_SEND_RESULT: this->reqForStateMesItemSendResult(reqId); break;
    case STATE_MES_POKE_SEND_RESULT: this->reqForStateMesPokeSendResult(reqId); break;
    };
  }
}

void ZukanFramePokePutCell::reqForStateQAPokeSend(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_QA_ANSWER0:
    {
      // 手持ちに入れたい→手持ちから入れ替えるポケモンを選ぶために、ポケモンリストを呼び出す
      this->setupForStateMesPokeSendSelect();
      m_view->ReqStartDialogue(&m_dialogueContent);
      break;
    }
  case REQ_ID_QA_ANSWER1:
  case REQ_ID_QA_ANSWER2:
    {
      // ポケモンステータスを呼び出す
      *m_param.nextSceneType = ZukanType::SCENE_TYPE_NEXT_POKE_STATUS;
      
      if(reqId == REQ_ID_QA_ANSWER1)
      {
        // 新参者の強さを見たい
        m_param.param->returnVal = ZukanProcParam::RETURN_VAL_NEXT_POKE_STATUS_NEW;
      }
      else  // (reqId == REQ_ID_QA_ANSWER2)
      {
        // 手持ちの強さを見たい
        m_param.param->returnVal = ZukanProcParam::RETURN_VAL_NEXT_POKE_STATUS_PARTY;
      }

      m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。

      ZukanViewPokeDialogue::PokeDialogueParam pokeDialogueParam;
      pokeDialogueParam.frameCellReqIdEnd = REQ_ID_END;
      pokeDialogueParam.fadeOutFlag       = true;  // ポケモンリスト画面へ遷移するためにフェードアウトするのでtrue
      m_view->SetPokeDialogueParam(pokeDialogueParam);

      m_view->Req(ZukanViewPokeDialogue::REQ_ID_OUT); 
      break;
    }
  case REQ_ID_QA_ANSWER3:
    {
      // ボックスへ送る
      m_pokeSendType = POKE_SEND_TYPE_TARGET_TO_BOX;
     
      if( this->getPokeForBox()->GetItem() == ITEM_DUMMY_DATA )
      {
        // 道具を持っていない
        this->setupForStateMesPokeSendResult();
      }
      else
      {
        // 道具を持っている
        this->setupForStateQAItemSend();
      }
      
      m_view->ReqStartDialogue(&m_dialogueContent);
      break;
    }
  }
}
void ZukanFramePokePutCell::reqForStateMesPokeSendSelect(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_MES:
    {
      // 手持ちに入れたい→手持ちから入れ替えるポケモンを選ぶために、ポケモンリストを呼び出す
      *m_param.nextSceneType = ZukanType::SCENE_TYPE_NEXT_POKE_LIST;

      m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
     
      ZukanViewPokeDialogue::PokeDialogueParam pokeDialogueParam;
      pokeDialogueParam.frameCellReqIdEnd = REQ_ID_END;
      pokeDialogueParam.fadeOutFlag       = true;  // ポケモンリスト画面へ遷移するためにフェードアウトするのでtrue
      m_view->SetPokeDialogueParam(pokeDialogueParam);

      m_view->Req(ZukanViewPokeDialogue::REQ_ID_OUT); 
      break;
    }
  }
}
void ZukanFramePokePutCell::reqForStateMesPokeChangeNG(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_MES:
    {
      this->setupForStateQAPokeSend();
      m_view->ReqStartDialogue(&m_dialogueContent);
      break;
    }
  }
}
void ZukanFramePokePutCell::reqForStateQAItemSend(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_QA_ANSWER0:
    {
      // 道具をバッグに入れる(ゼンリョクピースのときは預かる)
      this->setupForStateMesItemSendResult();
      m_view->ReqStartDialogue(&m_dialogueContent);
      break;
    }
  case REQ_ID_QA_ANSWER1:
    {
      // 道具をバッグに入れない
      this->setupForStateMesPokeSendResult();
      m_view->ReqStartDialogue(&m_dialogueContent);
      break;
    }
  }
}
void ZukanFramePokePutCell::reqForStateMesItemSendResult(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_MES:
    {
      if(m_itemSendType == ITEM_SEND_TYPE_BAG_IS_FULL)
      {
        this->setupForStateQAPokeSend();
        m_view->ReqStartDialogue(&m_dialogueContent);
      }
      else  // m_itemSendType == ITEM_SEND_TYPE_TARGET_TO_BAG
      {
        this->setupForStateMesPokeSendResult();
        m_view->ReqStartDialogue(&m_dialogueContent);
      }
      break;
    }
  }
}
void ZukanFramePokePutCell::reqForStateMesPokeSendResult(s32 reqId)
{
  switch(reqId)
  {
  case REQ_ID_MES:
    {
      *m_param.nextSceneType = ZukanType::SCENE_TYPE_END;

      m_param.rootView->SetInputEnabledAllView(false);  // app::ui::UIViewのメンバ  // ZukanFrameSceneTemplateCellはモーダルウィンドウなのでrootViewのメンバを呼ぶ。
     
      ZukanViewPokeDialogue::PokeDialogueParam pokeDialogueParam;
      pokeDialogueParam.frameCellReqIdEnd = REQ_ID_END;
      pokeDialogueParam.fadeOutFlag       = true;  // ここで終わるからフェードアウトするのでtrue
      m_view->SetPokeDialogueParam(pokeDialogueParam);

      m_view->Req(ZukanViewPokeDialogue::REQ_ID_OUT); 
      break;
    }
  }
}

void ZukanFramePokePutCell::putPartyPokeToBox(gfl2::heap::HeapBase* heap, u32 posInParty, u32* a_tray)
{
  *a_tray = Savedata::BoxPokemon::TRAY_MAX;  // エラー値で初期化しておく

  pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
  const pml::pokepara::PokemonParam* ppInParty = party->GetMemberPointerConst(posInParty);

  Savedata::BOX* svBox = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBOX();
  Savedata::BoxPokemon* svBoxPoke = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetBoxPokemon();

  if( svBoxPoke->GetSpaceCountAll(heap->GetLowerHandle()) > 0 )  // heapは一時的に使用するだけなので後方確保にしておく。
  {
    u32 tray = svBox->GetCurrentTray();
    if( tray >= Savedata::BoxPokemon::TRAY_MAX )
    {
      tray = 0;
    }
    u16 posInTray = 0;
    tray = svBoxPoke->PutPokemonEmptyTrayAll( ppInParty, tray, heap->GetLowerHandle(), &posInTray );  // heapは一時的に使用するだけなので後方確保にしておく。
    if( tray < Savedata::BoxPokemon::TRAY_MAX )
    {
      *a_tray = tray;
    }
    else
    {
      GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。起こり得ないはず。ポケモンを配置しない。
    }
  }
  else
  {
    GFL_ASSERT(0);  // 開発中に気付かせるためのASSERT。空きがない。ポケモンを配置しない。
  }
}


void ZukanFramePokePutCell::setupForStateQAPokeSend(s32 cursorPos)
{
  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_QUESTION];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_18_01;
    expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
    expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }
  
  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER0];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_18_02;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER1];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_21_01;
    expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
    expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER2];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_21_02;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }
   
  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER3];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_18_03;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  m_dialogueContent.pattern           = ZukanViewDialogue::PATTERN_QA4;
  m_dialogueContent.strBuf            = m_strBuf[STR_BUF_QUESTION];
  m_dialogueContent.strBuf0           = m_strBuf[STR_BUF_ANSWER0];
  m_dialogueContent.strBuf1           = m_strBuf[STR_BUF_ANSWER3];
  m_dialogueContent.strBuf2           = m_strBuf[STR_BUF_ANSWER1];
  m_dialogueContent.strBuf3           = m_strBuf[STR_BUF_ANSWER2];
  m_dialogueContent.button3UseBButton = false;
  m_dialogueContent.button1UseBButtonDecideSE = true;
  m_dialogueContent.reqId0            = REQ_ID_QA_ANSWER0;
  m_dialogueContent.reqId1            = REQ_ID_QA_ANSWER3;
  m_dialogueContent.reqId2            = REQ_ID_QA_ANSWER1;
  m_dialogueContent.reqId3            = REQ_ID_QA_ANSWER2;
  m_dialogueContent.cursorPos         = cursorPos;

  m_state = STATE_QA_POKE_SEND;
}
void ZukanFramePokePutCell::setupForStateMesPokeSendSelect(void)
{
  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_QUESTION];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_18_04;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  m_dialogueContent.pattern = ZukanViewDialogue::PATTERN_MES;
  m_dialogueContent.strBuf  = m_strBuf[STR_BUF_QUESTION];
  m_dialogueContent.reqId0  = REQ_ID_MES;

  m_state = STATE_MES_POKE_SEND_SELECT;
}
void ZukanFramePokePutCell::setupForStateMesPokeChangeNG(void)
{
  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_QUESTION];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_30_01;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  m_dialogueContent.pattern = ZukanViewDialogue::PATTERN_MES;
  m_dialogueContent.strBuf  = m_strBuf[STR_BUF_QUESTION];
  m_dialogueContent.reqId0  = REQ_ID_MES;

  m_state = STATE_MES_POKE_CHANGE_NG;
}
void ZukanFramePokePutCell::setupForStateQAItemSend(void)
{
  u16 itemID = this->getPokeForBox()->GetItem();

  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_QUESTION];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
    expandStrBufParam.srcMsgStrId      = msg_zkn_19_01;
    expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
    expandStrBufParam.srcRegBufType[1] = ZukanTool::REG_BUF_TYPE_ITEM_NAME;
    expandStrBufParam.srcCP[0]         = this->getPokeForBox();
    expandStrBufParam.srcItemID[1]     = itemID;
    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  if( item::ITEM_CheckPiece(itemID) )
  {
    // ゼンリョクピースのとき
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER0];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_20_01;
      expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_ITEM_NAME;
      expandStrBufParam.srcItemID[0]     = itemID;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER1];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_20_02;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
  }
  else
  {
    // ゼンリョクピースでないとき
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER0];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_19_02;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_ANSWER1];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
      expandStrBufParam.srcMsgStrId      = msg_zkn_19_03;
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
  }
  
  m_dialogueContent.pattern           = ZukanViewDialogue::PATTERN_QA;
  m_dialogueContent.strBuf            = m_strBuf[STR_BUF_QUESTION];
  m_dialogueContent.strBuf0           = m_strBuf[STR_BUF_ANSWER0];
  m_dialogueContent.strBuf1           = m_strBuf[STR_BUF_ANSWER1];
  m_dialogueContent.button1UseBButton = true;
  m_dialogueContent.reqId0            = REQ_ID_QA_ANSWER0;
  m_dialogueContent.reqId1            = REQ_ID_QA_ANSWER1;
 
  m_state = STATE_QA_ITEM_SEND;
}
void ZukanFramePokePutCell::setupForStateMesItemSendResult(void)
{
  // 道具をバッグに入れる(ゼンリョクピースのときは預かる)
  u16 itemID = this->getPokeForBox()->GetItem();

  // 道具をバッグに入れる(ゼンリョクピースのときは預かる)
  {
    if( item::ITEM_CheckPiece(itemID) )
    {
      // ゼンリョクピースのとき
      this->getPokeForBox()->RemoveItem();  // ポケモンから道具を取る
      this->changePokeFormByNoItem(this->getPokeForBox());  // アイテムを外したときのフォルムチェンジ処理
      
      m_itemSendType = ITEM_SEND_TYPE_TARGET_TO_BAG;
    }
    else
    {
      // ゼンリョクピースでないとき

      // 道具をバッグに入れたい
      Savedata::MyItem* svMyItem = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetMyItem();
      bool isNew    = svMyItem->IsNew(itemID);
      bool addCheck = svMyItem->AddCheck(itemID, 1);  // 1個限定
      if(addCheck)
      {
        // バッグに入れられる
        this->getPokeForBox()->RemoveItem();  // ポケモンから道具を取る
        this->changePokeFormByNoItem(this->getPokeForBox());  // アイテムを外したときのフォルムチェンジ処理
        svMyItem->Add(itemID, 1, isNew);  // バッグに入れる  // 1個限定
     
        m_itemSendType = ITEM_SEND_TYPE_TARGET_TO_BAG;
      }
      else
      {
        // バッグに入れられない
        m_itemSendType = ITEM_SEND_TYPE_BAG_IS_FULL;
      }
    }
  }

  // メッセージをつくる
  {
    ZukanTool::ExpandStrBufParam expandStrBufParam;
    expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_QUESTION];
    expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;

    switch(m_itemSendType)
    {
    case ITEM_SEND_TYPE_TARGET_TO_BAG:
      {
        if( item::ITEM_CheckPiece(itemID) )
        {
          // ゼンリョクピースのとき
          expandStrBufParam.srcMsgStrId      = msg_zkn_20_03;
          expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_ITEM_NAME;
          expandStrBufParam.srcItemID[0]     = itemID;
        }
        else
        {
          // ゼンリョクピースでないとき
          expandStrBufParam.srcMsgStrId      = msg_zkn_19_04;
          expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_ITEM_NAME;
          expandStrBufParam.srcItemID[0]     = itemID;
        }
        break;
      }
    case ITEM_SEND_TYPE_BAG_IS_FULL:
      {
        expandStrBufParam.srcMsgStrId      = msg_zkn_19_05;
        break;
      }
    }

    m_param.tool->ExpandStrBuf(expandStrBufParam);
  }

  m_dialogueContent.pattern = ZukanViewDialogue::PATTERN_MES;
  m_dialogueContent.strBuf  = m_strBuf[STR_BUF_QUESTION];
  m_dialogueContent.reqId0  = REQ_ID_MES;

  m_state = STATE_MES_ITEM_SEND_RESULT;
}
void ZukanFramePokePutCell::setupForStateMesPokeSendResult(void)
{
  // メッセージのセットアップ
  {
    {
      ZukanTool::ExpandStrBufParam expandStrBufParam;
      expandStrBufParam.dstStrBuf        = m_strBuf[STR_BUF_QUESTION];
      expandStrBufParam.srcMsgType       = ZukanTool::MSG_TYPE_ZUKAN;
 
      switch(m_pokeSendType)
      {
      case POKE_SEND_TYPE_TARGET_TO_PARTY:
        {
          expandStrBufParam.srcMsgStrId      = msg_zkn_18_05;
          expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
          expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
          break;
        }
      case POKE_SEND_TYPE_TARGET_TO_PARTY_AND_OTHER_TO_BOX:
        {
          expandStrBufParam.srcMsgStrId      = msg_zkn_18_06;
          expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
          expandStrBufParam.srcRegBufType[1] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
          expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
          expandStrBufParam.srcCP[1]         = this->getPokeForBox();
          break;
        }
      case POKE_SEND_TYPE_TARGET_TO_BOX:
        {
          if(    m_param.param->zukanRegisterEventParam->pp->GetMonsNo() == MONSNO_NIKKOU    // ソルガレオ
              || m_param.param->zukanRegisterEventParam->pp->GetMonsNo() == MONSNO_GEKKOU )  // ルナアーラ
          {
            expandStrBufParam.srcMsgStrId      = msg_zkn_40_01;  // をボックスへてんそうします  ボールをかいしゅうしてください
          }
          else
          {
            expandStrBufParam.srcMsgStrId      = msg_zkn_18_07;  // をボックスへてんそうしました！
          }
          expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
          expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
          break;
        }
      case POKE_SEND_TYPE_TARGET_STAY:
        {
          expandStrBufParam.srcMsgStrId      = msg_zkn_18_05;  // エラーになっているので、てきとーな文言を出しておく。
          expandStrBufParam.srcRegBufType[0] = ZukanTool::REG_BUF_TYPE_POKE_NICK_NAME;
          expandStrBufParam.srcCP[0]         = m_param.param->zukanRegisterEventParam->pp;
          break;
        }
      }
      m_param.tool->ExpandStrBuf(expandStrBufParam);
    }
 
    m_dialogueContent.pattern = ZukanViewDialogue::PATTERN_MES;
    m_dialogueContent.strBuf  = m_strBuf[STR_BUF_QUESTION];
    m_dialogueContent.reqId0  = REQ_ID_MES;
 
    m_state = STATE_MES_POKE_SEND_RESULT;
  }
  // ボックスのトレイ名も出すとなると、ポインタppの指す先が削除される前か後かに注意する必要がある。


  // 配置&フラグ処理
  {
    // 配置
    if( PokeTool::PokeGetUtil::DoesPut_NotSetFlag(
            m_param.param->zukanRegisterEventParam->pp,
            m_param.param->zukanRegisterEventParam->callerType ) )
    {
      switch(m_pokeSendType)
      {
      case POKE_SEND_TYPE_TARGET_TO_PARTY:
        { 
          PokeTool::PokeGetUtil::Put_NotSetFlag(
              m_param.appHeap->GetDeviceHeap()->GetLowerHandle(),
              m_param.param->zukanRegisterEventParam->pp,
              PokeTool::PokeGetUtil::PutPlace::PARTY,
              PokeTool::PokeGetUtil::PutSpace::FREE,
              0,  // 使わないのでてきとー
              0,  // 使わないのでてきとー
              &(m_param.param->zukanRegisterEventParam->returnParam)
          );
          break;
        }
      case POKE_SEND_TYPE_TARGET_TO_PARTY_AND_OTHER_TO_BOX:
        {
          // (1) 手持ちにいたものをボックスへ送る
          u32 tray = Savedata::BoxPokemon::TRAY_MAX;  // 送った先のボックスのトレイ番号を受け取る
          this->putPartyPokeToBox(m_param.appHeap->GetDeviceHeap()->GetLowerHandle(), m_param.param->pokeListSelectPos, &tray);
         
          // (2) 手持ちの今ボックスへ送った場所に配置する  // party->RemoveMemberで消す必要はない
          PokeTool::PokeGetUtil::Put_NotSetFlag(
              m_param.appHeap->GetDeviceHeap()->GetLowerHandle(),
              m_param.param->zukanRegisterEventParam->pp,
              PokeTool::PokeGetUtil::PutPlace::PARTY,
              PokeTool::PokeGetUtil::PutSpace::FIX,
              m_param.param->pokeListSelectPos,
              0,  // 使わないのでてきとー
              &(m_param.param->zukanRegisterEventParam->returnParam)
          );
          break;
        }
      case POKE_SEND_TYPE_TARGET_TO_BOX:
        {
          PokeTool::PokeGetUtil::Put_NotSetFlag(
              m_param.appHeap->GetDeviceHeap()->GetLowerHandle(),
              m_param.param->zukanRegisterEventParam->pp,
              PokeTool::PokeGetUtil::PutPlace::BOX,
              PokeTool::PokeGetUtil::PutSpace::FREE,
              0,  // 使わないのでてきとー
              0,  // 使わないのでてきとー
              &(m_param.param->zukanRegisterEventParam->returnParam)
          );
          break;
        }
      case POKE_SEND_TYPE_TARGET_STAY:
        {
          break;  // エラーになっているので、何もしない。
        }
      }
    }

    // フラグ処理
    if( PokeTool::PokeGetUtil::DoesSetFlag(
            m_param.param->zukanRegisterEventParam->pp,
            m_param.param->zukanRegisterEventParam->callerType ) )
    {
      //PokeTool::PokeGetUtil::SetFlag( m_param.param->zukanRegisterEventParam->pp );  // SetFlagはZukanContentsをつくるときに済ませてあるのでコメントアウト。
    }
  }
}


pml::pokepara::PokemonParam* ZukanFramePokePutCell::getPokeForBox(void) const
{
  pml::pokepara::PokemonParam* pp = NULL;
  switch(m_pokeSendType)
  {
  case POKE_SEND_TYPE_TARGET_TO_PARTY:
    {
      pp = NULL;  // 新たに加わるポケモンを手持ちに入れるので、NULLを返す。
      break;
    }
  case POKE_SEND_TYPE_TARGET_TO_PARTY_AND_OTHER_TO_BOX:
    {
      pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();  // 今手持ちにいるポケモンをボックスに送る予定なので、今手持ちにいるポケモンを返す。
      pml::pokepara::PokemonParam* ppInParty = party->GetMemberPointer(m_param.param->pokeListSelectPos);
      pp = ppInParty;
      break;
    }
  case POKE_SEND_TYPE_TARGET_TO_BOX:
    {
      pp = m_param.param->zukanRegisterEventParam->pp;  // 新たに加わるポケモンをボックスに送るので、新たに加わるポケモンを返す。
      break;
    }
  case POKE_SEND_TYPE_TARGET_STAY:
    {
      pp = NULL;  // エラーになっているので、NULLを返す。
      break;
    }
  }
  return pp;
}


void ZukanFramePokePutCell::changePokeFormByNoItem(pml::pokepara::PokemonParam* pp)  // @fix NMCat[1284] もちものを外してボックスに転送してもフォルムが戻らない
{
  pml::FormNo nextFormNo = pp->GetNextFormNoFromHoldItem( ITEM_DUMMY_DATA );
  if( pp->GetFormNo() != nextFormNo )
  {
    pp->ChangeFormNo( nextFormNo );
    // @fix GFNMCat[4178] 【図鑑登録】フォルムチェンジ時に図鑑登録されない
    Savedata::ZukanData* svZukanData = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetZukanData();
    svZukanData->SetPokeSee(*pp);  // 別フォルムを見たので、見たフラグを立てる。(捕まえたフラグを立てにいっても同じ結果が得られる)
  }
}


bool ZukanFramePokePutCell::isPokeChangeOK(void) const
{
  // ポケモンリストから戻ってきて、「入手したポケモン」と「手持ちのポケモン」を入れ替えることを決定しているとき、この関数が呼ばれる。
  // この前提を満たしていないときにはこの関数を呼ばないように。

  // 入れ替えてもバトルできるか否か判定する。入れ替えるとバトルできなくなってしまうときfalseを返す。

  pml::PokeParty* party = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetPlayerParty();
  
  u8 passIndexBit = 1 << m_param.param->pokeListSelectPos;
  u32 count = party->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_BATTLE_ENABLE, passIndexBit );

  if(count > 0)
  {
    // パーティにバトル可能なポケモンが残っている
    return true;
  }
  else
  {
    if(    ( m_param.param->zukanRegisterEventParam->pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
        && ( m_param.param->zukanRegisterEventParam->pp->IsHpZero() == false ) )
    {
      // 入手したポケモンがバトル可能なポケモン
      return true;
    }
    else
    {
      return false;
    }
  }
}


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)

