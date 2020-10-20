﻿#pragma once
//=================================================================================================
/**
*  @file   FieldMoveCodeBase.h
*  @brief  フィールド用 動作コード ベース
*  @author yy-ikeuchi@gamefreak.co.jp
*  @date   2015.02.17
*/
//=================================================================================================
#include "../MoveCode/FieldMoveCode_define.h"

namespace Field{ namespace MoveModel {

//----------------------------------------------------------------------------------------
//! @brief 動作コード ベース(何もしない
//----------------------------------------------------------------------------------------
class FieldMoveCodeBase
{
public:

  //-----------------------------------------------------------------------------
  /**
  *  @brief  コンストラクタ
  */
  //-----------------------------------------------------------------------------
  FieldMoveCodeBase(void){}

  //-----------------------------------------------------------------------------
  /**
  *  @brief  デストラクタ
  */
  //-----------------------------------------------------------------------------
  virtual ~FieldMoveCodeBase(){}

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  初期化
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void InitProcess(FieldMoveCodeWork* p_wk) const {};

  //----------------------------------------------------------------------------
  /**
  *  @brief  動作コード処理  メイン
  *
  *  @param  p_wk  動作コードワーク
  */
  //-----------------------------------------------------------------------------
  virtual void MainProcess(FieldMoveCodeWork* p_wk) const {};
  
};

}; //end of namespace MoveModel
}; //end of namespace Field
