#if !defined(GIFTFRAMEDEF_H_INCLUDED)
#define GIFTFRAMEDEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftFrameDef.h
 * @date   2015/07/31 Fri. 14:56:29
 * @author muto_yu
 * @brief  ふしぎなおくりもの Frame関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"


namespace NetApp  {
namespace Gift  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    各フレームのID
  *  @note    NetAppLib::System::FrameIDとの互換を取る
  */
//------------------------------------------------------------------
enum  FrameID
{
  FRAME_Main  = NetAppLib::System::FRAME_ID_START,

  FRAME_Instruction,
  FRAME_CardAlbum,
  FRAME_Receive,
  FRAME_ReceiveIr,
  FRAME_ReceiveInternet,
  FRAME_ReceiveSerial,
  FRAME_SelectData,
  FRAME_ReceiveResult,
  /*  receiveSerial  */
  FRAME_ReceiveSerial_Connection,
  FRAME_ReceiveSerial_InputCode,
  FRAME_ReceiveSerial_Receiving,

  FRAME_INVALID     = NetAppLib::System::FRAME_ID_INVALID,
  FRAME_STARTUP     = FRAME_Main,
  FRAME_AFTER_DEMO  = FRAME_ReceiveResult
};



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief   各フレームの結果定義
  *  @note    NetAppLib::System::FrameResultとの互換を取る
  */
//------------------------------------------------------------------
enum  FrameResult
{
  /*  main  */
  RESULT_MAIN_Exit,
  RESULT_MAIN_Gift,
  RESULT_MAIN_Album,
  RESULT_MAIN_Instruction,

  /*  instruction  */
  RESULT_INSTRUCTION_Exit,

  /*  album  */
  RESULT_CARDALBUM_CleanupDone,
  RESULT_CARDALBUM_Exit,

  /*  receive  */
  RESULT_Receive_Exit,
  RESULT_Receive_Ir,        /**<  赤外線でうけとる            */
  RESULT_Receive_UDS,       /**<  ローカル通信で受け取「った」*/
  RESULT_Receive_Internet,  /**<  インターネットで受け取る    */
  RESULT_Receive_Serial,    /**<  シリアルコードで受け取る    */
  RESULT_Receive_Password,  /**<  あいことばで受け取る        */

  /*  receiveIr  */
  RESULT_RECEIVE_IR_Exit,
  RESULT_RECEIVE_IR_Received,

  /*  receiveInternet  */
  RESULT_RECEIVE_INTERNET_Received,
  RESULT_RECEIVE_INTERNET_Exit,

  /*  receiveSerial  */
  RESULT_RECEIVE_SERIAL_Exit,
  RESULT_RECEIVE_SERIAL_Received,
  RESULT_RECEIVE_SERIAL_TransitToEshopPatchPage,

  /*  selectData  */
  RESULT_SELECTDATA_Exit,
  RESULT_SELECTDATA_AlbumCleanup, /**<  アルバム整理  */
  RESULT_SELECTDATA_SelectedItem,

  /*  receiveResult  */
  RESULT_RECEIVERESULT_Exit,      /**<  継続終了、次の受信        */
  RESULT_RECEIVERESULT_Nodata,    /**<  受信データがないので終了  */

  /*  ReceiveSerial_Connection  */
  RESULT_RECEIVESERIAL_CONNECTION_Exit,

  /*  ReceiveSerial_InputCode   */
  RESULT_RECEIVESERIAL_INPUTCODE_Exit,

  /*  ReceiveSerial_Receiving   */
  RESULT_RECEIVESERIAL_RECEIVING_Exit,


  RESULT_INVALID  = NetAppLib::System::FRAME_RESULT_INVALID,
  RESULT_
};


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(const NetAppLib::System::FrameResult frameResult);



} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(GIFTFRAMEDEF_H_INCLUDED)  */
