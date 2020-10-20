#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERALOLATOP_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERALOLATOP_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewRegisterAlolaTop.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録の際のアローラ図鑑トップ画面のビュー
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
#include "ZukanDefine.h"
#include "ZukanSceneParam.h"
#include "ZukanViewRegisterComment.h"


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
  class ZukanDataManager;
  class ZukanPicScrollDataAccessor;
  class ZukanPicScrollDemo;
  class ZukanFrameRegisterAlolaTopCell;
  class ZukanViewRegisterAlolaTopListener;
#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  class ZukanPrismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegisterAlolaTop
 @brief     図鑑の登録の際のアローラ図鑑トップ画面のビュー
 */
//==============================================================================
class ZukanViewRegisterAlolaTop : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegisterAlolaTop);

  friend class ZukanViewRegisterAlolaTopListener;
  friend class ZukanViewRegisterAlolaTopCommentResultListener;
  
public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewRegisterAlolaTop(const ZukanSceneParam& param);
  
  virtual ~ZukanViewRegisterAlolaTop();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  //! @name  friendクラスZukanViewRegisterAlolaTopListener, ZukanViewRegisterAlolaTopCommentResultListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  enum ReqId
  {
    REQ_ID_TOUCH_DECIDE      ,  // reqParamは使わない
    REQ_ID_KEY_DECIDE        ,  // reqParamは使わない
    REQ_ID_OUT               ,  // reqParamは使わない
  };

  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}

private:
  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,          // 表示していない
    DISP_STATE_ON_IN,        // 表示している(イン中)
    DISP_STATE_ON_WORK,      // 表示している(稼働中)
    DISP_STATE_ON_COMMENT_WORK,   // 表示している(コメント層が稼働中)
    DISP_STATE_ON_OUT,       // 表示している(アウト中)
    DISP_STATE_WAIT_SAFETY,  // 表示したまま安全に破棄できるようになるを待っている(一部表示を消したりしている)
    DISP_STATE_WAIT_END,     // 表示したまま終了するのを待っている(呼び出し元が描画しないようにしたりしている)
  };

private:
  void startInSeq(void);
  void startOutSeq(void);

  void changeDispState(DispState dispState);

  void createComment(void);
  void destroyComment(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewRegisterAlolaTopListener* m_viewListener;
  
  u32 m_waitAnimCount;
  
  DispState m_dispState;
  u32       m_dispStateCount;

  ZukanPicScrollDataAccessor* m_picScrollDataAccessor;
  ZukanPicScrollDemo*         m_picScrollDemo;

  // コメント層のビュー
  ZukanViewRegisterComment*                 m_viewComment;
  ZukanViewRegisterComment::ResultListener* m_commentResultListener;

  // 図鑑登録用一時記憶
  bool m_registerIsBingo;
  u32  m_registerLegendLevel;

#if NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
  ZukanPrismSheet*   m_prismSheet;
#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTERALOLATOP_H_INCLUDED

