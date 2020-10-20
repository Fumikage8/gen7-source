//========================================================================================
/**
 * @brief 1まとまりのレイアウト用テクスチャデータを管理するクラス
 * @file app_tool_LytTexReplaceSystem.cpp
 * @author obata_toshihiro
 * @date 2011.06.01
 */
//======================================================================================== 
#include <AppLib/include/Tool/app_tool_LytTexReplaceSystem.h>

#include <base/include/gfl2_Singleton.h>

#include <GameSys/include/GameManager.h>
#include <AppLib/include/Tool/app_tool_LytTexData.h>


namespace app {
  namespace tool {


    // テクスチャデータバッファのアラインメント
    const u32 LytTexReplaceSystem::TEX_BUFFER_ALIGNMENT = 128;


    //------------------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     * @param heap_for_management システムの作業領域に使用するヒープ
     * @param heap_for_tex_buffer テクスチャバッファの確保に使用するヒープ
     * @param max_tex_size        読み込むテクスチャの最大サイズ
     * @param tex_num             システムが保持可能なテクスチャの数
     */
    //------------------------------------------------------------------------------------
    LytTexReplaceSystem::LytTexReplaceSystem( 
        gfl2::heap::HeapBase* heap_for_management,
        gfl2::heap::HeapBase* heap_for_tex_buffer,
        u32 max_tex_size,
        u32 tex_num,
				gfl2::fs::AsyncFileManager* frman )
      : m_tex_data( NULL )
      , m_tex_data_num( tex_num )
      , m_asyncFileManager( frman )
    {
      if( frman == NULL )
      {
        m_asyncFileManager = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
      }
      m_tex_data = GFL_NEW_ARRAY( heap_for_management ) LytTexData*[ tex_num ];
      for( u32 i=0; i<tex_num; i++ ) {
        m_tex_data[i] = GFL_NEW( heap_for_management ) 
          LytTexData( heap_for_tex_buffer, max_tex_size, TEX_BUFFER_ALIGNMENT, frman );
      }
    } 

    //------------------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //------------------------------------------------------------------------------------
    LytTexReplaceSystem::~LytTexReplaceSystem()
    {
      for( u32 i=0; i<m_tex_data_num; i++ ) {
        GFL_SAFE_DELETE( m_tex_data[i] );
      }
      GFL_SAFE_DELETE_ARRAY( m_tex_data );
    }

    //----------------------------------------------------------------------------------
    /**
     * @brief ピクチャーペインのテクスチャを置き換える(static版)
     * @param tex_info        使用するテクスチャインフォ
     * @param target_pane     差し替え対象のペイン
     * @param material_index  差し替えるマテリアルのインデックス
     * @param texture_index   差し替えるテクスチャのインデックス
     *
     * @note 指定したテクスチャがロードされていない場合, 失敗する
     */
    //----------------------------------------------------------------------------------
    bool LytTexReplaceSystem::ReplaceTextureStatic( const gfl2::lyt::LytResTextureInfo* tex_info, gfl2::lyt::LytPicture* target_pane, u32 material_index, u32 texture_index )
    {
      if( tex_info == NULL )
      {
        GFL_ASSERT( 0 );
        return  false;
      }

      if( target_pane == NULL )
      {
        GFL_ASSERT( 0 );
        return  false;;
      }

      GFL_ASSERT( material_index < target_pane->GetMaterialNum() );  // material_index が不正
      gfl2::lyt::LytMaterial* material = target_pane->GetMaterial( material_index );
      GFL_ASSERT( texture_index < material->GetTexMapNum() );  // texture_index が不正

      material->SetTexMap( texture_index, tex_info );

      return  true;
    }

