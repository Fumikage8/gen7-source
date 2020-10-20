#pragma once
//=================================================================================================
/**
 *  @file   BaseCollisionScene.h
 *  @brief  ベースコリジョンシーン
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <types/include/gfl2_Typedef.h>
#include <math/include/gfl2_Vector4.h>
#include "RaycastCustomCallback.h"

class BaseSolver;
class BaseActor;
class StaticActor;
class DynamicActor;
class CylinderShape;

namespace gfl2 {
  namespace gfx {
    class IGLAllocator;
  }
  namespace renderingengine { namespace scenegraph { namespace instance {
    class InstanceNode;
  } } }
}

//----------------------------------------------------------------------------------------
//! @brief コリジョンシステム
//----------------------------------------------------------------------------------------
class BaseCollisionScene
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  BaseCollisionScene();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~BaseCollisionScene();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  初期化
  *
  *  @param  pAllocator  利用するアロケータ
  *  @param  pHeap       利用するヒープ
  */
  //-----------------------------------------------------------------------------
  virtual void Setup(gfl2::gfx::IGLAllocator* pAllocator,gfl2::heap::HeapBase* pHeap);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  削除
  */
  //-----------------------------------------------------------------------------
  virtual void Terminate(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  コリジョン判定
  */
  //-----------------------------------------------------------------------------
  virtual void Update(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  メッシュ形状を軽量化し保持する
  *
  *  @param  pos         ポリゴンを収集する中心値
  *  @param  collectionR ポリゴンを収集する範囲
  */
  //-----------------------------------------------------------------------------
  virtual void MeshCooking(gfl2::math::Vector3 pos,f32 collectionR);

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突されるアクターの作成
  *
  *  @param  pAllocator  利用するアロケータ
  *  @param  pHeap       利用するヒープ
  *  @return アクター
  */
  //-----------------------------------------------------------------------------
  StaticActor* CreateStaticActor(gfl2::gfx::IGLAllocator* pAllocator,gfl2::heap::HeapBase* pHeap);

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突するアクターの作成
  *
  *  @param  pAllocator  利用するアロケータ
  *  @param  pHeap       利用するヒープ
  *  @return アクター
  */
  //-----------------------------------------------------------------------------
  DynamicActor* CreateDynamicActor(gfl2::gfx::IGLAllocator* pAllocator,gfl2::heap::HeapBase* pHeap);

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突されるアクターの追加
  *
  *  @param  pActor  コリジョンシステムに登録したいアクター
  */
  //-----------------------------------------------------------------------------
  void RegistStaticActor(StaticActor *pActor);

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突するアクターの追加
  *
  *  @param  pActor  コリジョンシステムに登録したいアクター
  */
  //-----------------------------------------------------------------------------
  void RegistDynamicActor(DynamicActor *pActor);

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突されるアクターの削除
  *
  *  @param  pActor  コリジョンシステムから削除したいアクター
  */
  //-----------------------------------------------------------------------------
  void UnRegistStaticActor(StaticActor *pActor);

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突するアクターの削除
  *
  *  @param  pActor  コリジョンシステムから削除したいアクター
  */
  //-----------------------------------------------------------------------------
  void UnRegistDynamicActor(DynamicActor *pActor);

  //----------------------------------------------------------------------------
  /**
  *  @brief  シーンに登録されている衝突されるメッシュに対してのレイキャスト
  *
  *  @param  start     レイ開始位置
  *  @param  end       レイ終了位置
  *  @param  pHitData  レイの結果を受け取るための構造体(スタートからの最近接点が返ってきます)
  *  @return           レイキャストがヒットしたかどうか
  */
  //-----------------------------------------------------------------------------
  b32 RaycastFromStaticActorsMesh(gfl2::math::Vector4& start,gfl2::math::Vector4& end,RaycastCustomCallback::HIT_DATA* pHitData);

  //----------------------------------------------------------------------------
  /**
  *  @brief  シーンに登録されている衝突されるメッシュに対してのレイキャスト
  *
  *  @param  start     レイ開始位置
  *  @param  end       レイ終了位置
  *  @return           レイキャストがヒットした回数( 計測を失敗した場合は-1を返します )
  */
  //-----------------------------------------------------------------------------
  s32 RaycastFromStaticActorsMesh(gfl2::math::Vector4& start,gfl2::math::Vector4& end);

  //----------------------------------------------------------------------------
  /**
  *  @brief  シーンに登録されている衝突されるシリンダーに対してのレイキャスト
  *
  *  @param  start     レイ開始位置
  *  @param  end       レイ終了位置
  *  @return           レイキャストがヒットしたかどうか
  */
  //-----------------------------------------------------------------------------
  b32 RaycastFromStaticActorsCylinder(gfl2::math::Vector4& start,gfl2::math::Vector4& end);

  //----------------------------------------------------------------------------
  /**
  *  @brief  シーンに登録されている衝突される箱に対してのレイキャスト
  *
  *  @param  start     レイ開始位置
  *  @param  end       レイ終了位置
  *  @return           レイキャストがヒットしたかどうか
  */
  //-----------------------------------------------------------------------------
  b32 RaycastFromStaticActorsBox(gfl2::math::Vector4& start,gfl2::math::Vector4& end);

#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突されるアクターをデバッグ表示へ登録
  *
  *  @param  color          コリジョン表示色
  *  @param  onlyActiveFlag true:アクティブフラグが立っているアクターのみ表示する false:シーンに登録されているアクターを全て表示する
  *  @param  positon        表示位置(アクターの座標にさらに加算する値)
  *  @param  pColorTable    色指定拡張機能、カラーテーブル渡し
  *  @param  colorTableSize 色指定拡張機能、カラーテーブルのサイズ
  */
  //-----------------------------------------------------------------------------
  void DebugRegistRenderStaticActor( gfl2::math::Vector4 color, b32 onlyActiveFlag = true, gfl2::math::Vector3 positon = gfl2::math::Vector3::GetZero(), const gfl2::math::Vector4* pColorTable = NULL, u32 colorTableSize = 0 );

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突するアクターをデバッグ表示へ登録
  *
  *  @param  color          コリジョン表示色
  *  @param  onlyActiveFlag true:アクティブフラグが立っているアクターのみ表示する false:シーンに登録されているアクターを全て表示する
  *  @param  positon        表示位置(アクターの座標にさらに加算する値)
  *  @param  pColorTable    色指定拡張機能、カラーテーブル渡し
  *  @param  colorTableSize 色指定拡張機能、カラーテーブルのサイズ
  */
  //-----------------------------------------------------------------------------
  void DebugRegistRenderDynamicActor( gfl2::math::Vector4 color, b32 onlyActiveFlag = true, gfl2::math::Vector3 positon = gfl2::math::Vector3::GetZero(), const gfl2::math::Vector4* pColorTable = NULL, u32 colorTableSize = 0 );

#endif  //#if PM_DEBUG

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief   解決者の作成
  *
  *  @return  solver
  *  @note    この関数をオーバーライドすることにより衝突の挙動を変更できます
  */
  //-----------------------------------------------------------------------------
  virtual BaseSolver* CreateSolver(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief   光線vsスフィア
  */
  //-----------------------------------------------------------------------------
  u32 IntersectRaySphere(gfl2::math::Vector3 p,gfl2::math::Vector3 d,f32 dLen,f32 cylinderR,gfl2::math::Vector3 cylinderPos);

  //----------------------------------------------------------------------------
  /**
  *  @brief   光線vsOBB
  */
  //-----------------------------------------------------------------------------
  u32 IntersectRayObb(gfl2::math::Vector3 p0,
                      gfl2::math::Vector3 p1,
                      gfl2::math::Vector3 c,
                      gfl2::math::Vector3 u0,
                      gfl2::math::Vector3 u1,
                      gfl2::math::Vector3 u2,
                      gfl2::math::Vector3 e,
                      f32                 e0,
                      f32                 e1,
                      f32                 e2);

protected:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ヒープを取得
  *
  *  @return ヒープ
  */
  //-----------------------------------------------------------------------------
  gfl2::heap::HeapBase* GetHeap(void){ return m_pHeap; }

private:

  gfl2::gfx::IGLAllocator*                                   m_pAllocator;
  gfl2::heap::HeapBase*                                      m_pHeap;
  gfl2::renderingengine::scenegraph::instance::InstanceNode* m_rootStaticActorRoot;
  gfl2::renderingengine::scenegraph::instance::InstanceNode* m_rootDynamicActorRoot;
  BaseSolver*                                                m_pSolver;

};
