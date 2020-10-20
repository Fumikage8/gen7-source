#ifndef __gl2ext_h_
#define __gl2ext_h_

/* $Id: gl2ext.h 4571 2007-11-28 16:33:30Z benj $ */

#ifdef __cplusplus
extern "C" {
#endif

/*
** License Applicability. Except to the extent portions of this file are
** made subject to an alternative license as permitted in the SGI Free
** Software License B, Version 1.0 (the "License"), the contents of this
** file are subject only to the provisions of the License. You may not use
** this file except in compliance with the License. You may obtain a copy
** of the License at Silicon Graphics, Inc., attn: Legal Services, 1600
** Amphitheatre Parkway, Mountain View, CA 94043-1351, or at:
**
** http://oss.sgi.com/projects/FreeB
**
** Note that, as provided in the License, the Software is distributed on an
** "AS IS" basis, with ALL EXPRESS AND IMPLIED WARRANTIES AND CONDITIONS
** DISCLAIMED, INCLUDING, WITHOUT LIMITATION, ANY IMPLIED WARRANTIES AND
** CONDITIONS OF MERCHANTABILITY, SATISFACTORY QUALITY, FITNESS FOR A
** PARTICULAR PURPOSE, AND NON-INFRINGEMENT.
**
** Original Code. The Original Code is: OpenGL Sample Implementation,
** Version 1.2.1, released January 26, 2000, developed by Silicon Graphics,
** Inc. The Original Code is Copyright (c) 1991-2000 Silicon Graphics, Inc.
** Copyright in any portions created by third parties is as indicated
** elsewhere herein. All Rights Reserved.
**
** Additional Notice Provisions: The application programming interfaces
** established by SGI in conjunction with the Original Code are The
** OpenGL(R) Graphics System: A Specification (Version 1.2.1), released
** April 1, 1999; The OpenGL(R) Graphics System Utility Library (Version
** 1.3), released November 4, 1998; and OpenGL(R) Graphics with the X
** Window System(R) (Version 1.3), released October 19, 1998. This software
** was created using the OpenGL(R) version 1.2.1 Sample Implementation
** published by SGI, but has not been independently verified as being
** compliant with the OpenGL(R) version 1.2.1 Specification.
*/

#ifndef GL_APIENTRYP
#   define GL_APIENTRYP GL_APIENTRY*
#endif

/*------------------------------------------------------------------------*
 * OES extension tokens
 *------------------------------------------------------------------------*/

/* GL_OES_compressed_ETC1_RGB8_texture */
/*
#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_ETC1_RGB8_OES                                        0x8D64
#endif
*/

/* GL_OES_compressed_paletted_texture */
#ifndef GL_OES_compressed_paletted_texture
#define GL_PALETTE4_RGB8_OES                                    0x8B90
#define GL_PALETTE4_RGBA8_OES                                   0x8B91
#define GL_PALETTE4_R5_G6_B5_OES                                0x8B92
#define GL_PALETTE4_RGBA4_OES                                   0x8B93
#define GL_PALETTE4_RGB5_A1_OES                                 0x8B94
#define GL_PALETTE8_RGB8_OES                                    0x8B95
#define GL_PALETTE8_RGBA8_OES                                   0x8B96
#define GL_PALETTE8_R5_G6_B5_OES                                0x8B97
#define GL_PALETTE8_RGBA4_OES                                   0x8B98
#define GL_PALETTE8_RGB5_A1_OES                                 0x8B99
#endif

/* GL_OES_EGL_image */
#ifndef GL_OES_EGL_image
typedef void* GLeglImageOES;
#endif

/* GL_OES_depth24 */
#ifndef GL_OES_depth24
#define GL_DEPTH_COMPONENT24_OES                                0x81A6
#endif

/* GL_OES_depth32 */
/*#ifndef GL_OES_depth32
#define GL_DEPTH_COMPONENT32_OES                                0x81A7
#endif*/

/* GL_OES_mapbuffer */
#ifndef GL_OES_mapbuffer
/* GL_READ_ONLY and GL_READ_WRITE not supported */
#define GL_WRITE_ONLY_OES                                       0x88B9
#define GL_BUFFER_ACCESS_OES                                    0x88BB
#define GL_BUFFER_MAPPED_OES                                    0x88BC
#define GL_BUFFER_MAP_POINTER_OES                               0x88BD
#endif

/* GL_OES_rgb8_rgba8 */
#ifndef GL_OES_rgb8_rgba8
#define GL_RGB8_OES                                             0x8051
#define GL_RGBA8_OES                                            0x8058
#endif

/* GL_OES_stencil1 */
/*#ifndef GL_OES_stencil1
#define GL_STENCIL_INDEX1_OES                                   0x8D46
#endif*/

/* GL_OES_stencil4 */
/*#ifndef GL_OES_stencil4
#define GL_STENCIL_INDEX4_OES                                   0x8D47
#endif*/

/* GL_OES_texture3D */
/*#ifndef GL_OES_texture3D
#define GL_TEXTURE_WRAP_R_OES                                   0x8072
#define GL_TEXTURE_3D_OES                                       0x806F
#define GL_TEXTURE_BINDING_3D_OES                               0x806A
#define GL_MAX_3D_TEXTURE_SIZE_OES                              0x8073
#define GL_SAMPLER_3D_OES                                       0x8B5F
#define GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_OES        0x8CD4
#endif*/

/* GL_OES_texture_half_float */
/*#ifndef GL_OES_texture_half_float
#define GL_HALF_FLOAT_OES                                       0x8D61
#endif*/

/* GL_OES_vertex_half_float */
/* GL_HALF_FLOAT_OES defined in GL_OES_texture_half_float already. */

/* GL_AMD_compressed_3DC_texture */
/*#ifndef GL_AMD_compressed_3DC_texture
#define GL_3DC_X_AMD                                            0x87F9
#define GL_3DC_XY_AMD                                           0x87FA
#endif*/

/* GL_AMD_compressed_ATC_texture */
/*#ifndef GL_AMD_compressed_ATC_texture
#define GL_ATC_RGB_AMD                                          0x8C92
#define GL_ATC_RGBA_EXPLICIT_ALPHA_AMD                          0x8C93
#define GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD                      0x87EE
#endif*/

/* GL_EXT_texture_filter_anisotropic */
/*#ifndef GL_EXT_texture_filter_anisotropic
#define GL_TEXTURE_MAX_ANISOTROPY_EXT                           0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT                       0x84FF
#endif*/

/* LogicOp */
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F

/*------------------------------------------------------------------------*
 * OES extension functions
 *------------------------------------------------------------------------*/

/* GL_OES_compressed_ETC1_RGB8_texture */
#ifndef GL_OES_compressed_ETC1_RGB8_texture
#define GL_OES_compressed_ETC1_RGB8_texture 1
#endif

/* GL_OES_compressed_paletted_texture */
#ifndef GL_OES_compressed_paletted_texture
#define GL_OES_compressed_paletted_texture 1
#endif

/* GL_OES_EGL_image */
#ifndef GL_OES_EGL_image
#define GL_OES_EGL_image 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glEGLImageTargetTexture2DOES (GLenum target, GLeglImageOES image);
GL_APICALL void GL_APIENTRY glEGLImageTargetRenderbufferStorageOES (GLenum target, GLeglImageOES image);
#endif
typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETTEXTURE2DOESPROC) (GLenum target, GLeglImageOES image);
typedef void (GL_APIENTRYP PFNGLEGLIMAGETARGETRENDERBUFFERSTORAGEOESPROC) (GLenum target, GLeglImageOES image);
#endif

