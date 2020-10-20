

#if !defined(UPPERVIEW_MAIN_H_INCLUDED)
#define UPPERVIEW_MAIN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   UpperView_Main.h
 * @date   2016/01/18 Mon. 15:16:08
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



namespace NetApp    {
namespace QRReader {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================
class UpperView_Main
  : public  NetAppLib::UI::NetAppEmptyView
{
  GFL_FORBID_COPY_AND_ASSIGN( UpperView_Main );
public:
  //==============================================================================
  /**
    *  @class  
    *  @brief  
    *  @date   2016/03/07 Mon. 20:55:53
    */
  //==============================================================================
  class IEventHandler
  {
  public:
    enum  EventCode
    {
      /*    */
      EVENT_PokeFrameAnimEnd,

      EVENT_
    };

    virtual ~IEventHandler(){}
    virtual void  UpperView_Main_OnEvent(const EventCode eventCode) = 0;
  };
  void  SetEventHandler(IEventHandler* pHandler)  {m_pHandler = pHandler;}


  //==============================================================================
  /**
    *  @brief  
    */
  //==============================================================================
  class IQRScanPowerProvider
  {
  public:
    virtual u8 QRScanPowerProvider_OnQueryPower(void) = 0;
    virtual ~IQRScanPowerProvider(void)  {}
  };
  void  SetQRScanPowerProvider(IQRScanPowerProvider* pObject) {m_pQRScanPowerProvider = pObject;}


  /*
  */
  enum  ViewMode
  {
    VIEWMODE_None,
    VIEWMODE_Camera,    /**<  カメラ画像    */
    VIEWMODE_OnScan,    /**<  QRスキャン中  */
    VIEWMODE_PokeFrame, /**<  ポケモン登場  */
    
    VIEWMODE_INVALID
  };
  void  SetViewMode(const ViewMode viewMode);


  UpperView_Main(QRReaderWork* pWork);
  virtual ~UpperView_Main();

  /*  implement UIView I/F  */
  virtual void  Update(void);
  virtual void  Draw(gfl2::gfx::CtrDisplayNo displayNo);
//  virtual bool  IsDrawing(void) const;
  /*  endimplement UIView I/F  */

  void  SetBatteryGaugeVisibility(const bool isVisible);
  void  SetBatteryGaugeInvilibeImmediate(void);
  void  SetBatteryGaugeChangeImmediately(void)    {m_IsBatteryGaugeChangeImmediately = true;}   /**<  次回の更新を補間なしに即時に行う  */
  void  SetGaugeEnable(const bool bEnable)        {m_bGaugeEnable = bEnable;}


  gfl2::lyt::LytPicture*  GetCameraImagePane(void)  {return m_pPicturePane_CameraImage;}
  gfl2::lyt::LytPicture*  GetQRImagePane(void)      {return m_pPicturePane_QRImage;}
  gfl2::lyt::LytPane*     GetPokeFramePane(void)    {return m_pPanes[PANE_Win_PokeFrame];}

  void  SetScanCount(const u32 scans);

  void  SetTargetPokemonCoreParam(const pml::pokepara::CoreParam* pTargetPokemonCoreParam)
        {
          if(pTargetPokemonCoreParam)
          {
            m_TargetMonsNo  = pTargetPokemonCoreParam->GetMonsNo();
            m_TargetFormNo  = pTargetPokemonCoreParam->GetFormNo();
          }
          else
          {
            m_TargetMonsNo  = MONSNO_NULL;
            m_TargetFormNo  = 0;
          }
        }

  void  StartShilhouetteAnim(void);
  void  GetSilhouetteColor(gfl2::gfx::Color& rColor) const;

protected:
  void  Event(const IEventHandler::EventCode eventCode);
  void  UpdatePaneSE(void) const;

  enum  TextBoxID
  {
    TEXTBOX_Label_Scans,
    TEXTBOX_Scans,
    
    TEXTBOX_NUM
  };


  enum  PaneID
  {
    PANE_Win_PokeFrame,
    PANE_NULL_QRScanGuide,
    PANE_NULL_CameraImage,
    PANE_Pic_SilhouetteColor,

    /*  SEtiming  */
    PANE_NULL_SE_QRMove,
    PANE_NULL_SE_QREffect,
    PANE_NULL_SE_QRPulseBeat,
    PANE_NULL_SE_QRPulseBeat2,
    PANE_NULL_SE_QRPulseBeat3,
    PANE_NULL_SE_AppearMons,
    PANE_NULL_SE_CryMons,

    PANE_NUM
  };


  /*
    members
  */
  QRReaderWork*                     m_pWork;
  IEventHandler*                    m_pHandler;
  ViewMode                          m_ViewMode;
  IQRScanPowerProvider*             m_pQRScanPowerProvider;
  MonsNo                            m_TargetMonsNo;
  FormNo                            m_TargetFormNo;

  class BatteryGaugeController;
  BatteryGaugeController*           m_pBatteryGaugeControllers;

  class BatteryValueInterp;
  BatteryValueInterp*               m_pBatteryValueInterp;
  bool                              m_IsBatteryGaugeChangeImmediately;

  bool                              m_bNeedNotifyPokeFrameAnimEnd;
  bool                              m_bGaugeEnable;

  /*    */
  gfl2::lyt::LytPicture*            m_pPicturePane_CameraImage;
  gfl2::lyt::LytPicture*            m_pPicturePane_QRImage;
  gfl2::lyt::LytTextBox*            m_pTextBoxes[TEXTBOX_NUM];
  gfl2::lyt::LytPane*               m_pPanes[PANE_NUM];
};


} /*  namespace QRReader */
} /*  namespace NetApp    */
#endif  /*  #if !defined(UPPERVIEW_MAIN_H_INCLUDED)  */


