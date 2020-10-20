//=============================================================================================
/**
 * @file  btl_DmgAffRec.h
 * @brief ポケモンXY バトルシステム ダメージ相性記録機構
 * @author  taya
 *
 * @date  2011.01.27  作成
 */
//=============================================================================================
#pragma once
#if !defined( __BTL_DMGAFFREC_H__ )
#define __BTL_DMGAFFREC_H__

#include  "btl_common.h"


GFL_NAMESPACE_BEGIN(btl)


class DmgAffRec 
{
  GFL_FORBID_COPY_AND_ASSIGN(DmgAffRec);

public:

  DmgAffRec( void );
  ~DmgAffRec(){ };

  void        Init( void );
  void        Add( u8 pokeID, BtlTypeAff aff, bool isNoEffectByFloatingStatus );
  BtlTypeAff  Get( u8 pokeID ) const;
  BtlTypeAff  GetIfEnable( u8 pokeID ) const;
  bool        IsNoEffectByFloatingStatus( u8 pokeID ) const;



private:

  struct AffinityData
  {
    BtlTypeAff typeAffinity;                // タイプ相性
    bool       isNoEffectByFloatingStatus;  // 「ふゆう」状態によって無効化された場合 true
  };

  AffinityData m_affinityData[ BTL_POKEID_MAX ];

};


GFL_NAMESPACE_END(btl)

#endif // __BTL_DMGAFFREC_H__
