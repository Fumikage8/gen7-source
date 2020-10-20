/**
* @brief  フィールドカメラの操作、カメラエリア用
* @file   FieldCameraControllerTypeArea.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERACONTROLLERERTYPEAREA_H_INCLUDED__ )
#define __FIELDCAMERACONTROLLERERTYPEAREA_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Easing.h>
#include <Collision/include/gfl2_CollisionModel.h>
#include <math/include/gfl2_SpringSimulation.h>

// field
#include "Field/FieldRo/include/Camera/FieldCameraTypes.h"
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"


GFL_NAMESPACE_BEGIN( Field );

class Fieldmap;
class PlacementDataManager;

GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );
class AreaDataManager;
class AreaCollisionManager;
class Support;
class Focus;
class ScrollStopManager;
struct CAMERA_AREA_SETTING_DATA;
struct AreaData;
GFL_NAMESPACE_BEGIN( SubController );
class SubControllerBase;
GFL_NAMESPACE_END( SubController );
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

class ControllerTypeArea;
class ControllerTypeAreaCallBack
{
public:
  ControllerTypeAreaCallBack(){}

  /**
    *  @brief  コールバックわりあて直後
    */
  virtual void SetAfter(Camera::Controller::ControllerTypeArea* pThis){}

  /**
    *  @brief  エリア変更直後
    */
  virtual void AreaChangeAfter(ControllerTypeArea* pThis, u32 areaDataNo){}

  /**
  *  @brief  内部補間前処理
  */
  virtual void InSideInterpolationPrev(ControllerTypeArea* pThis, gfl2::math::Vector3* pCamera, gfl2::math::Vector3* pTarget ){}

};

/**
* @brief フィールドカメラの操作、カメラエリア用
*/
class ControllerTypeArea : public ControllerBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( ControllerTypeArea );

public:



  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    const Fieldmap*                   m_pFieldMap;              // for 現在ゾーン
    const Area::AreaDataManager*      m_pAreaDataManager;
    const Area::AreaCollisionManager* m_pAreaCollisionManager;
    const Target::TargetBase*         m_pTarget;                // 注視対象
    gfl2::heap::HeapBase*             m_pSystemHeap;
    const PlacementDataManager*       m_pPlacementDataManager;
    const Area::ScrollStopManager*    m_pScrollStopManager;
  };

#if PM_DEBUG
  struct DEBUG_SETTING
  {
    b32 invalidShake;     // カメラシェイク無効化
  };
