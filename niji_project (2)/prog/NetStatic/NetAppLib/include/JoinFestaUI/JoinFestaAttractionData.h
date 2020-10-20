// ============================================================================
/*
 * @file JoinFestaAttractionData.h
 * @brief アトラクションデータ
 * @date 2015.11.17
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONDATA_H_INCLUDE )
#define JOINFESTAATTRACTIONDATA_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "niji_reference_files/script/JoinFestaPawnDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaAttractionData : public NetAppLib::JoinFesta::JoinFestaPersonalData
{
public:
  JoinFestaAttractionData();
  JoinFestaAttractionData( const JoinFestaAttractionData& data );
  virtual~JoinFestaAttractionData();

  JoinFestaAttractionData& operator = ( const JoinFestaAttractionData& data );

  void SetAttractionID( JoinFestaScript::AttractionId eAttractionID ) { m_eAttractionID = eAttractionID; }
  JoinFestaScript::AttractionId GetAttractionID();

  u32 GetTimeLimit();

  bool GetGroupID( u8& groupID );

  void SetPassive( bool bPassive ) { m_bPassive = bPassive; }
  bool IsPassive() const { return m_bPassive; }

private:

  JoinFestaScript::AttractionId     m_eAttractionID;
  bool                              m_bPassive;

};


typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaAttractionData>             JoinFestaAttractionDataList;
typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaAttractionData>::Iterator   ItJoinFestaAttractionDataList;


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAATTRACTIONDATA_H_INCLUDE
