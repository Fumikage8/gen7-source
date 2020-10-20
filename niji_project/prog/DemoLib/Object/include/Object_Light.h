//======================================================================
/**
 * @file	Object_Light.h
 * @brief	ライトＯＢＪ (ENV+MOT)
 * @author	Pete
 * @data	15/07/16
 */
//======================================================================
#if !defined( __OBJECT_LIGHT_H__ )
#define __OBJECT_LIGHT_H__
#pragma once


#include <fs/include/gfl2_Fs.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>
#include <animation/include/gfl2_DrawEnvAnimationController.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryEnvData.h>
#include <DemoLib/Object/include/Object.h>

GFL_NAMESPACE_BEGIN(DemoLib)
  GFL_NAMESPACE_BEGIN(Obj)

class ObjectRenderPipeLine;

class Object_Light :public Object
{
public:
    
  Object_Light(ObjectRenderPipeLine *pipe);  
  virtual ~Object_Light();

  virtual void Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator);  
  virtual void CreateBuffer(void *buf,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator);
  virtual void Delete(void);

  //ObjectRenderPipeLine経由で追加する
  virtual void AddRenderPath(int opt = 0);
  virtual void RemoveRenderPath(void);
  virtual bool IsAddRenderPath(void){return mIsAddRenderPath;}

  void ChangeAnimation( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,gfl2::heap::HeapBase *heap,bool loop );
  void UpdateAnimation(int step);

  gfl2::animation::DrawEnvAnimationController *GetAnimationController(void) { return(m_AnimationController); }

protected:
  virtual void UpdateAlways(int step=1);

private:
  bool mIsAddRenderPath;
  ObjectRenderPipeLine *mRenderingPipeLine;
  int mSceneOpt;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode *m_pDrawEnvNode;	
  gfl2::renderingengine::scenegraph::resource::ResourceNode *m_pDrawEnvResourceRootNode;                   ///< DrawEnvリソース
  gfl2::animation::DrawEnvAnimationController *m_AnimationController;
};

  GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_LIGHT_H__