//==============================================================================
/**
 * @file   EvolutionFrameDef.cpp
 * @date   2015/08/28 Fri. 17:15:21
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./System/EvolutionWork.h"

#include  "./EvolutionFrameDef.h"


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(EvolutionWork* pWork, const NetAppLib::System::FrameResult frameResult)
{
  switch(frameResult)
  {
    /*
      demo
    */
    case  RESULT_EVOLUTIONDMO_Evolved:      return  FRAME_CheckZukan;       /*  進化        */
    case  RESULT_EVOLUTIONDMO_Cancel:       return  FRAME_WaitForDemoEnd;   /*  デモ終了    */


    /*
      checkzukan
    */
    case  RESULT_CHECKZUKAN_NewPokemon:     return  FRAME_WaitForDemoEnd;   /*  図鑑呼び出しのためproc終了  */
    case  RESULT_CHECKZUKAN_KnownPokemon:   return  FRAME_CheckWaza;        /*  既知である                  */


    /*
      checkWaza
    */
    case  RESULT_CHECKWAZA_Forget:          return  FRAME_WaitForDemoEnd;   /*  技忘れ呼び出しのためproc終了  */
    case  RESULT_CHECKWAZA_Continue:        return  FRAME_CheckWaza;
    case  RESULT_CHECKWAZA_Exit:            return  FRAME_WaitForDemoEnd;   /*  proc終了  */

    /*
      waitForDemoEnd
    */
    case  RESULT_WAITDEMO_Exit:             return  FRAME_INVALID;          /*  proc終了  */
  }
  return  FRAME_INVALID; 
}




} /*  namespace Evolution */
} /*  namespace NetApp    */
