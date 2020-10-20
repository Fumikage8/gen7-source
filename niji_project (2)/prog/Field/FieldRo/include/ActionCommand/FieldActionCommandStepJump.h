#pragma once
//=================================================================================================
/**
*  @file   FieldActionCommandStepJump.h
*  @brief  フィールド用 アクションコマンド 段差ジャンプ
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "FieldActionCommandBase.h"
#include "FieldActionCommand_define.h"
#include <math/include/gfl2_Easing.h>

namespace Field{ namespace MoveModel {

class ActionCommandStepJump : public ActionCommandBase
{
public:

  // 未ライド時の段差ジャンプはZに150cm程前に飛ぶ
  static const gfl2::math::Vector3 PLAYER_JUMP_MOVE;
  // ライド時の段差ジャンプはZに250cm程前に飛ぶ
  static const gfl2::math::Vector3 RIDE_JUMP_MOVE;

  struct CAMERA_SHAKE_SETTING_CORE
  {
    f32 mass;
    f32 k;
    f32 firstVelocity;
    f32 damping;
    u32 wait;
  };
    
  // 着地時のカメラ揺れ設定
  struct CAMERA_SHAKE_SETTING
  {
    CAMERA_SHAKE_SETTING_CORE vertical;
    CAMERA_SHAKE_SETTING_CORE horizon;
  };
  
  // 着地時のカメラ揺れ設定
  enum CameraShakeType
  {
    CAMERA_SHAKE_TYPE_LIGHT = 0,    // 軽量
    CAMERA_SHAKE_TYPE_MIDDLE,       // 中量
    CAMERA_SHAKE_TYPE_HEAVY,        // 重量

    CAMERA_SHAKE_TYPE_MAX
  };

  // ジャンプ中のカメラ遅延設定
  struct CAMERA_DELAY
  {
    u32 waitFrame;
    u32 moveFrame;
    gfl2::math::Easing::EaseFunc moveEaseFunc;
  };

private:

  //  登録できるシーケンスの最大数
  static const u32 ACMDFUNC_SEQ_MAX = 5;

  // このアクションでのワークのマッピング方法
  struct Work
  {
    s16 frameMax;
    s16 frame;
    gfl2::math::Vector3    startPos;
    gfl2::math::Quaternion startQua;
    gfl2::math::Vector3    dstPos;
    gfl2::math::Vector3    dstPosAfterAdjustGround;
  };

  // アクション処理型( 静的関数ではなく非静的関数(メンバ関数)を利用したいためにBaseクラスに纏めずに記載しています )
  typedef Field::MoveModel::ACTION_COMMAND_RETURN (ActionCommandStepJump::*ActionCommandFunc)( ActionCommandWork* );

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  ActionCommandStepJump(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~ActionCommandStepJump(){}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドIDを返す
  *
  *  @return アクションコマンド
  */
  //-----------------------------------------------------------------------------
  virtual FieldActionCommandId GetActionCommandID(void){ return FIELD_ACTION_COMMAND_STEP_JUMP; }

#if PM_DEBUG
  // デバッグメニューで設定を操作するため、ポインタ取得を公開
  static CAMERA_DELAY* GetCameraDelaySettingPtr( void ) { return &m_CameraDelaySetting; }
  static CAMERA_DELAY* GetCameraDelaySettingRidePtr( void ) { return &m_CameraDelaySettingRide; }
  static CAMERA_SHAKE_SETTING* GetCameraShakeSettingPtr( CameraShakeType type ) { return &m_CameraShakeSetting[ type ]; }
#endif

private:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの初期化(ジャンプ)
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionInitJump(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの実行(ジャンプ)
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionMainJump(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの初期化(着地)
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionInitLanding(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //----------------------------------------------------------------------------
  /**
  *  @brief  アクションコマンドの実行(着地)
  *
  *  @param  pActionCommandWork アクションコマンドワーク
  *  @return アクションコマンドの状態
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionMainLanding(Field::MoveModel::ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  * @brief  アクションコマンドの終了
  *
  * @param  pActionCommandWork  アクションコマンドワーク
  *
  * @retval ACTION_COMMAND_RETURN_END
  */
  //-----------------------------------------------------------------------------
  ACTION_COMMAND_RETURN ActionEnd(ActionCommandWork* pActionCommandWork);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  UpdateActionCommand関数内で実行される処理
  *
  *  @retval ACTION_COMMAND_RETURN_ONE_TIME_END(続く)
  *  @retval ACTION_COMMAND_RETURN_END         (終わり)
  *  @note   静的関数ではなく非静的関数(メンバ関数)を利用したいためにBaseクラスに纏めずに記載しています
  */
  //-----------------------------------------------------------------------------
  virtual Field::MoveModel::ACTION_COMMAND_RETURN ExecuteFunc(ActionCommandWork* pActionCommandWork) { return (this->*m_pFuncTbl[ pActionCommandWork->sequence ])( pActionCommandWork ); }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void) const;

private:

  ActionCommandFunc  m_pFuncTbl[ACMDFUNC_SEQ_MAX];    ///< シーケンス分の関数テーブル
  
  
  // カメラ設定
  static CAMERA_DELAY m_CameraDelaySetting;
  static CAMERA_DELAY m_CameraDelaySettingRide;
  static CAMERA_SHAKE_SETTING m_CameraShakeSetting[ CAMERA_SHAKE_TYPE_MAX ];
};

} // MoveModel
} // Field

