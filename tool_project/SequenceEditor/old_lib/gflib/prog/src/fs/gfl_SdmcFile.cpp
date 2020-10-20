//=======================================================================================
/**
 * @brief  SDカードファイルアクセス
 * @file   gfl_SdmcFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#include "gfl_fs.h"
#if GFL_FILE_DEBUG
#include <nn.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_SdmcFile.h>


namespace gfl {
  namespace fs {


    //-----------------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-----------------------------------------------------------------------------------
    SdmcFile::SdmcFile( void ) : 
      File(),
      m_nn_file_stream(),
      m_file_open_flag( false ),
      m_file_open_mode( OPEN_MODE_READ )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-----------------------------------------------------------------------------------
    SdmcFile::~SdmcFile( void )
    {
      this->Close();
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルを開く
     *
     * @param[in] path  ファイルパス (sdmc:/ で始まるパスを指定してください)
     * @param[in] mode  オープンモード
     *
     * @retval true   オープンに成功した場合
     * @retval false  オープンに失敗した場合
     *
     * @note パスの区切りには '/' を使用してください.
     */
    //-----------------------------------------------------------------------------------
    bool SdmcFile::Open( const wchar_t* path, OpenMode mode )
    {
      if( this->IsOpen() ) { 
        this->Close();  // すでに開いているファイルを閉じる
      }

      // SDカードの抜き差しが行われた場合を想定し, マウントし直す
      // @todo 適切な場所で実行する > file system?
      nn::fs::Unmount( "sdmc" );
      nn::fs::MountSdmc();

      if( mode == OPEN_MODE_WRITE_NEW ) {
        nn::fs::TryDeleteFile( path );  // 既存のファイルを削除
      }

      bit32 nn_open_mode = 0;

      switch( mode ) {
      case OPEN_MODE_READ:
        nn_open_mode = nn::fs::OPEN_MODE_READ;
        break;
      case OPEN_MODE_WRITE_ADD:
      case OPEN_MODE_WRITE_NEW:
        nn_open_mode = nn::fs::OPEN_MODE_WRITE | nn::fs::OPEN_MODE_CREATE;
        break;
      }

      nn::Result result = m_nn_file_stream.TryInitialize( path, nn_open_mode );

      if( result.IsSuccess() )
      {
        m_file_open_flag = true;
        m_file_open_mode = mode;
        return true;
      }
      else
      {
        return false;  // 失敗
      }
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルを閉じる
     */
    //-----------------------------------------------------------------------------------
    void SdmcFile::Close( void )
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
    size_t SdmcFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s32 read_size = 0;
      nn::Result result = m_nn_file_stream.TryRead( &read_size, buffer, size );

      if( result.IsSuccess() )
      {
        return read_size;
      }
      else
      {
        return 0;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルに書き込む
     *
     * @param[in] buffer  書き込むデータ
     * @param[in] size    書き込むサイズ
     *
     * @return 実際に書き込んだサイズ
     */
    //-----------------------------------------------------------------------------------
    size_t SdmcFile::Write( const void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s32 written_size = 0;
      nn::Result result = m_nn_file_stream.TryWrite( &written_size, buffer, size, true );  // Flashする

      if( result.IsSuccess() )
      {
        return written_size;
      }
      else
      {
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
    void SdmcFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      m_nn_file_stream.TrySeek( offset, nn_base );
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルサイズを取得する
     */
    //-----------------------------------------------------------------------------------
    size_t SdmcFile::GetSize( void ) const
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
    u32 SdmcFile::GetPosition( void ) const
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
    bool SdmcFile::CanRead( void ) const
    {
      if( !this->IsOpen() ) 
      {
        return false;  // 開いていない
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return true;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return false;
      default:
        GFL_ASSERT(0);  // 未対応
        return false;
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
    bool SdmcFile::CanWrite( void ) const
    {
      if( !this->IsOpen() ) 
      {
        return false;  // 開いていない
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return false;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return true;
      default:
        GFL_ASSERT(0);  // 未対応
        return false;
      }
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
    bool SdmcFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // ファイルが開いていない
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );

      return result.IsSuccess();
    }


  } // namespace fs
} // namespace gfl

#endif // GFL_FILE_DEBUG
