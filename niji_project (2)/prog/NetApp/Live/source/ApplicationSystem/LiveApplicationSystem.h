#if !defined(LIVEAPPLICATIONSYSTEM_H_INCLUDED)
#define LIVEAPPLICATIONSYSTEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveApplicationSystem.h
 * @date   2015/07/09 Thu. 15:03:03
 * @author muto_yu
 * @brief  Live大会ApplicationSystem
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemBase.h"


namespace NetApp  {
namespace Live {
  class LiveWork;
  class LiveViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  ApplicationSystem
  *  @brief  ApplicationSystem
  *  @date   2015/07/02 Thu. 13:09:19
  */
//==============================================================================
class ApplicationSystem
  : public NetAppLib::System::ApplicationSystemBase
{
  GFL_FORBID_COPY_AND_ASSIGN( ApplicationSystem );
public:
  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  ApplicationSystem(LiveWork* pWork);


  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~ApplicationSystem();




  /*
    super
  */


//  virtual void Build( NetAppLib::System::IApplicationSystemBuilder* pApplicationSystemBuilder );
//  virtual bool Loading();
//  virtual bool Update();
//  virtual void PreDraw();
//  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );
//  virtual bool End();





private:
  /*  super  */
  virtual void LoadResourceRequest();
  virtual void CreateMessageData();
  virtual void CreateView();
  virtual void AddModel();
  virtual void RemoveModel();
  virtual void SetupCamera();
  virtual void UnsetCamera();
  virtual void UpdateView();
  virtual void DrawView( gfl2::gfx::CtrDisplayNo displayNo );
  virtual bool IsExitView();
//  virtual void UpdateNet();



  LiveWork*                                                     m_pWork;
  LiveViewContainer*                                            m_pViewContainer;
};




} /*  namespace Live */
} /*  namespace NetApp      */
#endif  /*  #if !defined(LiveAPPLICATIONSYSTEM_H_INCLUDED)  */
