// ============================================================================
/*
 * @file JoinFestaRecordListManager.cpp
 * @brief レコードリスト管理クラス
 * @date 2015.10.22
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRecordListManager.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <niji_conv_header/message/msg_jf_playerlist.h>

#include "Savedata/include/EventWork.h"
#include "Savedata/include/MyStatus.h"

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaRecordListManager::JoinFestaRecordListManager( gfl2::heap::HeapBase* pHeap ) :
  m_JoinFestaRecordDataList( pHeap, RECORD_LIST_MAX )
{
}


JoinFestaRecordListManager::~JoinFestaRecordListManager()
{
}


void JoinFestaRecordListManager::SetupRecordDataList( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData )
{
  Savedata::Record* pRecord = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetRecordSaveData();
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetEventWork();

  NetAppLib::JoinFesta::PacketGameStatus1 data1;
  NetAppLib::JoinFesta::PacketGameStatus2 data2;
  NetAppLib::JoinFesta::PacketGameStatus3 data3;
  NetAppLib::JoinFesta::PacketGameStatus4 data4;

  bool bResult1 = pData->GetPacketGameStatus1( &data1 );
  bool bResult2 = pData->GetPacketGameStatus2( &data2 );
  bool bResult3 = pData->GetPacketGameStatus3( &data3 );
  bool bResult4 = pData->GetPacketGameStatus4( &data4 );

  bool bClearFlag_ALL = pEventWork->CheckEventFlag( SYS_FLAG_GAME_CLEAR ); // ストーリー全クリアフラグ

  m_JoinFestaRecordDataList.Erase();

  JoinFestaRecordData data;

  gfl2::std::MemClear( &data, sizeof( JoinFestaRecordData ) );

  {
    int index = 0;

    data.titleMessageID = jf_playerlist_cap_29;
    data.menuMessageID  = jf_playerlist_cap_21;
    data.rank           = pData->GetRank( JoinFestaPlayerData::RECORD_CATEGORY_BATTLE );

    setRecordParam( data, index, jf_playerlist_cap_rec_01_01, data2.record1, bResult2, true ); // RECID_COMM_BATTLE
    ++index;

    setRecordParam( data, index, jf_playerlist_cap_rec_01_02, data2.record2, bResult2, true ); // RECID_WIFI_BATTLE
    ++index;

    if( bClearFlag_ALL )
    {
      setRecordParam( data, index, jf_playerlist_cap_rec_01_03, data2.record3, bResult2, true ); // RECID_BTREE_CHALLENGE
      ++index;
    }

    setRecordParam( data, index, jf_playerlist_cap_rec_01_04, data2.record4, bResult2, true ); // RECID_BTL_ENCOUNT
    ++index;

    if( bClearFlag_ALL )
    {
      setRecordParam( data, index, jf_playerlist_cap_rec_01_05, data4.record1, bResult4, true ); // RECID_DENDOU_CNT
      ++index;
    }

    setRecordParam( data, index, jf_playerlist_cap_rec_01_06, data2.record5, bResult2, true ); // RECID_WEAK_ATTACK
    ++index;

    if( pRecord->Get( Savedata::Record::RECID_ZWAZA_CNT ) >= 1 )
    {
      setRecordParam( data, index, jf_playerlist_cap_rec_01_07, data3.record10, bResult3, true ); // RECID_ZWAZA_CNT
      ++index;
    }
    
    m_JoinFestaRecordDataList.PushBack( data );
  }

  gfl2::std::MemClear( &data, sizeof( JoinFestaRecordData ) );

  {
    data.titleMessageID = jf_playerlist_cap_30;
    data.menuMessageID  = jf_playerlist_cap_22;
    data.rank           = pData->GetRank( JoinFestaPlayerData::RECORD_CATEGORY_TRADE );

    setRecordParam( data, 0, jf_playerlist_cap_rec_02_01, data2.record6, bResult2, true ); // RECID_COMM_TRADE
    setRecordParam( data, 1, jf_playerlist_cap_rec_02_02, data2.record7, bResult2, true ); // RECID__MIRACLE_TRADE_COUNT
    setRecordParam( data, 2, jf_playerlist_cap_rec_02_03, data2.record8, bResult2, true ); // RECID_GTS_TRADE_COUNT
    setRecordParam( data, 3, jf_playerlist_cap_rec_02_04, data2.record9, bResult2, true ); // RECID_GTS_PUT

    m_JoinFestaRecordDataList.PushBack( data );
  }

  gfl2::std::MemClear( &data, sizeof( JoinFestaRecordData ) );

  {
    data.titleMessageID = jf_playerlist_cap_31;
    data.menuMessageID  = jf_playerlist_cap_23;
    data.rank           = pData->GetRank( JoinFestaPlayerData::RECORD_CATEGORY_GREED );

    setRecordParam( data, 0, jf_playerlist_cap_rec_03_01, data2.record11, bResult2, true ); // RECID_MAME_GET_CNT
    setRecordParam( data, 1, jf_playerlist_cap_rec_03_02, data3.record1, bResult3, true ); // RECID_BERRY_HARVEST
    setRecordParam( data, 2, jf_playerlist_cap_rec_03_03, data3.record2, bResult3, true ); // RECID_SHOPPING_MONEY
    setRecordParam( data, 3, jf_playerlist_cap_rec_03_04, data3.record3, bResult3, true ); // RECID_USE_BP
    setRecordParam( data, 4, jf_playerlist_cap_rec_03_05, data1.haveDressUpItemNum, bResult1, true ); // 着せ替え衣装の数

    m_JoinFestaRecordDataList.PushBack( data );
  }

  gfl2::std::MemClear( &data, sizeof( JoinFestaRecordData ) );

  {
    data.titleMessageID = jf_playerlist_cap_32;
    data.menuMessageID  = jf_playerlist_cap_24;
    data.rank           = pData->GetRank( JoinFestaPlayerData::RECORD_CATEGORY_POKEMON );

    setRecordParam( data, 0, jf_playerlist_cap_rec_04_01, data3.record4, bResult3, true ); // RECID_CAPTURE_POKE
    setRecordParam( data, 1, jf_playerlist_cap_rec_04_02, data3.record5, bResult3, true ); // RECID_TAMAGO_HATCHING
    setRecordParam( data, 2, jf_playerlist_cap_rec_04_03, data4.record2, bResult4, true ); // RECID_KAWAIGARI_CNT
    setRecordParam( data, 3, jf_playerlist_cap_rec_04_04, data3.record6, bResult3, true ); // RECID_MAME_PRESENT_CNT
    setRecordParam( data, 4, jf_playerlist_cap_rec_04_05, data3.record7, bResult3, true ); // RECID_NICKNAME
    setRecordParam( data, 5, jf_playerlist_cap_rec_04_06, data3.record8, bResult3, true ); // RECID_RIDE_POKE

    m_JoinFestaRecordDataList.PushBack( data );
  }

  gfl2::std::MemClear( &data, sizeof( JoinFestaRecordData ) );

  {
    data.titleMessageID = jf_playerlist_cap_33;
    data.menuMessageID  = jf_playerlist_cap_25;
    data.rank           = pData->GetRank( JoinFestaPlayerData::RECORD_CATEGORY_DAILY );

    setRecordParam( data, 0, jf_playerlist_cap_rec_05_01, data2.record10, bResult2, true ); // RECID_DAYMAX_EXP
    setRecordParam( data, 1, jf_playerlist_cap_rec_05_02, data4.record3, bResult4, true ); // RECID_DAYMAX_CAPTURE
    setRecordParam( data, 2, jf_playerlist_cap_rec_05_03, data4.record4, bResult4, true ); // RECID_DAYMAX_EVOLUTION
    setRecordParam( data, 3, jf_playerlist_cap_rec_05_04, data4.record5, bResult4, true ); // RECID_DAYMAX_KILL
    setRecordParam( data, 4, jf_playerlist_cap_rec_05_05, data4.record6, bResult4, true ); // RECID_DAYMAX_TRAINER_BATTLE

    m_JoinFestaRecordDataList.PushBack( data );
  }

  gfl2::std::MemClear( &data, sizeof( JoinFestaRecordData ) );

  {
    data.titleMessageID = jf_playerlist_cap_34;
    data.menuMessageID  = jf_playerlist_cap_26;
    data.rank           = pData->GetRank( JoinFestaPlayerData::RECORD_CATEGORY_CIRCLE );

    setRecordParam( data, 0, jf_playerlist_cap_rec_06_01, data1.jfRank, bResult1, true );
    setRecordParam( data, 1, jf_playerlist_cap_rec_06_02, data4.record7, bResult4, true ); // RECID_JF_ATT_CNT
    setRecordParam( data, 2, jf_playerlist_cap_rec_06_03, data4.record8, bResult4, true ); // RECID_JF_SHOP_GET_CNT
    setRecordParam( data, 3, jf_playerlist_cap_rec_06_04, data3.record11, bResult3, true ); // RECID_JF_GEST_TALK_CNT
    setRecordParam( data, 4, jf_playerlist_cap_rec_06_05, data3.record9, bResult3, true ); // RECID_JF_USE_JCOIN

    m_JoinFestaRecordDataList.PushBack( data );
  }

  gfl2::std::MemClear( &data, sizeof( JoinFestaRecordData ) );

  {
    data.titleMessageID = jf_playerlist_cap_35;
    data.menuMessageID  = jf_playerlist_cap_27;
    data.rank           = pData->GetRank( JoinFestaPlayerData::RECORD_CATEGORY_VARIETY );

    setRecordParam( data, 0, jf_playerlist_cap_rec_07_01, data4.record9,  bResult4, true ); // RECID_RARE_COLOUR_ENC_COUNT
    setRecordParam( data, 1, jf_playerlist_cap_rec_07_02, data4.record10, bResult4, true ); // RECID_IDKUJI_ATARI_CNT
    setRecordParam( data, 2, jf_playerlist_cap_rec_07_03, data4.record11, bResult4, true ); // RECID_POINTUP_USE
    setRecordParam( data, 3, jf_playerlist_cap_rec_07_04, data4.record12, bResult4, true ); // RECID_KISEKAE_CNT
    setRecordParam( data, 4, jf_playerlist_cap_rec_07_05, data4.record13, bResult4, true ); // RECID_QR_SCAN
    setRecordParam( data, 5, jf_playerlist_cap_rec_07_06, data4.record14, bResult4, true ); // RECID_CAMERA_CNT

    // momiji追加
    if( pData->GetRomVersion() == VERSION_SUN2 ||
        pData->GetRomVersion() == VERSION_MOON2 )
    {
      int index = 6;
      // momijiの人はレコードが追加されている
      if( pRecord->Get( Savedata::Record::RECID_LUNASOL_BESTSCORE ) >= 1 )
      {
        setRecordParam( data, index, jf_playerlist_cap_rec_07_07, data4.record15, bResult4, true ); // RECID_LUNASOL_BESTSCORE
        index++;
      }
      if( pRecord->Get( Savedata::Record::RECID_MANTAIN_SURF_BEST_BP ) >= 1 )
      {
        setRecordParam( data, index, jf_playerlist_cap_rec_07_08, data4.record16, bResult4, true ); // RECID_MANTAIN_SURF_BEST_BP
      }
    }

    

    m_JoinFestaRecordDataList.PushBack( data );
  }
}


void JoinFestaRecordListManager::setRecordParam( JoinFestaRecordData& data, u32 index, u32 messageID, u32 param, bool bEnable, bool bVisible )
{
  data.recordMenuMessageIDTable[ index ] = messageID;
  data.recordParams[ index ]             = param;
  data.bEnable[ index ]                  = bEnable;
  data.bVisible[ index ]                 = bVisible;
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
