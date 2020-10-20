#if defined(GF_PLATFORM_CTR)
/*---------------------------------------------------------------------------*
 File:          helper_RgbImage.h
 Description:   Helper file that manages 2D drawing of RGB textured images.

 Copyright (C)2011 Mobiclip, Inc.
 *---------------------------------------------------------------------------*/

#include <nn.h>
#include <nn/gx/CTR/gx_UniformLocationForFragmentShader.h>
#include <math.h>

/*---------------------------------------------------------------------------*/

#include "helper_RgbImage.h"

/*---------------------------------------------------------------------------*/

using namespace mw::mo::helper;

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

static int       s_InitializedCount = 0;
static GLuint    s_RgbImageProgramID;
static GLuint    s_RgbImageShaderID;

static const float PI_ONE = 3.1415926535897932384626433832795f;

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::InternalCreateTexture (GLenum target)
{
	/*
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_TextureId);

    if (glGetError() != GL_NO_ERROR)
    {
        NN_PANIC("glGenTextures() failed.\n");
    }

    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    if ((m_Format == GL_RGBA_NATIVE_DMP || m_Format == GL_RGB_NATIVE_DMP) && !target)
        target|=NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP;
    target|=GL_TEXTURE_2D;

    glTexImage2D(target, 0, m_Format, static_cast<GLsizei>(m_TextureWidth), static_cast<GLsizei>(m_TextureHeight), 0, m_Format, m_Type, m_Pixels);

    if (glGetError() != GL_NO_ERROR)
    {
        NN_PANIC("glTexImage2D() failed.\n");
    }
	*/
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::InternalDeleteTexture (void)
{
	/*
    glDeleteTextures(1, &m_TextureId);

    if (glGetError() != GL_NO_ERROR)
    {
        NN_PANIC("glDeleteTextures() failed.\n");
    }
	*/
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::Initialize (
	                         const int textureWidth, const int textureHeight,
                             const int imagePosX, const int imagePosY,
                             const int imageWidth, const int imageHeight,
                             const GLenum format, const GLenum type, void* pixels,GLenum target)
{
    // Creates image vertex shader program if not done yet
	/*
    if (s_InitializedCount == 0)
    {    
        u8 shaderBuffer[1024];

        s_RgbImageProgramID = glCreateProgram();
        s_RgbImageShaderID = glCreateShader(GL_VERTEX_SHADER);

        nn::fs::FileInputStream file;
        nn::Result result = file.TryInitialize(L"rom:/ito/shader.bin");
        NN_UTIL_PANIC_IF_FAILED(result);

        size_t fileSize = file.GetSize();
        if (fileSize > sizeof(shaderBuffer))
        {
            NN_PANIC("Shader buffer too small.\n");
        }
        
        s32 read = file.Read(shaderBuffer, fileSize);
        file.Finalize();

        glShaderBinary(1, &s_RgbImageShaderID, GL_PLATFORM_BINARY_DMP, shaderBuffer, read);

        glAttachShader(s_RgbImageProgramID, s_RgbImageShaderID);
        glAttachShader(s_RgbImageProgramID, GL_DMP_FRAGMENT_SHADER_DMP);

        glBindAttribLocation(s_RgbImageProgramID, 0, "InputPosition");
        glBindAttribLocation(s_RgbImageProgramID, 1, "InputTexCoord");

        glLinkProgram(s_RgbImageProgramID);
        glValidateProgram(s_RgbImageProgramID);
    }

    s_InitializedCount++;
	*/
    m_TextureWidth = textureWidth;
    m_TextureHeight = textureHeight;
    m_Format = format;
    m_Type = type;
    m_Pixels = pixels;

    // @fix KW[987]
		/*
	gfl2::grp::RenderColorFormat _format = gfl::grp::RENDER_COLOR_FORMAT_RGB8;
	switch(m_Format)
	{
	case GL_RGBA_NATIVE_DMP:
		_format = gfl2::grp::RENDER_COLOR_FORMAT_RGBA8;
		break;
	case GL_RGB_NATIVE_DMP:
		_format = gfl2::grp::RENDER_COLOR_FORMAT_RGB8;
		break;
	default:
		break;
	}
	*/
//	m_tex = gfl::grp::util::DrawUtil::CreateTexture(_format, textureWidth, textureHeight, pixels);  //@ohno 

	/*
    // Create GL texture object
    InternalCreateTexture(target);

    // Vertex buffer generation for positions
    glGenBuffers(1, &m_VertexPositionID);
    SetImagePosition(0.0f, 0.0f, 2.0f, 2.0f);

    // Vertex buffer generation for texture coordinates
    glGenBuffers(1, &m_VertexTexCoordID);
    SetImageSelection(imagePosX, imagePosY, imageWidth, imageHeight);

    m_OrthoProjection.SetupIdentity();
    SetImageRotation(0.f);

    m_ProjectionMatrixID = glGetUniformLocation(s_RgbImageProgramID, "ProjectionMatrix");
    m_ModelViewMatrixID = glGetUniformLocation(s_RgbImageProgramID, "ModelViewMatrix");
	*/
    m_Transparency = 1.f;
    m_TextureAlpha = false;
    m_TransparencyAlpha = false;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::Finalize (void)
{
    // Delete vertex buffer for positions
    //glDeleteBuffers(1, &m_VertexPositionID);

    // Delete vertex buffer for texture coordinates
    //glDeleteBuffers(1, &m_VertexTexCoordID);

    // Delete GL texture object

  //GFL_DELETE(m_tex);    //@ohno


    m_Pixels = NULL;
}
#if 0
/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::SetImagePosition(float centerX, float centerY, float width, float height)
{
    // Bottom left vertex 
    m_VertexPosition[0].Set(centerX-width/2.0f, centerY-height/2.0f, 0.f);

    // Bottom right vertex 
    m_VertexPosition[1].Set(centerX+width/2.0f, centerY-height/2.0f, 0.f);

    // Top left vertex 
    m_VertexPosition[2].Set(centerX-width/2.0f, centerY+height/2.0f, 0.f);

    // Top right vertex 
    m_VertexPosition[3].Set(centerX+width/2.0f, centerY+height/2.0f, 0.f);

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexPositionID);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(nn::math::VEC3), m_VertexPosition, GL_STATIC_DRAW);
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::SetImageSelection (const int imagePosX, const int imagePosY,
                                  const int imageWidth, const int imageHeight)
{
    // Compute texture coordinate based on image and texture width and height
    f32 s1 = (f32)(imagePosX + imageWidth)/(f32)m_TextureWidth;
    f32 t1 = (f32)(imagePosY + imageHeight)/(f32)m_TextureHeight;
    f32 s0 = (f32)imagePosX/(f32)m_TextureWidth;
    f32 t0 = (f32)imagePosY/(f32)m_TextureHeight;

    // Bottom left texture coordinate
    if (m_Format == GL_RGBA_NATIVE_DMP || m_Format == GL_RGB_NATIVE_DMP)
        m_VertexTexCoord[0].Set(s0, 1.f - t0);
    else
        m_VertexTexCoord[0].Set(s0, t0);

    // Bottom right texture coordinate
    if (m_Format == GL_RGBA_NATIVE_DMP || m_Format == GL_RGB_NATIVE_DMP)
        m_VertexTexCoord[1].Set(s1, 1.f - t0);
    else
        m_VertexTexCoord[1].Set(s1, t0);

    // Top left texture coordinate
    if (m_Format == GL_RGBA_NATIVE_DMP || m_Format == GL_RGB_NATIVE_DMP)
        m_VertexTexCoord[2].Set(s0, 1.f - t1);
    else
        m_VertexTexCoord[2].Set(s0, t1);
    
    // Top right texture coordinate
    if (m_Format == GL_RGBA_NATIVE_DMP || m_Format == GL_RGB_NATIVE_DMP)
        m_VertexTexCoord[3].Set(s1, 1.f - t1);
    else
        m_VertexTexCoord[3].Set(s1, t1);

    glBindBuffer(GL_ARRAY_BUFFER, m_VertexTexCoordID);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(nn::math::VEC2), m_VertexTexCoord, GL_STATIC_DRAW);
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/
#endif
void RgbImage::SetImageRotation (float clockwiseDegreeRotationZ)
{
    m_clockwiseDegreeRotationZ = clockwiseDegreeRotationZ;
    m_ModelView.SetupRotateXyz(nn::math::VEC3(PI_ONE , 0.f, -PI_ONE/2.f + (clockwiseDegreeRotationZ * PI_ONE)/180.f));
}
#if 0
/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::SetModelViewMatrix (nn::math::MTX44& modelView)
{
    m_ModelView = modelView;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::SetProjectionMatrix (nn::math::MTX44& projection)
{
    m_OrthoProjection = projection;
}

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/

void RgbImage::SetTransparency (float transparency)
{
    m_Transparency = transparency;
}
#endif

/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/
#include "heap/include/gfl2_Heap.h"
//#include <grp/util/gfl_UtilDrawUtil.h>    @ohno
void RgbImage::UpdateTexture ()
{

//  glBindTexture(GL_TEXTURE_2D, m_tex->GetTextureHandle());
//	glTexImage2D( GL_TEXTURE_2D, 0, m_Format, m_TextureWidth, m_TextureHeight, 0, m_Format, GL_UNSIGNED_BYTE, m_Pixels );


}
void RgbImage::Draw (void)
{
//	gfl::grp::util::DrawUtil::MaterialInfo material;
//	material.m_ShadingType = gfl::grp::util::DrawUtil::MaterialInfo::TEXTURE_SHADER;
//	material.m_pTexture = m_tex;
//	gfl::grp::util::DrawUtil::SetMaterial(material);
//	gfl::grp::util::DrawUtil::DrawRect(0,0,512,256);
}

/*---------------------------------------------------------------------------*/
#endif// defined(GF_PLATFORM_CTR)
