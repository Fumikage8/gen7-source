#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @brief  バックアップファイルアクセス
 * @file   gfl_ExtSaveFile.cpp
 * @author obata_toshihiro
 * @author tamada
 * @date   2012.09.13
 *
 *  gfl_BackupFile.cppからコピペで作成
 */
//=============================================================================
#include <fs/include/internal/gfl2_FsFileBase.h>
#include <fs/include/CTR/gfl2_FsCtrExtSaveFile.h>
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>

namespace gfl2 {
  namespace fs {


    //-------------------------------------------------------------------------
    /**
     * @brief バックアップファイルのアクセス結果を取得する
     * @param nn_result nn::fs::TryXXXX系APIの結果
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::Result ExtSaveFile::GetAccessResult( const nn::Result& nn_result )
    {
      if( nn_result.IsSuccess() )
      {
        return ACCESS_RESULT_OK;
      }

      if(nn_result <= nn::fs::ResultNotFound())
      {
        if(nn_result <= nn::fs::ResultMediaNotFound())
        {
          return ACCESS_RESULT_NG_MEDIA_NOT_FOUND;
           // SD カードが挿さっていません。
           // (補足) 壊れた SD カードや SD カードではないメディアが挿されているときにも、このエラーが返ります。
           //      但し、その場合は RegisterSdmcInsertedEvent で登録した挿入イベントはシグナルされます。
        }
        else
        {
          return ACCESS_RESULT_NG_PATH_NOT_EXIST;
           // 指定したパスは存在していません。
           // (補足) ファイルオープン時にこのエラーが返ってきた場合は、SD 上のファイルが PC などで削除された可能性があります。
           // (注意) 拡張セーブデータは、他のアプリケーションからもアクセスされる可能性があるため、
           //      このエラーはハンドリングする必要があります。
        }
      }
      else if(nn_result <= nn::fs::ResultAlreadyExists())
      {
        return ACCESS_RESULT_NG_PATH_ALREADY_EXIST;
        // 指定したパスは、既に存在しています。
        // (補足) ディレクトリ削除時には、このエラーは返りません。
        //      ファイル作成時にこのエラーが返ってきた場合は、SD 上のファイルが PC などで削除された可能性があります。
        // (注意) 拡張セーブデータは、他のアプリケーションからもアクセスされる可能性があるため、
        //      このエラーはハンドリングする必要があります。
      }
      else if(nn_result <= nn::fs::ResultVerificationFailed())
      {
        return ACCESS_RESULT_NG_BROKEN_DATA;
        // このファイル (ディレクトリ) は削除して、作り直す必要があります。
        // (理由) 拡張セーブデータが壊れているか、改竄されています。
        // (補足) もし解決しない場合は、拡張セーブデータを作り直す必要があります。
        // (注意) 一度も書き込みを行っていない領域を読み込んだ場合、このエラーが返ります。
        //      nn::fs::TryCreateFile 関数や nn::fs::FileStream::TrySetSize 関数などを実行した後は、注意してください。
      }
      else if(nn_result <= nn::fs::ResultArchiveInvalidated())
      {
        return ACCESS_RESULT_NG_ARCHIVE_INVALIDATED;
        // アーカイブをマウントし直す必要があります。
        // (理由) SD カードが抜かれたことにより、アーカイブが無効になっています。
        // (補足) 開いているファイルを全て閉じ、nn::fs::Unmount 関数を呼び出してからマウントし直してください。
      }
      else if(nn_result <= nn::fs::ResultOperationDenied())
      {
        if(nn_result <= nn::fs::ResultWriteProtected())
        {
          return ACCESS_RESULT_NG_WRITE_PROTECTED;
           // SD カードがライトロックされています。
           // (補足) このエラーは、SD カードに書き込みを行うときに発生します。ファイルをオープンしただけでは発生しません。
        }
        else if(nn_result <= nn::fs::ResultMediaAccessError())
        {
          return ACCESS_RESULT_NG_MEDIA_ACCESS_ERROR;
           // 接触不良等によるハードウェア的な要因のときのみ、このエラーが返ります。
           // そのときは、SD カードの挿し直し、本体の再起動などで復帰する可能性があります。
        }
        else
        {
           return ACCESS_RESULT_NG_ERROR;
           // 上記以外の何らかの理由で、アクセスが拒否されました。
           // (例)  書込みできないファイルに書き込もうとした。
           //      他で開いているファイルを削除しようとした。
           //      アーカイブを跨いでファイルやディレクトリを移動させようとした。
           //      ルートディレクトリを削除しようとした。
           // (注意) 拡張セーブデータは、他のアプリケーションからも内容の改変が行えるため（運用によって変わります）、
           //      また、ユーザが PC 等で SD カード内のデータを操作できてしまうことから、このエラーは必ずハンドリングする必要があります。
        }
      }
      else if(nn_result <= nn::fs::ResultNotEnoughSpace())
      {
        return ACCESS_RESULT_NG_LACK_OF_SPACE;
        // アーカイブ、またはメディアの空き容量がありません。
        // (補足) ファイル・ディレクトリ作成時、ファイルサイズ変更時に発生する可能性があります。
        //      ファイル作成時にこのエラーが発生した場合は、ごみファイルが作成されていることがありますので、削除するようにしてください。
        //      ファイルサイズ変更時にこのエラーが発生した場合は、ファイルサイズが可能な範囲で伸長されていることがあります。
      }
      else
      {
        NN_DBG_PRINT_RESULT( nn_result );
      
        GFL_ASSERT( nn::fs::ResultOutOfResource::Includes( nn_result ) );         //@check プログラムミス検出用：製品版ではアプリで未定義エラー扱いすることが望ましい
        GFL_ASSERT( nn::fs::ResultAccessDenied::Includes( nn_result ) );          //@check プログラムミス検出用：製品版ではアプリで未定義エラー扱いすることが望ましい
        GFL_ASSERT( nn::fs::ResultInvalidArgument::Includes( nn_result ) );       //@check プログラムミス検出用：製品版ではアプリで未定義エラー扱いすることが望ましい
        GFL_ASSERT( nn::fs::ResultNotInitialized::Includes( nn_result ) );        //@check プログラムミス検出用：製品版ではアプリで未定義エラー扱いすることが望ましい
        GFL_ASSERT( nn::fs::ResultAlreadyInitialized::Includes( nn_result ) );    //@check プログラムミス検出用：製品版ではアプリで未定義エラー扱いすることが望ましい
        GFL_ASSERT( nn::fs::ResultUnsupportedOperation::Includes( nn_result ) );  //@check プログラムミス検出用：製品版ではアプリで未定義エラー扱いすることが望ましい
        // この拡張セーブデータ、もしくは SD カードにアクセスできませんでした。
        // ここに該当するエラーは、まとめてハンドリングを行ってください。(FATAL エラー表示画面には遷移させないでください。)
        //GFL_ASSERT(0); // 上記以外のエラーは発生しません。
        GFL_PRINT( "===========================\n" );
        GFL_PRINT( "gfl_ExtSaveFile Error[%d]\n", nn_result.GetDescription() );
        GFL_PRINT( "===========================\n" );
        return ACCESS_RESULT_NG_UNKNOWN;
      }
    }





    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::ExtSaveFile( void ) :
      m_nn_file_stream(),
      m_file_open_flag( false ),
      m_file_open_mode( OPEN_MODE_READ ),
      //m_nn_result(static_cast<nn::Result>(0)),
      m_access_result( ACCESS_RESULT_OK ),
      m_need_flush( false )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::~ExtSaveFile( void )
    {
      this->Close();
    }








    //-------------------------------------------------------------------------
    /**
     * @brief ファイルを開く
     *
     * @param[in] path  ファイルパス
     * @param[in] mode  オープンモード (OPEN_MODE_READ のみ有効です)
     *
     * @retval true   オープンに成功した場合
     * @retval false  オープンに失敗した場合
     *
     * @note
     * パスの頭には "exdt:/" を付けてください.
     * パスの区切りには "/" を使用してください.
     */
    //-------------------------------------------------------------------------
    bool ExtSaveFile::Open( const wchar_t* path, OpenMode mode )
    {
      // すでに開いているファイルを閉じる
      if( this->IsOpen() ) {
        this->Close();
      }

      // 既存のファイルを削除
      //if( mode == OPEN_MODE_WRITE_NEW ) {
      //  nn::fs::TryDeleteFile( path );
      //}

      bit32 nn_open_mode = this->GetNnOpenMode( mode );
      m_nn_result = m_nn_file_stream.TryInitialize( path, nn_open_mode );
      m_access_result = GetAccessResult( m_nn_result );

      if( m_access_result == ACCESS_RESULT_OK )
      {
        m_file_open_flag = true;
        m_file_open_mode = mode;
        return true;
      }
      else
      {
        //NN_DBG_PRINT_RESULT( m_nn_result );
        return false;  // 失敗
      }
    }

