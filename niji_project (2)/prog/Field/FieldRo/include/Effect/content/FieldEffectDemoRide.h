//======================================================================
/**
 * @file FieldEffectDemoRide.h
 * @date 2016/02/03 14:53:05
 * @author miyachi_soichi
 * @brief 簡単な説明
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_DEMORIDE_H_INCLUDED__
#define __FIELD_EFFECT_DEMORIDE_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"
// model
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel.h"

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Effect)

class EffectDemoRide : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectDemoRide);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::Effect::GFHeap        *pEffectHeap;
    gfl2::Effect::System        *pEffectSystem;
    Field::IField3DObjectNode   *pParentObject;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectDemoRide( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectDemoRide( void );

  /**
   *  @brief  初期化処理
   */
  void Initialize( SetupData &setupData );

  /**
   *  @brief  破棄
   */
  virtual bool Terminate( void );

  /**
   *  @brief  終了しているか.
   */
  virtual bool IsAnimationLastFrame( void );

protected:
  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  ジョイント設定
   */
  void setJointNode( Field::IField3DObjectNode *pParent );

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );

  /**
   *  @brief  ジョイント座標の取得
   */
  gfl2::math::Vector3 getJointPosition( void );

private:
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  gfl2::Effect::Handle                                            *m_pEffectHandle;
  poke_3d::model::BaseModel                                       *m_pParent;
  gfl2::renderingengine::scenegraph::instance::JointInstanceNode  *m_pJointNode;
  gfl2::math::Vector3                                             m_vPosition;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_DEMORIDE_H_INCLUDED__
