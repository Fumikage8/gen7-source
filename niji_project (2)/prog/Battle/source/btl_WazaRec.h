#if !defined( __BTL_WAZAREC_H__ )
#define __BTL_WAZAREC_H__
//=============================================================================================
/**
 * @file  btl_WazaRec.h
 * @brief 出たワザ記録機構
 * @author  taya
 *
 * @date  2011.01.06  作成
 */
//=============================================================================================
#pragma once

//#include "waza_tool\wazadata.h"
#include "btl_common.h"

GFL_NAMESPACE_BEGIN(btl)

class WazaRec {
  GFL_FORBID_COPY_AND_ASSIGN( WazaRec );

public:

  /**
   *   定数
   */
  enum {
//    BTL_WAZAREC_TURN_MAX   = 20,
//    BTL_WAZAREC_RECORD_MAX = BTL_WAZAREC_TURN_MAX*BTL_FRONT_POKE_MAX,
    TURN_MAX   = 20,
    RECORD_MAX = TURN_MAX * TURN_MAX,
  };

  WazaRec( void ){ };
  ~WazaRec(){ };


  void   Init( void );
  void   Add( WazaID waza, u32 turn, u8 pokeID );
  void   SetEffectiveLast( void );

  bool   IsUsedWaza( WazaID waza, u32 turn ) const;
  u32    GetUsedWazaCount( WazaID waza, u32 turn ) const;
  WazaID GetPrevEffectiveWaza( u32 turn ) const;

private:
  u32   m_ptr;

  struct {
    u32     turn;
    WazaID  wazaID;
    u8      pokeID;
    u8      fEffective;
  }m_record[ RECORD_MAX ];

};  // class WAZAREC
GFL_NAMESPACE_END(btl)
#endif // __BTL_WAZAREC_H__
