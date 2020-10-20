//=======================================================================================
/**
 * @brief  ROMファイルアクセス
 * @file   gfl_RomFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#include <nn.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_RomFile.h>



namespace gfl {
  namespace fs {


    //-----------------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-----------------------------------------------------------------------------------
    RomFile::RomFile( void ) : 
      File(),
      m_nn_file_stream(),
      m_file_open_flag( false )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-----------------------------------------------------------------------------------
    RomFile::~RomFile( void )
    {
      this->Close();
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルを開く
     *
     * @param[in] path  ファイルパス (rom:/ で始まるパスを指定してください)
     * @param[in] mode  オープンモード (OPEN_MODE_READ のみ有効です)
     *
     * @retval true   オープンに成功した場合
     * @retval false  オープンに失敗した場合
     *
     * @note パスの区切りには '/' を使用してください.
     */
    //-----------------------------------------------------------------------------------
    bool RomFile::Open( const wchar_t* path, OpenMode mode )
    {
      if( mode != OPEN_MODE_READ ) {  
        return false;  // このクラスは読み込み専用
      }

      if( this->IsOpen() ) { 
        this->Close();  // すでに開いているファイルを閉じる
      }


      nn::Result result = m_nn_file_stream.TryInitialize( path, nn::fs::OPEN_MODE_READ );


#if GFL_FILE_DEBUG
      // profilerのために、パスを保存。
      m_file_path = path;
#endif // GFL_FILE_DEBUG

      // メインスレッドの場合 プライオリティを設定
      if( base::Thread::IsMainThread( nn::os::Thread::GetCurrentId() ) ){
        this->SetPriority( FileSystem::GetMainThreadPriority() );
      }

      if( result.IsSuccess() )
      {
        m_file_open_flag = true;
        return true;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
        return false;  // 失敗
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルを閉じる
     */
    //-----------------------------------------------------------------------------------
    void RomFile::Close( void )
    {
      m_nn_file_stream.Finalize();
      m_file_open_flag = false;
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルから読み込む
     *
     * @param[out] buffer  読み込んだデータの格納先
     * @param[in]  size    読み込むサイズ
     *
     * @return 実際に読み込んだサイズ
     */
    //-----------------------------------------------------------------------------------
    size_t RomFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

#if GFL_FILE_DEBUG
      nn::os::Tick start = nn::os::Tick::GetSystemCurrent();
      s32 position = m_nn_file_stream.GetPosition();
#endif // GFL_FILE_DEBUG

      s32 read_size = 0;
      nn::Result result = m_nn_file_stream.TryRead( &read_size, buffer, size );

#if GFL_FILE_DEBUG
      if( FileSystem::IsProfiling() ){
        nn::os::Tick end = nn::os::Tick::GetSystemCurrent();
        nn::os::Tick ans = end - start;
        bit32 ID = nn::os::Thread::GetCurrentId();

        FileSystem::AddProfilerData( *m_file_path, ID, size, position, ans.ToTimeSpan().GetMicroSeconds() );
      }
#endif // GFL_FILE_DEBUG
      

      if( result.IsSuccess() )
      {
        return read_size;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
        return 0;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルをシークする
     *
     * @param[in] base_pos  シークの起点となる位置
     * @param[in] offset    起点位置からのオフセット
     */
    //-----------------------------------------------------------------------------------
    void RomFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      nn::Result result = m_nn_file_stream.TrySeek( offset, nn_base );
      if( !result.IsSuccess() )
      {
        NN_ERR_THROW_FATAL_ALL(result);
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルサイズを取得する
     */
    //-----------------------------------------------------------------------------------
    size_t RomFile::GetSize( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s64 size = 0;
      nn::Result result = m_nn_file_stream.TryGetSize( &size );

      if( result.IsSuccess() )
      {
        return size;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
        return 0;
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 現在位置を取得する
     *
     * @return ファイル先頭からのオフセット[Byte]
     */
    //-----------------------------------------------------------------------------------
    u32 RomFile::GetPosition( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s64 pos = 0;
      nn::Result result = m_nn_file_stream.TryGetPosition( &pos );

      if( result.IsSuccess() )
      {
        return pos;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
        return 0;
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 読み込みが出来るかどうかを調べる
     *
     * @retval true   読み込み可能
     * @retval false  読み込み不可能
     */
    //-----------------------------------------------------------------------------------
    bool RomFile::CanRead( void ) const
    {
      if( !this->IsOpen() ) 
      {
        return false;  // 開いていない
      }
      else
      {
        return true;  // 開いているなら, 読み込みのみ可能
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 書き込みが出来るかどうかを調べる
     *
     * @retval true   書き込み可能
     * @retval false  書き込み不可能
     */
    //-----------------------------------------------------------------------------------
    bool RomFile::CanWrite( void ) const
    {
      // 書き込みは出来ない
      return false;
    }

    //----------------------------------------------------------------------------
    /**
     *	@brief  ファイル読み込み優先順位を設定する
     *
     *	@param	priority    優先順位
     *   PRIORITY_APP_REALTIME = nn::fs::PRIORITY_APP_REALTIME,  /// 利用注意！！！　ドキュメントを良く読むこと
     *   PRIORITY_APP_NORMAL   = nn::fs::PRIORITY_APP_NORMAL,    /// 普通
     *   PRIORITY_APP_LOW      = nn::fs::PRIORITY_APP_LOW,       /// 低い
     */
    //-----------------------------------------------------------------------------
    bool RomFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // ファイルが開いていない
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );
      if(result.IsSuccess())
      {
        return true;
      }
      else
      {
        NN_ERR_THROW_FATAL_ALL(result);
        return false;
      }
    }


  } // namespace fs
} // namespace gfl
