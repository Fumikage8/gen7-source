//=============================================================================
/**
 * @brief  バックアップファイル
 * @file   gfl_BackupFile.h
 * @author obata_toshihiro
 * @date   2011.02.16
 */
//=============================================================================
#ifndef __GFL_BACKUPFILE_H__
#define __GFL_BACKUPFILE_H__
#pragma once


#include <gfl_Macros.h>
#include <fs/gfl_FileBase.h>


namespace gfl {
  namespace fs {


    class BackupFile : public File
    {
      public:

        /**
         * @brief ファイルアクセスの結果
         */
        enum Result {
          ACCESS_RESULT_OK,                     // 成功
          ACCESS_RESULT_NG_PATH_NOT_EXIST,      // 失敗：指定したパスが存在しない
          ACCESS_RESULT_NG_PATH_ALREADY_EXIST,  // 失敗：指定したパスが既に存在する
          ACCESS_RESULT_NG_BAD_FORMAT,          // 失敗：フォーマットが不正な状態になっている
          ACCESS_RESULT_NG_BROKEN_DATA,         // 失敗：データが壊れているか, 改竄されている
          ACCESS_RESULT_NG_ERROR,               // 失敗：接触不良など, ハードウェア要因
          ACCESS_RESULT_NG_LACK_OF_SPACE,       // 失敗：メディアの空き容量不足
          ACCESS_RESULT_OUT_OF_RESOURCE,        // 失敗：リソース不足
          ACCESS_RESULT_ACCESS_DENIED,          // 失敗：アクセス権がない
          ACCESS_RESULT_NG_UNKNOWN,             // 失敗：想定外のエラー
        };

        /**
         * @brief バックアップファイルのアクセス結果を取得する
         *
         * @param nn_result nn::fs::TryXXXX系APIの結果
         */
        static Result GetAccessResult( const nn::Result& nn_result );


        //-------------------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //-------------------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        BackupFile( void );

        /**
         * @brief デストラクタ
         */
        virtual ~BackupFile( void );


        //-------------------------------------------------------------------------------
        // ファイルアクセス
        //-------------------------------------------------------------------------------
        /**
         * @brief ファイルを開く
         *
         * @param[in] path  ファイルパス
         * @param[in] mode  オープンモード
         *
         * @retval true   オープンに成功した場合
         * @retval false  オープンに失敗した場合
         *
         * @note
         * パスの頭には "data:/" を付けてください.
         * パスの区切りには "/" を使用してください.
         */
        virtual bool Open( const wchar_t* path, OpenMode mode );

        /**
         * @brief ファイルを閉じる
         */
        virtual void Close( void );

        /**
         * @brief ファイルから読み込む
         *
         * @param[out] buffer  読み込んだデータの格納先( 4 byte alignment )
         * @param[in]  size    読み込むサイズ
         *
         * @return 実際に読み込んだサイズ
         */
        virtual size_t Read( void* buffer, u32 size );

        /**
         * @brief ファイルに書き込む
         *
         * @param[in] buffer  書き込むデータ( 4 byte alignment )
         * @param[in] size    書き込むサイズ
         *
         * @return 実際に書き込んだサイズ
         */
        virtual size_t Write( const void* buffer, u32 size, bool flush = true );

        /**
         * @brief ファイルをシークする
         *
         * @param[in] base_pos  シークの起点となる位置
         * @param[in] offset    起点位置からのオフセット
         *
         * @note ( base_pos + offset )の位置に移動します。
         */
        virtual void Seek( SeekBasePos base_pos, s32 offset );

        /**
         * @brief ファイルサイズを取得する
         */
        virtual size_t GetSize( void ) const;

        /**
         * @brief 現在位置を取得する
         *
         * @return ファイル先頭からのオフセット[Byte]
         */
        virtual u32 GetPosition( void ) const;

        /**
         * @brief ファイルが開いているかどうかを調べる
         *
         * @retval true   ファイルを開いている
         * @retval false  ファイルを開いていない
         */
        virtual bool IsOpen( void ) const { return m_file_open_flag; }

        /**
         * @brief 読み込みが出来るかどうかを調べる
         *
         * @retval true   読み込み可能
         * @retval false  読み込み不可能
         */
        virtual bool CanRead( void ) const;

        /**
         * @brief 書き込みが出来るかどうかを調べる
         *
         * @retval true   書き込み可能
         * @retval false  書き込み不可能
         */
        virtual bool CanWrite( void ) const;

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
        virtual bool SetPriority( s32 priority );

        /**
         * @brief 直前に行ったファイルアクセスの結果を取得する
         */
        inline Result GetLastAccessResult( void ) const { return m_access_result; }




      private:

        nn::fs::FileStream m_nn_file_stream;
        b32 m_file_open_flag;        // ファイルが開いているかどうか
        OpenMode m_file_open_mode;   // オープンモード
        Result m_access_result;      // ファイルアクセスの結果

        bit32 GetNnOpenMode( OpenMode mode ) const;
    };


  } // namespace fs
} // namespace gfl

#endif // __GFL_FILEBASE_H__
