#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeGimmickEncountEscape.h
*  @brief  フィールド用 動作コード ギミックエンカウント逃走型
*  @author saita_kazuki
*  @date   2015.10.07
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

// 前方宣言
namespace Field {
  class FieldGimmickEncountEscapeActor;
  class TrafficPath;
}
namespace poke_3d { namespace model {
  class BaseModel;
}}

namespace Field{ namespace MoveModel {

class FieldMoveCodeGimmickEncountEscape : public FieldMoveCodeBase
{
public:

  enum PathType
  {
    PATH_TYPE_1,
    PATH_TYPE_2,

    PATH_TYPE_MAX,
    PATH_TYPE_NONE = PATH_TYPE_MAX,
  };
  
  struct Work
  {
    FieldGimmickEncountEscapeActor*   pActor;
    TrafficPath*                      pPath[ PATH_TYPE_MAX ];
    u32                               state;
    f32                               progressSpeed;
    u32                               standbySEID;
    u32                               moveSEID;
    f32                               progress;
    u32                               pathType;
  };

  /**
  *  @brief 状態
  */
  enum State
  {
    STATE_NONE,           ///< 未初期化
    STATE_STANDBY,        ///< 待機
    STATE_MOVE_PATH,      ///< パス移動
    STATE_VANISH,         ///< 消滅
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeGimmickEncountEscape(void);

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeGimmickEncountEscape();

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_GIMMICK_ENCOUNT_ESCAPE; }
  
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

  void MainProcess_StateNone( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateStandby( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateMovePath( FieldMoveModel* pMoveModel, Work* pWork) const ;
  void MainProcess_StateVanish( FieldMoveModel* pMoveModel, Work* pWork) const ;

  void InitializeCharaDrawInstanceByTrafficPath( poke_3d::model::BaseModel* pCharaDrawInstance, TrafficPath* pPath) const ;
  void UpdateCharaDrawInstanceByTrafficPath( poke_3d::model::BaseModel* pCharaDrawInstance, TrafficPath* pPath) const ;

  void InitializeStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const ;
  void UpdateStateParameter( u32 state, poke_3d::model::BaseModel* pModel, Work* pWork) const ;

};  // end of class FieldMoveCodeGimmickEncountEscape;
  
}; //end of namespace mmodel
}; //end of namespace fld
