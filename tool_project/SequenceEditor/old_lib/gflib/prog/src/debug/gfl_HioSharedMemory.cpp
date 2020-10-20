//=============================================================================================
/**
 * @file	  gfl_HioSharedMemory.h
 * @author	soga
 * @brief	  HIOのSharedMemoryを使用した通信
 * @date    2013.04.13
 */
//=============================================================================================

#if GFL_HIO_ENABLE
#include  "debug/gfl_HioSharedMemory.h"

#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::hio::HioSharedMemory);
template class gfl::base::SingletonAccessor<gfl::hio::HioSharedMemory>;
#endif

namespace gfl{  
namespace hio{  

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  コンストラクタ
 *
 *  @param[in]  appHeap ヒープ
 *  @param[in]  devHeap デバイスヒープ
 *  @param[in]  ch      オープンするチャンネル
 */
//---------------------------------------------------------------------------------------------------------
HioSharedMemory::HioSharedMemory( gfl::heap::HeapBase* appHeap, gfl::heap::HeapBase* devHeap, int ch )
{ 
  mWorkMemory   = GflHeapAllocMemory( devHeap, nn::hio::CTR::WORKMEMORY_SIZE );
  mSharedMemory = GFL_NEW( appHeap ) nn::hio::CTR::SharedMemoryChannel();
  mWritePosTop  = 0;
  this->Initialize();
  this->Open( ch, mWorkMemory );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  デストラクタ
 */
//---------------------------------------------------------------------------------------------------------
HioSharedMemory::~HioSharedMemory()
{ 
  GFL_DELETE( mWorkMemory );
  GFL_DELETE( mSharedMemory );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  初期化 
 */
//---------------------------------------------------------------------------------------------------------
void  HioSharedMemory::Initialize( void )
{ 
  mWritePos = mWritePosTop;
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  オープン
 *
 *  @param[in]  ch          オープンするチャンネル
 *  @param[in]  pWorkMemory ライブラリが使用するワークメモリ（デバイスメモリである必要がある）
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Open( int ch, void* pWorkMemory )
{ 
  return  mSharedMemory->Open( ch, pWorkMemory );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  クローズ
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Close( void )
{ 
  this->Initialize();
  return  mSharedMemory->Close();
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  共有メモリのサイズを取得
 */
//---------------------------------------------------------------------------------------------------------
size_t  HioSharedMemory::GetSpaceSize()
{ 
  return  mSharedMemory->GetSpaceSize();
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  共有メモリから読み込み
 *
 *  @param[in]  read_buf  読み込みバッファ
 *  @param[in]  length    読み込みサイズ
 *  @param[in]  offset    読み込み先共有メモリの先頭からのオフセット
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Read( void* read_buf, size_t length, int offset )
{ 
  return  mSharedMemory->Read( read_buf, length, offset );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  共有メモリへ書き込み
 *
 *  @param[in]  write_buf 書き込みバッファ
 *  @param[in]  length    書き込みサイズ
 *  @param[in]  offset    書き込み先共有メモリの先頭からのオフセット
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Write( const void* write_buf, size_t length, int offset )
{ 
  return  mSharedMemory->Write( write_buf, length, offset );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  共有メモリへ書き込み（リングバッファ的に書き込む）
 *
 *  @param[in]  write_buf     書き込みバッファ
 *  @param[in]  length        書き込みサイズ
 *  @param[in]  to_top_buf    先頭への戻りが発生するときに書き込む値
 *  @param[in]  to_top_length 先頭への戻りが発生するときに書き込む値のサイズ
 *  @param[out] to_top_result 先頭への戻りが発生したかどうか
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::WriteRingBuffer( const void* write_buf, size_t length, const void* to_top_buf, size_t to_top_length, bool* to_top_result )
{ 
  bool        write_result = true;
  nn::Result  result;

  if( to_top_result != NULL ) *to_top_result = false;

  if( ( mWritePos + length + to_top_length ) > this->GetSpaceSize() )
  { 
    result = mSharedMemory->Write( to_top_buf, to_top_length, mWritePos );
    if( result.IsSuccess() )
    { 
      char  buf[ 8 ];
      sprintf( buf, "%04d ", mWritePosTop );
      result = mSharedMemory->Write( buf, 5, mWritePos + to_top_length );
      if( result.IsSuccess() )
      { 
        GFL_PRINT("to_top w_pos:%d\n",mWritePos);
        mWritePos = mWritePosTop;
        if( to_top_result != NULL ) *to_top_result = true;
      }
    }
    if( result.IsFailure() )
    { 
      write_result = false;
    }
  }

  if( write_result != false )
  { 
    result = mSharedMemory->Write( write_buf, length, mWritePos );
    if( result.IsSuccess() )
    { 
      mWritePos += length;
    }
  }
  return result;
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  共有メモリへの書き込み開始位置を指定（リングバッファ的に使うときの末尾からの戻り位置）
 *
 *  @param[in]  value 書き込み開始位置
 *
 *  @retval true:正常　false:共有メモリサイズより大きい値を指定した
 */
//---------------------------------------------------------------------------------------------------------
bool  HioSharedMemory::SetWritePosTop( s32 value )
{ 
  if( this->GetSpaceSize() <= value ) return false;

  mWritePosTop = value;

  return true;
}

} //namespace hio
} //namespace gfl

#endif
