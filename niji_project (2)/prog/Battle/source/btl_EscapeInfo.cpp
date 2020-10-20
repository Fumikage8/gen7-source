//=============================================================================================
/**
 * @file  btl_EscapeInfo.cpp
 * @brief ポケモンXY バトルシステム 逃げたクライアントの情報記録
 * @author  taya
 *
 * @date  2010.12.28  作成
 */
//=============================================================================================
#include <util/include/gfl2_std_string.h>

#include "btl_common.h"
#include "btl_mainmodule.h"
#include "btl_EscapeInfo.h"


GFL_NAMESPACE_BEGIN(btl)


EscapeInfo::EscapeInfo( void )
{
  clear();
}

EscapeInfo::~EscapeInfo()
{
}

/**
 *  初期化
 */
void EscapeInfo::Clear( void )
{
  clear();
}
/**
 *  逃げたクライアントIDを１件追加
 */
void EscapeInfo::Add( u8 clientID )
{
  for(u32 i=0; i<GFL_NELEMS(m_param.clientID); ++i)
  {
    if( m_param.clientID[i] == clientID ){ break; }
    if( m_param.clientID[i] == BTL_CLIENTID_NULL )
    {
      m_param.clientID[i] = clientID;
      m_param.count++;
      BTL_N_Printf( DBGSTR_CALC_EscInfoAdd, m_param.clientID[i], m_param.count);
      break;
    }
  }
}
/**
 *  勝敗判定
 */
BtlResult EscapeInfo::CheckWinner( const MainModule& mainModule, u8 myClientID, BtlCompetitor competitorType ) const
{
  bool fMyTeamEscaped = false;
  bool fOtherTeamEscaped = false;

  for(u32 i=0; i<m_param.count; ++i)
  {
    if( mainModule.IsFriendClientID( m_param.clientID[i], myClientID ) ){
      fMyTeamEscaped = true;
    }else{
      fOtherTeamEscaped = true;
    }
  }

  if( fMyTeamEscaped && fOtherTeamEscaped )
  {
    if( (competitorType == BTL_COMPETITOR_WILD) || (competitorType == BTL_COMPETITOR_TRAINER) ){
      return BTL_RESULT_RUN;
    }
    return BTL_RESULT_DRAW;
  }
  if( fMyTeamEscaped ){
    return BTL_RESULT_RUN;
  }
  return BTL_RESULT_RUN_ENEMY;
}


void  EscapeInfo::Copy( EscapeInfo* dst ) const
{
  dst->m_param.count = m_param.count;
  for(u32 i=0; i<GFL_NELEMS(m_param.clientID); ++i)
  {
    dst->m_param.clientID[ i ] = m_param.clientID[ i ];
  }
}


/**
 *  シリアライズ
 */
const void* EscapeInfo::Serialize( u32* size ) const
{
  *size = sizeof(m_param);
  return reinterpret_cast<const void*>( &m_param );
}

/**
 *  デシリアライズ
 */
void EscapeInfo::Deserialize( const void* data, u32 size )
{
  GFL_ASSERT(size == sizeof(m_param));
  gfl2::std::MemCopy( data, &m_param, sizeof(m_param) );
}

GFL_NAMESPACE_END(btl)


