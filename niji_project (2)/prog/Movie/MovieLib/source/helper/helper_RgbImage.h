#if defined(GF_PLATFORM_CTR)
#if !defined( __HELPER_RGBIMAGE_H__ )
#define __HELPER_RGBIMAGE_H__
/*---------------------------------------------------------------------------*
 File:          helper_RgbImage.h
 Description:   Helper file that manages 2D drawing of RGB textured images.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#ifndef helper_RgbImage2d_h
#define helper_RgbImage2d_h

/*---------------------------------------------------------------------------*/

#include <nn/types.h>
#include <nn/math.h>
#include <nn/gx.h>

//#include <gflib.h>
#include "heap/include/gfl2_Heap.h"
//#include <grp/util/gfl_UtilDrawUtil.h>
#include <util/include/gfl2_DrawUtilText.h>

/*---------------------------------------------------------------------------*
  C++ API interface
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus

/*---------------------------------------------------------------------------*/

namespace mw {
namespace mo {
namespace helper {

/*---------------------------------------------------------------------------*/

class RgbImage
{
    private:
    
    //GLuint          m_TextureId;
    //GLuint          m_VertexPositionID;
    //GLuint          m_VertexTexCoordID;
    //GLuint          m_ProjectionMatrixID;
    //GLuint          m_ModelViewMatrixID;
    int             m_TextureWidth;
    int             m_TextureHeight;
    GLenum          m_Format;
    GLenum          m_Type;
    void*           m_Pixels;
    //nn::math::VEC3  m_VertexPosition[4];
    //nn::math::VEC2  m_VertexTexCoord[4];
    //nn::math::MTX44 m_OrthoProjection;
    nn::math::MTX44 m_ModelView;
    GLfloat         m_Transparency;
    bool            m_TextureAlpha;
    bool            m_TransparencyAlpha;
    int:16;
//	gfl2::grp::util::Texture* m_tex;    //@ohno
    float           m_clockwiseDegreeRotationZ;
    void InternalCreateTexture (GLenum target);
    void InternalDeleteTexture (void);

    public:

    RgbImage() : m_TextureWidth(0), m_TextureHeight(0), m_Format(0), m_Type(0), m_Pixels(NULL), m_ModelView(),  m_Transparency(0), m_TextureAlpha(false), m_TransparencyAlpha(false), m_clockwiseDegreeRotationZ(0) {}

    void Initialize (
					 const int textureWidth, const int textureHeight,
                     const int imagePosX, const int imagePosY,
                     const int imageWidth, const int imageHeight,
                     const GLenum format, const GLenum type, void* pixels,GLenum target=0);
    void Finalize   (void);
/*
    void SetImagePosition  (float centerX, float centerY, float width, float height);
    void SetImageSelection (const int imagePosX, const int imagePosY,
                            const int imageWidth, const int imageHeight);*/
    void SetImageRotation  (float clockwiseDegreeRotationZ);
    /*
    void SetModelViewMatrix  (nn::math::MTX44& modelView);
    void SetProjectionMatrix (nn::math::MTX44& projection);
    void SetTransparency     (float transparency);
    void EnableAlpha         (void) { m_TransparencyAlpha=true; }
    void DisableAlpha        (void) { m_TransparencyAlpha=false; }
    void EnableTextureAlpha  (void) { m_TextureAlpha=true; }
    void DisableTextureAlpha (void) { m_TextureAlpha=false; }
    */

	void UpdateTexture();
    void Draw (void);

    const void* GetPixels(void) const { return m_Pixels; }
    float GetRotationZ(void) const { return m_clockwiseDegreeRotationZ; }
};

/*---------------------------------------------------------------------------*/

} // end of namespace helper
} // end of namespace mo
} // end of namespace mw

/*---------------------------------------------------------------------------*/

#endif  // ifdef __cplusplus

/*---------------------------------------------------------------------------*/

#endif  // ifndef helper_RgbImage_h

/*---------------------------------------------------------------------------*/

#endif // __HELPER_RGBIMAGE_H__
#endif// defined(GF_PLATFORM_CTR)
