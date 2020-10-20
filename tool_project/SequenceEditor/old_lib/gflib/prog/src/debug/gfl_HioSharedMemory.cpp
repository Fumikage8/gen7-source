//=============================================================================================
/**
 * @file	  gfl_HioSharedMemory.h
 * @author	soga
 * @brief	  HIO��SharedMemory���g�p�����ʐM
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
 *  @brief  �R���X�g���N�^
 *
 *  @param[in]  appHeap �q�[�v
 *  @param[in]  devHeap �f�o�C�X�q�[�v
 *  @param[in]  ch      �I�[�v������`�����l��
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
 *  @brief  �f�X�g���N�^
 */
//---------------------------------------------------------------------------------------------------------
HioSharedMemory::~HioSharedMemory()
{ 
  GFL_DELETE( mWorkMemory );
  GFL_DELETE( mSharedMemory );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  ������ 
 */
//---------------------------------------------------------------------------------------------------------
void  HioSharedMemory::Initialize( void )
{ 
  mWritePos = mWritePosTop;
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  �I�[�v��
 *
 *  @param[in]  ch          �I�[�v������`�����l��
 *  @param[in]  pWorkMemory ���C�u�������g�p���郏�[�N�������i�f�o�C�X�������ł���K�v������j
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Open( int ch, void* pWorkMemory )
{ 
  return  mSharedMemory->Open( ch, pWorkMemory );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  �N���[�Y
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Close( void )
{ 
  this->Initialize();
  return  mSharedMemory->Close();
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  ���L�������̃T�C�Y���擾
 */
//---------------------------------------------------------------------------------------------------------
size_t  HioSharedMemory::GetSpaceSize()
{ 
  return  mSharedMemory->GetSpaceSize();
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  ���L����������ǂݍ���
 *
 *  @param[in]  read_buf  �ǂݍ��݃o�b�t�@
 *  @param[in]  length    �ǂݍ��݃T�C�Y
 *  @param[in]  offset    �ǂݍ��ݐ拤�L�������̐擪����̃I�t�Z�b�g
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Read( void* read_buf, size_t length, int offset )
{ 
  return  mSharedMemory->Read( read_buf, length, offset );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  ���L�������֏�������
 *
 *  @param[in]  write_buf �������݃o�b�t�@
 *  @param[in]  length    �������݃T�C�Y
 *  @param[in]  offset    �������ݐ拤�L�������̐擪����̃I�t�Z�b�g
 */
//---------------------------------------------------------------------------------------------------------
nn::Result  HioSharedMemory::Write( const void* write_buf, size_t length, int offset )
{ 
  return  mSharedMemory->Write( write_buf, length, offset );
}

//---------------------------------------------------------------------------------------------------------
/**
 *  @brief  ���L�������֏������݁i�����O�o�b�t�@�I�ɏ������ށj
 *
 *  @param[in]  write_buf     �������݃o�b�t�@
 *  @param[in]  length        �������݃T�C�Y
 *  @param[in]  to_top_buf    �擪�ւ̖߂肪��������Ƃ��ɏ������ޒl
 *  @param[in]  to_top_length �擪�ւ̖߂肪��������Ƃ��ɏ������ޒl�̃T�C�Y
 *  @param[out] to_top_result �擪�ւ̖߂肪�����������ǂ���
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
 *  @brief  ���L�������ւ̏������݊J�n�ʒu���w��i�����O�o�b�t�@�I�Ɏg���Ƃ��̖�������̖߂�ʒu�j
 *
 *  @param[in]  value �������݊J�n�ʒu
 *
 *  @retval true:����@false:���L�������T�C�Y���傫���l���w�肵��
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
