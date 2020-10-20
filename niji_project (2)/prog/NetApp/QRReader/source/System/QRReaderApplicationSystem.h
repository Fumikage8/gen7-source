
#if !defined(QRREADERAPPLICATIONSYSTEM_H_INCLUDED)
#define QRREADERAPPLICATIONSYSTEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderApplicationSystem.h
 * @date   2016/01/14 Thu. 17:08:10
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"
#include  "NetStatic/NetAppLib/include/Util/NetAppPokeModelutility.h"

#include  "AppLib/include/Tool/AppCommonGrpIconData.h"

#if PM_DEBUG
  #include  "../Debug/QRReaderDebug.h"
#endif

namespace NetApp   {
namespace QRReader {
  class QRReaderWork;
  class QRReaderViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  ApplicationSystem
  */
//==============================================================================
class ApplicationSystem
  : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ApplicationSystem );
public:
  ApplicationSystem(QRReaderWork* pWork);
  virtual ~ApplicationSystem();

  /*
    super
  */
//  virtual void Build( NetAppLib::System::IApplicationSystemBuilder* pApplicationSystemBuilder );
  virtual bool Loading();
//  virtual bool Update();
//  virtual void PreDraw(){}
//  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){}
//  virtual bool End();

#if PM_DEBUG
  void DebugDraw( gfl2::gfx::CtrDisplayNo displayNo );
#endif

private:
  /*  super  */
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
//  virtual void AddModel();
//  virtual void RemoveModel();
//  virtual void SetupCamera();
//  virtual void UnsetCamera();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();
//  virtual void UpdateNet();

  /*  members  */
  QRReaderWork*                             m_pWork;
  QRReaderViewContainer*                    m_pViewContainer;
  NetAppLib::Util::NetAppPokeModelUtility*  m_pPokeModelUtil;

  u32                                       m_LoadState;


  app::tool::AppCommonGrpIconData           m_CommonIconData;
  bool                                      m_IsCommonIconDataTerminated;

#if PM_DEBUG
  Debug::QRReaderDebug                      m_Debug;
#endif
};



} /*  namespace QRReader */
} /*  namespace NetApp   */
#endif  /*  #if !defined(QRREADERAPPLICATIONSYSTEM_H_INCLUDED)  */
