/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_CommandAccess.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_COMMAND_ACCESS_H_
#define NN_GX_CTR_GX_COMMAND_ACCESS_H_

#include <nn/types.h>

#include <nn/gx/CTR/gx_MacroReg.h>
#include <nn/gx/CTR/gx_MacroCommon.h>
#include <nn/gx/CTR/gx_MacroFragment.h>
#include <nn/gx/CTR/gx_MacroTexture.h>
#include <nn/gx/CTR/gx_MacroShader.h>
#include <nn/gx/CTR/gx_MacroMisc.h>
#include <nn/gx/CTR/gx_MacroOld.h>

/*--------------------------------------------------------------------------
    C API
 *-------------------------------------------------------------------------*/

#ifdef  __cplusplus
extern "C" {
#endif // __cplusplus

    /*--------------------------------------------------------------------------*
      @brief        仮想アドレスから物理アドレスへの変換を行います。
                    
                    詳細については対応する C++ の API である
                    nn::gx::CTR::GetPhysicalAddr の説明を参照して下さい。
                    
      @param[in]    virtualAddr  変換する論理アドレス
      @return       変換後の物理アドレス
     *---------------------------------------------------------------------------*/
    uptr nngxGetPhysicalAddr( uptr virtualAddr );

    /*--------------------------------------------------------------------------*
      @brief        メインメモリ上の頂点バッファ・テクスチャバッファ等の
                    内容の更新をグラフィックスプロセッサ側に反映させます。
                    
                    詳細については対応する C++ の API である
                    nn::gx::CTR::UpdateBuffer の説明を参照して下さい。
                    
      @param[in]    pBuffer  バッファの先頭へのポインタ
      @param[in]    size     バッファのサイズ
      @return       なし
     *---------------------------------------------------------------------------*/
    void nngxUpdateBuffer( const void* pBuffer, size_t size );

#ifdef  __cplusplus
} // extern "C"
#endif // __cplusplus


/*--------------------------------------------------------------------------
    C++ API
 *-------------------------------------------------------------------------*/
#ifdef  __cplusplus
namespace nn {
namespace gx {
namespace CTR {

    /*!--------------------------------------------------------------------------*
      @brief        仮想アドレスから物理アドレスへの変換を行います。
                    
      @param[in]    virtualAddr  変換する論理アドレス
      @return       変換後の物理アドレス
     *---------------------------------------------------------------------------*/
    inline uptr GetPhysicalAddr( uptr virtualAddr )
    {
        return nngxGetPhysicalAddr(virtualAddr);
    }

    /*!--------------------------------------------------------------------------*
      @brief        メインメモリ上の頂点バッファ・テクスチャバッファ等の
                    内容の更新をグラフィックスプロセッサ側に反映させます。
                    
      @param[in]    pBuffer  バッファの先頭へのポインタ
      @param[in]    size     バッファのサイズ
      @return       なし
     *---------------------------------------------------------------------------*/
    inline void UpdateBuffer( const void* pBuffer, size_t size )
    {
        nngxUpdateBuffer(pBuffer, size);
    }

}}} // namespace nn::gx::CTR
#endif // __cplusplus


#endif /* NN_GX_CTR_GX_COMMAND_ACCESS_H_ */
