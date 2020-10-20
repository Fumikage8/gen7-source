/*------------------------------------------------------------------------------*/
/*  gx_CTRRaw.h                                                                 */
/*      nngxRaw interface definitions                                           */
/*                                                                              */
/*  Copyright (c) 2012, Digital Media Professional. All rights reserved.        */
/*                                                                              */
/*------------------------------------------------------------------------------*/

#ifndef NN_GX_CTR_GX_CTR_RAW_H_
#define NN_GX_CTR_GX_CTR_RAW_H_

#include <GLES2/gl2.h>
#include <nn/gx/CTR/gx_CTR.h>

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------*
 * NNGX Raw API
 *------------------------------------------------------------------------*/

/* Command list object structure */
typedef struct tag_nngxCommandList {
    unsigned int    Reserved[17];
} nngxCommandList;

NNGX_APICALL void NNGX_APIENTRY nngxCmdlistStorageRaw(nngxCommandList* cmdlist, GLsizei bufsize, GLvoid* commandbuffer, GLsizei requestcount, GLvoid* commandrequest);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetCommandRequestSizeRaw(GLsizei requestcount);
NNGX_APICALL void NNGX_APIENTRY nngxSplitDrawCmdlistRaw(nngxCommandList* cmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxFlush3DCommandNoCacheFlushRaw(nngxCommandList* cmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxFlush3DCommandPartiallyRaw(nngxCommandList* cmdlist, GLsizei buffersize);
NNGX_APICALL void NNGX_APIENTRY nngxAdd3DCommandNoCacheFlushRaw(nngxCommandList* cmdlist, const GLvoid* bufferaddr, GLsizei buffersize);
NNGX_APICALL void NNGX_APIENTRY nngxMoveCommandbufferPointerRaw(nngxCommandList* cmdlist, GLint offset);
NNGX_APICALL void NNGX_APIENTRY nngxAddB2LTransferCommandRaw(nngxCommandList* cmdlist, const GLvoid* srcaddr, GLsizei srcwidth, GLsizei srcheight, GLenum srcformat, GLvoid* dstaddr, GLsizei dstwidth, GLsizei dstheight, GLenum dstformat, GLenum aamode, GLboolean yflip, GLsizei blocksize);
NNGX_APICALL void NNGX_APIENTRY nngxAddBlockImageCopyCommandRaw(nngxCommandList* cmdlist, const GLvoid* srcaddr, GLsizei srcunit, GLsizei srcinterval, GLvoid* dstaddr, GLsizei dstunit, GLsizei dstinterval, GLsizei totalsize);
NNGX_APICALL void NNGX_APIENTRY nngxAddL2BTransferCommandRaw(nngxCommandList* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei width, GLsizei height, GLenum format, GLsizei blocksize);
NNGX_APICALL void NNGX_APIENTRY nngxAddMemoryFillCommandRaw(nngxCommandList* cmdlist, GLvoid* startaddr0, GLsizei size0, GLuint data0, GLsizei width0, GLvoid* startaddr1, GLsizei size1, GLuint data1, GLsizei width1);
NNGX_APICALL void NNGX_APIENTRY nngxAddVramDmaCommandNoCacheFlushRaw(nngxCommandList* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei size);
NNGX_APICALL void NNGX_APIENTRY nngxFilterBlockImageRaw(nngxCommandList* cmdlist, const GLvoid* srcaddr, GLvoid* dstaddr, GLsizei width, GLsizei height, GLenum format);
NNGX_APICALL void NNGX_APIENTRY nngxEnableCmdlistCallbackRaw(nngxCommandList* cmdlist, GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxDisableCmdlistCallbackRaw(nngxCommandList* cmdlist, GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxSetCmdlistCallbackRaw(nngxCommandList* cmdlist, void (*func)(GLint));
NNGX_APICALL void NNGX_APIENTRY nngxRunCmdlistRaw(nngxCommandList* cmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxReserveStopCmdlistRaw(nngxCommandList* cmdlist, GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxClearCmdlistRaw(nngxCommandList* cmdlist);
NNGX_APICALL void NNGX_APIENTRY nngxClearFillCmdlistRaw(nngxCommandList* cmdlist, GLuint data);
NNGX_APICALL void NNGX_APIENTRY nngxSetGasAutoAccumulationUpdateRaw(nngxCommandList* cmdlist, GLint id);
NNGX_APICALL void NNGX_APIENTRY nngxSetGasUpdateRaw(nngxCommandList* cmdlist, GLboolean enable);
NNGX_APICALL GLboolean NNGX_APIENTRY nngxGetIsRunningRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetUsedBufferSizeRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetUsedRequestCountRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetMaxBufferSizeRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetMaxRequestCountRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLvoid* NNGX_APIENTRY nngxGetTopBufferAddrRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetRunBufferSizeRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetRunRequestCountRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLvoid* NNGX_APIENTRY nngxGetTopRequestAddrRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLenum NNGX_APIENTRY nngxGetNextRequestTypeRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLvoid* NNGX_APIENTRY nngxGetNextBufferAddrRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLsizei NNGX_APIENTRY nngxGetNextBufferSizeRaw(nngxCommandList* cmdlist);
NNGX_APICALL GLbitfield NNGX_APIENTRY nngxGetHWStateRaw(void);
NNGX_APICALL GLvoid* NNGX_APIENTRY nngxGetCurrentBufferAddrRaw(nngxCommandList* cmdlist);

#ifdef __cplusplus
}
#endif

#endif /* NN_GX_CTR_GX_CTR_RAW_H_ */
