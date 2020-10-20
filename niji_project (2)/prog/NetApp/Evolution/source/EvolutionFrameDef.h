#if !defined(EVOLUTIONFRAMEDEF_H_INCLUDED)
#define EVOLUTIONFRAMEDEF_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionFrameDef.h
 * @date   2015/08/28 Fri. 17:07:52
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "NetStatic/NetAppLib/include/System/ApplicationSystemTypedef.h"


namespace NetApp    {
namespace Evolution {
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================

//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
enum  FrameID
{
  FRAME_EvolutionDemo    = NetAppLib::System::FRAME_ID_START,
  FRAME_CheckZukan,
  FRAME_CheckWaza,
  FRAME_WaitForDemoEnd,

  FRAME_INVALID = NetAppLib::System::FRAME_ID_INVALID,
  FRAME_STARTUP = FRAME_EvolutionDemo
};


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
enum  FrameResult
{
  /*  evolutionDemo  */
  RESULT_EVOLUTIONDMO_Evolved,    /**<  進化した          */
  RESULT_EVOLUTIONDMO_Cancel,     /**<  キャンセルされた  */

  /*  FRAME_CheckZukan */
  RESULT_CHECKZUKAN_NewPokemon,   /**<  新種である        */
  RESULT_CHECKZUKAN_KnownPokemon, /**<  既知である        */

  /*  FRAME_Waza */
  RESULT_CHECKWAZA_Forget,        /**<  忘れる            */
  RESULT_CHECKWAZA_Continue,      /**<  もう一度          */
  RESULT_CHECKWAZA_Exit,          /**<  技関連完了        */

  /*  FRAME_WaitForDemoEnd */
  RESULT_WAITDEMO_Exit,           /**<  終了した          */

  RESULT_INVALID  = NetAppLib::System::FRAME_RESULT_INVALID,
  RESULT_
};




//------------------------------------------------------------------
/**
  *  @func     
  *  @brief    
  */
//------------------------------------------------------------------
NetAppLib::System::FrameID GetNextFrameIDCore(EvolutionWork* pWork, const NetAppLib::System::FrameResult frameResult);



} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONFRAMEDEF_H_INCLUDED)  */



