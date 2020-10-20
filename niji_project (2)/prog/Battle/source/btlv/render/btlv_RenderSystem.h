//======================================================================
/**
 * @file  btlv_RenderSystem.cpp
 * @brief バトル描画 GLAllocatar
 * @author  ariizumi
 * @data  15/03/07
 */
//======================================================================
#ifndef __BTLV_RENDER_SYSTEM_H__
#define __BTLV_RENDER_SYSTEM_H__
#pragma once

#include <Math/include/gfl2_Matrix.h>
#include <Math/include/gfl2_Quaternion.h>
#include <Macro/include/gfl2_Macros.h>
#include <DemoLib/Object/include/ObjectRenderingPipeLine.h>

#include "System/include/nijiAllocator.h"

#include "../btlv_Types.h"
#include "btlv_RenderPipeLine.h"

GFL_NAMESPACE_BEGIN( gfl2 )
  GFL_NAMESPACE_BEGIN( renderingengine )
    GFL_NAMESPACE_BEGIN( scenegraph )
      GFL_NAMESPACE_BEGIN( resource )
        class ResourceNode;
      GFL_NAMESPACE_END( resource )
      GFL_NAMESPACE_BEGIN( instance )
        class ModelInstanceNode;
        class DrawEnvNode;
        class LightSetNode;
        class LightNode;
      GFL_NAMESPACE_END( instance )
    GFL_NAMESPACE_END( scenegraph )
  GFL_NAMESPACE_END( renderingengine )
  GFL_NAMESPACE_BEGIN( gfx )
    class IGLAllocator;
  GFL_NAMESPACE_END( gfx )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( app )
  GFL_NAMESPACE_BEGIN( util )
    class AppRenderingManager;
  GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( app )

class BattleRenderingPipeLine;

GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;
class BtlvGLAllocator;

class BtlvRenderSystem
{
public:
  BtlvRenderSystem(BattleViewSystem *btlvSys);
  ~BtlvRenderSystem();

  void Initialize(void);
  void Terminate(void);

  //初期化
  bool Setup(void);
  //終了
  bool Cleanup(void);

  //更新
  void CalcMatrix(void);  //天球のUpdateのために分離
  void Update(void);
  //描画
  void PreDraw(void);
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  BattleRenderingPipeLine* GetRenderingPipeLine(void){return mRenderingPipeLine;}

  ::app::util::AppRenderingManager* GetAppRenderingManager(void){return mAppRenderingManager;}

  gfl2::math::Matrix34* GetCamViewMtx(void){return &mCamViewMtx;}
  void SetCamViewMtx(const gfl2::math::Matrix34 &mtx);
  gfl2::math::Matrix44* GetCamProjMtx(void){return &mCamProjMtx;}
//  void SetCamProjMtx(const gfl2::math::Matrix44 &mtx){mCamProjMtx = mtx;}
  //天球用にNear・Farが違うやつを作るので数値をもらう。
  void SetCamProjMtx(f32 fovy, f32 aspect,f32 nearVal,f32 farVal);

  //視錘台カリング用テーブル
  gfl2::math::Matrix44* GetFrustumCullingTable(void);

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode* GetDrawEnvNode(void){return m_pDrawEnvNode;}
  gfl2::renderingengine::scenegraph::instance::LightNode* GetLightNode(void)const{return mLightNode;}
  gfl2::renderingengine::scenegraph::instance::LightNode* GetCharaLightNode(int idx)const{return mCharaLightNode[idx];}

private:
  //仮
  void SetupDebugEnv(void);
  void SetupDebugData(void);
  gfl2::math::Matrix44 mCamProjMtx;   // 透視変換行列
  gfl2::math::Matrix34 mCamViewMtx;   // ビュー変換行列
  gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列
  gfl2::math::Matrix44 mCamFarProjMtx;   // 透視変換行列(遠景用にNear・Farが違う)
  gfl2::math::Matrix44 mFarProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mFarViewMtx[2];   // ビュー変換行列
  gfl2::math::Matrix44 mCamSkyProjMtx;   // 透視変換行列(天球用にNear・Farが違う)
  gfl2::math::Matrix44 mSkyProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mSkyViewMtx[2];   // ビュー変換行列

  gfl2::renderingengine::scenegraph::instance::DrawEnvNode		*m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::instance::LightNode      *mLightNode;
  gfl2::renderingengine::scenegraph::instance::LightNode      *mCharaLightNode[TRAINER_MODEL_NUM];
  gfl2::gfx::Texture			*mDepthStencilTexture;

  bool mUpdateCamMatrix;
  gfl2::math::Matrix44 mFrustumCullingTable[4];

  int mSetupSeq;
  bool mIsFinishSetup;
  BattleViewSystem *mBtlvCore;

  BattleRenderingPipeLine *mRenderingPipeLine;
  ::app::util::AppRenderingManager* mAppRenderingManager;

  void *mEdgeMapRenderPathRes;
  void *mOutLineRenderPathRes;
  void *mFullPowerLineRenderPathRes;

};


GFL_NAMESPACE_END(btl)

#endif //__BTLV_RENDER_SYSTEM_H__