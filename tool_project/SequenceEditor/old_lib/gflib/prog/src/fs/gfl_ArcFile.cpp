//=============================================================================
/**
 * @brief  GFアーカイブファイルの読み込み
 * @file   gfl_ArcFile.cpp
 * @author obata_toshihiro
 * @date   2011.03.20
 */
//=============================================================================
#include <gfl_debug.h>
#include "cx/gfl_StreamingUncomp.h"
#include "fs/gfl_RomFile.h"
#include "fs/gfl_ArcFile.h"
#include "./gfl_ArcFile_SetupThread.h"
#include "./ArcFileAccessor/ver.2.x/gfl_ArcFileAccessor.h"
#include "./ArcFileAccessor/ver.3.x/gfl_ArcFileAccessor.h"
#include "./ArcFileAccessor/ver.4.x/gfl_ArcFileAccessor.h"


namespace gfl {
  namespace fs {


    u32 ArcFile::LANGUAGE_CODE = 0;
    size_t ArcFile::ARCFILE_NUM = 0;
    const wchar_t** ArcFile::FILENAME_TABLE = NULL; 
#if GFL_FILE_DEBUG
    const wchar_t** ArcFile::SD_FILENAME_TABLE = NULL;
    bool ArcFile::m_isEnableSd = false;
#endif
#if GFL_HIO_ENABLE
    const wchar_t** ArcFile::HIO_FILENAME_TABLE = NULL; 
#endif


