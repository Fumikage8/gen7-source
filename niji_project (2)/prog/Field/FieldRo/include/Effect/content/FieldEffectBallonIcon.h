/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectBallonIcon.h
 *  @brief  フィールドエフェクト：吹き出し基礎
 *  @author miyachi_soichi
 *  @date   2015.06.09
 */

#if !defined(__FIELD_EFFECT_BALLONICON_H__)
#define __FIELD_EFFECT_BALLONICON_H__
#pragma once

// poke_3d
#include <model/include/gfl2_BaseModel.h>
// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class GLHeapAllocator;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( Field )
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_BEGIN( Camera )
class CameraManager;
GFL_NAMESPACE_END( Camera )

GFL_NAMESPACE_BEGIN( Effect )

class EffectBallonIcon : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectBallonIcon );

public:
  /**
   *  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::heap::HeapBase        *pHeap;
    FieldRootNode               *pRootNode;
    Field::MyRenderingPipeLine  *pPipeLine;
    Field::IField3DObjectNode   *pParentObject;
    Camera::CameraManager       *pCameraManager;
    gfl2::math::Vector3         vPosition;
    bool                        bPosMode;

    //! ctor
    SetupData()
      : vPosition()
      , bPosMode( false )
    {
    }
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectBallonIcon( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectBallonIcon( void );

  /**
   *  @brief  初期化処理：座標指定
   *  @param  setupData 初期化構造体
   */
  virtual void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄処理
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );
  
  /**
   *  @brief  アニメーション再生終了を検知
   *  @return アニメーション終了
   */
  virtual bool IsAnimationLastFrame( void );

  /**
   *  @brief  アニメーション再生速度を設定
   *
   *  @param  stepFrame 再生速度
   */
  virtual void SetAnimationStepFrame( f32 stepFrame );

  /**
   *  @brief  拡大率を指定
   *
   *  @param  拡大率
   */
  virtual void SetScale( f32 scale );

  /**
   *  @brief  親となるジョイントを設定(頭以外につけたい時に使用)
   *
   *  @param  pJointNode ジョイント(ポインタを保持する)
   */
  virtual void SetParentJoint( gfl2::renderingengine::scenegraph::instance::JointInstanceNode* pJointNode );

  /**
  *  @brief  表示オフセットを設定
  *
  *  @param  rOffset　表示オフセット
  */
  virtual void SetOffest( const gfl2::math::Vector3& rOffset );

  // リソースID取得
  virtual u32 GetModelDataID( void ) = 0;
  virtual u32 GetMotionDataID( void ) = 0;

protected:
  /**
   *  @brief  初期化
   */
  void setupBallon( SetupData &setupData );

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  モデルの作成
   *  @param  pModelBinary  モデルバイナリ
   */
  void createModel( void *pModelBinary );

  /**
   *  @brief  モーションの作成
   *  @param  pMotionBinary モーションバイナリ
   */
  void createMotion( void *pMotionBinary );

  /**
   *  @brief  親にジョイント
   */
  void jointParent( Field::IField3DObjectNode *pParent );

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );

protected:
  poke_3d::model::BaseModel                                       m_Model;

  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode  *m_pJointNode;
  Camera::CameraManager                                           *m_pCameraManager;

  u32                                                             m_nSeID;
  gfl2::math::Vector3                                             m_Offset;

  // 以下座標モードでの使用
  gfl2::math::Vector3                                             m_vPosition;
  bool                                                            m_bPosMode;
}; // class EffectBallonIcon : public IEffectBase

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // !defined(__FIELD_EFFECT_BALLONICON_H__)
