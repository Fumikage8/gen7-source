#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodePlayer.h
*  @brief  フィールド用 動作コード テンプレート(こちらをコピーして量産してください)
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include <math/include/gfl2_Vector.h>
#include <math/include/gfl2_Matrix.h>

#include "FieldMoveCode_define.h"
#include "FieldMoveCodeBase.h"

namespace Field{ namespace MoveModel {

class FieldMoveCodePlayer : public FieldMoveCodeBase
{
public:
  
  struct Work
  {
    u32 temp;
  };

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodePlayer(void){}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodePlayer(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  対応している動作コードの取得
  *
  *  @return 動作コード
  */
  //-----------------------------------------------------------------------------
  virtual FIELD_MOVE_CODE_ID GetMVCode(void) const { return FIELD_MOVE_CODE_PLAYER; }
  
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

private:

  //----------------------------------------------------------------------------
  /**
  *  @brief  アナログスティックの倒し具合からステートを取得
  *
  *  @param  stick  アナログスティックの倒し具合
  *  @return ステート
  */
  //-----------------------------------------------------------------------------
  u32 getStateFromStickLevel( f32 stick ) const;

};  // end of class FieldMoveCodePlayer;
  
}; //end of namespace mmodel
}; //end of namespace fld
