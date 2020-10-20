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
#include <math/include/gfl2_Easing.h>
#include <math/include/gfl2_SpringSimulation.h>

// poke_3d_lib
#include <model/include/gfl2_BaseCamera.h>

// field
#include "Field/FieldRo/include/Camera/Controller/FieldCameraControllerBase.h"


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
  * @brief 補間を打ち切り、最新の状態に更新する
  *
  * @return 無し
  */
  void Fix( void );

  /**
  * @brief ターゲット座標、ポジションを取得
  *
  * @param  pTarget   ターゲット座標格納先
  * @oaram  pPosition ポジション格納先
  *
  * @return 無し
  */
  void GetCameraAndTargetPosition( gfl2::math::Vector3* pTarget, gfl2::math::Vector3* pPosition ) const;

  /**
  * @brief 操作機能変更
  *
  * @param  pController 変更後の機能  (寿命を管理する)
  *
  * @return 無し
  */
  void ChangeController( Controller::ControllerBase* pController );

  /**
  * @brief 操作機能入れ替え
  *
  * @param pNextController 変更後の機能 (使用者が寿命の管理をしてください)
  *
  * @return 変更前の機能 (使用者が保持し、使用後に戻してください)
  *
  * @caution 使用者は使用後にこのメソッドを使用して変更前の機能を戻してください
  *
  * @return 無し
  */
  Controller::ControllerBase* SwapController( Controller::ControllerBase* pNextController );

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
  * @brief カメラのY軸回転量を取得
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
  * @brief 正射影か？
  *
  * @return true 正射影 false 射影
  */
  b32 IsOrtho( void ) const;

  /**
  * @brief 現在の設定値から生成されるビュー行列を取得する
  *
  * @param  pOut 行列格納先
  *
  * @return 引数で渡したポインタ
  */
  gfl2::math::Matrix34* GetViewMatrix( gfl2::math::Matrix34* pOut ) const;

  /**
  * @brief 設定されているコントローラーを取得
  *
  * @return コントローラー
  * @retval NULL 設定されていない
  */
  Controller::ControllerBase* GetControllerBase( void ) const { return m_pController; }

  /**
  * @brief 指定位置を始点とし、指定フレームで最新の状態まで遷移する
  *
  * @param  rTargetPosition カメラターゲット座標( 始点 )
  * @param  rCameraPosition カメラ座標( 始点 )
  * @param  frame           遷移にかけるフレーム
  *
  * @return 成否( コントローラーが設定されていないとき、コントローラーの状態によっては失敗する )
  */
  b32 SetInterpolation( const gfl2::math::Vector3& rTargetPosition, const gfl2::math::Vector3& rCameraPosition, u32 frame, gfl2::math::Easing::EaseFunc easeFunc = gfl2::math::Easing::LINEAR );

  /**
  * @brief ワールド振動をカメラに適用
  */
  void ApplyGlobalShake( gfl2::math::SpringSimulation& rHorizonSpring, gfl2::math::SpringSimulation& rVerticalSpring );

#if PM_DEBUG
  void Dump( void );
#endif

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

  //
  // デバッグ用
  //
#if PM_DEBUG
  b32                         m_IsUseSwap;                                    // カメラ操作機能を交換したら切り替わる。デフォルトfalse
#endif // PM_DEBUG

};  // class FieldCameraUnit


GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAUNITER_H_INCLUDED__
