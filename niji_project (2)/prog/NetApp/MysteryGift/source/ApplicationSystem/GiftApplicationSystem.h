#if !defined(GIFTAPPLICATIONSYSTEM_H_INCLUDED)
#define GIFTAPPLICATIONSYSTEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   GiftApplicationSystem.h
 * @date   2015/07/31 Fri. 15:09:51
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
#include  "NetStatic/NetAppLib/include/Util/NetAppPokeModelUtility.h"

#include  "../Common/AuthenticationHelper.h"
#include  "../Debug/GiftDebug.h"


namespace NetApp  {
namespace Gift    {
  class GiftWork;
  class GiftViewContainer;
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
  ApplicationSystem(GiftWork* pWork);


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


#if PM_DEBUG
    void  DebugDraw(const gfl2::gfx::CtrDisplayNo displayNo);
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

  GiftWork*                                 m_pWork;
  GiftViewContainer*                        m_pViewContainer;
  AuthenticationHelper                      m_AuthHelper;

#if PM_DEBUG
  Debug::GiftDebug                         m_GiftDebug;
#endif

};



} /*  namespace Gift */
} /*  namespace NetApp      */
#endif  /*  #if !defined(GIFTAPPLICATIONSYSTEM_H_INCLUDED)  */


