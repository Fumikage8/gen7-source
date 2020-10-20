#if !defined(NIJI_PROJECT_TEST_KAWADATESTMODELSAMPLE_H_INCLUDED)
#define NIJI_PROJECT_TEST_KAWADATESTMODELSAMPLE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    KawadaTestModelSample.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.04.23
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




// テスト内容の切り替え
#if defined(GF_PLATFORM_WIN32)
  #define WIN32_IKEUCHI_FOG_TEST   (0)  // Win32版の池内さんによるフォグのテストを行う場合1  // これが0でもWin32版でフォグは出ていますので0でいいです。
#elif defined(GF_PLATFORM_CTR)
  #define WIN32_IKEUCHI_FOG_TEST   (0)  // CTR版では必ず0
#endif  // GF_PLATFORM_WIN32, GF_PLATFORM_CTR




// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
  
  GFL_NAMESPACE_BEGIN(renderingengine)
    
    GFL_NAMESPACE_BEGIN(scenegraph)
      GFL_NAMESPACE_BEGIN(resource)
        class ResourceNode;
      GFL_NAMESPACE_END(resource)
      GFL_NAMESPACE_BEGIN(instance)
        class DrawEnvNode;
      GFL_NAMESPACE_END(instance)
    GFL_NAMESPACE_END(scenegraph)
  
  GFL_NAMESPACE_END(renderingengine)

  GFL_NAMESPACE_BEGIN(animation)
    class DrawEnvAnimationController;
  GFL_NAMESPACE_END(animation)
  
  GFL_NAMESPACE_BEGIN(util)
    class GLHeapAllocator;
  GFL_NAMESPACE_END(util)

GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(poke_3d)
  GFL_NAMESPACE_BEGIN(model)
    class BaseModel;
  GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)


GFL_NAMESPACE_BEGIN(System)
  class nijiAllocator;
GFL_NAMESPACE_END(Syste)




GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(KawadaTest)


// 前方宣言
class TestRenderingPipeLine;


//==============================================================================
/**
 @class     KawadaTestModelSample
 @brief     KawadaTest
 */
//==============================================================================
class KawadaTestModelSample : public KawadaTestSampleBase
{
  GFL_FORBID_COPY_AND_ASSIGN(KawadaTestModelSample);

  ////////////////////////////////////////////////////////////////////////////////
public:
  KawadaTestModelSample(GameSys::GameManager* gameManager, gfl2::heap::HeapBase* parentHeapMem, gfl2::heap::HeapBase* parentDevMem);
  virtual ~KawadaTestModelSample();

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
    STEP_CREATE0,
    STEP_CREATE1,
    STEP_WORKING,
    STEP_DESTROY,
  };

private:
  void changeStep(Step step);
  void updateFunc_None(void);
  void updateFunc_Create0(void);
  void updateFunc_Create1(void);
  void updateFunc_Working(void);
  void updateFunc_Destroy(void);
  void updateForDraw(void);

private:
  Step m_step;
  u32  m_stepCount;

  ////////////////////////////////////////////////////////////////////////////////
private:
#if WIN32_IKEUCHI_FOG_TEST
  static const u32 MODEL_NUM          = 3;
  static const u32 MODEL_RESOURCE_NUM = 3;
#else 
  static const u32 MODEL_NUM          = 2;
  static const u32 MODEL_RESOURCE_NUM = 2;
#endif

private:
  void setFogParamToMaterial(void);

private:
  gfl2::util::GLHeapAllocator*                               m_glAllocator;
  System::nijiAllocator*                                     m_nijiAllocator;
  poke_3d::model::BaseModel*                                 m_model[MODEL_NUM];
  void*                                                      m_modelResourceBuf[MODEL_RESOURCE_NUM];
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_modelResourceNode[MODEL_RESOURCE_NUM];
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*  m_envNode;
  void*                                                      m_envResourceBuf;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_envResourceNode;
  TestRenderingPipeLine*                                     m_renderingPipeLine;

  // ブルームのアルファマスクテクスチャ
  void*                                                         m_bloomAlphaMaskTexResBuf;
  gfl2::renderingengine::scenegraph::resource::ResourceNode*    m_bloomAlphaMaskTexResNode;

  //! @name  Envのアニメーション
  //@{
 // Env
  void*                                                      m_anEnvResourceBuf;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_anEnvResourceNode;
  // Envのモーション
  void*                                                      m_anEnvMotResourceBuf;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_anEnvMotResourceNode;
  // Envのアニメーションコントローラー
  gfl2::animation::DrawEnvAnimationController*               m_anEnvAnimCtrl;
  //@}
};


GFL_NAMESPACE_END(KawadaTest)
GFL_NAMESPACE_END(Test)


#endif  // PM_DEBUG


#endif  // NIJI_PROJECT_TEST_KAWADATESTMODELSAMPLE_H_INCLUDED

