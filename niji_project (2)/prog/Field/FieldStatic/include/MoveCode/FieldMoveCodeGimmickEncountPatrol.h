#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountPatrol.h
*  @brief  フィールド用 動作コード ギミックエンカウント巡回型
*  @author saita_kazuki
*  @date   2015.09.05
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

// 前方宣言
namespace Field {
  class FieldGimmickEncountPatrolActor;
  class TrafficPath;
}
namespace poke_3d { namespace model {
  class BaseModel;
}}

namespace Field{ namespace MoveModel {

class FieldMoveCodeGimmickEncountPatrol : public FieldMoveCodeBase
{
public:

  struct Work
  {
    FieldGimmickEncountPatrolActor*   pActor;
    TrafficPath*                      pPath;
    u32                               state;
    f32                               progressSpeed;
    u32                               standbySEID;
    u32                               moveSEID;
    f32                               progress;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeGimmickEncountPatrol(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeGimmickEncountPatrol();

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_GIMMICK_ENCOUNT_PATROL; }
  
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
  virtual void Retirement(FieldMoveCodeWork* pMoveCodeWork,b32 isSave) const ;

  //-----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  復帰
  *
  *  @param  pMoveCodeWork  動作コードワーク
  *  @param  isLoad         ロードによって動作コードワークが上書きされたかどうか
  */
  //-----------------------------------------------------------------------------
  virtual void Comeback( FieldMoveCodeWork *pMoveCodeWork,b32 isLoad ) const;

private:

  /**
  *  @brief 状態
  */
  enum State
  {
    STATE_NONE,           ///< 未初期化
    STATE_MOVE_PATH       ///< パス移動
  };

  void MainProcess_StateNone( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateMovePath( FieldMoveModel* pMoveModel, Work* pWork) const ;

  void UpdateCharaDrawInstanceByTrafficPath( poke_3d::model::BaseModel* pCharaDrawInstance, TrafficPath* pPath) const ;

  void UpdateStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const ;

};  // end of class FieldMoveCodeGimmickEncountPatrol;
  
}; //end of namespace mmodel
}; //end of namespace fld
