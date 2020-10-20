
#if !defined(PHOTOVIEWPROC_H_INCLUDED)
#define PHOTOVIEWPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   PhotoViewProc.h
 * @date   2017/02/07 Tue. 12:29:05
 * @author muto_yu
 * @brief  
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
#include  "../source/PhotoViewFrameDef.h"
#include  "./PhotoViewAppParam.h"


namespace NetApp {
namespace PhotoView {
  class ApplicationSystem;
  class PhotoViewWork;
// =============================================================================
/**/
//==============================================================================


//==============================================================================
/**
  */
//==============================================================================
class PhotoViewProc
  : public  GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( PhotoViewProc );

public:
  //------------------------------------------------------------------
  /**
    *  @brief    ctor
    */
  //------------------------------------------------------------------
  PhotoViewProc( APP_PARAM* pAppParam );
  //------------------------------------------------------------------
  /**
    *  @brief    dtor
    */
  //------------------------------------------------------------------
  virtual ~PhotoViewProc();

  /** proc  */
  virtual gfl2::proc::Result InitFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result EndFunc( gfl2::proc::Manager* pProcManager );
  virtual gfl2::proc::Result UpdateFunc( gfl2::proc::Manager* pProcManager );
  virtual void               PreDrawFunc( gfl2::proc::Manager* pProcManager );
  virtual void               DrawFunc( gfl2::proc::Manager* pProcManager, gfl2::gfx::CtrDisplayNo displayNo );

private:
  FrameID GetFirstFrameID(void);

  APP_PARAM*          m_pAppParam;             /**<  起動パラメータ  */
  ApplicationSystem*  m_pApplicationSystem;    /**<  ApplicationSystem */
  PhotoViewWork*      m_pWork;
};




}  /*  namespace PhotoView  */
}  /*  namespace NetApp  */
#endif  /*  #if !defined(PHOTOVIEWPROC_H_INCLUDED)  */
