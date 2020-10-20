//======================================================================
/**
 * @file FieldEffectPokeShadow.h
 * @date 2015/09/09 16:20:44
 * @author miyachi_soichi
 * @brief エフェクト：バトルモデル用影
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_POKESHADOW_H_INCLUDED__
#define __FIELD_EFFECT_POKESHADOW_H_INCLUDED__
#pragma once

// poke_3d
#include <model/include/gfl2_BaseModel.h>
// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"

// 前方宣言
class BaseCollisionScene;
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class GLHeapAllocator;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN(Field)
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_BEGIN(TrialModel)
class FieldTrialModel;
GFL_NAMESPACE_END(TrialModel)

GFL_NAMESPACE_BEGIN(Effect)

/**
 * @class EffectPokeShadow
 * @brief エフェクト：バトルモデル用影
 */
class EffectPokeShadow : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectPokeShadow);
  
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
    TrialModel::FieldTrialModel *pTrialModel;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectPokeShadow( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectPokeShadow( void );

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
  
  void Joint( void );
  void SetCollisionScene( BaseCollisionScene *pScene ){ m_pCollisionScene = pScene; }

  /**
   *  @brief  描画設定
   */
  void SetVisible( bool flag );

protected:
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
   *  @brief  座標更新
   */
  void updatePosition( void );

protected:
  poke_3d::model::BaseModel                                       *m_pModel;
  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  Field::TrialModel::FieldTrialModel                              *m_pParent;

  gfl2::renderingengine::scenegraph::instance::JointInstanceNode  *m_pJointNode;
  BaseCollisionScene                                              *m_pCollisionScene;
  bool                                                            m_bRegist;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_POKESHADOW_H_INCLUDED__
