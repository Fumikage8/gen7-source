/**
* @brief  フィールドカメラのターゲット、主人公動作モデル
* @file   FieldCameraTargetTypeMoveModelPlayer.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERATARGETTYPEMOVEMODELER_H_INCLUDED__ )
#define __FIELDCAMERATARGETTYPEMOVEMODELER_H_INCLUDED__
#pragma once

// @note 最初は動作モデルをターゲットにしていたが、仕様変更により自機を管理するようになった。修正時期が開発終盤だったため、ファイル名をそのままにクラス名のみを変更している

// field
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"

// camera
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

#if PM_DEBUG
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );
#endif

// 前方宣言
GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModelPlayer;
GFL_NAMESPACE_END( MoveModel );
GFL_NAMESPACE_END( Field );


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Target );

/**
* @brief フィールドカメラのターゲット、動作モデル
*/
class TargetTypeMoveModelPlayer : public TargetBase
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( TargetTypeMoveModelPlayer );

public:
 
  /**
  * @brief コンストラクタ
  *
  * @param  pMoveModelPlayer 中身、寿命は管理しない
  *
  * @return 無し
  */ 
  TargetTypeMoveModelPlayer( gfl2::heap::HeapBase* pHeap, MoveModel::FieldMoveModelPlayer* pMoveModelPlayer = 0 );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~TargetTypeMoveModelPlayer( void );
  
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
  virtual void Ready( void );

  /**
  * @brief 補間を打ち切り、最新の状態に更新する
  *
  * @return 無し
  */
  virtual void Fix( void );

  /**
  * @brief 遅延追従を停止する
  *
  * @param  frame 遅延を打ち切るフレーム数
  *
  * @return 無し
  */
  virtual void DelayFollowPause( u32 frame );

  /**
  * @brief 遅延追従を再開する
  *
  * @return 無し
  */
  virtual void DelayFollowResume( void );
  
  /**
  * @brief 指定フレーム遅らせる
  *
  * @param  waitFrame     待ちフレーム
  * @param  moveFrame     移動フレーム
  * @param  easeFunc      移動中のイージング
  *
  * @return 無し
  */
  virtual void Delay( u32 waitFrame, u32 moveFrame, gfl2::math::Easing::EaseFunc easeFunc );

  /**
  * @brief カメラがみる座標を取得する
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetCameraTargetPosition( gfl2::math::Vector3* pOut ) const;

  /**
  * @brief カメラ制御に使用する座標を取得する
  * @note  カメラエリアの判定など
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetCameraControlPosition( gfl2::math::Vector3* pOut ) const;

  /**
  * @brief カメラ制御の元になる座標を取得
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  virtual b32 GetTargetRowPosition( gfl2::math::Vector3* pOut ) const;

  /**
  * @brief 中身を設定
  *
  * @param  pMoveModelPlayer 中身、寿命は管理しない
  *
  * @return 無し
  */ 
  void SetContent( MoveModel::FieldMoveModelPlayer* pMoveModelPlayer );

private:

#if PM_DEBUG
  void ResistDebugMenu( void );
  void UnResistDebugMenu( void );
  void DebugUpdateCameraCheck( void );
#endif

  /**
  * @brief プレイヤーに遅延追従するかチェック
  *
  * @param targetPositionDiff 追従座標の差分
  *
  * @return true : 追従する false : 吸着する
  */ 
  b32 IsPlayerFollow( f32 targetPositionDiff ) const;

  /**
  * @brief 未来予測の更新速度を更新
  *
  * @return 無し
  */ 
  void UpdateFutureMoveStepFreme( void );

  /**
  * @brief ムーランド専用カメラ挙動更新
  *
  * @return 無し
  */ 
  void UpdateMuurando( void );
  
  /**
  * @brief ムーランドか取得
  *
  * @return true ムーランド false 違う
  */ 
  b32 IsMuurando( void ) const;

  /**
  * @brief 座標を取得する
  *
  * @param  pOut  格納先
  *
  * @return 成否
  */
  b32 GetModelPosition( gfl2::math::Vector3* pOut ) const;


  void UpdateDirectionDir( void );

  struct State
  {
    enum Value
    {
      READY = 0,
      NORMAL,
      RETURN,
      DIRECT
    };
  };

  struct OffsetState
  {
    enum Value
    {
      NONE = 0,
      STATE_IN,
      STATE_OUT
    };
  };

  struct Position
  {
    enum Value
    {
      ROW = 0,
      FUTURE_BLEND,
      RETURN,
      FIX
    };
  };

  State::Value                         m_State;

  Field::Camera::Area::ValueController m_MuurandOffsetValueController;
  gfl2::math::Vector2                  m_MuurandOffsetDir;
  gfl2::math::Vector3                  m_TargetPosition;
  gfl2::math::Vector3                  m_PrevTargetPosition;


  gfl2::math::Vector3                  m_FuturePosition;
  gfl2::math::Vector3                  m_PreFixPosition;
  gfl2::math::Vector3                  m_FixPosition;
  Camera::Area::ValueController        m_ReturnValueController;
  Camera::Area::ValueController        m_ReturnToMoveValueController;
  f32                                  m_ReturnToMoveStartDistance;
  gfl2::math::Vector3                  m_ReturnStartPosition;
  Camera::Area::ValueController        m_DistanceValueController;


  Camera::Area::ValueController        m_DelayValueController;
  gfl2::math::Vector3                  m_DelayPositon;


  // 進行方向先取り機能
  class FutureMove;
  FutureMove*                          m_pFutureMove;

#if PM_DEBUG
  gfl2::debug::DebugWinGroup*       m_pDebugWinGroup;
  b32                               m_DebugForceMuurandFlag;      // true:強制ムーランドカメラ
  b32                               m_DebugForceDirFlag;          // true:強制向き固定
  gfl2::math::Vector3               m_DebugForceDir;              // 強制向き

#endif

  //
  // 外部オブジェクト
  //
  MoveModel::FieldMoveModelPlayer* m_pMoveModelPlayer;


};  // class FieldTargetTypeMoveModelPlayer


GFL_NAMESPACE_END( Target );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERATARGETTYPEMOVEMODELER_H_INCLUDED__