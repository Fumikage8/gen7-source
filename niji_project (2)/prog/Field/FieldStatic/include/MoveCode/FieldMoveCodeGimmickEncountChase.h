#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountChase.h
*  @brief  フィールド用 動作コード ギミックエンカウント接近型
*  @author saita_kazuki
*  @date   2015.02.17
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

// 前方宣言
namespace Field {
  class FieldGimmickEncountChaseActor;
}
namespace poke_3d { namespace model {
  class BaseModel;
}}

namespace Field{ namespace MoveModel {

class FieldMoveCodeGimmickEncountChase : public FieldMoveCodeBase
{
public:
  
  struct Work
  {
    FieldGimmickEncountChaseActor*    pActor;
    u32                               state;
    f32                               moveSpeed;
    u32                               waitCnt;
    u32                               standbySEID;
    u32                               moveSEID;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeGimmickEncountChase(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeGimmickEncountChase();

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_GIMMICK_ENCOUNT_CHASE; }
  
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

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　再開
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Resume( FieldMoveCodeWork *pMoveCodeWork ) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理　中断
  *
  *  @param  pMoveCodeWork  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void Suspend( FieldMoveCodeWork *pMoveCodeWork ) const;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  終了
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  isSave         セーブ時に呼ばれたかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void Retirement(FieldMoveCodeWork* pMoveCodeWork, b32 isSave) const ;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  復帰
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  isLoad         ロードによって動作コードワークが上書きされたかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void Comeback( FieldMoveCodeWork *pMoveCodeWork, b32 isLoad ) const;

private:

  /**
  *  @brief 状態
  */
  enum State
  {
    STATE_NONE,           ///< 未初期化
    STATE_WAIT,           ///< 待機
    STATE_CHASE_PLAYER,   ///< プレイヤーに接近
    STATE_MISS,           ///< プレイヤーに逃げられた(=コリジョン範囲からプレイヤーが出た)
    STATE_GO_BACK,        ///< 初期位置に戻る
  };

  void MainProcess_StateNone( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateWait( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateChasePlayer( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateMiss( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateGoBack( FieldMoveModel* pMoveModel, Work* pWork) const ;




  void InitializeStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const ;
  void UpdateStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const ;

};  // end of class FieldMoveCodeGimmickEncountChase;
  
}; //end of namespace mmodel
}; //end of namespace fld
