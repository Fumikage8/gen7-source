// ============================================================================
/*
 * @file JoinFestaAttractionListManager.h
 * @brief アトラクションリスト管理クラス
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTAATTRACTIONLISTMANAGER_H_INCLUDE )
#define JOINFESTAATTRACTIONLISTMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include <math/include/gfl2_math_random.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


class JoinFestaAttractionListManager
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaAttractionListManager );

private:

  typedef struct
  {
    u16                                                                 playCount;
    u8                                                                  group;
    NetAppLib::JoinFesta::JoinFestaAttractionDefine::AppearanceLimit    eAppearanceLimit;
    JoinFestaScript::AttractionId                                       eAttractionID;
  } AttractionInfo;

public:

  JoinFestaAttractionListManager( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaAttractionListManager();


  void UpdateEntryTimeLimit();


  void UpdateList( bool bFirstUpdateFlag );
  void SortList();


  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& GetOpenList() { return m_OpenList; }
  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& GetEntryList() { return m_EntryList; }

private:

  void createAttractionInfoList();
  bool find( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& list, u8 groupID );
  JoinFestaScript::AttractionId getAttractionID( const AttractionInfo& info );
  JoinFestaScript::AttractionId getAttractionIDForOnlyType( const AttractionInfo& info );
  JoinFestaScript::AttractionId getAttractionIDForRandomType( const AttractionInfo& info );
  void updateOpenList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList );
  void updateEntryList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList );
  void copyList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList, NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& srcList );
  void sortList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList, NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& srcList );
  void timeSort( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& list );

  bool isEntry( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData );

private:

  gfl2::heap::HeapBase*                                   m_pHeap;
  gfl2::math::Random                                      m_Random;
  gfl2::util::List<AttractionInfo>                        m_AttractionInfoList;
  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList     m_OpenList;
  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList     m_EntryList;
  NetAppLib::JoinFestaUI::JoinFestaAttractionDataList     m_TempList;
  bool                                                    m_bUpdateOpenList;
  bool                                                    m_bFirstUpdateCompleteFlag;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTAATTRACTIONLISTMANAGER_H_INCLUDE
