// ============================================================================
/*
 * @file NetAppRegulationDrawInfo.cpp
 * @brief レギュレーション表示情報クラスです。
 * @date 2015.07.29
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/UI/NetAppRegulationDrawInfo.h"
#include <niji_conv_header/message/msg_regulation.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)


RegulationDrawInfo::RegulationDrawInfo( gfl2::heap::HeapBase* pHeap ) :
  m_pHeap( pHeap ),
  m_Regulation( pHeap ),
  m_eRank( RANK_NORMAL ),
  m_bIsPreset( false ),
  m_bIsDownloaded( false ),
  m_preset( Regulation::PRESET_MAX ),
  m_FileIndex( -1 )
{
}


RegulationDrawInfo::RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const Regulation::PRESET presetRegulation, Rank rank ) :
  m_pHeap( pHeap ),
  m_Regulation( pHeap ),
  m_eRank( rank ),
  m_bIsPreset( true ),
  m_bIsDownloaded( false ),
  m_preset( presetRegulation ),
  m_FileIndex( -1 )
{
  m_Regulation.LoadData( presetRegulation );
}


#if PM_DEBUG
RegulationDrawInfo::RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const Regulation::PRESET presetRegulation, Rank rank, bool preset ) :
  m_pHeap( pHeap ),
  m_Regulation( pHeap ),
  m_eRank( rank ),
  m_bIsPreset( preset ),
  m_bIsDownloaded( false ),
  m_preset( Regulation::PRESET_MAX ),
  m_FileIndex( -1 )
{
  m_Regulation.LoadData( presetRegulation );
}
#endif // PM_DEBUG


RegulationDrawInfo::RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const void* pBuffer, const u32 size ) :
  m_pHeap( pHeap ),
  m_Regulation( pHeap ),
  m_eRank( RANK_NORMAL ),
  m_bIsPreset( false ),
  m_bIsDownloaded( false ),
  m_preset( Regulation::PRESET_MAX ),
  m_FileIndex( -1 )
{
  m_Regulation.SetData( pBuffer, size );
}


RegulationDrawInfo::RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const void* pBuffer, const u32 size, int fileIndex ) :
  m_pHeap( pHeap ),
  m_Regulation( pHeap ),
  m_eRank( RANK_NORMAL ),
  m_bIsPreset( false ),
  m_bIsDownloaded( false ),
  m_preset( Regulation::PRESET_MAX ),
  m_FileIndex( fileIndex )
{
  m_Regulation.SetData( pBuffer, size );
}


RegulationDrawInfo::~RegulationDrawInfo()
{
}

void RegulationDrawInfo::SetRegulation( const void* pBuffer )
{
  m_Regulation.Deserialize( pBuffer, m_Regulation.GetSerializeDataSize() );
  m_eRank = RANK_NORMAL;
  m_bIsPreset = false;
  m_bIsDownloaded = false;
  m_preset = Regulation::PRESET_MAX;
}

void RegulationDrawInfo::CopyRegulation( RegulationDrawInfo* pRegulationDrawInfo )
{
  if( pRegulationDrawInfo )
  {
    u32 dataSize = pRegulationDrawInfo->GetRegulation().GetSerializeDataSize();
    u8* pData = GFL_NEW_ARRAY( m_pHeap ) u8[ dataSize ];
    pRegulationDrawInfo->GetRegulation().Serialize( pData );
    m_Regulation.Deserialize( pData, dataSize );
    GFL_SAFE_DELETE_ARRAY( pData );

    m_bIsPreset = pRegulationDrawInfo->IsPreset();
    m_bIsDownloaded = pRegulationDrawInfo->IsDownloaded();
    m_preset = pRegulationDrawInfo->GetPreset();
  }
}


u32 RegulationDrawInfo::GetBattleSystemMessageID()
{
  u32 messageID = msg_reg_cap_16; // シングルバトル

  Regulation::BATTLE_RULE battleRule = m_Regulation.GetBattleRule();

  switch( battleRule )
  {
    case Regulation::BATTLE_RULE_SINGLE: messageID = msg_reg_cap_16; break; // シングルバトル
    case Regulation::BATTLE_RULE_DOUBLE: messageID = msg_reg_cap_17; break; // ダブルバトル
    case Regulation::BATTLE_RULE_ROYAL:  messageID = msg_reg_cap_19; break; // バトルロイヤル
    case Regulation::BATTLE_RULE_MULTI:  messageID = msg_reg_cap_18; break; // マルチバトル
  }

  return messageID;
}


u32 RegulationDrawInfo::GetBattleRuleMessageID()
{
  u32 messageID = msg_reg_cap_20; // ノーマルルール

  Regulation::LEVEL_RANGE_TYPE levelRange = m_Regulation.GetLevelRangeType();

  switch( levelRange )
  {
    case Regulation::LEVEL_RANGE_SAME:      messageID = msg_reg_cap_20; break; // ノーマルルール
    case Regulation::LEVEL_RANGE_DRAG_DOWN: messageID = msg_reg_cap_21; break; // フラットルール
    case Regulation::LEVEL_RANGE_NORMAL:    messageID = msg_reg_cap_22; break; // 制限無し
  }

  return messageID;
}


u32 RegulationDrawInfo::GetBattleNameMessageID()
{
  u32 messageID = msg_reg_cap_23; // バトルツリー

  Regulation::BATTLE_RULE battleRule = m_Regulation.GetBattleRule();

  switch( battleRule )
  {
    case Regulation::BATTLE_RULE_ROYAL: messageID = msg_reg_cap_24; break; // バトルロイヤル施設名
  }

  return messageID;
}


u32 RegulationDrawInfo::GetBattleRankMessageID()
{
  u32 messageID = msg_reg_cap_29; // シングル

  Regulation::BATTLE_RULE battleRule = m_Regulation.GetBattleRule();
  
  switch( battleRule )
  {
    case Regulation::BATTLE_RULE_SINGLE:
      switch( m_eRank )
      {
        case NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL: messageID = msg_reg_cap_29; break; // シングル
        case NetAppLib::UI::RegulationDrawInfo::RANK_SUPER:  messageID = msg_reg_cap_32; break; // スーパーシングル
      }
    break;
    
    case Regulation::BATTLE_RULE_DOUBLE:
      switch( m_eRank )
      {
        case NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL: messageID = msg_reg_cap_30; break; // ダブル
        case NetAppLib::UI::RegulationDrawInfo::RANK_SUPER:  messageID = msg_reg_cap_33; break; // スーパーダブル
      }
    break;
    
    case Regulation::BATTLE_RULE_ROYAL:
      switch( m_eRank )
      {
        case NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL: messageID = msg_reg_cap_35; break; // ノーマルランク
        case NetAppLib::UI::RegulationDrawInfo::RANK_SUPER:  messageID = msg_reg_cap_36; break; // スーパーランク
        case NetAppLib::UI::RegulationDrawInfo::RANK_HYPER:  messageID = msg_reg_cap_37; break; // ハイパーランク
        case NetAppLib::UI::RegulationDrawInfo::RANK_MASTER: messageID = msg_reg_cap_38; break; // マスターランク
      }
    break;
    
    case Regulation::BATTLE_RULE_MULTI:
      switch( m_eRank )
      {
        case NetAppLib::UI::RegulationDrawInfo::RANK_NORMAL: messageID = msg_reg_cap_31; break; // マルチ
        case NetAppLib::UI::RegulationDrawInfo::RANK_SUPER:  messageID = msg_reg_cap_34; break; // スーパーマルチ
      }
    break;
  }

  return messageID;
}


u32 RegulationDrawInfo::GetP2PDescriptionMessageID()
{
  u32 messageID = msg_reg_exp_01; // ノーマルルールの説明文

  Regulation::LEVEL_RANGE_TYPE levelRange = m_Regulation.GetLevelRangeType();

  switch( levelRange )
  {
    case Regulation::LEVEL_RANGE_SAME:      messageID = msg_reg_exp_01; break; // ノーマルルールの説明文
    case Regulation::LEVEL_RANGE_DRAG_DOWN: messageID = msg_reg_exp_02; break; // フラットルールの説明文
    case Regulation::LEVEL_RANGE_NORMAL:    messageID = msg_reg_exp_03; break; // 制限無しの説明文
  }

  return messageID;
}


u32 RegulationDrawInfo::GetDescriptionMessageID()
{
  u32 messageID = msg_reg_exp_04; // バトルツリーの説明文

  Regulation::BATTLE_RULE battleRule = m_Regulation.GetBattleRule();

  switch( battleRule )
  {
    case Regulation::BATTLE_RULE_ROYAL: messageID = msg_reg_exp_05; break; // バトルロイヤルの説明文
  }

  return messageID;
}


GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)
