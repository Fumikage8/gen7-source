#pragma once
//=================================================================================================
/**
 *  @file   BaseActor.h
 *  @brief  アクターベース
 *  @author yy-ikeuchi@gamefreak.co.jp
 *  @date   2015.02.17
 */
//=================================================================================================
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Quaternion.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceNode.h>

class BaseShape;

#define USE_MESH_COOKING 1

namespace gfl2{ namespace collision { 
  class CollisionModel;
}}

//----------------------------------------------------------------------------------------
//! @brief アクターベース
//----------------------------------------------------------------------------------------
class BaseActor : public gfl2::renderingengine::scenegraph::NodeTrait<BaseActor, gfl2::renderingengine::scenegraph::instance::InstanceNode>
{
public:

  enum SHAPE
  {
    SHAPE_NONE,
    SHAPE_CYLINDER,
    SHAPE_SPHERE,
    SHAPE_BOX,
    SHAPE_MESH,
    SHAPE_LINE,
    SHAPE_OBB,
    SHAPE_TRIANGLE,
  };

  struct CollisionResult
  {
    CollisionResult(){ init(); }
    void init  (void){ pTargetActor = NULL;}
    b32  isFree(void){ return pTargetActor ? false:true; }

    BaseActor* pTargetActor;  //< ぶつかったアクター
  };

  // CollisionResultの保持数
  static const s32 MAX_COLLISION_RESULT_CNT = 4;

  // CookingDataの最大数
  static const u32 MAX_COOKING_SIZE = 256;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  BaseActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~BaseActor();

