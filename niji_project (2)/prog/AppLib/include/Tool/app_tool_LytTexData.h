//========================================================================================
/**
 * @brief 1つのレイアウト用テクスチャデータを管理するクラス
 * @file app_tool_LytTexData.h
 * @author obata_toshihiro
 * @date 2011.06.01
 */
//========================================================================================
#ifndef __APP_TOOL_LYT_TEX_DATA_H__
#define __APP_TOOL_LYT_TEX_DATA_H__

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_fs.h>
#include <Layout/include/gfl2_Layout.h>

namespace app {
  namespace tool {


    class LytTexData
    {
      GFL_FORBID_COPY_AND_ASSIGN( LytTexData );  // コピーを禁止
    public:

      // memo マニュアルから探し出せなかったのでバイナリ解析から推測（危険だよ〜）
      struct BclimFooter
      { 
      /*  lyt1ではいけた
        nw::ut::BinaryFileHeader  clim;
        nw::ut::BinaryBlockHeader imag;
        u16                       width;
        u16                       height;
        u32                       format;
        u32                       size;
        */
      //  こちらはlyt2の構成
        nw::lyt::res::Lim       clim;
        nw::lyt::res::Image     imag;
        nw::lyt::res::ImageSize size;
      };

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
      static bool CreateTexDataFromTexInfo( gfl2::lyt::LytResTextureInfo* pOutTexInfo, void* pMemBuffer, const u32 memBufferSize, void* pSrcTextureBuffer, gfl2::lyt::LytResTextureInfo& rTexInfo );

      //----------------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap_for_buffer テクスチャデータ用バッファの確保に使用するヒープ
       * @param buffer_size     バッファのサイズ
       * @param buffer_align    バッファのアラインメント
       */
      //----------------------------------------------------------------------------------
      LytTexData( gfl2::heap::HeapBase* heap_for_buffer, u32 buffer_size, u32 buffer_align, gfl2::fs::AsyncFileManager* frman = NULL );

      //----------------------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //----------------------------------------------------------------------------------
      virtual ~LytTexData();

      //----------------------------------------------------------------------------------
      /**
       * @brief テクスチャデータを読み込む(ロードは同期読み)
       * @param heap_for_work     読み込みの作業用ワーク領域に使用するヒープ
       * @param arc_id            テクスチャデータをロードするarcID
       * @param data_id           読み込むテクスチャデータを指定
       * @param compressed        true: 圧縮されている, false: 圧縮されていない
       */
      //----------------------------------------------------------------------------------
      void LoadTexture(
          gfl2::heap::HeapBase* heap_for_work,
          gfl2::fs::ArcFile::ARCID arc_id,
          gfl2::fs::ArcFile::ARCDATID data_id,
          bool compressed = false );

      //----------------------------------------------------------------------------------
      /**
       * @brief テクスチャデータを読み込む(ロードは非同期)
       * @param heap_for_work     読み込みの作業用ワーク領域に使用するヒープ
       * @param arc_id            テクスチャデータをロードするarcID
       * @param data_id           読み込むテクスチャデータを指定
       * @param compressed        true: 圧縮されている, false: 圧縮されていない
       * @note
       *    ロード終了は IsLoadTextureFinished() 関数を使う
       *    非同期なので処理を実行したら IsLoadTextureFinished() 関数で true になるまでクラスの破棄はしない
       */
      //----------------------------------------------------------------------------------
      void LoadTextureAsync(
          gfl2::heap::HeapBase* heap_for_work,
          gfl2::fs::ArcFile::ARCID arc_id,
          gfl2::fs::ArcFile::ARCDATID data_id,
          bool compressed = false );

      //----------------------------------------------------------------------------------
      /**
       * @brief テクスチャデータを読み込んだかどうかを調べる
       * @retval true   読み込み済み
       * @retval false  読み込んでいない
       */
      //----------------------------------------------------------------------------------
      bool IsLoaded( void ) const;

      //----------------------------------------------------------------------------------
      /**
       * @brief テクスチャデータを読み込み中か
       * @retval true   読み込み中
       * @retval false  読み込み中でない
       */
      //----------------------------------------------------------------------------------
      bool IsLoadRun() const { return m_load_run_flag; }

      //----------------------------------------------------------------------------------
      /**
       * @brief テクスチャ情報を取得する
       */
      //----------------------------------------------------------------------------------
      const gfl2::lyt::LytResTextureInfo& GetTexInfo( void ) const;

      //----------------------------------------------------------------------------------
      /**
       * @brief		テクスチャデータを取得
       *
       * @param		none
       *
       * @return	テクスチャデータ
      */
      //----------------------------------------------------------------------------------
      void * GetTexData(void);

		//----------------------------------------------------------------------------------
		/**
		 * @brief		読み込んだテクスチャのサイズを取得
		 *
		 * @param		none
		 *
		 * @return	テクスチャサイズ
		 */
		//----------------------------------------------------------------------------------
		u32 GetTexSize(void) const;


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
		bool LoadTextureRequest(
					gfl2::heap::HeapBase * heap_for_work,
					gfl2::fs::ArcFile::ARCID arcID, gfl2::fs::ArcFile::ARCDATID datID, bool compressed );

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
		bool LoadTexture(void);

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
		bool CancelLoadTextureRequest(void);

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込み終了チェック ( FileReadManager使用 )
		 *
		 * @param		none
		 *
		 * @retval	"true = 読み込み終了"
		 * @retval	"false = それ以外"
		 */
		//----------------------------------------------------------------------------------
		bool IsLoadTextureFinished(void);

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
		bool IsModuleFree(void);

		//----------------------------------------------------------------------------------
		/**
		 * @brief		メモリ作成
		 *
		 * @param		none
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void AllocTextureBuffer(void);

		//----------------------------------------------------------------------------------
		/**
		 * @brief		メモリ解放
		 *
		 * @param		none
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void FreeTextureBuffer(void);

		//----------------------------------------------------------------------------------
    /**
     * @brief   読み込みフラグを落とす
     */
		//----------------------------------------------------------------------------------
    void Clear(void);


    //----------------------------------------------------------------------------------
    /**
     * @brief   読み込んだテクスチャのサイズを取得
     *
     * @return  テクスチャサイズ
     */
    //----------------------------------------------------------------------------------
    u32 GetTextureBufferSize(void) const;

    //----------------------------------------------------------------------------------
    /**
     * @brief   テクスチャデータをコピー
     *
     * @param   src   コピー元データ
     */
    //----------------------------------------------------------------------------------
    void CopyTextureBuffer( LytTexData * src );


  private:
		//----------------------------------------------------------------------------------
		/**
		 * @brief		tex_info削除
		 *
		 * @param		none
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void FreeTextInfo(void);

   private:
      bool m_load_flag;                 // 読み込んだかどうか
      gfl2::lyt::LytResTextureInfo m_tex_info;  // テクスチャ情報

			bool m_tex_info_load_flg;					// テクスチャ情報を作成したかどうか
      void* m_tex_buffer;               // テクスチャのバイナリデータ
      u32 m_tex_buffer_size;            // バッファサイズ
      u32 m_tex_buffer_align;           // バッファアライン
      gfl2::heap::HeapBase* m_heap;      // 使用するヒープベース

			u32 m_tex_read_size;							// 実際に読み込んだサイズ
			gfl2::fs::AsyncFileManager * m_asyncFileManager;
			u32	m_read_seq;

      bool  m_load_run_flag;
    };


  } // namespace tool
} // namespace app


#endif // __APP_TOOL_LYT_TEX_DATA_H__
