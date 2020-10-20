#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTERINTRO_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTERINTRO_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanFrameRegisterIntro.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.23
 @brief   図鑑の登録の際の導入画面のフレーム
 */
//==============================================================================


// 図鑑のインクルード
#include "ZukanFrameSceneTemplate.h"
#include "ZukanViewPokeDialogue.h"
#include "ZukanViewDialogue.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class StrBuf;
GFL_NAMESPACE_END(Str)
GFL_NAMESPACE_END(gfl2)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanFrameRegisterIntroCell
 @brief     図鑑の登録の際の導入画面のフレームのセル
 */
//==============================================================================
class ZukanFrameRegisterIntroCell : public ZukanFrameSceneTemplateCell<ZukanViewPokeDialogue>
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanFrameRegisterIntroCell);

public:
  ZukanFrameRegisterIntroCell(void);
  virtual ~ZukanFrameRegisterIntroCell();

  virtual applib::frame::Result InitFunc(void);
  virtual applib::frame::Result EndFunc(void);
  virtual applib::frame::Result UpdateFunc(void);

  virtual void Req(s32 reqId);

private:
  // Req関数に渡すリクエストID
  enum
  {
    REQ_ID_DIALOGUE_END    = REQ_ID_MAX
  };

  enum
  {
    STR_BUF_CHAR_NUM_MAX = 512,  // 全言語で十分大きい値
  };

  enum RegisterFadeState
  {
    REGISTER_FADE_STATE_OFF,
    REGISTER_FADE_STATE_OFF_REQ_IN,
    REGISTER_FADE_STATE_ON_IN,
    REGISTER_FADE_STATE_ON_WORK,
  };

private:
  ZukanViewDialogue::ResultListener* m_dialogueResultListener;
  ZukanViewDialogue::Content*        m_dialogueContent;
  gfl2::str::StrBuf*                 m_strBuf;
  RegisterFadeState                  m_registerFadeState;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANFRAMEREGISTERINTRO_H_INCLUDED

