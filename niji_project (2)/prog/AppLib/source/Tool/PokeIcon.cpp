//=============================================================================
/**
 * @file    PokeIcon.cpp
 * @brief   ポケモンアイコン関連
 * @author  Hiroyuki Nakamura
 * @date    12.01.17
 */
//=============================================================================

// module
#include "AppLib/include/Tool/PokeIcon.h"
// gflib2
#include <base/include/gfl2_Singleton.h>
// pmlib
#include <pml/include/pokepara/pml_PokemonCoreParam.h>
// niji
#include "GameSys/include/GameManager.h"
#include "AppLib/include/Util/app_util_FileAccessor.h"
#include "AppLib/include/Tool/app_tool_LytTexData.h"
#include "PokeTool/include/PokeTool.h"
// resource
#include "arc_def_index/arc_def.h"
#include "arc_index/PokeIcon.gaix"
#include "niji_conv_header/app/common/texture/poke_icon/pokeicon_table.cdat"


GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

const bool PokeIcon::IS_COMP = true;   // 圧縮されているか


//-----------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param		heap		  ヒープ
 * @param		devHeap	  デバイスヒープ
 * @param		max			  テクスチャ登録最大数
 * @param		is_dummy  ？アイコン読み込み
 */
//-----------------------------------------------------------------------------
PokeIcon::PokeIcon( gfl2::heap::HeapBase * heap, gfl2::heap::HeapBase * devHeap, u32 max, bool is_dummy )
  : m_pTexSys( NULL )
  , m_pWorkHeap( heap )
  , m_pBuffHeap( devHeap )
  , m_isFileOpen( false )
  , m_dummyTexID( 0 )
{
  if( is_dummy != false )
  {
    m_dummyTexID = max;
    max += 1;
  }
	m_pTexSys = GFL_NEW( heap ) app::tool::LytTexReplaceSystem( heap, devHeap, TEX_SIZE, max );
}

//-----------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//-----------------------------------------------------------------------------
PokeIcon::~PokeIcon()
{
  if( m_isFileOpen != false )
  {
    GFL_ASSERT( m_isFileOpen );
    FileCloseSync();
  }
	GFL_DELETE m_pTexSys;
}

//-----------------------------------------------------------------------------
/**
 * @func    FileOpenSync
 * @brief   アークファイルを開く（同期版）
 * @date    2015.03.02
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void PokeIcon::FileOpenSync( gfl2::heap::HeapBase * heap )
{
  app::util::FileAccessor::FileOpenSync( ARCID_POKEICON, heap );
  m_isFileOpen = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    FileOpen
 * @brief   アークファイルを開く（非同期版）
 * @date    2015.03.02
 *
 * @param   heap  ヒープ
 */
