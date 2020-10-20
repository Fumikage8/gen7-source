/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEffectBase.h
 *  @brief  フィールドエフェクト：アクションボタン
 *  @author miyachi_soichi
 *  @date   2015.06.05
 */

#if !defined( __FIELD_EFFECT_ACTIONBUTTON_H__ )
#define __FIELD_EFFECT_ACTIONBUTTON_H__
#pragma once

// gfl2
#include <Animation/include/gfl2_AnimationController.h>
// poke_3d
#include <model/include/gfl2_BaseModel.h>
// layout
#include <AppLib/include/Util/app_util_heap.h>
// Field
#include "Field/FieldRo/include/Effect/FieldEffectBase.h"
#include "Field/FieldRo/include/Effect/FieldEffectLayout.h"

// 3DModel <-> layout
#define FIELDEFFECT_SWITCH_LAYOUT 1

// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( util )
class GLHeapAllocator;
GFL_NAMESPACE_END( util )
GFL_NAMESPACE_END( gfl2 )

GFL_NAMESPACE_BEGIN( Field )
class MyRenderingPipeLine;
class FieldRootNode;
GFL_NAMESPACE_BEGIN(Camera)
class CameraManager;
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_BEGIN(FieldWindow)
class FieldWindowManager;
GFL_NAMESPACE_END(FieldWindow)
GFL_NAMESPACE_BEGIN( Effect )

class EffectActionButton : public IEffectBase
{
  //! コピー禁止
  GFL_FORBID_COPY_AND_ASSIGN( EffectActionButton );

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

    Field::FieldWindow::FieldWindowManager *pWinMgr;

    gfl2::math::Vector3         vPos;
    gfl2::math::Quaternion      quaternion;
  };

public:
  /**
   *  @brief  コンストラクタ
   */
  EffectActionButton( void );

  /**
   *  @brief  デストラクタ
   */
  virtual ~EffectActionButton( void );

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
   *  @brief  更新処理
   */
  virtual void updateNormal( void );

  /**
   *  @brief  各エフェクトで必要な破棄作業
   */
  virtual void Delete( void );

  /**
   *  @brief  座標設定
   */
  void SetPosition( const gfl2::math::Vector3 &vPos );

  /**
   *  @brief  アニメーションフレーム設定：アイコン切り替え
   */
  void SetAnimationFrame( u32 frame );

private:
#if FIELDEFFECT_SWITCH_LAYOUT
#else
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
#endif // FIELDEFFECT_SWITCH_LAYOUT

  /**
   *  @brief  座標変換
   */
  void updatePosition( void );

private:

  Camera::CameraManager                                           *m_pCameraManager;
#if FIELDEFFECT_SWITCH_LAYOUT
  // layoutで必要なもの
  app::util::Heap                                                 m_appUtilHeap;
  Field::Effect::EffectLayout                                     *m_pLayout;
  Field::FieldWindow::FieldWindowManager                          *m_pWindowManager;
#else
  // 3DModelで必要なもの
  poke_3d::model::BaseModel                                       m_Model;

  gfl2::util::GLHeapAllocator                                     *m_pAllocator;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pModelResourceRootNode;
  gfl2::renderingengine::scenegraph::resource::ResourceNode       *m_pMotionResourceRootNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode  *m_pModelInstanceNode;
  FieldRootNode                                                   *m_pFieldRootNode;      //!< フィールドルートノード
  Field::MyRenderingPipeLine                                      *m_pRenderingPipeLine;  //!< 描画シーンへの追加削除用


  f32 m_fScale;
#endif
  // 共通で必要なもの
  gfl2::math::Vector3                                             m_vPosition;

}; // class EffectActionButton : public IEffectBase

GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_END( Field )

#endif // !defined( __FIELD_EFFECT_ACTIONBUTTON_H__ )
