//=================================================================================================
/**
*  @file   FieldEffectShadow.cpp
*  @brief  影の定義
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================

#if !defined __FIELD_EFFECT_SHADOW_H_INCLUDED__
#define __FIELD_EFFECT_SHADOW_H_INCLUDED__
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
 *  @class  EffectShadow
 *  @brief  影
 */
class EffectShadow : public IEffectBase
{
  GFL_FORBID_COPY_AND_ASSIGN(EffectShadow);

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
  EffectShadow( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectShadow( void );

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
   *  @brief  エフェクト位置の設定
   */ 
  inline void setSRT( u32 index , f32 scale , gfl2::math::Quaternion qua , gfl2::math::Vector3 pos )
  {
    if( m_pModelInstanceNode->GetJointNum() > index )
    {
      m_pModelInstanceNode->GetJointInstanceNode(index)->SetLocalTranslate( pos );
      m_pModelInstanceNode->GetJointInstanceNode(index)->SetLocalRotation( qua );
      m_pModelInstanceNode->GetJointInstanceNode(index)->SetLocalScale( gfl2::math::Vector3( scale,scale,scale ) );
    }
  }

  /**
   *  @brief  エフェクト位置の設定
   */ 
  inline void setSRT( f32 scale , gfl2::math::Quaternion qua , gfl2::math::Vector3 pos )
  {
    for( u32 i=1 ; i<m_pModelInstanceNode->GetJointNum() ; i++ )
    {
      m_pModelInstanceNode->GetJointInstanceNode(i)->SetLocalTranslate( pos );
      m_pModelInstanceNode->GetJointInstanceNode(i)->SetLocalRotation( qua );
      m_pModelInstanceNode->GetJointInstanceNode(i)->SetLocalScale( gfl2::math::Vector3( scale,scale,scale ) );
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
  gfl2::math::Vector4                                              m_defaultConstantColor;
};

GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_EFFECT_SHADOW_H_INCLUDED__