    //-------------------------------------------------------------------------
    /**
     * @brief 言語コードを設定する
     * @param language  設定する言語コード
     */
    //-------------------------------------------------------------------------
    void ArcFile::SetLanguage( u32 language )
    {
      LANGUAGE_CODE = language;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief アーカイブファイルの名前テーブルを設定する
     * @param filename_table  アーカイブIDに対応するファイル名の配列
     * @param arcfile_num     ファイル名配列の要素数
     */
    //-------------------------------------------------------------------------
    void ArcFile::SetFileNameTable( const wchar_t** filename_table, size_t arcfile_num )
    {
      FILENAME_TABLE = filename_table;
      ARCFILE_NUM = arcfile_num;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アーカイブIDに対応するファイル名を取得する
     * @param arcid  ファイル名を取得するアーカイブID
     * @return 対応するROMアーカイブ上のファイル名
     */
    //-------------------------------------------------------------------------
    const wchar_t* ArcFile::GetRomFileName( ARCID arcid )
    {
      return GetFileName( arcid, FILENAME_TABLE, ARCFILE_NUM );
    }

    /**
     * @brief アーカイブIDに対応するファイル名を取得する
     * @param arcid          アーカイブID
     * @param filename_table ファイル名テーブル
     * @param table_size     ファイル名テーブルの要素数
     */
    const wchar_t* ArcFile::GetFileName( 
      ARCID arcid,
      const wchar_t** filename_table,
      u32 table_size )
    {
      GFL_ASSERT_MSG( ( arcid < table_size ), "不正なアーカイブIDです。(%d)\n", arcid ); //@check
      GFL_ASSERT_MSG( ( filename_table != NULL ), "ファイル名テーブルがセットされていません。\n" );//@check
      return filename_table[ arcid ];
    }

#if GFL_FILE_DEBUG
    //SDカード読み込み機能
    /**
     * @brief ArcをSDカードから読み込むためのファイル名テーブルの設定
     * @param filename_table  アーカイブIDに対応するファイル名の配列
     */
    void ArcFile::SetSDMountFileNameTable( const wchar_t** filename_table )
    {
      SD_FILENAME_TABLE = filename_table;
    }

    /**
     * @brief ArcをSDカードから読み込むかのフラグ(ファイルが存在しなければ通常の読み込みをします)
     * @param bool 有効・無効
     */
    void ArcFile::SetSDMountIsEnable( bool flg )
    {
      m_isEnableSd = flg;
    }

    /**
     * @brief ArcをSDカードから読み込むかのフラグの取得
     * @return bool 有効・無効
     */
    bool ArcFile::GetSDMountIsEnable( void )
    {
      return m_isEnableSd;
    }

      /**
       * @brief ArcをSDカードから読み込む時に対応するファイル名を取得する
       * @param arcid  ファイル名を取得するアーカイブID
       * @return 対応するアーカイブ上のファイル名
       */
    const wchar_t* ArcFile::GetSDMountFileName( ARCID arcid )
    {
      return GetFileName( arcid, SD_FILENAME_TABLE, ARCFILE_NUM );
    }

#endif

#if GFL_HIO_ENABLE
    //-------------------------------------------------------------------------
    /**
     * @brief HIOアクセスするためのファイル名テーブルを設定する
     * @param filename_table  アーカイブIDに対応するファイル名の配列
     */
    //-------------------------------------------------------------------------
    void ArcFile::SetHioFileNameTable( const wchar_t** filename_table )
    {
      HIO_FILENAME_TABLE = filename_table;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アーカイブIDに対応するファイル名を取得する
     * @param arcid  ファイル名を取得するアーカイブID
     * @return 対応するホストファイルシステム上のファイル名
     */
    //-------------------------------------------------------------------------
    const wchar_t* ArcFile::GetHioFileName( ARCID arcid )
    {
      return GetFileName( arcid, HIO_FILENAME_TABLE, ARCFILE_NUM );
    }
#endif





    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param heap       作業ワークに使用するヒープ
     * @param arcID      関連付けるアーカイブファイル
     * @param init_flag  コンストラクタで行う処理を InitBit の論理和で指定
     */
    //-------------------------------------------------------------------------
    ArcFile::ArcFile( gfl::heap::HeapBase* heap, ARCID arcID, u32 init_flag ) :
      m_workHeap( heap ),
      m_file(),
      m_filename( NULL ),
      m_dataNum( 0 ),
      m_maxDataSize( 0 ),
      m_fastAccessTable( NULL ),
      m_setupThread( NULL ),
      m_fileOpenEvent(),
      m_fastSetupEvent(),
      m_criticalSectionForFileOpen(),
      m_criticalSectionForFileRead(),
      m_criticalSectionForFastSetup()
#if GFL_HIO_ENABLE
      ,m_isHioFile( false )
#endif
#if GFL_DEBUG
      ,m_majorVersion( 0 )
      ,m_minorVersion( 0 )
#endif
    {
#if GFL_FILE_DEBUG
      //SDカードにアーカイブがあるか？
      if( m_isEnableSd && SD_FILENAME_TABLE )
      {
        GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
        //そのファイルがあるかをチェックし、実際に読み込むかを決める。
        if( gfl::fs::FileSystem::CheckRomFileExist( SD_FILENAME_TABLE[ arcID ] ) )
        {
          m_filename = SD_FILENAME_TABLE[ arcID ];
          ARIIZUMI_PRINT("SDカードからアーク発見！[%ls]\n",m_filename);
        }
      }
#endif
      //SDカードが読まれていたら読まない
      if( m_filename == NULL )
      {
#if GFL_HIO_ENABLE
        if( init_flag & HIO ) {
          if( HIO_FILENAME_TABLE )
          {

            GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
            m_filename = HIO_FILENAME_TABLE[ arcID ];
            //そのファイルがあるかをチェックし、実際に読み込むかを決める。
            if( gfl::fs::FileSystem::CheckHioFileExist( m_filename ) ){
              m_isHioFile = true;
            }
          }
          if( m_isHioFile == false )
          {
            // HIO無効
            TOMOYA_PRINT( "ファイルがないHIO無効 %ls\n", m_filename );
            m_filename = FILENAME_TABLE[ arcID ];
            m_isHioFile = false;
          }
        }
        else {
          GFL_ASSERT( FILENAME_TABLE );  //@check
          GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
          m_filename = FILENAME_TABLE[ arcID ];
          m_isHioFile = false;
        }
#else
        GFL_ASSERT( FILENAME_TABLE );  //@check
        GFL_ASSERT( arcID < ARCFILE_NUM );  //@check
        m_filename = FILENAME_TABLE[ arcID ];
#endif
      }

      this->InitExclusiveObjects();

      if( init_flag & OPEN ) {
        this->Open();
      }

      if( (init_flag & OPEN_FASTMODE) == OPEN_FASTMODE ) {
        this->SetupForFastMode( m_workHeap );
      }
    }

    /**
     * @brief 排他制御オブジェクトを初期化する
     */
    void ArcFile::InitExclusiveObjects( void )
    {
      m_fileOpenEvent.Initialize( true );  // 自動リセットしない
      m_fastSetupEvent.Initialize( true );  // 自動リセットしない
      m_criticalSectionForFileOpen.Initialize();
      m_criticalSectionForFileRead.Initialize();
      m_criticalSectionForFastSetup.Initialize();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     *
     * @param heap       作業ワークに使用するヒープ
     * @param filename   関連付けるアーカイブファイル
     * @param init_flag  コンストラクタで行う処理を InitBit の論理和で指定
     */
    //-------------------------------------------------------------------------
    ArcFile::ArcFile( gfl::heap::HeapBase* heap, const wchar_t* filename, u32 init_flag ) :
      m_workHeap( heap ),
      m_file(),
      m_filename( filename ),
      m_dataNum( 0 ),
      m_maxDataSize( 0 ),
      m_fastAccessTable( NULL ),
      m_setupThread( NULL ),
      m_fileOpenEvent(),
      m_fastSetupEvent(),
      m_criticalSectionForFileOpen(),
      m_criticalSectionForFileRead(),
      m_criticalSectionForFastSetup()
#if GFL_HIO_ENABLE
      ,m_isHioFile( false )
#endif
#if GFL_DEBUG
      ,m_majorVersion( 0 )
      ,m_minorVersion( 0 )
#endif
    {
#if GFL_HIO_ENABLE
      m_isHioFile = ( (init_flag & HIO) != 0 );
#endif
      this->InitExclusiveObjects();

      if( init_flag & OPEN ) {
        this->Open();
      }

      if( (init_flag & OPEN_FASTMODE) == OPEN_FASTMODE ) {
        this->SetupForFastMode( m_workHeap );
      }
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルを開く
     */
    //-------------------------------------------------------------------------
    void ArcFile::Open( void )
    {
      // 同じインスタンスについて、
      // 複数のスレッドから並列に Open() が呼ばれる可能性がある。
      // しかし、Open() は排他的に実行する必要があるため、
      // 1回目の実行時にクリティカルセクションをロックする。
      // なお、1回だけ実行すれば良いはずなので、以降、クリティカルセクションはアンロックしない。
      if( m_criticalSectionForFileOpen.TryEnter() )
      {
        if( this->OpenFile( m_filename ) )
        {
          m_fileOpenEvent.Signal();
        }
      }
    } 

    /**
     * @brief ファイルを開く
     * @param filename 開くファイルのパス
     */
    bool ArcFile::OpenFile( const wchar_t* filename )
    {
      bool result = m_file.Open( filename, OPEN_MODE_READ );
      if( !result ) {
        GFL_PRINT( "ファイルオープンに失敗しました。( %ls )\n", filename );
        return false;
      }

#if GFL_DEBUG


      // デバッグ版のみ, 
      // ファイルシグネチャとファイルバージョンの確認を行う.
      // なお, 確認にはファイルアクセスが必要となるため, 処理速度を考慮してデバッグ版でのみ行う.
      //
      // リリース版では,
      // アーカイブファイル以外のファイルがオープンされるコードは排除されるべき.
      // また, オープンされるアーカイブファイルは, すべて最新のバージョンであるべき.

      // ファイルシグネチャの確認
      {
        char signature[4];
        this->SeekFile( 0 );
        this->LoadDataPtr( 4, &signature );
        if( ( signature[0] != 'C' ) || 
            ( signature[1] != 'R' ) || 
            ( signature[2] != 'A' ) || 
            ( signature[3] != 'G' ) ) {
          GFL_PRINT( ".garc以外のファイルを読み込みました。\n" );
          GFL_ASSERT(0);  //@check
        }
      }

      // ファイルバージョンの確認
      {
        const u16 LATEST_MAJOR_VERSION = 4;
        const u16 SUPPORTED_MAJOR_VERSION[] = { 2, 3, 4 };  // ver.2.x, ver.3.x ver.4.x がサポート対象

        u16 version;
        this->SeekFile( 10 );
        this->LoadDataPtr( sizeof(u16), &version );
        m_majorVersion = ( version >> 8 );
        m_minorVersion = version & 0x00ff;

        bool supported = false;
        for( u32 i=0; i<GFL_NELEMS(SUPPORTED_MAJOR_VERSION); i++ ) {
          if( m_majorVersion == SUPPORTED_MAJOR_VERSION[i] ) {
            supported = true;
            break;
          }
        }

        if( !supported ) 
        {
          GFL_PRINT( "WARNING!! アーカイブファイル %ls は ver.%d.%d で、ライブラリのサポートは終了しています。ver.%d.x に再アーカイブしてください。\n",
              filename, m_majorVersion, m_minorVersion, LATEST_MAJOR_VERSION );
          GFL_ASSERT(0);  //@check
        }
        else if( m_majorVersion != LATEST_MAJOR_VERSION ) 
        {
          GFL_PRINT( "WARNING!! アーカイブファイル %ls は ver.%d.%d で、ライブラリのサポートは終了予定です。ver.%d.x に再アーカイブしてください。\n",
              filename, m_majorVersion, m_minorVersion, LATEST_MAJOR_VERSION );
        }
      }

#endif 

      return true;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルを開いているかどうかを調べる
     * @retval true
     * @retval false  
     */
    //-------------------------------------------------------------------------
    bool ArcFile::IsOpen( void ) const 
    { 
      return ( m_fileOpenEvent.IsSignaled() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルが開いた時点でシグナル状態になるイベントオブジェクトを取得する
     * @note 一度シグナル状態になったら、以降リセットはされません。
     * @attention 取得したイベントオブジェクトの状態を変更しないでください。
     */
    //-------------------------------------------------------------------------
    nn::os::LightEvent* ArcFile::GetOpenEvent( void )
    {
      return &m_fileOpenEvent;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ArcFile::~ArcFile( void )
    {
      GFL_ASSERT( !this->IsSettingUpForFastMode() ); //@check
      this->EndSetupForFastMode();

      GFL_ASSERT( !this->CheckAsyncRead() );//@check
      this->EndAsyncRead();

      GFL_SAFE_DELETE_ARRAY( m_fastAccessTable );
      m_file.Close();

      m_fileOpenEvent.Finalize();
      m_fastSetupEvent.Finalize();
      m_criticalSectionForFileOpen.Finalize();
      m_criticalSectionForFileRead.Finalize();
      m_criticalSectionForFastSetup.Finalize();
    }





    //-------------------------------------------------------------------------
    /**
     * @brief データ1件の読み込みポインタをシークする( 取り扱い注意！)
     * @param[in] data_id            アーカイブファイル内データID
     * @param[in] offset_from_data   シークサイズ( byte, データ先頭からのオフセット)
     */
    //-------------------------------------------------------------------------
    void ArcFile::SeekDataPtr( ARCDATID data_id, u32 offset_from_data )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      this->SeekFile( access_info.offsetFromArchiveTop + offset_from_data );
    } 

    /**
     * @brief ファイルシーク処理
     * @param offset_from_top  ファイル先頭からのオフセット
     */
    void ArcFile::SeekFile( size_t offset_from_top )
    {
      m_file.Seek( SEEK_BASE_BEGIN, offset_from_top );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief SeekDataPtr()でシークしたポインタからデータ読み込む( 取り扱い注意！)
     * @param[in]  size         読み込みサイズ
     * @param[out] dest_buffer  読み込んだデータの格納先( 4 byte alignment )
     * @retval 実際に読み込んだバイト数
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::LoadDataPtr( u32 size, void* dest_buffer )
    {
      size_t read_size = m_file.Read( dest_buffer, size );
      return read_size;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データを1件分読み込む
     * @param[in]  data_id      アーカイブファイル内データID
     * @param[out] dest_buffer  読み込んだデータの格納先( 4 byte alignment )
     * @retval 実際に読み込んだバイト数
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::LoadData( ARCDATID data_id, void* dest_buffer )
    { 
      size_t load_size = 0;
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      this->ExclusiveRead( access_info.offsetFromArchiveTop, access_info.dataSize, dest_buffer, &load_size );
      return load_size;
    }

    /**
     * @brief 排他的にファイルを読む
     * @param[in]  readPos     読み込む位置
     * @param[in]  readSize    読み込むサイズ
     * @param[out] pDestBuffer 読み込んだデータの格納先
     * @param[out] pReadSize   読み込んだサイズ( 不要ならNULL )
     */
    void ArcFile::ExclusiveRead( u32 readPos, u32 readSize, void* pDestBuffer, u32* pReadSize )
    {
      m_criticalSectionForFileRead.Enter();
      {
        this->SeekFile( readPos );
        size_t load_size = this->LoadDataPtr( readSize, pDestBuffer );
        if( pReadSize ) {
          *pReadSize = load_size;
        }
      }
      m_criticalSectionForFileRead.Leave();
    }

    //-------------------------------------------------------------------------
    /**
     * @brief	圧縮データを1件分展開して読み込む
     * @param[in]		  data_id	          アーカイブファイル内データID
     * @param[in]		  heap		          解凍作業用ヒープ
     * @param[in,out]	buffer	          解凍データの展開先バッファ
     * @param[in]	    buffer_size       解凍データの展開先バッファのサイズ
     * @param[out]	  uncompressed_size 展開データのサイズ
     */
    //-------------------------------------------------------------------------
    void ArcFile::LoadData( 
        ARCDATID data_id,
        gfl::heap::HeapBase* heap, 
        void* buffer,
        size_t buffer_size,
        size_t* uncompressed_size )
    {
      this->LoadCompressedData( data_id, heap, buffer, buffer_size, uncompressed_size );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief	圧縮データを1件分展開して読み込む
     * @param[in]		  data_id	          アーカイブファイル内データID
     * @param[in]		  heap		          解凍作業用ヒープ
     * @param[in,out]	buffer	          解凍データの展開先バッファ
     * @param[in]	    buffer_size       解凍データの展開先バッファのサイズ
     * @param[out]	  uncompressed_size 展開データのサイズ
     */
    //-------------------------------------------------------------------------
    void ArcFile::LoadCompressedData( 
        ARCDATID data_id,
        gfl::heap::HeapBase* heap, 
        void* buffer,
        size_t buffer_size,
        size_t* uncompressed_size )
    {
      // 圧縮データを読み込む
			size_t comp_size = this->GetDataSize( data_id ); 
			void * comp_data = GflHeapAllocMemoryLowAlign( heap, comp_size, 4 );
			s32 result = this->LoadData( data_id, comp_data );

      if( 0 < result ) {
        this->Uncompress( comp_data, comp_size, buffer, buffer_size, uncompressed_size ); // 指定されたバッファに展開する
      }
      else {
        GFL_ASSERT(0);  //@fix
      }

      GflHeapFreeMemory( comp_data );
    }

    /**
     * @brief 圧縮データを解凍する
     * @param[in]     compressed_data   圧縮データ
     * @param[in]     compressed_size   圧縮データのサイズ
     * @param[in,out]	buffer	          解凍データの展開先バッファ
     * @param[in]	    buffer_size       解凍データの展開先バッファ
     * @param[out]	  uncompressed_size 展開データのサイズ
     */
    void ArcFile::Uncompress( 
        void* compressed_data, 
        size_t compressed_size,
        void* buffer,
        size_t buffer_size,
        size_t* uncompressed_size )
    {
			size_t uncomp_size = nn::cx::GetUncompressedSize( compressed_data ); 

      // 元データサイズが0なら読み込まない
      if( compressed_data == 0 ) {
        return; 
      } 

      // バッファサイズが足りない
      if( buffer_size < uncomp_size ) {
        GFL_PRINT( "バッファサイズが足りません(buffer=0x%x, data=0x%x)\n", buffer_size, uncomp_size );
        GFL_ASSERT(0);  //@fix
        return; 
      }

			s32 result = gfl::cx::SecureUncompress( compressed_data, compressed_size, buffer, m_workHeap );
      GFL_ASSERT( result == 0 ); //@check

      if( uncompressed_size != NULL ) {
        *uncompressed_size = uncomp_size;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief	圧縮データを1件分展開して読み込む
     * @param[in]		datID			アーカイブファイル内データID
     * @param[in]		heap			ヒープ
     * @param[in]		align			展開データのアライン
     * @param[out]	pSize			展開データのサイズ
     * @return	展開したデータ
     */
    //-------------------------------------------------------------------------
    void * ArcFile::LoadData( 
        ARCDATID datID,
        gfl::heap::HeapBase * heap,
        int align,
        size_t * pSize )
    {
      return this->LoadCompressedDataAlloc( datID, heap, heap, align, pSize );
		}

    //-------------------------------------------------------------------------
    /**
     * @brief	圧縮データを1件分展開して読み込む
     * @param[in]		datID			  アーカイブファイル内データID
     * @param[in]		heapForWork	解凍作業に使用するヒープ
     * @param[in]		heapForData	解凍データを確保するヒープ
     * @param[in]		align				展開データのアライン
     * @param[out]	pSize				展開データのサイズ
     * @return	展開したデータ
     */
    //-------------------------------------------------------------------------
    void * ArcFile::LoadCompressedDataAlloc( 
        ARCDATID datID, 
        gfl::heap::HeapBase * heapForWork,
        gfl::heap::HeapBase * heapForData,
        int align,
        size_t * pSize )
    {
      // 圧縮データを読み込む
			size_t comp_size = this->GetDataSize( datID );
			void * comp_data = GflHeapAllocMemoryLowAlign( heapForWork, comp_size, 4 );
			s32 loaded_size = this->LoadData( datID, comp_data );

			void * uncomp_data = NULL;
      if( 0 < loaded_size )
      {
        // 解凍バッファを確保して展開する
        size_t uncomp_size = nn::cx::GetUncompressedSize( comp_data );
        uncomp_data = GflHeapAllocMemoryAlign( heapForData, uncomp_size, align );
        this->Uncompress( comp_data, comp_size, uncomp_data, uncomp_size, pSize );
      }
      else
      {
        GFL_ASSERT(0); //@fix
      }

			GflHeapFreeMemory( comp_data );
			return uncomp_data;
		}

    //-------------------------------------------------------------------------
    /**
     * @brief データ1件の一部を読み込む
     * @param[in]  data_id           アーカイブファイル内データID
     * @param[in]  offset_from_data  読み込み開始オフセット(byte, データ先頭からのオフセット)
     * @param[in]  read_size         読み込みサイズ(byte)
     * @param[out] dest_buffer       読み込んだデータの格納先( 4 byte alignment )
     */
    //-------------------------------------------------------------------------
    void ArcFile::LoadDataPiece(
        ARCDATID data_id, u32 offset_from_data, u32 read_size, void* dest_buffer )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      this->ExclusiveRead( access_info.offsetFromArchiveTop + offset_from_data, read_size, dest_buffer );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief データ1件分のサイズを取得する
     * @param data_id  アーカイブファイル内データID
     *
     * @note 
     * 通常モードの場合, 
     * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
     */
    //-------------------------------------------------------------------------
    u32 ArcFile::GetDataSize( ARCDATID data_id )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      return access_info.dataSize;
    } 

    /**
     * @brief アーカイブ内データへのアクセス情報を適切な場所から取得する
     * @param[in]  data_id      ファイル内データID
     * @param[out] access_info  取得したアクセス情報の格納先
     *
     * @note 通常モードの場合
     * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
     */
    void ArcFile::GetAccessInfo( ARCDATID data_id, AccessInfo* access_info )
    { 
      if( this->IsFastMode() )
      {
        this->GetAccessInfoFromTable( data_id, access_info );
      } 
      else 
      {
        size_t total_block_size = this->ReadTotalBlockSize();
        void* block_data = GFL_NEW_LOW_ARRAY( m_workHeap ) u8[ total_block_size ];
        this->GetAccessInfoFromFile_Fast( data_id, access_info, total_block_size, block_data );
        GFL_DELETE_ARRAY block_data;
      }
    } 

    /**
     * @brief アーカイブ内データへのアクセス情報をテーブルから取得する
     * @param[in]  data_id      ファイル内データID
     * @param[out] access_info  取得したアクセス情報の格納先
     */
    void ArcFile::GetAccessInfoFromTable( ARCDATID data_id, AccessInfo* access_info )
    {
      GFL_ASSERT( this->IsFastMode() );  //@check 高速モードでないとダメ
      GFL_ASSERT( m_fastAccessTable );  //@check 高速モードなら持っているはず
      GFL_ASSERT_STOP_MSG( data_id < m_dataNum, "ArcName[%ls] DataId[%d]\n",m_filename,data_id);  //@check 高速モードなら正しいファイル数を持っているはず
      *access_info = m_fastAccessTable[ data_id ];
    } 

    /**
     * @brief アーカイブ内データへのアクセス情報をファイルから取得する( 高速, 要作業ヒープ版 )
     * @param[in]     data_id           ファイル内データID
     * @param[out]    access_info       取得したアクセス情報の格納先
     * @param[in]     total_block_size  アーカイブファイルのブロックの合計サイズ
     * @param[in,out] temp_buffer       作業ワーク( total_block_size以上の大きさが必要 )
     */
    void ArcFile::GetAccessInfoFromFile_Fast( 
        ARCDATID data_id, AccessInfo* access_info, size_t total_block_size, void* temp_buffer )
    {
      this->ExclusiveRead( 0, total_block_size, temp_buffer );
      IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
      accessor->AttachData( temp_buffer );
      GFL_ASSERT_STOP( data_id < accessor->GetFileIdNum() ); //@check
      access_info->offsetFromArchiveTop = accessor->GetOffsetToData( data_id, LANGUAGE_CODE );
      access_info->dataSize = accessor->GetDataSize( data_id, LANGUAGE_CODE );
      access_info->realSize = accessor->GetRealDataSize( data_id, LANGUAGE_CODE );
      GFL_DELETE accessor;
    }

#if 0
    // アーカイブファイルに直接アクセスするためのオフセット情報
    //const size_t ArcFile::OFFSET_TO_TOTAL_BLOCK_SIZE = 16;
    //const size_t ArcFile::OFFSET_TO_FILE_ID_NUM = 32;
    //const size_t ArcFile::OFFSET_TO_ALLOCATION_OFFSET = 36;
    //const size_t ArcFile::OFFSET_TO_FILE_NUM_FROM_ALLOCATION_TABLE_BLOCK = 8;
    //const size_t ArcFile::ALLOCATION_OFFSET_ELEMENT_SIZE = 4;
    //const size_t ArcFile::ALLOCATION_TABLE_BLOCK_HEADER_SIZE = 12;
    //const size_t ArcFile::ALLOCATION_TABLE_HEADER_SIZE = 4;
    //const size_t ArcFile::ALLOCATION_TABLE_ELEMENT_SIZE = 8;
    //const size_t ArcFile::FILE_IMAGE_BLOCK_SIZE = 12;

    /**
     * @brief アーカイブ内データへのアクセス情報をファイルから取得する( 低速, 作業ヒープ不要版 )
     * @param[in]  data_id      ファイル内データID
     * @param[out] access_info  取得したアクセス情報の格納先
     *
     * @note 現在は呼ばれないが, アロケートせずにアクセス情報を取得する方法として残しておく.
     */
    void ArcFile::GetAccessInfoFromFile_Slow( ARCDATID data_id, AccessInfo* access_info )
    {
      const u16 FILE_ID_NUM = this->GetFileCountFromFile();
      GFL_ASSERT( data_id < FILE_ID_NUM );

      const size_t OFFSET_TO_ALLOCATION_OFFSET = m_accessor->GetOffsetToAllocationOffset();
      const size_t ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK = m_accessor->GetElementSizeOfAllocationOffsetBlock();
      const size_t OFFSET_TO_ALLOCATION_TABLE_BLOCK = OFFSET_TO_ALLOCATION_OFFSET + ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK * FILE_ID_NUM;
      const size_t OFFSET_TO_FILE_NUM = OFFSET_TO_ALLOCATION_TABLE_BLOCK + m_accessor->GetOffsetToFileNumFromAllocationTableBlock();

      size_t file_num = 0;
      this->SeekFile( OFFSET_TO_FILE_NUM );
      this->LoadDataPtr( sizeof(u32), &file_num ); 

      size_t offset_from_allocation_table = 0;
      this->SeekFile( OFFSET_TO_ALLOCATION_OFFSET + ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK * data_id );
      this->LoadDataPtr( ELEMENT_SIZE_OF_ALLOCATION_OFFSET_BLOCK, &offset_from_allocation_table );

      u32 language_bit = 0;
      const size_t OFFSET_TO_ALLOCATION_TABLE = OFFSET_TO_ALLOCATION_TABLE_BLOCK + m_accessor->GetHeaderSizeOfAllocationTableBlock();
      this->SeekFile( OFFSET_TO_ALLOCATION_TABLE + offset_from_allocation_table );
      this->LoadDataPtr( sizeof(u32), &language_bit );

      const size_t HEADER_SIZE_OF_ALLOCATION_TABLE_ELEMENT = m_accessor->GetHeaderSizeOfAllocationTableElement();
      const size_t ELEMENT_SIZE_OF_ALLOCATION_TABLE = m_accessor->GetElementSizeOfAllocationTable();
      size_t offset_to_target_element = HEADER_SIZE_OF_ALLOCATION_TABLE_ELEMENT;
      if( language_bit & (1<<LANGUAGE_CODE) ) {  // 指定された言語専用のファイルが存在
        for( u32 i=0; i<LANGUAGE_CODE; i++ ) {
          if( language_bit & (1<<i) ) {
            offset_to_target_element += ELEMENT_SIZE_OF_ALLOCATION_TABLE;
          }
        }
      } 

      u32 file_top = 0;
      u32 file_bottom = 0;
      u32 buffer[2];
      this->SeekFile( OFFSET_TO_ALLOCATION_TABLE + offset_from_allocation_table + offset_to_target_element );
      this->LoadDataPtr( sizeof(u32)*2, buffer );
      file_top = buffer[0];
      file_bottom = buffer[1];

      const size_t OFFSET_TO_FILE_IMAGE_FROM_ARCHIVE_TOP = 
        OFFSET_TO_ALLOCATION_TABLE + 
        HEADER_SIZE_OF_ALLOCATION_TABLE_ELEMENT * FILE_ID_NUM + 
        ELEMENT_SIZE_OF_ALLOCATION_TABLE * file_num + 
        m_accessor->GetSizeOfImageBlock();

      access_info->offsetFromArchiveTop = OFFSET_TO_FILE_IMAGE_FROM_ARCHIVE_TOP + file_top;
      access_info->dataSize = file_bottom - file_top;
    } 
#endif

    //-------------------------------------------------------------------------
    /**
     * @brief データ1件分の( パディングを含まない )サイズを取得する
     * @param data_id  アーカイブファイル内データID
     *
     * @note 通常モードの場合
     * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
     *
     * @note データを読み込むために必要なバッファサイズは, パディングを含んだサイズです。
     * データを読み込むためのデータサイズは GetDataSize() で取得してください。
     *
     * @attention
     * このメソッドは, ver.4.0 以降のアーカイブファイルでのみ有効となります。
     * ver.3.x までのアーカイブファイルではパディングを含むデータサイズが返ります。
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::GetRealDataSize( ARCDATID data_id )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info );
      return access_info.realSize;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アーカイブに含まれるデータの数を取得する
     *
     * @note 通常モードの場合, 
     * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
     */
    //-------------------------------------------------------------------------
    u16 ArcFile::GetDataCount( void )
    {
      if( this->IsFastMode() )
      {
        return m_dataNum;
      } 
      else 
      {
        return this->GetFileCountFromFile();
      }
    }

    /**
     * @brief 格納ファイル数をアーカイブファイルから読み込む
     */
    u16 ArcFile::GetFileCountFromFile( void )
    {
      IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
      const size_t OFFSET_TO_FILE_ID_NUM = accessor->GetOffsetToFileIDNum();
      GFL_DELETE accessor;

      u16 file_count = 0;
      this->ExclusiveRead( OFFSET_TO_FILE_ID_NUM, sizeof(u16), &file_count );
      return file_count;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief アーカイブに含まれるデータのうち, 最も大きなデータのサイズを取得する
     *
     * @note 
     * 通常モードの場合, 
     * ファイルを読む必要があるためスレッドの実行権が移る可能性があります.
     *
     * @attention
     * このメソッドは, ver.3.0 以降のアーカイブファイルでのみ有効となります。
     * ver.2.x までのアーカイブファイルでは必ず0が返ります。
     */
    //-------------------------------------------------------------------------
    size_t ArcFile::GetMaxDataSize( void )
    {
      if( this->IsFastMode() )
      {
        return m_maxDataSize;
      }
      else 
      {
        size_t total_block_size = this->ReadTotalBlockSize();
        void* temp_buffer = GFL_NEW_ARRAY( m_workHeap ) u8[ total_block_size ];
        this->ExclusiveRead( 0, total_block_size, temp_buffer );

        IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
        accessor->AttachData( temp_buffer );
        size_t max_size = accessor->GetMaxDataSize();

        GFL_DELETE accessor;
        GFL_DELETE_ARRAY temp_buffer;
        return max_size;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 非同期読み込みを開始する
     *
     * @param[in]  heap             スレッドの生成に使用するヒープ
     * @param[in]  data_id          アーカイブファイル内データID
     * @param[out] buffer           読み込んだデータの格納先( 4 byte alignment )
     * @param[in]  thread_priority  スレッドの優先度( 高 0 ～ 31 低 )
     * @param[in]  callback_func    読み込み終了時のコールバック関数( NULLなら呼ばれない )
     * @param[in]  callback_work    コールバック関数に渡す任意のワーク( 不要ならNULL )
     *
     * @return 非同期読み込みが開始できたかどうか
     * @retval true   開始できた
     * @retval false  開始できなかった
     *
     * @attention 
     * ArcFileインスタンス1つにつき, 同時に2つ以上の非同期読み込みを実行することは出来ません.
     * すでに実行中の非同期読み込みがある場合, この関数は失敗します( false が返ります ).
     *
     * @note
     * WaitAsyncRead()で読み込みの完了を待ってください.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::StartAsyncRead( 
        gfl::heap::HeapBase* heap,
        ARCDATID data_id, 
        void* buffer, 
        gfl::base::Thread::Priority thread_priority,
        File::CallBackFuncOnAsyncAccessFinished callback_func,
        void* callback_work )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info ); 

      return m_file.StartAsyncRead( heap, m_filename,
          access_info.offsetFromArchiveTop, 
          access_info.dataSize, access_info.dataSize, buffer,  // 一括で読む
          thread_priority, callback_func, callback_work );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 非同期読み込みを開始する( オフセット指定ver. )
     *
     * @param[in]  heap             スレッドの生成に使用するヒープ
     * @param[in]  data_id          アーカイブファイル内データID
     * @param[in]  read_offset      指定したデータの先頭からのオフセット
     * @param[in]  read_size        読み込むデータサイズ
     * @param[out] buffer           読み込んだデータの格納先( 4 byte alignment )
     * @param[in]  thread_priority  スレッドの優先度( 高 0 ～ 31 低 )
     * @param[in]  callback_func    読み込み終了時のコールバック関数( NULLなら呼ばれない )
     * @param[in]  callback_work    コールバック関数に渡す任意のワーク( 不要ならNULL )
     *
     * @return 非同期読み込みが開始できたかどうか
     * @retval true   開始できた
     * @retval false  開始できなかった
     *
     * @attention 
     * ArcFileインスタンス1つにつき, 同時に2つ以上の非同期読み込みを実行することは出来ません.
     * すでに実行中の非同期読み込みがある場合, この関数は失敗します( false が返ります ).
     *
     * @note
     * WaitAsyncRead()で読み込みの完了を待ってください.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::StartAsyncRead( 
        gfl::heap::HeapBase* heap, 
        ARCDATID data_id,
        u32 read_offset,
        u32 read_size,
        void* buffer, 
        gfl::base::Thread::Priority thread_priority,
        File::CallBackFuncOnAsyncAccessFinished callback_func,
        void* callback_work )
    {
      AccessInfo access_info;
      this->GetAccessInfo( data_id, &access_info ); 

      return m_file.StartAsyncRead( heap, m_filename, 
          access_info.offsetFromArchiveTop + read_offset, 
          read_size, read_size, buffer,  // 一括で読む
          thread_priority, callback_func, callback_work );
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 非同期読み込みの完了を待つ
     * @retval true   未完了
     * @retval false  完了
     *
     * @attention
     * false が返るまで繰り返し呼び, 移行の完了を待ってください.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::WaitAsyncRead( void )
    {
      if( this->CheckAsyncRead() == false ) 
      {
        this->EndAsyncRead();
        return false;
      }
      return true;
    } 

    /**
     * @brief 非同期読み込みが実行中かどうかを調べる
     * @retval true   実行中
     * @retval false  実行していない
     */
    bool ArcFile::CheckAsyncRead( void ) const
    {
      return m_file.CheckAsyncAccess();
    } 

    /**
     * @brief 非同期読み込みを完了する
     */
    void ArcFile::EndAsyncRead( void )
    {
      m_file.EndAsyncAccess();
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief 実行中の非同期読み込みをキャンセルする
     *
     * @note
     * WaitAsyncRead()で読み込みの完了を待ってください.
     */
    //-------------------------------------------------------------------------
    void ArcFile::CancelAsyncRead( void )
    {
      m_file.CancelAsyncAccess();
    } 



    const u32 ArcFile::STACK_SIZE_OF_SETUP_THREAD_FOR_FAST_MODE = 4 * 1024;

    //-------------------------------------------------------------------------
    /**
     * @brief 高速モードへの移行を開始する
     * @param heap_for_thread_work       スレッドの作業領域として使用するヒープ
     * @param heap_for_fastaccess_table  高速アクセス用データの確保に使用するヒープ
     *
     * @note WaitSetupForFastMode()で移行の完了を待ってください.
     */
    //-------------------------------------------------------------------------
    void ArcFile::StartSetupForFastMode( 
        gfl::heap::HeapBase* heap_for_thread_work,
        gfl::heap::HeapBase* heap_for_fastaccess_table )
    {
      if( !this->IsFastMode() && !this->IsSettingUpForFastMode() ) 
      {
        // 既存のスレッドを破棄
        GFL_SAFE_DELETE( m_setupThread );

        // スレッドを起動する
        // スレッドは処理完了後に自殺するので、
        // スレッド自身とスタック領域はヒープの後方から確保する。
        gfl::heap::HeapBase* thread_heap = heap_for_thread_work->GetLowerHandle();
        m_setupThread = GFL_NEW_LOW( thread_heap ) 
          SetupThread( this, heap_for_fastaccess_table, thread_heap, STACK_SIZE_OF_SETUP_THREAD_FOR_FAST_MODE );
        m_setupThread->Start( gfl::base::Thread::DefaultPriority );
      }
    }

    /**
     * @brief 高速モードへの移行中かどうかを調べる
     * @retval true   移行中
     * @retval false  移行中でない
     */
    bool ArcFile::IsSettingUpForFastMode( void ) const
    {
      return ( m_setupThread && m_setupThread->IsAlive() );
    }

    //-------------------------------------------------------------------------
    /**
     * @breif 高速モードへの移行完了を待つ
     * @retval true   完了した
     * @retval false  未完了
     *
     * @attention
     * true が返るまで繰り返し呼び, 移行の完了を待ってください.
     */
    //-------------------------------------------------------------------------
    bool ArcFile::WaitSetupForFastMode( void )
    {
      if( this->IsSetupForFastModeFinished() ) 
      {
        this->EndSetupForFastMode();
        return true;
      }
      return false;
    } 

    /**
     * @brief 高速モードへの移行処理が終了したかどうかを調べる
     * @retval true   完了した
     * @retval false  未完了
     */
    bool ArcFile::IsSetupForFastModeFinished( void ) const
    {
      // すでに高速モード
      if( this->IsFastMode() ) {
        return true;  
      }
      //万一StartSetupForFastMode()が呼ばれずにWaitが呼び出された時のフォロー
      if( m_setupThread == NULL ){
        GFL_ASSERT(0); //@check
        return true;
      }
      // 移行完了
      if( m_setupThread && !m_setupThread->IsAlive() ) {
        return true;  
      }

      return false;
    }

    /**
     * @brief 高速モードへの移行を完了する
     */
    void ArcFile::EndSetupForFastMode( void )
    {
      GFL_SAFE_DELETE( m_setupThread );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 高速モードかどうかを調べる
     * @retval true   高速モード
     * @retval false  通常モード
     */
    //-------------------------------------------------------------------------
    bool ArcFile::IsFastMode( void ) const
    {
      return ( m_fastSetupEvent.IsSignaled() );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 高速モードへの移行が完了した時点でシグナル状態になるイベントオブジェクトを取得する
     * @note 一度シグナル状態になったら、以降リセットはされません。
     * @attention 取得したイベントオブジェクトの状態を変更しないでください。
     */
    //-------------------------------------------------------------------------
    nn::os::LightEvent* ArcFile::GetFastSetupEvent( void )
    {
      return &m_fastSetupEvent;
    }

    //=========================================================================
    /**
     * @brief 高速モードへ移行する( 同期処理 )
     * @param heap  高速アクセスするためのデータ確保に使用するヒープ
     */
    //=========================================================================
    void ArcFile::SetupForFastMode( gfl::heap::HeapBase* heap )
    {
      // 同じインスタンスについて、
      // 複数のスレッドから並列に SetupForFastMode() が呼ばれる可能性がある。
      // しかし、SetupForFastMode() は排他的に実行する必要があるため、
      // 1回目の実行時にクリティカルセクションをロックする。
      // なお、1回だけ実行すれば良いはずなので、以降、クリティカルセクションはアンロックしない。
      if( m_criticalSectionForFastSetup.TryEnter() )
      { 
        gfl::heap::HeapBase* lower_heap = m_workHeap->GetLowerHandle();

        size_t total_block_size = this->ReadTotalBlockSize();
        void* block_data = GFL_NEW_ARRAY( lower_heap ) u8[ total_block_size ];
        this->ExclusiveRead( 0, total_block_size, block_data );

        IArcFileAccessor* accessor = this->CreateArcFileAccessor( lower_heap );
        accessor->AttachData( block_data );

        GFL_SAFE_DELETE_ARRAY( m_fastAccessTable );
        m_fastAccessTable = this->CreateDataAccessTable( heap, accessor );
        m_dataNum = accessor->GetFileIdNum();
        m_maxDataSize = accessor->GetMaxDataSize();

        GFL_DELETE accessor;
        GFL_DELETE_ARRAY block_data; 

        m_fastSetupEvent.Signal();
      }
    }

    /**
     * @brief アーカイブファイルへのアクセッサを生成する
     * @param heap アクセッサの確保に使用するヒープ
     * @return 生成したアクセッサ
     */
    IArcFileAccessor* ArcFile::CreateArcFileAccessor( gfl::heap::HeapBase* heap )
    {
      IArcFileAccessor* accessor = NULL;

#if GFL_DEBUG
      switch( m_majorVersion ) {
      case 2: accessor = GFL_NEW( heap ) ver2::ArcFileAccessor(); break; 
      case 3: accessor = GFL_NEW( heap ) ver3::ArcFileAccessor(); break; 
      case 4: accessor = GFL_NEW( heap ) ver4::ArcFileAccessor(); break; 
      } 
      if( accessor == NULL ) {
        GFL_PRINT( "m_majorVersion=%d\n", m_majorVersion );
        GFL_ASSERT_STOP(0); //@check
      }
#else
      accessor = GFL_NEW( heap ) ver4::ArcFileAccessor();
#endif

      return accessor;
    }

    /**
     * @brief データブロックの合計サイズを読み込む
     * @return 自身のデータブロックの合計サイズ
     */
    size_t ArcFile::ReadTotalBlockSize( void )
    {
      IArcFileAccessor* accessor = this->CreateArcFileAccessor( m_workHeap );
      const size_t OFFSET_TO_TOTAL_BLOCK_SIZE = accessor->GetOffsetToTotalBlockSize();
      GFL_DELETE accessor;

      size_t total_block_size = 0;
      this->ExclusiveRead( OFFSET_TO_TOTAL_BLOCK_SIZE, sizeof(u32), &total_block_size );
      return total_block_size;
    } 

    /**
     * @brief アーカイブ内データへのアクセステーブルを生成する
     * @param heap      テーブルの生成に使用するヒープ
     * @param accessor  アーカイブファイルのアクセッサ
     * @return 生成したアクセステーブル
     */
    ArcFile::AccessInfo* ArcFile::CreateDataAccessTable( gfl::heap::HeapBase* heap, const IArcFileAccessor* accessor ) const
    {
      u32 file_id_num = accessor->GetFileIdNum();
      AccessInfo* access_table = GFL_NEW_ARRAY( heap ) ArcFile::AccessInfo[ file_id_num ];

      for( u32 file_id=0; file_id<file_id_num; file_id++ ) {
        access_table[ file_id ].dataSize = accessor->GetDataSize( file_id, LANGUAGE_CODE );
        access_table[ file_id ].realSize = accessor->GetRealDataSize( file_id, LANGUAGE_CODE );
        access_table[ file_id ].offsetFromArchiveTop = accessor->GetOffsetToData( file_id, LANGUAGE_CODE );
      } 
      return access_table;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief HIO による読み込みか？
     * @retval true   HIO 読み込み
     * @retval false  HIO 読み込みでない
     */
    //-------------------------------------------------------------------------
    bool ArcFile::IsHioFile( void ) const
    {
#if GFL_HIO_ENABLE
      return m_isHioFile;
#else
      return false;
#endif
    }


  }  //namespace fs
}  //namespace gfl
