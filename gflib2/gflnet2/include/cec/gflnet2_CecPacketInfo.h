// ============================================================================
/*
 * @file gflnet2_CecPacketInfo.h
 * @brief すれ違い通信のパケット情報です。
 * @date 2015.02.13
 */
// ============================================================================
#if !defined( GFLNET2_CECPACKETINFO_H_INCLUDE )
#define GFLNET2_CECPACKETINFO_H_INCLUDE
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


GFL_NAMESPACE_BEGIN(gflnet2)
GFL_NAMESPACE_BEGIN(cec)


/*!
    @brief  曜日を表す定数です。
    @note   fnd_DateTime.h の Week
*/
enum Week
{
    WEEK_SUNDAY = 0,   ///< 日曜日
    WEEK_MONDAY,       ///< 月曜日
    WEEK_TUESDAY,      ///< 火曜日
    WEEK_WEDNESDAY,    ///< 水曜日
    WEEK_THURSDAY,     ///< 木曜日
    WEEK_FRIDAY,       ///< 金曜日
    WEEK_SATURDAY,     ///< 土曜日
    WEEK_MAX
};

/*!
    @brief  日時を表す構造体です。
    @note   fnd_DateTime.h の DateTimeParameters
*/
struct DateTimeParameters
{
    s32     year;         ///< 年
    s8      month;        ///< 月
    s8      day;          ///< 日
    Week    week;         ///< 曜日
    s8      hour;         ///< 時
    s8      minute;       ///< 分
    s8      second;       ///< 秒
    s16     milliSecond;  ///< ミリ秒
};


typedef struct
{
  void*             pSendData;              // 送信データ
  size_t            sendSize;               // 送信データのサイズ
  void*             pIconData;              // アイコンデータ
  size_t            iconSize;               // アイコンデータのサイズ
  wchar_t*          pInfoTextData;          // 情報テキスト
  size_t            infoTextSize;           // 情報テキストのサイズ
} CecSendPacket;


typedef struct
{
  void*               pRecvData;            // 受信データ
  size_t              recvSize;             // 実際に受信した受信データのサイズ
  void*               pIconData;            // 受信アイコン
  size_t              iconSize;             // 実際に受信した受信アイコンのサイズ
  wchar_t*            pInfoTextData;        // 受信情報テキスト
  size_t              infoTextSize;         // 実際に受信した受信情報テキストのサイズ

  DateTimeParameters  createDateTime;       // 受信したデータの作成日時の情報

} CecRecvPacket;


GFL_NAMESPACE_END(cec)
GFL_NAMESPACE_END(gflnet2)

#endif // GFLNET2_CECPACKETINFO_H_INCLUDE
