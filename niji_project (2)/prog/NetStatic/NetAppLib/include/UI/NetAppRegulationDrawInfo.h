// ============================================================================
/*
 * @file NetAppRegulationDrawInfo.h
 * @brief レギュレーション表示情報クラスです。
 * @date 2015.07.29
 */
// ============================================================================
#if !defined( NETAPPREGULATIONDRAWINFO_H_INCLUDE )
#define NETAPPREGULATIONDRAWINFO_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include "AppLib/include/Util/app_util_heap.h"
#include "Battle/Regulation/include/Regulation.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(UI)


class RegulationDrawInfo
{
  GFL_FORBID_COPY_AND_ASSIGN( RegulationDrawInfo );

public:

  enum Rank
  {
    RANK_NORMAL,
    RANK_SUPER,
    RANK_HYPER,
    RANK_MASTER
  };

  RegulationDrawInfo( gfl2::heap::HeapBase* pHeap );
  RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const Regulation::PRESET presetRegulation, Rank rank );
#if PM_DEBUG
  RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const Regulation::PRESET presetRegulation, Rank rank, bool preset );
#endif // PM_DEBUG
  RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const void* pBuffer, const u32 size );
  RegulationDrawInfo( gfl2::heap::HeapBase* pHeap, const void* pBuffer, const u32 size, int fileIndex );
  virtual~RegulationDrawInfo();

  Regulation& GetRegulation() { return m_Regulation; }

  bool IsPreset() const { return m_bIsPreset; }
  Regulation::PRESET GetPreset() const { return m_preset; }

  void SetPreset( const Regulation::PRESET preset ){ m_preset = preset; m_bIsPreset = true; }
  void ResetPreset(){ m_preset = Regulation::PRESET_MAX; m_bIsPreset = false; }

  void SetDownloaded( bool bIsDownloaded ){ m_bIsDownloaded = bIsDownloaded; }
  bool IsDownloaded() const { return m_bIsDownloaded; }

  void SetRegulation( const void* pBuffer );

  void CopyRegulation( RegulationDrawInfo* pRegulationDrawInfo );

  u32 GetBattleSystemMessageID();
  u32 GetBattleRuleMessageID();
  u32 GetBattleNameMessageID();
  u32 GetBattleRankMessageID();
  u32 GetP2PDescriptionMessageID();
  u32 GetDescriptionMessageID();

  int GetFileIndex() const { return m_FileIndex; }

private:
  gfl2::heap::HeapBase* m_pHeap;
  Regulation            m_Regulation;
  Rank                  m_eRank;
  bool                  m_bIsPreset;
  bool                  m_bIsDownloaded;
  Regulation::PRESET    m_preset;
  int                   m_FileIndex;

};


GFL_NAMESPACE_END(UI)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPPREGULATIONDRAWINFO_H_INCLUDE
