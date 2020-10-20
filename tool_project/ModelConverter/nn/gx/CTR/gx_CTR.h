/*------------------------------------------------------------------------------*/
/*  gx_CTR.h                                                                    */
/*      nngx interface definitions                                              */
/*                                                                              */
/*  Copyright (c) 2009-2012, Digital Media Professional. All rights reserved.   */
/*                                                                              */
/*------------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_CTR_H_
#define NN_GX_CTR_GX_CTR_H_

#include <GLES2/gl2.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32) || defined(__VC32__)             /* Win32 */
#   if defined (NNGX_EXPORTS)
#       define NNGX_APICALL __declspec(dllexport)
#   else
#       define NNGX_APICALL __declspec(dllimport)
#   endif
#elif defined (__ARMCC_VERSION)                      /* ADS */
#   define NNGX_APICALL
#elif defined (__SYMBIAN32__) && defined (__GCC32__) /* Symbian GCC */
#   define NNGX_APICALL __declspec(dllexport)
#elif defined (__GNUC__)                             /* GCC dependencies (kludge) */
#   define NNGX_APICALL
#endif

#if !defined (NNGX_APICALL)
#   error Unsupported platform!
#endif

#define NNGX_APIENTRY

/*------------------------------------------------------------------------*
 * NNGX Initialize API
 *------------------------------------------------------------------------*/
/* parameter for allocator and deallocator */
/* allocation area */
#define NN_GX_MEM_FCRAM             0x00010000
#define NN_GX_MEM_VRAMA             0x00020000
#define NN_GX_MEM_VRAMB             0x00030000

/* allocation usage */
#define NN_GX_MEM_SYSTEM            0x0100
#define NN_GX_MEM_TEXTURE           0x0101
#define NN_GX_MEM_VERTEXBUFFER      0x0102
#define NN_GX_MEM_RENDERBUFFER      0x0103
#define NN_GX_MEM_DISPLAYBUFFER     0x0104
#define NN_GX_MEM_COMMANDBUFFER     0x0105

NNGX_APICALL GLboolean NNGX_APIENTRY nngxInitialize(GLvoid* (*allocator)(GLenum, GLenum, GLuint, GLsizei), void (*deallocator)(GLenum, GLenum, GLuint, GLvoid*));
NNGX_APICALL void NNGX_APIENTRY nngxFinalize(void);
NNGX_APICALL void NNGX_APIENTRY nngxGetAllocator(GLvoid* (**allocator)(GLenum, GLenum, GLuint, GLsizei), void (**deallocator)(GLenum, GLenum, GLuint, GLvoid*));
NNGX_APICALL GLboolean NNGX_APIENTRY nngxGetIsInitialized(void);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetInitializationCommand(GLsizei datasize, GLvoid* data);

/*------------------------------------------------------------------------*
 * NNGX Execution control API
 *------------------------------------------------------------------------*/
/* command list object parameter */
#define NN_GX_CMDLIST_RUN_MODE          0x0200
#define NN_GX_CMDLIST_IS_RUNNING        0x0201
#define NN_GX_CMDLIST_USED_BUFSIZE      0x0202
#define NN_GX_CMDLIST_USED_REQCOUNT     0x0203
#define NN_GX_CMDLIST_MAX_BUFSIZE       0x0204
#define NN_GX_CMDLIST_MAX_REQCOUNT      0x0205
#define NN_GX_CMDLIST_TOP_BUFADDR       0x0206
#define NN_GX_CMDLIST_BINDING           0x0207
#define NN_GX_CMDLIST_CURRENT_BUFADDR   0x0208
#define NN_GX_CMDLIST_RUN_BUFSIZE       0x0209
#define NN_GX_CMDLIST_RUN_REQCOUNT      0x020a
#define NN_GX_CMDLIST_TOP_REQADDR       0x020b
#define NN_GX_CMDLIST_NEXT_REQTYPE      0x020c
#define NN_GX_CMDLIST_NEXT_REQINFO      0x020d
#define NN_GX_CMDLIST_HW_STATE          0x020e
#define NN_GX_CMDLIST_GAS_UPDATE        0x020f

/* parameter for run mode */
#define NN_GX_CMDLIST_SERIAL_RUN        0x0300
#define NN_GX_CMDLIST_PARALLEL_RUN      0x0301
#define NN_GX_CMDLIST_SYNC_RUN          0x0302

/* request type */
#define NN_GX_CMDLIST_REQTYPE_DMA       0x0310
#define NN_GX_CMDLIST_REQTYPE_RUN3D     0x0311
#define NN_GX_CMDLIST_REQTYPE_FILLMEM   0x0312
#define NN_GX_CMDLIST_REQTYPE_POSTTRANS 0x0313
#define NN_GX_CMDLIST_REQTYPE_COPYTEX   0x0314

