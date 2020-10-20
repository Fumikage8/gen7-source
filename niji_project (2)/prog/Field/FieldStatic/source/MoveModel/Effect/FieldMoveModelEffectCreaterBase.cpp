//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelEffectCreaterBase.cpp
 *	@brief  動作モデル　エフェクト生成処理　共通処理
 *	@author	tomoya takahashi
 *	@date		2015.06.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/MoveModel/Effect/FieldMoveModelEffectCreaterBase.h"
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(Field);
GFL_NAMESPACE_BEGIN(MoveModel);

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					実装
*/
//-----------------------------------------------------------------------------
IFieldMoveModelEffectCreaterBase::IFieldMoveModelEffectCreaterBase() :
 m_ppCreaterMethodArray(NULL)
,m_CreaterMethodArrayNum(0)
{
}

/** 
 *  @brief  ジャンプしたタイミングのエフェクト生成
 */
void IFieldMoveModelEffectCreaterBase::CreateTimingJump()
{
  for( u32 i=0; i<m_CreaterMethodArrayNum; ++i )
  {
    m_ppCreaterMethodArray[i]->CreateTimingJump();
  }
}

/** 
 *  @brief  移動したタイミングのエフェクト生成
 */
void IFieldMoveModelEffectCreaterBase::CreateTimingMove()
{
  for( u32 i=0; i<m_CreaterMethodArrayNum; ++i )
  {
    m_ppCreaterMethodArray[i]->CreateTimingMove();
  }
}

/** 
 *  @brief  ターンしたタイミングのエフェクト生成
 */
void IFieldMoveModelEffectCreaterBase::CreateTimingTurn()
{
  for( u32 i=0; i<m_CreaterMethodArrayNum; ++i )
  {
    m_ppCreaterMethodArray[i]->CreateTimingTurn();
  }
}

/** 
 *  @brief  現座標でのブロックとグリッド座標を更新
 */
void IFieldMoveModelEffectCreaterBase::UpdateBlockAndGridPos()
{
  for( u32 i=0; i<m_CreaterMethodArrayNum; ++i )
  {
    m_ppCreaterMethodArray[i]->UpdateBlockAndGridPos();
  }
}

GFL_NAMESPACE_END(MoveModel);
GFL_NAMESPACE_END(Field);