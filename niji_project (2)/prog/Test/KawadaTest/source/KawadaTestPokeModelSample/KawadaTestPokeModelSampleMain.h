#if !defined(NIJI_PROJECT_TEST_KAWADATESTPOKEMODELSAMPLEMAIN_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTPOKEMODELSAMPLEMAIN_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestPokeModelSampleMain.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.15
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
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(renderingengine)
GFL_NAMESPACE_BEGIN(scenegraph)
GFL_NAMESPACE_BEGIN(resource)
  class GfBinaryMdlDataPack;
  class GfBinaryMotData;
  class ResourceNode;
GFL_NAMESPACE_END(resource)
GFL_NAMESPACE_END(scenegraph)
GFL_NAMESPACE_END(renderingengine)
GFL_NAMESPACE_END(gfl2)
 
GFL_NAMESPACE_BEGIN(app)
  GFL_NAMESPACE_BEGIN(util)
    class Heap;
    class AppRenderingManager;
    class AppCamera;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(PokeTool)
  class PokeModelSystem;
  class PokeModel;
GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(poke_3d)
  GFL_NAMESPACE_BEGIN(model)
    class BaseCamera;
  GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(System)
  class nijiAllocator;
GFL_NAMESPACE_END(System)

GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(Camera)
  class CModelViewerInFrame;
  class InFrameCamera;
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_END(System)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


//==============================================================================
/**
 @class     KawadaTestPokeModelSampleMain
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestPokeModelSampleMain : public KawadaTestSampleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestPokeModelSampleMain);

  ////////////////////////////////////////////////////////////////////////////////
public:
  KawadaTestPokeModelSampleMain(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual ~KawadaTestPokeModelSampleMain();

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
  // ポケモンモデル
  bool asyncCreatePokeModel(void);
  void addPokeModel(void);
  void removePokeModel(void);
  bool asyncDestroyPokeModel(void);

private:
  // DrawUtil
  void initDrawUtil(void);
  void termDrawUtil(void);
  void drawDrawUtil(gfl2::gfx::CtrDisplayNo displayNo);  // gflib2/gfx/Sample/DrawUtil/main.cppを参考にした

private:
  // InFrame
  void testInFrameDebugDraw(gfl2::gfx::CtrDisplayNo displayNo);  // インフレームのデバッグ表示がうまくできるかのテスト

private:
  // メモリ
  app::util::Heap*       m_appHeap;                         //!< このアプリのapp::util::Heap
  System::nijiAllocator* m_nijiAllocatorForEdgeMapTexture;  //!< VRAMアロケータ

  // レンダリングマネージャ
  app::util::AppRenderingManager*  m_appRenderingManager;

  // ポケモンモデル
  static const u32 POKE_MODEL_NUM = 2;
  s32                        m_asyncCreateDestroyPokeModelStep;
  PokeTool::PokeModelSystem* m_pokeModelSys;
  PokeTool::PokeModel*       m_pokeModel[POKE_MODEL_NUM];
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNodeOfCamera;  // カメラを含む環境
  poke_3d::model::BaseCamera*                               m_camera;               // カメラ
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_drawEnvNode;          // ライトなどの環境

  // インフレーム
  System::Camera::CModelViewerInFrame* m_inFrameSys;
  System::Camera::InFrameCamera*       m_inFrameCamera;

  // AppCamera
  app::util::AppCamera*  m_appCamera;

  // テストモデル
  void**                                                             m_testModelDataPackBuf;
  void**                                                             m_testMotDataBuf;
  gfl2::renderingengine::scenegraph::resource::GfBinaryMdlDataPack** m_testModelDataPack;
  gfl2::renderingengine::scenegraph::resource::GfBinaryMotData**     m_testMotData;
  gfl2::renderingengine::scenegraph::resource::ResourceNode**        m_testModelResourceNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode**        m_testMotResourceNode;
  poke_3d::model::BaseModel**                                        m_testBaseModel;        
  s32                                                                m_testModelCount;
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTPOKEMODELSAMPLEMAIN_H_INCLUDED

