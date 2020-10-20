#if !defined(NIJI_PROJECT_TEST_KAWADATESTABCDEFSAMPLEMAIN_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTABCDEFSAMPLEMAIN_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestAbcdefSampleMain.h
 @author  kawada_koji@gamefreak.co.jp
 @date    0000.00.00
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// nijiのインクルード
#include <GameSys/include/GameManager.h>

// KawadaTestのインクルード
#include "../KawadaTestSampleBase.h"




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

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(KawadaTest)
    class KawadaTestAbcdefSampleDataManager;
    class KawadaTestAbcdefSampleFrameListener;
    class KawadaTestAbcdefSampleFrameCell;
  GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestAbcdefSampleMain
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestAbcdefSampleMain : public KawadaTestSampleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestAbcdefSampleMain);

  ////////////////////////////////////////////////////////////////////////////////
public:
  KawadaTestAbcdefSampleMain(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual ~KawadaTestAbcdefSampleMain();

  virtual void Update(void);
  virtual void PreDraw(void);
  virtual void Draw(gfl2::gfx::CtrDisplayNo displayNo);

private:
  virtual void initialize(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual void terminate(void);

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
  Step m_step;
  u32  m_stepCount;
  
  ////////////////////////////////////////////////////////////////////////////////
private:
  // メモリ
  app::util::Heap*       m_appHeap;             //!< このアプリのapp::util::Heap
  gfl2::heap::HeapBase*  m_frameManagerUniMem;  //!< フレームマネージャのヒープ  // フレームマネージャのヒープは荒れるので別途管理

  // データマネージャ
  KawadaTestAbcdefSampleDataManager*  m_dataManager;

  // フレーム
  applib::frame::Manager*               m_frameManager;
  KawadaTestAbcdefSampleFrameListener*  m_frameListener;
  KawadaTestAbcdefSampleFrameCell*      m_frameCell;
  bool                                  m_frameCreated;     // trueのときフレームが生成された
  bool                                  m_frameReqDestroy;  // trueのときフレームが破棄をリクエストしている

  // レンダリングマネージャ
  app::util::AppRenderingManager*  m_appRenderingManager;
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTABCDEFSAMPLEMAIN_H_INCLUDED

