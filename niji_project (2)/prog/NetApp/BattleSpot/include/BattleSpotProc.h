
#if !defined(BATTLESPOTPROC_H_INCLUDED)
#define BATTLESPOTPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   BattleSpotProc.h
 * @date   2015/10/02 Fri. 11:52:01
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

#include  <GameSys/include/GameProc.h>
#include  "../source/BattleSpotFrameDef.h"
#include  "./BattleSpotProcParam.h"


namespace NetApp      {
namespace BattleSpot  {
  class ApplicationSystem;
  class BattleSpotWork;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/10/02 Fri. 11:53:27
  */
//==============================================================================
class BattleSpotProc
  : public  GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(BattleSpotProc);

public:
  BattleSpotProc( APP_PARAM* pAppParam );
  virtual ~BattleSpotProc();

  /*  overfide GameSys::GameProc I/F  */
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void               PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void               DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

protected:
  FrameID   GetFirstFrameID(void) const;

  APP_PARAM*          m_pAppParam;              /**<  起動パラメータ  */
  ApplicationSystem*  m_pApplicationSystem;     /**<  ApplicationSystem */
  BattleSpotWork*     m_pWork;
};


} /*  namespace BattleSpot  */
} /*  namespace NetApp      */
#endif  /*  #if !defined(BATTLESPOTPROC_H_INCLUDED)  */

