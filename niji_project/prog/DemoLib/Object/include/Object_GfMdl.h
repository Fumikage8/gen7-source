//======================================================================
/**
 * @file	Object_GfMdl.h
 * @brief	オブジェクト GfMdl
 * @author	ariizumi
 * @data	15/03/16
 */
//======================================================================
#if !defined( __OBJECT_GFMDL_H__ )
#define __OBJECT_GFMDL_H__
#pragma once


#include <fs/include/gfl2_Fs.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

#include <DemoLib/Object/include/Object.h>

GFL_NAMESPACE_BEGIN( gfl2 )
  GFL_NAMESPACE_BEGIN( renderingengine )
    GFL_NAMESPACE_BEGIN( scenegraph )
      GFL_NAMESPACE_BEGIN( resource )
        class ResourceNode;
        class GfBinaryMotData;
      GFL_NAMESPACE_END( resource )
      GFL_NAMESPACE_BEGIN( instance )
        class ModelInstanceNode;
      GFL_NAMESPACE_END( instance )
    GFL_NAMESPACE_END( scenegraph )
  GFL_NAMESPACE_END( renderingengine )
  GFL_NAMESPACE_BEGIN( gfx )
    class IGLAllocator;
  GFL_NAMESPACE_END( gfx )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( poke_3d )
  GFL_NAMESPACE_BEGIN( model )
    class BaseModel;
  GFL_NAMESPACE_END( model )
GFL_NAMESPACE_END( poke_3d )

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class ObjectRenderPipeLine;

class Object_GfMdl :public Object
{
public:
  struct LOAD_PARAM
  {
    gfl2::fs::ArcFile::ARCID    arcId;
    gfl2::fs::ArcFile::ARCDATID mdlDataId;
    gfl2::fs::ArcFile::ARCDATID anmDataId;
    bool                        isComp;
    gfl2::heap::HeapBase        *dataHeap;
    gfl2::heap::HeapBase        *workHeap;
#if PM_DEBUG
    char *mdlFilePath;
    char *anmFilePath;
#endif
    LOAD_PARAM()
      :arcId(gfl2::fs::ArcFile::ARCID_NULL)
      ,mdlDataId(gfl2::fs::ArcFile::ARCDATID_NULL)
      ,anmDataId(gfl2::fs::ArcFile::ARCDATID_NULL)
      ,isComp(false)
      ,dataHeap(NULL)
      ,workHeap(NULL)
#if PM_DEBUG
      ,mdlFilePath(NULL)
      ,anmFilePath(NULL)
#endif
      {}
  };


  //コンストラクタ
  Object_GfMdl(ObjectRenderPipeLine *pipe);
  //デストラクタ
  virtual ~Object_GfMdl();

  //モデル生成
  virtual void StartLoad(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param);
#if PM_DEBUG
  virtual void StartLoad_Debug(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param);
#endif
  virtual bool IsFinishLoad(void);
  virtual void Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator);
  virtual void CreateBufferTexture(void *buf,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,gfl2::renderingengine::scenegraph::DagNode* pTextureShaderNodeRoot);
  virtual void CreateBuffer(void *buf,gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator);
  virtual void Delete(void);

  //ObjectRenderPipeLine経由で追加する
  virtual void AddRenderPath(int opt = 0);
  virtual void RemoveRenderPath(void);
  virtual bool IsAddRenderPath(void){return mIsAddRenderPath;}
  //継承
  virtual void SetVisible(const bool flg);

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelNode(void);
  poke_3d::model::BaseModel* GetBaseModel(void);

  void SetAnimation( gfl2::gfx::IGLAllocator *allocator , void*buf,int aniSlot=0 );
  void RemoveAnimation(int aniSlot);
  virtual void SetAnimationIsLoop( bool flg,int aniSlot=0 );
  virtual void SetAnimationSpeed(f32 speed,int aniSlot=0 );
  void SetAnimationFrame(f32 frame,int aniSlot=0 );
  b32 IsFinishAnimation(void);
  void UpdateAnimation(int step);
  void UpdatePosition(void);
  
  //追従処理用
  virtual void UpdateAnimationForce(void);
  virtual gfl2::renderingengine::scenegraph::instance::JointInstanceNode* GetJointNode( char *name );

  //コンスタントカラー操作
  gfl2::math::Vector4 GetConstantColor(const char* matName , const int colNo );
  //名前NULL指定で全マテリアル対象
  void SetConstantColor(const char* matName , const int colNo , const gfl2::math::Vector4 &color );

protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //こっちはUpdateCoreの後に呼ばれる。座標反映後の更新はこちらで。
  virtual void UpdateAlwaysAfterCore(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);

  gfl2::renderingengine::scenegraph::instance::MaterialInstanceNode* GetMaterialNode(const char* matName);

  gfl2::fs::AsyncFileManager *mFileManager;

  ObjectRenderPipeLine *mRenderingPipeLine;

  void *mMdlRes;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mMdlResNode;
  poke_3d::model::BaseModel *mModel;

  bool mIsLoadAnm;
  void *mAnmRes;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mAnmResNode;

  gfl2::animation::AnimationController mAnmController;
  gfl2::animation::JointController     mJointController;

  bool mIsAddRenderPath;
  int mSceneOpt;
  bool aniSlot1Loaded;


#if PM_DEBUG
  bool mIsDebugLoad;
#endif
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_GFMDL_H__