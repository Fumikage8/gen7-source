
#if !defined(QRREADERBUILDER_H_INCLUDED)
#define QRREADERBUILDER_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderBuilder.h
 * @date   2016/01/14 Thu. 17:13:08
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
#include  "NetStatic/NetAppLib/include/System/IApplicationSystemBuilder.h"


namespace NetApp    {
namespace QRReader {
  class QRReaderWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class QRReaderBuilder
  : public NetAppLib::System::IApplicationSystemBuilder
{
  GFL_FORBID_COPY_AND_ASSIGN( QRReaderBuilder );

public:
  QRReaderBuilder(
    QRReaderWork*            pWork,
    NetAppLib::System::FrameID  startFrameID
  );
  virtual ~QRReaderBuilder();


  /*
    implement IApplicationSystemBuilder I/F
  */
  virtual NetAppLib::System::IApplicationFrameSwitcher*   CreateApplicationFrameSwitcher(void);
  virtual NetAppLib::System::IApplicationFrameFactory*    CreateApplicationFrameFactory(void);

//  virtual u32 CreateAppRenderingManagerCount();
  virtual app::util::AppRenderingManager::Description*    CreateAppRenderingManagerDescription();
//  virtual app::util::AppRenderingManager::Description* CreateAppRenderingManagerDescriptions();

  virtual NetAppLib::System::ApplicationWorkBase*         GetApplicationWork();
  virtual NetAppLib::System::FrameID                      GetApplicationStartFrameID(void);
  virtual u32                                             GetApplicationResourceCount(void);
  virtual bool                                            IsCreateMessageUtility()  {return true;}
//  virtual void                                            GetMessageUtilityParameter( u32& dataNum, u32& wordMax, u32& bufLen );

  //------------------------------------------------------------------
  /**
    *  @brief    UIViewマネージャのボタン監視モード
    */
  //------------------------------------------------------------------
  virtual bool IsEnableInputExclusiveKeyMode(){ return true; }

private:
  QRReaderWork*               m_pWork;          /**<  ワーク        */
  NetAppLib::System::FrameID  m_StartFrameID;   /**<  StartFrameID  */

};


} /*  namespace QRReader */
} /*  namespace NetApp   */
#endif  /*  #if !defined(QRREADERBUILDER_H_INCLUDED)  */
