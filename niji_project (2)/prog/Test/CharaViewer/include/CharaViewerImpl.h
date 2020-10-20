//======================================================================
/**
 * @file CharaViewerImpl.h
 * @date 2015/07/21 21:08:40
 * @author araki_syo
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if PM_DEBUG

#if !defined CHARA_VIEWER_IMPL_H_INCLUDED
#define CHARA_VIEWER_IMPL_H_INCLUDED
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

GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)
  class CharaViewerProcParam;
  class CharaViewerDataManager;
  class CharaViewerFrameListener;
  class CharaViewerFrameCell;
  class CharaViewerCharaModelManager;
GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)




GFL_NAMESPACE_BEGIN(test)
GFL_NAMESPACE_BEGIN(chara_viewer)


//==============================================================================
/**
 @class     CharaViewerImpl
 @brief     CharaViewerの実装
 */
//==============================================================================
class CharaViewerImpl
{
  GFL_FORBID_COPY_AND_ASSIGN(CharaViewerImpl);

public:
  //! @brief  コンストラクタ
  //! @param[in]  param        paramのポインタを保持するだけ。
  //!                          呼び出し元はparamを破棄しないように。
  //! @param[in]  appHeap      使ってもいいヒープ。
  //! @param[in]  gameManager  ゲームマネージャ。
  CharaViewerImpl(CharaViewerProcParam* param, app::util::Heap* appHeap, GameSys::GameManager* gameManager);

  virtual ~CharaViewerImpl();

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
  CharaViewerProcParam*       m_param;
  app::util::Heap*      m_appHeap;
  GameSys::GameManager* m_gameManager;

  ////////////////////////////////////////
  // 内部で生成したもの
  ////////////////////////////////////////
  // ステップ
  Step m_step;
  u32  m_stepCount;

  // データマネージャ
  CharaViewerDataManager*        m_dataManager;

  // フレーム
  gfl2::heap::HeapBase*    m_frameManagerDevMem;
  applib::frame::Manager*  m_frameManager;
  CharaViewerFrameListener*      m_frameListener;
  CharaViewerFrameCell*          m_frameCell;
  bool                     m_frameCreated;     // trueのときフレームが生成された
  bool                     m_frameReqDestroy;  // trueのときフレームが破棄をリクエストしている
  
  // レンダリングマネージャ
  app::util::AppRenderingManager* m_appRenderingManager;

  // キャラ描画
  CharaViewerCharaModelManager* m_charaModelManager;
};


GFL_NAMESPACE_END(chara_viewer)
GFL_NAMESPACE_END(test)

#endif // CHARA_VIEWER_IMPL_H_INCLUDED

#endif // PM_DEBUG