/* display id */
#define NN_GX_DISPLAY0                  0x0400
#define NN_GX_DISPLAY1                  0x0401
#define NN_GX_DISPLAY_BOTH              0x0402
#define NN_GX_DISPLAY0_EXT              0x0410

NNGX_APICALL void NNGX_APIENTRY nngxGenCmdlists(GLsizei n, GLuint* cmdlists);
NNGX_APICALL void NNGX_APIENTRY nngxDeleteCmdlists(GLsizei n, const GLuint* cmdlists);
NNGX_APICALL void NNGX_APIENTRY nngxBindCmdlist(GLuint cmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxCmdlistStorage(GLsizei bufsize, GLsizei requestcount);
NNGX_APICALL void NNGX_APIENTRY nngxRunCmdlist(void);
NNGX_APICALL void NNGX_APIENTRY nngxRunCmdlistByID(GLuint cmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxStopCmdlist(void);
NNGX_APICALL void NNGX_APIENTRY nngxReserveStopCmdlist(GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxSplitDrawCmdlist(void);
NNGX_APICALL void NNGX_APIENTRY nngxClearCmdlist(void);
NNGX_APICALL void NNGX_APIENTRY nngxSetCmdlistCallback(void (*func)(GLint));
NNGX_APICALL void NNGX_APIENTRY nngxEnableCmdlistCallback(GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxDisableCmdlistCallback(GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxSetCmdlistParameteri(GLenum pname, GLint param);
NNGX_APICALL void NNGX_APIENTRY nngxGetCmdlistParameteri(GLenum pname, GLint* param);
NNGX_APICALL GLint NNGX_APIENTRY nngxCheckVSync(GLenum display);
NNGX_APICALL void NNGX_APIENTRY nngxWaitVSync(GLenum display);
NNGX_APICALL void NNGX_APIENTRY nngxSetVSyncCallback(GLenum display, void (*func)(GLenum));
NNGX_APICALL void NNGX_APIENTRY nngxWaitCmdlistDone(void);
NNGX_APICALL void NNGX_APIENTRY nngxAddVramDmaCommand(const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei size);
NNGX_APICALL void NNGX_APIENTRY nngxAddVramDmaCommandNoCacheFlush(const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei size);
NNGX_APICALL void NNGX_APIENTRY nngxClearFillCmdlist(GLuint data);
NNGX_APICALL void NNGX_APIENTRY nngxSetTimeout(GLint64EXT time, void (*callback)(void));
NNGX_APICALL void NNGX_APIENTRY nngxSetGasAutoAccumulationUpdate(GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxAddL2BTransferCommand(const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei width, GLsizei height, GLenum format, GLsizei blocksize);
NNGX_APICALL void NNGX_APIENTRY nngxAddB2LTransferCommand(const GLvoid* srcaddr, GLsizei srcwidth, GLsizei srcheight, GLenum srcformat, GLvoid* dstaddr, GLsizei dstwidth, GLsizei dstheight, GLenum dstformat, GLenum aamode, GLboolean yflip, GLsizei blocksize);
NNGX_APICALL void NNGX_APIENTRY nngxAddBlockImageCopyCommand(const GLvoid* srcaddr, GLsizei srcunit, GLsizei srcinterval, GLvoid* dstaddr, GLsizei dstunit, GLsizei dstinterval, GLsizei totalsize);
NNGX_APICALL void NNGX_APIENTRY nngxAddMemoryFillCommand(GLvoid* startaddr0, GLsizei size0, GLuint data0, GLsizei width0, GLvoid* startaddr1, GLsizei size1, GLuint data1, GLsizei width1);
NNGX_APICALL void NNGX_APIENTRY nngxFlush3DCommand(void);
NNGX_APICALL void NNGX_APIENTRY nngxFlush3DCommandNoCacheFlush(void);
NNGX_APICALL void NNGX_APIENTRY nngxFlush3DCommandPartially(GLsizei buffersize);
NNGX_APICALL void NNGX_APIENTRY nngxAddJumpCommand(const GLvoid* bufferaddr, GLsizei buffersize);
NNGX_APICALL void NNGX_APIENTRY nngxAddSubroutineCommand(const GLvoid* bufferaddr, GLsizei buffersize);
NNGX_APICALL GLboolean NNGX_APIENTRY nngxGetIsRunning(void);

/*------------------------------------------------------------------------*
 * NNGX Display control API
 *------------------------------------------------------------------------*/
/* parameter for antialiase mode */
#define NN_GX_ANTIALIASE_NOT_USED       0x0500
#define NN_GX_ANTIALIASE_2x1            0x0501
#define NN_GX_ANTIALIASE_2x2            0x0502

/* display buffer object parameter */
#define NN_GX_DISPLAYBUFFER_ADDRESS     0x0600
#define NN_GX_DISPLAYBUFFER_FORMAT      0x0601
#define NN_GX_DISPLAYBUFFER_WIDTH       0x0602
#define NN_GX_DISPLAYBUFFER_HEIGHT      0x0603

/* display mode */
#define NN_GX_DISPLAYMODE_NORMAL        0x0700
#define NN_GX_DISPLAYMODE_RESERVED      0x0701
#define NN_GX_DISPLAYMODE_STEREO        0x0702

/* display id aliase */
#define NN_GX_DISPLAY0_LEFT             NN_GX_DISPLAY0
#define NN_GX_DISPLAY0_RIGHT            NN_GX_DISPLAY0_EXT

NNGX_APICALL void NNGX_APIENTRY nngxGenDisplaybuffers(GLsizei n, GLuint* buffers);
NNGX_APICALL void NNGX_APIENTRY nngxDeleteDisplaybuffers(GLsizei n, GLuint* buffers);
NNGX_APICALL void NNGX_APIENTRY nngxActiveDisplay(GLenum display);
NNGX_APICALL void NNGX_APIENTRY nngxBindDisplaybuffer(GLuint buffer);
NNGX_APICALL void NNGX_APIENTRY nngxDisplaybufferStorage(GLenum format, GLsizei width, GLsizei height, GLenum area);
NNGX_APICALL void NNGX_APIENTRY nngxDisplayEnv(GLint displayx, GLint displayy);
NNGX_APICALL void NNGX_APIENTRY nngxTransferRenderImage(GLuint buffer, GLenum mode, GLboolean yflip, GLint colorx, GLint colory);
NNGX_APICALL void NNGX_APIENTRY nngxSwapBuffers(GLenum display);
NNGX_APICALL void NNGX_APIENTRY nngxGetDisplaybufferParameteri(GLenum pname, GLint* param);
NNGX_APICALL void NNGX_APIENTRY nngxSetDisplayMode(GLenum mode);
NNGX_APICALL void NNGX_APIENTRY nngxTransferLinearImage(const GLvoid* srcaddr, GLuint dstid, GLenum target);
NNGX_APICALL void NNGX_APIENTRY nngxFilterBlockImage(const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei width, GLsizei height, GLenum format);
NNGX_APICALL void NNGX_APIENTRY nngxSwapBuffersByAddress(GLenum display, const GLvoid* addr, const GLvoid* addrB, GLsizei width, GLenum format);

/*------------------------------------------------------------------------*
 * NNGX Command cache API
 *------------------------------------------------------------------------*/
/* parameter for command state */
#define NN_GX_STATE_SHADERBINARY   0x00000001
#define NN_GX_STATE_SHADERPROGRAM  0x00000002
#define NN_GX_STATE_SHADERMODE     0x00000004
#define NN_GX_STATE_SHADERFLOAT    0x00000008
#define NN_GX_STATE_VSUNIFORM      0x00000010
#define NN_GX_STATE_FSUNIFORM      0x00000020
#define NN_GX_STATE_LUT            0x00000040
#define NN_GX_STATE_TEXTURE        0x00000080
#define NN_GX_STATE_FRAMEBUFFER    0x00000100
#define NN_GX_STATE_VERTEX         0x00000200
#define NN_GX_STATE_TRIOFFSET      0x00000400
#define NN_GX_STATE_FBACCESS       0x00000800
#define NN_GX_STATE_SCISSOR        0x00001000
#define NN_GX_STATE_OTHERS         0x00002000
#define NN_GX_STATE_ALL            0x00003fff

/* parameter for command generation mode */
#define NN_GX_CMDGEN_MODE_CONDITIONAL    0x0800
#define NN_GX_CMDGEN_MODE_UNCONDITIONAL  0x0801

NNGX_APICALL void NNGX_APIENTRY nngxStartCmdlistSave(void);
NNGX_APICALL void NNGX_APIENTRY nngxStopCmdlistSave(GLuint* bufferoffset, GLsizei* buffersize, GLuint* requestid, GLsizei* requestsize);
NNGX_APICALL void NNGX_APIENTRY nngxUseSavedCmdlist(GLuint cmdlist, GLuint bufferoffset, GLsizei buffersize, GLuint requestid, GLsizei requestsize, GLbitfield statemask, GLboolean copycmd);
NNGX_APICALL void NNGX_APIENTRY nngxUseSavedCmdlistNoCacheFlush(GLuint cmdlist, GLuint bufferoffset, GLsizei buffersize, GLuint requestid, GLsizei requestsize, GLbitfield statemask);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxExportCmdlist(GLuint cmdlist, GLuint bufferoffset, GLsizei buffersize, GLuint requestid, GLsizei requestsize, GLsizei datasize, GLvoid* data);
NNGX_APICALL void NNGX_APIENTRY nngxImportCmdlist(GLuint cmdlist, GLvoid* data, GLsizei datasize);
NNGX_APICALL void NNGX_APIENTRY nngxCopyCmdlist(GLuint scmdlist, GLuint dcmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxValidateState(GLbitfield statemask, GLboolean drawelements);
NNGX_APICALL void NNGX_APIENTRY nngxUpdateState(GLbitfield statemask);
NNGX_APICALL void NNGX_APIENTRY nngxGetExportedCmdlistInfo(GLvoid* data, GLsizei* buffersize, GLsizei* requestsize, GLuint* bufferoffset);
NNGX_APICALL void NNGX_APIENTRY nngxSetCommandGenerationMode(GLenum mode);
NNGX_APICALL void NNGX_APIENTRY nngxGetCommandGenerationMode(GLenum* mode);
NNGX_APICALL void NNGX_APIENTRY nngxAdd3DCommand(const GLvoid* bufferaddr, GLsizei buffersize, GLboolean copycmd);
NNGX_APICALL void NNGX_APIENTRY nngxAdd3DCommandNoCacheFlush(const GLvoid* bufferaddr, GLsizei buffersize);
NNGX_APICALL void NNGX_APIENTRY nngxAddCmdlist(GLuint cmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxGetUpdatedState(GLbitfield* statemask);
NNGX_APICALL void NNGX_APIENTRY nngxInvalidateState(GLbitfield statemask);
NNGX_APICALL void NNGX_APIENTRY nngxMoveCommandbufferPointer(GLint offset);

/*------------------------------------------------------------------------*
 * NNGX Profiling API
 *------------------------------------------------------------------------*/
 /* profling items */
#define NN_GX_PROFILING_BUSY                0x0900
#define NN_GX_PROFILING_INTERRUPT           0x0901
#define NN_GX_PROFILING_VERTEX_SHADER0      0x0902
#define NN_GX_PROFILING_VERTEX_SHADER1      0x0903
#define NN_GX_PROFILING_VERTEX_SHADER2      0x0904
#define NN_GX_PROFILING_VERTEX_SHADER3      0x0905
#define NN_GX_PROFILING_VERTEX_CACHE        0x0906
#define NN_GX_PROFILING_POLYGON             0x0907
#define NN_GX_PROFILING_FRAGMENT            0x0908
#define NN_GX_PROFILING_MEMORY_ACCESS       0x0909

/* profiling parameter */
#define NN_GX_PROFILING_BUSY_SAMPLING_TIME  0x0910
#define NN_GX_PROFILING_BUSY_SAMPLING_TIME_MICRO_SECOND  0x0915
#define NN_GX_PROFILING_BUSY_SAMPLING_TIME_NANO_SECOND   0x0916
#define NN_GX_PROFILING_BUSY_SAMPLING_COUNT 0x0911
#define NN_GX_PROFILING_INTERRUPT_ENABLE    0x0912
#define NN_GX_PROFILING_INTERRUPT_TRIGGER   0x0913
#define NN_GX_PROFILING_INTERRUPT_IS_LOGGING   0x0914

/* size of buffer required to get profiling results */
#define NN_GX_PROFILING_RESULT_BUFSIZE_BUSY             4
#define NN_GX_PROFILING_RESULT_BUFSIZE_INTERRUPT        48
#define NN_GX_PROFILING_RESULT_BUFSIZE_VERTEX_SHADER0   5
#define NN_GX_PROFILING_RESULT_BUFSIZE_VERTEX_SHADER1   5
#define NN_GX_PROFILING_RESULT_BUFSIZE_VERTEX_SHADER2   5
#define NN_GX_PROFILING_RESULT_BUFSIZE_VERTEX_SHADER3   5
#define NN_GX_PROFILING_RESULT_BUFSIZE_VERTEX_CACHE     1
#define NN_GX_PROFILING_RESULT_BUFSIZE_POLYGON          3
#define NN_GX_PROFILING_RESULT_BUFSIZE_FRAGMENT         1
#define NN_GX_PROFILING_RESULT_BUFSIZE_MEMORY_ACCESS    18

NNGX_APICALL void NNGX_APIENTRY nngxStartProfiling(GLenum item);
NNGX_APICALL void NNGX_APIENTRY nngxStopProfiling(GLenum item);
NNGX_APICALL void NNGX_APIENTRY nngxGetProfilingResult(GLenum item, GLuint* result);
NNGX_APICALL void NNGX_APIENTRY nngxSetProfilingParameter(GLenum pname, GLuint param);
NNGX_APICALL void NNGX_APIENTRY nngxGetProfilingParameter(GLenum pname, GLuint* param);

#ifdef __cplusplus
}
#endif

#endif /* NN_GX_CTR_GX_CTR_H_ */