//-----------------------------------------------------------------------------
void PokeIcon::FileOpen( gfl2::heap::HeapBase * heap )
{
  app::util::FileAccessor::FileOpen( ARCID_POKEICON, heap );
  m_isFileOpen = true;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileOpen
 * @brief   アークファイルオープンチェック
 * @date    2015.03.02
 *
 * @retval  false = オープン中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeIcon::IsFileOpen(void)
{
  return app::util::FileAccessor::IsFileOpen( ARCID_POKEICON );
}

//-----------------------------------------------------------------------------
/**
 * @func    FileCloseSync
 * @brief   アークファイルを閉じる（同期版）
 * @date    2015.03.02
 */
//-----------------------------------------------------------------------------
void PokeIcon::FileCloseSync(void)
{
  // 開いていない
  if( m_isFileOpen == false )
  {
    GFL_ASSERT( 0 );
  }
  app::util::FileAccessor::FileCloseSync( ARCID_POKEICON );
  m_isFileOpen = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    FileClose
 * @brief   アークファイルを閉じる（非同期版）
 * @date    2015.03.02
 */
//-----------------------------------------------------------------------------
void PokeIcon::FileClose( gfl2::heap::HeapBase * heap )
{
  // 開いていない
  if( m_isFileOpen == false )
  {
    GFL_ASSERT( 0 );
  }
  app::util::FileAccessor::FileClose( ARCID_POKEICON, heap );
  m_isFileOpen = false;
}

//-----------------------------------------------------------------------------
/**
 * @func    IsFileClose
 * @brief   アークファイルクローズチェック
 * @date    2015.03.02
 *
 * @retval  false = オープン中
 * @retval  true  = それ以外
 */
//-----------------------------------------------------------------------------
bool PokeIcon::IsFileClose(void)
{
  return app::util::FileAccessor::IsFileClose( ARCID_POKEICON );
}

//-----------------------------------------------------------------------------
/**
 * @brief		メモリ確保
 *
 * @param		id	確保する場所のID
 *
 * @return	none
 *
 * @li	あらかじめ確保しておく場合に使用
 */
//-----------------------------------------------------------------------------
void PokeIcon::AllocTextureBuffer( u32 id )
{
  m_pTexSys->AllocTextureBuffer( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャを読み込んでペインを変更
 *
 * @param		id				読み込む場所のID
 * @param		dat_id		アーカイブデータID
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void PokeIcon::ReplacePaneTextureCore( u32 id, u32 dat_id, gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  if( man->GetArcFile(ARCID_POKEICON) == NULL )
  {
    m_pTexSys->LoadTexture( m_pWorkHeap, id, ARCID_POKEICON, dat_id, IS_COMP );
	}else{
    m_pTexSys->LoadTextureCore( m_pWorkHeap, id, ARCID_POKEICON, dat_id, IS_COMP );
	}
  if( pane != NULL )
  {
    m_pTexSys->ReplaceTexture( id, pane, material, texture );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャを読み込んでペインを変更 ( CoreParam指定版 )
 *
 * @param		id				読み込む場所のID
 * @param		pp				CoreParam
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 * @param		flip			反転フラグ
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void PokeIcon::ReplacePaneTexture(
			u32 id, const pml::pokepara::CoreParam * pp,
			gfl2::lyt::LytPicture * pane, u32 material, u32 texture, bool flip )
{
	ReplacePaneTextureCore( id, GetResourceIndex(pp,flip), pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャを読み込んでペインを変更 ( SimpleParam指定版 )
 *
 * @param		id				読み込む場所のID
 * @param		simple_param  描画用ポケモン情報
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 * @param		flip			反転フラグ
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void PokeIcon::ReplacePaneTexture(
			u32 id, const PokeTool::SimpleParam * simple_param,
			gfl2::lyt::LytPicture * pane, u32 material, u32 texture, bool flip )
{
	ReplacePaneTextureCore( id, GetResourceIndex(simple_param,flip), pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		ペインのテクスチャを変更（データ読み込みなし版）
 *
 * @param		id				読み込む場所のID
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 *
 * @return	none
 *
 * @li  非同期読み込みの直後はReplaceReadTexture()を使用すること。
 */
//-----------------------------------------------------------------------------
void PokeIcon::ReplacePaneTexture( u32 id, gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  m_pTexSys->ReplaceTexture( id, pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャリソースのインデックスを取得 ( CoreParam指定版 )
 *
 * @param		pp		CoreParam
 * @param		flip	反転フラグ
 *
 * @return	テクスチャリソースのインデックス
 *
 * @li	アーカイブデータのインデックス取得です
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID PokeIcon::GetResourceIndex( const pml::pokepara::CoreParam * pp, bool flip )
{
	MonsNo mons = pp->GetMonsNo();
	pml::FormNo frm = pp->GetFormNo();
	pml::Sex sex = pp->GetSex();
	bool egg = pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG );

	return GetResourceIndex( mons, frm, sex, egg, flip );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャリソースのインデックスを取得 ( SimpleParam指定版 )
 *
 * @param		simple_param  描画用ポケモン構造体
 * @param		flip					反転フラグ
 *
 * @return	テクスチャリソースのインデックス
 *
 * @li	アーカイブデータのインデックス取得です
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID PokeIcon::GetResourceIndex( const PokeTool::SimpleParam * simple_param, bool flip )
{
  return GetResourceIndex( simple_param->monsNo, simple_param->formNo, simple_param->sex, simple_param->isEgg, flip );
}

//-----------------------------------------------------------------------------
/**
 * @brief		テクスチャリソースのインデックスを取得
 *
 * @param		mons	ポケモン番号
 * @param		frm		フォルム番号
 * @param		sex		性別
 * @param		egg		タマゴフラグ
 * @param		flip	反転フラグ
 *
 * @return	テクスチャリソースのインデックス
 */
//-----------------------------------------------------------------------------
gfl2::fs::ArcFile::ARCDATID PokeIcon::GetResourceIndex( MonsNo mons, pml::FormNo frm, pml::Sex sex, bool egg, bool flip )
{
	// MonsNoがタマゴ、ダメタマゴのとき
	if( mons == MONSNO_TAMAGO || mons == MONSNO_DAMETAMAGO ){
		return ( GARC_PokeIcon_poke_icon_egg_normal_m_001_bflim_COMP );
	}

	if( mons >= GFL_NELEMS(PokeIconGraphicTable) ){
		GFL_ASSERT( 0 );
		mons = MONSNO_NULL;
	}

	// タマゴ
	if( egg == true ){
		return ( GARC_PokeIcon_poke_icon_egg_normal_m_001_bflim_COMP );
	}

	const POKEICON_GRAPHIC * dat = &PokeIconGraphicTable[mons];

	// 反転
	if( flip == true ){
		if( dat->flipTable != NULL ){
			// フォルムなし
			if( dat->formTableSize == 0 ){
				// ♀
				if( sex == pml::SEX_FEMALE ){
					return dat->flipTable[1];
				// ♀以外
				}else{
					return dat->flipTable[0];
				}
			// フォルムあり
			}else{
				if( frm >= dat->formTableSize ){
					return dat->flipTable[0];
				}
				return dat->flipTable[frm];
			}
		}
	}

	// フォルムなし or テーブル数オーバー
	if( frm >= dat->formTableSize ){
		// ♀
		if( sex == pml::SEX_FEMALE ){
			return dat->mesuDataIndex;
		// ♀以外
		}else{
			return dat->osuDataIndex;
		}
	}

	return dat->formTable[frm];
}


//-----------------------------------------------------------------------------
/**
 * @brief		読み込みリクエスト ( CoreParam版 )
 *
 * @param		id		読み込む場所のID
 * @param		pp		CoreParam
 * @param		flip	反転フラグ
 *
 * @retval	"true = リクエスト成功 "
 * @retval	"false = リクエスト失敗"
 */
//-----------------------------------------------------------------------------
bool PokeIcon::ReadRequest( u32 id, const pml::pokepara::CoreParam * pp, bool flip )
{
  bool result = m_pTexSys->LoadTextureRequestCore(
                  m_pWorkHeap,
                  id,
                  ARCID_POKEICON,
                  GetResourceIndex(pp,flip),
                  IS_COMP );
  GFL_ASSERT( result );
  return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief		読み込みリクエスト ( SimpleParam版 )
 *
 * @param		id						読み込む場所のID
 * @param		simple_param	SimpleParam
 * @param		flip					反転フラグ
 *
 * @retval	"true = リクエスト成功 "
 * @retval	"false = リクエスト失敗"
 */
//-----------------------------------------------------------------------------
bool PokeIcon::ReadRequest( u32 id, const PokeTool::SimpleParam * simple_param, bool flip )
{
  bool result = m_pTexSys->LoadTextureRequestCore(
                  m_pWorkHeap,
                  id,
                  ARCID_POKEICON,
                  GetResourceIndex(simple_param,flip),
                  IS_COMP );
  GFL_ASSERT( result );
  return result;
}

//-----------------------------------------------------------------------------
/**
 * @brief		読み込み終了チェック
 *
 * @param		id		読み込む場所のID
 *
 * @retval	"true = 終了"
 * @retval	"false = それ以外"
 */
//-----------------------------------------------------------------------------
bool PokeIcon::IsLoadFinished( u32 id )
{
	return m_pTexSys->IsLoadTextureFinished( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief		読み込みリクエストをキャンセル
 *
 * @param		id		読み込む場所のID
 *
 * @retval	"true = キャンセル成功"
 * @retval	"false = キャンセル失敗"
 */
//-----------------------------------------------------------------------------
bool PokeIcon::CancelLoadTextureRequest( u32 id )
{
  return m_pTexSys->CancelLoadTextureRequest( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief		ペインのテクスチャを変更（データ読み込みなし版）
 *
 * @param		id				読み込む場所のID
 * @param		pane			ペイン
 * @param		material	マテリアルのインデックス
 * @param		texture		テクスチャのインデックス
 *
 * @return	none
 *
 * @li	非同期読み込み後、最初の変更はこれで。
 */
//-----------------------------------------------------------------------------
void PokeIcon::ReplaceReadTexture( u32 id, gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  m_pTexSys->LoadTexture( id );
  m_pTexSys->ReplaceTexture( id, pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief		クラスを破棄できるか
 *
 * @param		none
 *
 * @retval	"true = 可"
 * @retval	"false = 不可"
 */
//-----------------------------------------------------------------------------
bool PokeIcon::IsModuleFree(void)
{
	return m_pTexSys->IsModuleFree();
}

//-----------------------------------------------------------------------------
/**
 * @brief		ダミーテクスチャの読み込み
 *
 * @param		none
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void PokeIcon::LoadDummyTexture(void)
{
	if( m_dummyTexID == 0 )
  {
		GFL_ASSERT( 0 );
		return;
	}
  gfl2::fs::AsyncFileManager * man = GFL_SINGLETON_INSTANCE(GameSys::GameManager)->GetAsyncFileManager();
  if( man->GetArcFile(ARCID_POKEICON) == NULL )
  {
    m_pTexSys->LoadTexture( m_pWorkHeap, m_dummyTexID, ARCID_POKEICON, GARC_PokeIcon_poke_icon_000_m_001_bflim_COMP, IS_COMP );
  }
  else
  {
    m_pTexSys->LoadTextureCore( m_pWorkHeap, m_dummyTexID, ARCID_POKEICON, GARC_PokeIcon_poke_icon_000_m_001_bflim_COMP, IS_COMP );
  }
}

//-----------------------------------------------------------------------------
/**
 * @brief		ダミーテクスチャの読み込みリクエスト
 *
 * @param		none
 *
 * @return	none
 */
//-----------------------------------------------------------------------------
void PokeIcon::LoadDummyTextureRequest(void)
{
	if( m_dummyTexID == 0 )
  {
		GFL_ASSERT( 0 );
		return;
	}

  bool result = m_pTexSys->LoadTextureRequestCore(
                  m_pWorkHeap,
                  m_dummyTexID,
                  ARCID_POKEICON,
                  GARC_PokeIcon_poke_icon_000_m_001_bflim_COMP,
                  IS_COMP );
  GFL_ASSERT( result );
}

//-----------------------------------------------------------------------------
/**
 * @brief		ダミーテクスチャの読み込み終了チェック
 *
 * @param		none
 *
 * @retval	"true = 終了"
 * @retval	"false = それ以外"
 */
//-----------------------------------------------------------------------------
bool PokeIcon::IsLoadDummyTextureFinished(void)
{
	if( m_dummyTexID == 0 )
  {
		GFL_ASSERT( 0 );
		return true;
	}
	return IsLoadFinished( m_dummyTexID );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ペインのテクスチャをダミーテクスチャに変更
 *
 * @param   pane      ペイン
 * @param   material  マテリアルのインデックス
 * @param   texture   テクスチャのインデックス
 *
 * @li  非同期読み込みの直後はReplaceReadDummyTexture()を使用すること。
 */
//-----------------------------------------------------------------------------
void PokeIcon::ReplaceDummyTexture( gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  if( m_dummyTexID == 0 )
  {
    GFL_ASSERT( 0 );
    return;
  }
  ReplacePaneTexture( m_dummyTexID, pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief   ペインのテクスチャをダミーテクスチャに変更
 *
 * @param   pane      ペイン
 * @param   material  マテリアルのインデックス
 * @param   texture   テクスチャのインデックス
 *
 * @li  非同期読み込み後、最初の変更はこれで。
 */
//-----------------------------------------------------------------------------
void PokeIcon::ReplaceReadDummyTexture( gfl2::lyt::LytPicture * pane, u32 material, u32 texture )
{
  if( m_dummyTexID == 0 )
  {
    GFL_ASSERT( 0 );
    return;
  }
  ReplaceReadTexture( m_dummyTexID, pane, material, texture );
}

//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャがロード済みかどうかを調べる
 *
 * @param   id  チェックするテクスチャの管理番号
 *
 * @retval  "true = ロード済み"
 * @retval  "false = ロードしていない"
 */
//-----------------------------------------------------------------------------
bool PokeIcon::IsTextureLoaded( u32 id ) const
{
  return m_pTexSys->IsTextureLoaded( id );
}

//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャデータを取得する
 *
 * @param   id  場所のID
 *
 * @return  テクスチャデータ
 */
//-----------------------------------------------------------------------------
app::tool::LytTexData * PokeIcon::GetLytTexData( u32 id ) const
{
  return m_pTexSys->GetLayoutTextureData( id );
}


//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャデータをコピーする
 *
 * @param   src_id    コピー元ID
 * @param   dest_id   コピー先ID
 */
//-----------------------------------------------------------------------------
void PokeIcon::CopyTextureBuffer( u32 src_id, u32 dest_id )
{
  app::tool::LytTexData * dest = m_pTexSys->GetLayoutTextureData( dest_id );
  dest->CopyTextureBuffer( m_pTexSys->GetLayoutTextureData(src_id) );
}

//-----------------------------------------------------------------------------
/**
 * @brief   テクスチャデータを入れ替える
 *
 * @param   id1     ID1
 * @param   id2     ID2
 * @param   tmp_id  テンポラリID
 */
//-----------------------------------------------------------------------------
void PokeIcon::ChangeTextureBuffer( u32 id1, u32 id2, u32 tmp_id )
{
  CopyTextureBuffer( id1, tmp_id );
  CopyTextureBuffer( id2, id1 );
  CopyTextureBuffer( tmp_id, id2 );
}


GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)
