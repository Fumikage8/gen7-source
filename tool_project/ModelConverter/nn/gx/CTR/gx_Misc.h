/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gx_Misc.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_MISC_H_
#define NN_GX_CTR_GX_MISC_H_

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

    typedef enum nngxMemAccessPrioMode
    {
        NN_GX_MEM_ACCESS_PRIO_MODE_0 = 0x00000000,
        NN_GX_MEM_ACCESS_PRIO_MODE_1 = 0x00000205,
        NN_GX_MEM_ACCESS_PRIO_MODE_2 = 0x00000203,
        NN_GX_MEM_ACCESS_PRIO_MODE_3 = 0x00000305,
        NN_GX_MEM_ACCESS_PRIO_MODE_4 = 0x00000105,
        
        NN_GX_MEM_ACCESS_PRIO_MODE_MAX = 0x7FFFFFFF
    } nngxMemAccessPrioMode;
    
    typedef enum nngxInternalDriverPrioMode
    {
        NN_GX_INTERNAL_DRIVER_PRIO_MODE_HIGH,
        NN_GX_INTERNAL_DRIVER_PRIO_MODE_LOW
    } nngxInternalDriverPrioMode;
    
    /*!--------------------------------------------------------------------------*
      @brief        メインメモリ(FCRAM)へのアクセス優先度の設定を行います。
                    
                    詳細については対応する C++ の API である
                    nn::gx::CTR::SetMemAccessPrioMode の説明を参照して下さい。
                    
      @param[in]    mode    アクセス優先度設定モード
      @return       なし。
     *---------------------------------------------------------------------------*/
    void nngxSetMemAccessPrioMode( nngxMemAccessPrioMode mode );


    /*!--------------------------------------------------------------------------*
      @brief        立体視表示が許可されているかどうかを取得します。
                    
                    詳細については対応する C++ の API である
                    nn::gx::CTR::IsStereoVisionAllowed の説明を参照して下さい。
                    
      @return       立体視表示が許可されているなら true、そうでなければ false を返します。
     *---------------------------------------------------------------------------*/
    bool nngxIsStereoVisionAllowed( void );


    /*!--------------------------------------------------------------------------*
      @brief       グラフィックスドライバ内部動作の優先度を設定します。
                   
                   詳細については対応する C++ の API である
                   nn::gx::CTR::SetInternalDriverPrioMode の説明を参照して下さい。
                   
      @param[in]   mode    内部動作の優先度モード
      @return      なし。
     *---------------------------------------------------------------------------*/
    void nngxSetInternalDriverPrioMode( nngxInternalDriverPrioMode mode );

    /*---------------------------------------------------------------------------*
      :private
     *---------------------------------------------------------------------------*/
    void nngxSetAutoWaitMode( s32 paramIndex );

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
      @enum         MemAccessPrioMode
      @brief        メインメモリへのアクセス優先度の設定値です。
     *---------------------------------------------------------------------------*/
    enum MemAccessPrioMode
    {
        /*! @brief  CPU, GPU, その他デバイスが全て均等に扱われます。*/
        MEM_ACCESS_PRIO_MODE_0 = NN_GX_MEM_ACCESS_PRIO_MODE_0,
        /*! @brief  CPU からのアクセスが優先されます。*/
        MEM_ACCESS_PRIO_MODE_1 = NN_GX_MEM_ACCESS_PRIO_MODE_1,
        /*! @brief  CPU からのアクセスが強く優先されます。*/
        MEM_ACCESS_PRIO_MODE_2 = NN_GX_MEM_ACCESS_PRIO_MODE_2,
        /*! @brief  CPU と GPU からのアクセスが優先されます。*/
        MEM_ACCESS_PRIO_MODE_3 = NN_GX_MEM_ACCESS_PRIO_MODE_3,
        /*! @brief  GPU からのアクセスが優先されます。*/
        MEM_ACCESS_PRIO_MODE_4 = NN_GX_MEM_ACCESS_PRIO_MODE_4,
        
        MEM_ACCESS_PRIO_MODE_MAX = NN_GX_MEM_ACCESS_PRIO_MODE_MAX
    };

    /*!--------------------------------------------------------------------------*
      @brief        メインメモリ(FCRAM)へのアクセス優先度の設定を行います。
                    
      @param[in]    mode    アクセス優先度設定モード
      @return       なし。
     *---------------------------------------------------------------------------*/
    inline void SetMemAccessPrioMode( MemAccessPrioMode mode )
    {
        nngxSetMemAccessPrioMode(static_cast<nngxMemAccessPrioMode>(mode));
    }

    /*!--------------------------------------------------------------------------*
      @brief        立体視表示が許可されているかどうかを取得します。
                    
      @return       立体視表示が許可されているなら true、そうでなければ false を返します。
     *---------------------------------------------------------------------------*/
    inline bool IsStereoVisionAllowed( void )
    {
        return nngxIsStereoVisionAllowed();
    }

    /*!--------------------------------------------------------------------------*
      @enum         InternalDriverPrioMode
      @brief        グラフィックスドライバ内部動作の優先度の設定値です。
     *---------------------------------------------------------------------------*/
    enum InternalDriverPrioMode
    {
        /*! @brief グラフィックスドライバを高優先度で動作させます。*/
        INTERNAL_DRIVER_PRIO_MODE_HIGH = NN_GX_INTERNAL_DRIVER_PRIO_MODE_HIGH,
        /*! @brief グラフィックスドライバを低優先度で動作させます。*/
        INTERNAL_DRIVER_PRIO_MODE_LOW  = NN_GX_INTERNAL_DRIVER_PRIO_MODE_LOW
    };

    /*!--------------------------------------------------------------------------*
      @brief       グラフィックスドライバ内部動作の優先度を設定します。
      
      @param[in]   mode    内部動作の優先度モード
      @return      なし。
     *---------------------------------------------------------------------------*/
    inline void SetInternalDriverPrioMode( InternalDriverPrioMode mode )
    {
        nngxSetInternalDriverPrioMode(static_cast<nngxInternalDriverPrioMode>(mode));
    }

    /*---------------------------------------------------------------------------*
      :private
     *---------------------------------------------------------------------------*/
    inline void SetAutoWaitMode( s32 paramIndex )
    {
        nngxSetAutoWaitMode(paramIndex);
    }

}}} // namespace nn::gx::CTR
#endif // __cplusplus

/*---------------------------------------------------------------------------*/

#endif /* NN_GX_CTR_GX_MISC_H_ */
