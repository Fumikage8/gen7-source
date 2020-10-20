//======================================================================
/**
 * @file	Object_Poke.h
 * @brief	オブジェクト PokeModel
 * @author	ariizumi
 * @data	15/05/16
 */
//======================================================================
#if !defined( __OBJECT_POKE_H__ )
#define __OBJECT_POKE_H__
#pragma once


#include <fs/include/gfl2_Fs.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>

#include <Animation/include/gfl2_AnimationController.h>
#include <Animation/include/gfl2_JointController.h>

#include <DemoLib/Object/include/Object.h>

#include "PokeTool/include/PokeModelSystem.h"

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

GFL_NAMESPACE_BEGIN(DemoLib)
GFL_NAMESPACE_BEGIN(Obj)

class ObjectRenderPipeLine;

class Object_Poke :public Object
{
public:
  struct LOAD_PARAM
  {
    PokeTool::PokeModelSystem *modelSys;
    PokeTool::SimpleParam     *param;
    PokeTool::PokeModel::SetupOption *opt;

    gfl2::heap::HeapBase        *dataHeap;
    gfl2::heap::HeapBase        *workHeap;
    LOAD_PARAM()
      :modelSys(NULL)
      ,param(NULL)
      ,opt(NULL)
      ,dataHeap(NULL)
      ,workHeap(NULL)
    {}
  };


  //コンストラクタ
  Object_Poke(ObjectRenderPipeLine *pipe);
  //デストラクタ
  virtual ~Object_Poke();

  //モデル生成
  virtual void StartLoad(gfl2::fs::AsyncFileManager *fileMng, const LOAD_PARAM &param);
  virtual bool IsFinishLoad(void);
  virtual void Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,bool useShader);
  virtual void Delete(void);

  void ForceDelete(void);

  //ObjectRenderPipeLine経由で追加する
  virtual void AddRenderPath(int opt = 0);
  virtual void RemoveRenderPath(void);
  virtual bool IsAddRenderPath(void){return mIsAddRenderPath;}
  //継承
  virtual void SetVisible(const bool flg);
  virtual void SetShadowVisible(const bool flg);

  const gfl2::math::Vector4 GetShaderColor(void)const;

  /**
   * @brief カラーシェーダの色の設定
   * @param color カラーシェーダで適用する色
   * @param executeFrame 実行するフレーム
   * @note executeFrameは基本1. 処理落ち発生の際に, 確実に実行してほしいフレーム数を指定する
   */
  void SetShaderColor(const gfl2::math::Vector4& col, s32 executeFrame = 1);
  
  //アニメーション関係
  // @value forceReset モーション番号が同じだった場合リセットをかけるか？
  void ChangeAnimation( PokeTool::MODEL_ANIME type , bool forceReset = false );       //推奨
  virtual void ChangeAnimationDirect( int type , bool forceReset = false );
  PokeTool::MODEL_ANIME GetAnimationType(void);
  f32 GetAnimationMaxFrame(void);
  void SetAnimationFrame(f32 frame);
  bool IsFinishAnimation(void);
  bool IsAvailableAnimation( PokeTool::MODEL_ANIME type );
  f32 GetAnimationSpeed(void);
  void SetAnimationSpeed(f32 spd);
  //リソース渡し再生(全力技・デモ等外部でモーションを持つ場合)
  //戻す際は普通にChangeAnimationしてください。
  void ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot=0 );
  bool IsPlayAnimationResource(int aniSlot=0 );

  //現在のアニメーションが終了した時に自動的に切り替える
  void SetReserveAnimation( PokeTool::MODEL_ANIME type ){mReserveAnime = type;}
  void SetAnimationIsLoop( bool flg, int aniSlot=0 );

  bool GetNodePosition( PokeTool::MODEL_NODE node , int idx , gfl2::math::Vector3 *ret);

  PokeTool::PokeModel* GetPokeModel(void){return mModel;}

  void UpdateAnimation(int step=1,int slot=0);
  void UpdatePosition(void);

  //追従処理用
  virtual void UpdateAnimationForce(void);

  // IsEgg:
  bool IsEgg(void) { return(mIsEgg); }

protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);

  gfl2::fs::AsyncFileManager *mFileManager;

  ObjectRenderPipeLine *mRenderingPipeLine;

  PokeTool::PokeModelSystem *mModelSys;
  PokeTool::PokeModel *mModel;

  bool mIsAddRenderPath;
  int mSceneOpt;
  int mReserveAnime;
  //外部リソースのアニメ再生中か？
  bool mSlotAnimationLoaded[2];
  bool mIsEgg;

#if PM_DEBUG
  bool mIsDebugLoad;
#endif
};

GFL_NAMESPACE_END(Obj)
GFL_NAMESPACE_END(DemoLib)


#endif //__OBJECT_POKE_H__