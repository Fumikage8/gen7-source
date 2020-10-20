#if !defined( __BTL_AFFCOUNTER_H__ )
#define __BTL_AFFCOUNTER_H__
//=============================================================================================
/**
 * @file    btl_AffCounter.h
 * @brief   ポケモンXY バトルシステム ワザ相性カウンタ（バトル検定用）
 * @author  taya
 *
 * @date  2011.01.07  作成
 */
//=============================================================================================
#pragma once

#include  "btl_common.h"
#include  "btl_PokeParam.h"

GFL_NAMESPACE_BEGIN(btl)

class AffCounter {
  GFL_FORBID_COPY_AND_ASSIGN( AffCounter );

public:
  /**
   *  定数
   */
  enum {
    COUNTER_MAX = 9999,   // カウンタ最大値
  };

  AffCounter( void );
  ~AffCounter(){ };

  void    Clear( void );
  void    CountUp( const MainModule& mainModule, const BTL_POKEPARAM* attacker, const BTL_POKEPARAM* defender, BtlTypeAff affinity );

  u16   GetVoid( void ) const             { return m_putVoid;           };
  u16   GetAdvantage( void ) const        { return m_putAdvantage;      };
  u16   GetDisadvantage( void ) const     { return m_putDisadvantage;   };
  u16   GetRecvVoid( void ) const         { return m_recvVoid;          };
  u16   GetRecvAdvantage( void ) const    { return m_recvAdvantage;     };
  u16   GetRecvDisadvantage( void ) const { return m_recvDisadvantage;  };

private:

  /**
   *  ワザ相性カウンタ（バトル検定用）
   */
  u16   m_putVoid;          ///< 「効果なし」を出した
  u16   m_putAdvantage;     ///< 「効果ばつぐん」を出した
  u16   m_putDisadvantage;  ///< 「効果いまいち」を出した
  u16   m_recvVoid;         ///< 「効果無し」を受けた
  u16   m_recvAdvantage;    ///< 「効果ばつぐん」を受けた
  u16   m_recvDisadvantage; ///< 「効果いまいち」を受けた

}; // class AffCounter

GFL_NAMESPACE_END(btl)
#endif // __BTL_AFFCOUNTER_H__
