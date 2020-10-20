// ============================================================================
/*
 * @file JoinFestaPlayerData.cpp
 * @brief プレイヤーデータ
 * @date 2015.10.19
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerData.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaPlayerData::JoinFestaPlayerData() :
  NetAppLib::JoinFesta::JoinFestaPersonalData(),
  m_BlackPlayerCoreData(),
  m_RecordRank(),
  m_eType( TYPE_GUEST ),
  m_bSelect( false ),
  m_bPassive( false ),
  /*  BFC  */
  m_pCoreParam( NULL ),
  m_bRecentPartner( false ),
  /*  photo  */
  m_PhotoDataStatus( PhotoData_NotAvailable )
{
}


JoinFestaPlayerData::JoinFestaPlayerData( const JoinFestaPlayerData& data ) :
  NetAppLib::JoinFesta::JoinFestaPersonalData( data ),
  m_BlackPlayerCoreData( data.m_BlackPlayerCoreData ),
  m_RecordRank( data.m_RecordRank ),
  m_eType( data.m_eType ),
  m_bSelect( data.m_bSelect ),
  m_bPassive( data.m_bPassive ),
  /*  BFC  */
  m_pCoreParam( data.m_pCoreParam ),
  m_bRecentPartner( data.m_bRecentPartner ),
  /*  photo  */
  m_PhotoDataStatus( data.m_PhotoDataStatus )
{
}


JoinFestaPlayerData::~JoinFestaPlayerData()
{
}


JoinFestaPlayerData& JoinFestaPlayerData::operator = ( const JoinFestaPlayerData& data )
{
  Copy( data );
  m_BlackPlayerCoreData = data.m_BlackPlayerCoreData;
  m_RecordRank = data.m_RecordRank;
  m_eType = data.m_eType;
  m_bSelect = data.m_bSelect;
  m_bPassive = data.m_bPassive;
  /*  BFC  */
  m_pCoreParam      = data.m_pCoreParam;
  m_bRecentPartner  = data.m_bRecentPartner;
  /*  photo  */
  m_PhotoDataStatus = data.m_PhotoDataStatus;
  return *this;
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
