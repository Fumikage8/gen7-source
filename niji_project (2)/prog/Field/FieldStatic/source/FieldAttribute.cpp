//===================================================================
/**
 * @file    FieldAttribute.cpp
 * @brief   フィールドアトリビュート
 * @author  saita_kazuki
 * @date    2015.05.28
 */
//===================================================================

#include "Field/FieldStatic/include/FieldAttribute.h"

// gflib2
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN( Field )

Attribute::AttributeParameterTable* Attribute::m_pParameterTable = NULL;

/**
 * @brief 初期化
 * @param pParameterTableData バイナリデータ
 * @note FieldResidentから取得してセットしてください
 */
void Attribute::Initialize( void* pParameterTableData)
{
  m_pParameterTable = reinterpret_cast<AttributeParameterTable*>( pParameterTableData);
}

/**
 * アトリビュート数の取得
 */
u32 Attribute::GetAttributeNum()
{
  return m_pParameterTable->header.num;
}

/**
   * @brief 水アトリビュートかチェック
   */
bool Attribute::IsWater( Attr attr )
{
  if( IsEnableAttribute( attr))
  {
    switch( attr )
    {
    case ATTR_SEA:
    case ATTR_TANSUI:
      return true;
    }
  }

  return false;
}


/**
 *  @brief 草むらアトリビュートかチェック
 */
bool Attribute::IsGrass( Attr attr )
{
  if( IsEnableAttribute( attr))
  {
    switch( attr )
    {
    case ATTR_KUSAMURA:
    case ATTR_MIJIKAIKUSA:
    case ATTR_FLOWER_Y:
    case ATTR_KAREKUSA:
    case ATTR_FLOWER_R:
    case ATTR_NAGAKUSA:
    case ATTR_KUSAHANA:
      return true;
    }
  }

  return false;
}

/**
 *  @brief  海アトリビュートかチェック
 */
bool Attribute::IsSea( Attr attr )
{
  if( IsEnableAttribute( attr))
  {
    switch( attr )
    {
    case ATTR_SEA:
      return true;
    }
  }

  return false;
}

/**
 *  @brief ロバ２しか入れない地面アトリビュートチェック
 */
bool Attribute::IsRoba2Ground( Attr attr )
{
  if( IsEnableAttribute( attr))
  {
    switch( attr )
    {
    case ATTR_HORSE_IWABA:
      return true;
    }
  }

  return false;
}


/**
 * @brief 戦闘背景近景ID取得
 */
u8 Attribute::GetBattleNearBGID( Attr attr, u32 pattern)
{
  if( IsEnableAttribute( attr))
  {
    GFL_ASSERT( pattern < m_pParameterTable->header.battleBGNearTypeNum );

    const Core* pCore = GetCoreData( attr);
    c8* pSeek = reinterpret_cast<c8*>(m_pParameterTable);
    pSeek = (pSeek + pCore->battleBGNearTypeAddrOffset) + (pattern);

    return static_cast<u8>( *pSeek );
  }
  return 0;
}

/**
 * @brief 戦闘エフェクトID取得
 */
u8 Attribute::GetBattleEffectID( Attr attr)
{
  if( IsEnableAttribute( attr))
  {
    const Core* pCore = GetCoreData( attr);
    return pCore->battleEffectID;
  }
  return 0;
}

/**
 * @brief 有効なアトリビュートか
 * @retval true 有効なアトリビュート(attribute_id.hに定義がある)
 * @retval false それ以外
 */
bool Attribute::IsEnableAttribute( Attr attr)
{
  if( attr < ATTRIBUTE_MAX)
  {
    return true;
  }
  return false;
}

/**
 * @brief 指定のアトリビュートIDのデータを取得
 */
const Attribute::Core* Attribute::GetCoreData( Attr attr)
{
  GFL_ASSERT_MSG( m_pParameterTable, "初期化してください");
  GFL_ASSERT_MSG( attr < m_pParameterTable->header.num, "アトリビュートIDが範囲外");
  return &m_pParameterTable->core[attr];
}

GFL_NAMESPACE_END( Field )