    /**
     * @brief CTRSDKのオープンモードを取得する
     */
    bit32 ExtSaveFile::GetNnOpenMode( OpenMode mode ) const
    {
      bit32 nn_open_mode = 0;

      switch( mode ) {
      case OPEN_MODE_READ:
        nn_open_mode = nn::fs::OPEN_MODE_READ;
        break;
      case OPEN_MODE_WRITE_ADD:
        nn_open_mode = nn::fs::OPEN_MODE_WRITE;
        break;
      case OPEN_MODE_WRITE_NEW:
        GFL_ASSERT( 0 );  // @check 拡張セーブデータではFileStreamからのファイル生成はできない
        //nn_open_mode = nn::fs::OPEN_MODE_WRITE | nn::fs::OPEN_MODE_CREATE;
        break;
      }

      return nn_open_mode;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief ファイルを閉じる
     */
    //-------------------------------------------------------------------------
    void ExtSaveFile::Close( void )
    {
      if( m_need_flush )
      {
        m_nn_result = m_nn_file_stream.TryFlush();
        m_access_result = GetAccessResult( m_nn_result );
        m_need_flush = false;
      }
      m_nn_file_stream.Finalize();
      m_file_open_flag = false;
    }







    //-------------------------------------------------------------------------
    /**
     * @brief ファイルから読み込む
     *
     * @param[out] buffer  読み込んだデータの格納先
     * @param[in]  size    読み込むサイズ
     *
     * @return 実際に読み込んだサイズ
     */
    //-------------------------------------------------------------------------
    size_t ExtSaveFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s32 read_size = 0;
      m_nn_result = m_nn_file_stream.TryRead( &read_size, buffer, size );
      m_access_result = GetAccessResult( m_nn_result );
      return ( m_access_result == ACCESS_RESULT_OK ) ? read_size : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルに書き込む
     *
     * @param[in] buffer  書き込むデータ
     * @param[in] size    書き込むサイズ
     *
     * @return 実際に書き込んだサイズ
     */
    //-------------------------------------------------------------------------
    size_t ExtSaveFile::Write( const void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      GFL_PRINT("ExtSaveFile::Write\n");
      s32 written_size = 0;
      m_nn_result =
        //m_nn_file_stream.TryWrite( &written_size, buffer, size, true );  // flushする
        m_nn_file_stream.TryWrite( &written_size, buffer, size, false );  // flushしない
      NN_DBG_PRINT_RESULT( m_nn_result );
      m_access_result = GetAccessResult( m_nn_result );
      if ( m_access_result == ACCESS_RESULT_OK )
      {
        m_need_flush = true;
      }
      return ( m_access_result == ACCESS_RESULT_OK ) ? written_size : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルキャッシュをデバイスに書き戻す
     */
    //-------------------------------------------------------------------------
    ExtSaveFile::Result ExtSaveFile::Flush( void )
    {
      m_nn_result = m_nn_file_stream.TryFlush();
      NN_DBG_PRINT_RESULT( m_nn_result );
      m_access_result = GetAccessResult( m_nn_result );
      return m_access_result;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief ファイルをシークする
     *
     * @param[in] base_pos  シークの起点となる位置
     * @param[in] offset    起点位置からのオフセット
     */
    //-------------------------------------------------------------------------
    void ExtSaveFile::Seek( SeekBasePos base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      nn::Result m_nn_result = m_nn_file_stream.TrySeek( offset, nn_base );
      m_access_result = GetAccessResult( m_nn_result );
    }


    //-------------------------------------------------------------------------
    /**
     * @brief ファイルサイズを取得する
     */
    //-------------------------------------------------------------------------
    size_t ExtSaveFile::GetSize( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s64 size = 0;
      nn::Result nn_result = m_nn_file_stream.TryGetSize( &size );
      return ( nn_result.IsSuccess() ) ? size : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 現在位置を取得する
     *
     * @return ファイル先頭からのオフセット[Byte]
     */
    //-------------------------------------------------------------------------
    u32 ExtSaveFile::GetPosition( void ) const
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s64 pos = 0;
      nn::Result nn_result = m_nn_file_stream.TryGetPosition( &pos );
      return ( nn_result.IsSuccess() ) ? pos : 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 読み込みが出来るかどうかを調べる
     *
     * @retval true   読み込み可能
     * @retval false  読み込み不可能
     */
    //-------------------------------------------------------------------------
    bool ExtSaveFile::CanRead( void ) const
    {
      if( !this->IsOpen() ) {
        return false;  // 開いていない
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return true;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return false;
      default:
        GFL_ASSERT(0);  // @check 未対応
        return false;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 書き込みが出来るかどうかを調べる
     *
     * @retval true   書き込み可能
     * @retval false  書き込み不可能
     */
    //-------------------------------------------------------------------------
    bool ExtSaveFile::CanWrite( void ) const
    {
      if( !this->IsOpen() ) {
        return false;  // 開いていない
      }

      switch( m_file_open_mode ) {
      case OPEN_MODE_READ:
        return false;
      case OPEN_MODE_WRITE_NEW:
      case OPEN_MODE_WRITE_ADD:
        return true;
      default:
        GFL_ASSERT(0);  // @check 未対応
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
    bool ExtSaveFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // ファイルが開いていない
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );

      return result.IsSuccess();
    }

    /**
     * @brief nn::fs::PositionBase に変換する
     */
    nn::fs::PositionBase ExtSaveFile::GetPositionBase( s32 base_pos ) const
    {
      switch( base_pos ) 
      {
      case SEEK_BASE_BEGIN: 
        return nn::fs::POSITION_BASE_BEGIN;

      case SEEK_BASE_CURRENT: 
        return nn::fs::POSITION_BASE_CURRENT;

      case SEEK_BASE_END: 
        return nn::fs::POSITION_BASE_END;

      default:
        GFL_ASSERT(0); //@check
        return nn::fs::POSITION_BASE_BEGIN;
      }
    }

  } // namespace fs
} // namespace gfl
#endif