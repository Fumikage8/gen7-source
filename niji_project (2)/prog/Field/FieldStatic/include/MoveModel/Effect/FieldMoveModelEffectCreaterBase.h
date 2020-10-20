//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelEffectCreaterBase.h
 *	@brief  動作モデル　エフェクト生成処理　共通処理
 *	@author	tomoya takahashi
 *	@date		2015.06.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#if !defined( __FIELD_MOVEMODEL_EFFECT_CREATER_BASE_H__ )
#define __FIELD_MOVEMODEL_EFFECT_CREATER_BASE_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(Field);
GFL_NAMESPACE_BEGIN(MoveModel);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class IFieldMoveModelEffectCreaterMethodBase; // 前方宣言

/**
 *  @class エフェクト生成管理　共通処理
 */
class IFieldMoveModelEffectCreaterBase
{
  GFL_FORBID_COPY_AND_ASSIGN( IFieldMoveModelEffectCreaterBase );
public:

  /**
   *  @brief  コンストラクタ
   */
  IFieldMoveModelEffectCreaterBase();

  /**
   *  @brief  デストラクタ
   */
  virtual ~IFieldMoveModelEffectCreaterBase(){}

  /** 
   *  @brief  ジャンプしたタイミングのエフェクト生成
   */
  void CreateTimingJump();

  /** 
   *  @brief  移動したタイミングのエフェクト生成
   */
  void CreateTimingMove();

  /** 
   *  @brief  ターンしたタイミングのエフェクト生成
   */
  void CreateTimingTurn();

  /**
   *  @brief  破棄処理
   */
  virtual void Terminate(){}

  /** 
   *  @brief  現座標でのブロックとグリッド座標を更新
   */
  void UpdateBlockAndGridPos();

protected:

  /**
   *  @brief  生成メソッド配列の設定
   */
  inline void SetArray( IFieldMoveModelEffectCreaterMethodBase ** ppCreaterMethodArray, u32 ArrayNum )
  {
    m_ppCreaterMethodArray = ppCreaterMethodArray;
    m_CreaterMethodArrayNum = ArrayNum;
  }

private:

  // Create処理の配列
  IFieldMoveModelEffectCreaterMethodBase ** m_ppCreaterMethodArray;
  u32                                       m_CreaterMethodArrayNum;
  
};


/**
 *  @class エフェクト生成メソッド  ベースクラス
 */
class IFieldMoveModelEffectCreaterMethodBase
{
  GFL_FORBID_COPY_AND_ASSIGN( IFieldMoveModelEffectCreaterMethodBase );

public:
  /**
   *  @brief  コンストラクタ
   */
  IFieldMoveModelEffectCreaterMethodBase()
  {
  }

  /**
   *  @brief  デストラクタ
   */
  virtual ~IFieldMoveModelEffectCreaterMethodBase()
  {
  }

  /** 
   *  @brief  ジャンプしたタイミングのエフェクト生成
   */
  virtual void CreateTimingJump() = 0;

  /** 
   *  @brief  移動したタイミングのエフェクト生成
   */
  virtual void CreateTimingMove() = 0;

  /** 
   *  @brief  ターンしたタイミングのエフェクト生成
   */
  virtual void CreateTimingTurn() = 0;

  /** 
   *  @brief  現座標でのブロックとグリッド座標を更新
   */
  virtual void UpdateBlockAndGridPos() = 0;

private:
};

GFL_NAMESPACE_END(MoveModel);
GFL_NAMESPACE_END(Field);


#endif	// __FIELD_MOVEMODEL_EFFECT_CREATER_BASE_H__

