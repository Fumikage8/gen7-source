//============================================================================================
/**
 * @file		gfl_StreamingUncomp.cpp
 * @brief		LZ77���k�f�[�^�̃X�g���[�~���O�W�J����
 * @author	Hiroyuki Nakamura
 * @date		11.10.25
 */
//============================================================================================
#include "cx/gfl_StreamingUncomp.h"

namespace gfl {
namespace cx {



//----------------------------------------------------------------------------
/**
 *	@brief  ���k�𓀌�̃������T�C�Y���擾
 *
 *	@return ���k�𓀌�̃������T�C�Y
 */
//-----------------------------------------------------------------------------
u32 GetUncompressedSize( const void* cpData )
{
  return nn::cx::GetUncompressedSize(cpData);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���k�𓀏���
 *
 * @param		cpSrc		���k�f�[�^
 * @param		pDest		�𓀐惁����
 * @param		heap		�q�[�v
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Uncompress( const void * cpSrc, void * pDest, gfl::heap::HeapBase * heap )
{
	switch( nn::cx::GetCompressionType(cpSrc) ){
	case nn::cx::COMPRESSION_LZ:			// LZ77
	  nn::cx::UncompressLZ( cpSrc, pDest );
		break;

	case nn::cx::COMPRESSION_HUFFMAN:	// �n�t�}�����k
	  nn::cx::UncompressHuffman( cpSrc, pDest );
		break;

	case nn::cx::COMPRESSION_RL:			// ���������O�X���k
	  nn::cx::UncompressRL( cpSrc, pDest );
		break;

	case nn::cx::COMPRESSION_LH:			// LZ-�n�t�}���������k
		if( heap ){
  			void * work = GflHeapAllocMemoryLowAlign( heap, nn::cx::UNCOMPRESS_LH_WORK_SIZE, 2 );
	  		nn::cx::UncompressLH( reinterpret_cast<const u8 *>(cpSrc), reinterpret_cast<u8 *>(pDest), work );
		  	GflHeapFreeMemory( work );
    }
    else{
      GFL_ASSERT_STOP_MSG(0, "Uncompress heap == NULL");
    }
		break;

	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder�������k
		if( heap ){
			void * work = GflHeapAllocMemoryLowAlign( heap, nn::cx::UNCOMPRESS_LRC_WORK_SIZE, 4 );
			nn::cx::UncompressLRC( reinterpret_cast<const u8 *>(cpSrc), reinterpret_cast<u8 *>(pDest), work );
			GflHeapFreeMemory( work );
		}
    else{
      GFL_ASSERT_STOP_MSG(0, "Uncompress heap == NULL");
    }
		break;

	default:
		GFL_ASSERT( 0 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�G���[�`�F�b�N�t�����k�𓀏����i�f�[�^�^�C�v�w��j
 *
 * @param		cpSrc				���k�f�[�^
 * @param		cpSrcSize		���k�f�[�^�̃T�C�Y
 * @param		type				���k�f�[�^�^�C�v
 * @param		pDest				�𓀐惁����
 * @param		heap				�q�[�v
 *
 * @retval	"0 = ����"
 * @retval	"0 > �G���[�R�[�h"
 */
//--------------------------------------------------------------------------------------------
s32 SecureUncompress( const void * cpSrc, u32 cpSrcSize, nn::cx::CompressionType type, void * pDest, gfl::heap::HeapBase * heap )
{
	switch( type ){
	case nn::cx::COMPRESSION_LZ:			// LZ77
		return nn::cx::SecureUncompressLZ( cpSrc, cpSrcSize, pDest );

	case nn::cx::COMPRESSION_HUFFMAN:	// �n�t�}�����k
		return nn::cx::SecureUncompressHuffman( cpSrc, cpSrcSize, pDest );

	case nn::cx::COMPRESSION_RL:			// ���������O�X���k
		return nn::cx::SecureUncompressRL( cpSrc, cpSrcSize, pDest );

	case nn::cx::COMPRESSION_LH:			// LZ-�n�t�}���������k
		{
			void * work = GflHeapAllocMemoryLowAlign( heap, nn::cx::UNCOMPRESS_LH_WORK_SIZE, 4 );
			s32 ret = nn::cx::SecureUncompressLH(
									reinterpret_cast<const u8 *>(cpSrc), cpSrcSize, reinterpret_cast<u8 *>(pDest), work );
			GflHeapFreeMemory( work );
			return ret;
		}

	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder�������k
		{
			void * work = GflHeapAllocMemoryLowAlign( heap, nn::cx::UNCOMPRESS_LRC_WORK_SIZE, 4 );
			s32 ret = nn::cx::SecureUncompressLRC(
									reinterpret_cast<const u8 *>(cpSrc), cpSrcSize, reinterpret_cast<u8 *>(pDest), work );
			GflHeapFreeMemory( work );
			return ret;
		}

	default:
		GFL_ASSERT( 0 );
	}
	return 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�G���[�`�F�b�N�t�����k�𓀏���
 *
 * @param		cpSrc				���k�f�[�^
 * @param		cpSrcSize		���k�f�[�^�̃T�C�Y
 * @param		pDest				�𓀐惁����
 * @param		heap				�q�[�v
 *
 * @retval	"0 = ����"
 * @retval	"0 > �G���[�R�[�h"
 */
//--------------------------------------------------------------------------------------------
s32 SecureUncompress( const void * cpSrc, u32 cpSrcSize, void * pDest, gfl::heap::HeapBase * heap )
{
	return SecureUncompress( cpSrc, cpSrcSize, nn::cx::GetCompressionType(cpSrc), pDest, heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�R���X�g���N�^
 */
//--------------------------------------------------------------------------------------------
StreamingUncomp::StreamingUncomp(void) :
	pSrc( NULL ),
	context( NULL ),
	pArcFile( NULL ),
	seq( 0 ),
	cancelSeq( 0 )
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�f�X�g���N�^
 */
//--------------------------------------------------------------------------------------------
StreamingUncomp::~StreamingUncomp(void)
{
	ExitCompType();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���k�f�[�^�W�J����
 *
 * @param		devHeap		�f�o�C�X�q�[�v(���ʂ̃q�[�v�������ł�OK�ł�)
 * @param		src				���k�f�[�^
 * @param		src_size	���k�f�[�^�̃T�C�Y
 * @param		dest			�W�J��
 *
 * @return	none
 *
 * @li	dest�͊e���폜���B
 */
//--------------------------------------------------------------------------------------------
void StreamingUncomp::InitUncomp( gfl::heap::HeapBase * devHeap, void * src, u32 src_size, void ** dest )
{
	InitUncompPackage( devHeap, src, src_size, dest );
	InitCompType( devHeap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���k�^�C�v���Ƃ̐ݒ�
 *
 * @param		heap	�q�[�v
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void StreamingUncomp::InitCompType( gfl::heap::HeapBase * heap )
{
	compType = nn::cx::GetCompressionType( pSrc );

	switch( compType ){
	case nn::cx::COMPRESSION_LZ:			// LZ77
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextLZ), 4 );
		nn::cx::InitUncompContextLZ( static_cast<nn::cx::UncompContextLZ*>(context), *pDest );
		break;
	case nn::cx::COMPRESSION_HUFFMAN:	// �n�t�}�����k
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextHuffman), 4 );
		nn::cx::InitUncompContextHuffman( static_cast<nn::cx::UncompContextHuffman*>(context), *pDest );
		break;
	case nn::cx::COMPRESSION_RL:			// ���������O�X���k
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextRL), 4 );
		nn::cx::InitUncompContextRL( static_cast<nn::cx::UncompContextRL*>(context), *pDest );
		break;
	case nn::cx::COMPRESSION_LH:			// LZ-�n�t�}���������k
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextLH), 4 );
		nn::cx::InitUncompContextLH( static_cast<nn::cx::UncompContextLH*>(context), *pDest );
		break;
	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder�������k
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextLRC), 4 );
		nn::cx::InitUncompContextLRC( static_cast<nn::cx::UncompContextLRC*>(context), *pDest );
		break;

	default:
		GFL_ASSERT( 0 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���k�^�C�v���Ƃ̐ݒ�폜
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void StreamingUncomp::ExitCompType(void)
{
	if( context != NULL ){
		GflHeapFreeMemory( context );
		context = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���k�f�[�^�W�J
 *
 * @param		none
 *
 * @retval	"true = ������"
 * @retval	"false = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
bool StreamingUncomp::ReadUncomp(void)
{
	s32	ret;

	switch( compType ){
	case nn::cx::COMPRESSION_LZ:			// LZ77
		ret = nn::cx::ReadUncompLZ( static_cast<nn::cx::UncompContextLZ*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_HUFFMAN:	// �n�t�}�����k
		ret = nn::cx::ReadUncompHuffman( static_cast<nn::cx::UncompContextHuffman*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_RL:			// ���������O�X���k
		ret = nn::cx::ReadUncompRL( static_cast<nn::cx::UncompContextRL*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_LH:			// LZ-�n�t�}���������k
		ret = nn::cx::ReadUncompLH( static_cast<nn::cx::UncompContextLH*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder�������k
		ret = nn::cx::ReadUncompLRC( static_cast<nn::cx::UncompContextLRC*>(context), pSrc, srcSize );
		break;

	default:
		GFL_ASSERT( 0 );
		ret = -1;
	}

	// �I��
	if( ret == 0 ){
		ExitCompType();
		return false;
	// �G���[
	}else if( ret < 0 ){
		GFL_ASSERT( 0 );
		ExitCompType();
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X���b�h���g�p�������k�f�[�^�W�J�J�n
 *
 * @param		arcID			�A�[�J�C�uID
 * @param		datID			�f�[�^ID
 * @param		heap			�q�[�v
 * @param		devHeap		�f�o�C�X�q�[�v(���ʂ̃q�[�v�������ł�OK�ł�)
 * @param		dest			�W�J��
 * @param		priority	�X���b�h�̃v���C�I���e�B
 *
 * @return	none
 *
 * @li	dest�͊e���폜���B( = NULL �̏ꍇ���� )
 */
//--------------------------------------------------------------------------------------------
void StreamingUncomp::InitThreadUncomp(
			gfl::fs::ArcFile::ARCID arcID, gfl::fs::ArcFile::ARCDATID datID,
			gfl::heap::HeapBase * heap, gfl::heap::HeapBase * devHeap, void ** dest, u32 priority )
{
	pArcFile = GFL_NEW_LOW( heap ) gfl::fs::ArcFile( heap, arcID, gfl::fs::ArcFile::OPEN );
	pDest    = dest;
	*pDest   = NULL;
	srcSize  = pArcFile->GetDataSize( datID );
	pSrc     = GflHeapAllocMemoryLowAlign( devHeap, srcSize, 128 );
	pDevHeap = devHeap;

	pArcFile->StartAsyncRead( heap, datID, pSrc, priority, NULL, NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X���b�h���g�p�������k�f�[�^�W�J
 *
 * @param		none
 *
 * @retval	"true = ������"
 * @retval	"false = ����ȊO"
 */
//--------------------------------------------------------------------------------------------
bool StreamingUncomp::ReadThreadUncomp(void)
{
	switch( seq ){
	case 0:
		if( pArcFile->WaitAsyncRead() == false ){
			GFL_DELETE pArcFile;
			pArcFile = NULL;

			*pDest = GflHeapAllocMemoryAlign( pDevHeap, nn::cx::GetUncompressedSize(pSrc), 128 );

			InitCompType( pDevHeap );

			seq++;
		}else{
			break;
		}
	case 1:
		if( ReadUncomp() == false ){
			GflHeapFreeMemory( pSrc );
			pSrc = NULL;
			seq = 2;
			return false;
		}
		break;

	case 2:
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		�X���b�h���g�p�������k�f�[�^�W�J���L�����Z������
 *
 * @param		none
 *
 * @retval	"true = ������"
 * @retval	"false = ����"
 *
 * @li	�W�J����폜����܂�
 */
//--------------------------------------------------------------------------------------------
bool StreamingUncomp::CancelThreadUncomp(void)
{
	switch( cancelSeq ){
	case 0:
		if( pArcFile == NULL ){
			return false;
		}
		pArcFile->CancelAsyncRead();
		cancelSeq++;

	case 1:
		if( pArcFile->WaitAsyncRead() == true ){
			break;
		}
		GFL_DELETE pArcFile;

		if( *pDest != NULL ){
			GflHeapFreeMemory( *pDest );
			*pDest = NULL;
		}

		if( pSrc != NULL ){
			GflHeapFreeMemory( pSrc );
			pSrc = NULL;
		}

		cancelSeq = 0;
		return false;
	}

	return true;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		���k�f�[�^�ꊇ�W�J����
 *
 * @param		devHeap		�f�o�C�X�q�[�v(���ʂ̃q�[�v�������ł�OK�ł�)
 * @param		src				���k�f�[�^
 * @param		src_size	���k�f�[�^�̃T�C�Y
 * @param		dest			�W�J��
 *
 * @return	none
 *
 * @li	dest�͊e���폜���B
 */
//--------------------------------------------------------------------------------------------
void StreamingUncomp::InitUncompPackage( gfl::heap::HeapBase * devHeap, void * src, u32 src_size, void ** dest )
{
	pDevHeap = devHeap;

	pSrc    = src;
	srcSize = src_size;

	pDest  = dest;
	*pDest = GflHeapAllocMemoryAlign( devHeap, nn::cx::GetUncompressedSize(pSrc), 128 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		���k�f�[�^�ꊇ�W�J(�G���[�`�F�b�N)
 *
 * @param		none
 *
 * @return  �ϊ��ɐ��������ꍇ�� 0 ���A���s�����ꍇ�ɂ͕��̃G���[�R�[�h��Ԃ��܂��B
 */
//--------------------------------------------------------------------------------------------
s32 StreamingUncomp::SecureUncomp(void)
{
	return SecureUncompress( pSrc, srcSize, *pDest, pDevHeap );
}



}		// cx
}		// gfl
