#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeRideSamehader.h
*  @brief  フィールド用 動作コード サメハダーライド
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "FieldMoveCode_define.h"
#include "FieldMoveCodePlayer.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodeRideSamehader : public FieldMoveCodePlayer
{
private:

  struct SamehaderWork
  {
    FieldMoveCodePlayer::Work      playerWork;
  };

  struct ExtendWork
  {
    static const u32 QUA_ARRAY_SIZE = 32;
    gfl2::math::Quaternion moveModelQuaArray[QUA_ARRAY_SIZE];
  };

public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeRideSamehader(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeRideSamehader();

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
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_RIDE_SAMEHADER; }

  //-----------------------------------------------------------------------------
  /**
  *  @brief  RTTIマネージャーに自身のプロパティを登録
  */
  //-----------------------------------------------------------------------------
  virtual void RegistRtti(void) const;

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

private:
  
  //note: 動作コードに持たせてもいいメンバ変数は、自身の動作コードを持つ全動作モデルに影響させてもいいもののみ

  f32 m_slipRateDash;
  f32 m_slipRateWalk;

};  // end of class FieldMoveCodeRideSamehader;
  
}; //end of namespace mmodel
}; //end of namespace fld
