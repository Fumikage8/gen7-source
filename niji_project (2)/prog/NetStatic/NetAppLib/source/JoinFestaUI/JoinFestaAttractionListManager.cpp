// ============================================================================
/*
 * @file JoinFestaAttractionListManager.cpp
 * @brief アトラクションリスト管理クラス
 * @date 2015.10.27
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaAttractionListManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"

#include "Savedata/include/EventWork.h"
#include "Savedata/include/Record.h" // レコードID
#include "Savedata/include/MyStatus.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


static const u32 ATTRACTION_OPEN_LIST_MAX = JoinFestaScript::ATTRACTION_NUM;

static const u32 ATTRACTION_ENTRY_LIST_MAX = NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX * NetAppLib::JoinFesta::JoinFestaPersonalDataManager::LIST_MAX;

static const u32 DRAW_LIST_MAX = 16;


JoinFestaAttractionListManager::JoinFestaAttractionListManager( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_Random(),
  m_AttractionInfoList( pHeap, JoinFestaScript::ATTRACTION_NUM ),
  m_OpenList( pHeap, ATTRACTION_OPEN_LIST_MAX ),
  m_EntryList( pHeap, ATTRACTION_ENTRY_LIST_MAX ),
  m_TempList( pHeap, ATTRACTION_ENTRY_LIST_MAX ),
  m_bUpdateOpenList( false ),
  m_bFirstUpdateCompleteFlag( false )
{
}


JoinFestaAttractionListManager::~JoinFestaAttractionListManager()
{
}


void JoinFestaAttractionListManager::UpdateEntryTimeLimit()
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  for( NetAppLib::JoinFestaUI::ItJoinFestaAttractionDataList it = m_EntryList.Begin(); it != m_EntryList.End(); ++it )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData = pJoinFestaPersonalDataManager->GetPersonalData( it->GetJoinFestaFriendKey() );
    
    if( pJoinFestaPersonalData )
    {
      NetAppLib::JoinFesta::PacketAttraction* pPacketAttraction = it->GetPacketAttractionPtr();
      if( pPacketAttraction )
      {
        if( pJoinFestaPersonalData->IsOpenAttraction() && pJoinFestaPersonalData->IsSameAttraction( *pPacketAttraction ) )
        {//アトラクション継続中
          pPacketAttraction->time = pJoinFestaPersonalData->GetPacketAttraction().time;
        }
        else
        {//アトラクション終わっている
          pPacketAttraction->time = 0;
        }
      }
    }
  }
}


void JoinFestaAttractionListManager::UpdateList( bool bFirstUpdateFlag )
{
  if( bFirstUpdateFlag )
  {
    if( m_bFirstUpdateCompleteFlag )
    {
      return;
    }
  }

  m_Random.Initialize();
  createAttractionInfoList();
  if( !m_bUpdateOpenList )
  {
    updateOpenList( m_OpenList );
    m_bUpdateOpenList = true;
  }
  updateEntryList( m_EntryList );

  m_bFirstUpdateCompleteFlag = true;
}


void JoinFestaAttractionListManager::SortList()
{
  copyList( m_TempList, m_EntryList );
  sortList( m_EntryList, m_TempList );
}


void JoinFestaAttractionListManager::createAttractionInfoList()
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

  Savedata::JoinFestaDataSave* pJoinFestaDataSave = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetJoinFestaData();
  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();
  Savedata::MyStatus* myStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();

  m_AttractionInfoList.Erase();

  u32 attractionNum = static_cast<u32>( JoinFestaScript::ATTRACTION_NUM );
  const bool isGameClear = pEventWork->CheckEventFlag( SYS_FLAG_GAME_CLEAR );

  for( u32 i = 0; i < attractionNum; ++i )
  {
    JoinFestaScript::AttractionId eAttractionID = static_cast<JoinFestaScript::AttractionId>(i);

    NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData attractionData = pJoinFestaAttractionManager->GetAttractionData( eAttractionID );

    bool isPushOk = true; // リストに加えて良いかフラグ

    // クリア後解禁かどうかチェック
    if( attractionData.isAfterClear == 1 && isGameClear == false  )
    {
      isPushOk = false;
    }

    // 参加回数開放を超えていないと表示しない
    if( !(pRecord->Get( Savedata::Record::RECID_JF_ATT_CNT ) >= attractionData.openPlayCount) )
    {
      isPushOk = false;
    }

    // 言語レッスンで自分の言語コードと同じのは表示しない
    {
      const u8 languageId = myStatus->GetPokeLanguageId();

      if( 
        ( languageId == POKEMON_LANG_JAPAN && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_JP ) ||
        ( languageId == POKEMON_LANG_ENGLISH && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_EN ) ||
        ( languageId == POKEMON_LANG_FRANCE && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_FR ) ||
        ( languageId == POKEMON_LANG_ITALY && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_IT ) ||
        ( languageId == POKEMON_LANG_GERMANY && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_GE ) ||
        ( languageId == POKEMON_LANG_SPAIN && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_SP ) ||
        ( languageId == POKEMON_LANG_KOREA && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_KR ) ||
        ( languageId == POKEMON_LANG_CHINA && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_CT ) ||
        ( languageId == POKEMON_LANG_TAIWAN && eAttractionID == JoinFestaScript::ATTRACTION_LANGUAGE_LESSON_CT )
        )
      {
        isPushOk = false;
      }
    }


    if( isPushOk == true )
    {
      AttractionInfo info;

      info.eAttractionID    = eAttractionID;
      info.eAppearanceLimit = attractionData.appearanceLimit;
      info.group            = attractionData.group;
      info.playCount        = pJoinFestaDataSave->GetPlayCount( eAttractionID );

      m_AttractionInfoList.PushBack( info );
    }
  }
}


bool JoinFestaAttractionListManager::find( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& list, u8 groupID )
{
  bool bFind = false;

  for( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList::Iterator it = list.Begin(); it != list.End(); ++it )
  {
    u8 outGroupID = 0;
    
    bool bResult = it->GetGroupID( outGroupID );
    
    if( bResult )
    {
      if( outGroupID == groupID )
      {
        bFind = true;
      }
    }
  }

  return bFind;
}


JoinFestaScript::AttractionId JoinFestaAttractionListManager::getAttractionID( const AttractionInfo& info )
{
  JoinFestaScript::AttractionId eAttractionID = JoinFestaScript::ATTRACTION_NULL;

  switch( info.eAppearanceLimit )
  {
    case NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_LIMIT_ONLY:
    {
      eAttractionID = getAttractionIDForOnlyType( info );
    }
    break;

    case NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_LIMIT_RND:
    {
      eAttractionID = getAttractionIDForRandomType( info );
    }
    break;

    case NetAppLib::JoinFesta::JoinFestaAttractionDefine::APPEARANCE_LIMIT_NONE:
    {
      eAttractionID = info.eAttractionID;
    }
    break;
  }

  return eAttractionID;
}


JoinFestaScript::AttractionId JoinFestaAttractionListManager::getAttractionIDForOnlyType( const AttractionInfo& info )
{
  AttractionInfo temp = info;

  for( gfl2::util::List<AttractionInfo>::Iterator it = m_AttractionInfoList.Begin(); it != m_AttractionInfoList.End(); ++it )
  {
    if( temp.group == it->group && temp.eAppearanceLimit == it->eAppearanceLimit )
    {
      if( temp.playCount > it->playCount )
      {
        temp = (*it);
      }
    }
  }

  return temp.eAttractionID;
}


JoinFestaScript::AttractionId JoinFestaAttractionListManager::getAttractionIDForRandomType( const AttractionInfo& info )
{
  JoinFestaScript::AttractionId eAttractionID = JoinFestaScript::ATTRACTION_NULL;

  u32 size = 0;

  for( gfl2::util::List<AttractionInfo>::Iterator it = m_AttractionInfoList.Begin(); it != m_AttractionInfoList.End(); ++it )
  {
    if( info.group == it->group && info.eAppearanceLimit == it->eAppearanceLimit )
    {
      ++size;
    }
  }

  if( size > 0 )
  {
    AttractionInfo* pTempBuffer = GFL_NEW_ARRAY( m_pHeap->GetLowerHandle() ) AttractionInfo[ size ];

    u32 i = 0;

    for( gfl2::util::List<AttractionInfo>::Iterator it = m_AttractionInfoList.Begin(); it != m_AttractionInfoList.End(); ++it )
    {
      if( info.group == it->group && info.eAppearanceLimit == it->eAppearanceLimit )
      {
        pTempBuffer[i] = (*it);

        ++i;
      }
    }

    u32 randIndex = m_Random.Next( size );

    eAttractionID = pTempBuffer[ randIndex ].eAttractionID;

    GFL_SAFE_DELETE_ARRAY( pTempBuffer );
  }

  return eAttractionID;
}


void JoinFestaAttractionListManager::updateOpenList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList )
{
  destList.Erase();

  for( gfl2::util::List<AttractionInfo>::Iterator it = m_AttractionInfoList.Begin(); it != m_AttractionInfoList.End(); ++it )
  {
    if( !find( destList, it->group ) )
    {
      JoinFestaScript::AttractionId eAttractionID = getAttractionID( (*it) );

      if( eAttractionID != JoinFestaScript::ATTRACTION_NULL )
      {
        NetAppLib::JoinFestaUI::JoinFestaAttractionData temp;

        temp.SetAttractionID( eAttractionID );

        destList.PushBack( temp );
      }
    }
  }
}


void JoinFestaAttractionListManager::updateEntryList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList )
{
  copyList( m_TempList, destList );

  destList.Erase();

  NetAppLib::JoinFestaUI::JoinFestaAttractionData temp;

  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pJoinFestaPersonalDataManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaPersonalDataManager );

  for( u32 i = 0; i < NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION_MAX; ++i )
  {
    NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pList = pJoinFestaPersonalDataManager->GetPersonalDataList( static_cast<NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION>(i) );

    if( pList != NULL )
    {
      if( pList->Size() > 0 )
      {
        for( NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalListIterator src = pList->Begin(); src != pList->End(); ++src )
        {
          if( (*src) && NetAppLib::JoinFesta::JoinFestaPacketUtil::CheckAttractionJoinEnable( (*src) ) && !isEntry( (*src) ) )
          {
            temp.Copy( *(*src) );
            temp.SetAttractionID( JoinFestaScript::ATTRACTION_NULL );
            temp.SetPassive( false );

            for( ItJoinFestaAttractionDataList it = m_TempList.Begin(); it != m_TempList.End(); ++it )
            {
              if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsSameFriendKey( (*it).GetJoinFestaFriendKey(), (*src)->GetJoinFestaFriendKey() ) )
              {
                temp.SetPassive( (*it).IsPassive() );
              }
            }

            destList.PushBack( temp );
          }
        }
      }
    }
  }

  timeSort( destList );
}


bool JoinFestaAttractionListManager::isEntry( NetAppLib::JoinFesta::JoinFestaPersonalData* pJoinFestaPersonalData )
{
  if( pJoinFestaPersonalData->IsOpenAttraction() == false )
  {
    return false;
  }

  bool bResult = false;

  const NetAppLib::JoinFesta::PacketAttraction& packetAttraction = pJoinFestaPersonalData->GetPacketAttraction();

  for( ItJoinFestaAttractionDataList it = m_EntryList.Begin(); it != m_EntryList.End(); ++it )
  {
    if( it->IsSameAttraction( packetAttraction ) )
    {
      bResult = true;
      break;
    }
  }

  return bResult;
}


void JoinFestaAttractionListManager::copyList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList, NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& srcList )
{
  destList.Erase();

  for( ItJoinFestaAttractionDataList it = srcList.Begin(); it != srcList.End(); ++it )
  {
    destList.PushBack( *it );
  }
}


void JoinFestaAttractionListManager::sortList( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& destList, NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& srcList )
{
  destList.Erase();

  // フラグ操作
  for( ItJoinFestaAttractionDataList it = srcList.Begin(); it != srcList.End(); ++it )
  {
    if( (*it).IsPassive() && (*it).IsOnline() )
    {
      (*it).SetPassive( false );
    }
    else
    if( !(*it).IsOnline() )
    {
      (*it).SetPassive( true );
    }
  }

  // 以下の順番に追加する

  ItJoinFestaAttractionDataList it = srcList.Begin();

  // オンライン状態の物を優先
  while( it != srcList.End() )
  {
    if( destList.Size() >= DRAW_LIST_MAX )
    {
      break;
    }

    if( (*it).IsOnline() && !(*it).IsPassive() )
    {
      destList.PushBack( (*it) );
      it = srcList.Erase( it );
    }
    else
    {
      ++it;
    }
  }

  // @fix : [niji] System #1166 アトラクションリストのオフラインの人はリストに出さない
  /*
  it = srcList.Begin();

  while( it != srcList.End() )
  {
    destList.PushBack( (*it) );
    it = srcList.Erase( it );
  }
  */

}


void JoinFestaAttractionListManager::timeSort( NetAppLib::JoinFestaUI::JoinFestaAttractionDataList& list )
{
  for( NetAppLib::JoinFestaUI::ItJoinFestaAttractionDataList it1 = list.Begin(); it1 != list.End(); ++it1 )
  {
    NetAppLib::JoinFesta::PacketAttraction* pPacketAttraction1 = it1->GetPacketAttractionPtr();
    u32 time1 = ( pPacketAttraction1 ) ? pPacketAttraction1->time : 0;

    NetAppLib::JoinFestaUI::ItJoinFestaAttractionDataList it2 = it1;
    ++it2;

    for( ; it2 != list.End(); ++it2 )
    {
      NetAppLib::JoinFesta::PacketAttraction* pPacketAttraction2 = it2->GetPacketAttractionPtr();
      u32 time2 = ( pPacketAttraction2 ) ? pPacketAttraction2->time : 0;

      if( time1 < time2 )
      {
        NetAppLib::JoinFestaUI::JoinFestaAttractionData temp = (*it1);

        (*it1) = (*it2);
        (*it2) = temp;

        time1 = time2;
      }
    }
  }
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
