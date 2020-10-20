#if !defined(QRREADERMESSAGEMENU_H_INCLUDED)
#define QRREADERMESSAGEMENU_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderMessageMenu.h
 * @date   2016/01/20 Wed. 17:44:53
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppMessageMenu.h"


namespace NetApp   {
namespace QRReader {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/20 Wed. 17:47:35
  */
//==============================================================================
class MessageMenu
  : public  NetAppLib::UI::NetAppMessageMenu
{
  GFL_FORBID_COPY_AND_ASSIGN(MessageMenu);
public:

  /*  ctor/dtor  */
  MessageMenu(QRReaderWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler = NULL);
  virtual ~MessageMenu();

  enum  Mode
  {
    MODE_None,

    MODE_ConfirmTutorial,                       /**<  チュートリアルを表示するか      */
    MODE_ActivateCameraInformation,             /**<  カメラ起動                      */
    MODE_InactivateCameraInformation,           /**<  カメラ終了                      */
    MODE_CameraBroken,                          /**<  カメラが故障しています          */

    MODE_QRReadGuide,                           /**<  枠内に入れてください            */
    MODE_QRSearching,                           /**<  QRを探しています                */
    MODE_QRAnalyizing,                          /**<  QRを解析しています              */
    MODE_LowBattery,                            /**<  バッテリーがないのでQR読めない  */

    /*
      2016/04/19  Yu Muto
      未使用のため寺地さん要望により削除
    */
//    MODE_CannotChargePowerConfirm,              /**<  チャージされないが続けますか    */

    MODE_Found_Pokemon_Alola,                   /**<  ポケモンデータ：アローラ種      */
    MODE_Found_Pokemon_NotAlola,                /**<  ポケモンデータ：非アローラ種    */
    MODE_Found_Pokemon_AlreadyIdentified,       /**<  ポケモンデータ：既に載っている  */
    MODE_Found_Pokemon_RecentData,              /**<  ポケモンデータ：最近解析した(履歴ヒット)  */
    MODE_Found_Pokemon_MagianaNew,              /**<  ポケモンデータ：マギアナ新規                            */
    MODE_Found_Pokemon_MagianaUpdate,           /**<  ポケモンデータ：マギアナ図鑑更新                        */
    MODE_Found_Pokemon_MagianaUpdateAfterEvent, /**<  ポケモンデータ：マギアナ図鑑更新（特殊イベント完了後）  */
    MODE_Found_Pokemon_M20PikatyuNew,              /**<  ポケモンデータ：M20ピカチュウ新規                            */
    MODE_Found_Pokemon_M20PikatyuUpdate,           /**<  ポケモンデータ：M20ピカチュウ図鑑更新                        */
    MODE_Found_Pokemon_M20PikatyuUpdateAfterEvent, /**<  ポケモンデータ：M20ピカチュウ図鑑更新（特殊イベント完了後）  */

    MODE_Found_Pokemon_CannotAnalyze,       /**<  ポケモンデータ：まだ解析できない（未クリア時マギアナ）  */
    MODE_Found_M20Pikatyu_CannotAnalyze,    /**<  ポケモンデータ：まだ解析できない（島2未到達M20ピカチュウ）  */

    MODE_Found_NormalData,                  /**<  一般：ミラクルQRを読み込んだ      */
    MODE_Found_PokedexData,                 /**<  図鑑：アローラ図鑑QRを読み込んだ  */
    MODE_Found_PromotionalData,             /**<  連動QRを読み込んだ                */

    MODE_DataNotAvailable_Console,          /**<  この本体では解析できない        */
    MODE_DataNotAvailable_App,              /**<  このゲームでは解析できない      */

    MODE_DataIsAlreadyRead,                 /**<  既に読み込んだQRだ              */
    MODE_Charged,                           /**<  n%チャージされた                */
    MODE_ScanPowerTotal,                    /**<  合計でn%になった                */
    /*
      2016/04/25  Yu Muto
      未使用のため寺地さん要望により削除
    */
//    MODE_CannotCharge,                      /**<  フィールドスキャン発動中        */
    MODE_ScanPowerFullCharged,              /**<  100%になった                    */
    MODE_ScanPowerAlreadyFull,              /**<  既に100%                        */

    /*  fieldscan  */
    MODE_FieldScan_Available,               /**<  フィールドスキャン有効          */
    MODE_FieldScan_NotAvailable,            /**<  フィールドスキャン無効          */
    MODE_FieldScan_Scanning,                /**<  探しています                    */
    MODE_FieldScan_StartConfirm,            /**<  スキャン開始しますか            */

    MODE_FieldScan_Found,                   /**<  見つかった                      */
    MODE_FieldScan_NotFound,                /**<  見つからなかった                */
    MDOE_FieldScan_LetsHunt,                /**<  気配を探そう                    */

    MODE_FieldScanPowerEmpty,               /**<  パワーがなくなったので中断      */

    /*  analyze  */
    MODE_QRNG_FriendMatchData,              /**<  NG:なかま大会のデータ           */
    MODE_QRNG_LiveMatchData,                /**<  NG:ライブ大会のデータ           */
    MODE_QRNG_TeamData,                     /**<  NG:バトルチームのデータ         */
    MODE_QRNG_ZukanData,                    /**<  NG:図鑑のデータ                 */
    MODE_QRNG_GameSyncID,                   /**<  NG:GameSyncID mismatch          */
    MODE_QRNG_Region,                       /**<  NG:invalid region               */
    MODE_QRNG_ROM,                          /**<  NG:invalid ROM                  */

    MODE_QROK,                              /**<  OK:データを読み取った           */

    MODE_QRSearch_FriendMatchData,          /**<  なかま大会のデータを読んで      */
    MODE_QRSearch_LiveMatchData,            /**<  ライブ大会のデータを読んで      */
    MODE_QRSearch_TeamData,                 /**<  チームデータを読んで            */


    MODE_MAX
  };

  void  Show(const Mode mode);
  void  Hide(void)  {HideMessage();}

  void  SetScanPowerGainValue(const u8 gainValue) {m_GainValue  = gainValue;}
  void  SetFieldScanIslandID(const u32 islandID);


protected:
  void  PreShow(void);
  void  ShowYN(const u32 messageID, const bool isImmediate);

  /*
    members
  */
  QRReaderWork*   m_pWork;
  Mode            m_Mode;

  u8              m_GainValue;
  u32             m_IslandNameMessageID;
};



} /*  namespace QRReader */
} /*  namespace NetApp   */
#endif  /*  #if !defined(QRREADERMESSAGEMENU_H_INCLUDED)  */
