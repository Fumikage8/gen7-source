//==============================================================================
/**
 * @file	JoinFestaAttractionManager.cpp
 * @brief	ジョインフェスタアトラクションマネージャ
 * @author	ichiraku_katsuhiko
 * @date	2015/11/09　12:47
 */
// =============================================================================
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionManager.h"

// ライブラリ
#include <debug/include/gfl2_Assert.h>
#include <debug/include/gfl2_DebugPrint.h>

#include <Sound/include/Sound.h>

// niji
#include "GameSys/include/GameManager.h"
#include "GameSys/include/GameData.h"
#include "System/include/HeapDefine.h"
#include "System/include/GflUse.h"

// JoinFesta
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketUtil.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPacketManager.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaAttraction/JoinFestaAttractionUtil.h"

// Field
#include "Field/FieldStatic/include/Event/FieldEventPokemonRideTool.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModel_define.h"
#include "Field/FieldStatic/include/MoveModel/FieldMoveModelManager.h"
#include "niji_conv_header/field/flagwork/FlagDefine.h"

// savedata
#include "Savedata/include/EventWork.h"
#include "Savedata/include/Record.h"
#include "Savedata/include/PokeDiarySave.h"

// Save
#include "Savedata/include/JoinFestaDataSave.h"

// msgId
#include <niji_conv_header/message/msg_jf_attraction.h>
#include <niji_conv_header/message/msg_jf_questiondata.h>

// ロトムお題用
#include "App/AppEvent/include/FieldMenu/FieldMenuGamedata.h"

// cdatファイル
#include "JoinFestaAttractionTable.cdat"

//　ホームスリープ制御
#include "System/include/HomeSleepAndPowerSystem.h"

/* ----------------------------------------------------------------------------------------- */
/**
*    Singletonインスタンスの実体を定義
*/
/* ----------------------------------------------------------------------------------------- */
SINGLETON_INSTANCE_DEFINITION(NetAppLib::JoinFesta::JoinFestaAttractionManager);
template class gfl2::base::SingletonAccessor<NetAppLib::JoinFesta::JoinFestaAttractionManager>;

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

// コンストラクタ
JoinFestaAttractionManager::JoinFestaAttractionManager( gfl2::heap::HeapBase* pHeap ) :
  m_requestState(REQUEST_NONE)
  ,m_requestResult(REQUEST_FAILURE)
  ,m_playAttractionId( JoinFestaScript::ATTRACTION_NULL )
  ,m_privateScore(0)
  ,m_totalScore(0)
  ,m_timeLimitFrame(0)
  ,m_isTimeLimitStart(false)
  ,m_retireRequest(false)
  ,m_isErrorRetire(false)
  ,m_saveRockDispNum(0)
  ,m_lastAttractionUpdateMsec(0)
#if PM_DEBUG
  ,m_debugIsDummySend(false)
  ,m_debugOtherTimeLimitEnable(true)
#endif
{
  m_scoreList.CreateBuffer( pHeap , JoinFestaAttractionDefine::SCORE_NUM_MAX );
  gfl2::std::MemClear( &m_resultData, sizeof(JoinFestaAttractionDefine::ResultData) );
}

