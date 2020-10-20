// ============================================================================
/*
 * @file SkeltonProc.h
 * @brief SkeltonのメインProcです。
 * @date 2015.10.02
 * @author Junya Ikuta
 */
// ============================================================================
#if !defined( SKELTONPROC_H_INCLUDED )
#define SKELTONPROC_H_INCLUDED
#pragma once
#if defined(GF_PLATFORM_WIN32)

#include <macro/include/gfl2_macros.h>
#include <GameSys/include/GameProc.h>
#include <RenderingEngine/include/renderer/gfl2_RenderingPipeLine.h>
#include <Animation/include/gfl2_DrawEnvAnimationController.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_GfCollisionMdlData.h>
#include <Collision/include/gfl2_CollisionModel.h>

// niji
#include "System/include/nijiAllocator.h"
#include "System/include/RenderPath/ModelRenderPath.h"
#include <util/include/gfl2_UtilTextRenderPath.h>
#include "SkeltonNameManglingManager.h"


GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

class SkeltonFieldMapBlock;
class SkeltonFieldMapBlockManager;
class SkeltonRscManager;
class SkeltonModel;
class SkeltonModelManager;
class PlayerControl;
class SkeltonCamera;
class SkeltonCameraDataManager;
class CollisionDataManager;
class JumpDataManager;

class SkeltonProc : public GameSys::GameProc, public gfl2::renderingengine::renderer::RenderingPipeLine
{
	GFL_FORBID_COPY_AND_ASSIGN( SkeltonProc );

public:
  SkeltonProc();
  virtual~SkeltonProc();

  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pManager );
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );


  //============================================================================
  //RenderingPipeLine
  //============================================================================
  //----------------------------------------------------------------------------
  /**
  *  @brief  最初のパスから描画できる状態にする
  */
  //-----------------------------------------------------------------------------
  virtual b32 StartRenderPath(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  現状のパスにあったシーンを返す
  *  @return シーン
  */
  //-----------------------------------------------------------------------------
  virtual gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetRenderPath(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  次のパスへ遷移
  *  @return trueなら次のパス、falseなら描画終了
  */
  //-----------------------------------------------------------------------------
  virtual b32 NextRenderPath(void);

private:

  void controlPC( void );
  void CreateEnv( const c16* pMapRootPath );
  void DeleteEnv( void );
  void LoadSettingFirst( void );
  b32 LoadSetting( void );
  void UpdateCameraCollision( b32 forceChangeFlag = false );
  void UpdateMapJumpCollision(); 
  s32 GetHitMapJumpDataName( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel, CollisionDataManager* pJumpCollisionDataManager, JumpDataManager* pJumpDataManager);
  s32 GetHitMapJumpDataName_Model( gfl2::math::Vector4& pos, gfl2::collision::CollisionModel* pCollisionModel, JumpDataManager* pJumpDataManager);
  s32 GetHitMapJumpDataName_Data( gfl2::math::Vector4& pos, CollisionDataManager* pJumpCollisionDataManager, JumpDataManager* pJumpDataManager);

  void OutPutPlayerData( void );
  void OutPutCameraData( void );


  int mRenderPathCnt;
  
  gfl2::math::Matrix44 mProjMtx[2];   // 透視変換行列
  gfl2::math::Matrix34 mViewMtx[2];   // ビュー変換行列
  
  // 内部オブジェクト
  SkeltonFieldMapBlockManager*  m_pSkeltonFieldMapBlockManager;;
  SkeltonRscManager*            m_pSkeltonRscManager;
  SkeltonModelManager*          m_pSkeltonModelManager;
  PlayerControl * m_playerControl;
  SkeltonModel*   m_player;

  SkeltonCamera* m_pCamera;
  SkeltonCameraDataManager* m_pCameraDataManager;

  JumpDataManager* m_pMapJumpDataManager;
  CollisionDataManager* m_pMapJumpCollisionDataManager;

  // デバッグ機能
  SkeltonNameMangling::GroupNode* m_pDebugSkeltonNameMangling;


  // 環境関連
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pEnvNode;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*	 m_pDrawEnvNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pEnvAnimationNode;
  gfl2::animation::DrawEnvAnimationController                m_DrawEnvAnimationController;
  b32                                                        m_isInitDrawEnvAnimation;



  gfl2::heap::HeapBase    *mHeap;
  System::nijiAllocator   *mAllocator;
  gfl2::heap::NwAllocator*  m_pNwAllocator;  
  System::ModelRenderPath *mModelRenderPath;
  gfl2::util::DrawUtilText*       m_DebugDrawUtilText;
  gfl2::util::UtilTextRenderPath *m_UtilTextRenderPath;
};


GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif // GF_PLATFORM_WIN32
#endif // SKELTONPROC_H_INCLUDED
