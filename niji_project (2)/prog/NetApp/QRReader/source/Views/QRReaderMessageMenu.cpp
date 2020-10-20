//==============================================================================
/**
 * @file   QRReaderMessageMenu.cpp
 * @date   2016/01/20 Wed. 17:56:57
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../System/QRReaderWork.h"

#include  <print/include/PrintSystem.h>

#include  "../QRReaderResource.h"
#include  "../Common/FieldFlag/QRReaderFieldFlagUtility.h"
#include  "./QRReaderMessageMenu.h"


// =============================================================================
/**/
//==============================================================================
namespace
{
  enum  FinishMode
  {
    FINISH_None,
    FINISH_UserInput,
    FINISH_Timeout,
    FINISH_UserInputOrTimeout
  };


  static const struct MessageDefinition
  {
    u32         messageID;
    bool        isImmediate;
    bool        isYesNo;
    bool        isShowTimerIcon;
    bool        isShowBG;
    FinishMode  finishMode;
    u8          timeoutFrame;
  }messageDefinitions[NetApp::QRReader::MessageMenu::MODE_MAX]  =  {
                           /*  messageID, immediate,  yesNo,  timericon,  BG,     finishMode,   timeoutFrame    */
#define   MSGDEF_Immediate(_id)   {(_id), true,       false,  false,      false,  FINISH_None,  0}
#define   MSGDEF_YN(_id)          {(_id), false,      true,   false,      true,   FINISH_None,  0}
#define   MSGDEF_Timer(_id)       {(_id), true,       false,  true,       true,   FINISH_None,  0}
#define   MSGDEF_TimerNoBG(_id)   {(_id), true,       false,  true,       false,  FINISH_None,  0}
//#define   MSGDEF_Stream(_id)    {(_id), false,  false,  false, false, FINISH_UserInput, 0}
#define   MSGDEF_Stream(_id)      {(_id), false,      false,  false,      true,   FINISH_None,  0}
#define   MSGDEF_StreamNoBG(_id)  {(_id), false,      false,  false,      false,  FINISH_None,  0}


    
    MSGDEF_Immediate(   UINT_MAX),          //    MODE_None
    MSGDEF_YN(          qrscan_win_00_01),  /*  system_msg_18    */  //    MODE_ConfirmTutorial,                   /**<  チュートリアルを表示するか      */
    MSGDEF_Timer(       qrscan_win_00_02),  /*  system_msg_18    */  //    MODE_ActivateCameraInformation,         /**<  カメラ起動                      */
    MSGDEF_Timer(       qrscan_win_00_03),  /*  system_msg_18    */  //    MODE_InactivateCameraInformation,       /**<  カメラ終了                      */
    MSGDEF_Stream(      qrscan_win_01_06),                           //    MODE_CameraBroken,                      /**<  カメラが故障しています          */

    MSGDEF_Immediate(   qrscan_win_01_01),  /*  system_msg_18    */  //    MODE_QRReadGuide,                       /**<  枠内に入れてください            */
    MSGDEF_TimerNoBG(   qrscan_win_01_03),  /*  system_msg_18    */  //    MODE_QRSearching,                       /**<  QRを探しています                */
    MSGDEF_TimerNoBG(   qrscan_win_01_05),  /*  system_msg_18    */  //    MODE_QRAnalyizing,                      /**<  QRを解析しています              */

    MSGDEF_Immediate(   qrscan_win_01_18),  /*  system_msg_18    */  //    MODE_LowBattery,                        /**<  バッテリーがないのでQR読めない  */

    /*
      2016/04/19  Yu Muto
      未使用のため寺地さん要望により削除
    */
//    MSGDEF_YN(          qrscan_win_01_06),  /*  keywait_text_18  */  //    MODE_CannotChargePowerConfirm,          /**<  チャージされないが続けますか    */

    MSGDEF_Stream(      qrscan_win_01_08),  /*  keywait_text_18  */  //    MODE_Found_Pokemon_Alola,               /**<  ポケモンデータ：アローラ種      */
    MSGDEF_Stream(      qrscan_win_01_10),  /*  keywait_text_18  */  //    MODE_Found_Pokemon_NotAlola,            /**<  ポケモンデータ：非アローラ種    */
    MSGDEF_Stream(      qrscan_win_01_09),  /*  keywait_text_18  */  //    MODE_Found_Pokemon_AlreadyIdentified,   /**<  ポケモンデータ：既に載っている  */
    MSGDEF_Stream(      qrscan_win_01_21),  /*  keywait_text_18  */  //    MODE_Found_Pokemon_RecentData,          /**<  ポケモンデータ：最近解析した(履歴ヒット)  */
    MSGDEF_Stream(      qrscan_win_01_23),                           //    MODE_Found_Pokemon_MagianaNew,              /**<  ポケモンデータ：マギアナ新規                            */
    MSGDEF_Stream(      qrscan_win_01_24),                           //    MODE_Found_Pokemon_MagianaUpdate,           /**<  ポケモンデータ：マギアナ図鑑更新                        */
    MSGDEF_Stream(      qrscan_win_01_25),                           //    MODE_Found_Pokemon_MagianaUpdateAfterEvent, /**<  ポケモンデータ：マギアナ図鑑更新（特殊イベント完了後）  */
    MSGDEF_Stream(      qrscan_win_01_29),                           //    MODE_Found_Pokemon_M20PikatyuNew,              /**<  ポケモンデータ：M20ピカチュウ新規                            */
    MSGDEF_Stream(      qrscan_win_01_30),                           //    MODE_Found_Pokemon_M20PikatyuUpdate,           /**<  ポケモンデータ：M20ピカチュウ図鑑更新                        */
    MSGDEF_Stream(      qrscan_win_01_25),                           //    MODE_Found_Pokemon_M20PikatyuUpdateAfterEvent, /**<  ポケモンデータ：M20ピカチュウ図鑑更新（特殊イベント完了後）  */

    MSGDEF_Stream(      qrscan_win_01_20),                           //    MODE_Found_Pokemon_CannotAnalyze,       /**<  ポケモンデータ：まだ解析できない（未クリア時マギアナ）  */
    MSGDEF_Stream(      qrscan_win_01_28),                           //    MODE_Found_M20Pikatyu_CannotAnalyze,    /**<  ポケモンデータ：まだ解析できない（島2未到達M20ピカチュウ）  */

    MSGDEF_Stream(      qrscan_win_01_26),                           //    MODE_Found_NormalData,                  /**<  一般：ミラクルQRを読み込んだ      */
    MSGDEF_Stream(      qrscan_win_01_27),                           //    MODE_Found_PokedexData,                 /**<  図鑑：アローラ図鑑QRを読み込んだ  */
    MSGDEF_Stream(      qrscan_win_01_19),                           //    MODE_Found_PromotionalData,             /**<  連動QRを読み込んだ              */

    MSGDEF_Stream(      qrscan_win_01_12),  /*  keywait_text_18  */  //    MODE_DataNotAvailable_Console,          /**<  この本体では解析できない        */
    MSGDEF_Stream(      qrscan_win_01_13),  /*  keywait_text_18  */  //    MODE_DataNotAvailable_App,              /**<  このゲームでは解析できない      */


    MSGDEF_Stream(      qrscan_win_01_21),  /*  keywait_text_18  */  //    MODE_DataIsAlreadyRead,                 /**<  既に読み込んだQRだ              */

    MSGDEF_StreamNoBG(  qrscan_win_01_14),  /*  keywait_text_18  */  //    MODE_Charged,                           /**<  n%チャージされた                */
    MSGDEF_StreamNoBG(  qrscan_win_01_15),  /*  keywait_text_18  */  //    MODE_ScanPowerTotal,                    /**<  合計でn%になった                */
    
    /*
      2016/04/25  Yu Muto
      未使用のため寺地さん要望により削除
    */
//    MSGDEF_Stream(      qrscan_win_01_16),  /*  keywait_text_18  */  //    MODE_CannotCharge,                      /**<  フィールドスキャン発動中        */
    MSGDEF_StreamNoBG(  qrscan_win_01_17),  /*  keywait_text_18  */  //    MODE_ScanPowerFullCharged,              /**<  100%になった                    */
    MSGDEF_StreamNoBG(  qrscan_win_01_22),  /*  keywait_text_18  */  //    MODE_ScanPowerAlreadyFull,              /**<  既に100%                        */

    MSGDEF_Immediate(   qrscan_win_02_01),  /*  system_msg_18    */  //    MODE_FieldScan_Available,               /**<  フィールドスキャン有効          */
    MSGDEF_StreamNoBG(  qrscan_win_02_03),  /*  system_msg_18    */  //    MODE_FieldScan_NotAvailable,            /**<  フィールドスキャン無効（ポイントが足りないため） */
    MSGDEF_TimerNoBG(   qrscan_win_02_02),  /*  system_msg_18    */  //    MODE_FieldScan_Scanning,                /**<  探しています                    */

    /** 
      開始しますかのtextがない
      @note 仕様的に不要となった（長押しによる発動のため）
    */
    MSGDEF_YN(        qrscan_win_02_05),  /*  system_msg_18    */  //    MODE_FieldScan_StartConfirm,            /**<  スキャン開始しますか            */

    MSGDEF_Stream(    qrscan_win_02_06),  /*  keywait_text_18  */  //    MODE_FieldScan_Found,                   /**<  見つかった                      */
    MSGDEF_Stream(    qrscan_win_02_07),  /*  keywait_text_18  */  //    MODE_FieldScan_NotFound,                /**<  見つからなかった                */
    MSGDEF_Immediate( qrscan_win_02_05),  /*  system_msg_18    */  //    MDOE_FieldScan_LetsHunt,                /**<  気配を探そう                    */

    MSGDEF_Stream(    qrscan_win_01_02),  /*  keywait_text_18  */  //    MODE_FieldScanPowerEmpty,               /**<  パワーがなくなったので中断      */

    /*  analyze  */
    MSGDEF_Stream(    qrscan_win_03_07),   //    MODE_QRNG_FriendMatchData,              /**<  NG:なかま大会のデータ           */
    MSGDEF_Stream(    qrscan_win_03_08),   //    MODE_QRNG_LiveMatchData,                /**<  NG:ライブ大会のデータ           */
    MSGDEF_Stream(    qrscan_win_03_09),   //    MODE_QRNG_TeamData,                     /**<  NG:バトルチームのデータ         */
    MSGDEF_Stream(    qrscan_win_03_10),   //    MODE_QRNG_ZukanData,                    /**<  NG:図鑑のデータ                 */
    MSGDEF_Stream(    qrscan_win_03_14),   //    MODE_QRNG_GameSyncID,                   /**<  NG:GameSyncID mismatch          */

    MSGDEF_Stream(    qrscan_win_03_16),   //    MODE_QRNG_Region,                       /**<  NG:invalid region               */

    MSGDEF_Stream(    qrscan_win_01_13),   //    MODE_QRNG_ROM,                          /**<  NG:invalid ROM                  */
    MSGDEF_Stream(    qrscan_win_03_06),   //    MODE_QROK,                              /**<  OK:データを読み取った           */


    MSGDEF_Stream(    qrscan_win_03_12),   //    MODE_QRSearch_FriendMatchData,          /**<  なかま大会のデータを読んで      */
    MSGDEF_Stream(    qrscan_win_03_13),   //    MODE_QRSearch_LiveMatchData,            /**<  ライブ大会のデータを読んで      */
    MSGDEF_Stream(    qrscan_win_03_11),   //    MODE_QRSearch_TeamData,                 /**<  チームデータを読んで            */

#undef    MSGDEF_Immediate
#undef    MSGDEF_YN
#undef    MSGDEF_Timer
#undef    MSGDEF_TimerNoBG
#undef    MSGDEF_Stream
  };