/* GL_OES_depth24 */
#ifndef GL_OES_depth24
#define GL_OES_depth24 1
#endif

/* GL_OES_depth32 */
/*#ifndef GL_OES_depth32
#define GL_OES_depth32 1
#endif*/

/* GL_OES_element_index_uint */
/*#ifndef GL_OES_element_index_uint
#define GL_OES_element_index_uint 1
#endif*/

/* GL_OES_fbo_render_mipmap */
#ifndef GL_OES_fbo_render_mipmap
#define GL_OES_fbo_render_mipmap 1
#endif

/* GL_OES_fragment_precision_high */
/*#ifndef GL_OES_fragment_precision_high
#define GL_OES_fragment_precision_high 1
#endif*/

/* GL_OES_mapbuffer */
#ifndef GL_OES_mapbuffer
#define GL_OES_mapbuffer 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void* GL_APIENTRY glMapBufferOES (GLenum target, GLenum access);
GL_APICALL GLboolean GL_APIENTRY glUnmapBufferOES (GLenum target);
GL_APICALL void GL_APIENTRY glGetBufferPointervOES (GLenum target, GLenum pname, void** params);
#endif
typedef void* (GL_APIENTRYP PFNGLMAPBUFFEROESPROC) (GLenum target, GLenum access);
typedef GLboolean (GL_APIENTRYP PFNGLUNMAPBUFFEROESPROC) (GLenum target);
typedef void (GL_APIENTRYP PFNGLGETBUFFERPOINTERVOESPROC) (GLenum target, GLenum pname, void** params);
#endif

/* GL_OES_rgb8_rgba8 */
#ifndef GL_OES_rgb8_rgba8
#define GL_OES_rgb8_rgba8 1
#endif

/* GL_OES_stencil1 */
/*#ifndef GL_OES_stencil1
#define GL_OES_stencil1 1
#endif*/

/* GL_OES_stencil4 */
/*#ifndef GL_OES_stencil4
#define GL_OES_stencil4 1
#endif*/

/* GL_OES_texture_3D */
/*#ifndef GL_OES_texture_3D
#define GL_OES_texture_3D 1
#ifdef GL_GLEXT_PROTOTYPES
GL_APICALL void GL_APIENTRY glTexImage3DOES (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void* pixels);
GL_APICALL void GL_APIENTRY glTexSubImage3DOES (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
GL_APICALL void GL_APIENTRY glCopyTexSubImage3DOES (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
GL_APICALL void GL_APIENTRY glCompressedTexImage3DOES (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
GL_APICALL void GL_APIENTRY glCompressedTexSubImage3DOES (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
GL_APICALL void GL_APIENTRY glFramebufferTexture3DOES (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
#endif
typedef void (GL_APIENTRYP PFNGLTEXIMAGE3DOESPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
typedef void (GL_APIENTRYP PFNGLTEXSUBIMAGE3DOESPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void* pixels);
typedef void (GL_APIENTRYP PFNGLCOPYTEXSUBIMAGE3DOESPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
typedef void (GL_APIENTRYP PFNGLCOMPRESSEDTEXIMAGE3DOESPROC) (GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void* data);
typedef void (GL_APIENTRYP PFNGLCOMPRESSEDTEXSUBIMAGE3DOESPROC) (GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void* data);
typedef void (GL_APIENTRYP PFNGLFRAMEBUFFERTEXTURE3DOES) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
#endif*/

/* GL_OES_texture_float_linear */
/*#ifndef GL_OES_texture_float_linear
#define GL_OES_texture_float_linear 1
#endif*/

/* GL_OES_texture_half_float_linear */
/*#ifndef GL_OES_texture_half_float_linear
#define GL_OES_texture_half_float_linear 1
#endif*/

/* GL_OES_texture_float */
/*#ifndef GL_OES_texture_float
#define GL_OES_texture_float 1
#endif*/

/* GL_OES_texture_half_float */
/*#ifndef GL_OES_texture_half_float
#define GL_OES_texture_half_float 1
#endif*/

/* GL_OES_texture_npot */
/*#ifndef GL_OES_texture_npot
#define GL_OES_texture_npot 1
#endif*/

/* GL_OES_vertex_half_float */
/*#ifndef GL_OES_vertex_half_float
#define GL_OES_vertex_half_float 1
#endif*/

/* GL_AMD_compressed_3DC_texture */
/*#ifndef GL_AMD_compressed_3DC_texture
#define GL_AMD_compressed_3DC_texture 1
#endif*/

/* GL_AMD_compressed_ATC_texture */
/*#ifndef GL_AMD_compressed_ATC_texture
#define GL_AMD_compressed_ATC_texture 1
#endif*/

/* GL_EXT_texture_filter_anisotropic */
/*#ifndef GL_EXT_texture_filter_anisotropic
#define GL_EXT_texture_filter_anisotropic 1
#endif*/

/* GL_EXT_packed_depth_stencil */
#define GL_DEPTH24_STENCIL8_EXT           0x88F0
/* GL_ARB_framebuffer_object */
#define GL_DEPTH_STENCIL_ATTACHMENT       0x821A

#define GL_PLATFORM_BINARY_DMP            0x6000
#define GL_DMP_FRAGMENT_SHADER_DMP        0xFFFFFFFF
#define GL_GEOMETRY_SHADER_DMP            0x6001

#define GL_VERTEX_STATE_COLLECTION_DMP	  0x6800
#define GL_VERTEX_STATE_COLLECTION_BINDING_DMP	  0x6801

#ifndef GL_DMP_texture_collection
#define GL_TEXTURE_COLLECTION_DMP         0x6600
#define GL_TEXTURE_COLLECTION_BINDING_DMP 0x6601
#endif

#ifndef GL_DMP_texture_internal_format
#define GL_LUMINANCEF_DMP                 0x6605
#endif

