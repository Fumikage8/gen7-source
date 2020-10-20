//=======================================================================================
/**
 * @brief  SDカードファイルアクセス
 * @file   gfl_HioFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=======================================================================================
#include "gfl_fs.h"
#if GFL_HIO_ENABLE
#include <nn.h>
#include <fs/gfl_FileBase.h>
#include <fs/gfl_HioFile.h>


namespace gfl {
  namespace fs {


    //-----------------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-----------------------------------------------------------------------------------
    HioFile::HioFile( void ) : 
      File(),
      m_nn_host_file(),
      m_file_open_flag( false ),
      m_file_size( 0 ),
      m_current_position( 0 ),
      m_file_open_mode( OPEN_MODE_READ )
    {
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-----------------------------------------------------------------------------------
    HioFile::~HioFile( void )
    {
      this->Close();
    }



    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルを開く
     *
     * @param[in] path  ファイルパス (絶対パスで指定してください)
     * @param[in] mode  オープンモード
     *
     * @retval true   オープンに成功した場合
     * @retval false  オープンに失敗した場合
     *
     * @note パスの区切りには '/' を使用してください.
     */
    //-----------------------------------------------------------------------------------
    bool HioFile::Open( const wchar_t* path, OpenMode mode )
    {
      nn::hio::HostFile::AccessMode access_mode;
      nn::hio::HostFile::OpenDisp open_disp;

      switch( mode ) {
      case OPEN_MODE_READ:
        access_mode = nn::hio::HostFile::ACCESS_MODE_READ;
        open_disp = nn::hio::HostFile::OPEN_DISP_OPEN_EXISTING;
        break;
      case OPEN_MODE_WRITE_NEW:
        access_mode = nn::hio::HostFile::ACCESS_MODE_WRITE;
        open_disp = nn::hio::HostFile::OPEN_DISP_CREATE_ALWAYS;
        break;
      case OPEN_MODE_WRITE_ADD:
        access_mode = nn::hio::HostFile::ACCESS_MODE_WRITE;
        open_disp = nn::hio::HostFile::OPEN_DISP_OPEN_EXISTING;
        break;
      default:
        GFL_ASSERT(0);
        break;
      }

      this->Close();  // すでに開いているファイルを閉じる
      nn::Result result = m_nn_host_file.Open( path, access_mode, open_disp ); 

      if( result.IsSuccess() )
      {
        m_file_open_flag = true;
        m_file_open_mode = mode;
        m_file_size = m_nn_host_file.Seek( 0, nn::hio::HostFile::SEEK_TYPE_END );
        m_nn_host_file.Seek( 0, nn::hio::HostFile::SEEK_TYPE_SET );
        m_current_position = 0;
        return true;
      }
      else
      {
        return false;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルを閉じる
     */
    //-----------------------------------------------------------------------------------
    void HioFile::Close( void )
    {
      if( this->IsOpen() ) 
      {
        m_file_open_flag = false;
        m_file_size = 0;
        m_current_position = 0;
        m_nn_host_file.Close();
      } 
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
    size_t HioFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;
      }

      size_t read_size = 0;
      nn::Result result = m_nn_host_file.Read( &read_size, buffer, size );

      if( result.IsSuccess() )
      {
        m_current_position += read_size;
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
    size_t HioFile::Write( const void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;
      }

      size_t written_size = 0;
      nn::Result result = m_nn_host_file.Write( &written_size, buffer, size );

      if( result.IsSuccess() )
      {
        m_current_position += written_size;
        if( m_file_size < m_current_position ) {
          m_file_size = m_current_position;
        }
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
    void HioFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      if( !this->IsOpen() ) {
        return;
      }

      nn::hio::HostFile::SeekType seek_type;

      switch( base_pos ) {
      case SEEK_BASE_BEGIN:
        seek_type = nn::hio::HostFile::SEEK_TYPE_SET;
        break;
      case SEEK_BASE_CURRENT:
        seek_type = nn::hio::HostFile::SEEK_TYPE_CURRENT;
        break;
      case SEEK_BASE_END:
        seek_type = nn::hio::HostFile::SEEK_TYPE_END;
        break; 
      default:
        GFL_ASSERT(0);
        break;
      }

      m_current_position = m_nn_host_file.Seek( offset, seek_type );

      if( m_current_position < 0 ) {  // エラー発生
        m_current_position = 0;
      }
    }


    //-----------------------------------------------------------------------------------
    /**
     * @brief ファイルサイズを取得する
     */
    //-----------------------------------------------------------------------------------
    size_t HioFile::GetSize( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;
      }
      return m_file_size;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 現在位置を取得する
     *
     * @return ファイル先頭からのオフセット[Byte]
     */
    //-----------------------------------------------------------------------------------
    u32 HioFile::GetPosition( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;
      }
      return m_current_position;
    }

    //-----------------------------------------------------------------------------------
    /**
     * @brief 読み込みが出来るかどうかを調べる
     *
     * @retval true   読み込み可能
     * @retval false  読み込み不可能
     */
    //-----------------------------------------------------------------------------------
    bool HioFile::CanRead( void ) const
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
    bool HioFile::CanWrite( void ) const
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
    bool HioFile::SetPriority( s32 /*priority*/ )
    {
      if( !this->IsOpen() ) {
        return false;  // ファイルが開いていない
      }
      // HIOには優先順位がない

      return true;
    }


  } // namespace fs
} // namespace gfl
#endif // GFL_HIO_ENABLE
