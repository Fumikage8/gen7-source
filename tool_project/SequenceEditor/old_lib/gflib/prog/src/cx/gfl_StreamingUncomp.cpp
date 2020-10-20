//============================================================================================
/**
 * @file		gfl_StreamingUncomp.cpp
 * @brief		LZ77圧縮データのストリーミング展開処理
 * @author	Hiroyuki Nakamura
 * @date		11.10.25
 */
//============================================================================================
#include "cx/gfl_StreamingUncomp.h"

namespace gfl {
namespace cx {



//----------------------------------------------------------------------------
/**
 *	@brief  圧縮解凍後のメモリサイズを取得
 *
 *	@return 圧縮解凍後のメモリサイズ
 */
//-----------------------------------------------------------------------------
u32 GetUncompressedSize( const void* cpData )
{
  return nn::cx::GetUncompressedSize(cpData);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		圧縮解凍処理
 *
 * @param		cpSrc		圧縮データ
 * @param		pDest		解凍先メモリ
 * @param		heap		ヒープ
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

	case nn::cx::COMPRESSION_HUFFMAN:	// ハフマン圧縮
	  nn::cx::UncompressHuffman( cpSrc, pDest );
		break;

	case nn::cx::COMPRESSION_RL:			// ランレングス圧縮
	  nn::cx::UncompressRL( cpSrc, pDest );
		break;

	case nn::cx::COMPRESSION_LH:			// LZ-ハフマン複合圧縮
		if( heap ){
  			void * work = GflHeapAllocMemoryLowAlign( heap, nn::cx::UNCOMPRESS_LH_WORK_SIZE, 2 );
	  		nn::cx::UncompressLH( reinterpret_cast<const u8 *>(cpSrc), reinterpret_cast<u8 *>(pDest), work );
		  	GflHeapFreeMemory( work );
    }
    else{
      GFL_ASSERT_STOP_MSG(0, "Uncompress heap == NULL");
    }
		break;

	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder複合圧縮
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
 * @brief		エラーチェック付き圧縮解凍処理（データタイプ指定）
 *
 * @param		cpSrc				圧縮データ
 * @param		cpSrcSize		圧縮データのサイズ
 * @param		type				圧縮データタイプ
 * @param		pDest				解凍先メモリ
 * @param		heap				ヒープ
 *
 * @retval	"0 = 成功"
 * @retval	"0 > エラーコード"
 */
//--------------------------------------------------------------------------------------------
s32 SecureUncompress( const void * cpSrc, u32 cpSrcSize, nn::cx::CompressionType type, void * pDest, gfl::heap::HeapBase * heap )
{
	switch( type ){
	case nn::cx::COMPRESSION_LZ:			// LZ77
		return nn::cx::SecureUncompressLZ( cpSrc, cpSrcSize, pDest );

	case nn::cx::COMPRESSION_HUFFMAN:	// ハフマン圧縮
		return nn::cx::SecureUncompressHuffman( cpSrc, cpSrcSize, pDest );

	case nn::cx::COMPRESSION_RL:			// ランレングス圧縮
		return nn::cx::SecureUncompressRL( cpSrc, cpSrcSize, pDest );

	case nn::cx::COMPRESSION_LH:			// LZ-ハフマン複合圧縮
		{
			void * work = GflHeapAllocMemoryLowAlign( heap, nn::cx::UNCOMPRESS_LH_WORK_SIZE, 4 );
			s32 ret = nn::cx::SecureUncompressLH(
									reinterpret_cast<const u8 *>(cpSrc), cpSrcSize, reinterpret_cast<u8 *>(pDest), work );
			GflHeapFreeMemory( work );
			return ret;
		}

	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder複合圧縮
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
 * @brief		エラーチェック付き圧縮解凍処理
 *
 * @param		cpSrc				圧縮データ
 * @param		cpSrcSize		圧縮データのサイズ
 * @param		pDest				解凍先メモリ
 * @param		heap				ヒープ
 *
 * @retval	"0 = 成功"
 * @retval	"0 > エラーコード"
 */
//--------------------------------------------------------------------------------------------
s32 SecureUncompress( const void * cpSrc, u32 cpSrcSize, void * pDest, gfl::heap::HeapBase * heap )
{
	return SecureUncompress( cpSrc, cpSrcSize, nn::cx::GetCompressionType(cpSrc), pDest, heap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
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
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
StreamingUncomp::~StreamingUncomp(void)
{
	ExitCompType();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		圧縮データ展開準備
 *
 * @param		devHeap		デバイスヒープ(普通のヒープメモリでもOKです)
 * @param		src				圧縮データ
 * @param		src_size	圧縮データのサイズ
 * @param		dest			展開先
 *
 * @return	none
 *
 * @li	destは各自削除を。
 */
//--------------------------------------------------------------------------------------------
void StreamingUncomp::InitUncomp( gfl::heap::HeapBase * devHeap, void * src, u32 src_size, void ** dest )
{
	InitUncompPackage( devHeap, src, src_size, dest );
	InitCompType( devHeap );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		圧縮タイプごとの設定
 *
 * @param		heap	ヒープ
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
	case nn::cx::COMPRESSION_HUFFMAN:	// ハフマン圧縮
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextHuffman), 4 );
		nn::cx::InitUncompContextHuffman( static_cast<nn::cx::UncompContextHuffman*>(context), *pDest );
		break;
	case nn::cx::COMPRESSION_RL:			// ランレングス圧縮
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextRL), 4 );
		nn::cx::InitUncompContextRL( static_cast<nn::cx::UncompContextRL*>(context), *pDest );
		break;
	case nn::cx::COMPRESSION_LH:			// LZ-ハフマン複合圧縮
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextLH), 4 );
		nn::cx::InitUncompContextLH( static_cast<nn::cx::UncompContextLH*>(context), *pDest );
		break;
	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder複合圧縮
		context = GflHeapAllocMemoryLowAlign( heap, sizeof(nn::cx::UncompContextLRC), 4 );
		nn::cx::InitUncompContextLRC( static_cast<nn::cx::UncompContextLRC*>(context), *pDest );
		break;

	default:
		GFL_ASSERT( 0 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		圧縮タイプごとの設定削除
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
 * @brief		圧縮データ展開
 *
 * @param		none
 *
 * @retval	"true = 処理中"
 * @retval	"false = それ以外"
 */
//--------------------------------------------------------------------------------------------
bool StreamingUncomp::ReadUncomp(void)
{
	s32	ret;

	switch( compType ){
	case nn::cx::COMPRESSION_LZ:			// LZ77
		ret = nn::cx::ReadUncompLZ( static_cast<nn::cx::UncompContextLZ*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_HUFFMAN:	// ハフマン圧縮
		ret = nn::cx::ReadUncompHuffman( static_cast<nn::cx::UncompContextHuffman*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_RL:			// ランレングス圧縮
		ret = nn::cx::ReadUncompRL( static_cast<nn::cx::UncompContextRL*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_LH:			// LZ-ハフマン複合圧縮
		ret = nn::cx::ReadUncompLH( static_cast<nn::cx::UncompContextLH*>(context), pSrc, srcSize );
		break;
	case nn::cx::COMPRESSION_LRC:			// LZ-RangeCoder複合圧縮
		ret = nn::cx::ReadUncompLRC( static_cast<nn::cx::UncompContextLRC*>(context), pSrc, srcSize );
		break;

	default:
		GFL_ASSERT( 0 );
		ret = -1;
	}

	// 終了
	if( ret == 0 ){
		ExitCompType();
		return false;
	// エラー
	}else if( ret < 0 ){
		GFL_ASSERT( 0 );
		ExitCompType();
		return false;
	}

	return true;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スレッドを使用した圧縮データ展開開始
 *
 * @param		arcID			アーカイブID
 * @param		datID			データID
 * @param		heap			ヒープ
 * @param		devHeap		デバイスヒープ(普通のヒープメモリでもOKです)
 * @param		dest			展開先
 * @param		priority	スレッドのプライオリティ
 *
 * @return	none
 *
 * @li	destは各自削除を。( = NULL の場合あり )
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
 * @brief		スレッドを使用した圧縮データ展開
 *
 * @param		none
 *
 * @retval	"true = 処理中"
 * @retval	"false = それ以外"
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
 * @brief		スレッドを使用した圧縮データ展開をキャンセルする
 *
 * @param		none
 *
 * @retval	"true = 処理中"
 * @retval	"false = 完了"
 *
 * @li	展開先も削除されます
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
 * @brief		圧縮データ一括展開準備
 *
 * @param		devHeap		デバイスヒープ(普通のヒープメモリでもOKです)
 * @param		src				圧縮データ
 * @param		src_size	圧縮データのサイズ
 * @param		dest			展開先
 *
 * @return	none
 *
 * @li	destは各自削除を。
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
 * @brief		圧縮データ一括展開(エラーチェック)
 *
 * @param		none
 *
 * @return  変換に成功した場合は 0 を、失敗した場合には負のエラーコードを返します。
 */
//--------------------------------------------------------------------------------------------
s32 StreamingUncomp::SecureUncomp(void)
{
	return SecureUncompress( pSrc, srcSize, *pDest, pDevHeap );
}



}		// cx
}		// gfl