#ifndef GL_DMP_lut_texture
#define GL_LUT_TEXTURE0_DMP               0x6610
#define GL_LUT_TEXTURE1_DMP               0x6611
#define GL_LUT_TEXTURE2_DMP               0x6612
#define GL_LUT_TEXTURE3_DMP               0x6613
#define GL_LUT_TEXTURE4_DMP               0x6614
#define GL_LUT_TEXTURE5_DMP               0x6615
#define GL_LUT_TEXTURE6_DMP               0x6616
#define GL_LUT_TEXTURE7_DMP               0x6617
#define GL_LUT_TEXTURE8_DMP               0x6618
#define GL_LUT_TEXTURE9_DMP               0x6619
#define GL_LUT_TEXTURE10_DMP              0x661A
#define GL_LUT_TEXTURE11_DMP              0x661B
#define GL_LUT_TEXTURE12_DMP              0x661C
#define GL_LUT_TEXTURE13_DMP              0x661D
#define GL_LUT_TEXTURE14_DMP              0x661E
#define GL_LUT_TEXTURE15_DMP              0x661F
#define GL_LUT_TEXTURE16_DMP              0x6620
#define GL_LUT_TEXTURE17_DMP              0x6621
#define GL_LUT_TEXTURE18_DMP              0x6622
#define GL_LUT_TEXTURE19_DMP              0x6623
#define GL_LUT_TEXTURE20_DMP              0x6624
#define GL_LUT_TEXTURE21_DMP              0x6625
#define GL_LUT_TEXTURE22_DMP              0x6626
#define GL_LUT_TEXTURE23_DMP              0x6627
#define GL_LUT_TEXTURE24_DMP              0x6628
#define GL_LUT_TEXTURE25_DMP              0x6629
#define GL_LUT_TEXTURE26_DMP              0x662A
#define GL_LUT_TEXTURE27_DMP              0x662B
#define GL_LUT_TEXTURE28_DMP              0x662C
#define GL_LUT_TEXTURE29_DMP              0x662D
#define GL_LUT_TEXTURE30_DMP              0x662E
#define GL_LUT_TEXTURE31_DMP              0x662F
#define GL_SAMPLER_1D                     0x8B5D
#define GL_MAX_LUT_TEXTURES_DMP           0x660E
#define GL_MAX_LUT_ENTRIES_DMP            0x660F
#define GL_TEXTURE_BINDING_LUT0_DMP       0x6680
#define GL_TEXTURE_BINDING_LUT1_DMP       0x6681
#define GL_TEXTURE_BINDING_LUT2_DMP       0x6682
#define GL_TEXTURE_BINDING_LUT3_DMP       0x6683
#define GL_TEXTURE_BINDING_LUT4_DMP       0x6684
#define GL_TEXTURE_BINDING_LUT5_DMP       0x6685
#define GL_TEXTURE_BINDING_LUT6_DMP       0x6686
#define GL_TEXTURE_BINDING_LUT7_DMP       0x6687
#define GL_TEXTURE_BINDING_LUT8_DMP       0x6688
#define GL_TEXTURE_BINDING_LUT9_DMP       0x6689
#define GL_TEXTURE_BINDING_LUT10_DMP      0x668A
#define GL_TEXTURE_BINDING_LUT11_DMP      0x668B
#define GL_TEXTURE_BINDING_LUT12_DMP      0x668C
#define GL_TEXTURE_BINDING_LUT13_DMP      0x668D
#define GL_TEXTURE_BINDING_LUT14_DMP      0x668E
#define GL_TEXTURE_BINDING_LUT15_DMP      0x668F
#define GL_TEXTURE_BINDING_LUT16_DMP      0x6690
#define GL_TEXTURE_BINDING_LUT17_DMP      0x6691
#define GL_TEXTURE_BINDING_LUT18_DMP      0x6692
#define GL_TEXTURE_BINDING_LUT19_DMP      0x6693
#define GL_TEXTURE_BINDING_LUT20_DMP      0x6694
#define GL_TEXTURE_BINDING_LUT21_DMP      0x6695
#define GL_TEXTURE_BINDING_LUT22_DMP      0x6696
#define GL_TEXTURE_BINDING_LUT23_DMP      0x6697
#define GL_TEXTURE_BINDING_LUT24_DMP      0x6698
#define GL_TEXTURE_BINDING_LUT25_DMP      0x6699
#define GL_TEXTURE_BINDING_LUT26_DMP      0x669A
#define GL_TEXTURE_BINDING_LUT27_DMP      0x669B
#define GL_TEXTURE_BINDING_LUT28_DMP      0x669C
#define GL_TEXTURE_BINDING_LUT29_DMP      0x669D
#define GL_TEXTURE_BINDING_LUT30_DMP      0x669E
#define GL_TEXTURE_BINDING_LUT31_DMP      0x669F

GL_APICALL void GL_APIENTRY glTexImage1D (GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
GL_APICALL void GL_APIENTRY glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);

#endif

/* GL_DMP_geometry_primitive */
#ifndef GL_DMP_geometry_primitive

#define GL_GEOMETRY_PRIMITIVE_DMP         0x6010

#endif
/* GL_DMP_geometry_primitive */

/* GL_DMP_fragment_operation */
#ifndef GL_DMP_fragment_operation

#define GL_FRAGOP_MODE_GL_DMP             0x6030

#endif
/* GL_DMP_fragment_operation */

/* GL_DMP_shadow */
#ifndef GL_DMP_shadow

#define GL_SHADOW_DMP                     0x6040
#define GL_FRAGOP_MODE_SHADOW_DMP         0x6048
#define GL_TEXTURE_BORDER_COLOR           0x1004
#define GL_CLAMP_TO_BORDER                0x812D
#define GL_GENERATE_MIPMAP                0x8191

#endif
/* GL_DMP_shadow*/

/* texture_lod */
#define GL_TEXTURE_MIN_LOD                0x813A

/* GL_DMP_gas */
#ifndef GL_DMP_gas
#define GL_FOG                            0x0B60	//from OpenGLES1-1 enable cap

#define GL_GAS_DMP                        0x6050
#define GL_FRAGOP_MODE_GAS_ACC_DMP        0x6051

#define GL_GAS_PLAIN_DENSITY_DMP		0x605E
#define GL_GAS_DEPTH_DENSITY_DMP		0x605F
#define GL_GAS_DENSITY_DMP				0x6060
#define GL_GAS_LIGHT_FACTOR_DMP			0x6061

void glGetQueryObjectfv ( GLuint id, GLenum pname, GLfloat *params ) ;

#endif
/* GL_DMP_gas */

#define GL_BGRA                           0x80E1

/* GL_DMP Procedural texture*/
#ifndef GL_DMP_proctex

#define GL_PROCTEX_U_DMP                  0x609A
#define GL_PROCTEX_V_DMP                  0x609B
#define GL_PROCTEX_U2_DMP                 0x609C	//u2
#define GL_PROCTEX_V2_DMP                 0x609D	//v2
#define GL_PROCTEX_ADD_DMP                0x609E	//u+v
#define GL_PROCTEX_ADD2_DMP               0x609F	//u2+v2
#define GL_PROCTEX_ADDSQRT2_DMP	          0x60A1	//u2+v2
#define GL_PROCTEX_MIN_DMP                0x60A2
#define GL_PROCTEX_MAX_DMP                0x60A3
#define GL_PROCTEX_RMAX_DMP               0x60A4
#define GL_PROCTEX_ALPHA_SEPARATE_DMP     0x60A5

