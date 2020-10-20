﻿//======================================================================
/**
 * @file FieldEffectSharkWait.h
 * @date 2015/09/03 11:10:51
 * @author miyachi_soichi
 * @brief サメハダーの待機
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_SHARKWAIT_H_INCLUDED__
#define __FIELD_EFFECT_SHARKWAIT_H_INCLUDED__
#pragma once

// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

GFL_NAMESPACE_BEGIN(Field)
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_BEGIN(Effect)

/**
 * @class EffectSharkWait
 * @brief サメハダーの待機
 */
class EffectSharkWait : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectSharkWait);

public:
  /**
   *  @brief  初期化構造体
   */
  struct SetupData
  {
    void                        *pResource;
    gfl2::heap::HeapBase        *pHeap;
    FieldRootNode               *pRootNode;
    Field::MyRenderingPipeLine  *pPipeLine;
    Field::IField3DObjectNode   *pParentObject;
  };
public:
  /**
   *  @brief  コンストラクタ
   */
  EffectSharkWait( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectSharkWait( void );

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
   *  @brief  描画切り替え
   */
  void SetVisible( bool flag );

  /**
   *  @brief  描画状態取得
   */
  inline bool IsVisible( void ) const { return m_bVisible; }

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
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  座標更新
   */
  void updatePosition( void );

private:
  poke_3d::model::BaseModel                                       *m_pParent;
  poke_3d::model::BaseModel                                       *m_pModel;
  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用

  bool                                                            m_bVisible;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_SHARKWAIT_H_INCLUDED__
