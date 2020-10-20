#if !defined(EVOLUTIONFRAME_CHECKZUKAN_H_INCLUDED)
#define EVOLUTIONFRAME_CHECKZUKAN_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionFrame_CheckZukan.h
 * @date   2015/09/10 Thu. 17:00:11
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
#include  "AppLib/include/Util/app_util_AppDemoUtilityHelper.h"

#include  "NetStatic/NetAppLib/include/System/NetAppFrameBase.h"
#include  "../EvolutionFrameDef.h"


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
  class EvolutionViewContainer;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @brief  図鑑登録が必要かどうかをチェックする
  *  @date   2015/09/10 Thu. 17:01:19
  */
//==============================================================================
class Frame_CheckZukan
  : public  NetAppLib::System::NetAppFrameBase
{
  GFL_FORBID_COPY_AND_ASSIGN( Frame_CheckZukan );
public:
  Frame_CheckZukan(EvolutionWork* pWork);
  virtual ~Frame_CheckZukan();

  /*  implement FrameCellSuper I/F  */
//  virtual applib::frame::Result   InitFunc(void);
//  virtual applib::frame::Result   UpdateFunc(void);
  virtual void                    DrawFunc(gfl2::gfx::CtrDisplayNo dispNo);
//  virtual applib::frame::Result   EndFunc(void);
  /*  end implement FrameCellSuper I/F  */

private:
  /*  NetAppLib::System::NetAppFrameBase  */
  virtual bool  startup();
  virtual void  setListener();
  virtual void  removeListener();
  virtual void  updateSequence();
  virtual bool  cleanup();

  void  ExitFrame(const FrameResult = RESULT_CHECKZUKAN_KnownPokemon);


  /*
    members
  */
  EvolutionWork*                    m_pWork;
  EvolutionViewContainer*           m_pViewContainer;
  u32                               m_State;
  FrameResult                       m_Result;
};


} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONFRAME_CHECKZUKAN_H_INCLUDED)  */
