/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

/*! @file
    @brief    グラフィックスAPIの宣言
*/

#ifndef NN_GX_H_
#define NN_GX_H_

/*! @namespace  nn::gx
    @brief      グラフィックスライブラリの名前空間
*/

/*! @namespace  nn::gx::CTR
    @brief      CTR のグラフィックスライブラリの名前空間です。
                <BR>("::CTR" の部分は省略して記述可能です。)
*/

#if defined(NN_PLATFORM_CTR)
    #include <gles2/gl2.h>
    #include <gles2/gl2ext.h>
    #include <nn/gx/CTR/gx_CTR.h>
    #include <nn/gx/CTR/gx_CTRRaw.h>
    #include <nn/gx/CTR/gx_Vram.h>
    #include <nn/gx/CTR/gx_Lcd.h>
    #include <nn/gx/CTR/gx_Misc.h>
    
#ifdef  __cplusplus
    namespace nn {
        namespace gx {
            using namespace nn::gx::CTR;
        }
    }
#endif // __cplusplus

#else
    #error no platform selected
#endif


#endif  // ifndef NN_GX_H_
