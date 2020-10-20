//======================================================================
/**
 * @file FieldTrialModelFactory.h
 * @date 2015/09/08 13:07:01
 * @author miyachi_soichi
 * @brief TrialModel生成
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_TRIALMODEL_FACTORY_H_INCLUDED__
#define __FIELD_TRIALMODEL_FACTORY_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// GameSys
#include "GameSys/include/TimeZone.h"
// poketool
#include "PokeTool/include/PokeModelSystem.h"
#include "PokeTool/include/PokeTool.h"
#include "PokeTool/include/PokeModel.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)
class BaseCollisionScene;
GFL_NAMESPACE_BEGIN(Field)
class Fieldmap;
class MyRenderingPipeLine;
GFL_NAMESPACE_BEGIN(Camera)
class CameraManager;
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_BEGIN(Effect)
class EffectManager;
class EffectShadow;
GFL_NAMESPACE_END(Effect)
GFL_NAMESPACE_BEGIN(TrialModel)
class FieldTrialModel;
class FinderModel;
struct FinderModelData;

//! 影タイプ定義
enum ShadowType
{
  SHADOW_CIRCLE,
  SHADOW_LONG,
  SHADOW_WIDE,
  SHADOW_MAXI,
};
//! 影データ
struct ShadowData
{
  u32 monsNo;
  u32 form;
  u32 type;
  f32 scale;
  f32 offset_x;
  f32 offset_z;
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止
struct ShadowDataHeader
{
  u32 num;
  ShadowData data[];
};
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

/**
 * @class TrialModelFactory
 * @brief クラスの説明
 */
class TrialModelFactory
{
  GFL_FORBID_COPY_AND_ASSIGN(TrialModelFactory);

public:
  /**
   *  Factory初期化構造体
   */
  struct SetupParam
  {
    Field::Fieldmap                                           *pFieldmap;
    Field::MyRenderingPipeLine                                *pPipeLine;
    Field::Effect::EffectManager                              *pEffectManager;
    BaseCollisionScene                                        *pColScene;
    Field::Camera::CameraManager                              *pCameraManager;
    void                                                      *pTrialShadow;
    void                                                      *pFinderShadow;
  };
  /**
   *  FinderModel初期化構造体
   */
  struct FinderModelSetupParam
  {
    gfl2::heap::HeapBase                    *pHeap;   //!< ヒープ
    const PokeTool::SimpleParam             *param;   //!< パラメータ
    const PokeTool::PokeModel::SetupOption  *option;  //!< セットアップオプション
    const FinderModelData                   *model;   //!< モデルデータ
  };
  
  //! ポケモンライティング用ライトセット番号
  static const u32 LIGHT_SET_NO = 30;

public:
  /**
   *  @brief  コンストラクタ
   */
  TrialModelFactory();

  /**
   *  @brief  デストラクタ
   */
  ~TrialModelFactory( void );

  /**
   *  @brief  Setup
   */
  void Setup( const SetupParam &prm );

  /**
   *  @brief  PokeModelSystem初期化
   *  @param  pSysHeap    システム
   *  @param  pDevHeap    デバイス
   *  @param  poke_num    ポケモン登場数
   */
  void InitializeSystem( gfl2::heap::HeapBase *pSysHeap, gfl2::heap::HeapBase *pDevHeap, s32 poke_num, bool create_shadow = true );

  /**
   *  @brief  PokeModelSystem初期化待ち
   */
  bool IsInitialize( void );

  /**
   *  @brief  さくせい
   *  @param  pDevHeap    デバイス
   */
  void CreateSystem( gfl2::heap::HeapBase *pDevHeap, gfl2::gfx::IGLAllocator *pAllocator = NULL, PokeTool::PokeModelSystem::HeapOption* pHeapOption = NULL, bool create_heap_array = true );

  /**
   *  @brief  破棄
   */
  void DeleteSystem();

  /**
   *  @brief  破棄待ち
   */
  bool IsDelete();

  /**
   *  @brief  破棄
   */
  void FinalizeSystem();

  /**
   *  @brief  更新
   */
  void Update( void );

