#if !defined(LIVEPROC_H_INCLUDED)
#define LIVEPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   LiveProc.h
 * @date   2015/07/09 Thu. 12:50:33
 * @author muto_yu
 * @brief  ライブ大会proc
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <macro/include/gfl2_Macros.h>

#include  <GameSys/include/GameProc.h>
#include  "./LiveAppParam.h"
#include  "../source/LiveFrameDef.h"

// =============================================================================
/**/
//==============================================================================


namespace NetApp  {
namespace Live {
  class ApplicationSystem;
  class LiveWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  *  @class  LiveProc
  *  @brief  ライブ大会Proc
  *  @date   2015.05.13 Wed.
  */
//==============================================================================
class LiveProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( LiveProc );

public:
  //------------------------------------------------------------------
  /**
    *  @func     LiveProc
    *  @brief    ctor
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  LiveProc( APP_PARAM* pAppParam );

  //------------------------------------------------------------------
  /**
    *  @func     ~LiveProc
    *  @brief    dtor
    *  @param    
    *  @return   
    */
  //------------------------------------------------------------------
  virtual ~LiveProc();

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
  FrameID GetFirstFrameID(void);

  APP_PARAM*          m_pAppParam;             /**<  起動パラメータ  */
  ApplicationSystem*  m_pApplicationSystem;    /**<  ApplicationSystem */
  LiveWork*           m_pWork;
};


} /*  namespace Live    */
} /*  namespace NetApp  */
#endif  /*  #if !defined(LIVEPROC_H_INCLUDED)  */


