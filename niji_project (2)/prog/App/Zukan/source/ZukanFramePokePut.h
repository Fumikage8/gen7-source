#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEPOKEPUT_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEPOKEPUT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFramePokePut.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の送り先選択画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneBase.h"
#include "ZukanViewPokeDialogue.h"
#include "ZukanViewDialogue.h"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFramePokePutCell
 @brief     図鑑の送り先選択画面のフレームのセル
 */
//==============================================================================
class ZukanFramePokePutCell : public ZukanFrameSceneBaseCell
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFramePokePutCell);

public:
  ZukanFramePokePutCell(void);
  virtual ~ZukanFramePokePutCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);

  virtual void Req(s32 reqId);

public:
  // Req関数に渡すリクエストID
  enum
  {
    REQ_ID_MES,
    REQ_ID_QA_ANSWER0,
    REQ_ID_QA_ANSWER1,
    REQ_ID_QA_ANSWER2,
    REQ_ID_QA_ANSWER3,

    REQ_ID_END,
  };

private:
  enum State
  {
    STATE_QA_POKE_SEND,          // フシギバナをどちらにおくりますか？
    STATE_MES_POKE_SEND_SELECT,  // てもちからこうかんするポケモンをえらんでください
    STATE_MES_POKE_CHANGE_NG,    // そのポケモンをこうかんするとぼうけんできなくなります！
    STATE_QA_ITEM_SEND,          // フシギバナがクラボのみをもっていますがどうしますか？
    STATE_MES_ITEM_SEND_RESULT,  // クラボのみをバッグにいれた！ / バッグがいっぱいです！ / さずけているゼンリョクピースをはずしました
    STATE_MES_POKE_SEND_RESULT,  // フシギバナをてもちにくわえました！ / フシギバナをてもちにくわえてリザードンをボックスへおくりました！ / フシギバナをボックスにてんそうしました！
  };
  enum PokeSendType
  {
    POKE_SEND_TYPE_TARGET_TO_PARTY,                    // フシギバナをてもちにくわえました！
    POKE_SEND_TYPE_TARGET_TO_PARTY_AND_OTHER_TO_BOX,   // フシギバナをてもちにくわえてリザードンをボックスへおくりました！
    POKE_SEND_TYPE_TARGET_TO_BOX,                      // フシギバナをボックスにてんそうしました！
    POKE_SEND_TYPE_TARGET_STAY,                        // 配置済み。配置済みで到達することはないはずなので、この状態になるのはエラー。
  };
  enum ItemSendType
  {
    ITEM_SEND_TYPE_TARGET_TO_BAG,           // クラボのみをバッグにいれた！ / さずけているゼンリョクピースをはずしました
    ITEM_SEND_TYPE_BAG_IS_FULL,             // バッグがいっぱいです！
  };

private:
  enum
  {
    STR_BUF_QUESTION = 0,  // STATE_MESのときはSTR_BUF_QUESTIONを使わせてもらう
    STR_BUF_ANSWER0,
    STR_BUF_ANSWER1,
    STR_BUF_ANSWER2,
    STR_BUF_ANSWER3,
    STR_BUF_NUM,

    STR_BUF_CHAR_NUM_MAX = 512,  // 全言語で十分大きい値
  };

private:
  //! @brief  手持ちにいるポケモンをBOXへ送る。
  //! @param[in]  heap        ヒープ。通常のヒープメモリでもデバイスメモリでもどちらでもよい。本関数内で一時的に使用するだけ。
  //! @param[in]  posInParty  手持ちの場所。
  //! @param[out] a_tray      送った先のボックスのトレイ番号を入れて返す。送れなかったときはSavedata::BoxPokemon::TRAY_MAXを入れて返す。
  static void putPartyPokeToBox(gfl2::heap::HeapBase* heap, u32 posInParty, u32* a_tray);

  void setupForStateQAPokeSend(s32 cursorPos=0);
  void setupForStateMesPokeSendSelect(void);
  void setupForStateMesPokeChangeNG(void);
  void setupForStateQAItemSend(void);
  void setupForStateMesItemSendResult(void);
  void setupForStateMesPokeSendResult(void);

  void reqForStateQAPokeSend(s32 reqId);
  void reqForStateMesPokeSendSelect(s32 reqId);
  void reqForStateMesPokeChangeNG(s32 reqId);
  void reqForStateQAItemSend(s32 reqId);
  void reqForStateMesItemSendResult(s32 reqId);
  void reqForStateMesPokeSendResult(s32 reqId);

  pml::pokepara::PokemonParam* getPokeForBox(void) const;

  // アイテムを外したときのフォルムチェンジ処理
  void changePokeFormByNoItem(pml::pokepara::PokemonParam* pp);  // @fix NMCat[1284] もちものを外してボックスに転送してもフォルムが戻らない

  bool isPokeChangeOK(void) const;

private:
  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  gfl2::str::StrBuf*          m_strBuf[STR_BUF_NUM];
  
  // ビュー
  ZukanViewPokeDialogue*             m_view;
  ZukanViewDialogue::ResultListener* m_dialogueResultListener;
  ZukanViewDialogue::Content         m_dialogueContent;
  bool                               m_endReq;

  // 状態
  State         m_state;
  PokeSendType  m_pokeSendType;
  ItemSendType  m_itemSendType;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEPOKEPUT_H_INCLUDED

