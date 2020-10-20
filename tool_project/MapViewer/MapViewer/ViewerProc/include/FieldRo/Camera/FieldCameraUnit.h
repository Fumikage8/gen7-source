/**
* @brief  フィールドカメラ、1制御単位クラス
* @file   FieldCameraUnit.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAUNITER_H_INCLUDED__ )
#define __FIELDCAMERAUNITER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field
#include "ViewerProc/include/FieldRo/Camera/Controller/FieldCameraControllerBase.h"



// 前方宣言
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( renderingengine );
GFL_NAMESPACE_BEGIN( scenegraph );
GFL_NAMESPACE_BEGIN( instance );
class DrawEnvNode;
GFL_NAMESPACE_END( instance );
GFL_NAMESPACE_END( scenegraph );
GFL_NAMESPACE_END( renderingengine );
GFL_NAMESPACE_END( gfl2 );

GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );

/**
* @brief フィールドカメラ、1制御単位クラス
*/
class CameraUnit
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( CameraUnit );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
    gfl2::heap::HeapBase*         m_pSystemHeap;
    gfl2::heap::HeapBase*         m_pDeviceHeap;
    gfl2::gfx::IGLAllocator*      m_pDeviceAllocator;
    //gfl2::fs::BinLinkerAccessor*  pAreaResourceDataAccessor;   // 1エリア分の3Dリソースデータ(テクスチャ/シェーダー/アニメーション)
    //gfl2::fs::AsyncFileUnit*   m_pFileUnit;              // ファイル読み込みマネージャー
  };

  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  CameraUnit( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~CameraUnit( void );
  
  /**
  * @brief 初期化処理
  *
  * @param  rInitDesc   初期化設定
  *
  * @return 無し
  */
  void Initialize( const InitializeDescription& rInitDesc );

  /**
  * @brief 終了処理
  *
  * @return 無し
  */
  void Terminate( void );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( void );

  /**
  * @brief 操作機能変更
  *
  * @param  pController 変更後の機能  (寿命を管理する)
  *
  * @return 無し
  */
  void ChangeController( Controller::ControllerBase* pController );


  /**
  * @brief カメラ実体を取得
  *
  * @return カメラ実体
  */
  const poke_3d::model::BaseCamera* GetBaseCamera( void ) const
  {
     return m_pBaseCamera;
  }

  /**
  * @brief カメラのY軸回転量を取得 @todo この関数は消すかも
  *
  * @return Y軸回転量(ラジアン、Z軸正方向==0.0f)
  */
  f32 GetRotationY( void ) const;

  /**
  * @brief 更新フラグを設定
  *
  * @param  updateFlag  更新フラグ
  *
  * @return 無し
  */
  void SetUpdateFlag( b32 updateFlag );

  /**
  * @brief カメラの基本パラメータをコピーする
  * @note  View行列、射影行列に関わるパラメータのみをコピーする。アニメーション関連、コントローラー関連の値はコピーされません
  *
  * @param  rSrc コピー元
  *
  * @return 無し
  */
  void CopyBaseParam( const CameraUnit& rSrc );

  /**
  * @brief 現在の設定値から生成されるプロジェクション行列を取得する
  *
  * @param  pOut 行列格納先
  *
  * @return 引数で渡したポインタ
  */
  gfl2::math::Matrix44* GetProjectionMatrix( gfl2::math::Matrix44* pOut ) const;

  /**
  * @brief 現在の設定値から生成されるビュー行列を取得する
  *
  * @param  pOut 行列格納先
  *
  * @return 引数で渡したポインタ
  */
  gfl2::math::Matrix34* GetViewMatrix( gfl2::math::Matrix34* pOut ) const;


private:


  //
  // 制御
  //
  b32                         m_IsUpdate;                                     // 更新するならtrue

  //
  // 内部オブジェクト
  //
  poke_3d::model::BaseCamera* m_pBaseCamera;                                  // カメラ実体
  Controller::ControllerBase* m_pController;                                  // カメラ操作機能
  gfl2::renderingengine::scenegraph::instance::DrawEnvNode*  m_pDrawEnvNode;  // カメラアニメーションで必要

  //
  // 外部オブジェクト
  //

};  // class FieldCameraUnit


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAUNITER_H_INCLUDED__