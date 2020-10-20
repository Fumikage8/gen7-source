#if !defined(QRRECORDANDDIARY_H_INCLUDED)
#define QRRECORDANDDIARY_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   QRRecordAndDiary.h
 * @date   2016/02/18 Thu. 17:35:22
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "./QRUtility.h"


namespace NetApp  {
namespace QR      {
// =============================================================================
/**/
//==============================================================================
extern void QRRecordFunc(const QR_TYPE_APP dataType);
extern void FieldScanRecordFunc(void);

extern void QRDiaryFunc(const QR_TYPE_APP         dataType);
extern void QRDiaryFunc(const QR_TYPE_BATTLE_TEAM dataType);
extern void QRDiaryFunc(const QR_TYPE_FRIEND_CUP  dataType);
extern void QRDiaryFunc(const QR_TYPE_LIVE_CUP    dataType);

extern u32  GetQRScanCount(void);

} /*  namespace QR      */
} /*  namespace NetApp  */
#endif  /*  #if !defined(QRRECORDANDDIARY_H_INCLUDED)  */
