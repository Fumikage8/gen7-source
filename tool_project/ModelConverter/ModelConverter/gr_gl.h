/*---------------------------------------------------------------------------*
  Project:  Horizon
  File:     gr_Combiner.h

  Copyright (C)2009-2013 Nintendo Co., Ltd.  All rights reserved.

  These coded instructions, statements, and computer programs contain
  proprietary information of Nintendo of America Inc. and/or Nintendo
  Company Ltd., and are protected by Federal copyright law.  They may
  not be disclosed to third parties or copied or duplicated in any form,
  in whole or in part, without the prior written consent of Nintendo.

  $Rev: 50823 $
 *---------------------------------------------------------------------------*/

#ifndef NN_GR_CTR_GL_H_
#define NN_GR_CTR_GL_H_

#define			bit32			u32
#define			NN_GR_ASSERT		assert
// TORIAEZU: コンパイラ依存ではないのでここにあるのはおかしい
#define NN_PADDING1             int :  8
#define NN_PADDING2             int : 16
#define NN_PADDING3             int : 24
#define NN_PADDING4             int : 32

#define __gl2_h_
//#include <GLES2/gl2.h>のインクルードを抑止
#include <gl/glew.h>
#include <gl/gl.h>

#include <gfx/include/gfl2_types.h>

#endif // NN_GR_CTR_GR_COMBINER_H_
