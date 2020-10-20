//
//------------------------------------------------------------
// Copyright(c) 2009-2010 by Digital Media Professionals Inc.
// All rights reserved.
//------------------------------------------------------------
// This source code is the confidential and proprietary
// of Digital Media Professionals Inc.
//------------------------------------------------------------
//
#if defined( GF_PLATFORM_CTR )

#if !defined( GFL2_CTR_TEXTURE_CONVERTER_H_INCLUDE )
#define GFL2_CTR_TEXTURE_CONVERTER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>

#include <nn/config.h>
#include <nn/gx.h>
#include <gles2/gl2ext.h>

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(qr)
GFL_NAMESPACE_BEGIN(ctr)
    
    /*!
        @brief OpenGLのフォーマットのテクスチャ用データをPICAのネイティブフォーマットに変換します。
        
        入力するテクスチャデータと出力するテクスチャデータのデータ型は同じである必要があります。
        サポートしているテクスチャのデータ型はGL_RGBA_NATIVE_DMPはR8G8B8A8(各成分8bitずつ)、
        GL_RGB_NATIVE_DMPはR8G8B8(各成分8bitずつ)、GL_LUMINANCE_ALPHA_NATIVE_DMPはL8A8(各成分8bitずつ)です。
        
        @param[in]    format    テクスチャのデータ型 (GL_RGBA_NATIVE_DMPまたはGL_RGB_NATIVE_DMPまたはGL_LUMINANCE_ALPHA_NATIVE_DMP)
        @param[in]    width     テクスチャのピクセル単位の幅
        @param[in]    height    テクスチャのピクセル単位の高さ        
        @param[in]    glData    OpenGLフォーマットのテクスチャデータへのポインタ
        @param[in]    dmpData   PICAネイティブフォーマットのテクスチャデータへのポインタ(glDataと同じサイズの領域が必要)
        @return      変換に成功したときにtrue、そうでないときにfalseを返します。
    */
    bool ConvertGLTextureToNative(const GLenum format, const u32 width, const u32 height,
        void* glData, void* dmpData);
        
GFL_NAMESPACE_END(ctr)
GFL_NAMESPACE_END(qr)
GFL_NAMESPACE_END(gfl2)


#endif // GFL2_CTR_TEXTURE_CONVERTER_H_INCLUDE

#endif // defined( GF_PLATFORM_CTR )
