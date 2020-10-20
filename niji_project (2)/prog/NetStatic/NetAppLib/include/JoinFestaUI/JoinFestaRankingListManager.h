// ============================================================================
/*
 * @file JoinFestaRankingListManager.h
 * @brief アンケートリスト管理クラス
 * @date 2015.10.22
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTARANKINGLISTMANAGER_H_INCLUDE )
#define JOINFESTARANKINGLISTMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include <str/include/gfl2_StrBuf.h>
#include "System/include/GameStrLen.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionDefine.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


struct JoinFestaRankingData
{
  s32                   rank;
  s32                   score;
  gfl2::str::STRCODE    name[ ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN ];
};


typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaRankingData>             JoinFestaRankingDataList;
typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaRankingData>::Iterator   ItJoinFestaRankingDataList;


class JoinFestaRankingListManager
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaRankingListManager );

private:

  enum
  {
    RANKING_LIST_MAX = 100
  };

  typedef struct
  {
    NetAppLib::JoinFesta::JoinFestaAttractionDefine::ScoreInfo scoreInfo;
    s32                                                        rank;
    s32                                                        myScoreFlag;
  } ScoreRankInfo;

public:

  JoinFestaRankingListManager( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaRankingListManager();

  NetAppLib::JoinFestaUI::JoinFestaRankingDataList& GetJoinFestaRankingDataList() { return m_JoinFestaRankingDataList; }

  void SetupRankingDataList();

private:

  void sort( ScoreRankInfo* pScoreRankInfoList, int listSize );
  void myScoreSort( ScoreRankInfo* pScoreRankInfoList, int listSize );

#if PM_DEBUG
  void debugPrintScoreRankInfoList( ScoreRankInfo* pScoreRankInfoList, int listSize );
#endif // PM_DEBUG

private:

  NetAppLib::JoinFestaUI::JoinFestaRankingDataList     m_JoinFestaRankingDataList;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTARANKINGLISTMANAGER_H_INCLUDE
