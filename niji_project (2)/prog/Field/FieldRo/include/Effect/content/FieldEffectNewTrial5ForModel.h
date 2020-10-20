//======================================================================
/**
 * @file FieldEffectNewTrial5ForModel.h
 * @date 2016/01/12 11:31:18
 * @author kawazoe_shinichi
 * @brief 電気試練用エフェクト for モデルタイプ
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_NEW_TRIAL_5_FOR_MODEL_H_INCLUDED__
#define __FIELD_EFFECT_NEW_TRIAL_5_FOR_MODEL_H_INCLUDED__
#pragma once

// gfl2
#include <Effect/include/gfl2_EffectHeap.h>
#include <Effect/include/gfl2_EffectSystem.h>
// Effect
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
// poke_3d
#include <model/include/gfl2_BaseModel.h>

// 前方宣言
GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)
class GLHeapAllocator;
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(Field)
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_BEGIN(Effect)

/**
*  @class  FieldEffectNewTrial5ForModel
*  @brief  電気試練用エフェクト for モデルタイプ
*/
class EffectNewTrial5ForModel : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectNewTrial5ForModel);

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
    gfl2::math::Vector3         position;
  };

public:
  /**
  *  @brief  コンストラクタ
  */
  EffectNewTrial5ForModel(void);

  /**
  *  @brief  デストラクタ
  */
  virtual ~EffectNewTrial5ForModel(void);

  /**
  *  @brief  初期化処理
  */
  void Initialize(SetupData &setupData);

  /**
  *  @brief  破棄
  */
  virtual bool Terminate(void);

  /**
  *  @brief  各エフェクトで必要な破棄作業
  */
  virtual void Delete(void);

  /**
  *  @brief  エフェクトを再生する
  */
  void CreateEffect(s32 idx);


  /**
  *  @brief  再生確認
  *  @retval true  再生中
  *  @retval false そのほか
  */
  bool IsAlive(void);
  virtual bool IsAnimationLastFrame(void){ return !IsAlive(); }

protected:
  /**
  *  @brief  更新処理
  */
  virtual void updateNormal(void);

  /**
  *  @brief  モデルの作成
  *  @param  pModelBinary  モデルバイナリ
  */
  void createModel(void *pModelBinary);

  /**
  *  @brief  モーションの作成
  *  @param  pMotionBinary モーションバイナリ
  */
  void createMotion(void *pMotionBinary);

private:
  poke_3d::model::BaseModel                                       *m_pModel;
  gfl2::heap::HeapBase                                            *m_pHeap;
  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  void                                                            *m_pResource;

  gfl2::math::Vector3                                             m_vPosition;
  gfl2::math::Vector3                                             m_vScale;



};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_NEW_TRIAL_5_FOR_MODEL_H_INCLUDED__
