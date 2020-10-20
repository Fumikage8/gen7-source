#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeRideMuurando.h
*  @brief  フィールド用 動作コード ムーランドライド
*  @author ikuta_junya@gamefreak.co.jp
*  @date   2015.10.15
*/
//=================================================================================================
#include "FieldMoveCode_define.h"
#include "FieldMoveCodePlayer.h"
#include "Field/FieldRo/include/Effect/content/FieldEffectDashSmoke.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodeRideMuurando : public FieldMoveCodePlayer
{
public:

  struct MuurandoWork
  {
    FieldMoveCodePlayer::Work       playerWork;
    s8                              distanceRank;            // 現在の距離評価
    u8                              searchStartWaitCount;    // サーチ開始を遅らせるためのカウンタ
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeRideMuurando(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeRideMuurando();

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

  //-----------------------------------------------------------------------------
  /**
  *  @brief  ムーランドのサーチ状態かどうか
  *
  *  @return trueならその状態、falseならその状態ではない
  */
  //-----------------------------------------------------------------------------
  b32 IsMuurandSearchState(const FieldMoveCodeWork* pMoveCodeWork) const;
  b32 IsMuurandSearchStateEx(const FieldMoveCodeWork* pMoveCodeWork) const;
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

  f32     m_SearchAngle;
  u8      m_SearchStartWait;
  bool    m_isAcceptJump;
  gfl2::math::Vector3 m_EffectOffset;
  f32 m_EffectScaleTable[2];

};  // end of class FieldMoveCodeRideMuurando;
  
}; //end of namespace mmodel
}; //end of namespace fld