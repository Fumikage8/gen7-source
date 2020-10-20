
#if !defined(EVOLUTIONAPPLICATIONSYSTEM_H_INCLUDED)
#define EVOLUTIONAPPLICATIONSYSTEM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionApplicationSystem.h
 * @date   2015/08/28 Fri. 17:39:29
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


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
  class EvolutionViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  ApplicationSystem
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
  ApplicationSystem(EvolutionWork* pWork);


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



  EvolutionWork*                                                m_pWork;
  EvolutionViewContainer*                                       m_pViewContainer;

  gfl2::renderingengine::scenegraph::resource::ResourceNode*    m_pSceneGraphResourceNode;
  poke_3d::model::BaseModel*                                    m_pBGModel;

  struct
  {
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode*   pDrawEnvNode;
    poke_3d::model::BaseCamera*                                 pCamera;
  }m_UpperCameraInfo, m_LowerCameraInfo;


};




} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONAPPLICATIONSYSTEM_H_INCLUDED)  */


