//==============================================================================
/**
 * @file   GiftFrameDef.cpp
 * @date   2015/07/31 Fri. 15:01:23
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./GiftFrameDef.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================



//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(const NetAppLib::System::FrameResult frameResult)
{
  switch(frameResult)
  {
    /*  mainmenu  */
    case  RESULT_MAIN_Gift:                 return FRAME_Receive;
    case  RESULT_MAIN_Album:                return FRAME_CardAlbum;
    case  RESULT_MAIN_Instruction:          return FRAME_Instruction;

    /*  instruction  */
    case  RESULT_INSTRUCTION_Exit:          return FRAME_Main;

    /*    */
    case  RESULT_CARDALBUM_CleanupDone:     return FRAME_SelectData;
    case  RESULT_CARDALBUM_Exit:            return FRAME_Main;

    /*  receive  */
    case  RESULT_Receive_Exit:              return FRAME_Main;
    case  RESULT_Receive_Ir:                return FRAME_ReceiveIr;         /**<  赤外線でうけとる            */
    case  RESULT_Receive_UDS:               return FRAME_SelectData;        /**<  ローカル通信で受け取「った」*/
    case  RESULT_Receive_Internet:          return FRAME_ReceiveInternet;   /**<  インターネットで受け取る    */
    case  RESULT_Receive_Serial:            return FRAME_ReceiveSerial;     /**<  シリアルコードで受け取る    */
    case  RESULT_Receive_Password:          return FRAME_Main;              /**<  あいことばで受け取る        */

    /*  Ir  */
    case  RESULT_RECEIVE_IR_Exit:           return FRAME_Main;
    case  RESULT_RECEIVE_IR_Received:       return FRAME_SelectData;

    /*  receiveInternet  */
    case  RESULT_RECEIVE_INTERNET_Exit:     return FRAME_Main;
    case  RESULT_RECEIVE_INTERNET_Received: return FRAME_SelectData;

    /*  receiveSerial  */
    case  RESULT_RECEIVE_SERIAL_Exit:       return FRAME_Main;
    case  RESULT_RECEIVE_SERIAL_Received:   return FRAME_SelectData;
    case  RESULT_RECEIVE_SERIAL_TransitToEshopPatchPage:  return FRAME_INVALID;

    /*  SelectData  */
    case  RESULT_SELECTDATA_Exit:           return FRAME_Main;
    case  RESULT_SELECTDATA_AlbumCleanup:   return FRAME_CardAlbum; /**<  アルバム整理  */
    case  RESULT_SELECTDATA_SelectedItem:   return FRAME_INVALID;   /*  demo再生のため一旦Procを抜ける  */

    /*  receiveResult  */
    case  RESULT_RECEIVERESULT_Exit:        return FRAME_SelectData;
    case  RESULT_RECEIVERESULT_Nodata:      return FRAME_Main;



    case  RESULT_MAIN_Exit:   /*  through  */
    default:
      return  FRAME_INVALID;
  }
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
