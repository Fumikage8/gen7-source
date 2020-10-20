//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldMoveModelEffectCreaterNpc.h
 *	@brief  プレイヤー用エフェクト生成処理
 *	@author	tomoya takahashi
 *	@date		2015.06.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/MoveModel/Effect/FieldMoveModelEffectCreaterNpc.h"
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
/**
 *  @brief  コンストラクタ
 */
FieldMoveModelEffectCreaterNpc::FieldMoveModelEffectCreaterNpc() : 
  IFieldMoveModelEffectCreaterBase(),
  m_EncountGrass()
{
  for( u32 i=0; i<NPC_CREATER_METHOD_NUM; ++i )
  {
    m_pCreaterMethodArray[i] = NULL;
  }
}

/**
 *  @brief  デストラクタ
 */
FieldMoveModelEffectCreaterNpc::~FieldMoveModelEffectCreaterNpc()
{
  this->Terminate();
}

/**
 *  @brief  初期化
 */
void FieldMoveModelEffectCreaterNpc::Initialize( const Description& desc )
{
  // それぞれ初期化
  {
    m_EncountGrass.Initialize( desc.pTerrainManager, desc.pEffectManager, desc.pNpc );
  }

  // Method配列の作成
  {
    m_pCreaterMethodArray[NPC_CREATER_METHOD_ENCOUNTGRASS] = &m_EncountGrass;
  }

  // Method配列の設定
  this->SetArray( m_pCreaterMethodArray, NPC_CREATER_METHOD_NUM );
}

/**
 *  @brief  破棄処理
 */
void FieldMoveModelEffectCreaterNpc::Terminate()
{
  m_EncountGrass.Terminate();
}


GFL_NAMESPACE_END(MoveModel);
GFL_NAMESPACE_END(Field);
