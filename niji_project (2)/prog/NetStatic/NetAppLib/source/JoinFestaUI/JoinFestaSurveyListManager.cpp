// ============================================================================
/*
 * @file JoinFestaSurveyListManager.cpp
 * @brief アンケートリスト管理クラス
 * @date 2015.10.22
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaSurveyListManager.h"
#include <niji_conv_header/message/msg_jf_questiondata.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaSurveyListManager::JoinFestaSurveyListManager( gfl2::heap::HeapBase* pHeap ) :
  m_JoinFestaSurveyDataList( pHeap, SURVEY_LIST_MAX )
{
}


JoinFestaSurveyListManager::~JoinFestaSurveyListManager()
{
}


void JoinFestaSurveyListManager::SetupSurveyDataList( NetAppLib::JoinFestaUI::JoinFestaPlayerData* pData )
{
  NetAppLib::JoinFesta::PacketGameStatus1 data1;
  bool bResult1 = pData->GetPacketGameStatus1( &data1 );

  const NetAppLib::JoinFesta::PacketPersonalInfo& info = pData->GetPacketPersonalInfo();

  m_JoinFestaSurveyDataList.Erase();

  const u32 questionMessageIDTable[ SURVEY_LIST_MAX ] =
  {
    jf_question_cap_q_00,
    jf_question_cap_q_01,
    jf_question_cap_q_02,
    jf_question_cap_q_03,
    jf_question_cap_q_04,
    jf_question_cap_q_05,
    jf_question_cap_q_06,
    jf_question_cap_q_07,
    jf_question_cap_q_08,
    jf_question_cap_q_09,
    jf_question_cap_q_10,
    jf_question_cap_q_11,
    jf_question_cap_q_12,
    jf_question_cap_q_13,
    jf_question_cap_q_14,
    jf_question_cap_q_15
  };

  const u32 answerMessageIDTable[ SURVEY_LIST_MAX ] =
  {
    jf_question_sel_q_00_ans_01,
    jf_question_sel_q_01_ans_01,
    jf_question_sel_q_02_ans_01,
    jf_question_sel_q_03_ans_01,
    jf_question_sel_q_04_ans_01,
    jf_question_sel_q_05_ans_01,
    jf_question_sel_q_06_ans_01,
    jf_question_sel_q_07_ans_01,
    jf_question_sel_q_08_ans_01,
    jf_question_sel_q_09_ans_01,
    jf_question_sel_q_10_ans_01,
    jf_question_sel_q_11_ans_01,
    jf_question_sel_q_12_ans_01,
    jf_question_sel_q_13_ans_01,
    jf_question_sel_q_14_ans_01,
    jf_question_sel_q_15_ans_01
  };

  JoinFestaSurveyData data;

  for( u32 i = 0; i < SURVEY_LIST_MAX; ++i )
  {
    data.questionMessageID = questionMessageIDTable[i];
    data.answerMessageID = convertAnswerMessageID( bResult1, answerMessageIDTable[i], data1.questionnaireAnswer[i], info.sex );
    m_JoinFestaSurveyDataList.PushBack( data );
  }

}


u32 JoinFestaSurveyListManager::convertAnswerMessageID( bool bResult1, u32 answerMessageStart, u32 answer, u16 sex )
{
  u32 messageID = 0;

  u32 messageIDOffset = ( sex == 0 ) ? 0 : 12;

  if( bResult1 )
  {
    if( answer == 0 )
    {
      messageID = jf_question_sel_noans;
    }
    else
    {
      messageID = answerMessageStart + answer - 1 + messageIDOffset;
    }
  }
  else
  {
    messageID = jf_question_sel_noans;
  }

  return messageID;
}


GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
