/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Prefix.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GR_PREFIX_H_
#define NN_GR_CTR_GR_PREFIX_H_

#include <nn/types.h>
//#include <nn/gx.h>
#include <nn/gx/CTR/gx_CommandAccess.h>

#define NN_GR_ASSERT   NN_ASSERT
#define NN_GR_ERR(...) do { NN_LOG(__VA_ARGS__); int* a = NULL; *a = 1; } while(0)

namespace nn
{
    namespace gr
    {
        namespace CTR
        {
            /*!
                @brief 頂点ストリームのデータの型です。型は @ref PicaDataVertexAttrType です。
            */
            typedef PicaDataVertexAttrType PicaDataVertexArrayType;

            /*!
                @brief LookUp Table の種類の型です。
                型は @ref PicaDataFragLightSampler です。
            */
            typedef PicaDataFragLightSampler PicaDataLookUpTableType;

            /*!
                @brief フラグメントライトのテクスチャの型です。型は @ref PicaDataFragLightEnvTexture です。
            */
            typedef PicaDataFragLightEnvTexture PicaDataFragLightTexture;

            /*!
                @brief フラグメントライトのレイヤーコンフィグの型です。型は @ref PicaDataFragLightEnvLayerConfig です。
            */
            typedef PicaDataFragLightEnvLayerConfig PicaDataFragLightLayerConfig;

            /*!
                @brief フラグメントライトのバンプマップの型です。型は @ref PicaDataFragLightEnvBump です。
            */
            typedef PicaDataFragLightEnvBump PicaDataFragLightBump;

            /*!
                @brief 入力する LookUpTable の型です。型は @ref PicaDataFragLightEnvLutInput です。
            */
            typedef PicaDataFragLightEnvLutInput PicaDataLutInput;

            /*!
                @brief LookUpTable のスケールの型です。型は @ref PicaDataFragLightEnvLutScale です。
            */
            typedef PicaDataFragLightEnvLutScale PicaDataLutScale;

        } // namespace CTR
    } // namespace gr
}  // namespace nn

#endif // NN_GR_CTR_GR_PREFIX_H_
