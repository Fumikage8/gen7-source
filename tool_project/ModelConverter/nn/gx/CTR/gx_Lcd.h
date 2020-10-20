/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_Lcd.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_LCD_H_
#define NN_GX_CTR_GX_LCD_H_

#include <nn/types.h>
#include <nn/assert.h>

/*--------------------------------------------------------------------------
    C API
 *-------------------------------------------------------------------------*/
/*!
  @addtogroup  nn_gx    gx
  @{
*/

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

#ifndef NN_GX_DISPLAY0_WIDTH
#define NN_GX_DISPLAY0_WIDTH    240
#endif // NN_GX_DISPLAY0_WIDTH

#ifndef NN_GX_DISPLAY0_HEIGHT
#define NN_GX_DISPLAY0_HEIGHT   400
#endif // NN_GX_DISPLAY0_HEIGHT

#ifndef NN_GX_DISPLAY1_WIDTH
#define NN_GX_DISPLAY1_WIDTH    240
#endif // NN_GX_DISPLAY1_WIDTH

#ifndef NN_GX_DISPLAY1_HEIGHT
#define NN_GX_DISPLAY1_HEIGHT   320
#endif  // NN_GX_DISPLAY1_HEIGHT

/*!--------------------------------------------------------------------------*
  @brief        LCDの画像表示を開始します。
                
  @return       なし。
 *---------------------------------------------------------------------------*/
void nngxStartLcdDisplay( void );

#ifdef  __cplusplus
} // extern "C"
#endif // __cplusplus

/*!
  @}
*/

/*--------------------------------------------------------------------------
    C++ API
 *-------------------------------------------------------------------------*/
#ifdef  __cplusplus
namespace nn {
namespace gx {
namespace CTR {

/*!--------------------------------------------------------------------------*
  @brief        上下LCD の解像度です。
 *---------------------------------------------------------------------------*/
enum DisplaySize
{
    /*! @brief 上LCD の幅（短辺方向の長さ） */
    DISPLAY0_WIDTH  = NN_GX_DISPLAY0_WIDTH,
    /*! @brief 上LCD の高さ（長辺方向の長さ） */
    DISPLAY0_HEIGHT = NN_GX_DISPLAY0_HEIGHT,
    /*! @brief 下LCD の幅（短辺方向の長さ） */
    DISPLAY1_WIDTH  = NN_GX_DISPLAY1_WIDTH,
    /*! @brief 下LCD の高さ（長辺方向の長さ） */
    DISPLAY1_HEIGHT = NN_GX_DISPLAY1_HEIGHT
};

/*!--------------------------------------------------------------------------*
  @brief        LCDの画像表示を開始します。
  
  @return       なし。
 *---------------------------------------------------------------------------*/
inline void StartLcdDisplay( void )
{
    nngxStartLcdDisplay();
}


}}} // namespace nn::gx::CTR
#endif // __cplusplus


#endif /* NN_GX_CTR_GX_LCD_H_ */