    //----------------------------------------------------------------------------------
    /**
      * @brief ピクチャーペインのテクスチャを置き換える(UV指定版)
      * @param tex_info        使用するテクスチャインフォ
      * @param target_pane     差し替え対象のペイン
      * @param uvPos           UV座標
      * @param uvRect          UV矩形
      * @param material_index  差し替えるマテリアルのインデックス
      * @param texture_index   差し替えるテクスチャのインデックス
      *
      * @note 指定したテクスチャがロードされていない場合, 失敗する
      */
    //----------------------------------------------------------------------------------
    bool LytTexReplaceSystem::ReplaceTextureStatic( const gfl2::lyt::LytResTextureInfo* tex_info, gfl2::lyt::LytPicture* target_pane, gfl2::math::Vector2& uvPos, gfl2::math::Vector2& uvRect, u32 material_index, u32 texture_index )
    {
      bool ret = ReplaceTextureStatic( tex_info, target_pane, material_index, texture_index );
      if(ret == false) return false;

      const nw::lyt::TexSize&  texSize  = tex_info->GetSize();

      /*
      nw::lyt::TexCoordQuad coord = 
      {
        gfl2::math::VEC2( 1.0f - uvPos.y / (float)texSize.width, uvPos.x / texSize.height ),
        gfl2::math::VEC2( 1.0f - uvPos.y / texSize.width, (uvPos.x + uvRect.x) / texSize.height ),
        gfl2::math::VEC2( 1.0f - (uvPos.y + uvRect.y) / texSize.width, uvPos.x / texSize.height ),
        gfl2::math::VEC2( 1.0f - (uvPos.y + uvRect.y) / texSize.width, (uvPos.x + uvRect.x) / texSize.height )
      };
      */

      nw::lyt::TexCoordQuad coord = 
      {
        //  左上
        gfl2::math::VEC2( ( uvPos.x <= 0 ? 0 : uvPos.x / (f32)texSize.width ), ( uvPos.y <= 0 ? 0 : uvPos.y / (f32)texSize.height ) ),
        //  右上
        gfl2::math::VEC2( ( uvPos.x + uvRect.x ) <= 0 ? 0 : ( uvPos.x + uvRect.x ) / (f32)texSize.width, ( uvPos.y <= 0 ? 0 : uvPos.y / (f32)texSize.height ) ),
        //  左下
        gfl2::math::VEC2( ( uvPos.x <= 0 ? 0 : uvPos.x / (f32)texSize.width ), ( ( uvPos.y + uvRect.y ) <= 0 ? 0 : (uvPos.y + uvRect.y) / (f32)texSize.height ) ),
        //  右下
        gfl2::math::VEC2( ( uvPos.x + uvRect.x ) <= 0 ? 0 : ( uvPos.x + uvRect.x ) / (f32)texSize.width, (uvPos.y + uvRect.y) <= 0 ? 0 : (uvPos.y + uvRect.y) / (f32)texSize.height )
      };

      target_pane->SetTexCoord( texture_index, coord );

      return  true;
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief ピクチャーペインのテクスチャを置き換える
     * @param tex_id          使用するテクスチャの管理番号
     * @param target_pane     差し替え対象のペイン
     * @param material_index  差し替えるマテリアルのインデックス(通常は0。複数存在するなら使用）
     * @param texture_index   差し替えるテクスチャのインデックス(通常は0。複数存在するなら使用）
     *
     * @note 指定したテクスチャがロードされていない場合, 失敗する
     */
    //------------------------------------------------------------------------------------
    bool LytTexReplaceSystem::ReplaceTexture( 
        u32 tex_id,
        gfl2::lyt::LytPicture* target_pane, u32 material_index, u32 texture_index )
    {
      if( this->IsTextureLoaded( tex_id ) ) {
        return ReplaceTextureStatic( &m_tex_data[ tex_id ]->GetTexInfo(), target_pane, material_index, texture_index );
      }

      return  false;
    }

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
    bool LytTexReplaceSystem::ReplaceTexture( 
        u32 tex_id,
        gfl2::lyt::LytPicture* target_pane, gfl2::math::Vector2& uvPos, gfl2::math::Vector2& uvRect, u32 material_index, u32 texture_index )
    {
      if( IsTextureLoaded( tex_id ) ) {
        return  ReplaceTextureStatic( &m_tex_data[ tex_id ]->GetTexInfo(), target_pane, uvPos, uvRect, material_index, texture_index );
      }

      return  false;
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief テクスチャがロード済みかどうかを調べる
     * @param tex_id  チェックするテクスチャの管理番号
     * @retval true   ロード済み
     * @retval false  ロードしていない
     */
    //------------------------------------------------------------------------------------
    bool LytTexReplaceSystem::IsTextureLoaded( u32 tex_id ) const
    {
			if( tex_id >= m_tex_data_num ){
				GFL_ASSERT( 0 );	// テクスチャの管理番号はm_tex_data_numより小さい必要があります
				return false;
			}
      return m_tex_data[ tex_id ]->IsLoaded();
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief テクスチャデータを読み込む
     * @param heap_for_work      作業用ワークに使用するヒープ
     * @param tex_id             読み込んだテクスチャの管理番号
     * @parma arc_id             読みこむテクスチャデータのアーカイブID
     * @parma data_id            読みこむテクスチャデータのアーカイブ内データID
     * @param compressed         true: 圧縮されている, false: 圧縮されていない
     */
    //------------------------------------------------------------------------------------
    void LytTexReplaceSystem::LoadTexture( 
        gfl2::heap::HeapBase* heap_for_work,
        u32 tex_id,
        gfl2::fs::ArcFile::ARCID arc_id,
        gfl2::fs::ArcFile::ARCDATID data_id,
        bool compressed )
    {
      // arcファイルを開く
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
        openReq.arcId       = arc_id;
        //  断片化対策
        openReq.heapForFile = heap_for_work->GetLowerHandle();
        openReq.heapForReq  = heap_for_work->GetLowerHandle();

        m_asyncFileManager->SyncArcFileOpen( openReq );
      }

      m_tex_data[ tex_id ]->LoadTexture( heap_for_work, arc_id, data_id, compressed );

      // arcファイルを閉じる
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = arc_id;
        m_asyncFileManager->SyncArcFileClose( closeReq );
      }

      GFL_PRINT("m_tex %d = %08x\n", tex_id, m_tex_data[tex_id]);  
    }

    //------------------------------------------------------------------------------------
    /**
     * @brief テクスチャデータを読み込む（ファイルオープン済み版）
     * @param heap_for_work      作業用ワークに使用するヒープ
     * @param tex_id             読み込んだテクスチャの管理番号
     * @parma arc_id             読みこむテクスチャデータのアーカイブID
     * @parma data_id            読みこむテクスチャデータのアーカイブ内データID
     * @param compressed         true: 圧縮されている, false: 圧縮されていない
     */
    //------------------------------------------------------------------------------------
    void LytTexReplaceSystem::LoadTextureCore( 
        gfl2::heap::HeapBase* heap_for_work,
        u32 tex_id,
        gfl2::fs::ArcFile::ARCID arc_id,
        gfl2::fs::ArcFile::ARCDATID data_id,
        bool compressed )
    {
      m_tex_data[tex_id]->LoadTexture( heap_for_work, arc_id, data_id, compressed ); 
    }

		//----------------------------------------------------------------------------------
		/**
		 * @brief		テクスチャデータ読み込みリクエスト ( FileReadManager使用 )
		 *
		 * @param		heap_for_work	読み込みの作業用ワーク領域に使用するヒープ
		 * @param		tex_id				テクスチャの管理番号
		 * @param		arcID					テクスチャデータを含むアーカイブデータ
		 * @param		datID					読み込むテクスチャデータを指定
		 * @param		compressed		true = 圧縮されている, false = 圧縮されていない
		 *
		 * @return	trueリクエスト成功  falseリクエスト失敗
		 */
		//----------------------------------------------------------------------------------
		bool LytTexReplaceSystem::LoadTextureRequest(
					gfl2::heap::HeapBase * heap_for_work,
					u32	tex_id,
					gfl2::fs::ArcFile::ARCID arcID,
					gfl2::fs::ArcFile::ARCDATID datID,
					bool compressed )
		{
      // arcファイルを開く
      {
        gfl2::fs::AsyncFileManager::ArcFileOpenReq openReq;
        openReq.arcId       = arcID;
        //  断片化対策
        openReq.heapForFile = heap_for_work->GetLowerHandle();
        openReq.heapForReq  = heap_for_work->GetLowerHandle();

        m_asyncFileManager->SyncArcFileOpen( openReq );
      }

      // ロードリクエスト開始
      bool bRes = m_tex_data[tex_id]->LoadTextureRequest( heap_for_work, arcID, datID, compressed );

      // arcファイルを閉じる
      {
        gfl2::fs::AsyncFileManager::ArcFileCloseReq closeReq;
        closeReq.arcId = arcID;
        m_asyncFileManager->SyncArcFileClose( closeReq );
      }

      return bRes;
		}

		bool LytTexReplaceSystem::LoadTextureRequestCore(
					gfl2::heap::HeapBase * heap_for_work,
					u32	tex_id,
					gfl2::fs::ArcFile::ARCID arcID,
					gfl2::fs::ArcFile::ARCDATID datID,
					bool compressed )
		{
      return m_tex_data[tex_id]->LoadTextureRequest( heap_for_work, arcID, datID, compressed );
		}


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
		bool LytTexReplaceSystem::LoadTexture( u32 tex_id )
		{
			return m_tex_data[tex_id]->LoadTexture();
		}

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
		bool LytTexReplaceSystem::CancelLoadTextureRequest( u32 tex_id )
		{
			return m_tex_data[tex_id]->CancelLoadTextureRequest();
		}

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
		bool LytTexReplaceSystem::IsLoadTextureFinished( u32 tex_id )
		{
			return m_tex_data[tex_id]->IsLoadTextureFinished();
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
		bool LytTexReplaceSystem::IsModuleFree(void)
		{
			for( u32 i=0; i<m_tex_data_num; i++ ){
				if( m_tex_data[i]->IsModuleFree() == false ){
					return false;
				}
			}
			return true;
		}

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
		void LytTexReplaceSystem::AllocTextureBuffer( u32 tex_id )
		{
			m_tex_data[tex_id]->AllocTextureBuffer();
		}

		//----------------------------------------------------------------------------------
		/**
		 * @brief		確保したテクスチャバッファを削除
		 *
		 * @param		tex_id				テクスチャの管理番号
		 *
		 * @return	none
		 */
		//----------------------------------------------------------------------------------
		void LytTexReplaceSystem::FreeTextureBuffer( u32 tex_id )
		{
			m_tex_data[tex_id]->FreeTextureBuffer();
		}

    //----------------------------------------------------------------------------------
    /**
     * @brief   指定のテクスチャバッファのポインタを返す
     *
     * @param   tex_id        テクスチャの管理番号
     *
     * @return  LytTexData    テクスチャバッファのポインタ
     */
    //----------------------------------------------------------------------------------
    LytTexData* LytTexReplaceSystem::GetLayoutTextureData( u32 tex_id )
    {
      GFL_ASSERT( tex_id < m_tex_data_num );    // @check

      return m_tex_data[tex_id];
    }


  } // namespace tool
} // namespace app
