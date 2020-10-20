// ============================================================================
/*
 * @file JoinFestaSurveyListManager.h
 * @brief アンケートリスト管理クラス
 * @date 2015.10.22
 * @author endo_akira
 */
// ============================================================================
#if !defined( JOINFESTASURVEYLISTMANAGER_H_INCLUDE )
#define JOINFESTASURVEYLISTMANAGER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_List.h>
#include <str/include/gfl2_StrBuf.h>
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaPlayerListManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


struct JoinFestaSurveyData
{
  u32 questionMessageID;
  u32 answerMessageID;
};


typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaSurveyData>             JoinFestaSurveyDataList;
typedef gfl2::util::List<NetAppLib::JoinFestaUI::JoinFestaSurveyData>::Iterator   ItJoinFestaSurveyDataList;


class JoinFestaSurveyListManager
{
  GFL_FORBID_COPY_AND_ASSIGN( JoinFestaSurveyListManager );

private:

  enum
  {
    SURVEY_LIST_MAX = 16
  };

public:

  JoinFestaSurveyListManager( gfl2::heap::HeapBase* pHeap );
  virtual~JoinFestaSurveyListManager();

  NetAppLib::JoinFestaUI::JoinFestaSurveyDataList& GetJoinFestaSurveyDataList() { return m_JoinFestaSurveyDataList; }

  void SetupSurveyDataList( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData );

private:

  u32 convertAnswerMessageID( bool bResult1, u32 answerMessageStart, u32 answer, u16 sex );

private:

  NetAppLib::JoinFestaUI::JoinFestaSurveyDataList     m_JoinFestaSurveyDataList;

};


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)

#endif // JOINFESTASURVEYLISTMANAGER_H_INCLUDE
