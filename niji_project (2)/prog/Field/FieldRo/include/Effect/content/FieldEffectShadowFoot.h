//=================================================================================================
/**
*  @file   FieldEffectShadowFoot.cpp
*  @brief  影(足型)の定義
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================

#if !defined __FIELD_EFFECT_SHADOW_FOOT_H_INCLUDED__
#define __FIELD_EFFECT_SHADOW_FOOT_H_INCLUDED__
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
 *  @class  EffectShadowFoot
 *  @brief  影(足型)
 */
class EffectShadowFoot : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectShadowFoot);

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
  EffectShadowFoot( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectShadowFoot( void );

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
   *  @brief  影切り替え
   */
  void SetVisibleShadow( bool flag );

  /**
   *  @brief  影状態取得
   */
  inline bool IsVisible( void ) const { return m_bVisible; }

  /**
   *  @brief  SRT更新
   */
  inline void setSRT( b32 isLeft , f32 scale , gfl2::math::Quaternion qua , gfl2::math::Vector3 pos , f32 alpha)
  {
    s32 jointIndex    = isLeft ? m_pModel->GetJointIndex("LFootShadow") : m_pModel->GetJointIndex("RFootShadow");
    s32 materialIndex = isLeft ? m_pModel->GetMaterialIndex("shadowL")  : m_pModel->GetMaterialIndex("shadowR");
    if( jointIndex >= 0 && materialIndex >= 0 )
    {
      gfl2::math::Vector4 color( m_pModel->GetMaterialColorConstant(materialIndex,0) );
      m_pModel->SetMaterialColorConstant(materialIndex,0,color.x,color.y,color.z,alpha);
      m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->SetLocalScale( gfl2::math::Vector3(scale,scale,scale) );
      m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->SetLocalRotation( qua );
      m_pModelInstanceNode->GetJointInstanceNode(jointIndex)->SetLocalTranslate( pos );
    }
  }

  /**
   *  @brief  BaseModelの取得
   */
  inline poke_3d::model::BaseModel* GetBaseModel() const { return m_pModel; }

protected:

  /**
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

private:

  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::Effect::GFHeap                                            *m_pEffectHeap;
  gfl2::Effect::System                                            *m_pEffectSystem;
  poke_3d::model::BaseModel                                       *m_pModel;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  bool                                                             m_bVisible;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_SHADOW_FOOT_H_INCLUDED__
