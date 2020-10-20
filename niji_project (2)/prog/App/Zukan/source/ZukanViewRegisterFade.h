#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERFADE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERFADE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewRegisterFade.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.02.13
 @brief   図鑑の登録画面に出すフェード層のビュー
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <AppLib/include/Ui/UIInputListener.h>
#include <AppLib/include/Ui/UIView.h>

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
  class ZukanDataManager;
  class ZukanFrameRegisterCell;
  class ZukanViewRegisterFadeListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegisterFade
 @brief     図鑑の登録画面に出すフェード層のビュー
 */
//==============================================================================
class ZukanViewRegisterFade : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterFade);

  friend class ZukanViewRegisterFadeListener;

public:
  class Param
  {
  public:
    ZukanProcParam*                 param;                 // paramのポインタを保持するだけ。
                                                           // 呼び出し元はparamを破棄しないように。
    app::util::Heap*                appHeap;
    GameSys::GameManager*           gameManager;
    ZukanDataManager*               dataManager;
    applib::frame::Manager*         frameManager;
    app::util::AppRenderingManager* appRenderingManager;
    gfl2::lyt::LytSys*              lytSys;
    app::ui::UIView*                parentView;
  public:
    Param(void)
      : param(NULL),
        appHeap(NULL),
        gameManager(NULL),
        dataManager(NULL),
        frameManager(NULL),
        appRenderingManager(NULL),
        lytSys(NULL),
        parentView(NULL)
    {}
  };

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewRegisterFade(const Param& param);
  
  virtual ~ZukanViewRegisterFade();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

public:
  bool IsInFinished(void) const { return (m_dispState == DISP_STATE_ON_WORK); }
  bool IsOutFinished(void) const { return (m_dispState == DISP_STATE_OFF); }
  bool IsDispOn(void) const { return (m_dispState != DISP_STATE_OFF); }
      // リクエストだけ出ていて表示されていない、ということはないので、これだけ確認すれば表示中か否か分かる。
  bool IsStateStill(void) const { return (m_dispState == DISP_STATE_OFF || m_dispState == DISP_STATE_ON_WORK); }
      // リクエストだけ出ていて表示されていない、ということはないので、これだけ確認すればフェードアニメ終了中か否か分かる。

  enum
  {
    REQ_ID_IN,   // フェード画面を非表示→表示に遷移させる
    REQ_ID_OUT,  // フェード画面を非表示→表示に遷移させる
  };
  void Req(s32 reqId);

private:
  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,      // フェード画面が非表示状態
    DISP_STATE_ON_IN,    // フェード画面が非表示→表示に遷移中
    DISP_STATE_ON_WORK,  // フェード画面が表示状態
    DISP_STATE_ON_OUT,   // フェード画面が表示→非表示に遷移中
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_UPPER,
    LYT_WK_ID_NUM,
  };

  // アニメ
  enum AnimId
  {
    ANIM_ID_IN,   // フェード画面が表示→非表示に遷移
    ANIM_ID_OUT,  // フェード画面が非表示→表示に遷移
    ANIM_ID_NUM
  };

private:
  void changeDispState(DispState dispState);
  void setupAnimId(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  Param  m_param;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewRegisterFadeListener* m_viewListener;

  u32 m_lytWkId;
  u32 m_animId[LYT_WK_ID_NUM][ANIM_ID_NUM];

  DispState m_dispState;
  u32       m_dispStateCount;
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERFADE_H_INCLUDED

