//==============================================================================
/**
 * @file	JoinFestaPersonalEvent.h
 * @brief	ジョインフェスタパーソナルイベント（スクリプトのイベント種類）
 * @author	ichiraku_katsuhiko
 * @date	2015/11/09　12:47
 */
// =============================================================================
#ifndef __JOIN_FESTA_PERSONAL_EVENT_H__
#define __JOIN_FESTA_PERSONAL_EVENT_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <util/include/gfl2_FixedSizeContainer.h>

// JoinFestaScript
#include "niji_reference_files/script/JoinFestaPawnDefine.h"

// JoinFesta
#include "NetApp/JoinFesta/include/JoinFestaDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalEvent/JoinFestaPersonalEventDefine.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalData.h"
#include "NetStatic/NetAppLib/include/JoinFesta/JoinFestaPersonalDataManager.h"


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// 前方宣言

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaPersonalEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(JoinFestaPersonalEvent);
public:

  //-----------------------------------------------------------------------------
  //-----------------------------------------------------------------------------
private:
  /**
   * @struct EventData
   * パーソナルイベント１つに付随するデータ
   */
  struct EventData{
    JoinFestaPersonalEventDefine::Difficulty    m_difficulty;     //!< 難易度  
    u32                                         m_baseCoin;       //!< 基礎コイン値
  };

  /**
   * @struct EventActuate
   * パーソナルイベント発動データ
   */
  struct EventActuate{
    u8            m_jfRank;         //!< JFランク
    u8            m_easyEventNum;   //!< 難易度低のイベント数
    u8            m_normalEventNum; //!< 難易度中のイベント数
    u8            m_hardEventNum;   //!< 難易度高のイベント数
  };


public:
  // コンストラクタ
  JoinFestaPersonalEvent();
  // デストラクタ
  virtual ~JoinFestaPersonalEvent();

  //=============================================================================
  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パーソナルイベントが存在するか（フィールドメニュー用）
   *
   * @retval  true:存在する  
   * @retval  false:存在しない 
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsEventExist();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パーソナルイベント抽選時間更新
   *          一定時間経過していた場合は、リセット処理も走る
   */
  /* ----------------------------------------------------------------------------------------- */
  static void UpdateLotteryTime();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パーソナルイベント発動抽選処理
   */
  /* ----------------------------------------------------------------------------------------- */
  static void PersonalEventActuate();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   イベント発動データからパーソナルイベントセーブを設定
   */
  /* ----------------------------------------------------------------------------------------- */
  static void SetupPersonalEventSaveForEventActuate( u16 jfRank, JoinFestaPersonalEventDefine::SlotData* pSlotDataArray );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パーソナルイベントから一言ウィンドウに出すメッセージのID取得
   */
  /* ----------------------------------------------------------------------------------------- */
  static u32 GetWordWindowMessageIdFromPersonalEvent( u32 personalEventType, u32 sex );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パーソナルイベントから基礎値取得
   */
  /* ----------------------------------------------------------------------------------------- */
  static u32 GetBaseFesCoin( u32 personalEventType );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パーソナルからレコード値が大きいレコードを取得
   * @param[out]   pOutCategory1  レコードカテゴリー1
   * @param[out]   pOutCategory2  レコードカテゴリー2
   * @param[out]   pOutCategory3  レコードカテゴリー3
   * @param[out]   pOutRecord1    レコードINDEX1
   * @param[out]   pOutRecord2    レコードINDEX2
   * @param[out]   pOutRecord3    レコードINDEX3
   */
  /* ----------------------------------------------------------------------------------------- */
  static void GetHighRecord( const NetAppLib::JoinFesta::JoinFestaPersonalData* personal,
                             u8* pOutCategory1, u8* pOutCategory2, u8* pOutCategory3,
                             u8* pOutRecord1, u8* pOutRecord2, u8* pOutRecord3 );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   レコードカテゴリの総数を取得
   * @param[in]   categoryId    レコードカテゴリ
   * @return  レコードカテゴリ総数
   */
  /* ----------------------------------------------------------------------------------------- */
  static u8 GetRecordCategoryNum( u8 categoryId );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定したパーソナルデータで、指定したパーソナルイベントが行えるかチェック
   *
   * @param[in]   personalEventType チェックしたいパーソナルイベント種類
   * @param[in]   personalData      チェックしたいパーソナルデータ
   *
   * @return  trueで行える
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsCanEventFromPersonal( JoinFestaScript::PersonalEventType personalEventType, const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData );

private:    

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定した難易度以下でパーソナルイベント発動
   *
   * @param[in]   targetDifficulty  パーソナルイベント難易度
   */
  /* ----------------------------------------------------------------------------------------- */
  static void SetPersonalEventForUnderDifficulty( const u32 personalEventSaveIndex, JoinFestaPersonalEventDefine::Difficulty targetDifficulty, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定した難易度でイベントが行えるパーソナルデータを取得
   *
   * @param[in]   targetDifficulty  パーソナルイベント難易度
   *
   * @return      パーソナルデータ　見つからなければNULL
   */
  /* ----------------------------------------------------------------------------------------- */
  static NetAppLib::JoinFesta::JoinFestaPersonalData* GetPersonalDataFromDifficulty( JoinFestaPersonalEventDefine::Difficulty targetDifficulty, NetAppLib::JoinFesta::JoinFestaPersonalDataManager::JoinFestaPersonalDataList* pPersonalList );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   パーソナルイベントからパーソナルデータが一致するものがあるかチェック
   *
   * @param[in]   personal  チェックするパーソナルデータ
   *
   * @return      trueで一致するものがあった
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsSamePersonalDataFromPersonalEvent( const NetAppLib::JoinFesta::JoinFestaPersonalData& personal );

    /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定したパーソナルデータで、指定した難易度のイベントが行えるかチェック
   *
   * @param[in]   targetDifficulty  チェックしたいパーソナルイベント難易度
   * @param[in]   personalData      チェックしたいパーソナルデータ
   *
   * @return  trueで行える
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool IsCanDifficultyFromPersonal( JoinFestaPersonalEventDefine::Difficulty targetDifficulty, const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief   指定した難易度でイベント抽選する
   *
   * @param[in]   targetDifficulty  イベント難易度
   * @param[in]   personalData      パーソナルデータ
   *
   * @return  抽選したイベント
   */
  /* ----------------------------------------------------------------------------------------- */
  static JoinFestaScript::PersonalEventType LotteryPersonalEvent( JoinFestaPersonalEventDefine::Difficulty targetDifficulty, const NetAppLib::JoinFesta::JoinFestaPersonalData& personalData );

private:

  enum{
    EVENT_ACTUATE_TABLE_MAX = 11,
    RECORD_TABLE_MAX = 36,
    RECORD_CATEGORY_NUM_TABLE_MAX = 7,
  };


  //! パーソナルイベントデータテーブル
  static const EventData      m_eventDataTable[ JoinFestaScript::PERSONAL_EVENT_MAX ];

  //! イベント発動抽選テーブル
  static const EventActuate   m_eventActuateTable[EVENT_ACTUATE_TABLE_MAX];

  //! 一言ウィンドウメッセージIDテーブル
  static const u32  m_wordWindowMessageId[ JoinFestaScript::PERSONAL_EVENT_MAX ];

  //! レコードカテゴリ最大数テーブル
  static const u8   m_recordCategoryNumTable[ RECORD_CATEGORY_NUM_TABLE_MAX ];

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // __JOIN_FESTA_PERSONAL_EVENT_H__
