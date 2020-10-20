
#if !defined(EGGHATCHINGPROC_H_INCLUDED)
#define EGGHATCHINGPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EggHatchingProc.h
 * @date   2015/11/24 Tue. 12:05:01
 * @author muto_yu
 * @brief  タマゴ孵化
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_macros.h>
#include  <types/include/gfl2_Typedef.h>

#include  <GameSys/include/GameProc.h>
#include  "../source/EggHatchingFrameDef.h"
#include  "./EggHatchingProcParam.h"


namespace NetApp      {
namespace EggHatching {
  class ApplicationSystem;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2015/11/24 Tue. 12:06:55
  */
//==============================================================================
class EggHatchingProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( EggHatchingProc );

public:
  EggHatchingProc( APP_PARAM* pAppParam );
  virtual ~EggHatchingProc();

  /*
    gameproc I/F
  */
  virtual gfl2::proc::Result InitFunc(    gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc(     gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc(  gfl2::proc::Manager* pProcManager );
  virtual void               PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void               DrawFunc(    gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:
  FrameID GetFirstFrameID(void) const;

  APP_PARAM*          m_pAppParam;               /**<  起動パラメータ  */
  ApplicationSystem*  m_pApplicationSystem;     /**<  ApplicationSystem */
  EggHatchingWork*    m_pWork;
};


} /*  namespace EggHatching */
} /*  namespace NetApp      */
#endif  /*  #if !defined(EGGHATCHINGPROC_H_INCLUDED)  */
