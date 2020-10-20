//============================================================================================
/**
 * @file		gfl2_StreamingUncomp.cpp
 * @brief		LZ77圧縮データのストリーミング展開処理
 * @author	Hiroyuki Nakamura
 * @date		11.10.25
 */
//============================================================================================
#include <cx/include/gfl2_StreamingUncomp.h>
#include <debug/include/gfl2_Assert.h>

#if defined(GF_PLATFORM_WIN32)
// cafe
#include <zlib.h>		//$(CAFE_ROOT)/system/include/zlib.h
// gflib2
#include <debug/include/gfl2_DebugPrint.h>
#endif

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(cx)

#if defined(GF_PLATFORM_WIN32)

enum WindowBits
{
	WB_DEFAULT		= 15,								//! 標準？inflateInitではデフォルトとして15を指定
	WB_GZIP_ONLY	= WB_DEFAULT + 16,	//! GZIPフォーマットのみデコード
	WB_ZIP_GZIP		= WB_DEFAULT + 32		//! 自動検出ヘッダでZIPデコードとGZIPデコードを有効
};

#endif

//----------------------------------------------------------------------------
/**
 *	@brief  圧縮解凍後のメモリサイズを取得
 *
 *	@return 圧縮解凍後のメモリサイズ
 */
//-----------------------------------------------------------------------------
u32 GetUncompressedSize( const void* cpData, u32 nCompSize )
{
#if defined(GF_PLATFORM_WIN32)
	u8* pCompBuf = (u8*)cpData;

	const u8 offset_size = 4;
	u8 size_buf[offset_size];
	memcpy(size_buf, &pCompBuf[nCompSize - offset_size], offset_size);
	
	u32 uncomp_size = size_buf[0];
	uncomp_size += size_buf[1] << 8;
	uncomp_size += size_buf[2] << 16;
	uncomp_size += size_buf[3] << 24;

	return uncomp_size;

#elif defined(GF_PLATFORM_CTR)
  return nn::cx::GetUncompressedSize(cpData);
#endif
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
void Uncompress( const void * cpSrc, void * pDest, gfl2::heap::HeapBase * heap, u32 nCompSize )
{
#if defined(GF_PLATFORM_WIN32)
	// 展開に必要なストリームを初期化
	z_stream stream;
	memset( &stream, 0x00, sizeof(stream) );

	int ret = inflateInit2( &stream, WB_ZIP_GZIP );
	GFL_ASSERT_STOP( ret == Z_OK );

	stream.avail_in = nCompSize;
	stream.next_in = (Bytef*)cpSrc;

	// 展開後のファイル用バッファ
	u32 uncomp_size = GetUncompressedSize( cpSrc, nCompSize );
	Bytef* uncomp_buf = (Bytef*)pDest;

	stream.avail_out = uncomp_size;
	stream.next_out = uncomp_buf;

	// 展開処理
	ret = inflate( &stream, Z_FINISH );
	GFL_ASSERT_STOP( ret == Z_STREAM_END );

	// 展開の終了処理
	ret = inflateEnd( &stream );
	GFL_ASSERT_STOP( ret == Z_OK );

#elif defined(GF_PLATFORM_CTR)

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
#endif
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
#if defined(GF_PLATFORM_CTR)
s32 SecureUncompress( const void * cpSrc, u32 cpSrcSize, nn::cx::CompressionType type, void * pDest, gfl2::heap::HeapBase * heap )
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
#endif

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
u32 SecureUncompress( const void * cpSrc, u32 cpSrcSize, void * pDest, gfl2::heap::HeapBase * heap )
{
#if defined(GF_PLATFORM_WIN32)
	Uncompress( cpSrc, pDest, heap, cpSrcSize );
	return 0;
#elif defined(GF_PLATFORM_CTR)
	return SecureUncompress( cpSrc, cpSrcSize, nn::cx::GetCompressionType(cpSrc), pDest, heap );
#endif
}

#if 0
//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//--------------------------------------------------------------------------------------------
StreamingUncomp::StreamingUncomp(void) :
	pSrc( NULL ),
	context( NULL ),
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
void StreamingUncomp::InitUncomp( gfl2::heap::HeapBase * devHeap, void * src, u32 src_size, void ** dest )
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
void StreamingUncomp::InitCompType( gfl2::heap::HeapBase * heap )
{
#if defined(GF_PLATFORM_WIN32)
#elif defined(GF_PLATFORM_CTR)
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
#endif
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
#if defined(GF_PLATFORM_WIN32)
#elif defined(GF_PLATFORM_CTR)
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
#endif

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
void StreamingUncomp::InitUncompPackage( gfl2::heap::HeapBase * devHeap, void * src, u32 src_size, void ** dest )
{
#if defined(GF_PLATFORM_WIN32)
#elif defined(GF_PLATFORM_CTR)
	pDevHeap = devHeap;

	pSrc    = src;
	srcSize = src_size;

	pDest  = dest;
	*pDest = GflHeapAllocMemoryAlign( devHeap, nn::cx::GetUncompressedSize(pSrc), 128 );
#endif
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
#endif

////////////////////////////////////////////////////////////////////////////////
#if defined(GF_PLATFORM_WIN32)
// サンプル
void UncompTest( void )
{
	gfl2::heap::HeapBase* pHeap = gfl2::heap::Manager::GetHeapByHeapId( gfl2::heap::HEAPID_SYSTEM );
	u8 *pCompBuf = NULL, *pUncompBuf = NULL;

	// gzip圧縮ファイルの読み込み
	FILE* pFile = NULL;
	fopen_s( &pFile, "romfiles/data/cx_sample_data/gzip_test_file.txt.gz", "rb" );
	if( pFile == NULL ) return;

	// 圧縮時のファイルサイズの取得
	fseek( pFile, 0, SEEK_END );
	int comp_size = ftell( pFile );
	fseek( pFile, 0, SEEK_SET );

	// 圧縮データのバッファ
	pCompBuf = GFL_NEW(pHeap) u8[comp_size+1];
	if( pCompBuf == NULL ) return;
	
	memset( pCompBuf, 0x00, (comp_size + 1) );

	// ファイル読み込み
	fread( pCompBuf, 1, comp_size, pFile );

	// 展開後のファイルサイズを取得
	u32 uncomp_size = gfl2::cx::GetUncompressedSize( pCompBuf, comp_size );
	// 展開後データのバッファ
	pUncompBuf = GFL_NEW(pHeap) u8[uncomp_size+1];
	if( pUncompBuf != NULL )
	{
		memset( pUncompBuf, 0x00, (uncomp_size + 1) );

		// 展開
		gfl2::cx::Uncompress( pCompBuf, pUncompBuf, NULL, comp_size );
		//gfl2::cx::SecureUncompress( comp_buf, comp_size, uncomp_buf, NULL );

		GFL_PRINT("comp_size[%d]\n", comp_size);
		GFL_PRINT("uncomp_size[%d]\n", uncomp_size);
		GFL_PRINT("uncomp_data[%s]\n", pUncompBuf);

		GFL_DELETE_ARRAY pUncompBuf;
		pUncompBuf = NULL;
	}

	GFL_DELETE_ARRAY pCompBuf;
	pCompBuf = NULL;
}
#endif
////////////////////////////////////////////////////////////////////////////////


GFL_NAMESPACE_END(cx)
GFL_NAMESPACE_END(gfl2)
