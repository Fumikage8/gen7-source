/**
* @brief  フィールドカメラ、サポート機能
* @file   FieldCameraAreaSupport.h
* @author ikuta_junya
* @data   2015.05.21
*/

#if !defined( __FIELDCAMERAAREASUPPORT_H_INCLUDED__ )
#define __FIELDCAMERAAREASUPPORT_H_INCLUDED__
#pragma once


// field
#include "Field/FieldRo/include/Camera/FieldCameraTypes.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaTypes.h"
#include "Field/FieldRo/include/Camera/Target/FieldCameraTargetBase.h"
#include "Field/FieldRo/include/Camera/Area/FieldCameraAreaUtility.h"

#if PM_DEBUG
GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinGroup;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );
#endif

GFL_NAMESPACE_BEGIN( Field );

GFL_NAMESPACE_BEGIN( MoveModel );
class FieldMoveModelPlayer;
GFL_NAMESPACE_END( MoveModel );

GFL_NAMESPACE_BEGIN( Camera );
GFL_NAMESPACE_BEGIN( Area );

class Focus;

/**
* @brief フィールドカメラのサポート挙動
*/
class Support
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( Support );

public:

  /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:
      const Target::TargetBase*               m_pTarget;                // 注視対象
      const Focus*                            m_pFocus;
      const MoveModel::FieldMoveModelPlayer*  m_pMoveModelPlayer;       // 主人公
  };

  struct MoveType
  {
    enum Value
    { 
      NONE = 0,
      MOVE,
      MOVE_KEEP,
      MOVE_IN,

      MAX
    };
  };

  /**
  * @brief コンストラクタ
  *
  * @param rInitDesc  初期化設定
  *
  * @return 無し
  */ 
  Support( const InitializeDescription& rInitDesc );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~Support( void );

  /**
  * @brief 更新処理
  *
  * @return 無し
  */
  void Update( void );

  /**
  * @brief サポート挙動が有効か取得する
  *
  * @return true 有効 false 無効
  */
  b32 IsValid( void ) const;
  
  /**
  * @brief 指定データの設定に変更
  *
  * @param  rData 設定
  *
  * @return なし
  */
  void Change( const CAMERA_AREA_SUPPORT_SETTING_DATA& rData, b32 face = false );

  /**
  * @brief 距離を変換
  *
  * @param  src 元の距離
  *
  * @return 結果
  */
  f32 ConvertDistance( f32 src ) const;

  /**
  * @brief カメラサポート挙動の制御アクションの実行
  *
  * @param  value     アクション種別
  *
  * @return 成否
  */
  b32 Action( SupportAction::Value value );

  /**
  * @brief 距離サポートが有効か取得する
  *
  * @return trueなら有効、falseなら無効
  */
  b32 IsDistanceSupport( void ) const;

  /**
  * @brief 現在のサポート係数を取得する
  *
  * @return サポート係数
  */
  f32 GetValue( void ) const;
  
  /**
  * @brief スクロールストップ挙動の制御アクションの実行
  *
  * @param  flag trueストップが発生している
  *
  * @return なし
  */
  void SetScrollStopFlag( f32 flag ) { m_IsScrollStop = flag; };

  
  void Reset( void );

#if PM_DEBUG
  // デバッグメニューへの登録・解除
  void ResistDebugMenu( void );
  void UnResistDebugMenu( void );
#endif

private:
  
  // 自機の状態をチェックし、サポートの動作を取得
  MoveType::Value CheckPlayerMove( const MoveModel::FieldMoveModelPlayer* pMoveModelPlayer );
  void UpdateValue( MoveType::Value moveType );


  void ResetMoveFrame( void );
  void ResetWaitFrame( void );


  struct State
  {
    enum Value
    {
      STOP = 0,
      MOVE_IN,
      MOVE_OUT,
    };
  };

  //
  // 内部制御
  //
  CAMERA_AREA_SUPPORT_SETTING_DATA  m_Data;   // 現在の設定

  State::Value                      m_State;
  u8                                m_StopBit;                  // 1個でもビットが立っていた場合は停止
  b32                               m_IsScrollStop;             // スクロールストップ中ならtrue
  ValueController                   m_ValueController;

  s32                               m_InWaitFrame;
  s32                               m_OutWaitFrame;

#if PM_DEBUG
  gfl2::debug::DebugWinGroup*       m_pDebugWinGroup;
#endif

  //
  // 外部オブジェクト
  //
  const Target::TargetBase*               m_pTarget;                // 注視対象
  const MoveModel::FieldMoveModelPlayer*  m_pMoveModelPlayer;       // 主人公
  const Focus*                            m_pFocus;
};  // class Support


GFL_NAMESPACE_END( Area );
GFL_NAMESPACE_END( Camera );
GFL_NAMESPACE_END( Field );

#endif // __FIELDCAMERAAREASUPPORT_H_INCLUDED__

  