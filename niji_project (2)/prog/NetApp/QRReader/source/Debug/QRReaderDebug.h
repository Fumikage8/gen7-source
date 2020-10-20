
#if PM_DEBUG

#if !defined(QRREADERDEBUG_H_INCLUDED)
#define QRREADERDEBUG_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRReaderDebug.h
 * @date   2016/02/05 Fri. 16:33:22
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
#include  <Debug/include/gfl2_DebugWinSystem.h>
#include  <Debug/include/gfl2_DebugWinItemTypes.h>


namespace NetApp    {
namespace QRReader  {
  class QRReaderWork;

namespace Debug {
// =============================================================================
/**/
//==============================================================================
//==============================================================================
/**
  */
//==============================================================================
class QRReaderDebug
{
  GFL_FORBID_COPY_AND_ASSIGN(QRReaderDebug);
public:
  QRReaderDebug(QRReaderWork* pWork);
  virtual ~QRReaderDebug();

  void  OnUpdate(void);
  void  OnDraw(const gfl2::gfx::CtrDisplayNo displayNo);


protected:
  static void FullTank(         void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void SetBattery(       void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void SetScanPower(     void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void SetScanRemaining( void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void ResetTutorial(    void* _pThis, gfl2::debug::DebugWinItem* pItem);
  static void ResetQRHistory(   void* _pThis, gfl2::debug::DebugWinItem* pItem);

  QRReaderWork*                     m_pWork;

  gfl2::debug::DebugWinGroup*       m_pGroup_Root;


  /*  items  */
  enum  {
    VALITEM_Battery,
    VALITEM_ScanPower,
    VALITEM_ScanRemaining,
    VALITEM_CameraForceFatal,
    
    VALITEM_MAX
  };

  struct  ItemInfo
  {
    QRReaderDebug*                          pThis;
    gfl2::debug::DebugWinItemValueU32<u32>* pViewItem;
    u32                                     value;
  };

  ItemInfo  m_ViewItemInfos[VALITEM_MAX];

};



} /*  namespace Debug     */
} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(QRREADERDEBUG_H_INCLUDED)  */

#endif  /*  #if PM_DEMUG  */
