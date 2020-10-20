//======================================================================
/**
 * @file	BgObject.h
 * @brief	戦闘背景 パーツ
 * @author	ariizumi
 * @data	15/04/10
 */
//======================================================================
#if !defined( __BG_OBJECT_H__ )
#define __BG_OBJECT_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <Heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_FsAsyncFileManager.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlDataPack.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMdlData.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>
#include <RenderingEngine/include/SceneGraph/gfl2_SceneGraphManager.h>

#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfBinaryMotData.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_AnimationResourceNode.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

#include <Battle/Background/include/BgSystem.h>

GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(util)
    class GLHeapAllocator;
  GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(gfl2)
  GFL_NAMESPACE_BEGIN(math)
    class Quaternion;
  GFL_NAMESPACE_END(math)
GFL_NAMESPACE_END(gfl2)


GFL_NAMESPACE_BEGIN(btl)

//戦闘背景パーツ
class BgObject
{
public:

  static const int DATA_MDL_POS = 0;
  static const int DATA_ANIM_START_POS = 1;

  /*
  typedef enum
  {
    DATA_MDL,
    DATA_ANM_LOOP,
    DATA_ANM_RAIN,
    DATA_ANM_SUNLIGHT,
    DATA_ANM_CLOUD,
    DATA_ANM_WIND,
    DATA_ANM_SNOW,
    DATA_ANM_1DAY,
    DATA_ANM_MORNING,
    DATA_ANM_NOON,
    DATA_ANM_EVENING,
    DATA_ANM_NIGHT,
  }DATA_IDX;
 */

  typedef enum
  {
    ANM_SLOT_LOOP,
    ANM_SLOT_WHEATHER,
    ANM_SLOT_DAY,
    ANM_SLOT_TIME,
    ANM_SLOT_NUM
  }ANM_SLOT;

  static const int ANM_SLOT_ARR[BgSystem::ANM_MAX];
  static const bool ANM_SLOT_LOOP_ONOFF[BgSystem::ANM_MAX];

  BgObject(void);
  ~BgObject();

  void** GetBufPointer(void){return &mBuf;}
  bool Create(gfl2::util::GLHeapAllocator *allocator);
  void Delete(void);
  void Update(void);

  void SetVisible(bool flg);

  void PlayAnimeCore(int anmType);
  void StopAnimeCore(int anmType);

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelNode(void){return mMdlNode;}

  bool IsAnimeEnd(int anmType);
  bool IsAnimPlaying(int anmType);

  int GetAnimationFrame(int anmType);
  void SetAnimationFrame(int anmType,float frame);

  int GetAnimationLastFrame(int anmType);

  int GetAnimationStepFrame(int anmType);

  void SetRead(bool flg);
  bool GetRead(void);
  
  //バトルでロイヤル背景をまわす対応
  void RotateY_Deg(const gfl2::math::Quaternion &rot);

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetEnvNode(void){return m_envNode;}

  // アニメーションの更新のオンオフを切り替える
  void SetAnimationUpdateFrameEnable(bool isUpdateAnimation);
  
private:
  void *mBuf;
  gfl2::fs::BinLinkerAccessor mBinData;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mMdlRes;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode *mMdlNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_envResourceNode;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*  m_envNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mAnmNode[BgSystem::ANM_MAX];
  gfl2::animation::AnimationController mAnmController;
  bool mbAnimPlaying[ANM_SLOT_NUM];
  bool mbRead;


};
GFL_NAMESPACE_END(btl)


#endif //__BG_OBJECT_H__