#if defined(GF_PLATFORM_CTR)
//==============================================================================
/**
 * @file   gfl2_CaptureBuffer.cpp
 * @date   2013/03/12 Tue.
 * @date   2015/07/30 Thu. 11:43:41 過去プロジェクトより移植
 * @author muto_yu
 * @brief  DeviceCamera, Y2Rで使用するバッファの実装
 * @note   CTR専用、それ以外ではシンボルが未定義となる
 *         ※機種依存はCriticalSectionだけなので、これを抽象化すればPlatform制限の解除が可能
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  "DeviceCamera/include/gfl2_CaptureBuffer.h"



namespace  gfl2  {
namespace  devicecamera  {
// =============================================================================
/**/
//==============================================================================


//------------------------------------------------------------------
/**
  *  @brief    
  */
//------------------------------------------------------------------
CaptureBuffer::CaptureBuffer(void) :
    mCS_Swap()
  , mWriteBufferIndex(0)
  , mReadBufferIndex(0)
  , mBufferBytes(0)
{
  this->Invalidate();
  for(u32 num = 0; num < GFL_NELEMS(mpBufferArray); ++num)  {
    mpBufferArray[num]  = NULL;
  }
  mCS_Swap.Initialize();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
CaptureBuffer::~CaptureBuffer() {
  mCS_Swap.Finalize();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  CaptureBuffer::Setup(gfl2::heap::HeapBase* pDevHeap, const u32 size, const u32 align) {
  Cleanup();

  for(u32 num = 0; num < GFL_NELEMS(mpBufferArray); ++num)  {
    mpBufferArray[num]  = (u8*)GflHeapAllocMemoryAlign(pDevHeap, size, align);
  }
  mWriteBufferIndex = 0;
  mBufferBytes      = size;
}

//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  CaptureBuffer::Cleanup(void)  {
  for(u32 num = 0; num < GFL_NELEMS(mpBufferArray); ++num)  {
    GflHeapFreeMemory(mpBufferArray[num]);
    mpBufferArray[num]  = NULL;
  }
  this->Invalidate();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
u8* CaptureBuffer::GetWriteBuffer(void) {
  u8*   pRet  = NULL;

  if((0 <= mWriteBufferIndex) && (mWriteBufferIndex < GFL_NELEMS(mpBufferArray)))  {
    pRet  = mpBufferArray[mWriteBufferIndex];
  }

  return pRet;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
const u8* CaptureBuffer::GetReadBuffer(void) const {
  u8*   pRet  = NULL;

  if((0 <= mReadBufferIndex) && (mReadBufferIndex < GFL_NELEMS(mpBufferArray)))  {
    pRet  = mpBufferArray[mReadBufferIndex];
  }

  return pRet;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
u8* CaptureBuffer::SwapWriteBuffer(void)  {
  nn::os::CriticalSection::ScopedLock   lock(mCS_Swap);

  do  {
    ++mWriteBufferIndex;
    mWriteBufferIndex %= GFL_NELEMS(mpBufferArray);
  }while(mWriteBufferIndex == mReadBufferIndex);

  return this->GetWriteBuffer();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
const u8*   CaptureBuffer::SwapReadBuffer(void)  {
  nn::os::CriticalSection::ScopedLock lock(mCS_Swap);

  do  {
    ++mReadBufferIndex;
    mReadBufferIndex %= GFL_NELEMS(mpBufferArray);
  }while(mWriteBufferIndex == mReadBufferIndex);

  return this->GetReadBuffer();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
Y2RBuffer::Y2RBuffer(void) :
    mCS_Swap()
  , mWriteBufferIndex(0)
  , mReadBufferIndex(0)
  , mBufferBytes(0)
{
  mCS_Swap.Initialize();
  this->Invalidate();
  for(u32 num = 0; num < GFL_NELEMS(mpBufferArray); ++num)  {
    mpBufferArray[num]  = NULL;
  }
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
Y2RBuffer::~Y2RBuffer() {
  mCS_Swap.Finalize();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  Y2RBuffer::Setup(gfl2::heap::HeapBase* pDevHeap, const u32 size, const u32 align) {
  Cleanup();

  for(u32 num = 0; num < GFL_NELEMS(mpBufferArray); ++num)  {
    mpBufferArray[num]  = (u8*)GflHeapAllocMemoryAlign(pDevHeap, size, align);
  }
  mWriteBufferIndex = 0;
  mBufferBytes      = size;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  Y2RBuffer::Cleanup(void) {
  
  for(u32 num = 0; num < GFL_NELEMS(mpBufferArray); ++num)  {
    GflHeapFreeMemory(mpBufferArray[num]);
    mpBufferArray[num]  = NULL;
  }
  this->Invalidate();
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
u8*  Y2RBuffer::GetWriteBuffer(void)  {
  u8*   pRet  = NULL;

  if((0 <= mWriteBufferIndex) && (mWriteBufferIndex < GFL_NELEMS(mpBufferArray))) {
    pRet  = mpBufferArray[mWriteBufferIndex];
  }

  return pRet;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
const u8*   Y2RBuffer::GetReadBuffer(void) const  {
  u8*   pRet  = NULL;

  if((0 <= mReadBufferIndex) && (mReadBufferIndex < GFL_NELEMS(mpBufferArray))) {
    pRet  = mpBufferArray[mReadBufferIndex];
  }

  return pRet;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
const u8*   Y2RBuffer::SwapBuffer(void) {
  nn::os::CriticalSection::ScopedLock  lock(mCS_Swap);

  if(0 <= mWriteBufferIndex)  {
    mWriteBufferIndex = (!mWriteBufferIndex) ? 1 : 0;
    mReadBufferIndex  = mWriteBufferIndex ^ 1;
  }

  return this->GetReadBuffer();
}




//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  CaptureBuffer::Invalidate(void)  {
  mWriteBufferIndex   =
  mReadBufferIndex    = -1;
}


//------------------------------------------------------------------
/**
  */
//------------------------------------------------------------------
void  Y2RBuffer::Invalidate(void) {
  mWriteBufferIndex   =
  mReadBufferIndex    = -1;
}



}  /*  namespace  devicecamera  */
}  /*  namespace  gfl2  */

#endif