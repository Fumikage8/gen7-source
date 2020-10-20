#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTER_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTER_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanViewRegister.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.08.18
 @brief   図鑑の登録画面のビュー
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
#include "ZukanViewInfoFunc.h"
#include "ZukanDefine.h"


// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(debug)
class DebugWinItem;
GFL_NAMESPACE_END(debug)
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
  class ZukanFrameRegisterCell;
  class ZukanViewRegisterListener;
GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanViewRegister
 @brief     図鑑の登録画面のビュー
 */
//==============================================================================
class ZukanViewRegister : public app::ui::UIView
{
  GFL_FORBID_COPY_AND_ASSIGN(ZukanViewRegister);

  friend class ZukanViewRegisterListener;

public:
  //! @brief  コンストラクタ
  //! @param[in]  param  呼び出し元はparamを破棄していい。
  //!                    paramの中にポインタがあるときはそれが指すものは破棄しないようにして下さい。
  ZukanViewRegister(const ZukanSceneParam& param);
  
  virtual ~ZukanViewRegister();

  virtual void Update(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);


  //! @name  friendクラスZukanViewRegisterListenerからアクセスするメンバ
  //@{
private:
  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  enum ReqId
  {
    REQ_ID_BUTTON_DECIDE,  // reqParamはBUTTON_ID_
  };

  //! @brief  ボタン
  enum
  {
    BUTTON_ID_OK,
    BUTTON_ID_VOICE,       
    BUTTON_ID_MOTION,       
    BUTTON_ID_PAUSE,       

    BUTTON_ID_NUM
  };

  //! @brief  ビューのリスナーから呼び出される終了リクエスト
  void Req(ReqId reqId, s32 reqParam=0);
  //@}


private:
  // 表示状態
  enum DispState
  {
    DISP_STATE_OFF,          // 表示していない
    DISP_STATE_ON_FADE_IN,   // 表示している(フェードイン中)
    DISP_STATE_ON_IN,        // 表示している(イン中)
    DISP_STATE_ON_WORK,      // 表示している(稼働中)
    DISP_STATE_ON_OUT,       // 表示している(アウト中)
    DISP_STATE_WAIT_SAFETY,  // 表示したまま安全に破棄できるようになるを待っている(一部表示を消したりしている)
    DISP_STATE_WAIT_END,     // 表示したまま終了するのを待っている(呼び出し元が描画しないようにしたりしている)
  };

  // レイアウトワーク
  enum LytWkId
  {
    LYT_WK_ID_LOWER,
    LYT_WK_ID_NUM,
  };

private:
  void createButtonManager(void);
  void destroyButtonManager(void);

  void startFadeInSeq(void);
  void startInSeq(void);
  void startOutSeq(void);

  void changeDispState(DispState dispState);

  void createPage(void);

  void playVoice(void);
  void changeMotion(void);
  void switchPause(void);

private:
  static const ZukanType::ButtonConstInfo BUTTON_CONST_INFO[BUTTON_ID_NUM];

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  ZukanSceneParam  m_param; 

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  ZukanViewRegisterListener* m_viewListener;
  
  ZukanType::ButtonPaneInfo  m_buttonPaneInfo[BUTTON_ID_NUM];

  u32 m_waitAnimIndex[LYT_WK_ID_NUM];
  
  DispState m_dispState;
  u32       m_dispStateCount;

  ZukanViewInfoFunc  m_infoFunc;

#if  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_REGISTER_POKE_CHANGE_ENABLE
private:
  static ZukanViewRegister*    s_debugInst;
  static PokeTool::SimpleParam s_debugSp;

  static void debugUpdatePokeSetting(void* userWork, gfl2::debug::DebugWinItem* item);
  static wchar_t* debugDrawPokeSetting(void* userWork, gfl2::debug::DebugWinItem* item);
  
  void debugCreateDebugWin(void);
  void debugDestroyDebugWin(void);
  void debugSetNextPoke(void);
  void debugSetPoke(MonsNo monsNo);

  void debugInitFormNo(void);
  void debugInitSex(void);

  gfl2::debug::DebugWinGroup* m_debugPokeChangeGroup;
#endif
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANVIEWREGISTER_H_INCLUDED

