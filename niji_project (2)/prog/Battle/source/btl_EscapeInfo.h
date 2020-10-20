#if !defined( __BTL_ESCAPEINFO_H__ )
#define __BTL_ESCAPEINFO_H__
//=============================================================================================
/**
 * @file  btl_EscapeInfo.h
 * @brief ポケモンXY バトルシステム 逃げたクライアントの情報記録
 * @author  taya
 *
 * @date  2010.12.28  作成
 */
//=============================================================================================
#pragma once

#include "btl_common.h"

GFL_NAMESPACE_BEGIN(btl)

class EscapeInfo {
  GFL_FORBID_COPY_AND_ASSIGN( EscapeInfo );

public:
  /**
   *  constructor / destructor
   */
  EscapeInfo( void );
  ~EscapeInfo();

  /**
   *  初期化
   */
  void Clear( void );

  /**
   *  逃げたクライアントIDを１件追加
   */
  void Add( u8 clientID );

  /**
   *  逃げたクライアントの総数を取得
   */
  u32 GetCount( void ) const { return m_param.count; }

  /**
   *  勝敗判定
   */
  BtlResult CheckWinner( const MainModule& mainModule, u8 myClientID, BtlCompetitor competitorType ) const;

  /**
   *  コピー
   */
  void  Copy( EscapeInfo* dst ) const;

  /**
   *  シリアライズ
   */
   const void* Serialize( u32* size ) const;


  /**
   *  デシリアライズ
   */
   void Deserialize( const void* data, u32 size );



private:
  struct {
    u32  count;
    u8   clientID[ BTL_CLIENT_MAX ];
  }m_param;

  inline void clear( void )
  {
    m_param.count = 0;
    for(u32 i=0; i<GFL_NELEMS(m_param.clientID); ++i)
    {
      m_param.clientID[ i ] = BTL_CLIENTID_NULL;
    }
  }
};


GFL_NAMESPACE_END(btl)

#endif // __BTL_ESCAPEINFO_H__
