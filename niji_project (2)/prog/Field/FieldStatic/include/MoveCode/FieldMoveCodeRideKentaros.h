#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeRideKentaros.h
*  @brief  フィールド用 動作コード ケンタロスライド
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "FieldMoveCode_define.h"
#include "FieldMoveCodePlayer.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDashSmoke.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodeRideKentaros : public FieldMoveCodePlayer
{
public:

  struct KentarosWork
  {
    FieldMoveCodePlayer::Work       playerWork;
    Field::Effect::EffectDashSmoke* pEffectDashSmoke;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeRideKentaros(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeRideKentaros();

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
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_RIDE_KENTAROS; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void) const;

  //----------------------------------------------------------------------------
  /**
  *  @brief  岩砕き時のヒットストップフレームを取得
  *
  *  @return ヒットストップフレーム
  */
  //-----------------------------------------------------------------------------
  s32 GetHitStopFrame(void) const { return m_hitStopFrame; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  岩砕き時のカメラ用バネの質量
  *
  *  @return 質量
  */
  //-----------------------------------------------------------------------------
  f32 GetCameraShakeMass(void) const { return m_cameraShakeMass; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  岩砕き時のカメラ用バネのバネ定数
  *
  *  @return バネ定数
  */
  //-----------------------------------------------------------------------------
  f32 GetCameraShakeK(void) const { return m_cameraShakeK; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  岩砕き時のカメラ用バネの加速度
  *
  *  @return 加速度
  */
  //-----------------------------------------------------------------------------
  f32 GetCameraShakeFirstVelocity(void) const { return m_cameraFirstVelocity; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  岩砕き時のカメラ用バネの減衰率
  *
  *  @return 減衰率
  */
  //-----------------------------------------------------------------------------
  f32 GetCameraShakeDamping(void) const { return m_cameraDamping; }

  //----------------------------------------------------------------------------
  /**
  *  @brief  段差ジャンプができるかどうか
  *
  *  @return trueなら可、falseなら不可
  */
  //-----------------------------------------------------------------------------
  bool IsAcceptJump(void) const { return m_isAcceptJump; }

private:

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

  s32     m_hitStopFrame;
  f32     m_cameraShakeMass;
  f32     m_cameraShakeK;
  f32     m_cameraDamping;
  f32     m_cameraFirstVelocity;
  bool    m_isAcceptJump;

};  // end of class FieldMoveCodeRideKentaros;
  
}; //end of namespace mmodel
}; //end of namespace fld