#endif // #if PM_DEBUG

  struct SETTING
  {
    gfl2::math::Easing::EaseFunc areaIpoEaseFunc;   // カメラエリア変更の遷移で使用するイージング関数
  };


  /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  ControllerTypeArea( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~ControllerTypeArea( void );
  

  /**
  * @brief 初期化処理
  * @note  ControllerがCameraUnitの設定された直後に実行される
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void SetUp( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 更新処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void Update( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 再開処理
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void Resume( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief 補間を打ち切り、最新の状態に更新する
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  virtual void Fix( poke_3d::model::BaseCamera* pCamera );

  /**
  * @brief フィールド操作開始時に実行する処理
  *
  * @return 無し
  */
  void Ready( void );

  /**
  * @brief  補間指定(外部指定)
  *
  * @param  rTargetPosition カメラターゲット座標(始点)
  * @param  rCameraPosition カメラ座標(始点)
  * @param  frame           補間にかけるフレーム
  *
  * @return 成否(コントローラーの初期化が終わっていない場合は失敗する)
  */
  b32 SetInterpolation( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rCameraPosition, u32 frame, gfl2::math::Easing::EaseFunc easeFunc );

  /**
  * @brief  横バネの取得
  *
  * @return バネのアドレス
  */
  gfl2::math::SpringSimulation* GetHorizonSpring(void){ return &m_horizonSpring; }

  /**
  * @brief カメラシェイクリクエスト(水平方向)
  *
  * @param  mass          質量
  * @param  k             バネ定数
  * @param  firstVelocity 初速度
  * @param  damping       減衰率
  *
  * @return リクエスト受理の成否
  */
  virtual b32 ShakeHorizonRequest(f32 mass,f32 k,f32 firstVelocity,f32 damping){ m_horizonSpring.Request(mass,k,firstVelocity,damping); return true; }

  /**
  * @brief カメラシェイクリクエスト(垂直方向)
  *
  * @param  mass          質量
  * @param  k             バネ定数
  * @param  firstVelocity 初速度
  * @param  damping       減衰率
  *
  * @return リクエスト受理の成否
  */
  virtual b32 ShakeVerticalRequest(f32 mass,f32 k,f32 firstVelocity,f32 damping ){ m_verticalSpring.Request(mass,k,firstVelocity,damping); return true; }

  /**
  * @brief カメラサポート挙動の制御アクションの実行
  *
  * @param  value     アクション種別
  *
  * @return 成否
  */
  b32 SetSupportAction( SupportAction::Value value );

  /**
  * @brief フィールドカメラフォーカスモジュールを取得
  *
  * @return フォーカスモジュール( 初期化前はNULLの可能性があります )
  */
  const Area::Focus* GetFocus( void ) const
  {
    return m_pFocus;
  }
  Area::Focus* GetFocus( void )
  {
    return m_pFocus;
  }

  /**
  * @brief フィールドカメラサポートモジュールを取得
  *
  * @return サポートモジュール( 初期化前はNULLの可能性があります )
  */
  const Area::Support* GetSupport( void ) const
  {
    return m_pSupport;
  }

  /**
  * @brief 現在適用されているエリアデータNoを取得
  *
  * @return エリアデータNo
  */
  u32 GetNowAreaDataNo( void ) const { return m_NowAreaDataNo; }  

  /**
  * @brief コールバッククラスを設定(メモリ管理は設定者が行う)
  */
  void SetCallBack(ControllerTypeAreaCallBack* pCallBack)
  {
    m_pCallBack = pCallBack;
    if (m_pCallBack)
    {
      m_pCallBack->SetAfter( this );
    }
  }

#if PM_DEBUG
  void DebugModeStart( void );
  void DebugModeEnd( b32 resetFlag = false );
  void DebugModeReset( void );



  // デバッグメニュー向け
  static SETTING*       GetSettingPtr( void ) { return &m_Setting;}
  static DEBUG_SETTING* GetDebugSettingPtr( void ) { return &m_DebugSetting; }
#endif

private:

  /**
  * @brief 所属するカメラエリアを更新し、SubControllerの切り替えを行う処理
  * @note  カメラエリア制御座標が前回から変わっていない場合はカメラエリアの更新はスキップされます
  *
  * @param force true カメラエリア制御座標が前回から変わっていなくてもカメラエリアの更新を行う
  *
  * @return 無し
  */
  void UpdateCameraArea( b32 force = false );

  /**
  * @brief ゲームデータから直前のカメラNoを復帰する
  *
  * @param nowAreaDataNo  復帰地点のエリアデータNo
  * @param pOldAreaDataNo 直前のカメラNo格納先
  *
  * @return true 復帰した false 復帰条件を満たしておらず、復帰しなかった
  */
  b32 RecoveryCameraArea( u32 nowAreaDataNo, u32* pOldAreaDataNo );

  /**
  * @brief SubControllerエリアチェンジ処理
  *
  * @param rData エリアデータ
  * @param force true 補間を行わない
  *
  * @return 無し
  */
  void ChangeSubController( const Area::AreaData& rData, b32 force = false );



  //
  // 内部制御
  //
  static SETTING                  m_Setting;
  u32                             m_NowAreaDataNo;          // 現在のエリアデータ番号
  u32                             m_OldAreaDataNo;          // 前回のエリアデータ番号
  f32                             m_NowRatio;
  gfl2::math::Vector3             m_PrevAreaCheckPosition;  // 前回エリアデータ取得に使用したターゲット座標 
  Area::AreaData*                 m_pNowAreaData;
  gfl2::math::Vector3             m_PrevTargetObjectPosition;
  gfl2::math::Vector3             m_PrevCameraTargetPosition;
  gfl2::math::Vector3             m_PrevCameraPosition;


#if PM_DEBUG
  b32                             m_DebugSupportSettingOverRideFlag;
  b32                             m_IsDebugController;
  b32                             m_IsDebugOpen;
  b32                             m_IsDebugDraw;

  static DEBUG_SETTING            m_DebugSetting;
#endif // #if PM_DEBUG

  //
  // 内部オブジェクト
  //
  Area::SubController::SubControllerBase* m_pSubController;               // 子制御
  gfl2::heap::HeapBase*                   m_pLocalHeapForSubController;   // サブコントローラー専用ヒープ
  Area::Interpolation                     m_InSideInterpolation;          // Area内部での補間
  Area::Interpolation                     m_OutSideInterpolation;         // Area外部からの補間
  gfl2::math::SpringSimulation            m_horizonSpring;                // 横バネ
  gfl2::math::SpringSimulation            m_verticalSpring;               // 縦バネ

  Area::Support*                          m_pSupport;                     // サポート挙動制御
  Area::Focus*                            m_pFocus;                       // フォーカス挙動制御

  //
  // 外部オブジェクト
  //
  poke_3d::model::BaseCamera*       m_pBaseCamera;            // カメラ実体
  const Fieldmap*                   m_pFieldMap;              // for 現在ゾーン
  const Area::AreaDataManager*      m_pAreaDataManager;       // エリア設定データ
  const Area::AreaCollisionManager* m_pAreaCollisionManager;  // エリア設定のコリジョンデータ
  const Target::TargetBase*         m_pTarget;                // 注視対象
  const Area::ScrollStopManager*    m_pScrollStopManager;
  ControllerTypeAreaCallBack*       m_pCallBack;

};  // class FieldControllerTypeArea


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEAREA_H_INCLUDED__

  