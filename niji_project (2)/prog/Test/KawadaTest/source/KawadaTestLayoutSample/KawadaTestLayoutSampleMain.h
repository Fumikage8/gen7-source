#if !defined(NIJI_PROJECT_TEST_KAWADATESTLAYOUTSAMPLEMAIN_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTLAYOUTSAMPLEMAIN_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestLayoutSampleMain.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.15
 @brief   KawadaTest
 */
//==============================================================================


#if PM_DEBUG




#define TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST (1)




#define TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL (0)  // どちらか一方だけ1にしていい
#define TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL   (1)  // どちらか一方だけ1にしていい




// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
#include <Effect/include/gfl2_EffectSystem.h>
#include <Effect/include/gfl2_EffectRenderPath.h>
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL

// nijiのインクルード
#include <GameSys/include/GameManager.h>

// KawadaTestのインクルード
#include "../KawadaTestSampleBase.h"




// 前方宣言
GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(util)
    class Heap;
    class AppRenderingManager;
    class AppCamera;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(applib)
  GFL_NAMESPACE_BEGIN(frame)
    class Manager;
  GFL_NAMESPACE_END(frame)
GFL_NAMESPACE_END(applib)

GFL_NAMESPACE_BEGIN(Test)
  GFL_NAMESPACE_BEGIN(KawadaTest)
    class KawadaTestLayoutSampleDataManager;
    class KawadaTestLayoutSampleFrameListener;
    class KawadaTestLayoutSampleFrameCell;
  GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
  class PokeModel;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(renderingengine)
    GFL_NAMESPACE_BEGIN(scenegraph)
      GFL_NAMESPACE_BEGIN(instance)
        class DrawEnvNode;
      GFL_NAMESPACE_END(instance)
    GFL_NAMESPACE_END(scenegraph)
  GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)
 
GFL_NAMESPACE_BEGIN(poke_3d)
  GFL_NAMESPACE_BEGIN(model)
    class BaseCamera;
  GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(System)
  class NijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(util)
    class RenderTargetTexture;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST


#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(util)
    class EffectUtil;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestLayoutSampleMain
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestLayoutSampleMain : public KawadaTestSampleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestLayoutSampleMain);

  ////////////////////////////////////////////////////////////////////////////////
public:
  KawadaTestLayoutSampleMain(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual ~KawadaTestLayoutSampleMain();

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
  KawadaTestLayoutSampleDataManager*  m_dataManager;

  // フレーム
  applib::frame::Manager*               m_frameManager;
  KawadaTestLayoutSampleFrameListener*  m_frameListener;
  KawadaTestLayoutSampleFrameCell*      m_frameCell;
  bool                                  m_frameCreated;     // trueのときフレームが生成された
  bool                                  m_frameReqDestroy;  // trueのときフレームが破棄をリクエストしている

  // レンダリングマネージャ
  app::util::AppRenderingManager*  m_appRenderingManager;


#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST
  System::nijiAllocator*     m_vramAllocator;  //!< VRAMアロケータ
  PokeTool::PokeModelSystem* m_pokeModelSys;
  PokeTool::PokeModel*       m_pokeModel;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNodeOfCamera;  // カメラを含む環境
  poke_3d::model::BaseCamera*                               m_camera;               // カメラ
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNodeOfLight;   // ライト
  app::util::RenderTargetTexture* m_renderTargetTexture;
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_POKEMON_MODEL_OFF_SCREEN_TEST

 
  app::util::AppCamera* m_appCamera;
  app::util::AppCamera* m_appCameraLower;


#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL
  // エフェクト
  gfl2::Effect::EffectRenderPath::Config  m_effectRenderPathConfig;
  gfl2::Effect::System*                   m_effectSys;
  gfl2::Effect::GFHeap*                   m_effectHeap;
  gfl2::util::List<u32>*                  m_effectGroupIDList;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_effectDrawEnvNodeOfCamera;  // カメラを含む環境
  poke_3d::model::BaseCamera*                               m_effectCamera;               // カメラ
  static const s32                        m_effectEmitterSetID = 0;  // esetファイルをbptclファイルへコンバートしたときにエミッターセットIDは決まる
  static const s32                        m_effectResID        = 0;  // プログラマがソース内で決める
  static const u8                         m_effectGroupID      = 0;  // プログラマがソース内で決める
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_NORMAL

#if TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
  // エフェクト
  app::util::EffectUtil*                  m_effectUtil;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_effectDrawEnvNodeOfCamera;  // カメラを含む環境
  poke_3d::model::BaseCamera*                               m_effectCamera;               // カメラ
  bool     m_isFirst;
  static const s32                        m_effectEmitterSetID = 0;  // esetファイルをbptclファイルへコンバートしたときにエミッターセットIDは決まる
  static const s32                        m_effectResID        = 0;  // プログラマがソース内で決める
  static const u8                         m_effectGroupID      = 0;  // プログラマがソース内で決める
#endif  // TEST_KAWADATESTLAYOUTSAMPLEMAIN_EFFECT_UTIL
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTLAYOUTSAMPLEMAIN_H_INCLUDED

