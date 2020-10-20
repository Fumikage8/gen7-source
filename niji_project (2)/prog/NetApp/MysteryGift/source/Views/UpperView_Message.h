#if !defined(UPPERVIEW_MESSAGE_H_INCLUDED)
#define UPPERVIEW_MESSAGE_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Message.h
 * @date   2015/07/31 Fri. 20:45:45
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>

#include  "./Base/UpperView_MessageBase.h"


namespace NetApp  {
namespace Gift    {
  class GiftWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/07/31 Fri. 20:46:29
  */
//==============================================================================
class UpperView_Message
  : public UpperView_MessageBase
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_Message );
public:

  UpperView_Message(GiftWork* pWork);
  virtual ~UpperView_Message();

  /** 表示モード  */
  enum  ViewMode
  {
    /*  mainmenu  */
    VIEWMODE_MainMenu_ChooseFunction,       /**<  機能選択      */

    /*  receive  */
    VIEWMODE_Receive_ReceiveMethod,         /**<  うけとりかたをえらんで  */

    /*  instruction  */
    VIEWMODE_Instruction,                   /**<  聞きたい説明を選んで      */
    VIEWMODE_Instruction_AboutGift,         /**<  おくりものについて        */
    VIEWMODE_Instruction_AboutAlbum,        /**<  アルバムについて          */
    VIEWMODE_Instruction_AboutAlbumFull,    /**<  アルバムがいっぱいのとき  */

    /*  selectData  */
    VIEWMODE_SelectData_ReceivedData,       /**<  受信データ情報                  */

    /*  serial  */
    VIEWMODE_Serial,

    VIEWMODE_NONE
  };




  //------------------------------------------------------------------
  /**
    *  @func     SetupViewMode
    *  @brief    モード切り替え
    */
  //------------------------------------------------------------------
  void  SetViewMode(const ViewMode viewMode);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    
    */
  //------------------------------------------------------------------
  void SetSerialString(const gfl2::str::STRCODE* pSerialStr0, const gfl2::str::STRCODE* pSerialStr1, const gfl2::str::STRCODE* pSerialStr2, const gfl2::str::STRCODE* pSerialStr3);

protected:
  ViewMode  m_ViewMode;
};


} /*  namespace Gift    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(UPPERVIEW_MESSAGE_H_INCLUDED)  */
