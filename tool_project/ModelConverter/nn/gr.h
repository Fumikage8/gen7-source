/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

/*! @file
    @brief    グラフィックス GPUレジスタアクセスAPIの宣言
*/

#ifndef NN_GR_H_
#define NN_GR_H_

/*! @namespace  nn::gr
    @brief      GPU レジスタアクセスライブラリの名前空間
*/

/*! @namespace  nn::gr::CTR
    @brief      CTR の GPU レジスタアクセスライブラリの名前空間です。
                <BR>("::CTR" の部分は省略して記述可能です。)
*/

#if defined(NN_PLATFORM_CTR)
    #include <nn/gr/CTR/gr_Shader.h>
    #include <nn/gr/CTR/gr_ShaderLite.h>
    #include <nn/gr/CTR/gr_BindSymbol.h>
    #include <nn/gr/CTR/gr_Vertex.h>
    #include <nn/gr/CTR/gr_Combiner.h>
    #include <nn/gr/CTR/gr_RenderState.h>
    #include <nn/gr/CTR/gr_Texture.h>
    #include <nn/gr/CTR/gr_FragmentLight.h>
    #include <nn/gr/CTR/gr_LookUpTable.h>
    #include <nn/gr/CTR/gr_Viewport.h>
    #include <nn/gr/CTR/gr_Scissor.h>
    #include <nn/gr/CTR/gr_Fog.h>
    #include <nn/gr/CTR/gr_ProcedureTexture.h>
    #include <nn/gr/CTR/gr_FrameBuffer.h>

#ifdef  __cplusplus
    namespace nn {
        namespace gr {
            using namespace nn::gr::CTR;
        }
    }
#endif // __cplusplus

#else
    #error no platform selected
#endif

#endif
