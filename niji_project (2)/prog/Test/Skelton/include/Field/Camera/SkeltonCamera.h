#if defined(GF_PLATFORM_WIN32)

/**
* @file    SkeltonCamera.h
* @brief   フィールド上のカメラ
* @author  ikuta_junya
* @date    2014.09.26
*/

#if !defined( FIELDCAMERA_H_INCLUDED )
#define FIELDCAMERA_H_INCLUDED 
#pragma once

#if defined(WIN32)

// ファイル操作系
#include <iostream>
#include <string>
#include <fstream>

#endif

#include <math/include/gfl2_Vector4.h>
#include <math/include/gfl2_Matrix.h>
#include "Test/Skelton/include/Field/Camera/SkeltonCameraType.h"
#include "Test/Skelton/include/Field/SkeltonModel.h"

GFL_NAMESPACE_BEGIN(Test)
GFL_NAMESPACE_BEGIN(Skelton)

// 前方宣言
class SkeltonCameraActionBase;

class SkeltonCamera
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
  SkeltonCamera( void );

  /**
  * @brief   デストラクタ
  *
  * @return  無し 
  */
  virtual ~SkeltonCamera( void );

  /**
  * @brief   初期化処理
  *
  * @param   pTarget ターゲット
  *
  * @return  無し 
  */
  void Initialize( SkeltonModel* pTarget ); 
  
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
   
  /**
  * @brief   カメラターゲット座標を取得
  *
  * @param   pOut 格納先
  *
  * @return  無し 
  */
  void GetTargetPosition( gfl2::math::Vector4* pOut ) const;

  /**
  * @brief   カメラポジション座標を取得
  *
  * @param   pOut 格納先
  *
  * @return  無し 
  */
  void GetCameraPosition( gfl2::math::Vector4* pOut ) const;
  
  /**
  * @brief   ビュー行列取得
  *
  * @param   pMtx 結果格納先
  *
  * @return  無し 
  */
  void GetViewMatrix( gfl2::math::Matrix* pMtx ) const;

  /**
  * @brief   射影行列取得
  *
  * @param   pMtx 結果格納先
  *
  * @return  無し 
  */
  void GetProjectionMatrix( gfl2::math::Matrix* pMtx ) const;


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

private:

  ProjectionSetting           m_ProjectionSetting;

  SkeltonCameraActionBase*      m_pCamera;
  SkeltonModel*     m_pTarget;
};

GFL_NAMESPACE_END(Skelton)
GFL_NAMESPACE_END(Test)

#endif
#endif // GF_PLATFORM_WIN32