  //-----------------------------------------------------------------------------
  /**
  *  @brief  解放
  */
  //-----------------------------------------------------------------------------
  void Clear(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作するかどうかを設定
  *
  *  @param  動作するかどうか
  */
  //-----------------------------------------------------------------------------
  void SetActive(b32 isActive){ m_isActive = isActive; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作するかどうかを取得
  *
  *  @retval 動作するかどうか
  */
  //-----------------------------------------------------------------------------
  b32 IsActive(void){ return m_isActive; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ヒープの設定
  *
  *  @param  ヒープ
  *  @note   BaseCollisionScene::CreateStaticActor/BaseCollisionScene::CreateDynamicActorの内部で呼ばれるので、アプリ側で呼び出しは不要です
  */
  //-----------------------------------------------------------------------------
  void SetHeap(gfl2::heap::HeapBase *pHeap){  m_pHeap = pHeap; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ヒープの取得
  *
  *  @return ヒープ
  */
  //-----------------------------------------------------------------------------
  gfl2::heap::HeapBase* GetHeap(void){ return m_pHeap; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  シェイプタイプを取得
  *
  *  @retval シェイプタイプ
  */
  //-----------------------------------------------------------------------------
  SHAPE GetShapeType(void){ return m_shape; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  シェイプオブジェクトを取得
  *
  *  @retval シェイプオブジェクト
  */
  //-----------------------------------------------------------------------------
  BaseShape* GetShape(void){ return m_pShape; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ポジションを設定
  *
  *  @param  ポジション
  */
  //-----------------------------------------------------------------------------
  void SetPosition(const gfl2::math::Vector3& position){ m_position = position; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ポジションを取得
  *
  *  @retval ポジション
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 GetPosition(void){ return m_position; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  姿勢を設定
  *
  *  @param  姿勢
  */
  //-----------------------------------------------------------------------------
  void SetQuaternion(const gfl2::math::Quaternion& quaternion){ m_quaternion = quaternion; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  姿勢を取得
  *
  *  @retval 姿勢
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Quaternion GetQuaternion(void){ return m_quaternion; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ユーザーデータを設定
  */
  //-----------------------------------------------------------------------------
  void SetUserData(void* pUserData){ m_userData = pUserData; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ユーザーデータを取得
  *
  *  @retval ユーザーデータ
  */
  //-----------------------------------------------------------------------------
  void* GetUserData(void){ return m_userData; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突の結果を初期化
  */
  //-----------------------------------------------------------------------------
  void InitCollisionResult(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突の結果を取得
  *
  *  @param  要素
  *  @retval 衝突結果
  */
  //-----------------------------------------------------------------------------
  CollisionResult* GetCollisionResult(u32 index){ return &m_result[index]; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突結果がトリガーかどうか調べる
  *
  *  @param  衝突結果
  *  @retval trueならトリガー、falseならホールド
  */
  //-----------------------------------------------------------------------------
  b32 CheckTrg(CollisionResult* pCollisionResult);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにシリンダーを追加
  *
  *  @param  pos        シリンダーの中心位置
  *  @param  r          シリンダーの半径
  *  @param  height     シリンダーの高さ
  */
  //-----------------------------------------------------------------------------
  void CreateCylinder( const gfl2::math::Vector3& pos,
                       const f32                  r,
                       const f32                  height);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにスフィアを追加
  *
  *  @param  pos        スフィアの中心位置
  *  @param  r          スフィアの半径
  */
  //-----------------------------------------------------------------------------
  void CreateSphere( const gfl2::math::Vector3& pos,
                     const f32                  r );

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにBox(X軸/Z軸回転は未対応)を追加
  *
  *  @param  pos        長方形の中心点
  *  @param  u0         長方形のローカルなX軸を表す単位ベクトル
  *  @param  u1         長方形のローカルなY軸を表す単位ベクトル
  *  @param  e0         X軸にそった長方形の長さの半分
  *  @param  e1         Y軸にそった長方形の長さの半分
  *  @param  height     長方形の高さ
  */
  //-----------------------------------------------------------------------------
  void CreateBox( const gfl2::math::Vector3& pos,
                  const gfl2::math::Vector3& u0,
                  const gfl2::math::Vector3& u1,
                  const f32                  e0,
                  const f32                  e1,
                  const f32                  height);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにObb(X軸/Y軸/Z軸回転に対応)を追加
  *
  *  @param  pos        長方形の中心点
  *  @param  u0         長方形のローカルなX軸を表す単位ベクトル
  *  @param  u1         長方形のローカルなY軸を表す単位ベクトル
  *  @param  u2         長方形のローカルなZ軸を表す単位ベクトル
  *  @param  e0         X軸にそった長方形の長さの半分
  *  @param  e1         Y軸にそった長方形の長さの半分
  *  @param  e2         Z軸にそった長方形の長さの半分
  */
  //-----------------------------------------------------------------------------
  void BaseActor::CreateObb( const gfl2::math::Vector3& pos,
                             const gfl2::math::Vector3& u0,
                             const gfl2::math::Vector3& u1,
                             const gfl2::math::Vector3& u2,
                             const f32                  e0,
                             const f32                  e1,
                             const f32                  e2);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにラインを追加
  *
  *  @param  pos0       ラインの先端座標
  *  @param  pos1       ラインの終端座標
  *  @param  height     ラインの高さ
  */
  //-----------------------------------------------------------------------------
  void CreateLine( const gfl2::math::Vector3 pos0,
                   const gfl2::math::Vector3 pos1,
                   const f32                 height);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにトライアングルを追加
  *
  *  @param  pos0       トライアングル座標0
  *  @param  pos1       トライアングル座標1
  *  @param  pos2       トライアングル座標2
  */
  //-----------------------------------------------------------------------------
  void CreateTriangle( const gfl2::math::Vector3 pos0,
                       const gfl2::math::Vector3 pos1,
                       const gfl2::math::Vector3 pos2);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにメッシュを追加
  *
  *  @param  pos              メッシュの中心点
  *  @param  pCollisionModel  メッシュ
  */
  //-----------------------------------------------------------------------------
  void CreateMesh( const gfl2::math::Vector3&       pos,
                   gfl2::collision::CollisionModel *pCollisionModel );

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにクッキングデータを追加
  */
  //-----------------------------------------------------------------------------
  void CreateCookingData(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  クッキングデータを取得
  *
  *  @retval クッキングデータ
  */
  //-----------------------------------------------------------------------------
  s32* GetCookingData(void){ return m_pCookingData; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void);

private:

  // 共通設定
  b32                          m_isActive;
  gfl2::heap::HeapBase        *m_pHeap;
  BaseShape                   *m_pShape;
  SHAPE                        m_shape;
  gfl2::math::Vector3          m_position;
  gfl2::math::Quaternion       m_quaternion;
  CollisionResult              m_result   [MAX_COLLISION_RESULT_CNT];
  CollisionResult              m_resultOld[MAX_COLLISION_RESULT_CNT];
  void*                        m_userData;
  s32*                         m_pCookingData;
};