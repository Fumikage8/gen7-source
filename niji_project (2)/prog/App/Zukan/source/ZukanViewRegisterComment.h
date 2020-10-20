#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERCOMMENT_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERCOMMENT_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewRegisterComment.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録画面に出すコメント層のビュー
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
GFL_NAMESPACE_BEGIN(lyt)
  class LytSys;
GFL_NAMESPACE_END(lyt)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(GameSys)
  class GameManager;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
  class Heap;
  class AppRenderingManager;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
GFL_NAMESPACE_BEGIN(frame)
  class Manager;
GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)
  class ZukanProcParam;
  class ZukanRotomVoice;
  class ZukanDataManager;
  class ZukanFrameRegisterCell;
  class ZukanViewRegisterCommentListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegisterComment
 @brief     図鑑の登録画面に出すコメント層のビュー
 */
//==============================================================================
class ZukanViewRegisterComment : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterComment);

  friend class ZukanViewRegisterCommentListener;

public:
  class ResultListener
  {
  public:
    ResultListener(void) {}
    virtual ~ResultListener() {}
  public:
    virtual void Req(s32 /*reqId*/) {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewRegisterComment(const ZukanSceneParam& param, bool registerIsBingo, u32 registerLegendLevel);
  
  virtual ~ZukanViewRegisterComment();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

public:
  void SetResultListener(ResultListener* resultListener) { m_resultListener = resultListener; }

  //! @name  friendクラスZukanViewRegisterCommentListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void ReqEnd(void);
  //@}

private:
  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,
    DISP_STATE_ON_IN,
    DISP_STATE_ON_WORK,
    DISP_STATE_ON_WAIT_END,
  };

  enum
  {
    STR_BUF_CHAR_NUM_MAX = 1024,  // 全言語で十分大きい値
    MSG_NUM_MAX  = 9
  };

private:
  void createPage(void);
  void startInSeq(void);

  void changeDispState(DispState dispState);

  void setupMsgIdList(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param;
  ResultListener*  m_resultListener;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewRegisterCommentListener* m_viewListener;

  gfl2::lyt::LytTextBox*  m_mesWinTextPane[2];
  gfl2::lyt::LytPane*     m_mesCursorPane;
  bool                    m_mesCursorVisible;
  gfl2::str::StrBuf*      m_strBuf;

  u32  m_msgNum;
  u32  m_msgNo;
  u32  m_msgIdList[MSG_NUM_MAX];

  gfl2::lyt::LytPane*     m_achievementTopPane[ZukanType::INFO_TYPE_NUM];
  gfl2::lyt::LytTextBox*  m_achievementZukanPane[ZukanType::INFO_TYPE_NUM];
  gfl2::lyt::LytTextBox*  m_achievementRatePane[ZukanType::INFO_TYPE_NUM];

  ZukanRotomVoice*  m_rotomVoice;

  DispState m_dispState;
  u32       m_dispStateCount;

  // 図鑑登録用一時記憶
  bool m_registerIsBingo;
  u32  m_registerLegendLevel;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERCOMMENT_H_INCLUDED

