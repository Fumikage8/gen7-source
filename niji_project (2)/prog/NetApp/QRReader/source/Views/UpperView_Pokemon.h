
#if !defined(UPPERVIEW_POKEMON_H_INCLUDED)
#define UPPERVIEW_POKEMON_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Pokemon.h
 * @date   2016/01/18 Mon. 15:37:13
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/UI/NetAppEmptyView.h"


#include  "../Common/FieldFlag/QRReaderFieldFlagUtility.h"

namespace NetApp    {
namespace QRReader {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================
class UpperView_Pokemon
  : public  NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_Pokemon );
public:
/*
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  UpperView_Pokemon_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}
*/



  class IRemainingTimeProvider
  {
  public:
    virtual ~IRemainingTimeProvider(){}
    virtual u32  IRemainingTimeProvider_OnQueryMinutes(void) = 0;
    virtual u32  IRemainingTimeProvider_OnQuerySeconds(void) = 0;
  };
  void  SetRemainingTimeProvider(IRemainingTimeProvider* pObject)  {m_pRemainingTimeProvider = pObject;}


  enum  ViewMode
  {
    VIEWMODE_None,
    VIEWMODE_Discover,
    VIEWMODE_Capture,
    
    VIEWMODE_
  };
  void  SetViewMode(const ViewMode viewMode);



  void  ShowFieldScanResult(const u8 islandID, const Field::ZoneID zoneID, const MonsNo targetMonsNo);
  void  ShowQRReadResult(const pml::pokepara::CoreParam* pCoreParam, bool isSameFormBothGender);

  void  HideResult(void);



  UpperView_Pokemon(QRReaderWork* pWork);
  virtual ~UpperView_Pokemon();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
  /*  endimplement UIView I/F  */


protected:
//  void  Event(const IEventHandler::EventCode eventCode);


  enum  TextboxID
  {
    /*  fieldscan  */
    TEXTBOX_Label_Place,
//    TEXTBOX_Label_RemainTime,

    TEXTBOX_Island,
    TEXTBOX_Area,
    TEXTBOX_RemainTime,

    /*  qrscan  */
//    TEXTBOX_QRScan_Label_Height,
//    TEXTBOX_QRScan_Label_Weight,

    TEXTBOX_QRScan_MonsName,
//    TEXTBOX_QRScan_MonsType,
//    TEXTBOX_QRScan_Height,
//    TEXTBOX_QRScan_Weight,

    TEXTBOX_NUM
  };


  enum  PaneID
  {
    PANE_Null_Discover,
    PANE_Null_Capture,
    PANE_NULL_ScanResultBG,
    
    PANE_GenderIcon_0,
    PANE_GenderIcon_1,
    PANE_StarIcon,

    PANE_NUM
  };

  enum PicturePaneID
  {
    PICTURE_TypeIcon_0,
    PICTURE_TypeIcon_1,

    PICTURE_NUM
  };



  /*
    members
  */
  QRReaderWork*                   m_pWork;
//  IEventHandler*                  m_pHandler;
  IRemainingTimeProvider*         m_pRemainingTimeProvider;
  ViewMode                        m_ViewMode;

  gfl2::lyt::LytTextBox*          m_pTextBoxes[TEXTBOX_NUM];
  gfl2::lyt::LytPicture*          m_pPictures[PICTURE_NUM];
  gfl2::lyt::LytPane*             m_pPanes[PANE_NUM];




};





} /*  namespace QRReader */
} /*  namespace NetApp    */

#endif  /*  #if !defined(UPPERVIEW_POKEMON_H_INCLUDED)  */


