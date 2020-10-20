#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWDIALOGUE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWDIALOGUE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewDialogue.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の対話のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>

// 図鑑のインクルード
#include "ZukanSceneParam.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
  class StrBuf;
GFL_NAMESPACE_END(str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanViewDialogueListener;
  class ZukanDialogueTwoButtonsViewEventHandler;
  class ZukanDialogueMultiButtonsViewEventHandler;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewDialogue
 @brief     図鑑の対話のビュー
 */
//==============================================================================
class ZukanViewDialogue : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewDialogue);

  friend class ZukanViewDialogueListener;
  friend class ZukanDialogueTwoButtonsViewEventHandler;
  friend class ZukanDialogueMultiButtonsViewEventHandler;

public:
  //! @brief  対話パターン
  enum Pattern
  {
    PATTERN_QA,   // 質問→二択
    PATTERN_MES,  // メッセージ
    PATTERN_QA4,  // 質問→四択
  };
  //! @brief  対話結果のリスナー
  class ResultListener
  {
  public:
    ResultListener(void) {}
    virtual ~ResultListener() {}
  public:
    virtual void Req(s32 /*reqId*/) {}
  };
  //! @brief  対話内容
  class Content
  {
  public:
    Pattern pattern;
    ResultListener* resultListener;

                                                   // PATTERN_QA                                                    | PATTERN_MES
    gfl2::str::StrBuf* strBuf;                     // 質問                                                          | メッセージ 
    gfl2::str::StrBuf* strBuf0;                    // 項目0                                                         | 未使用
    gfl2::str::StrBuf* strBuf1;                    // 項目1                                                         | 未使用
    gfl2::str::StrBuf* strBuf2;                    // 項目2(PATTERN_QA4で使用)                                      | 未使用
    gfl2::str::StrBuf* strBuf3;                    // 項目3(PATTERN_QA4で使用)                                      | 未使用
    bool               button1UseBButton;          // 項目1のボタンでBボタンを使うか                                | 未使用
    bool               button3UseBButton;          // 項目3のボタンでBボタンを使うか(PATTERN_QA4で使用)             | 未使用
    bool               button1UseBButtonDecideSE;  // 項目1のボタンでBボタンを使うか(PATTERN_QA4で使用)(SEは決定音) | 未使用
    s32 reqId0;                                    // 項目0を選択                                                   | メッセージ終了
    s32 reqId1;                                    // 項目1を選択                                                   | 未使用
    s32 reqId2;                                    // 項目2を選択(PATTERN_QA4で使用)                                | 未使用
    s32 reqId3;                                    // 項目3を選択(PATTERN_QA4で使用)                                | 未使用
    s32 cursorPos;                                 // どの項目にカーソルを合わせておくか                            | 未使用

  public:
    Content(void)
      : pattern(PATTERN_MES),
        resultListener(NULL),
        strBuf(NULL),
        strBuf0(NULL),
        strBuf1(NULL),
        strBuf2(NULL),
        strBuf3(NULL),
        button1UseBButton(false),
        button3UseBButton(false),
        button1UseBButtonDecideSE(false),
        reqId0(0),
        reqId1(0),
        reqId2(0),
        reqId3(0),
        cursorPos(0)
    {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param            呼び出し元はparamを破棄していい。
  //!                              paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewDialogue(const ZukanSceneParam& param);
  
  virtual ~ZukanViewDialogue();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

public:
  void StartDialogue(Content* content);


  //! @name  friendクラスZukanViewDialogueListener, ZukanDialogueTwoButtonsViewEventHandlerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  enum ReqId
  {
    REQ_ID_NONE                     ,  // 初期化にしか使わない
    REQ_ID_TWO_BUTTONS_VIEW_DECIDE  ,  // reqParamはTWO_BUTTONS_VIEW_ID_
    REQ_ID_TALK_WINDOW_DECIDE       ,  // reqParamは使わない
    REQ_ID_MULTI_BUTTONS_VIEW_DECIDE,  // reqParamはMULTI_BUTTONS_VIEW_ID_
  };

  //! @brief  ボタン
  enum
  {
    TWO_BUTTONS_VIEW_ID_YES          =   0,       
    TWO_BUTTONS_VIEW_ID_NO                ,       
  };
  enum
  {
    // この数値をそのまま配列の引数に使用しているので変更しないように！
    MULTI_BUTTONS_VIEW_ID_BUTTON0  = 0,
    MULTI_BUTTONS_VIEW_ID_BUTTON1     ,
    MULTI_BUTTONS_VIEW_ID_BUTTON2     ,
    MULTI_BUTTONS_VIEW_ID_BUTTON3     ,
  };
 
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}

private:
  enum State
  {
    STATE_NONE,
    STATE_CREATING,
    STATE_FLOATING,
    STATE_WAITING,
    STATE_NOT_DRAWING,
    STATE_DESTROYING,
  };

private:
  void updateForPatternQA(void);
  void updateForPatternMes(void);
  void updateForPatternQA4(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param;
  Content*         m_content;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  State                                    m_state;
  ZukanViewDialogueListener*       m_viewListener;
  ZukanDialogueTwoButtonsViewEventHandler*   m_twoButtonsViewEventHandler;
  ZukanDialogueMultiButtonsViewEventHandler* m_multiButtonsViewEventHandler;

  bool   m_closeReq;
  ReqId  m_reqIdAtClose;
  s32    m_reqParamAtClose;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWDIALOGUE_H_INCLUDED

