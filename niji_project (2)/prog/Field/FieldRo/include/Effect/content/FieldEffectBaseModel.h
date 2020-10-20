//======================================================================
/**
 * @file FieldEffectBaseModel.h
 * @date 2016/03/02 12:58:30
 * @author miyachi_soichi
 * @brief エフェクト：モデル単体呼出
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_EFFECT_BASEMODEL_H_INCLUDED__
#define __FIELD_EFFECT_BASEMODEL_H_INCLUDED__
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
GFL_NAMESPACE_BEGIN( Camera )
class CameraManager;
GFL_NAMESPACE_END( Camera )
GFL_NAMESPACE_END( Field )

GFL_NAMESPACE_BEGIN( Field )
class MyRenderingPipeLine;
class FieldRootNode;

GFL_NAMESPACE_BEGIN( Effect )

class EffectBaseModel : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectBaseModel );

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
    Camera::CameraManager       *pCameraManager;
    gfl2::math::Vector3         position;
    gfl2::math::Vector3         scale;
    u32                         nModelID;
    u32                         nMotionID;
    bool                        bSuicide;       //!< 自殺指定
    bool                        bBillboard;     //!< ビルボード設定
    bool                        bScreen;
    bool                        bLoop;

    SetupData(void)
      : pCameraManager( NULL )
      , scale( 1.0f, 1.0f, 1.0f )
      , bSuicide( false )
      , bBillboard( false )
      , bScreen( false )
      , bLoop( false )
    {
    }
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectBaseModel( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectBaseModel( void );

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
   *  @brief  終了しているか.
   */
  virtual bool IsAnimationLastFrame( void );

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
   *  @brief  モーションの作成
   *  @param  pMotionBinary モーションバイナリ
   */
  void createMotion( void *pMotionBinary );

  /**
   *  @brief  スクリーン更新
   */
  void updatePosition_Screen( void );

protected:
  poke_3d::model::BaseModel                                       *m_pModel;
  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用
  Camera::CameraManager                                           *m_pCameraManager;
  gfl2::math::Vector3                                             m_vPosition;
  gfl2::math::Vector3                                             m_vScale;

  bool                                                            m_bSuicide;
  bool                                                            m_bBillboard;
  bool                                                            m_bScreen;
  bool                                                            m_bLoop;
};

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_EFFECT_BASEMODEL_H_INCLUDED__