  /**
   *  @brief  更新
   *  @note   アニメーション更新前に行う
   */
  void UpdateBeforeAnimation( void );

  /**
   *  @brief  更新
   *  @note   アニメーション更新後に行う
   */
  void UpdateAfterAnimation( void );

  /**
   *  @brief  更新：トラバース後の処理
   */
  void UpdateAfterTraverse( void );

  /**
   *  @brief  TrialModelの生成
   */
  FieldTrialModel *CreateTrialModel( gfl2::heap::HeapBase *pHeap, const PokeTool::SimpleParam *param, const PokeTool::PokeModel::SetupOption &opt );

  /**
   *  @brief  TrialModelの破棄
   */
  void DeleteTrialModel( FieldTrialModel *mdl );

  /**
   *  @brief  FinderModelの生成
   */
  FinderModel *CreateFinderModel( const FinderModelSetupParam &prm );

  /**
   *  @brief  FinderModelの破棄
   */
  void DeleteFinderModel( FinderModel *mld );

  /**
   *  @brief  ポケモデルシステムの取得
   */
  PokeTool::PokeModelSystem * GetPokeModelSystem( void ) { return m_pPokeModelSystem; }

  //! ライト方向の更新リクエスト
  inline void RequestDirectionalLight( void ){ m_DirectionalUpdate = true; }
  inline void RequestDirectionalLight( f32 x, f32 y, f32 z )
  {
    m_DirectionalUpdate = true;
    m_fTrial_x = x;
    m_fTrial_y = y;
    m_fTrial_z = z;
  }
  //! ファインダーモードの設定
  inline void SetFinderMode( bool flag ){ m_IsFinderMode = flag; }

  // 環境光の上書き
  void SetOverWriteAmbientColor(const gfl2::math::Vector4& rColor)
  {
    m_IsOverWriteAmbientColor = true;
    m_OverWriteAmbientColor = rColor;
  }
  void ClearOverWriteAmbientColor( void )
  {
    m_IsOverWriteAmbientColor = false;
  }

private:
  /**
   *  @brief  影エフェクトの更新
   */
  void updateShadow( void );

  /**
   *  @brief  空きを取得
   */
  s32 getEmptyIndex( void );

  /**
   *  @brief  ライティング更新
   */
  void updateLight( void );
  //! 試練用ライティング更新
  void updateLight_TrialMode( void );
  //! ファインダー用ライティング更新
  void updateLight_FinderMode( void );


private:
  PokeTool::PokeModelSystem       *m_pPokeModelSystem;
  Field::Fieldmap                 *m_pFieldmap;
  Field::MyRenderingPipeLine      *m_pRenderingPipeLine;
  Field::Effect::EffectManager    *m_pEffectManager;              //!< 影生成
  Field::Effect::EffectShadow     *m_pEffectShadow[SHADOW_MAXI];  //!< 影エフェクト
  BaseCollisionScene              *m_pCollisionScene;             //!< 影座標確認用
  ShadowDataHeader                *m_pTrialShadowData;            //!< 汎用
  ShadowDataHeader                *m_pFinderShadowData;           //!< ファインダー専用
  System::nijiAllocator           *m_pAllocator;

  // ライティング用
  Field::Camera::CameraManager                              *m_pCameraManager;
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode  *m_pDrawEnvNode;

  u32                             m_nPokeModelNum;
  FieldTrialModel                 **m_ppModelArray;       //!< モデル管理配列
  bool                            m_IsCreatePokeHeap;
  bool                            m_IsOutside;            //!< 屋内外判定

  f32                             m_fValDegPoke;
  f32                             m_fHolDegPoke;
  f32                             m_fTrial_x;
  f32                             m_fTrial_y;
  f32                             m_fTrial_z;
  bool                            m_DirectionalUpdate;    //!< 並行光源の更新フラグ
  bool                            m_IsFinderMode;
  
  bool                            m_IsOverWriteAmbientColor;  //!< 環境光設定をデフォルトから書き換えるか
  gfl2::math::Vector4             m_OverWriteAmbientColor;      //!< 環境光上書き設定
};

GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_TRIALMODEL_FACTORY_H_INCLUDED__