// デストラクタ
JoinFestaAttractionManager::~JoinFestaAttractionManager()
{
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションIDからアトラクションデータを取得
 *
 * @param[in]   AttractionId アトラクションID
 * @return   アトラクションデータ
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData JoinFestaAttractionManager::GetAttractionData( const JoinFestaScript::AttractionId attractionId )
{
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    return attractionMasterDataTable[ attractionId ];
  }

  GFL_ASSERT_MSG(0, "%d\n", attractionId);
  return attractionMasterDataTable[0];
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション全体の初期化
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::ClearAttraction()
{
  // スコアクリア
  ClearScore();
  // パーソナルデータのアトラクションデータ初期化
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->ResetAttraction();

  // ポケモン抽選の履歴リセット
  JoinFestaAttractionUtil::ResetSaveHitPokeType();

  // 壊せる岩の初期化
  ClearKentarosuRideRockBreaker();

  m_playAttractionId  = JoinFestaScript::ATTRACTION_NULL;
  m_retireRequest = false;
  m_timeLimitFrame = 0;
  m_isTimeLimitStart = false;
  m_saveRockDispNum = 0;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション開催リクエスト
 *
 * @param[in]   AttractionId 開催するアトラクションID
 *
 * @note    IsEndRequestでリクエスト終了待ち
 * @note    GetRequestResultで結果取得
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::OpenRequest( const JoinFestaScript::AttractionId attractionId )
{
  GFL_PRINT("JoinFestaAttractionManager::OpenRequest %d \n", attractionId);
  ClearAttraction();

  // アトラクションデータ取得
  JoinFestaAttractionDefine::CoreData coreData = GetAttractionData( attractionId );

  // リクエスト中状態にする
  m_requestState = REQUEST_THROUGH;
  // リクエスト結果初期化
  m_requestResult = REQUEST_FAILURE;
  // リタイアフラグリセット
  m_isErrorRetire = false;

#if defined(GF_PLATFORM_CTR)
  // 通信リクエスト投げ
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->OpenAttractionRequest( this, attractionId, coreData.timeLimit );
#endif // GF_PLATFORM_CTR
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション参加
 *
 * @param[in]  person 開催者（参加者）のパーソナルデータ
 *
 * @note    IsEndRequestでリクエスト終了待ち
 * @note    GetRequestResultで結果取得
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::JoinRequest( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  GFL_PRINT("JoinFestaAttractionManager::JoinRequest \n");
  ClearAttraction();

  // リクエスト中状態にする
  m_requestState = REQUEST_THROUGH;
  // リクエスト結果初期化
  m_requestResult = REQUEST_FAILURE;
  // リタイアフラグリセット
  m_isErrorRetire = false;

#if defined(GF_PLATFORM_CTR)
  // 通信リクエスト投げ
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->JoinAttractionRequest( this, person );
#endif // GF_PLATFORM_CTR
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション終了リクエスト
 *
 * @note    IsEndRequestでリクエスト終了待ち
 * @note    GetRequestResultで結果取得
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::FinishRequest()
{
  GFL_PRINT("JoinFestaAttractionManager::FinishRequest \n");
  // リクエスト中状態にする
  m_requestState = REQUEST_THROUGH;
  // リクエスト結果初期化
  m_requestResult = REQUEST_FAILURE;

#if defined(GF_PLATFORM_CTR)
  // 通信リクエスト投げ
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->ExitAttractionRequest( this );
  // パーソナルデータ通知リスナー解除
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->RemovePersonalListener( this );
#endif // GF_PLATFORM_CTR
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクエスト終了待ち
 * @retval  true:リクエスト終了
 * @retval  false:リクエスト処理中  
 *
 * @note    GetRequestResultで結果取得
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::IsEndRequest()
{
  return ( m_requestState == REQUEST_FINISH );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リクエスト結果取得
 * @return  RequestResult リクエスト結果
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaAttractionManager::RequestResult JoinFestaAttractionManager::GetRequestResult()
{
  return m_requestResult;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   制限時間カウントダウン開始
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::TimeCountDownStart()
{
  m_isTimeLimitStart = true;
  GFL_PRINT(">Attraction CountDownStart!!!\n");
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション中かどうか
 * @return  trueでアトラクション中
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::IsPlaying()
{
  return ( m_playAttractionId != JoinFestaScript::ATTRACTION_NULL );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションプレイ中のアトラクションIDを取得
 * @return  JoinFestaScript::AttractionId
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaScript::AttractionId JoinFestaAttractionManager::GetPlayAttractionId()
{
  return m_playAttractionId;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   プレイ中のアトラクションIDからアトラクションデータを取得
 * @return   アトラクションデータ
 */
/* ----------------------------------------------------------------------------------------- */
NetAppLib::JoinFesta::JoinFestaAttractionDefine::CoreData JoinFestaAttractionManager::GetPlayAttractionData()
{
  return GetAttractionData( m_playAttractionId );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   強制リタイアチェック
 * @return  trueで強制リタイア
 *
 *          GFNMCat[275][2366]対処用
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::CheckForceRetire()
{
  // @fix GFNMcat[275] アトラクション中の無線通信OFF対応
  if( NetLib::NijiNetworkSystem::IsWLANSwitchEnable() == false )
  {
    m_isErrorRetire = true;
    return true;
  }

  // @fix GFNMcat[2366]：無線スイッチOFF→ON対策
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager)->GetMyData();
  if( pMyData->IsOpenAttraction() == false )
  {
    m_isErrorRetire = true;
    return true;
  }

  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リタイアリクエストフラグON
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::SetRetireRequest()
{
  m_retireRequest = true;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   リタイアリクエストフラグ取得
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::IsRetireRequest()
{
  return m_retireRequest;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   通信エラーでリタイアしたフラグ取得
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::IsErrorRetire()
{
  return m_isErrorRetire;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   自分のスコア加算
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::AddScore()
{
  m_privateScore += 1;  // 加算は1
  if( m_privateScore > JoinFestaAttractionDefine::PRIVATE_SCORE_MAX )
  {
    m_privateScore = JoinFestaAttractionDefine::PRIVATE_SCORE_MAX; 
  }

  CalcTotalScore();     // トータルスコア計算
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SetAttractionScore( m_privateScore );
#endif // GF_PLATFORM_CTR
}

#if PM_DEBUG
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：自分のスコア設定
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::DEBUG_SetScore( u16 score )
{
  m_privateScore = score;  // 加算は1
  if( m_privateScore > JoinFestaAttractionDefine::PRIVATE_SCORE_MAX )
  {
    m_privateScore = JoinFestaAttractionDefine::PRIVATE_SCORE_MAX; 
  }
  CalcTotalScore();     // トータルスコア計算
#if defined(GF_PLATFORM_CTR)
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->SetAttractionScore( m_privateScore );
#endif // GF_PLATFORM_CTR
}
#endif

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   自分のスコア取得
 * @return  自分のスコア
 */
/* ----------------------------------------------------------------------------------------- */
u16 JoinFestaAttractionManager::GetMyScore()
{
  return m_privateScore;
}

/* ----------------------------------------------------------------------------------------- */
/**
  * @brief   トータルスコア取得
  * @return  トータルスコア
  */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaAttractionManager::GetTotalScore()
{
  return m_totalScore;
}

/* ----------------------------------------------------------------------------------------- */
/**
  * @brief   参加人数取得
  * @return  参加人数（自分を含む）
  */
/* ----------------------------------------------------------------------------------------- */
u8 JoinFestaAttractionManager::GetMemberNum()
{
  return m_scoreList.Size() + 1;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション残り時間取得
 * @return  残り時間（フレーム値）
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaAttractionManager::GetTimeLimitFrame()
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
  NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = pPersonalManager->GetMyData();
  if( pMyData->IsOpenAttraction() )
  {
    return m_timeLimitFrame;
  }
  return 0;
}

#if PM_DEBUG
/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：残り時間を設定する
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::DEBUG_SetTimeLimit( u32 timeLimitFrame )
{
  m_timeLimitFrame = timeLimitFrame;  // フレーム値
  ICHI_PRINT(">ICHI 制限時間 %d 秒 \n", m_timeLimitFrame*30 );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   デバッグ用：残り時間カウントの有効化・無効化
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::DEBUG_SetTimeLimitEnable( bool isEnable )
{
  m_isTimeLimitStart = isEnable;
}
#endif

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション結果データ取得
 * @return  ResultData
 */
/* ----------------------------------------------------------------------------------------- */
JoinFestaAttractionDefine::ResultData& JoinFestaAttractionManager::GetResultData()
{
  return m_resultData;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションIDと回答Noから、回答メッセージのMSGID取得
 * @return  jf_questiondata.mstxtのメッセージID
 */
/* ----------------------------------------------------------------------------------------- */
u32 JoinFestaAttractionManager::GetQuestionAnswerMsgId( const JoinFestaScript::AttractionId attractionId, u32 answerNo )
{
  // アトラクションカテゴリ7種類 × 回答13個
  static const u32 questionAnswerMsgIdTable[7][13] =
  {
    // ねんすう
    { jf_question_sel_noans, jf_question_sel_q_15_ans_01, jf_question_sel_q_15_ans_02, jf_question_sel_q_15_ans_03, jf_question_sel_q_15_ans_04, jf_question_sel_q_15_ans_05, jf_question_sel_q_15_ans_06, jf_question_sel_q_15_ans_07, 0, 0, 0, 0, 0 },
    // いる・いない
    { jf_question_sel_noans, jf_question_sel_q_00_ans_01, jf_question_sel_q_00_ans_02, jf_question_sel_q_00_ans_03, jf_question_sel_q_00_ans_04, 0, 0, 0, 0, 0, 0, 0, 0 },
    // かぞく
    { jf_question_sel_noans, jf_question_sel_q_02_ans_01, jf_question_sel_q_02_ans_02, jf_question_sel_q_02_ans_03, jf_question_sel_q_02_ans_04, jf_question_sel_q_02_ans_05, jf_question_sel_q_02_ans_06, jf_question_sel_q_02_ans_07, jf_question_sel_q_02_ans_08, jf_question_sel_q_02_ans_09, 0, 0, 0 },
    // 兄弟構成
    { jf_question_sel_noans, jf_question_sel_q_01_ans_01, jf_question_sel_q_01_ans_02, jf_question_sel_q_01_ans_03, jf_question_sel_q_01_ans_04, jf_question_sel_q_01_ans_05, 0, 0, 0, 0, 0, 0, 0 },
    // がいけん・なかみ
    { jf_question_sel_noans, jf_question_sel_q_11_ans_01, jf_question_sel_q_11_ans_02, jf_question_sel_q_11_ans_03, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    // ひみつ
    { jf_question_sel_noans, jf_question_sel_q_06_ans_01, jf_question_sel_q_06_ans_02, jf_question_sel_q_06_ans_03, jf_question_sel_q_06_ans_04, 0, 0, 0, 0, 0, 0, 0, 0 },
    // しゅみ
    { jf_question_sel_noans, jf_question_sel_q_04_ans_01, jf_question_sel_q_04_ans_02, jf_question_sel_q_04_ans_03, jf_question_sel_q_04_ans_04, jf_question_sel_q_04_ans_05, jf_question_sel_q_04_ans_06, jf_question_sel_q_04_ans_07, jf_question_sel_q_04_ans_08, jf_question_sel_q_04_ans_09, jf_question_sel_q_04_ans_10, jf_question_sel_q_04_ans_11, jf_question_sel_q_04_ans_12 },
  };

  ICHI_PRINT(">ICHI AttractionId %d answerNo %d \n", attractionId, answerNo );
  if( attractionId < JoinFestaScript::ATTRACTION_NUM )
  {
    JoinFestaAttractionDefine::CoreData coreData = GetAttractionData( attractionId );
    // カテゴリ0はない
    if( 1 <= coreData.group && coreData.group <= 7 && answerNo < 13 )
    {
      return questionAnswerMsgIdTable[ coreData.group-1 ][ answerNo ];
    }
  }

  GFL_ASSERT_MSG(0, "%d %d\n", attractionId, answerNo );
  return questionAnswerMsgIdTable[0][0];
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクションIDと回答Noから、加点対象の回答なのかチェック
 * @return  trueで加点対象
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::IsQuestionAnswerScoreUp( const JoinFestaScript::AttractionId attractionId, u32 answerNo )
{
  static const u8 questionAnswerScoreUpTable[16][13] =
  {
    // 回答１３こある内の、加点の回答が1になっている
    { 0,0,0,0,1,1,1,1,0,0,0,0,0 },   //!< ベテラントレーナーに　あいさつ！
    { 0,0,1,1,0,0,0,0,0,0,0,0,0 },   //!< しんじんトレーナーに　ひとこと！
    { 0,0,0,1,1,0,0,0,0,0,0,0,0 },   //!< ラブラブな　ひとを　あつめろ！
    { 0,0,1,0,0,0,0,0,0,0,0,0,0 },   //!< クールなひとを　あつめろ！
    { 0,0,1,0,1,0,1,0,1,0,0,0,0 },   //!< おとこともだち　ちょうさ！
    { 0,0,0,1,0,1,0,1,0,1,0,0,0 },   //!< おんなともだち　ちょうさ！
    { 0,0,0,1,0,0,0,0,0,0,0,0,0 },   //!< あにき　あねきを　みつけろ！
    { 0,0,0,0,1,0,0,0,0,0,0,0,0 },   //!< まんなかっこを　みつけろ！
    { 0,0,0,0,0,1,0,0,0,0,0,0,0 },   //!< すえっこを　みつけろ！
    { 0,0,1,0,0,0,0,0,0,0,0,0,0 },  //!< ひとりっこを　みつけろ！
    { 0,0,0,1,0,0,0,0,0,0,0,0,0 },  //!< なかみが　だいじなひとを　さがせ！
    { 0,0,1,0,0,0,0,0,0,0,0,0,0 },  //!< がいけんが　だいじなひとを　さがせ！
    { 0,0,1,0,0,0,0,0,0,0,0,0,0 },  //!< ひみつはまもるもの
    { 0,0,0,1,1,0,0,0,0,0,0,0,0 },  //!< ひみつはもらすもの
    { 0,0,0,1,1,0,1,1,1,1,1,0,0 },  //!< 休日はまったり
    { 0,0,1,0,0,1,0,0,0,0,0,1,1 },  //!< 休日はアクティブ
  };

  // アトラクションIDを0オリジンにする
  const u8 dataTableIndex = attractionId - JoinFestaScript::ATTRACTION_QUESTIONNAIRE_1;

  ICHI_PRINT(">ICHI AttractionId %d answerNo %d \n", attractionId, answerNo );
  if( dataTableIndex < 16 && answerNo < 13 )
  {
    return (questionAnswerScoreUpTable[ dataTableIndex ][ answerNo ] == 1);
  }
  GFL_ASSERT_MSG(0, "%d %d\n", attractionId, answerNo );
  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   パーソナルから、加点対象なのかチェック
 * @return  trueで加点対象
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::IsKairikyRideScoreUp( const NetAppLib::JoinFesta::JoinFestaPersonalData* pTargetPerson )
{
   NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
   NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = pPersonalManager->GetMyData();

  // 自分と相手のトランスファードIDの下一桁でチェックする
  const u64 myId     = pMyData->GetTransferedID() % 10;
  const u64 targetId = pTargetPerson->GetTransferedID() % 10;
  ICHI_PRINT(">ICHI myId:%d targetId:%d \n", myId, targetId );

  // 相手 - 自分
  s32 tmp1 = static_cast<s32>( targetId - myId );
  // マイナスになってたら 10以内に丸める
  if( tmp1 < 0 ){ tmp1 += 10; }

  // 1,2,3なら加点対象
  if( tmp1 == 1 || tmp1 == 2 || tmp1 == 3 )
  {
    return true;
  }

  return false;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ケンタロスライドで使用する壊せる岩のクリア（非表示）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::ClearKentarosuRideRockBreaker()
{
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();

  // 全て非表示する
  for( u16 flag=SYS_FLAG_JF_ROCKBREAKER_00; flag<=SYS_FLAG_JF_ROCKBREAKER_15; flag++ )
  {
    if( Field::EventWork::CheckIDFlagRenge( flag ) )
    {
      pEventWork->SetEventFlag( flag );
    }
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ケンタロスライドで使用する壊せる岩のセット（表示）
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::SetKentarosuRideRockBreaker()
{
  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();


  // ランダムで１２個表示する
  for( u32 i=0; i<JoinFestaAttractionDefine::KENTAROSU_ROCK_DISP_MAX; i++ )
  {
    bool bLoop = true;
    while(bLoop)
    {
      u16 flag = System::GflUse::GetPublicRand( JoinFestaAttractionDefine::KENTAROSU_ROCK_NUM_MAX );
      flag += SYS_FLAG_JF_ROCKBREAKER_00;

      if( Field::EventWork::CheckIDFlagRenge( flag ) )
      {
        // フラグONのみチェック
        if( pEventWork->CheckEventFlag( flag ) == 1 )
        {
          // フラグOFFに出来たらOK
          pEventWork->ResetEventFlag( flag );
          bLoop = false;
        }
      }
    }
  }

  // 壊れていない岩の数を保存
  m_saveRockDispNum = JoinFestaAttractionDefine::KENTAROSU_ROCK_DISP_MAX;
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   ケンタロスライドで使用する壊せる岩を全て壊したかチェック
 */
/* ----------------------------------------------------------------------------------------- */
bool JoinFestaAttractionManager::IsKentarosuRideRockBreakerAllBreak()
{
  // ケンタロスライドのアトラクションのみチェック
  if( m_playAttractionId != JoinFestaScript::ATTRACTION_RIDE_KENTAROSU_1 )
  {
    return false;
  }

  Field::EventWork* pEventWork = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetEventWork();
  // 全てフラグが立っているかチェック
  u8 dispRockNum = 0;
  for( u16 flag=SYS_FLAG_JF_ROCKBREAKER_00; flag<=SYS_FLAG_JF_ROCKBREAKER_15; flag++ )
  {
    if( Field::EventWork::CheckIDFlagRenge( flag ) )
    {
      if( pEventWork->CheckEventFlag( flag ) == 0 )
      {
        dispRockNum++;
      }
    }
  }

  // 保存している岩の比べて減っていたらスコアアップさせる
  if( m_saveRockDispNum > dispRockNum )
  {
    AddScore();
    // SE
    Sound::PlaySE( SEQ_SE_JF_ATTRACTIONCOUNTUP );

    m_saveRockDispNum = dispRockNum;
  }

  if( dispRockNum > 0 )
  { // 1つでも壊れていないものがあればダメ
    return false; 
  }
  return true;
}


/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   更新
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::Update()
{
#if defined(GF_PLATFORM_CTR)
  u32 subValue = 1;
  const u32 frameMsec = 33;
  u64 currentMsec = nn::os::Tick::GetSystemCurrent().ToTimeSpan().GetMilliSeconds();
  if( currentMsec - m_lastAttractionUpdateMsec < frameMsec )
  {
    return;
  }

  subValue = ( currentMsec - m_lastAttractionUpdateMsec ) / frameMsec;
  m_lastAttractionUpdateMsec = m_lastAttractionUpdateMsec + (frameMsec*subValue);

  if( m_isTimeLimitStart )
  {
    if( m_timeLimitFrame > 0 )
    {
      m_timeLimitFrame -= gfl2::math::Min( (u32)( subValue ), (u32)( m_timeLimitFrame ) ); // 時間減らしているのはここ
    }
  }
#endif
}

//==========================================================================================
// JoinFestaAttractionListener
//==========================================================================================
//------------------------------------------------------------------
/**
 * @brief アトラクション参加成功
 * @param[out]  残り時間（フレーム値）
 */
//------------------------------------------------------------------
 void JoinFestaAttractionManager::OnJoinSuccess( u32 timeLimitFrame )
 {
   ICHI_PRINT("JoinFestaAttractionManager::OnJoinSuccess %d \n", timeLimitFrame );
   NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
   NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = pPersonalManager->GetMyData();
   if( pMyData->IsOpenAttraction() )
   {
     m_playAttractionId = static_cast<JoinFestaScript::AttractionId>(pMyData->GetPacketAttraction().attractionID);
     GFL_PRINT(">Attraction 参加成功 ID= %d \n", m_playAttractionId );
   }
   GFL_ASSERT( IsPlaying() );

   // リクエスト終了状態にする
   m_requestState = REQUEST_FINISH;
   // リクエスト結果
   m_requestResult = REQUEST_SUCCESS;

   // アトラクション開始処理
   StartAttraction();

   // 制限時間
   m_timeLimitFrame = timeLimitFrame;  // フレーム値
   ICHI_PRINT(">ICHI 制限時間 %d 秒 \n", m_timeLimitFrame*30 );

#if defined(GF_PLATFORM_CTR)
  // パーソナルデータ通知リスナー登録
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->RegistPersonalListener( this );
#endif // GF_PLATFORM_CTR
 }

//------------------------------------------------------------------
/**
 * @brief アトラクション参加失敗
 */
//------------------------------------------------------------------
 void JoinFestaAttractionManager::OnJoinFailed( void )
 {
   GFL_PRINT("JoinFestaAttractionManager::OnJoinFailed\n");
   // リクエスト終了状態にする
   m_requestState = REQUEST_FINISH;
   // リクエスト結果
   m_requestResult = REQUEST_FAILURE;
 }

//------------------------------------------------------------------
/**
 * @brief アトラクション作成成功
 */
//------------------------------------------------------------------
 void JoinFestaAttractionManager::OnOpenSuccess( void )
 {
   ICHI_PRINT("JoinFestaAttractionManager::OnOpenSuccess \n");
   NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);
   NetAppLib::JoinFesta::JoinFestaPersonalData* pMyData = pPersonalManager->GetMyData();
   if( pMyData->IsOpenAttraction() )
   {
     m_playAttractionId = static_cast<JoinFestaScript::AttractionId>(pMyData->GetPacketAttraction().attractionID);
     GFL_PRINT(">Attraction 作成成功 ID= %d \n", m_playAttractionId );
   }
   GFL_ASSERT( IsPlaying() );

   // リクエスト終了状態にする
   m_requestState = REQUEST_FINISH;
   // リクエスト結果
   m_requestResult = REQUEST_SUCCESS;

   // アトラクション開始処理
   StartAttraction();

#if defined(GF_PLATFORM_CTR)
  // パーソナルデータ通知リスナー登録
  GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPacketManager)->RegistPersonalListener( this );
#endif // GF_PLATFORM_CTR
 }

//------------------------------------------------------------------
/**
 * @brief アトラクション作成失敗
 */
//------------------------------------------------------------------
 void JoinFestaAttractionManager::OnOpenFailed( void )
 {
   GFL_PRINT("JoinFestaAttractionManager::OnOpenFailed\n");
   // リクエスト終了状態にする
   m_requestState = REQUEST_FINISH;
   // リクエスト結果
   m_requestResult = REQUEST_FAILURE;
 }

 //------------------------------------------------------------------
 /**
 * @brief アトラクション終了成功
 */
 //------------------------------------------------------------------
 void JoinFestaAttractionManager::OnExitSuccess( void )
 {
   ICHI_PRINT("JoinFestaAttractionManager::OnExitSuccess \n");

   // アトラクション結果を保存
   // ※リタイアの時は保存しない
   gfl2::std::MemClear( &m_resultData, sizeof(JoinFestaAttractionDefine::ResultData) );
   if( m_retireRequest != true )
   {
     SetupResultData();

     // アトラクション回数
     Savedata::IncRecord( Savedata::Record::RECID_JF_ATT_CNT );
     // ダイアリー：ローカルアトラクションに挑戦した
     PokeDiary::Set( PDID_COUNT_JF_ATTLACTION, GetPlayAttractionData().attractionName, m_totalScore );
   }

   ClearAttraction(); // アトラクション中が解除される
   // リクエスト終了状態にする
   m_requestState = REQUEST_FINISH;
   // リクエスト結果
   m_requestResult = REQUEST_SUCCESS;

   //ホームスリープ禁止解除
   System::HomeSleepAndPowerSystem::SubConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_JF_ATTRACTION_ACC );
 }

/*****************************************************************/
/* JoinFestaPersonalListener                                     */
/*****************************************************************/
//------------------------------------------------------------------
/**
 * @brief ジョインフェスタパーソナルデータ追加イベント通知
 *
 * @param[in]  person  追加されたパーソナル情報
 */
//------------------------------------------------------------------
void JoinFestaAttractionManager::OnAddJoinFestaPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  // スコア情報リスト更新
  UpdateScoreList( person );
}

//------------------------------------------------------------------
/**
 * @brief ジョインフェスタパーソナルデータ更新イベント通知
 *
 * @param[in]  person         更新されたパーソナル情報
 * @param[in]  isUpdate パケット更新フラグ
 */
//------------------------------------------------------------------
void JoinFestaAttractionManager::OnUpdateJoinFesatPerson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isUpdate )
{
  // スコア情報リスト更新
  UpdateScoreList( person );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション開始処理
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::StartAttraction()
{
  //ホームスリープ禁止
  System::HomeSleepAndPowerSystem::AddConditions( System::HomeSleepAndPowerSystem::HOME_SLEEP_JF_ATTRACTION_ACC );

   // 制限時間
#ifdef  GF_PLATFORM_CTR
   m_timeLimitFrame = GetPlayAttractionData().timeLimit * FRAME_PER_SECOND;  // フレーム値に変換
#else
   m_timeLimitFrame = GetPlayAttractionData().timeLimit * 30;  // フレーム値に変換
#endif
   ICHI_PRINT(">ICHI 制限時間 %d 秒 \n",GetPlayAttractionData().timeLimit );

   // ライドアトラクションの場合、プレイヤーをライド状態に上書きする
   switch( m_playAttractionId ){
   case JoinFestaScript::ATTRACTION_RIDE_KAIRIKY_1:              //!< みんなに　みせつけろ！　スプリント！
   case JoinFestaScript::ATTRACTION_RIDE_KENTAROSU_1:            //!< いわをこわせ！　スプリント！
     {
       Field::MoveModel::FieldMoveModelManager* pMoveModelManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetFieldCharaModelManager();
       Field::MoveModel::FieldMoveModelPlayer*  pMoveModelPlayer  = static_cast<Field::MoveModel::FieldMoveModelPlayer*>(pMoveModelManager->GetFieldMoveModel( Field::MoveModel::FIELD_MOVE_MODEL_PLAYER ));

       Field::MoveModel::FieldMoveModelHeaderWork     ride_headerWork;
       Field::MoveModel::FieldMoveModelHeaderResource ride_headerResource;

       Field::RIDE_POKEMON_ID ridePokemonId;

       // ライド：カイリキー
       if( m_playAttractionId == JoinFestaScript::ATTRACTION_RIDE_KAIRIKY_1 )
       {
         ridePokemonId = Field::RIDE_KAIRIKY;
       }
       // ライド：ケンタロス
       else
       {
         ridePokemonId = Field::RIDE_KENTAROSU;

         // 壊せる岩の配置
         SetKentarosuRideRockBreaker();
       }

       // ライド状態にする
       Field::EventPokemonRideTool::CreatePokemonRideWorld( ridePokemonId, &ride_headerWork, &ride_headerResource );
       pMoveModelPlayer->SetRideMoveModelHeader( ride_headerWork, ride_headerResource );
     }
     break;
   }

   // ロトムお題達成関数差し込み
   App::FieldMenu::PlayData::SetRotomRequestAchieved( Savedata::FieldMenu::ROTOM_REQ_FES_CIRCLE );
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   トータルスコア計算
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::CalcTotalScore()
{
  m_totalScore = 0;
  for( gfl2::util::Map<u64,JoinFestaAttractionDefine::ScoreInfo>::Iterator it = m_scoreList.Begin(); it != m_scoreList.End(); it++ )
  {
    m_totalScore += it->second.score;
  }

  m_totalScore += m_privateScore;
  if( m_totalScore > JoinFestaAttractionDefine::TOTAL_SCORE_MAX )
  {
    m_totalScore = JoinFestaAttractionDefine::TOTAL_SCORE_MAX; 
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スコア情報リスト更新
 * @param[in]  person         パーソナル情報
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::UpdateScoreList( const NetAppLib::JoinFesta::JoinFestaPersonalData& person )
{
  NetAppLib::JoinFesta::JoinFestaPersonalDataManager* pPersonalManager = GFL_SINGLETON_INSTANCE(NetAppLib::JoinFesta::JoinFestaPersonalDataManager);

  // アトラクション中のみ処理（そもそもアトラクション中にしか来ないが一応）
  if( IsPlaying() == false )
  {
    return;
  }
  // アトラクションでないなら処理しない
  if( person.IsOpenAttraction() == false )
  {
    return;
  }
  // 自分と同じアトラクションでないと処理しない
  if( person.IsSameAttraction( pPersonalManager->GetMyData()->GetPacketAttraction() ) == false )
  {
    return;
  }

  // スコア情報リストに登録
  const PacketAttraction& attractionInfo = person.GetPacketAttraction();
  gfl2::util::Map<u64,JoinFestaAttractionDefine::ScoreInfo>::Iterator listIterator = m_scoreList.Find( person.GetTransferedID() );
  if( listIterator == m_scoreList.End() )
  {//新規

    // @fix NMCat[2296]：アトラクション結果画面が最大100人（自分含む）なので、他人は99人までしか参加できないようにする
    //                   定義を他の場所でも使ってたのでここだけ修正することにする。
    if( m_scoreList.Size() < (JoinFestaAttractionDefine::SCORE_NUM_MAX-1) )
    {
      JoinFestaAttractionDefine::ScoreInfo scoreInfo;
      scoreInfo.score = attractionInfo.myScore;
      scoreInfo.friendKey.initialize( person.GetJoinFestaFriendKey().principalId , person.GetJoinFestaFriendKey().localFriendCode );
      ::std::memcpy( scoreInfo.name , person.GetName() , sizeof(scoreInfo.name) );

      m_scoreList.Insert( gfl2::util::Pair<u64,JoinFestaAttractionDefine::ScoreInfo>( static_cast<u64>( scoreInfo.friendKey.localFriendCode ) , scoreInfo ) );
    }
  }
  else
  {//更新
    if( listIterator->second.score < attractionInfo.myScore )
    {
      listIterator->second.score = attractionInfo.myScore;
    }
  }

  // トータルスコア計算
  CalcTotalScore();
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   スコアの初期化
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::ClearScore()
{
  m_privateScore  = 0;
  m_totalScore    = 0;

  //リスト初期化
  while( m_scoreList.Begin() != m_scoreList.End() )
  {
    m_scoreList.Erase( m_scoreList.Begin() );
  }
}

/* ----------------------------------------------------------------------------------------- */
/**
 * @brief   アトラクション結果データセットアップ
 */
/* ----------------------------------------------------------------------------------------- */
void JoinFestaAttractionManager::SetupResultData()
{
  GFL_ASSERT( IsPlaying() ); // 開発中のアサート

  Savedata::JoinFestaDataSave* pSave = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetGameData()->GetJoinFestaData();
  JoinFestaAttractionDefine::CoreData coreData = GetPlayAttractionData();

  m_resultData.attractionId   = m_playAttractionId;
  m_resultData.clearCondition = coreData.clearCondition;
  m_resultData.timeLimit      = coreData.timeLimit;
  m_resultData.privateScore   = m_privateScore;
  m_resultData.totalScore     = m_totalScore;

  // ★の数計算
  {
    // 個人評価
    if( coreData.manEvaluationStar5 <= m_privateScore )        { m_resultData.manEvaluationStarNum = 5; }
    else if( coreData.manEvaluationStar4 <= m_privateScore )   { m_resultData.manEvaluationStarNum = 4; }
    else if( coreData.manEvaluationStar3 <= m_privateScore )   { m_resultData.manEvaluationStarNum = 3; }
    else if( coreData.manEvaluationStar2 <= m_privateScore )   { m_resultData.manEvaluationStarNum = 2; }
    else if( coreData.manEvaluationStar1 <= m_privateScore )   { m_resultData.manEvaluationStarNum = 1; }
    else                                                       { m_resultData.manEvaluationStarNum = 0; }
    // 全体評価
    if( coreData.totalEvaluationStar5 <= m_totalScore )        { m_resultData.totalEvaluationStarNum = 5; }
    else if( coreData.totalEvaluationStar4 <= m_totalScore )   { m_resultData.totalEvaluationStarNum = 4; }
    else if( coreData.totalEvaluationStar3 <= m_totalScore )   { m_resultData.totalEvaluationStarNum = 3; }
    else if( coreData.totalEvaluationStar2 <= m_totalScore )   { m_resultData.totalEvaluationStarNum = 2; }
    else if( coreData.totalEvaluationStar1 <= m_totalScore )   { m_resultData.totalEvaluationStarNum = 1; }
    else                                                       { m_resultData.totalEvaluationStarNum = 0; }
  }

  // 入手フェスコイン
  {
    GFL_PRINT(">JFATTRACTION 獲得条件：%d 個人スコア:%d \n",coreData.clearCondition, m_privateScore );
    // 獲得条件満たしていたら
    if( m_totalScore >= coreData.clearCondition )
    {
      // 基礎報酬×★の数
      f32 getFesCoin = (f32)coreData.baseFasCoin * ( m_resultData.manEvaluationStarNum + m_resultData.totalEvaluationStarNum);

      // 占い結果補正
      switch( pSave->GetFortuneResultHitByAction(JoinFestaScript::FORTUNE_RESULT_ACTION_ATTRACTION) ){
      case JoinFestaScript::FORTUNE_RESULT_HIT_SMALL:   getFesCoin *= 1.1f; break;
      case JoinFestaScript::FORTUNE_RESULT_HIT_MIDDLE:  getFesCoin *= 1.2f; break;
      case JoinFestaScript::FORTUNE_RESULT_HIT_BIG:     getFesCoin *= 1.5f; break;
      }

      m_resultData.getFesCoin = static_cast<u16>(getFesCoin);
    }
    GFL_PRINT(">JFATTRACTION もらえるフェスコイン：%d \n", m_resultData.getFesCoin );
  }

  // 記録更新したかチェック
  {

    // 最高個人スコア
    if( m_privateScore > pSave->GetManHighScore( m_playAttractionId ) )
    { // 超えていたので更新
      pSave->SetManHighScore( m_playAttractionId, m_privateScore );
      m_resultData.isPrivateScoreNewRecord = true;
    }

    // 最高トータルスコア
    if( m_totalScore > pSave->GetTotalHighScore( m_playAttractionId ) )
    { // 超えていたので更新
      pSave->SetTotalHighScore( m_playAttractionId, m_totalScore );
      m_resultData.isTotalScoreNewRecord = true;
    }

    // プレイ回数加算
    pSave->AddPlayCount( m_playAttractionId );
  }

  // 収集リスト
  {
    // 全フレンドキー無効化
    for( u8 i=0;i<JoinFestaAttractionDefine::SCORE_NUM_MAX; i++ )
    {
      m_resultData.scoreInfoList[i].friendKey.initialize();
    }

    // 収集した分のみコピー
    int index=0;
    for( gfl2::util::Map<u64,JoinFestaAttractionDefine::ScoreInfo>::Iterator it = m_scoreList.Begin(); it != m_scoreList.End(); it++ )
    {
      m_resultData.scoreInfoList[index] = it->second;
      index++;
    }
  }
}



GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

