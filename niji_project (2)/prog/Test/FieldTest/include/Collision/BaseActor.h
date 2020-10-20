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

namespace gfl2{ namespace collision { 
  class CollisionModel;
}}

namespace fld { namespace mmodel {
  class MoveModel;
}}

//----------------------------------------------------------------------------------------
//! @brief アクターベース
//----------------------------------------------------------------------------------------
class BaseActor : public gfl2::renderingengine::scenegraph::NodeTrait<BaseActor, gfl2::renderingengine::scenegraph::instance::InstanceNode>
{
public:

  enum SHAPE
  {
    NONE,
    SPHERE_2D,
    SPHERE_3D,
    BOX_2D,
    MESH_3D,
  };

  struct CollisionResult
  {
    CollisionResult()
    {
      init();
    }

    void init(void)
    {
      closestPt     = gfl2::math::Vector3(0.0f,0.0f,0.0f);
      adjustVec     = gfl2::math::Vector3(0.0f,0.0f,0.0f);
      pTargetActor  = NULL;
    }

    gfl2::math::Vector3 closestPt;     //< 最近接点
    gfl2::math::Vector3 adjustVec;     //< 補正ベクトル
    BaseActor*          pTargetActor;  //< ぶつかったアクター
  };

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
  virtual void Clear(void);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アロケータの設定
  *
  *  @param  使用するアロケータ
  *  @note   BaseCollisionScene::createStaticActor/BaseCollisionScene::createDynamicActorの内部で呼ばれるので、アプリ側で呼び出しは不要です
  */
  //-----------------------------------------------------------------------------
  void setAllocator(gfl2::gfx::IGLAllocator* pAllocator){  m_pAllocator = pAllocator; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  シェイプタイプを取得
  *
  *  @retval シェイプタイプ
  */
  //-----------------------------------------------------------------------------
  SHAPE getShapeType(void){ return m_shape; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  シェイプオブジェクトを取得
  *
  *  @retval シェイプオブジェクト
  */
  //-----------------------------------------------------------------------------
  BaseShape* getShape(void){ return m_pShape; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ポジションを設定
  *
  *  @param  ポジション
  */
  //-----------------------------------------------------------------------------
  void setPosition(gfl2::math::Vector3& position){ m_position = position; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  ポジションを取得
  *
  *  @retval ポジション
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Vector3 getPosition(void){ return m_position; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  姿勢を設定
  *
  *  @param  姿勢
  */
  //-----------------------------------------------------------------------------
  void setQuaternion(gfl2::math::Quaternion& quaternion){ m_quaternion = quaternion; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  姿勢を取得
  *
  *  @retval 姿勢
  */
  //-----------------------------------------------------------------------------
  gfl2::math::Quaternion getQuaternion(void){ return m_quaternion; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  衝突の結果を取得
  *
  *  @retval 衝突結果
  */
  //-----------------------------------------------------------------------------
  CollisionResult* getCollisionResult(void){ return &m_result; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにスフィア2Dを追加
  *
  *  @param  pos        スフィアの中心位置
  *  @param  r          スフィアの半径
  */
  //-----------------------------------------------------------------------------
  void createSphere2D( const gfl2::math::Vector3& pos,
                       f32                        r );

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにスフィア3Dを追加
  *
  *  @param  pos        スフィアの中心位置
  *  @param  r          スフィアの半径
  */
  //-----------------------------------------------------------------------------
  void createSphere3D( const gfl2::math::Vector3& pos,
                       f32                        r );

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにBox2Dを追加
  *
  *  @param  pos        長方形の中心点
  *  @param  u0         長方形のローカルなX軸を表す単位ベクトル
  *  @param  u1         長方形のローカルなY軸を表す単位ベクトル
  *  @param  e0         X軸にそった長方形の長さの半分
  *  @param  e1         Y軸にそった長方形の長さの半分
  */
  //-----------------------------------------------------------------------------
  void createBox2D( const gfl2::math::Vector3& pos,
                    const gfl2::math::Vector3& u0,
                    const gfl2::math::Vector3& u1,
                    const f32                  e0,
                    const f32                  e1);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクターにメッシュを追加
  *
  *  @param  pos              メッシュの中心点
  *  @param  pCollisionModel  メッシュ
  */
  //-----------------------------------------------------------------------------
  void createMesh3D( const gfl2::math::Vector3&       pos,
                     gfl2::collision::CollisionModel *pCollisionModel );

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void);

private:

  // 共通設定
  gfl2::gfx::IGLAllocator     *m_pAllocator;
  BaseShape                   *m_pShape;
  SHAPE                        m_shape;
  gfl2::math::Vector3          m_position;
  gfl2::math::Quaternion       m_quaternion;
  CollisionResult              m_result;

};