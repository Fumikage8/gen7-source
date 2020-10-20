/**
* @file    FieldCamera.h
* @brief   フィールド上のカメラ
* @author  ikuta_junya
* @date    2014.09.26
*/

#if !defined( FIELDCAMERA_H_INCLUDED )
#define FIELDCAMERA_H_INCLUDED 
#pragma once

// gflib2
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

// field
#include "../movemodel.h"
#include "FieldCameraTypes.h"

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

// 前方宣言

class FieldCameraActionBase;

class FieldCamera
{
public:

  struct ProjectionSetting
  {
    f32 nearZ;
    f32 farZ;
    f32 aspect;
    f32 aov;
  };

  /**
  * @brief   コンストラクタ
  *
  * @return  無し 
  */
  FieldCamera( void );

  /**
  * @brief   デストラクタ
  *
  * @return  無し 
  */
  virtual ~FieldCamera( void );

  /**
  * @brief   初期化処理
  *
  * @param   pTarget ターゲット
  *
  * @return  無し 
  */
  void Initialize( fld::mmodel::MoveModel* pTarget ); 
  
  /**
  * @brief   終了処理
  *
  * @return  無し 
  */
  void Terminate( void );

  /**
  * @brief   更新処理
  *
  * @return  無し 
  */
  void Update( void );


  b32 Change( const CAMERA_DATA* pCameraDara, b32 isForce = false );
  b32 ChangeDebugCamera( void );
  b32 ChangeSnapCamera( void );
  b32 ChangeTopCamera( void );
  void EndDebugCamera( void );

  /**
  * @brief   カメラターゲット座標を取得
  *
  * @param   pOut 格納先
  *
  * @return  無し 
  */
  void GetTargetPosition( gfl2::math::Vector* pOut ) const;

  /**
  * @brief   カメラポジション座標を取得
  *
  * @param   pOut 格納先
  *
  * @return  無し 
  */
  void GetCameraPosition( gfl2::math::Vector* pOut ) const;
  
  /**
  * @brief   ビュー行列取得
  *
  * @param   pMtx 結果格納先
  *
  * @return  無し 
  */
  void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const;

  /**
  * @brief   射影行列取得
  *
  * @param   pMtx 結果格納先
  * @param   pivotFlag  trueのとき、左回転済みの行列を取得する(CTRのみ使用する)
  *
  * @return  無し 
  */
  void GetProjectionMatrix( gfl2::math::Matrix44* pMtx, b32 pivotFlag = true ) const;

  /**
  * @brief   カリング用行列取得
  *
  * @param   pMtx 結果格納先
  *
  * @return  無し 
  */
  void GetCullingMatrix( gfl2::math::Matrix44* pMtx ) const;

  /**
  * @brief   射影行列関連パラメータを設定
  *
  * @param   rSetting  設定内容
  *
  * @return  無し 
  */
  void SetProjectionSetting( const ProjectionSetting& rSetting );

  /**
  * @brief   射影行列関連パラメータを取得
  *
  * @param   pSetting 結果格納先
  *
  * @return  無し 
  */
  void GetProjectionSetting( ProjectionSetting* pSetting ) const;

  /**
  * @brief   カメラ方向のY軸回転値を取得
  *
  * @return  Y軸回転値(ラジアン)
  */
  f32 GetYaw( void ) const;

  /**
  * @brief   カメラ操作の入力を行っているか取得
  *
  * @return  行っているならtrue、行っていないならfalse
  */
  b32 IsManual( void ) const;

  /**
  * @brief   デバック機能を制御
  *
  * @param   enable デバック機能を有効化するならtrue、無効化するならfalse
  *
  * @return  無し
  */
  void SetDebug( b32 enable );

  /**
  * @brief   デバックカメラになっているか取得
  *
  * @return  デバックカメラならtrue、通常カメラならfalse
  */
  b32 IsDebug( void ) const;

  void OutPutData( std::ofstream* pOfstream ) const;
  const c8* GetDataName( void ) const;

#if PM_DEBUG
  void Dump( void );
#endif // #if PM_DEBUG


  const FieldCameraActionBase*      GetGameCamera( void ) const;    // デバッグカメラが有効でも通常カメラが返る
  const FieldCameraActionBase*      GetViewCamera( void ) const;    // デバッグカメラが有効ならデバッグカメラが返る

private:

  ProjectionSetting           m_ProjectionSetting;

  FieldCameraActionBase*      m_pCamera;
  FieldCameraActionBase*      m_pDebugCamera;
  fld::mmodel::MoveModel*     m_pTarget;
};


class FieldCameraActionBase
{
public:

  struct CAMERA_DATA
  {
    gfl2::math::Vector      targetPos;
    gfl2::math::Vector      cameraPos;
  };

  FieldCameraActionBase( void ) : m_DebugModeEnable( false ), m_pProjectionSetting( NULL )
  {
    m_Name[0] = '\0'; 
  } 

  virtual ~FieldCameraActionBase( void )
  {
  }

  virtual void Update( void ) = 0;
  virtual void GetViewMatrix( gfl2::math::Matrix34* pMtx ) const = 0;

  virtual void GetProjectionMatrix( gfl2::math::Matrix44* pMtx, b32 pivotFlag = false ) const
  {
    *pMtx = gfl2::math::Matrix44::GetPerspective( 
      gfl2::math::ConvDegToRad( m_pProjectionSetting->aov ), 
      m_pProjectionSetting->aspect, 
      m_pProjectionSetting->nearZ, 
      m_pProjectionSetting->farZ,
      pivotFlag
      );
  }


  virtual void GetCameraData( CAMERA_DATA* pCameraData ) const = 0;
  virtual u32 GetDataID( void ) const = 0;
  virtual b32 IsManual( void ) const = 0;
  virtual void OutPutData( std::ofstream* pOfstream ) const{};

  virtual b32 IsDebugMode( void ) const { return m_DebugModeEnable; }
  virtual void SetDebugMode( b32 enable ) { m_DebugModeEnable = enable; }

#if PM_DEBUG
  virtual void Dump ( void ) const {}
#endif

  c8   m_Name[512];                  // 識別名(今はmesh名)
  b32  m_DebugModeEnable;
  FieldCamera::ProjectionSetting* m_pProjectionSetting;
};


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // #if !defined( FIELDCAMERA_H_INCLUDED )
