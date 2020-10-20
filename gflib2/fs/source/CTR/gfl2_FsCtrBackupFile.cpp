#if   defined(GF_PLATFORM_CTR)
//=============================================================================
/**
 * @brief  バックアップファイルアクセス
 * @file   gfl2_FsCtrBackupFile.cpp
 * @author obata_toshihiro
 * @date   2011.02.03
 */
//=============================================================================
#include <nn.h>
//#include <fs/gfl2_FileBase.h>
#include <fs/include/CTR/gfl2_FsCtrBackupFile.h>
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
    FsCtrBackupFile::Result FsCtrBackupFile::GetAccessResult( const nn::Result& nn_result )
    {
      if( nn_result.IsSuccess() )
      {
        return ACCESS_RESULT_OK;
      }
      else
      {
        NN_DBG_PRINT_RESULT( nn_result );
        if( nn_result <= nn::fs::ResultNotFound() )
        {
          return ACCESS_RESULT_NG_PATH_NOT_EXIST;
          // 指定したパスは存在していません。
          // ( 補足 ) ファイル・ディレクトリ作成時には、
          //          アーカイブ名が間違っている以外に、このエラーは返りません。
          //          このような状況が発生しない設計であれば、ハンドリングの必要はありません。
        }
        else if( nn_result <= nn::fs::ResultAlreadyExists() )
        {
          return ACCESS_RESULT_NG_PATH_ALREADY_EXIST;
          // 指定したパスは、既に存在しています。
          // ( 補足 ) ファイル・ディレクトリ削除時には、このエラーは返りません。
          //        このような状況が発生しない設計であれば、ハンドリングの必要はありません。
        }
        else if( nn_result <= nn::fs::ResultBadFormat() )
        {
          return ACCESS_RESULT_NG_BAD_FORMAT;
          // デバッガでバックアップメモリ内のデータを破壊する以外の方法で、
          // ファイル・ディレクトリ操作中にこのエラーが返ることはありません。
          // 製品版でのハンドリングの必要はなくなりました。
          // このファイル (ディレクトリ) は削除して、作り直す必要があります。
          // ( 理由 ) ファイルフォーマットが不正な状態になっています。
          // ( 補足 ) もし解決しない場合は、セーブデータをフォーマットする必要があります。
        }
        else if( nn_result <= nn::fs::ResultVerificationFailed() )
        {
          return ACCESS_RESULT_NG_BROKEN_DATA;
          // このファイル (ディレクトリ) は削除して、作り直す必要があります。
          // ( 理由 ) セーブデータが壊れているか、改竄されています。
          // ( 補足 ) 二重化されている場合は、ユーザ操作( 書き込み中の電源断やカード抜かれ )で
          //          この状態になることはありません。
          //          二重化されていない場合は、ユーザ操作でもこのエラーは発生します。
          //          もし解決しない場合は、セーブデータをフォーマットする必要があります。
          // ( 注意 ) 一度も書き込みを行っていない領域を読み込んだ場合、このエラーが返ります。
          //          nn::fs::TryCreateFile 関数や
          //          nn::fs::FileStream::TrySetSize 関数などを実行した後は、注意してください。
        }
        else if( nn_result <= nn::fs::ResultOperationDenied() )
        {
          return ACCESS_RESULT_NG_ERROR;
          // 何らかの理由で、アクセスが拒否されました。
          // ( 例 ) 他で開いているファイルを削除しようとした。
          //        アーカイブを跨いでファイルやディレクトリを移動させようとした。
          //        ルートディレクトリを削除しようとした。
          // ( 補足 ) 上記のような状況が発生しない設計であれば、ハンドリングは必須ではありません。
          //          但し、接触不良等によるハードウェエア的な要因により失敗した場合、
          //          nn::fs::ResultMediaAccessError が返ります。
          //          そのときは、リトライ処理、ゲームカードの挿し直し、
          //          本体の再起動などで復帰する可能性があります。
        }
        else if( nn_result <= nn::fs::ResultNotEnoughSpace() )
        {
          return ACCESS_RESULT_NG_LACK_OF_SPACE;
          // アーカイブ、またはメディアの空き容量がありません。
          // ( 補足 ) ファイル・ディレクトリ作成時、ファイルサイズ変更時に発生する可能性があります。
          //          ファイル作成時にこのエラーが発生した場合は、
          //          ごみファイルが作成されていることがありますので、削除するようにしてください。
          //          ファイルサイズ変更時にこのエラーが発生した場合は、
          //          ファイルサイズが可能な範囲で伸長されていることがあります。
        }
        else if( nn_result <= nn::fs::ResultOutOfResource() )
        {
          //リソースが足りません。製品では発生しないようにするべきエラーです。
          return ACCESS_RESULT_OUT_OF_RESOURCE;
        }
        else if( nn_result <= nn::fs::ResultAccessDenied() )
        {
          // アクセス権がありません。製品では発生しないようにするべきエラーです。
          return ACCESS_RESULT_ACCESS_DENIED;
        }
        else
        {
          GFL_PRINT( "===========================\n" );
          GFL_PRINT( "gfl2_FsCtrBackupFile Error[0x%x]\n", nn_result.GetPrintableBits() );
          GFL_PRINT( "===========================\n" );
          return ACCESS_RESULT_NG_UNKNOWN;
        }
      }
    }






    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    FsCtrBackupFile::FsCtrBackupFile( void ) :
      //File(),
      m_nn_file_stream(),
      m_file_open_flag( false ),
      m_file_open_mode( OPEN_MODE_READ ),
      m_access_result( ACCESS_RESULT_OK )
    {
    }

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    FsCtrBackupFile::~FsCtrBackupFile( void )
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
     * パスの頭には "data:/" を付けてください.
     * パスの区切りには "/" を使用してください.
     */
    //-------------------------------------------------------------------------
    bool FsCtrBackupFile::Open( const wchar_t* path, OpenMode mode )
    {
      // すでに開いているファイルを閉じる
      if( this->IsOpen() ) {
        this->Close();
      }

      // 既存のファイルを削除
      if( mode == OPEN_MODE_WRITE_NEW ) {
        nn::fs::TryDeleteFile( path );
      }

      bit32 nn_open_mode = this->GetNnOpenMode( mode );
      nn::Result nn_result = m_nn_file_stream.TryInitialize( path, nn_open_mode );
      m_access_result = GetAccessResult( nn_result );

      if( m_access_result == ACCESS_RESULT_OK )
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

    /**
     * @brief CTRSDKのオープンモードを取得する
     */
    bit32 FsCtrBackupFile::GetNnOpenMode( OpenMode mode ) const
    {
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

      return nn_open_mode;
    }


    //-------------------------------------------------------------------------
    /**
     * @brief ファイルを閉じる
     */
    //-------------------------------------------------------------------------
    void FsCtrBackupFile::Close( void )
    {
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
    size_t FsCtrBackupFile::Read( void* buffer, u32 size )
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s32 read_size = 0;
      nn::Result nn_result = m_nn_file_stream.TryRead( &read_size, buffer, size );
      m_access_result = GetAccessResult( nn_result );
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
    size_t FsCtrBackupFile::Write( const void* buffer, u32 size, bool flush /* = true */ )
    {
      if( !this->IsOpen() ) {
        return 0;  // ファイルが開いていない
      }

      s32 written_size = 0;
      nn::Result nn_result =
        m_nn_file_stream.TryWrite( &written_size, buffer, size, flush );  // デフォルト値はflushあり
      m_access_result = GetAccessResult( nn_result );
      return ( m_access_result == ACCESS_RESULT_OK ) ? written_size : 0;
    }

    /**
     * @brief nn::fs::PositionBase に変換する
     */
    nn::fs::PositionBase FsCtrBackupFile::GetPositionBase( s32 base_pos ) const
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


    //-------------------------------------------------------------------------
    /**
     * @brief ファイルをシークする
     *
     * @param[in] base_pos  シークの起点となる位置
     * @param[in] offset    起点位置からのオフセット
     */
    //-------------------------------------------------------------------------
    void FsCtrBackupFile::Seek( u32 base_pos, s32 offset )
    {
      nn::fs::PositionBase nn_base = this->GetPositionBase( base_pos );
      nn::Result nn_result = m_nn_file_stream.TrySeek( offset, nn_base );
      m_access_result = GetAccessResult( nn_result );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief ファイルサイズを取得する
     */
    //-------------------------------------------------------------------------
    size_t FsCtrBackupFile::GetSize( void ) const
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
    u32 FsCtrBackupFile::GetPosition( void ) const
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
    bool FsCtrBackupFile::CanRead( void ) const
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
        GFL_ASSERT(0);  //@check 未対応
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
    bool FsCtrBackupFile::CanWrite( void ) const
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
        GFL_ASSERT(0);  //@check 未対応
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
    bool FsCtrBackupFile::SetPriority( s32 priority )
    {
      if( !this->IsOpen() ) {
        return false;  // ファイルが開いていない
      }

      nn::Result result = m_nn_file_stream.TrySetPriority( priority );

      return result.IsSuccess();
    }


  } // namespace fs
} // namespace gfl

#endif