  static const gfl2::fs::ArcFile::ARCDATID  MESSAGE_ARCDAT  = NetApp::QRReader::QRREADER_MESSAGE_GRAC_ID;
}


namespace NetApp    {
namespace QRReader  {
// =============================================================================
/**/
//==============================================================================
//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageMenu::MessageMenu(QRReaderWork* pWork, NetAppLib::UI::NetAppMessageMenu::IEventHandler* pHandler)
  : NetAppLib::UI::NetAppMessageMenu(pWork, pHandler)
  , m_pWork(pWork)
  , m_Mode(MODE_None)
  , m_GainValue(0)
  , m_IslandNameMessageID(qrscan_sys_02_02)
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
MessageMenu::~MessageMenu()
{
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  *  @param    
  *  @return   
  */
//------------------------------------------------------------------
void  MessageMenu::Show(const Mode mode)
{
  const MessageDefinition&  rMessageDefinition  = messageDefinitions[mode];

  if(m_Mode == mode)  return;


  m_Mode  = mode;

  PreShow();
  if(!m_pWork->GetMessageUtility()->IsValidMessageID(MESSAGE_ARCDAT, rMessageDefinition.messageID)) return;

  switch(rMessageDefinition.finishMode)
  {
    case  FINISH_None:                SetFinishMode_None();                                             break;
    case  FINISH_UserInput:           SetFinishMode_UserInput();                                        break;
    case  FINISH_Timeout:             SetFinishMode_Timer(rMessageDefinition.timeoutFrame);             break;
    case  FINISH_UserInputOrTimeout:  SetFinishMode_UserInputOrTimer(rMessageDefinition.timeoutFrame);  break;
  }

  if(rMessageDefinition.isYesNo)
  {
    ShowYN(
      rMessageDefinition.messageID,
      rMessageDefinition.isImmediate
    );
  }
  else
  {

    if((mode == MODE_Charged) || (mode == MODE_ScanPowerTotal))
    {
      /*  ScanPowerExpand  */
      const u32 digits  = 3;
      m_pWork->GetMessageUtility()->GetWordSet()->RegisterNumber(0, m_GainValue, digits, print::NUM_DISP_LEFT, print::NUM_CODE_HANKAKU);

      App::Tool::MessageMenuView::ShowMessage(
        &m_pWork->GetMessageUtility()->GetExpandedString(QRREADER_MESSAGE_GRAC_ID, rMessageDefinition.messageID),
        rMessageDefinition.isImmediate,
        rMessageDefinition.isShowTimerIcon,
        rMessageDefinition.isShowBG
        );
    }
    else if(mode == MODE_FieldScan_Scanning)
    {
      gfl2::str::MsgData*   pMessageData  = m_pWork->GetMessageUtility()->GetData(QRREADER_MESSAGE_GRAC_ID);

      m_pWork->GetMessageUtility()->GetWordSet()->RegisterWord(0, pMessageData, m_IslandNameMessageID);
      /*  islandnameExpand  */
      App::Tool::MessageMenuView::ShowMessage(
        &m_pWork->GetMessageUtility()->GetExpandedString(QRREADER_MESSAGE_GRAC_ID, rMessageDefinition.messageID),
        rMessageDefinition.isImmediate,
        rMessageDefinition.isShowTimerIcon,
        rMessageDefinition.isShowBG
        );
    }
    else
    {
      ShowMessage(
        MESSAGE_ARCDAT,
        rMessageDefinition.messageID,
        rMessageDefinition.isImmediate,
        rMessageDefinition.isShowTimerIcon,
        rMessageDefinition.isShowBG
      );
    }
  }
}

//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    showの前に共通でやっておくこと
  */
//------------------------------------------------------------------
void  MessageMenu::PreShow(void)
{
  SetYNListMode(false);
  SetFinishMode_None();
}


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenu::ShowYN(const u32 messageID, const bool isImmediate)
{
  SetYNListMode(true);
  ShowMessageWithListMenu(
    MESSAGE_ARCDAT,
    messageID,
    isImmediate,
    MESSAGE_ARCDAT,
    qrscan_sel_01_01,
    qrscan_sel_01_02
  );
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
void  MessageMenu::SetFieldScanIslandID(const u32 islandID)
{
  m_IslandNameMessageID = FieldFlagUtility::IslandIDtoIslandNameMessageID(islandID);
}


} /*  namespace QRReader  */
} /*  namespace NetApp    */
