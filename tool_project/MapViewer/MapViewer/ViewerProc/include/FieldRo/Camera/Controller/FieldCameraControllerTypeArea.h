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

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"
#include "ViewerProc/include/FieldRo/Camera/Target/FieldCameraTargetBase.h"



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );
class AreaDataManager;
struct DATA;
class MeshHitData;
GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );



GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Controller );

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
    const Area::AreaDataManager* m_pAreaDataManager;
    const Target::TargetBase*    m_pTarget;             // 注視対象
    // const gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel;  // @todo コリジョン関数がconstだと呼べないので
    gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel;
    gfl2::heap::HeapBase*        m_pSystemHeap;
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


private:

  /**
  * @brief 更新処理( DataType::FULL )
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void UpdateTypeFull( poke_3d::model::BaseCamera* pCamera );


  /**
  * @brief 更新処理( DataType::POINT_INTERPOLATION )
  *
  * @param  pCamera 操作するカメラの実体
  *
  * @return 無し
  */
  void UpdateTypePointInterpolation( poke_3d::model::BaseCamera* pCamera );

#if PM_DEBUG
  /**
  * @brief デバッグ操作による影響を全てクリア
  *
  * @return 無し
  */
  void ClearDebugEffect( void );

#endif // #if PM_DEBUG

  //
  // 内部制御
  //
  u32                             m_InterpolationNowFrame;  // 現在フレーム
  s32                             m_NowAreaDataId;          // 現在のエリアデータ番号
  s32                             m_AreaDataType;           // 現在のエリアデータの型式
  gfl2::math::Vector3             m_PrevAreaCheckPosition;  // 前回エリアデータ取得に使用したターゲット座標 
  gfl2::math::Vector3             m_PrevTargetPosition;     // 前回ターゲット座標 
  Area::DATA*                     m_pData;
  Area::DATA*                     m_pPrevData;
  Area::MeshHitData*              m_pMeshHitData;

  gfl2::math::Vector3             m_InterpolationCameraPosition;
  gfl2::math::Vector3             m_InterpolationTargetPosition;

  gfl2::math::Vector3             m_PrevCameraTargetPosition;
  gfl2::math::Vector3             m_PrevCameraPosition;
  gfl2::math::Vector3             m_CameraPosition;
  gfl2::math::Vector3             m_TargetPosition;

#if PM_DEBUG
  // デバッグ介入
  gfl2::math::Vector3             m_DebugRotation;
  f32                             m_DebugDistance;
#endif // #if PM_DEBUG

  //
  // 外部オブジェクト
  //
  poke_3d::model::BaseCamera*     m_pBaseCamera;         // カメラ実体 @todo システムで吸収？ 
  const Area::AreaDataManager*    m_pAreaDataManager;    // エリア設定データ
  const Target::TargetBase*       m_pTarget;             // 注視対象
  // const gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel;
  gfl2::collision::CollisionModel* m_pCameraAreaClollisionModel; // @todo コリジョン関数がconstだと呼べないので


};  // class FieldControllerTypeArea


GFL_NAMESPACE_END( Controller );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERACONTROLLERERTYPEAREA_H_INCLUDED__

  