
#if !defined(EGGHATCHINGAPPLICATIONSYSTEM_H_INCLUDED)
#define EGGHATCHINGAPPLICATIONSYSTEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingApplicationSystem.h
 * @date   2015/11/24 Tue. 12:27:15
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


namespace NetApp      {
namespace EggHatching {
  class EggHatchingWork;
  class EggHatchingViewContainer;
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
  //------------------------------------------------------------------
  /**
    *  @func     
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  ApplicationSystem(EggHatchingWork* pWork);


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
//  virtual void PreDraw(){}
//  virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo ){}
//  virtual bool End();


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
  EggHatchingWork*          m_pWork;
  EggHatchingViewContainer* m_pViewContainer;
};



} /*  namespace EggHatching */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EGGHATCHINGAPPLICATIONSYSTEM_H_INCLUDED)  */
