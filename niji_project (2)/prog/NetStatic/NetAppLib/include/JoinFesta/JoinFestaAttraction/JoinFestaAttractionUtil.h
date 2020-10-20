//==============================================================================
/**
 * @file	JoinFestaAttractionUtil.h
 * @brief	ジョインフェスタアトラクションユーティリティ
 * @author	ichiraku_katsuhiko
 * @date	2016/02/04　12:47
 */
// =============================================================================
#ifndef __JOIN_FESTA_ATTRACTION_UTIL_H__
#define __JOIN_FESTA_ATTRACTION_UTIL_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"

// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaNetDefine.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(NetApp)
GFL_NAMESPACE_BEGIN(JoinFesta)
struct JoinFestaDataTable;
GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetApp)

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 前方宣言
class JoinFestaPersonalData;

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaAttractionUtil
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaAttractionUtil);

public:
  // コンストラクタ
  JoinFestaAttractionUtil();
  // デストラクタ
  virtual ~JoinFestaAttractionUtil();

public:
  // メッセージIDの無効値
  static const u32 MSG_ID_NULL = 0xffffffff;

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   アトラクションIDから自分がプレイ可能かチェックする
   * @param[in]   attractionId    アトラクションID
   *
   * @retval  true : プレイ出来る
   * @retval  false : プレイ出来ない
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool CheckAttractionPlayEnable( const JoinFestaScript::AttractionId attractionId );

  //=============================================================================
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   アトラクションに出現するフィールド人物リスト作成
   * @param[in]   attractionId    アトラクションID
   */
  /* ----------------------------------------------------------------------------------------- */
  static void CreateFieldPersonalListForAttraction( const JoinFestaScript::AttractionId attractionId, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   アトラクション開催中のダミーデータの定型：あいさつを取得
   * @param[in]  ダミーデータのパーソナルデータ
   * @param[in]  アトラクション専用のもので決まらなかった場合に、ランダムで決めるか(trueで決める）
   *
   * @return  定型文挨拶のmsgID（無効値 MSG_ID_NULL )
   */
  /* ----------------------------------------------------------------------------------------- */
  static u32 GetPatternHelloFromDummyData( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, bool isRandomSelect );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   アトラクション開催中のダミーデータの定型：あいさつ以外取得
   * @param[in]  ダミーデータのパーソナルデータ
   *
   * @return  定型文挨拶のmsgID
   */
  /* ----------------------------------------------------------------------------------------- */
  static u32 GetPatternNotHelloFromDummyData( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   アンケートアトラクション用にアンケート回答を取得
   * @param[in]  ダミーデータのパーソナルデータ
   * @param[in]  questionnaireId  アンケートID
   * @param[in]  isHit  正解の回答を入れるか trueで入れる。falseでハズレ回答
   *
   * @return  回答値
   */
  /* ----------------------------------------------------------------------------------------- */
  static u8 GetQuestionnaireAnswwerFromDummyData( const NetAppLib::JoinFesta::JoinFestaPersonalData& person, const u8 questionnaireId, const bool isHit );

  // アトラクションIDからポケモンタイプを取得
  static u8 GetPokeTypeFromAttractionId( const JoinFestaScript::AttractionId attractionId );

  // アトラクションIDからアンケートIDを取得
  static u8 GetQuestionnaireIdFromAttractionId( const JoinFestaScript::AttractionId attractionId );

  // アンケートIDから回答数を取得
  static u8 GetAnswerMaxFromQuestionnaireId( const u8 questionnaireId );

  //! タイプ相性とタイプ好きで抽選されたポケモン番号をリセット
  static void ResetSaveHitPokeType();

private:
  // アトラクション中の人物最大数
  static const u32 PERSONAL_MAX = 16;

  // ランダムで選ばれる定型挨拶の最大数
  static const u32 RANDOM_PATTERN_TABLE_MAX = 261;

  
  /**
   * @enum DressUpParamPattern
   *       着せ替えパターン
   */
  enum DressUpParamPattern{
    DRESS_UP_PARAM_PATTERN_DEFAULT,   // A,R,Sからランダム
    DRESS_UP_PARAM_PATTERN_MY_COPY,   // 自分の着せ替え状況のコピー
    DRESS_UP_PARAM_PATTERN_MY_COPY2,  // 自分の着せ替え状況のコピーしてアルゴリズムで一部変更
    DRESS_UP_PARAM_PATTERN_A,         // A(基本)からランダム
    DRESS_UP_PARAM_PATTERN_A_MALE,    // A(基本)からランダム(男のみ）
    DRESS_UP_PARAM_PATTERN_A_FEMALE,  // A(基本)からランダム(女のみ）
    DRESS_UP_PARAM_PATTERN_B,         // B(赤：正解)からランダム
    DRESS_UP_PARAM_PATTERN_C,         // C(黄：正解)からランダム
    DRESS_UP_PARAM_PATTERN_D,         // D(緑：正解)からランダム
    DRESS_UP_PARAM_PATTERN_E,         // E(青：正解)からランダム
    DRESS_UP_PARAM_PATTERN_F,         // F(橙：正解)からランダム
    DRESS_UP_PARAM_PATTERN_G,         // G(藍：正解)からランダム
    DRESS_UP_PARAM_PATTERN_H,         // H(紫：正解)からランダム
    DRESS_UP_PARAM_PATTERN_I,         // I(桃：正解)からランダム
    DRESS_UP_PARAM_PATTERN_J,         // J(赤：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_K,         // K(黄：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_L,         // L(緑：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_M,         // M(青：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_N,         // N(橙：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_O,         // O(藍：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_P,         // P(紫：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_Q,         // Q(桃：不正解)からランダム
    DRESS_UP_PARAM_PATTERN_R,         // R(帽子×眼鏡)からランダム
    DRESS_UP_PARAM_PATTERN_S,         // Sからランダム
    DRESS_UP_PARAM_PATTERN_COLD,      // 寒色(青藍紫)からランダム
    DRESS_UP_PARAM_PATTERN_WARM,      // 暖色(赤黄緑橙桃)からランダム
  };


private:

  // 施設案内アトラクション可能かどうか
  static bool CheckFacilitiesAttractionPlayEnable( const JoinFestaScript::AttractionId attractionId, const JoinFestaScript::FacilitiesType type );

  // 人物比率でフィールドリスト作成
  static void CreateFieldPersonalList( const JoinFestaScript::AttractionId attractionId, const u32 guestNum, const u32 vipNum, const u32 npcMaxNum );
  // 関係別のリスト作成
  static u32 CreateFieldPersonalListForRelation( const JoinFestaScript::AttractionId attractionId, const NetAppLib::JoinFesta::JoinFestaDefine::E_JOIN_FESTA_RELATION relation, const u32 maxNum );

  //! 着せ替えセット
  static void SetDressUpPattern( NetAppLib::JoinFesta::JoinFestaPersonalData& person, DressUpParamPattern dressUpPattern, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable );

  // アンケートアトラクション
  static u32 GetPatternHelloForAttractionQuestionnaire( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );
  // 施設案内アトラクション
  static u32 GetPatternHelloForAttractionRecommendationShop( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );
  // 言語レッスンアトラクション
  static u32 GetPatternHelloForAttractionLanguageLesson( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );
  // カイリキーライドアトラクション
  static u32 GetPatternHelloForAttractionRideKairiky( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );
  // タイプ相性アトラクション
  static u32 GetPatternHelloForAttractionAffinity( const NetAppLib::JoinFesta::JoinFestaPersonalData& person );

  //! 施設案内アトラクションで発動するパーソナルイベント決める
  static void SetupFakePersonalEvent( const JoinFestaScript::AttractionId attractionId, NetAppLib::JoinFesta::JoinFestaPersonalData& person, const bool isHit );

  //! 言語レッスンで、出現人物の言語決める
  static void SetupLanguageTbl( const JoinFestaScript::AttractionId attractionId, u8 outLanguageTbl[PERSONAL_MAX] );

  //! タイプ相性診断で出るポケモンテーブル計算
  static void CalcAffinityMonsNoTable( u16 (&outMonsNoTable)[PERSONAL_MAX], const NetApp::JoinFesta::JoinFestaDataTable* pDataTable );
  //! タイプ好き探せで、ポケモンを決める
  static void SetSearchPokemon( NetAppLib::JoinFesta::JoinFestaPersonalData& person, const bool isAlola, const u8 hitPokeType, const u8 lotteryPokeType, const u16 ngMonsNo, const NetApp::JoinFesta::JoinFestaDataTable* pDataTable );
  //! タイプ好きのテーブル作る
  static void CalcSearchPokeTypeTable( const u8 hitPokeType, u8 (&outTypeTable)[PERSONAL_MAX] );

  //! タイプ相性とタイプ好きで抽選されたポケモン番号を保存
  static void SaveHitPokeType();

  // 着せ替えパーツ一部変更用テーブル
  static const u32 m_acceyeTable[ 2 ][ 5 ];
  static const u32 m_bagTable[ 2 ][ 5 ];
  static const u32 m_shoesTable[ 2 ][ 5 ];

  // ダミーで選ばれる定型文テーブル
  static const u16 m_patternTable[ RANDOM_PATTERN_TABLE_MAX ];

  // お店案内での発動するパーソナルイベントテーブル
  static const u8 m_fakeHitPersonalEventTable[7][9];

  // タイプ相性、タイプ好きの1回分の履歴
  static u16 m_saveHitPokeTypeTable[ PERSONAL_MAX ];
};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // __JOIN_FESTA_ATTRACTION_UTIL_H__
