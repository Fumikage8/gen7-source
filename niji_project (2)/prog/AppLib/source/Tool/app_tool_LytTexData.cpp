//========================================================================================
/**
 * @brief 1つのレイアウト用テクスチャデータを管理するクラス
 * @file app_tool_LytTexData.h
 * @author obata_toshihiro
 * @date 2011.06.01
 */
//========================================================================================
#include <AppLib/include/Tool/app_tool_LytTexData.h>

#include <base/include/gfl2_Singleton.h>
#include <util/include/gfl2_std_string.h>

#include <GameSys/include/GameManager.h>
#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>


namespace app {
  namespace tool {


    //----------------------------------------------------------------------------------
    /**
      * @brief テクスチャデータをTexInfoから生成する
      * @param[out]  pOutTexInfo:  作成して出力対象のTexInfo
      * @param[in]   pMemBuffer:     テクスチャーデータ生成で使用するメモリ領域
      * @param[in]   memBufferSize:  テクスチャーデータ生成で使用するメモリ領域サイズ
      * @param[in]   rTexInfo  テクスチャデータを生成する元になるTexInfo
      * @note
      *      ※引数 pMemBufferを使ってテクスチャーデータを生成しています。
      *             バッファサイズより大きいテクスチャーデータを生成するとAssertになります。
      *             あらかじめサイズを把握しておいてください
      */
    //----------------------------------------------------------------------------------
    bool LytTexData::CreateTexDataFromTexInfo( gfl2::lyt::LytResTextureInfo* pOutTexInfo, void* pMemBuffer, const u32 memBufferSize, void* pSrcTextureBuffer, gfl2::lyt::LytResTextureInfo& rTexInfo )
    {
      if( pOutTexInfo == NULL )
      {
        GFL_ASSERT( 0 );
        return  false;
      }

      if( pMemBuffer == NULL )
      {
        GFL_ASSERT( 0 );
        return  false;
      }

      if( pSrcTextureBuffer == NULL )
      {
        GFL_ASSERT( 0 );
        return  false;
      }

#if defined(GF_PLATFORM_CTR)
      //テクスチャフォーマットごとの１テクセル当たりのビット数
      static  const u32 texel_bit[] = 
      { 
         8, //TEXFORMAT_L8
         8, //TEXFORMAT_A8
         8, //TEXFORMAT_LA4
        16, //TEXFORMAT_LA8
        16, //TEXFORMAT_HILO8
        16, //TEXFORMAT_RGB565
        24, //TEXFORMAT_RGB8
        16, //TEXFORMAT_RGB5A1
        16, //TEXFORMAT_RGBA4
        32, //TEXFORMAT_RGBA8
        4,  //TEXFORMAT_ETC1
        8,  //TEXFORMAT_ETC1A4
        4,  //TEXFORMAT_L4
        4,  //TEXFORMAT_A4
      };
      nw::lyt::TexFormat format = rTexInfo.GetFormat();

      if( GFL_NELEMS( texel_bit ) <= format  )
      {
        //@fix　ありえないテクスチャフォーマットだと継続がむずかしいので、
        GFL_ASSERT_STOP( format < GFL_NELEMS( texel_bit ) );  

        return  false;
      }

      const nw::lyt::TexSize& r_size = rTexInfo.GetRealSize();

      size_t data_size = ( r_size.height * r_size.width * texel_bit[ format ] ) / 8;
      if( memBufferSize < ( data_size + sizeof( BclimFooter ) ))
      {
        GFL_ASSERT( 0 );
        return  false;
      }

      size_t real_size = data_size + app::tool::LytTexReplaceSystem::BCLIM_FOOTER_SIZE;
      //  ピクセルで埋める
      gfl2::std::MemCopy( pSrcTextureBuffer, pMemBuffer, data_size );

      //BCLIMフッタ作成（作る必要があるかどうか）
      BclimFooter footer;
      footer.clim.fileHeader.signature  = nw::lyt::res::FILESIGNATURE_FLIM;
      footer.clim.fileHeader.byteOrder  = 0xFEFF;
      footer.clim.fileHeader.headerSize = sizeof( footer.clim.fileHeader );
      footer.clim.fileHeader.version    = nw::lyt::res::BinaryFileFormatVersion;
      footer.clim.fileHeader.fileSize   = real_size;
      footer.clim.fileHeader.dataBlocks = 1;
      footer.clim.fileHeader.reserved   = 0;

      footer.imag.blockHeader.kind       = nw::lyt::res::DATABLOCKKIND_IMAGE;
      footer.imag.blockHeader.size       = 0x10;
      footer.imag.width                  = r_size.width;
      footer.imag.height                 = r_size.height;
      footer.imag.neededAlignment        = 128;
      footer.imag.format                 = format;
      footer.imag.flags                  = 0; //  nw::lyt::IMAGEFLAG_TEXELROTATION_LEN + nw::lyt::IMAGEFLAG_TEXELROTATION_POS;
      footer.size.imageSize              = data_size;

      u8* dst = (u8*)pMemBuffer;
      gfl2::std::MemCopy( &footer, (void*)&dst[ data_size ], sizeof( BclimFooter ) );

      // memo 引数でもらったtex_infoではテクスチャ書き換えがうまくいかないので、新たに生成する
      nw::lyt::LoadTexture( pOutTexInfo, pMemBuffer, real_size );
#endif
      return  true;
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_buffer テクスチャデータ用バッファの確保に使用するヒープ
     */
    //------------------------------------------------------------------------------------
    LytTexData::LytTexData(
        gfl2::heap::HeapBase* heap_for_buffer,
        u32 buffer_size,
        u32 buffer_align,
				gfl2::fs::AsyncFileManager* frman ) :
      m_load_flag( false ),
      m_tex_info(),
			m_tex_info_load_flg( false ),
      m_tex_buffer( NULL ),
      m_tex_buffer_size( buffer_size ),
      m_tex_buffer_align(buffer_align),
      m_heap(heap_for_buffer),
			m_asyncFileManager( frman ),
      m_tex_read_size( 0 ),
			m_read_seq( 0 )
    {
      m_load_run_flag = false;

      if( frman == NULL )
      {
        m_asyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      }
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //------------------------------------------------------------------------------------
    LytTexData::~LytTexData()
    {
			FreeTextureBuffer();
			FreeTextInfo();
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief テクスチャデータを読み込んだかどうかを調べる
     * @retval true   読み込み済み
     * @retval false  読み込んでいない
     */
    //------------------------------------------------------------------------------------
    bool LytTexData::IsLoaded( void ) const
    {
      return m_load_flag;
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief テクスチャデータを読み込む
     * @param heap_for_work     読み込みの作業用ワーク領域に使用するヒープ
     * @param arc_id            テクスチャデータをロードするarcID
     * @param data_id           読み込むテクスチャデータを指定
     * @param compressed        true: 圧縮されている, false: 圧縮されていない
     * @note 
     *   処理を呼ぶ前にあらかじめファイルを開く処理をしないといけない
     */
    //----------------------------------------------------------------------------------
    void LytTexData::LoadTexture(
        gfl2::heap::HeapBase* heap_for_work,
        gfl2::fs::ArcFile::ARCID    arc_id,
        gfl2::fs::ArcFile::ARCDATID data_id,
        bool compressed )
    {
      size_t data_size = 0;

      if( m_tex_info_load_flg == true ){
#if defined(GF_PLATFORM_WIN32)
        // @note DeleteTextureに相当するものがない
#elif defined(GF_PLATFORM_CTR)
        nw::lyt::DeleteTexture( m_tex_info );  // 2回目なので前の後処理
#endif
        m_tex_info_load_flg = false;
      }

      // 開いたarcファイルデータから目的のデータをロード(同期読み)
      if( m_tex_buffer_size == 0 )
      {
        FreeTextureBuffer();
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq loadReq;
        loadReq.arcId             = arc_id;
        loadReq.datId             = data_id;
        loadReq.ppBuf             = &m_tex_buffer;
        loadReq.pRealReadSize     = &data_size;
        loadReq.heapForBuf        = m_heap;
        loadReq.align             = m_tex_buffer_align;
        loadReq.heapForCompressed = (compressed)?(heap_for_work->GetLowerHandle()):(NULL);
        m_asyncFileManager->SyncArcFileLoadData( loadReq );
      }
      else
      {
        if( m_tex_buffer == NULL )
        {
          AllocTextureBuffer();
        }
        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq loadReq;
        loadReq.arcId               = arc_id;
        loadReq.datId               = data_id;
        loadReq.pBuf                = m_tex_buffer;
        loadReq.bufSize             = m_tex_buffer_size;
        loadReq.pRealReadSize       = &data_size;
        loadReq.alignForCompressed  = m_tex_buffer_align;
        loadReq.heapForReq          = heap_for_work->GetLowerHandle();
        loadReq.heapForCompressed   = (compressed)?(heap_for_work->GetLowerHandle()):(NULL);
        m_asyncFileManager->SyncArcFileLoadDataBuf( loadReq );
      }

      nw::lyt::LoadTexture( &m_tex_info, m_tex_buffer, data_size );

			m_tex_info_load_flg = true;
      m_load_flag = true;
			m_tex_read_size = data_size;
    }

    //----------------------------------------------------------------------------------
    /**
      * @brief テクスチャデータを読み込む(ロードは非同期)
      * @param heap_for_work     読み込みの作業用ワーク領域に使用するヒープ
      * @param arc_id            テクスチャデータをロードするarcID
      * @param data_id           読み込むテクスチャデータを指定
      * @param compressed        true: 圧縮されている, false: 圧縮されていない
      */
    //----------------------------------------------------------------------------------
    void LytTexData::LoadTextureAsync(
        gfl2::heap::HeapBase* heap_for_work,
        gfl2::fs::ArcFile::ARCID arc_id,
        gfl2::fs::ArcFile::ARCDATID data_id,
        bool compressed )
    {
      size_t data_size = 0;

      if( m_tex_info_load_flg == true ){
#if defined(GF_PLATFORM_WIN32)
        // @note DeleteTextureに相当するものがない
#elif defined(GF_PLATFORM_CTR)
        nw::lyt::DeleteTexture( m_tex_info );  // 2回目なので前の後処理
#endif
        m_tex_info_load_flg = false;
      }

      //  開いたarcファイルデータから目的のデータをロード
      if( LoadTextureRequest( heap_for_work, arc_id, data_id, compressed ) == false )
      {
        GFL_ASSERT( 0 );
      }
      else
      {
        m_load_run_flag  = true;
      }
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief テクスチャ情報を取得する
     */
    //------------------------------------------------------------------------------------
    const gfl2::lyt::LytResTextureInfo& LytTexData::GetTexInfo( void ) const
    {
      return m_tex_info;
    }

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータを取得
		 *
		 * @param		none
		 *
		 * @return	テクスチャデータ
		 */
		//----------------------------------------------------------------------------------
		void * LytTexData::GetTexData(void)
		{
			return m_tex_buffer;
		}

		//----------------------------------------------------------------------------------
		/**
		 * @brief		読み込んだテクスチャのサイズを取得
		 *
		 * @param		none
		 *
		 * @return	テクスチャサイズ
		 */
		//----------------------------------------------------------------------------------
		u32 LytTexData::GetTexSize(void) const
		{
			return m_tex_read_size;
		}



		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込みリクエスト ( FileReadManager使用 )
		 *
		 * @param		heap_for_work     読み込みの作業用ワーク領域に使用するヒープ
		 * @param		arcID             テクスチャデータを含むアーカイブデータ
		 * @param		datID             読み込むテクスチャデータを指定
		 * @param		compressed        true = 圧縮されている, false = 圧縮されていない
		 *
		 * @return	trueリクエスト成功  falseリクエスト失敗
     * @note 
     *    処理を呼ぶ前にあらかじめファイルを開く処理をしないといけない
     *    非同期読みをしている
		 */
		//----------------------------------------------------------------------------------
		bool LytTexData::LoadTextureRequest(
					gfl2::heap::HeapBase * heap_for_work,
					gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID datID, bool compressed )
		{
			m_load_flag = false;

			if( m_tex_buffer_size == 0 ){
				FreeTextureBuffer();
        gfl2::fs::AsyncFileManager::ArcFileLoadDataReq req;
        req.arcId               = arcID;
        req.datId               = datID;
        req.ppBuf               = &m_tex_buffer;
        req.pRealReadSize       = &m_tex_read_size;
        req.heapForBuf          = m_heap;
        req.align               = m_tex_buffer_align;
        req.heapForReq          = heap_for_work->GetLowerHandle();
        req.heapForCompressed   = (compressed)?(heap_for_work->GetLowerHandle()):(NULL);
        m_asyncFileManager->AddArcFileLoadDataReq( req );
			}else{
				if( m_tex_buffer == NULL ){
					AllocTextureBuffer();
				}
        gfl2::fs::AsyncFileManager::ArcFileLoadDataBufReq req;
        req.arcId               = arcID;
        req.datId               = datID;
        req.pBuf                = m_tex_buffer;
        req.bufSize             = m_tex_buffer_size;
        req.pRealReadSize       = &m_tex_read_size;
        req.alignForCompressed  = m_tex_buffer_align;
        req.heapForReq          = heap_for_work->GetLowerHandle();
        req.heapForCompressed   = (compressed)?(heap_for_work->GetLowerHandle()):(NULL);
        m_asyncFileManager->AddArcFileLoadDataBufReq( req );
			}

      return true;
		}

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込み ( FileReadManager使用 )
		 *
		 * @param		none
		 *
		 * @retval	"true = 読み込み終了"
		 * @retval	"false = それ以外"
		 */
		//----------------------------------------------------------------------------------
		bool LytTexData::LoadTexture(void)
		{
			if( m_tex_buffer == NULL ){
				m_load_flag = false;
			}else{
				m_load_flag = IsLoadTextureFinished();
			}

			if( m_load_flag == true ){
				FreeTextInfo();
        nw::lyt::LoadTexture( &m_tex_info, m_tex_buffer, m_tex_read_size );
				m_tex_info_load_flg = true;
        m_load_run_flag = false;
			}
			return m_load_flag;
		}

    //----------------------------------------------------------------------------------
    /**
     * @brief		テクスチャデータ読み込みリクエストをキャンセル ( FileReadManager使用 )
     *
     * @param		none
     *
     * @retval	"true = キャンセル成功"
     * @retval	"false = キャンセル失敗"
     */
    //----------------------------------------------------------------------------------
    bool LytTexData::CancelLoadTextureRequest(void)
    {
      if( m_tex_buffer_size == 0 )
      {
        return m_asyncFileManager->CancelArcFileLoadDataReq( &m_tex_buffer );
      }
      else
      {
        return m_asyncFileManager->CancelArcFileLoadDataBufReq( m_tex_buffer );
      }
    }

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込み終了チェック ( FileReadManager使用 )
		 *
		 * @param		none
		 *
		 * @retval	"true = 読み込み終了"
		 * @retval	"false = それ以外"
		 *
		 * @li	クラスが破棄できるかのチェックはIsModuleFree()を使用してください
		 */
		//----------------------------------------------------------------------------------
		bool LytTexData::IsLoadTextureFinished(void)
		{
      if( m_tex_buffer_size == 0 ){
        if( m_asyncFileManager->IsArcFileLoadDataFinished( &m_tex_buffer ) )
        {
          m_load_flag = true;
          return true;
        }
			}else{
        if( m_asyncFileManager->IsArcFileLoadDataBufFinished( m_tex_buffer ) )
        {
          m_load_flag = true;
          return true;
        }
			}
      return false;
		}

		//----------------------------------------------------------------------------------
		/**
		 * @brief		クラスが破棄できるかをチェック ( FileReadManager使用 )
		 *
		 * @param		none
		 *
		 * @retval	"true = 可"
		 * @retval	"false = 不可"
		 */
		//----------------------------------------------------------------------------------
		bool LytTexData::IsModuleFree(void)
		{
			if( m_tex_buffer == NULL ){
				return true;
			}
			return IsLoadTextureFinished();
		}

		//----------------------------------------------------------------------------------
		/**
		 * @brief		メモリ作成
		 *
		 * @param		none
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void LytTexData::AllocTextureBuffer(void)
		{
			if( m_tex_buffer_size == 0 ){
				GFL_ASSERT( 0 );
				return;
			}
			m_tex_buffer = GflHeapAllocMemoryAlign( m_heap, m_tex_buffer_size, m_tex_buffer_align );
		}

		//----------------------------------------------------------------------------------
		/**
		 * @brief		メモリ解放
		 *
		 * @param		none
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void LytTexData::FreeTextureBuffer(void)
		{
			if( m_tex_buffer != NULL ){
				GflHeapFreeMemory( m_tex_buffer );
				m_tex_buffer = NULL;
			}
			m_tex_read_size = 0;
			m_load_flag = false;
		}
    
		//----------------------------------------------------------------------------------
    /**
     * @brief   読み込みフラグを落とす
     */
		//----------------------------------------------------------------------------------
    void LytTexData::Clear(void)
    {
			m_load_flag = false;
    }

		//----------------------------------------------------------------------------------
		/**
		 * @brief		tex_info削除
		 *
		 * @param		none
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void LytTexData::FreeTextInfo(void)
		{
			if( m_tex_info_load_flg == true ){
#if defined(GF_PLATFORM_WIN32)
        // @note DeleteTextureに相当するものがない
#elif defined( GF_PLATFORM_CTR )
				nw::lyt::DeleteTexture( m_tex_info );  // 2回目なので前の後処理
#endif
				m_tex_info_load_flg = false;
			}
		}

    //----------------------------------------------------------------------------------
    /**
     * @brief   読み込んだテクスチャのサイズを取得
     *
     * @return  テクスチャサイズ
     */
    //----------------------------------------------------------------------------------
    u32 LytTexData::GetTextureBufferSize(void) const
    {
      return m_tex_buffer_size;
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief   読み込んだテクスチャのサイズを取得
     *
     * @return  テクスチャサイズ
     */
    //----------------------------------------------------------------------------------
    void LytTexData::CopyTextureBuffer( LytTexData * src )
    {
      if( m_tex_buffer == NULL )
      {
        GFL_ASSERT( 0 );
        return;
      }

      if( src->GetTextureBufferSize() != m_tex_buffer_size )
      {
        GFL_ASSERT( 0 );
        return;
      }

      gfl2::std::MemCopy( src->GetTexData(), m_tex_buffer, m_tex_buffer_size );
      m_tex_read_size = src->GetTexSize();
    }


  } // namespace tool
} // namespace app
