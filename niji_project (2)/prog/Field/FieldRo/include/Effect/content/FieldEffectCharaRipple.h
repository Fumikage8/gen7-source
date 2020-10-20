//======================================================================
/**
 * @file FieldEffectCharaRipple.h
 * @date 2016/01/20 17:50:07
 * @author miyachi_soichi
 * @brief エフェクト：キャラクタ波紋
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_CHARARIPPLE_H_INCLUDED__
#define __FIELD_EFFECT_CHARARIPPLE_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(Field)
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_BEGIN(Effect)

/**
 *  @class  EffectCharaRipple
 *  @brief  波紋
 */
class EffectCharaRipple : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectCharaRipple);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
    gfl2::heap::HeapBase        *pHeap;
    FieldRootNode               *pRootNode;
    Field::MyRenderingPipeLine  *pPipeLine;
    Field::IField3DObjectNode   *pParentObject;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectCharaRipple( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectCharaRipple( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief  停止
   */
  void Suspend( void );

  /**
   *  @brief  再開
   */
  void Resume( void );

protected:
  /**
   *  @brief  モデルの生成
   *  @param  pModelBinary    モデルリソース
   */
  void createModel( void *pModelBinary );

  /**
   *  @brief  モーションの作成
   *  @param  pMotionBinary   モーションリソース
   */
  void createMotion( void *pMotionBinary );

  /**
   *  @brief  ジョイント設定
   */
  void setJointNode( Field::IField3DObjectNode *pParent );

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );

  /**
   *  @brief  ジョイント座標の取得
   */
  gfl2::math::Vector3 getJointPosition( void );

  /**
   *  @brief  描画モードの切り替え
   */
  void changeMode( u32 mode );

private:
  enum
  {
    MODE_NONE,
    MODE_WAIT,
    MODE_MOVE,
  };
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;
  poke_3d::model::BaseModel                                       *m_pParent;
  poke_3d::model::BaseModel                                       *m_pModel;
  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode  *m_pJointNode;
  gfl2::math::Vector3                                             m_vPosition;
  u32                                                             m_eMode;
  bool                                                            m_bPause;

}; // class EffectDashSmoke : public IEffectBase

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_CHARARIPPLE_H_INCLUDED__
