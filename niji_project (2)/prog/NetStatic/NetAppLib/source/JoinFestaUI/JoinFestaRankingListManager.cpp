// ============================================================================
/*
 * @file JoinFestaRankingListManager.cpp
 * @brief アンケートリスト管理クラス
 * @date 2015.10.22
 * @author endo_akira
 */
// ============================================================================
#include "NetStatic/NetAppLib/include/JoinFestaUI/JoinFestaRankingListManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include <niji_conv_header/message/msg_jf_questiondata.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFestaUI)


JoinFestaRankingListManager::JoinFestaRankingListManager( gfl2::heap::HeapBase* pHeap ) :
  m_JoinFestaRankingDataList( pHeap, RANKING_LIST_MAX )
{
}


JoinFestaRankingListManager::~JoinFestaRankingListManager()
{
}


void JoinFestaRankingListManager::SetupRankingDataList()
{
  NetAppLib::JoinFesta::JoinFestaAttractionManager* pJoinFestaAttractionManager = GFL_SINGLETON_INSTANCE( NetAppLib::JoinFesta::JoinFestaAttractionManager );

  NetAppLib::JoinFesta::JoinFestaAttractionDefine::ResultData& resultData = pJoinFestaAttractionManager->GetResultData();

  ScoreRankInfo scoreRankInfoList[ NetAppLib::JoinFesta::JoinFestaAttractionDefine::SCORE_NUM_MAX ];

  gfl2::std::MemClear( scoreRankInfoList, sizeof( scoreRankInfoList ) );

  int scoreRankInfoListSize = 0;

  for( int i = 0; i < NetAppLib::JoinFesta::JoinFestaAttractionDefine::SCORE_NUM_MAX; ++i )
  {
    if( NetAppLib::JoinFesta::JoinFestaPacketUtil::IsValidFriendKey( resultData.scoreInfoList[i].friendKey ) )
    {
      gfl2::std::MemCopy( &resultData.scoreInfoList[i], &scoreRankInfoList[ scoreRankInfoListSize ].scoreInfo, sizeof( NetAppLib::JoinFesta::JoinFestaAttractionDefine::ScoreInfo ) );

      ++scoreRankInfoListSize;
    }
  }

  // デバッグ用コード
  /*
  {
    if( scoreRankInfoListSize < 20 )
    {
      for( int i = 0 ; i < 50; ++i )
      {
        scoreRankInfoList[ scoreRankInfoListSize ].scoreInfo.score   = i + 1;
        scoreRankInfoList[ scoreRankInfoListSize ].scoreInfo.name[0] = 0x3060;
        scoreRankInfoList[ scoreRankInfoListSize ].scoreInfo.name[1] = 0x307F;
        scoreRankInfoList[ scoreRankInfoListSize ].scoreInfo.name[2] = 0x3043;
        ++scoreRankInfoListSize;
      }
    }
  }
  */

  // スコア順にソート
  sort( scoreRankInfoList, scoreRankInfoListSize );

  // 自分のスコアは必ず入れる
  {
    scoreRankInfoListSize += 1;

    scoreRankInfoListSize = ( scoreRankInfoListSize > NetAppLib::JoinFesta::JoinFestaAttractionDefine::SCORE_NUM_MAX ) ? NetAppLib::JoinFesta::JoinFestaAttractionDefine::SCORE_NUM_MAX : scoreRankInfoListSize;

    Savedata::MyStatus* pMyStatus = GFL_SINGLETON_INSTANCE( GameSys::GameManager )->GetGameData()->GetPlayerStatus();

    gfl2::str::STRCODE name[ System::STRLEN_PLAYER_NAME + System::EOM_LEN ];

    pMyStatus->GetNameString( name );

    gfl2::std::MemCopy( name, scoreRankInfoList[ scoreRankInfoListSize - 1 ].scoreInfo.name, sizeof( name ) );

    scoreRankInfoList[ scoreRankInfoListSize - 1 ].scoreInfo.score = resultData.privateScore;
    
    scoreRankInfoList[ scoreRankInfoListSize - 1 ].myScoreFlag = 1;
  }
  
  // スコア順にソート
  sort( scoreRankInfoList, scoreRankInfoListSize );

#if PM_DEBUG
  debugPrintScoreRankInfoList( scoreRankInfoList, scoreRankInfoListSize );
#endif // PM_DEBUG

  myScoreSort( scoreRankInfoList, scoreRankInfoListSize );

#if PM_DEBUG
  debugPrintScoreRankInfoList( scoreRankInfoList, scoreRankInfoListSize );
#endif // PM_DEBUG

  m_JoinFestaRankingDataList.Erase();

  for( int i = 0; i < scoreRankInfoListSize; ++i )
  {
    JoinFestaRankingData data;
    data.rank  = scoreRankInfoList[i].rank;
    data.score = scoreRankInfoList[i].scoreInfo.score;
    gfl2::str::StrNCopy( &data.name[0], scoreRankInfoList[i].scoreInfo.name, ::System::STRLEN_PLAYER_NAME + ::System::EOM_LEN );
    m_JoinFestaRankingDataList.PushBack( data );
  }
}


