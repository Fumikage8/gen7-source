//=============================================================================================
/**
 * @file  btl_DmgAffRec.cpp
 * @brief ポケモンXY バトルシステム ダメージ相性記録機構
 * @author  taya
 *
 * @date  2011.01.27  作成
 */
//=============================================================================================

#include  "btl_DmgAffRec.h"

GFL_NAMESPACE_BEGIN(btl)

DmgAffRec::DmgAffRec( void )
{
  Init();
}

//----------------------------------------------------------------------------------
/**
 * 相性記録ワーク：初期化
 */
//----------------------------------------------------------------------------------
void DmgAffRec::Init( void )
{
  for( u32 i=0; i<GFL_NELEMS(m_affinityData); ++i )
  {
    m_affinityData[i].typeAffinity = pml::battle::TypeAffinity::TYPEAFF_NULL;
    m_affinityData[i].isNoEffectByFloatingStatus = false;
  }
}
//----------------------------------------------------------------------------------
/**
 * 相性記録ワーク：１体分追加
 *
 * @param pokeID                      記録対象ポケモンのID
 * @param aff                         タイプ相性
 * @param isNoEffectByFloatingStatus  「ふゆう」状態により無効化された場合 true
 */
//----------------------------------------------------------------------------------
void DmgAffRec::Add( u8 pokeID, BtlTypeAff aff, bool isNoEffectByFloatingStatus )
{
  if( pokeID < GFL_NELEMS(m_affinityData) )
  {
    m_affinityData[ pokeID ].typeAffinity = aff;
    m_affinityData[ pokeID ].isNoEffectByFloatingStatus = isNoEffectByFloatingStatus;
  }
  else
  {
    GFL_ASSERT(0);
  }
}
//----------------------------------------------------------------------------------
/**
 * 相性記録ワーク：１体分取得（記録が無ければASSERT停止）
 *
 * @param   rec
 * @param   pokeID
 */
//----------------------------------------------------------------------------------
BtlTypeAff DmgAffRec::Get( u8 pokeID ) const
{
  if( pokeID < GFL_NELEMS(m_affinityData) )
  {
    GFL_ASSERT(m_affinityData[pokeID].typeAffinity != pml::battle::TypeAffinity::TYPEAFF_NULL); // @check
    return m_affinityData[ pokeID ].typeAffinity;
  }
  else
  {
    GFL_ASSERT(0);
    return pml::battle::TypeAffinity::TYPEAFF_1;
  }
}

//----------------------------------------------------------------------------------
/**
 * 相性記録ワーク：１体分取得（記録がなければ pml::battle::TypeAffinity::TYPEAFF_NULL が帰る）
 *
 * @param   rec
 * @param   pokeID
 */
//----------------------------------------------------------------------------------
BtlTypeAff DmgAffRec::GetIfEnable( u8 pokeID ) const
{
  if( pokeID < GFL_NELEMS(m_affinityData) )
  {
    return m_affinityData[ pokeID ].typeAffinity;
  }
  else
  {
    GFL_ASSERT(0);
    return pml::battle::TypeAffinity::TYPEAFF_NULL;
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief 「ふゆう」状態により無効化されたかどうかを取得する
 * @param pokeID  取得対象ポケモンのID
 * @retval true   「ふゆう」状態により無効化された場合
 * @retval false  「ふゆう」状態により無効化されたわけではない場合
 */
//----------------------------------------------------------------------------------
bool DmgAffRec::IsNoEffectByFloatingStatus( u8 pokeID ) const
{
  if( pokeID < GFL_NELEMS(m_affinityData) )
  {
    return m_affinityData[ pokeID ].isNoEffectByFloatingStatus;
  }
  else
  {
    GFL_ASSERT(0);
    return false;
  }
}


GFL_NAMESPACE_END(btl)