#define GL_CLAMP_TO_ZERO_DMP              0x60C0
#define GL_SYMMETRICAL_REPEAT_DMP         0x60C1
#define GL_PULSE_DMP                      0x60C2

#define GL_NONE_DMP                       0x60D0
#define GL_ODD_DMP                        0x60D1
#define GL_EVEN_DMP                       0x60D2

//End of Procedural Texture
#endif
/* GL_DMP Procedural texture*/


/* GL_DMP_sampler_mode */

/* GL_TEXTURE_2D */
/* GL_TEXTURE_CUBE_MAP */
#define GL_TEXTURE_PROJECTION_DMP         0x6E00
#define GL_TEXTURE_SHADOW_2D_DMP          0x6E01
#define GL_TEXTURE_SHADOW_CUBE_DMP        0x6E02
#define GL_TEXTURE_PROCEDURAL_DMP         0x6E03

/* GL_DMP_sampler_mode */


/* GL_DMP_lighting */
#ifndef GL_DMP_lighting

#define GL_FRAGMENT_PRIMARY_COLOR_DMP	  0x6210
#define GL_FRAGMENT_SECONDARY_COLOR_DMP   0x6211

/* parameter values */
/* GL_LIGHT_ENV_LUT_INPUT_SELECTOR_XX_DMP */
#define GL_LIGHT_ENV_NH_DMP               0x62A0
#define GL_LIGHT_ENV_VH_DMP               0x62A1
#define GL_LIGHT_ENV_NV_DMP               0x62A2
#define GL_LIGHT_ENV_LN_DMP               0x62A3
#define GL_LIGHT_ENV_SP_DMP               0x62A4
#define GL_LIGHT_ENV_CP_DMP               0x62A5

/* GL_LIGHT_ENV_LAYER_CONFIG_DMP */
#define GL_LIGHT_ENV_LAYER_CONFIG0_DMP    0x62B0
#define GL_LIGHT_ENV_LAYER_CONFIG1_DMP    0x62B1
#define GL_LIGHT_ENV_LAYER_CONFIG2_DMP    0x62B2
#define GL_LIGHT_ENV_LAYER_CONFIG3_DMP    0x62B3
#define GL_LIGHT_ENV_LAYER_CONFIG4_DMP    0x62B4
#define GL_LIGHT_ENV_LAYER_CONFIG5_DMP    0x62B5
#define GL_LIGHT_ENV_LAYER_CONFIG6_DMP    0x62B6
#define GL_LIGHT_ENV_LAYER_CONFIG7_DMP    0x62B7

/* GL_LIGHT_ENV_FRESNEL_SELECTOR_DMP */
#define GL_LIGHT_ENV_NO_FRESNEL_DMP       0x62C0
#define GL_LIGHT_ENV_PRI_ALPHA_FRESNEL_DMP              0x62C1
#define GL_LIGHT_ENV_SEC_ALPHA_FRESNEL_DMP              0x62C2
#define GL_LIGHT_ENV_PRI_SEC_ALPHA_FRESNEL_DMP          0x62C3

/* GL_LIGHT_ENV_TEXY_USAGE_DMP */
#define GL_LIGHT_ENV_BUMP_NOT_USED_DMP    0x62C8
#define GL_LIGHT_ENV_BUMP_AS_BUMP_DMP     0x62C9
#define GL_LIGHT_ENV_BUMP_AS_TANG_DMP     0x62CA

#endif
/* GL_DMP_lighting */

//Blend Equation
#define GL_MIN                            0x8007
#define GL_MAX                            0x8008

/* GL_DMP_ext_tex_env */
#ifndef GL_DMP_ext_tex_env

#define GL_SUBTRACT                       0x84E7
#define GL_ADD_SIGNED                     0x8574
#define GL_INTERPOLATE                    0x8575
#define GL_CONSTANT                       0x8576
#define GL_PRIMARY_COLOR                  0x8577
#define GL_PREVIOUS                       0x8578
#define GL_PREVIOUS_BUFFER_DMP            0x8579

#define GL_MODULATE                       0x2100
#define GL_ADD                            0x0104
#define GL_DOT3_RGB                       0x86AE
#define GL_DOT3_RGBA                      0x86AF
#define GL_MULT_ADD_DMP                   0x6401
#define GL_ADD_MULT_DMP                   0x6402

#define GL_SRC_R_DMP                      0x8580
#define GL_SRC_G_DMP                      0x8581
#define GL_SRC_B_DMP                      0x8582
#define GL_ONE_MINUS_SRC_R_DMP            0x8583
#define GL_ONE_MINUS_SRC_G_DMP            0x8584
#define GL_ONE_MINUS_SRC_B_DMP            0x8585

#endif
/* GL_DMP_ext_tex_env */

#define GL_HILO8_DMP                      0x6700
#define GL_RGBA_NATIVE_DMP                0x6752
#define GL_RGB_NATIVE_DMP                 0x6754
#define GL_ALPHA_NATIVE_DMP               0x6756
#define GL_LUMINANCE_NATIVE_DMP           0x6757
#define GL_LUMINANCE_ALPHA_NATIVE_DMP     0x6758
#define GL_HILO8_DMP_NATIVE_DMP           0x6759
#define GL_ETC1_RGB8_NATIVE_DMP           0x675A
#define GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP  0x675B
#define GL_SHADOW_NATIVE_DMP              0x675C
#define GL_GAS_NATIVE_DMP                 0x675D

#define GL_UNSIGNED_BYTE_4_4_DMP          0x6760
#define GL_UNSIGNED_4BITS_DMP             0x6761

#define GL_UNSIGNED_INT24_DMP             0x6762

#define GL_MIRROR_CLAMP_TO_EDGE           0x8743
#define GL_MIRROR_CLAMP_TO_BORDER         0x8912
#define GL_TEXTURE_LOD_BIAS               0x8501

GL_APICALL void GL_APIENTRY glGetTexLevelParameterfv (GLenum target, GLint level, GLenum pname, GLfloat *params);
GL_APICALL void GL_APIENTRY glGetTexLevelParameteriv (GLenum target, GLint level, GLenum pname, GLint *params);
#define GL_TEXTURE_WIDTH                  0x1000
#define GL_TEXTURE_HEIGHT                 0x1001
#define GL_TEXTURE_DEPTH                  0x8071
#define GL_TEXTURE_INTERNAL_FORMAT        0x1003
#define GL_TEXTURE_BORDER                 0x1005
#define GL_TEXTURE_RED_SIZE               0x805C
#define GL_TEXTURE_GREEN_SIZE             0x805D
#define GL_TEXTURE_BLUE_SIZE              0x805E
#define GL_TEXTURE_ALPHA_SIZE             0x805F
#define GL_TEXTURE_LUMINANCE_SIZE         0x8060
#define GL_TEXTURE_INTENSITY_SIZE         0x8061
#define GL_TEXTURE_DEPTH_SIZE             0x884A
#define GL_TEXTURE_DENSITY1_SIZE_DMP      0x6770
#define GL_TEXTURE_DENSITY2_SIZE_DMP      0x6771
#define GL_TEXTURE_COMPRESSED             0x86A1
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE  0x86A0

