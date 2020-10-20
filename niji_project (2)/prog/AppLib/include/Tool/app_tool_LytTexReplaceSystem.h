//========================================================================================
/**
 * @brief 1まとまりのレイアウト用テクスチャデータを管理するクラス
 * @file app_tool_LytTexReplaceSystem.h
 * @author obata_toshihiro
 * @date 2011.06.01
 */
//========================================================================================
#ifndef __APP_TOOL_LYTTEX_REPLACE_SYSTEM_H__
#define __APP_TOOL_LYTTEX_REPLACE_SYSTEM_H__

#include <heap/include/gfl2_Heap.h>
#include <Layout/include/gfl2_Layout.h>

namespace app {
  namespace tool {

    class LytTexData;

    class LytTexReplaceSystem
    {
      GFL_FORBID_COPY_AND_ASSIGN( LytTexReplaceSystem );  // コピーを禁止

    public: 
      // レイアウトテクスチャのフッターサイズ
      /*
          @note:  2015/03/18 by nakahiro
            kujiraのときに任天堂様から頂いたrubyに記述してあったサイズ。
            *.bflimのサイズを調べたが、同じなのでそのまま使えるはず。

            例）ポケモンアイコン
                  format: RGB5A1
              image size: width x height x 16bit = 64 x 32 x 2 = 4096byte
               file size: 4136byte
      */
#if defined(GF_PLATFORM_CTR)
      static const u32 BCLIM_FOOTER_SIZE = 40;
#elif defined(GF_PLATFORM_WIN32)
      static const u32 BCLIM_FOOTER_SIZE = 40 * 3;  // @note PC版のサイズは適当です。
#endif

      //----------------------------------------------------------------------------------
      /**
       * @brief コンストラクタ
       * @param heap_for_management システムの作業領域に使用するヒープ
       * @param heap_for_tex_buffer テクスチャバッファの確保に使用するヒープ
       * @param max_tex_size        読み込むテクスチャの最大サイズ
       * @param tex_num             システムが保持可能なテクスチャの数
       */
      //----------------------------------------------------------------------------------
      LytTexReplaceSystem( 
          gfl2::heap::HeapBase* heap_for_management,
          gfl2::heap::HeapBase* heap_for_tex_buffer,
          u32 max_tex_size,
          u32 tex_num,
					gfl2::fs::AsyncFileManager* frman = NULL );

      //----------------------------------------------------------------------------------
      /**
       * @brief デストラクタ
       */
      //----------------------------------------------------------------------------------
      virtual ~LytTexReplaceSystem();


      //----------------------------------------------------------------------------------
      /**
       * @brief ピクチャーペインのテクスチャを置き換える(static版)
       * @param tex_info        使用するテクスチャインフォ
       * @param target_pane     差し替え対象のペイン
       * @param material_index  差し替えるマテリアルのインデックス
       * @param texture_index   差し替えるテクスチャのインデックス
       */
      //----------------------------------------------------------------------------------
      static bool ReplaceTextureStatic( const gfl2::lyt::LytResTextureInfo* tex_info, gfl2::lyt::LytPicture* target_pane, u32 material_index, u32 texture_index );

      //----------------------------------------------------------------------------------
      /**
       * @brief ピクチャーペインのテクスチャを置き換える(UV指定版 + static版)
       * @param tex_info        使用するテクスチャインフォ
       * @param target_pane     差し替え対象のペイン
       * @param uvPos           UV座標
       * @param uvRect          UV矩形
       * @param material_index  差し替えるマテリアルのインデックス
       * @param texture_index   差し替えるテクスチャのインデックス
       *
       * @note  UV指定して張り付ける処理は共通で処理が独立できるので別けて、どこからでも使用できるようにしました。
       */
      //----------------------------------------------------------------------------------
      static bool ReplaceTextureStatic( const gfl2::lyt::LytResTextureInfo* tex_info, gfl2::lyt::LytPicture* target_pane, gfl2::math::Vector2& uvPos, gfl2::math::Vector2& uvRect, u32 material_index, u32 texture_index );

      //----------------------------------------------------------------------------------
      /**
       * @brief ピクチャーペインのテクスチャを置き換える
       * @param tex_id          使用するテクスチャの管理番号
       * @param target_pane     差し替え対象のペイン
       * @param material_index  差し替えるマテリアルのインデックス
       * @param texture_index   差し替えるテクスチャのインデックス
       *
       * @note 指定したテクスチャがロードされていない場合, 失敗する
       */
      //----------------------------------------------------------------------------------
      bool ReplaceTexture( 
          u32 tex_id,
          gfl2::lyt::LytPicture* target_pane, u32 material_index, u32 texture_index ); 

      //----------------------------------------------------------------------------------
      /**
       * @brief ピクチャーペインのテクスチャを置き換える(UV指定版)
       * @param tex_id          使用するテクスチャの管理番号
       * @param target_pane     差し替え対象のペイン
       * @param uvPos           UV座標
       * @param uvRect          UV矩形
       * @param material_index  差し替えるマテリアルのインデックス
       * @param texture_index   差し替えるテクスチャのインデックス
       *
       * @note 指定したテクスチャがロードされていない場合, 失敗する
       */
      //----------------------------------------------------------------------------------
      bool ReplaceTexture( 
          u32 tex_id, gfl2::lyt::LytPicture* target_pane, gfl2::math::Vector2& uvPos, gfl2::math::Vector2& uvRect, u32 material_index, u32 texture_index ); 

