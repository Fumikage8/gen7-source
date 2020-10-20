//==============================================================================
/**
 * @file   UpperView_Message.cpp
 * @date   2015/07/31 Fri. 20:49:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "../GiftResource.h"
#include  "./UpperView_Message.h"

#include  "../ApplicationSystem/GiftWork.h"


namespace NetApp  {
namespace Gift    {
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Message::UpperView_Message(GiftWork* pWork)
  : UpperView_MessageBase(pWork)
  , m_ViewMode(VIEWMODE_NONE)
{
}


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
UpperView_Message::~UpperView_Message()
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
void  UpperView_Message::SetSerialString(const gfl2::str::STRCODE* pSerialStr0, const gfl2::str::STRCODE* pSerialStr1, const gfl2::str::STRCODE* pSerialStr2, const gfl2::str::STRCODE* pSerialStr3)
{
  GFL_ASSERT(pSerialStr0);
  GFL_ASSERT(pSerialStr1);
  GFL_ASSERT(pSerialStr2);
  GFL_ASSERT(pSerialStr3);


  GetG2DUtil()->SetTextBoxPaneString( m_pTextBoxes[TEXTBOX_Serial_1of4], pSerialStr0 );
  GetG2DUtil()->SetTextBoxPaneString( m_pTextBoxes[TEXTBOX_Serial_2of4], pSerialStr1 );
  GetG2DUtil()->SetTextBoxPaneString( m_pTextBoxes[TEXTBOX_Serial_3of4], pSerialStr2 );
  GetG2DUtil()->SetTextBoxPaneString( m_pTextBoxes[TEXTBOX_Serial_4of4], pSerialStr3 );

}

//------------------------------------------------------------------
/**
  *  @func     SetupViewMode
  *  @brief    モード切り替え
  */
//------------------------------------------------------------------
void  UpperView_Message::SetViewMode(const ViewMode viewMode)
{
  switch(viewMode)
  {
    case  VIEWMODE_MainMenu_ChooseFunction:
      SetLayoutMode(LAYOUT_MessageLarge);
      SetText(TEXTBOX_Large, msg_mystery_exp_01);   /*  ようこそ  */
      break;


    case  VIEWMODE_Receive_ReceiveMethod:         /**<  うけとりかたをえらんで  */
      SetLayoutMode(LAYOUT_MessageSmall);
      SetText(TEXTBOX_1of2, msg_mystery_cap_32);   /*  「おくりものをうけとる」  */
      SetText(TEXTBOX_2of2, msg_mystery_cap_33);   /*  受け取り方を選んで  */
      break;


    case  VIEWMODE_Instruction:
      SetLayoutMode(LAYOUT_MessageSmall);
      SetText(TEXTBOX_1of2, msg_mystery_cap_03);   /*  「せつめいを きく」  */
      SetText(TEXTBOX_2of2, msg_mystery_cap_04);   /*  聞きたい説明を選んで  */
      break;

    case  VIEWMODE_Instruction_AboutGift:         /**<  おくりものについて        */
      SetLayoutMode(LAYOUT_MessageSmall);
      SetText(TEXTBOX_1of2, msg_mystery_cap_03);   /*  「せつめいを きく」  */
      SetText(TEXTBOX_2of2, msg_mystery_cap_29);   /*  おくりものについて  */
      break;

    case  VIEWMODE_Instruction_AboutAlbum:        /**<  アルバムについて          */
      SetLayoutMode(LAYOUT_MessageSmall);
      SetText(TEXTBOX_1of2, msg_mystery_cap_03);   /*  「せつめいを きく」  */
      SetText(TEXTBOX_2of2, msg_mystery_cap_30);   /*  アルバムについて  */
      break;

    case  VIEWMODE_Instruction_AboutAlbumFull:    /**<  アルバムがいっぱいのとき  */
      SetLayoutMode(LAYOUT_MessageSmall);
      SetText(TEXTBOX_1of2, msg_mystery_cap_03);   /*  「せつめいを きく」  */
      SetText(TEXTBOX_2of2, msg_mystery_cap_31);   /*  いっぱいのとき  */
      break;

    /*
      selectData
    */
    case  VIEWMODE_SelectData_ReceivedData:
      SetLayoutMode(LAYOUT_ReceivedData);
      break;

    /*
      serial
    */
    case  VIEWMODE_Serial:
      SetLayoutMode(LAYOUT_SerialCode);
      break;

    case  VIEWMODE_NONE:    /*  through */
    default:
      break;
  }
}



} /*  namespace Gift    */
} /*  namespace NetApp  */
