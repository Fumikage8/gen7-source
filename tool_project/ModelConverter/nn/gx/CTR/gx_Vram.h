/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_Vram.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_VRAM_H_
#define NN_GX_CTR_GX_VRAM_H_

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

#ifndef NN_GX_MEM_VRAMA
#define NN_GX_MEM_VRAMA     0x00020000
#endif // NN_GX_MEM_VRAMA

#ifndef NN_GX_MEM_VRAMB
#define NN_GX_MEM_VRAMB     0x00030000
#endif // NN_GX_MEM_VRAMB
    
    /*!--------------------------------------------------------------------------*
      @brief        指定したVRAM領域の先頭アドレスを取得します。
                    
      @param[in]    area    取得するVRAM領域 (NN_GX_MEM_VRAMA もしくは NN_GX_MEM_VRAMB)
      @return       取得したVRAM領域の先頭アドレス。
     *---------------------------------------------------------------------------*/
    uptr nngxGetVramStartAddr( s32 area );
    
    /*!--------------------------------------------------------------------------*
      @brief        指定したVRAM領域の終端アドレスを取得します。
                    
      @param[in]    area    取得するVRAM領域 (NN_GX_MEM_VRAMA もしくは NN_GX_MEM_VRAMB)
      @return       取得したVRAM領域の終端アドレス。
     *---------------------------------------------------------------------------*/
    uptr nngxGetVramEndAddr( s32 area );
    
    /*!--------------------------------------------------------------------------*
      @brief        指定したVRAM領域のサイズを取得します。
                    
      @param[in]    area    取得するVRAM領域 (NN_GX_MEM_VRAMA もしくは NN_GX_MEM_VRAMB)
      @return       取得したVRAM領域のサイズ。
     *---------------------------------------------------------------------------*/
    size_t nngxGetVramSize( s32 area );
    
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
      @enum         VramArea
      @brief        VRAM領域の指定に用います。
     *---------------------------------------------------------------------------*/
    enum VramArea
    {
        /*! @brief  VRAM領域A */
        MEM_VRAMA = NN_GX_MEM_VRAMA,
        /*! @brief  VRAM領域B */
        MEM_VRAMB = NN_GX_MEM_VRAMB
    };
    
    /*!--------------------------------------------------------------------------*
      @brief        指定したVRAM領域の先頭アドレスを取得します。
      
                    VRAM領域の先頭アドレスは必ずこのAPIを用いて取得してください。
                    なお、取得されるアドレスは将来的に変更になる可能性があります。
                    
      @param[in]    area    取得するVRAM領域 (MEM_VRAMA もしくは MEM_VRAMB)
      @return       取得したVRAM領域の先頭アドレス。
     *---------------------------------------------------------------------------*/
    inline uptr GetVramStartAddr( VramArea area )
    {
        return nngxGetVramStartAddr(area);
    }
    
    /*!--------------------------------------------------------------------------*
      @brief        指定したVRAM領域の終端アドレスを取得します。
      
                    VRAM領域の終端アドレスは必ずこのAPIを用いて取得してください。
                    なお、取得されるアドレスは将来的に変更になる可能性があります。
                    
      @param[in]    area    取得するVRAM領域 (MEM_VRAMA もしくは MEM_VRAMB)
      @return       取得したVRAM領域の終端アドレス。
     *---------------------------------------------------------------------------*/
    inline uptr GetVramEndAddr( VramArea area )
    {
        return nngxGetVramEndAddr(area);
    }
    
    /*!--------------------------------------------------------------------------*
      @brief        指定したVRAM領域のサイズを取得します。
      
                    VRAM領域のサイズは必ずこのAPIを用いて取得してください。
                    なお、取得されるサイズは将来的に変更になる可能性があります。
                    
      @param[in]    area    取得するVRAM領域 (MEM_VRAMA もしくは MEM_VRAMB)
      @return       取得したVRAM領域のサイズ。
     *---------------------------------------------------------------------------*/
    inline size_t GetVramSize( VramArea area )
    {
        return nngxGetVramSize(area);
    }

}}} // namespace nn::gx::CTR
#endif // __cplusplus


#endif /* NN_GX_CTR_GX_VRAM_H_ */