#define GL_ALPHA4_EXT                     0x803B
#define GL_LUMINANCE4_EXT                 0x803F
#define GL_LUMINANCE4_ALPHA4_EXT          0x8043

GL_APICALL void GL_APIENTRY glPolygonMode(GLenum, GLenum);

#define GL_LOGIC_OP_MODE                  0x0BF0
GL_APICALL void GL_APIENTRY glLogicOp (GLenum opcode);

GL_APICALL GLsizei GL_APIENTRY glSaveProgramsDMP    ( GLuint n, GLuint* progs, GLuint flags, GLsizei bufsize, GLvoid* data ) ;
GL_APICALL void    GL_APIENTRY glRestoreProgramsDMP ( GLuint n, GLuint offset, GLuint* progs, GLuint flags, GLvoid* data ) ;

#define GL_SAVE_PROGRAMS_DMP                                        0x00000000
#define GL_SAVE_PROGRAMS_CTR_FORMAT_DMP                             0x00000001
#define GL_RESTORE_PROGRAMS_DMP                                     0x00000000
#define GL_RESTORE_UPDATE_LIGHT0_PROGRAM_STATE_DMP                  0x80000000
#define GL_RESTORE_UPDATE_LIGHT1_PROGRAM_STATE_DMP                  0x40000000
#define GL_RESTORE_UPDATE_LIGHT2_PROGRAM_STATE_DMP                  0x20000000
#define GL_RESTORE_UPDATE_LIGHT3_PROGRAM_STATE_DMP                  0x10000000
#define GL_RESTORE_UPDATE_LIGHT4_PROGRAM_STATE_DMP                  0x08000000
#define GL_RESTORE_UPDATE_LIGHT5_PROGRAM_STATE_DMP                  0x04000000
#define GL_RESTORE_UPDATE_LIGHT6_PROGRAM_STATE_DMP                  0x02000000
#define GL_RESTORE_UPDATE_LIGHT7_PROGRAM_STATE_DMP                  0x01000000
#define GL_RESTORE_UPDATE_LIGHTING_PROGRAM_STATE_DMP                0x00800000
#define GL_RESTORE_UPDATE_MATERIAL_PROGRAM_STATE_DMP                0x00400000
#define GL_RESTORE_UPDATE_TEXTURE_BLENDER_PROGRAM_STATE_DMP         0x00200000
#define GL_RESTORE_UPDATE_TEXTURE_SAMPLER_PROGRAM_STATE_DMP         0x00100000
#define GL_RESTORE_UPDATE_PROCEDURAL_TEXTURE_PROGRAM_STATE_DMP      0x00080000
#define GL_RESTORE_UPDATE_SHADOW_SAMPLING_PROGRAM_STATE_DMP         0x00040000
#define GL_RESTORE_UPDATE_PER_FRAGMENT_OPERATION_PROGRAM_STATE_DMP  0x00020000
#define GL_RESTORE_UPDATE_GAS_ACCUMULATION_PROGRAM_STATE_DMP        0x00010000
#define GL_RESTORE_UPDATE_FOG_PROGRAM_STATE_DMP                     0x00008000
#define GL_RESTORE_UPDATE_VERTEX_SHADER_STATE_DMP                   0x00004000
#define GL_RESTORE_UPDATE_GEOMETRY_SHADER_STATE_DMP                 0x00002000

GL_APICALL GLsizei GL_APIENTRY glSaveVertexStateCollectionsDMP    ( GLuint n, GLuint* vscolls, GLuint flags, GLsizei bufsize, GLvoid* data ) ;
GL_APICALL void    GL_APIENTRY glRestoreVertexStateCollectionsDMP ( GLuint n, GLuint offset, GLuint* vscolls, GLuint flags, GLvoid* data ) ;

#define GL_SAVE_VERTEX_STATE_COLLECTIONS_DMP                        0x00000000
#define GL_SAVE_VERTEX_STATE_COLLECTIONS_CTR_FORMAT_DMP             0x00000001
#define GL_RESTORE_VERTEX_STATE_COLLECTIONS_DMP                     0x00000000

GL_APICALL GLsizei GL_APIENTRY glSaveTextureCollectionsDMP    ( GLuint n, GLuint* txcolls, GLuint flags, GLsizei bufsize, GLvoid* data ) ;
GL_APICALL void    GL_APIENTRY glRestoreTextureCollectionsDMP ( GLuint n, GLuint offset, GLuint* txcolls, GLuint flags, GLvoid* data ) ;

#define GL_SAVE_TEXTURE_COLLECTIONS_DMP                             0x00000000
#define GL_SAVE_TEXTURE_COLLECTION_1D_TEXTURES_DMP                  0x80000000
#define GL_SAVE_TEXTURE_COLLECTION_2D_TEXTURES_DMP                  0x40000000
#define GL_SAVE_TEXTURE_COLLECTION_CUBE_TEXTURES_DMP                0x20000000
#define GL_SAVE_TEXTURE_COLLECTIONS_CTR_FORMAT_DMP                  0x00000001
#define GL_RESTORE_TEXTURE_COLLECTIONS_DMP                          0x00000000
#define GL_RESTORE_TEXTURE_COLLECTION_1D_TEXTURES_DMP               0x80000000
#define GL_RESTORE_TEXTURE_COLLECTION_2D_TEXTURES_DMP               0x40000000
#define GL_RESTORE_TEXTURE_COLLECTION_CUBE_TEXTURES_DMP             0x20000000

/* memory transfer extension */
#define GL_COPY_FCRAM_DMP                 0x01000000
#define GL_NO_COPY_FCRAM_DMP              0x02000000

/* early depth test */
#define GL_EARLY_DEPTH_TEST_DMP           0x6780
#define GL_EARLY_DEPTH_CLEAR_VALUE_DMP    0x6781
#define GL_EARLY_DEPTH_FUNC_DMP           0x6782
#define GL_EARLY_DEPTH_BUFFER_BIT_DMP     0x80000000
GL_APICALL void GL_APIENTRY glClearEarlyDepthDMP (GLuint depth);
GL_APICALL void GL_APIENTRY glEarlyDepthFuncDMP (GLenum func);

/* block mode setting */
#define GL_RENDER_BLOCK_MODE_DMP          0x6788
#define GL_RENDER_BLOCK8_MODE_DMP         0x6789
#define GL_RENDER_BLOCK32_MODE_DMP        0x678A
GL_APICALL void GL_APIENTRY glRenderBlockModeDMP(GLenum mode);

