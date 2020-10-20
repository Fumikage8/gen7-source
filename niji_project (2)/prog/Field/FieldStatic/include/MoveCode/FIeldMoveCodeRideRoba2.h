#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeRideRoba2.h
*  @brief  フィールド用 動作コード 地面馬ライド
*  @author ikuta_junya@gamefreak.co.jp
*  @date   2015.10.15
*/
//=================================================================================================
#include "FieldMoveCode_define.h"
#include "FieldMoveCodePlayer.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDashSmoke.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodeRideRoba2 : public FieldMoveCodePlayer
{
public:

  struct Roba2Work
  {
    FieldMoveCodePlayer::Work       playerWork;
  };


  struct CAMERA_SHAKE_SETTING_CORE
  {
    // 各足単位で有効化
    b32 isLeftHand;
    b32 isRightHand;
    b32 isLeftFoot;
    b32 isRightFoot;
    
    // 揺れ方
    f32 mass;
    f32 k;
    f32 firstVelocity;
    f32 damping;
    u32 wait;
  };


  // 接地時のカメラ揺れ設定
  struct CAMERA_SHAKE_SETTING
  {
    b32 isUseRun;     // 走りで有効化
    b32 isUseWalk;    // 歩きで有効化

    // 揺れ方
    CAMERA_SHAKE_SETTING_CORE walkShake;
    CAMERA_SHAKE_SETTING_CORE runShake;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeRideRoba2(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeRideRoba2();

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　初期化
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン　
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  ポスト
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void PostProcess(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  再開
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Resume(FieldMoveCodeWork* pMoveCodeWork) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Suspend(FieldMoveCodeWork* pMoveCodeWork) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_RIDE_MUURAND; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  段差ジャンプができるかどうか
  *
  *  @return trueなら可、falseなら不可
  */
  //-----------------------------------------------------------------------------
  bool IsAcceptJump(void) const { return m_isAcceptJump; }

#if PM_DEBUG
  //----------------------------------------------------------------------------
  /**
  *  @brief  カメラ揺れ設定へのポインタを取得(デバッグメニュー用)
  *
  *  @return カメラ設定へのポインタ
  */
  //-----------------------------------------------------------------------------
  static CAMERA_SHAKE_SETTING* GetCameraShakeSettingPtr(void) { return &m_CameraShakeSetting; }
#endif

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief  指定ステートがアクセル関連ステートかチェック
  *
  *  @param  state   チェック対象ステート
  *
  *  @return trueならアクセル関連、falseならその他
  */
  //-----------------------------------------------------------------------------
  static b32 IsAccelState( u32 state );

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートを上書きするタイミングで呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual u32 MainProcessSubOverrideState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メインのサブ関数(ステートが変わったときに呼ばれる)
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcessSubChangeState(FieldMoveCodeWork* pMoveCodeWork,u32 nowState,u32 oldState,u32 oldoldState) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  カメラ振動関連更新処理
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  void UpdateCameraShake( FieldMoveCodeWork* pMoveCodeWork ) const;

  bool    m_isAcceptJump;
  static  CAMERA_SHAKE_SETTING m_CameraShakeSetting;

};  // end of class FieldMoveCodeRideRoba2;
  
}; //end of namespace mmodel
}; //end of namespace fld