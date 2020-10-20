#if !defined(PHOTOVIEWAPPLICATIONSYSTEM_H_INCLUDED)
#define PHOTOVIEWAPPLICATIONSYSTEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewApplicationSystem.h
 * @date   2017/02/07 Tue. 13:13:27
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <NetStatic/NetAppLib/include/System/ApplicationSystemBase.h>


namespace NetApp  {
namespace PhotoView {
  class PhotoViewWork;
  class PhotoViewViewContainer;
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
  ApplicationSystem(PhotoViewWork* pWork);


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

  PhotoViewWork*            m_pWork;
  PhotoViewViewContainer*   m_pViewContainer;
};




} /*  namespace PhotoView */
} /*  namespace NetApp      */
#endif  /*  #if !defined(PHOTOVIEWAPPLICATIONSYSTEM_H_INCLUDED)  */
