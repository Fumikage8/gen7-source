//==============================================================================
/**
 * @file        GameOverImpl.h
 * @brief       要約説明
  
    このファイルの詳細説明
 * 
 * @author      N.Takeda
 * @date        2015/07/23(木) 21:18:19
 */
//==============================================================================

#if !defined(__GAMEOVERIMPL_H__)  // 重複定義防止
#define __GAMEOVERIMPL_H__  // 重複定義防止マクロ
#pragma once


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameProc.h>
#include <GameSys/include/GameManager.h>


// 前方宣言
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
GFL_NAMESPACE_BEGIN(GameOver)
  class GameOverProcParam;
  class GameOverFrameListener;
  class GameOverFrameCell;
GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)
  class GameOverDataManager;
GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(GameOver)


//==============================================================================
/**
 @class     GameOverImpl
 @brief     図鑑の実装
 */
//==============================================================================
class GameOverImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(GameOverImpl);

public:
  //! @brief  コンストラクタ
  //! @param[in]  param        paramのポインタを保持するだけ。
  //!                          呼び出し元はparamを破棄しないように。
  //! @param[in]  appHeap      使ってもいいヒープ。
  //! @param[in]  gameManager  ゲームマネージャ。
  GameOverImpl(GameOverProcParam* param, app::util::Heap* appHeap, GameSys::GameManager* gameManager);

  virtual ~GameOverImpl();

  gfl2::proc::Result InitFunc(void);
  gfl2::proc::Result UpdateFunc(void);
  void PreDrawFunc(void);
  void DrawFunc(gfl2::gfx::CtrDisplayNo displayNo);
  gfl2::proc::Result EndFunc(void);

private:
  enum Step
  {
    STEP_NONE,
    STEP_CREATE,
    STEP_WORKING,
    STEP_DESTROY,
  };

private:
  void changeStep(Step step);
  void updateFunc_None(void);
  void updateFunc_Create(void);
  void updateFunc_Working(void);
  void updateFunc_Destroy(void);
  void updateForDraw(void);

private:
  ////////////////////////////////////////
  // 外部から受け取ったもの
  ////////////////////////////////////////
  GameOverProcParam*       m_param;
  app::util::Heap*      m_appHeap;
  GameSys::GameManager* m_gameManager;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // ステップ
  Step m_step;
  u32  m_stepCount;

  // データマネージャ
  GameOverDataManager*        m_dataManager;

  // フレーム
  gfl2::heap::HeapBase*    m_frameManagerDevMem;
  applib::frame::Manager*  m_frameManager;
  GameOverFrameListener*      m_frameListener;
  GameOverFrameCell*          m_frameCell;
  bool                     m_frameCreated;     // trueのときフレームが生成された
  bool                     m_frameReqDestroy;  // trueのときフレームが破棄をリクエストしている
  
  // レンダリングマネージャ
  app::util::AppRenderingManager* m_appRenderingManager;
};


GFL_NAMESPACE_END(GameOver)
GFL_NAMESPACE_END(app)

#endif // __GAMEOVERIMPL_H__ 重複定義防止
