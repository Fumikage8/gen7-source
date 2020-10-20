
#if !defined(QRREADERPROC_H_INCLUDED)
#define QRREADERPROC_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderProc.h
 * @date   2016/01/14 Thu. 11:20:33
 * @author muto_yu
 * @brief  QR読み取り
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
#include  "../source/QRReaderFrameDef.h"
#include  "./QRReaderProcParam.h"

namespace NetApp    {
namespace QRReader  {
  class ApplicationSystem;
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  *  @class  
  *  @brief  
  *  @date   2016/01/14 Thu. 11:21:43
  */
//==============================================================================
class QRReaderProc
  : public  GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN(QRReaderProc);
public:
  QRReaderProc( APP_PARAM* pAppParam );
  virtual ~QRReaderProc();
  
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

  APP_PARAM*          m_pAppParam;              /**<  起動パラメータ    */
  ApplicationSystem*  m_pApplicationSystem;     /**<  ApplicationSystem */
  QRReaderWork*       m_pWork;

};

} /*  namespace QRReader  */
} /*  namespace NetApp    */
#endif  /*  #if !defined(QRREADERPROC_H_INCLUDED)  */

