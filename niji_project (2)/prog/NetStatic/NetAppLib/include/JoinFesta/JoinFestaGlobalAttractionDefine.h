//==============================================================================
/**
 * @file	JoinFestaGlobalAttractionDefine.h
 * @brief	ジョインフェスタグローバルアトラクション定義
 * @author	ichiraku_katsuhiko
 * @date	2016/03/02　12:47
 */
// =============================================================================
#ifndef __JOIN_FESTA_GLOBAL_ATTRACTION_DEFINE_H__
#define __JOIN_FESTA_GLOBAL_ATTRACTION_DEFINE_H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>

GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(JoinFesta)

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

class JoinFestaGlobalAttractionDefine
{
public:

  //! 参加履歴の件数
  static const int RECORD_MAX = 12;

  // 最大値
  enum{
    LOCAL_RECORD_MAX = 9999,       //! ローカル記録
    UPLOAD_RECORD_MAX = 9999,      //! アップロード記録
  };

  /*
   *  @enum OpenState
   *  開催状態
   */
  enum OpenState{
    OPEN_STATE_NULL,        //!< 開催していない
    OPEN_STATE_NOW,         //!< 開催中
    OPEN_STATE_TOTAL_NOW,   //!< 開催終了で集計中
    OPEN_STATE_TOTAL_FINISH,//!< 開催終了で集計完了
  };

  /**
   * @enum ParticipateState
   *  参加状態
   */
  enum ParticipateState{
    PARTICIPATE_STATE_NULL,   //!< 参加登録していない
    PARTICIPATE_STATE_OK,     //!< 参加登録した
  };

  /**
   * @struct SaveData
   *         グローバルアトラクション参加履歴1件のセーブデータ構造
   *         80byte
   */
  struct SaveData{
    u32           totalRecord;              //!< 集計記録
    u8            openState;                //!< 開催状態 OpenState定義
    u8            participateState;         //!< 参加状態 ParticipateState定義
    u16           localRecordMax;           //!< ローカル記録のカンスト値 // @fix GFNMCat[5599] カンスト値の追加
    //
    u16           localRecord;              //!< ローカル記録
    u16           uploadRecord;             //!< アップロード記録
    b8            isPglMember;              //!< アップロード時のPGL会員状態（0=未登録 1=登録）
    u8            globalAttractionID;       //!< グローバルアトラクションのID // @fix GFNMCat[5605] グローバルアトラクションIDの追加
    //
    u8            totalDlYear;              //!< 集計DL年
    u8            totalDlMonth;             //!< 集計DL月
    u8            totalDlDay;               //!< 集計DL日
    u8            totalDlHour;              //!< 集計DL時
    u8            totalDlMinitue;           //!< 集計DL分
    //
    u8            rewardState;              //!< ご褒美授与状態(0=授与していない 1=授与した)
    u16           rewardValue;              //!< ご褒美
    u8            rewardYear;               //!< ご褒美受取年
    u8            rewardMonth;              //!< ご褒美受取月
    u8            rewardDay;                //!< ご褒美受取日
    u8            rewardHour;               //!< ご褒美受取時
    u8            rewardMinitue;            //!< ご褒美受取分
    u8            endYear;                  //!< 終了年
    u8            endMonth;                 //!< 終了月
    u8            endDay;                   //!< 終了日
    u8            endHour;                  //!< 終了時
    u8            endMinitue;               //!< 終了分
    u8            dummy[48];
  };

};

GFL_NAMESPACE_END(JoinFesta)
GFL_NAMESPACE_END(NetAppLib)

#endif // __JOIN_FESTA_GLOBAL_ATTRACTION_DEFINE_H__