      //----------------------------------------------------------------------------------
      /**
       * @brief テクスチャデータを読み込む
       * @param heap_for_work 作業用ワークに使用するヒープ
       * @param pAsyncFileManager ファイル読み込み制御クラス
       * @param tex_id        読み込んだテクスチャの管理番号
       * @parma arc_id        読みこむテクスチャデータのアーカイブID
       * @parma data_id       読みこむテクスチャデータのアーカイブ内データID
       * @param compressed    true: 圧縮されている, false: 圧縮されていない
       * @note
       *                      ファイルオープン、ロード、クローズは同期読み
       */
      //----------------------------------------------------------------------------------
      void LoadTexture( 
          gfl2::heap::HeapBase* heap_for_work,
          u32 tex_id,
          gfl2::fs::ArcFile::ARCID arc_id,
          gfl2::fs::ArcFile::ARCDATID data_id, 
          bool compressed = false );

      //------------------------------------------------------------------------------------
      /**
       * @brief テクスチャデータを読み込む（ファイルオープン済み版）
       * @param heap_for_work      作業用ワークに使用するヒープ
       * @param pAsyncFileManager  ファイル読み込み制御クラス
       * @param tex_id             読み込んだテクスチャの管理番号
       * @parma arc_id             読みこむテクスチャデータのアーカイブID
       * @parma data_id            読みこむテクスチャデータのアーカイブ内データID
       * @param compressed         true: 圧縮されている, false: 圧縮されていない
       * @note
       *                           ロードは同期読み
       */
      //------------------------------------------------------------------------------------
      void LoadTextureCore(
          gfl2::heap::HeapBase* heap_for_work,
          u32 tex_id,
          gfl2::fs::ArcFile::ARCID arc_id,
          gfl2::fs::ArcFile::ARCDATID data_id,
          bool compressed = false );

      //----------------------------------------------------------------------------------
      /**
       * @brief テクスチャがロード済みかどうかを調べる
       * @param tex_id  チェックするテクスチャの管理番号
       * @retval true   ロード済み
       * @retval false  ロードしていない
       */
      //----------------------------------------------------------------------------------
      bool IsTextureLoaded( u32 tex_id ) const;

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込みリクエスト ( FileReadManager使用 )
		 *
		 * @param		heap_for_work	読み込みの作業用ワーク領域に使用するヒープ
         * @param       pAsyncFileManager ファイル読み込み制御クラス
		 * @param		tex_id				テクスチャの管理番号
		 * @param		arcID					テクスチャデータを含むアーカイブデータ
		 * @param		datID					読み込むテクスチャデータを指定
		 * @param		compressed		true = 圧縮されている, false = 圧縮されていない
		 *
		 * @return	trueリクエスト成功  falseリクエスト失敗
     * @note
     *          ファイルオープン、クローズは同期読み,ロードは非同期
		 */
		//----------------------------------------------------------------------------------
		bool LoadTextureRequest(
          gfl2::heap::HeapBase * heap_for_work,
          u32	tex_id,
          gfl2::fs::ArcFile::ARCID arcID,
          gfl2::fs::ArcFile::ARCDATID datID,
          bool compressed );

    //  ファイルロードは非同期読み
		bool LoadTextureRequestCore(
          gfl2::heap::HeapBase * heap_for_work,
          u32	tex_id,
          gfl2::fs::ArcFile::ARCID arcID,
          gfl2::fs::ArcFile::ARCDATID datID,
          bool compressed );

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込み ( FileReadManager使用 )
		 *
		 * @param		tex_id		テクスチャの管理番号
		 *
		 * @retval	"true = 読み込み終了"
		 * @retval	"false = それ以外"
		 */
		//----------------------------------------------------------------------------------
		bool LoadTexture( u32 tex_id );

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込みリクエストをキャンセル ( FileReadManager使用 )
		 *
		 * @param		tex_id				テクスチャの管理番号
		 *
		 * @retval	"true = キャンセル成功"
		 * @retval	"false = キャンセル失敗"
		 */
		//----------------------------------------------------------------------------------
		bool CancelLoadTextureRequest( u32 tex_id );

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込み終了チェック ( FileReadManager使用 )
		 *
		 * @param		tex_id				テクスチャの管理番号
		 *
		 * @retval	"true = 読み込み終了"
		 * @retval	"false = それ以外"
		 */
		//----------------------------------------------------------------------------------
		bool IsLoadTextureFinished( u32 tex_id );

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
		 * @brief		テクスチャバッファを確保
		 *
		 * @param		tex_id				テクスチャの管理番号
		 *
		 * @return	none
		 *
		 * @li	あらかじめ確保しておく場合に使用
		 */
		//----------------------------------------------------------------------------------
		void AllocTextureBuffer( u32 tex_id );

		//----------------------------------------------------------------------------------
		/**
		 * @brief		確保したテクスチャバッファを削除
		 *
		 * @param		tex_id				テクスチャの管理番号
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void FreeTextureBuffer( u32 tex_id );

    //----------------------------------------------------------------------------------
    /**
     * @brief   指定のテクスチャバッファのポインタを取得
     *
     * @param   tex_id        テクスチャの管理番号
     *
     * @return  LytTexData    テクスチャバッファのポインタ
     */
    //----------------------------------------------------------------------------------
    LytTexData* GetLayoutTextureData( u32 tex_id );

    private:
      static const u32 TEX_BUFFER_ALIGNMENT;

      LytTexData** m_tex_data;
      u32 m_tex_data_num;

			gfl2::fs::AsyncFileManager * m_asyncFileManager;
    };


  } // namespace tool
} // namespace app


#endif // __APP_TOOL_LYTTEX_REPLACE_SYSTEM_H__
