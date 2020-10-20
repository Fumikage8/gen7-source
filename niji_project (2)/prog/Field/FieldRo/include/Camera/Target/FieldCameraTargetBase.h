/**
* @brief  フィールドカメラのターゲット基底クラス
* @file   FieldCameraTargetBase.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERATARGETBASEER_H_INCLUDED__ )
#define __FIELDCAMERATARGETBASEER_H_INCLUDED__
#pragma once


// gfl2
#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_Vector3.h>
#include <math/include/gfl2_Easing.h>


// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );

/**
* @brief フィールドカメラのターゲット基底クラス
*/
class TargetBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TargetBase );

public:

  static const u32 STACK_SIZE = 4;  // 遅らせるフレーム + 1
 
  /**
  * @brief コンストラクタ
  *
  * @return 無し
  */ 
  TargetBase( void );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TargetBase( void );
  
  /**
  * @brief カメラがみる座標を更新する
  *
  * @return 無し
  */
  virtual void Update( void );

  /**
  * @brief フィールドの準備が完了した際に実行される関数
  *
  * @return 無し
  */
  virtual void Ready( void ) {};

  /**
  * @brief 補間を打ち切り、最新の状態に更新する
  *
  * @return 無し
  */
  virtual void Fix( void ) {};

  /**
  * @brief 遅延追従を停止する
  *
  * @return 無し
  */
  virtual void DelayFollowPause( void ) {};

  /**
  * @brief 遅延追従を再開する
  *
  * @return 無し
  */
  virtual void DelayFollowResume( void ) {};

  /**
  * @brief 指定フレーム遅らせる
  *
  * @param  waitFrame     待ちフレーム
  * @param  moveFrame     移動フレーム
  * @param  easeFunc      移動中のイージング
  *
  * @return 無し
  */
  virtual void Delay( u32 waitFrame, u32 moveFrame, gfl2::math::Easing::EaseFunc easeFunc ) {} ;
  
  /**
  * @brief ターゲット座標を強制的に上書きする
  *
  * @param  rPos  上書き座標
  *
  * @return 無し
  */
  virtual void OverWrite( const gfl2::math::Vector3& rPos );

  /**
  * @brief ターゲット座標を強制的に上書きしているか
  *
  * @return true している
  */
  virtual b32 IsOverWrite( void )const { return m_IsOverWrite; }
  
  /**
  * @brief ターゲット座標の上書きを取得
  *
  * @param  pPos  結果
  *
  * @return 無し
  */
  virtual void GetOverWrite( gfl2::math::Vector3* pPos ) const;

  /**
  * @brief ターゲット座標を強制的に上書きを無効にする
  *
  * @return 無し
  */
  virtual void ClearOverWrite( void );

  /**
  * @brief カメラがみる座標を取得する
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetCameraTargetPosition( gfl2::math::Vector3* pOut ) const = 0;

  /**
  * @brief カメラ制御に使用する座標を取得する
  * @note  カメラエリアの判定など
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetCameraControlPosition( gfl2::math::Vector3* pOut ) const = 0;

  /**
  * @brief カメラ制御の元になる座標を取得
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetTargetRowPosition( gfl2::math::Vector3* pOut ) const = 0;

private:

  b32                       m_IsStackSetUp;
  gfl2::math::Vector3       m_PositionStackTable[ STACK_SIZE ];

  b32                       m_IsOverWrite;
  gfl2::math::Vector3       m_OverWritePositon;

};  // class FieldTargetBase


GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERATARGETBASEER_H_INCLUDED__