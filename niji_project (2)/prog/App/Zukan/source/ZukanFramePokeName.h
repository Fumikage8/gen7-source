#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEPOKENAME_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEPOKENAME_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFramePokeName.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の名前入力画面のフレーム
 */
//==============================================================================


// nijiのインクルード
#include <App/StrInput/include/StrInputAppParam.h>

// 図鑑のインクルード
#include "ZukanFrameSceneBase.h"
#include "ZukanViewPokeDialogue.h"
#include "ZukanViewDialogue.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class StrBuf;
GFL_NAMESPACE_END(Str)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(StrInput)
class StrInputFrame;
GFL_NAMESPACE_END(StrInput)
GFL_NAMESPACE_END(App)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFramePokeNameCell
 @brief     図鑑の名前入力画面のフレームのセル
 */
//==============================================================================
class ZukanFramePokeNameCell : public ZukanFrameSceneBaseCell
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFramePokeNameCell);

public:
  ZukanFramePokeNameCell(void);
  virtual ~ZukanFramePokeNameCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result UpdateFunc(void);
  virtual void                  DrawFunc(gfl2::gfx::CtrDisplayNo no);
  virtual applib::frame::Result EndFunc(void);

  virtual void Req(s32 reqId);

public:
  // Req関数に渡すリクエストID
  enum
  {
    REQ_ID_QA_ANSWER0,  
    REQ_ID_QA_ANSWER1,  

    REQ_ID_STR_INPUT,
    REQ_ID_END,
  };

private:
  void reqEnd(void);

private:
  enum
  {
    STR_BUF_POKE_NICK_NAME = 0,
    STR_BUF_QUESTION,
    STR_BUF_ANSWER0,
    STR_BUF_ANSWER1,
    STR_BUF_NUM,

    STR_BUF_CHAR_NUM_MAX = 512,  // 全言語で十分大きい値
  };

private:
  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  gfl2::str::StrBuf*            m_strBuf[STR_BUF_NUM];
  
  // フレーム
  App::StrInput::APP_PARAM      m_strInputParam;
  App::StrInput::StrInputFrame* m_frameStrInput;

  // ビュー
  ZukanViewPokeDialogue*             m_view;
  ZukanViewDialogue::ResultListener* m_dialogueResultListener;
  ZukanViewDialogue::Content         m_dialogueContent;
  bool                               m_strInputReq;
  bool                               m_endReq;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEPOKENAME_H_INCLUDED