void JoinFestaRankingListManager::sort( ScoreRankInfo* pScoreRankInfoList, int listSize )
{
  for( int i = 0; i < listSize; ++i )
  {
    for( int j = i + 1; j < listSize; ++j )
    {
      if( pScoreRankInfoList[i].scoreInfo.score < pScoreRankInfoList[j].scoreInfo.score )
      {
        ScoreRankInfo temp;

        gfl2::std::MemCopy( &pScoreRankInfoList[i], &temp,                  sizeof( ScoreRankInfo ) );
        gfl2::std::MemCopy( &pScoreRankInfoList[j], &pScoreRankInfoList[i], sizeof( ScoreRankInfo ) );
        gfl2::std::MemCopy( &temp,                  &pScoreRankInfoList[j], sizeof( ScoreRankInfo ) );
      }
    }
  }

  s32 rank = 1;
  u16 score = pScoreRankInfoList[0].scoreInfo.score;
  s32 sameRankNum = 0;

  for( int i = 0; i < listSize; ++i )
  {
    if( score == pScoreRankInfoList[i].scoreInfo.score )
    {
      pScoreRankInfoList[i].rank = rank;

      ++sameRankNum;
    }
    else
    {
      rank += sameRankNum;

      sameRankNum = 1;

      score = pScoreRankInfoList[i].scoreInfo.score;

      pScoreRankInfoList[i].rank = rank;
    }
  }

}


void JoinFestaRankingListManager::myScoreSort( ScoreRankInfo* pScoreRankInfoList, int listSize )
{
  int myRank         = 0;
  int myRankTopIndex = -1;
  int myIndex        = -1;

  for( int i = 0; i < listSize; ++i )
  {
    if( pScoreRankInfoList[i].myScoreFlag )
    {
      myRank = pScoreRankInfoList[i].rank;
    }
  }

  for( int i = 0; i < listSize; ++i )
  {
    if( myRank == pScoreRankInfoList[i].rank )
    {
      if( myRankTopIndex < 0 )
      {
        myRankTopIndex = i;
      }

      if( pScoreRankInfoList[i].myScoreFlag )
      {
        myIndex = i;
        break;
      }
    }
  }

  if( myRank > 0 )
  {
    if( myRankTopIndex >= 0 && myIndex >= 0 )
    {
      if( myRankTopIndex != myIndex )
      {
        ScoreRankInfo temp;

        gfl2::std::MemCopy( &pScoreRankInfoList[ myRankTopIndex ], &temp,                                 sizeof( ScoreRankInfo ) );
        gfl2::std::MemCopy( &pScoreRankInfoList[ myIndex ],        &pScoreRankInfoList[ myRankTopIndex ], sizeof( ScoreRankInfo ) );
        gfl2::std::MemCopy( &temp,                                 &pScoreRankInfoList[ myIndex ],        sizeof( ScoreRankInfo ) );
      }
    }
  }

}


#if PM_DEBUG
void JoinFestaRankingListManager::debugPrintScoreRankInfoList( ScoreRankInfo* pScoreRankInfoList, int listSize )
{
  for( int i = 0; i < listSize; ++i )
  {
    GFL_PRINT( "%d : rank = %d, my = %d, score = %d \n", i, pScoreRankInfoList[i].rank, pScoreRankInfoList[i].myScoreFlag, pScoreRankInfoList[i].scoreInfo.score );
  }
}
#endif // PM_DEBUG



GFL_NAMESPACE_END(JoinFestaUI)
GFL_NAMESPACE_END(NetAppLib)