/* multi uniforms */
GL_APICALL void GL_APIENTRY glUniformsDMP(GLuint n, GLint* locations, GLsizei* counts, const GLuint* value);
GL_APICALL void GL_APIENTRY glGetUniformsDMP(GLuint program, GLuint n, GLint* locations, GLsizei* counts, GLuint* params);

/* target definition */
#define DMPGL_2_0_FINAL 1

/* error code */
#define GL_ERROR_SYSBASE_DMP   0x8000
#define GL_ERROR_8000_DMP   (GL_ERROR_SYSBASE_DMP + 0x0000)
#define GL_ERROR_8001_DMP   (GL_ERROR_SYSBASE_DMP + 0x0001)
#define GL_ERROR_8002_DMP   (GL_ERROR_SYSBASE_DMP + 0x0002)
#define GL_ERROR_8003_DMP   (GL_ERROR_SYSBASE_DMP + 0x0003)
#define GL_ERROR_8004_DMP   (GL_ERROR_SYSBASE_DMP + 0x0004)
#define GL_ERROR_8005_DMP   (GL_ERROR_SYSBASE_DMP + 0x0005)
#define GL_ERROR_8006_DMP   (GL_ERROR_SYSBASE_DMP + 0x0006)
#define GL_ERROR_8007_DMP   (GL_ERROR_SYSBASE_DMP + 0x0007)
#define GL_ERROR_8008_DMP   (GL_ERROR_SYSBASE_DMP + 0x0008)
#define GL_ERROR_8009_DMP   (GL_ERROR_SYSBASE_DMP + 0x0009)
#define GL_ERROR_800A_DMP   (GL_ERROR_SYSBASE_DMP + 0x000A)
#define GL_ERROR_800B_DMP   (GL_ERROR_SYSBASE_DMP + 0x000B)
#define GL_ERROR_800C_DMP   (GL_ERROR_SYSBASE_DMP + 0x000C)
#define GL_ERROR_800D_DMP   (GL_ERROR_SYSBASE_DMP + 0x000D)
#define GL_ERROR_800E_DMP   (GL_ERROR_SYSBASE_DMP + 0x000E)
#define GL_ERROR_800F_DMP   (GL_ERROR_SYSBASE_DMP + 0x000F)
#define GL_ERROR_8010_DMP   (GL_ERROR_SYSBASE_DMP + 0x0010)
#define GL_ERROR_8011_DMP   (GL_ERROR_SYSBASE_DMP + 0x0011)
#define GL_ERROR_8012_DMP   (GL_ERROR_SYSBASE_DMP + 0x0012)
#define GL_ERROR_8013_DMP   (GL_ERROR_SYSBASE_DMP + 0x0013)
#define GL_ERROR_8014_DMP   (GL_ERROR_SYSBASE_DMP + 0x0014)
#define GL_ERROR_8015_DMP   (GL_ERROR_SYSBASE_DMP + 0x0015)
#define GL_ERROR_8016_DMP   (GL_ERROR_SYSBASE_DMP + 0x0016)
#define GL_ERROR_8017_DMP   (GL_ERROR_SYSBASE_DMP + 0x0017)
#define GL_ERROR_8018_DMP   (GL_ERROR_SYSBASE_DMP + 0x0018)
#define GL_ERROR_8019_DMP   (GL_ERROR_SYSBASE_DMP + 0x0019)
#define GL_ERROR_801A_DMP   (GL_ERROR_SYSBASE_DMP + 0x001A)
#define GL_ERROR_801B_DMP   (GL_ERROR_SYSBASE_DMP + 0x001B)
#define GL_ERROR_801C_DMP   (GL_ERROR_SYSBASE_DMP + 0x001C)
#define GL_ERROR_801D_DMP   (GL_ERROR_SYSBASE_DMP + 0x001D)
#define GL_ERROR_801E_DMP   (GL_ERROR_SYSBASE_DMP + 0x001E)
#define GL_ERROR_801F_DMP   (GL_ERROR_SYSBASE_DMP + 0x001F)
#define GL_ERROR_8020_DMP   (GL_ERROR_SYSBASE_DMP + 0x0020)
#define GL_ERROR_8021_DMP   (GL_ERROR_SYSBASE_DMP + 0x0021)
#define GL_ERROR_8022_DMP   (GL_ERROR_SYSBASE_DMP + 0x0022)
#define GL_ERROR_8023_DMP   (GL_ERROR_SYSBASE_DMP + 0x0023)
#define GL_ERROR_8024_DMP   (GL_ERROR_SYSBASE_DMP + 0x0024)
#define GL_ERROR_8025_DMP   (GL_ERROR_SYSBASE_DMP + 0x0025)
#define GL_ERROR_8026_DMP   (GL_ERROR_SYSBASE_DMP + 0x0026)
#define GL_ERROR_8027_DMP   (GL_ERROR_SYSBASE_DMP + 0x0027)
#define GL_ERROR_8028_DMP   (GL_ERROR_SYSBASE_DMP + 0x0028)
#define GL_ERROR_8029_DMP   (GL_ERROR_SYSBASE_DMP + 0x0029)
#define GL_ERROR_802A_DMP   (GL_ERROR_SYSBASE_DMP + 0x002A)
#define GL_ERROR_802B_DMP   (GL_ERROR_SYSBASE_DMP + 0x002B)
#define GL_ERROR_802C_DMP   (GL_ERROR_SYSBASE_DMP + 0x002C)
#define GL_ERROR_802D_DMP   (GL_ERROR_SYSBASE_DMP + 0x002D)
#define GL_ERROR_802E_DMP   (GL_ERROR_SYSBASE_DMP + 0x002E)
#define GL_ERROR_802F_DMP   (GL_ERROR_SYSBASE_DMP + 0x002F)
#define GL_ERROR_8030_DMP   (GL_ERROR_SYSBASE_DMP + 0x0030)
#define GL_ERROR_8031_DMP   (GL_ERROR_SYSBASE_DMP + 0x0031)
#define GL_ERROR_8032_DMP   (GL_ERROR_SYSBASE_DMP + 0x0032)
#define GL_ERROR_8033_DMP   (GL_ERROR_SYSBASE_DMP + 0x0033)
#define GL_ERROR_8034_DMP   (GL_ERROR_SYSBASE_DMP + 0x0034)
#define GL_ERROR_8035_DMP   (GL_ERROR_SYSBASE_DMP + 0x0035)
#define GL_ERROR_8036_DMP   (GL_ERROR_SYSBASE_DMP + 0x0036)
#define GL_ERROR_8037_DMP   (GL_ERROR_SYSBASE_DMP + 0x0037)
#define GL_ERROR_8038_DMP   (GL_ERROR_SYSBASE_DMP + 0x0038)
#define GL_ERROR_8039_DMP   (GL_ERROR_SYSBASE_DMP + 0x0039)
#define GL_ERROR_803A_DMP   (GL_ERROR_SYSBASE_DMP + 0x003A)
#define GL_ERROR_803B_DMP   (GL_ERROR_SYSBASE_DMP + 0x003B)
#define GL_ERROR_803C_DMP   (GL_ERROR_SYSBASE_DMP + 0x003C)
#define GL_ERROR_803D_DMP   (GL_ERROR_SYSBASE_DMP + 0x003D)
#define GL_ERROR_803E_DMP   (GL_ERROR_SYSBASE_DMP + 0x003E)
#define GL_ERROR_803F_DMP   (GL_ERROR_SYSBASE_DMP + 0x003F)
#define GL_ERROR_8040_DMP   (GL_ERROR_SYSBASE_DMP + 0x0040)
#define GL_ERROR_8041_DMP   (GL_ERROR_SYSBASE_DMP + 0x0041)
#define GL_ERROR_8042_DMP   (GL_ERROR_SYSBASE_DMP + 0x0042)
#define GL_ERROR_8043_DMP   (GL_ERROR_SYSBASE_DMP + 0x0043)
#define GL_ERROR_8044_DMP   (GL_ERROR_SYSBASE_DMP + 0x0044)
#define GL_ERROR_8045_DMP   (GL_ERROR_SYSBASE_DMP + 0x0045)
#define GL_ERROR_8046_DMP   (GL_ERROR_SYSBASE_DMP + 0x0046)
#define GL_ERROR_8047_DMP   (GL_ERROR_SYSBASE_DMP + 0x0047)
#define GL_ERROR_8048_DMP   (GL_ERROR_SYSBASE_DMP + 0x0048)
#define GL_ERROR_8049_DMP   (GL_ERROR_SYSBASE_DMP + 0x0049)
#define GL_ERROR_804A_DMP   (GL_ERROR_SYSBASE_DMP + 0x004A)
#define GL_ERROR_804B_DMP   (GL_ERROR_SYSBASE_DMP + 0x004B)
#define GL_ERROR_804C_DMP   (GL_ERROR_SYSBASE_DMP + 0x004C)
#define GL_ERROR_804D_DMP   (GL_ERROR_SYSBASE_DMP + 0x004D)
#define GL_ERROR_804E_DMP   (GL_ERROR_SYSBASE_DMP + 0x004E)
#define GL_ERROR_804F_DMP   (GL_ERROR_SYSBASE_DMP + 0x004F)
#define GL_ERROR_8050_DMP   (GL_ERROR_SYSBASE_DMP + 0x0050)
#define GL_ERROR_8051_DMP   (GL_ERROR_SYSBASE_DMP + 0x0051)
#define GL_ERROR_8052_DMP   (GL_ERROR_SYSBASE_DMP + 0x0052)
#define GL_ERROR_8053_DMP   (GL_ERROR_SYSBASE_DMP + 0x0053)
#define GL_ERROR_8054_DMP   (GL_ERROR_SYSBASE_DMP + 0x0054)
#define GL_ERROR_8055_DMP   (GL_ERROR_SYSBASE_DMP + 0x0055)
#define GL_ERROR_8056_DMP   (GL_ERROR_SYSBASE_DMP + 0x0056)
#define GL_ERROR_8057_DMP   (GL_ERROR_SYSBASE_DMP + 0x0057)
#define GL_ERROR_8058_DMP   (GL_ERROR_SYSBASE_DMP + 0x0058)
#define GL_ERROR_8059_DMP   (GL_ERROR_SYSBASE_DMP + 0x0059)
#define GL_ERROR_805A_DMP   (GL_ERROR_SYSBASE_DMP + 0x005A)
#define GL_ERROR_805B_DMP   (GL_ERROR_SYSBASE_DMP + 0x005B)
#define GL_ERROR_805C_DMP   (GL_ERROR_SYSBASE_DMP + 0x005C)
#define GL_ERROR_805D_DMP   (GL_ERROR_SYSBASE_DMP + 0x005D)
#define GL_ERROR_805E_DMP   (GL_ERROR_SYSBASE_DMP + 0x005E)
#define GL_ERROR_805F_DMP   (GL_ERROR_SYSBASE_DMP + 0x005F)
#define GL_ERROR_8060_DMP   (GL_ERROR_SYSBASE_DMP + 0x0060)
#define GL_ERROR_8061_DMP   (GL_ERROR_SYSBASE_DMP + 0x0061)
#define GL_ERROR_8062_DMP   (GL_ERROR_SYSBASE_DMP + 0x0062)
#define GL_ERROR_8063_DMP   (GL_ERROR_SYSBASE_DMP + 0x0063)
#define GL_ERROR_8064_DMP   (GL_ERROR_SYSBASE_DMP + 0x0064)
#define GL_ERROR_8065_DMP   (GL_ERROR_SYSBASE_DMP + 0x0065)
#define GL_ERROR_8066_DMP   (GL_ERROR_SYSBASE_DMP + 0x0066)
#define GL_ERROR_8067_DMP   (GL_ERROR_SYSBASE_DMP + 0x0067)
#define GL_ERROR_8068_DMP   (GL_ERROR_SYSBASE_DMP + 0x0068)
#define GL_ERROR_8069_DMP   (GL_ERROR_SYSBASE_DMP + 0x0069)
#define GL_ERROR_806A_DMP   (GL_ERROR_SYSBASE_DMP + 0x006A)
#define GL_ERROR_806B_DMP   (GL_ERROR_SYSBASE_DMP + 0x006B)
#define GL_ERROR_806C_DMP   (GL_ERROR_SYSBASE_DMP + 0x006C)
#define GL_ERROR_806D_DMP   (GL_ERROR_SYSBASE_DMP + 0x006D)
#define GL_ERROR_806E_DMP   (GL_ERROR_SYSBASE_DMP + 0x006E)
#define GL_ERROR_806F_DMP   (GL_ERROR_SYSBASE_DMP + 0x006F)
#define GL_ERROR_8070_DMP   (GL_ERROR_SYSBASE_DMP + 0x0070)
#define GL_ERROR_8071_DMP   (GL_ERROR_SYSBASE_DMP + 0x0071)
#define GL_ERROR_8072_DMP   (GL_ERROR_SYSBASE_DMP + 0x0072)
#define GL_ERROR_8073_DMP   (GL_ERROR_SYSBASE_DMP + 0x0073)
#define GL_ERROR_8074_DMP   (GL_ERROR_SYSBASE_DMP + 0x0074)
#define GL_ERROR_8075_DMP   (GL_ERROR_SYSBASE_DMP + 0x0075)
#define GL_ERROR_8076_DMP   (GL_ERROR_SYSBASE_DMP + 0x0076)
#define GL_ERROR_8077_DMP   (GL_ERROR_SYSBASE_DMP + 0x0077)
#define GL_ERROR_8078_DMP   (GL_ERROR_SYSBASE_DMP + 0x0078)
#define GL_ERROR_8079_DMP   (GL_ERROR_SYSBASE_DMP + 0x0079)
#define GL_ERROR_807A_DMP   (GL_ERROR_SYSBASE_DMP + 0x007A)
#define GL_ERROR_807B_DMP   (GL_ERROR_SYSBASE_DMP + 0x007B)
#define GL_ERROR_807C_DMP   (GL_ERROR_SYSBASE_DMP + 0x007C)
#define GL_ERROR_807D_DMP   (GL_ERROR_SYSBASE_DMP + 0x007D)
#define GL_ERROR_807E_DMP   (GL_ERROR_SYSBASE_DMP + 0x007E)
#define GL_ERROR_807F_DMP   (GL_ERROR_SYSBASE_DMP + 0x007F)
#define GL_ERROR_8080_DMP   (GL_ERROR_SYSBASE_DMP + 0x0080)
#define GL_ERROR_8081_DMP   (GL_ERROR_SYSBASE_DMP + 0x0081)
#define GL_ERROR_8082_DMP   (GL_ERROR_SYSBASE_DMP + 0x0082)
#define GL_ERROR_8083_DMP   (GL_ERROR_SYSBASE_DMP + 0x0083)
#define GL_ERROR_8084_DMP   (GL_ERROR_SYSBASE_DMP + 0x0084)
#define GL_ERROR_8085_DMP   (GL_ERROR_SYSBASE_DMP + 0x0085)
#define GL_ERROR_8086_DMP   (GL_ERROR_SYSBASE_DMP + 0x0086)
#define GL_ERROR_8087_DMP   (GL_ERROR_SYSBASE_DMP + 0x0087)
#define GL_ERROR_8088_DMP   (GL_ERROR_SYSBASE_DMP + 0x0088)
#define GL_ERROR_8089_DMP   (GL_ERROR_SYSBASE_DMP + 0x0089)
#define GL_ERROR_808A_DMP   (GL_ERROR_SYSBASE_DMP + 0x008A)
#define GL_ERROR_808B_DMP   (GL_ERROR_SYSBASE_DMP + 0x008B)
#define GL_ERROR_808C_DMP   (GL_ERROR_SYSBASE_DMP + 0x008C)
#define GL_ERROR_808D_DMP   (GL_ERROR_SYSBASE_DMP + 0x008D)
#define GL_ERROR_808E_DMP   (GL_ERROR_SYSBASE_DMP + 0x008E)
#define GL_ERROR_808F_DMP   (GL_ERROR_SYSBASE_DMP + 0x008F)
#define GL_ERROR_8090_DMP   (GL_ERROR_SYSBASE_DMP + 0x0090)
#define GL_ERROR_8091_DMP   (GL_ERROR_SYSBASE_DMP + 0x0091)
#define GL_ERROR_8092_DMP   (GL_ERROR_SYSBASE_DMP + 0x0092)
#define GL_ERROR_8093_DMP   (GL_ERROR_SYSBASE_DMP + 0x0093)
#define GL_ERROR_8094_DMP   (GL_ERROR_SYSBASE_DMP + 0x0094)
#define GL_ERROR_8095_DMP   (GL_ERROR_SYSBASE_DMP + 0x0095)
#define GL_ERROR_8096_DMP   (GL_ERROR_SYSBASE_DMP + 0x0096)
#define GL_ERROR_8097_DMP   (GL_ERROR_SYSBASE_DMP + 0x0097)
#define GL_ERROR_8098_DMP   (GL_ERROR_SYSBASE_DMP + 0x0098)
#define GL_ERROR_8099_DMP   (GL_ERROR_SYSBASE_DMP + 0x0099)
#define GL_ERROR_809A_DMP   (GL_ERROR_SYSBASE_DMP + 0x009A)
#define GL_ERROR_809B_DMP   (GL_ERROR_SYSBASE_DMP + 0x009B)
#define GL_ERROR_809C_DMP   (GL_ERROR_SYSBASE_DMP + 0x009C)
#define GL_ERROR_809D_DMP   (GL_ERROR_SYSBASE_DMP + 0x009D)
#define GL_ERROR_809E_DMP   (GL_ERROR_SYSBASE_DMP + 0x009E)
#define GL_ERROR_809F_DMP   (GL_ERROR_SYSBASE_DMP + 0x009F)
#define GL_ERROR_80A0_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A0)
#define GL_ERROR_80A1_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A1)
#define GL_ERROR_80A2_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A2)
#define GL_ERROR_80A3_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A3)
#define GL_ERROR_80A4_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A4)
#define GL_ERROR_80A5_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A5)
#define GL_ERROR_80A6_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A6)
#define GL_ERROR_80A7_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A7)
#define GL_ERROR_80A8_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A8)
#define GL_ERROR_80A9_DMP   (GL_ERROR_SYSBASE_DMP + 0x00A9)
#define GL_ERROR_80AA_DMP   (GL_ERROR_SYSBASE_DMP + 0x00AA)
#define GL_ERROR_80AB_DMP   (GL_ERROR_SYSBASE_DMP + 0x00AB)
#define GL_ERROR_80AC_DMP   (GL_ERROR_SYSBASE_DMP + 0x00AC)
#define GL_ERROR_80AD_DMP   (GL_ERROR_SYSBASE_DMP + 0x00AD)
#define GL_ERROR_80AE_DMP   (GL_ERROR_SYSBASE_DMP + 0x00AE)
#define GL_ERROR_80AF_DMP   (GL_ERROR_SYSBASE_DMP + 0x00AF)
#define GL_ERROR_80B0_DMP   (GL_ERROR_SYSBASE_DMP + 0x00B0)
#define GL_ERROR_80B1_DMP   (GL_ERROR_SYSBASE_DMP + 0x00B1)
#define GL_ERROR_80B2_DMP   (GL_ERROR_SYSBASE_DMP + 0x00B2)
#define GL_ERROR_80B3_DMP   (GL_ERROR_SYSBASE_DMP + 0x00B3)
#define GL_ERROR_80B4_DMP   (GL_ERROR_SYSBASE_DMP + 0x00B4)

#define GL_ERROR_SYS2BASE_DMP   0x9000
#define GL_ERROR_9000_DMP   (GL_ERROR_SYS2BASE_DMP + 0x0000)
#define GL_ERROR_9001_DMP   (GL_ERROR_SYS2BASE_DMP + 0x0001)
#define GL_ERROR_9002_DMP   (GL_ERROR_SYS2BASE_DMP + 0x0002)
#define GL_ERROR_9003_DMP   (GL_ERROR_SYS2BASE_DMP + 0x0003)

#define GL_ERROR_COMMANDBUFFER_FULL_DMP    0x9800
#define GL_ERROR_COMMANDREQUEST_FULL_DMP   0x9801

/* extension to get additional information */
#define GL_TEXTURE_DATA_ADDR_DMP          0x6790
#define GL_BUFFER_DATA_ADDR_DMP           0x6791
#define GL_RENDERBUFFER_DATA_ADDR_DMP     0x6792

/* internalformat to copy depth stencil buffer */
#define GL_DEPTH_STENCIL_COPY_DMP         0x67A0

#ifdef __cplusplus
}
#endif

#endif /* __gl2ext_h_ */
