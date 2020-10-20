#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWPOKEDIALOGUE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWPOKEDIALOGUE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewPokeDialogue.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.10.23
 @brief   図鑑のポケモンモデルを表示した対話のビュー
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
#include "ZukanType.h"
#include "ZukanSceneParam.h"
#include "ZukanViewDialogue.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(str)
class StrBuf;
GFL_NAMESPACE_END(Str)
GFL_NAMESPACE_END(gfl2)

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
  class ZukanDataManager;
  class ZukanFrameSceneBaseCell;
  class ZukanViewPokeDialogueListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanPokeDialogueResultListener
 @brief     対話結果のリスナー
 */
//==============================================================================
class ZukanPokeDialogueResultListener : public ZukanViewDialogue::ResultListener
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanPokeDialogueResultListener);

public:
  ZukanPokeDialogueResultListener(ZukanFrameSceneBaseCell* frameCell);
  virtual ~ZukanPokeDialogueResultListener();

public:
  virtual void Req(s32 reqId);

private:
  ZukanFrameSceneBaseCell*  m_frameCell;
};




//==============================================================================
/**
 @class     ZukanViewPokeDialogue
 @brief     図鑑のポケモンモデルを表示した対話のビュー
 */
//==============================================================================
class ZukanViewPokeDialogue : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewPokeDialogue);

public:
  //! @brief
  class PokeDialogueParam
  {
  public:
    s32                           frameCellReqIdEnd;
    bool                          fadeInFlag;
    bool                          fadeOutFlag;
  public:
    PokeDialogueParam(void)
      : frameCellReqIdEnd(0),
        fadeInFlag(false),
        fadeOutFlag(false)
    {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewPokeDialogue(const ZukanSceneParam& param);
  
  virtual ~ZukanViewPokeDialogue();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  void SetPokeDialogueParam(const PokeDialogueParam& pokeDialogueParam);  // 途中書き換えOK。
  void ReqStartDialogue(ZukanViewDialogue::Content* dialogueContent);  // リクエスト後開始できるようになったら開始する。直ちに開始できるときは直ちに開始する。

  //! @name  ビューのリスナー、フレームからアクセスするメンバ
  //@{
public:
  //! @brief  ビューのリスナー、フレームから呼び出されるリクエスト
  enum ReqId
  {
    REQ_ID_IN,    // フレームから呼び出すリクエスト
    REQ_ID_OUT    // フレームから呼び出すリクエスト
  };

  //! @brief  ビューのリスナー、フレームから呼び出されるリクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}

private:
  // 表示状態
  enum DispState
  {
    // 起動画面
    DISP_STATE_OFF,                // 表示していない
    DISP_STATE_ON_FADE_IN,         // 表示している(フェードイン中)
    DISP_STATE_ON_IN,              // 表示している(イン)
    DISP_STATE_ON_WORK,            // 表示している(稼働中)
    DISP_STATE_ON_FADE_OUT,        // 表示している(フェードアウト中)
    DISP_STATE_ON_OUT,             // 表示している(アウト)
    DISP_STATE_WAIT_SAFETY,        // 表示したまま安全に破棄できるようになるを待っている(一部表示を消したりしている)
    DISP_STATE_WAIT_END,           // 表示したまま終了するのを待っている(呼び出し元が描画しないようにしたりしている)
  };

  enum
  {
    DIALOGUE_REQ_ID_MES  = 0
  };

private:
  void startInSeq(void);
  void startOutSeq(void);

  void changeDispState(DispState dispState);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam              m_param; 
  PokeDialogueParam            m_pokeDialogueParam;
  ZukanViewDialogue::Content*  m_dialogueContent;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewPokeDialogueListener* m_viewListener;  // ビューのリスナー

  ZukanViewDialogue*             m_viewDialogue;

  bool  m_startDialogueReq;

  DispState m_dispState;
  u32       m_dispStateCount;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWPOKEDIALOGUE_H_INCLUDED

