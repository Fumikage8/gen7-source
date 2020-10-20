
#if !defined(EVOLUTIONPROC_H_INCLUDED)
#define EVOLUTIONPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionProc.h
 * @date   2015/08/28 Fri. 19:32:07
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>

#include  <GameSys/include/GameProc.h>
#include  "../source/EvolutionFrameDef.h"
#include  "./EvolutionProcParam.h"


namespace NetApp    {
namespace Evolution {
  class ApplicationSystem;
  class EvolutionWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  EvolutionProc
  */
//==============================================================================
class EvolutionProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( EvolutionProc );

public:
  //------------------------------------------------------------------
  /**
    *  @func     EvolutionProc
    *  @brief    ctor
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  EvolutionProc( APP_PARAM* pAppParam );

  //------------------------------------------------------------------
  /**
    *  @func     ~EvolutionProc
    *  @brief    dtor
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual ~EvolutionProc();

  //------------------------------------------------------------------
  /**
    *  @func     InitFunc
    *  @brief    初期化
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     EndFunc
    *  @brief    終了
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     UpdateFunc
    *  @brief    更新
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     PreDrawFunc
    *  @brief    描画準備
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual void               PreDrawFunc( gfl2::proc::Manager* pProcManager );

  //------------------------------------------------------------------
  /**
    *  @func     DrawFunc
    *  @brief    描画
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual void               DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:
  FrameID GetFirstFrameID(void) const;

  APP_PARAM*          m_pAppParam;               /**<  起動パラメータ  */
  ApplicationSystem*  m_pApplicationSystem;     /**<  ApplicationSystem */
  EvolutionWork*      m_pWork;
};


} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONPROC_H_INCLUDED)  */
