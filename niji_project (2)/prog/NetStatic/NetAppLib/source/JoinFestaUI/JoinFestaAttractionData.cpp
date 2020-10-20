// ============================================================================
/*
 * @file JoinFestaAttractionData.cpp
 * @brief アトラクションデータ
 * @date 2015.11.17
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)

JoinFestaAttractionData::JoinFestaAttractionData() :
  NetAppLib::JoinFesta::JoinFestaPersonalData(),
  m_eAttractionID( JoinFestaScript::ATTRACTION_NULL ),
  m_bPassive( false )
{
}


JoinFestaAttractionData::JoinFestaAttractionData( const JoinFestaAttractionData& data ) :
  NetAppLib::JoinFesta::JoinFestaPersonalData( data ),
  m_eAttractionID( data.m_eAttractionID ),
  m_bPassive( data.m_bPassive )
{
}


JoinFestaAttractionData::~JoinFestaAttractionData()
{
}


JoinFestaAttractionData& JoinFestaAttractionData::operator = ( const JoinFestaAttractionData& data )
{
  Copy( data );
  m_eAttractionID = data.m_eAttractionID;
  m_bPassive = data.m_bPassive;
  return *this;
}


JoinFestaScript::AttractionId JoinFestaAttractionData::GetAttractionID()
{
  JoinFestaScript::AttractionId eAttractionID = m_eAttractionID;

  if( m_eAttractionID == JoinFestaScript::ATTRACTION_NULL )
  {
    if( IsOpenAttraction() )
    {
      const NetAppLib::JoinFesta::PacketAttraction& packetAttraction = GetPacketAttraction();

      eAttractionID = static_cast<JoinFestaScript::AttractionId>( packetAttraction.attractionID );
    }
  }

  return eAttractionID;
}


u32 JoinFestaAttractionData::GetTimeLimit()
{
  u32 timeLimit = 0;

  if( m_eAttractionID == JoinFestaScript::ATTRACTION_NULL && IsOpenAttraction() )
  {
    const NetAppLib::JoinFesta::PacketAttraction& packetAttraction = GetPacketAttraction();
    timeLimit = packetAttraction.time / 30;
  }
  else
  {
    NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( m_eAttractionID );

    timeLimit = static_cast<u32>( attractionData.timeLimit );
  }

  return timeLimit;
}


bool JoinFestaAttractionData::GetGroupID( u8& groupID )
{
  bool bResult = false;

  if( m_eAttractionID != JoinFestaScript::ATTRACTION_NULL )
  {
    NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( m_eAttractionID );

    groupID = attractionData.group;

    bResult = true;
  }

  return bResult;
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
