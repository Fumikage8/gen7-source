//============================================================================================
/**
 * @file		BoxPokemonSave.cpp
 * @brief		ボックス内のポケモンセーブデータ
 * @author	Hiroyuki Nakamura
 * @date		12.02.22
 */
//============================================================================================
#include "Savedata/include/BoxPokemonSave.h"
#include "PokeTool/include/PokeToolForm.h"



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	savedata
 * @brief			セーブデータ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
GFL_NAMESPACE_BEGIN(Savedata)


//------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 */
//------------------------------------------------------------------------------------------
BoxPokemon::BoxPokemon(void)
{
}

//------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//------------------------------------------------------------------------------------------
BoxPokemon::~BoxPokemon()
{
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ボックス画面のセーブデータをセット
 *
 * @param		box		ボックス画面のセーブデータ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::SetBoxSaveData( BOX * box )
{
	sv_box = box;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		初期化
 *
 * @param		heap		ヒープ
 *
 * @return	none
 *
 * @li	MONSNO_NULLのCoreParamを作成し、全データにコピー
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::InitPokemonData( gfl2::heap::HeapBase * heap )
{
	pml::pokepara::CoreParam * cp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap, MONSNO_NULL, 0, 0 );

	for( int i=0; i<TRAY_MAX; i++ ){
		for( int j=0; j<TRAY_POKE_MAX; j++ ){
			cp->Serialize_Core( &poke[i].pokemonCoreParam[j][0] );
		}
	}

	GFL_DELETE cp;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		指定位置データをCoreParamにセット
 *
 * @param		cp			CoreParam
 * @param		tray		トレイ番号
 * @param		pos			トレイ内の位置
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::GetPokemon( pml::pokepara::CoreParam * cp, u32 tray, u32 pos )
{
  if ( tray >= TRAY_MAX )
  {
    GFL_ASSERT_STOP(0);
    return;
  }
  if ( pos >= TRAY_POKE_MAX )
  {
    GFL_ASSERT_STOP(0);
    return;
  }
	cp->Deserialize_Core( &poke[tray].pokemonCoreParam[pos][0] );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		指定位置データを取得
 *
 * @param		tray		トレイ番号
 * @param		pos			トレイ内の位置
 * @param		heap		ヒープ
 *
 * @return	CoreParam
 *
 * @li	CoreParamは各自削除！
 */
//------------------------------------------------------------------------------------------
pml::pokepara::CoreParam * BoxPokemon::GetPokemon( u32 tray, u32 pos, gfl2::heap::HeapBase * heap )
{
	pml::pokepara::CoreParam * cp = GFL_NEW( heap ) pml::pokepara::CoreParam( heap );
	GetPokemon( cp, tray, pos );
	return cp;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		存在チェック
 *
 * @param		tray		トレイ番号
 * @param		pos			トレイ内の位置
 * @param		tmp			テンポラリ領域
 * @param		egg			タマゴを含めるか ( true = 含める )
 *
 * @retval	"ture = 存在する"
 * @retval	"false = それ以外"
 *
 * @li	ボックスデータをtmpに入れてチェックします
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::CheckPokemon( u32 tray, u32 pos, pml::pokepara::CoreParam * tmp, bool egg )
{
	GetPokemon( tmp, tray, pos );

	if( tmp->IsNull() == false ){
		if( egg == false && tmp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true ){
			return false;
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		存在チェック
 *
 * @param		tray		トレイ番号
 * @param		pos			トレイ内の位置
 * @param		heap		ヒープ
 * @param		egg			タマゴを含めるか ( true = 含める )
 *
 * @retval	"ture = 存在する"
 * @retval	"false = それ以外"
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::CheckPokemon( u32 tray, u32 pos, gfl2::heap::HeapBase * heap, bool egg )
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	bool ret = CheckPokemon( tray, pos, tmp, egg );
	GFL_DELETE tmp;
	return ret;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		存在してない場所を返す
 *
 * @param		tray		トレイ番号
 * @retval	"成功の時は0以上の空いた場所 失敗の場合ー１"
 *
 */
//------------------------------------------------------------------------------------------
int BoxPokemon::CheckPokemonTrayRetPos( u32 tray, pml::pokepara::CoreParam * tmp )
{
	for( int i=0; i<TRAY_POKE_MAX; i++ ){
    if( CheckPokemon( tray, i, tmp ) == false ){
			return i;
    }
	}
	return -1;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン配置 ( 全トレイ対象版 )
 *
 * @param		cp		配置データ
 * @param		tray	検索開始トレイ番号
 * @param		tmp		テンポラリ領域
 *
 * @retval	"成功 = トレイ番号"
 * @retval	"失敗 = PUT_POKEMON_ERROR"
 *
 * @li	ボックスデータをtmpに入れて存在チェックを行います
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::PutPokemonEmptyTrayAll( const pml::pokepara::CoreParam * cp, u32 tray, pml::pokepara::CoreParam * tmp ,u16* pPos)
{
	u16	openMax = sv_box->GetTrayMax();		// 開放されているトレイの数
	u16	trayMax = TRAY_MAX;								// 使用可能なトレイの数

	// 開放済みのトレイを検索
	for( int i=0; i<openMax; i++ ){
		if( tray >= openMax ){
			tray = 0;
		}
		if( PutPokemonEmptyTray( cp, tray, tmp , pPos) == true ){
			return tray;
		}
		tray++;
	}
	// 未開放のトレイを検索
	for( int i=openMax; i<trayMax; i++ ){
		if( PutPokemonEmptyTray( cp, i, tmp , pPos) == true ){
			return i;
		}
	}
	return PUT_POKEMON_ERROR;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン配置 ( 全トレイ対象版 )
 *
 * @param		cp		配置データ
 * @param		tray	検索開始トレイ番号
 * @param		heap	ヒープ
 *
 * @retval	"成功 = トレイ番号"
 * @retval	"失敗 = PUT_POKEMON_ERROR"
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::PutPokemonEmptyTrayAll( const pml::pokepara::CoreParam * cp, u32 tray, gfl2::heap::HeapBase * heap ,u16* pPos)
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	u32 ret = PutPokemonEmptyTrayAll( cp, tray, tmp ,pPos);
	GFL_DELETE tmp;
	return ret;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン配置 ( トレイ指定版 )
 *
 * @param		cp		配置データ
 * @param		tray	トレイ番号
 * @param		tmp		テンポラリ領域
 *
 * @retval	"true = 成功"
 * @retval	"false = 失敗"
 *
 * @li	ボックスデータをtmpに入れて存在チェックを行います
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::PutPokemonEmptyTray( const pml::pokepara::CoreParam * cp, u32 tray, pml::pokepara::CoreParam * tmp ,u16* pPos)
{
	for( int i=0; i<TRAY_POKE_MAX; i++ ){
		if( PutPokemon( cp, tray, i, tmp ) == true ){
      if(pPos){
        *pPos=i;
      }
			return true;
		}
	}
	return false;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン配置 ( トレイ指定版 )
 *
 * @param		cp		配置データ
 * @param		tray	トレイ番号
 * @param		heap	ヒープ
 *
 * @retval	"true = 成功"
 * @retval	"false = 失敗"
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::PutPokemonEmptyTray( const pml::pokepara::CoreParam * cp, u32 tray, gfl2::heap::HeapBase * heap ,u16* pPos)
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	bool ret = PutPokemonEmptyTray( cp, tray, tmp , pPos);
	GFL_DELETE tmp;
	return ret;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン配置 ( トレイと位置指定版 )
 *
 * @param		cp		配置データ
 * @param		tray	トレイ番号
 * @param		pos		トレイ内の位置
 * @param		tmp		テンポラリ領域
 *
 * @retval	"true = 成功"
 * @retval	"false = 失敗"
 *
 * @li	ボックスデータをtmpに入れて存在チェックを行います
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::PutPokemon( const pml::pokepara::CoreParam * cp, u32 tray, u32 pos, pml::pokepara::CoreParam * tmp )
{
	if( CheckPokemon( tray, pos, tmp ) == true ){
		return false;
	}
  
  if ( tray >= TRAY_MAX )
  {
    GFL_ASSERT(0);
    return false;
  }
  if ( pos >= TRAY_POKE_MAX )
  {
    GFL_ASSERT(0);
    return false;
  }

	cp->Serialize_Core( &poke[tray].pokemonCoreParam[pos][0] );

	// 強制フォルムチェンジ
//	tmp->Deserialize_Core( &poke[tray].pokemonCoreParam[pos][0] );
//	if( PokeTool::form::UpdateFormForBox( tmp ) ){     /// @attention nijiで仕様が変更され 手持ちに渡すときにフォルムチェンジ
//		tmp->Serialize_Core( &poke[tray].pokemonCoreParam[pos][0] );
//	}

	return true;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン配置 ( トレイと位置指定版 )
 *
 * @param		cp		配置データ
 * @param		tray	トレイ番号
 * @param		pos		トレイ内の位置
 * @param		heap	ヒープ
 *
 * @retval	"true = 成功"
 * @retval	"false = 失敗"
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::PutPokemon( const pml::pokepara::CoreParam * cp, u32 tray, u32 pos, gfl2::heap::HeapBase * heap )
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	bool ret = PutPokemon( cp, tray, pos, tmp );
	GFL_DELETE tmp;
	return ret;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン入れ替え
 *
 * @param		tray1		トレイ番号１
 * @param		pos1		トレイ内位置１
 * @param		tray2		トレイ番号２
 * @param		pos2		トレイ内位置２
 * @param		heap		ヒープ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::ChangePokemon( u32 tray1, u32 pos1, u32 tray2, u32 pos2, gfl2::heap::HeapBase * heap )
{
  if ( tray1 >= sv_box->GetTrayMax() )
  {
    GFL_ASSERT(0);
    return;
  }
  if ( pos1 >= TRAY_POKE_MAX )
  {
    GFL_ASSERT(0);
    return;
  }
  if ( tray2 >= sv_box->GetTrayMax() )
  {
    GFL_ASSERT(0);
    return;
  }
  if ( pos2 >= TRAY_POKE_MAX )
  {
    GFL_ASSERT(0);
    return;
  }

	u8 * cp1 = &poke[tray1].pokemonCoreParam[pos1][0];
	u8 * cp2 = &poke[tray2].pokemonCoreParam[pos2][0];
	u8 * tmp = reinterpret_cast<u8 *>( GflHeapAllocMemoryLow(heap,pml::pokepara::CoreParam::DATASIZE) );
	gfl2::std::MemCopy( cp1, tmp, pml::pokepara::CoreParam::DATASIZE );
	gfl2::std::MemCopy( cp2, cp1, pml::pokepara::CoreParam::DATASIZE );
	gfl2::std::MemCopy( tmp, cp2, pml::pokepara::CoreParam::DATASIZE );
	GflHeapFreeMemory( tmp );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイ入れ替え
 *
 * @param		tray1		トレイ番号１
 * @param		tray2		トレイ番号２
 * @param		heap		ヒープ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::ChangeTray( u32 tray1, u32 tray2, gfl2::heap::HeapBase * heap )
{
	BoxTrayData * td1 = &poke[tray1];
	BoxTrayData * td2 = &poke[tray2];
	BoxTrayData * tmp = reinterpret_cast<BoxTrayData *>( GflHeapAllocMemoryLow(heap,sizeof(BoxTrayData)) );
	gfl2::std::MemCopy( td1, tmp, sizeof(BoxTrayData) );
	gfl2::std::MemCopy( td2, td1, sizeof(BoxTrayData) );
	gfl2::std::MemCopy( tmp, td2, sizeof(BoxTrayData) );
	GflHeapFreeMemory( tmp );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモン範囲移動
 *
 * @param		tray1		トレイ番号１
 * @param		pos1		トレイ内位置１
 * @param		tray2		トレイ番号２
 * @param		pos2		トレイ内位置２
 * @param		tmp			テンポラリ領域
 * @param		heap		ヒープ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::MovePokemon(
			u32 tray1, u32 pos1, u32 tray2, u32 pos2, u32 width, u32 height, pml::pokepara::CoreParam * tmp, gfl2::heap::HeapBase * heap )
{
	u8 * buf = reinterpret_cast<u8 *>( GflHeapAllocMemoryLow(heap,pml::pokepara::CoreParam::DATASIZE*width*height) );
	u8 * cpy = buf;

	tmp->Clear();

	for( u32 i=0; i<height; i++ ){
		for( u32 j=0; j<width; j++ ){
			u8 * cp = &poke[tray1].pokemonCoreParam[pos1+i*TRAY_W_LINE+j][0];
			gfl2::std::MemCopy( cp, cpy, pml::pokepara::CoreParam::DATASIZE );
			tmp->Serialize_Core( cp );
			cpy += pml::pokepara::CoreParam::DATASIZE;
		}
	}

	cpy = buf;
	for( u32 i=0; i<height; i++ ){
		for( u32 j=0; j<width; j++ ){
			tmp->Deserialize_Core( cpy );
			if( tmp->IsNull() == false ){
				u8 * cp = &poke[tray2].pokemonCoreParam[pos2+i*TRAY_W_LINE+j][0];
				gfl2::std::MemCopy( cpy, cp, pml::pokepara::CoreParam::DATASIZE );
			}
			cpy += pml::pokepara::CoreParam::DATASIZE;
		}
	}

	GflHeapFreeMemory( buf );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンデータ更新（強制フォルムチェンジ込み版）
 *
 * @param		cp		更新データ
 * @param		tray	トレイ番号
 * @param		pos		トレイ内の位置
 *
 * @return	none
 *
 * @li	変なデータで書き換えないように注意！
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::UpdatePokemon( pml::pokepara::CoreParam * cp, u32 tray, u32 pos )
{
  if ( tray >= sv_box->GetTrayMax() )
  {
    GFL_ASSERT(0);
    return;
  }
  if ( pos >= TRAY_POKE_MAX )
  {
    GFL_ASSERT(0);
    return;
  }

//	PokeTool::form::UpdateFormForBox( cp );	// 強制フォルムチェンジ  @attention nijiで仕様が変更になりここでは書き換えない
	cp->Serialize_Core( &poke[tray].pokemonCoreParam[pos][0] );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンデータ更新（内部でCoreParamを書き換えない版）
 *
 * @param		cp		更新データ
 * @param		tray	トレイ番号
 * @param		pos		トレイ内の位置
 *
 * @return	none
 *
 * @li	変なデータで書き換えないように注意！
 * @li	フォルムチェンジしません
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::UpdatePokemonConst( const pml::pokepara::CoreParam * cp, u32 tray, u32 pos )
{
  if ( tray >= sv_box->GetTrayMax() )
  {
    GFL_ASSERT(0);
    return;
  }
  if ( pos >= TRAY_POKE_MAX )
  {
    GFL_ASSERT(0);
    return;
  }
	cp->Serialize_Core( &poke[tray].pokemonCoreParam[pos][0] );
}


//------------------------------------------------------------------------------------------
/**
 * @brief		データクリア
 *
 * @param		tray	トレイ番号
 * @param		pos		トレイ内の位置
 * @param		tmp		テンポラリ領域
 *
 * @return	none
 *
 * @li	tmpをクリアしてボックスデータに入れます
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::ClearPokemon( u32 tray, u32 pos, pml::pokepara::CoreParam * tmp )
{
  if ( tray >= TRAY_MAX )
  {
    GFL_ASSERT(0);
    return;
  }
  if ( pos >= TRAY_POKE_MAX )
  {
    GFL_ASSERT(0);
    return;
  }

	tmp->Clear();
	tmp->Serialize_Core( &poke[tray].pokemonCoreParam[pos][0] );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		データクリア
 *
 * @param		tray	トレイ番号
 * @param		pos		トレイ内の位置
 * @param		heap	ヒープ
 *
 * @return	none
 */
//------------------------------------------------------------------------------------------
void BoxPokemon::ClearPokemon( u32 tray, u32 pos, gfl2::heap::HeapBase * heap )
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	ClearPokemon( tray, pos, tmp );
	GFL_DELETE tmp;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		全トレイに存在するポケモンの数を取得
 *
 * @param		tmp		テンポラリ領域
 * @param		egg		タマゴを含めるか ( true = 含める )
 *
 * @return	ポケモンの数
 *
 * @li	ボックスデータをtmpに入れてチェックします
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetPokemonCountAll( pml::pokepara::CoreParam * tmp, bool egg )
{
	u32	max = TRAY_MAX;
	u32	cnt = 0;

	for( int i=0; i<max; i++ ){
		cnt += GetPokemonCount( i, tmp, egg );
	}

	return cnt;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		全トレイに存在するポケモンの数を取得
 *
 * @param		heap	ヒープ
 * @param		egg		タマゴを含めるか ( true = 含める )
 *
 * @return	ポケモンの数
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetPokemonCountAll( gfl2::heap::HeapBase * heap, bool egg )
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	u32	cnt = GetPokemonCountAll( tmp, egg );
	GFL_DELETE tmp;
	return cnt;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイに存在するポケモンの数を取得
 *
 * @param		tray	トレイ番号
 * @param		tmp		テンポラリ領域
 * @param		egg		タマゴを含めるか ( true = 含める )
 *
 * @return	ポケモンの数
 *
 * @li	ボックスデータをtmpに入れてチェックします
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetPokemonCount( u32 tray, pml::pokepara::CoreParam * tmp, bool egg )
{
	u32	cnt = 0;

	for( int i=0; i<TRAY_POKE_MAX; i++ ){
		if( CheckPokemon( tray, i, tmp, egg ) == true ){
			cnt++;
		}
	}

	return cnt;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイに存在するポケモンの数を取得
 *
 * @param		tray	トレイ番号
 * @param		heap	ヒープ
 * @param		egg		タマゴを含めるか ( true = 含める )
 *
 * @return	ポケモンの数
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetPokemonCount( u32 tray, gfl2::heap::HeapBase * heap, bool egg )
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	u32	cnt = GetPokemonCount( tray, tmp, egg );
	GFL_DELETE tmp;
	return cnt;
}


//------------------------------------------------------------------------------------------
/**
 * @brief		全トレイの空き数を取得
 *
 * @param		tmp		テンポラリ領域
 *
 * @return	空き数
 *
 * @li	ボックスデータをtmpに入れてチェックします
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetSpaceCountAll( pml::pokepara::CoreParam * tmp )
{
	u32	max = TRAY_MAX;
	max *= TRAY_POKE_MAX;

	u32	cnt = GetPokemonCountAll( tmp );
	if( cnt > max ){
		GFL_ASSERT( 0 );
		return 0;
	}

	return ( max - cnt );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		全トレイの空き数を取得
 *
 * @param		heap		ヒープ
 *
 * @return	空き数
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetSpaceCountAll( gfl2::heap::HeapBase * heap )
{
	u32	max = TRAY_MAX;
	max *= TRAY_POKE_MAX;

	u32	cnt = GetPokemonCountAll( heap );
	if( cnt > max ){
		GFL_ASSERT( 0 );
		return 0;
	}

	return ( max - cnt );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイの空き数を取得
 *
 * @param		tray		トレイ番号
 * @param		tmp		テンポラリ領域
 *
 * @return	空き数
 *
 * @li	ボックスデータをtmpに入れてチェックします
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetSpaceCount( u32 tray, pml::pokepara::CoreParam * tmp )
{
	return ( TRAY_POKE_MAX - GetPokemonCount(tray,tmp) );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイの空き数を取得
 *
 * @param		tray		トレイ番号
 * @param		heap		ヒープ
 *
 * @return	空き数
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::GetSpaceCount( u32 tray, gfl2::heap::HeapBase * heap )
{
	return ( TRAY_POKE_MAX - GetPokemonCount(tray,heap) );
}

//------------------------------------------------------------------------------------------
/**
 * @brief		空き位置を取得（トレイと位置指定）
 *
 * @param		tray	トレイ番号
 * @param		pos		位置
 * @param		tmp		テンポラリ領域
 *
 * @retval	"true = 空きあり"
 * @retval	"false = 空きなし"
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::GetSpacePos( u32 & tray, u32 & pos, pml::pokepara::CoreParam * tmp )
{
	u8	openMax   = sv_box->GetTrayMax();		// 開放されているトレイの数
	u8	trayMax   = TRAY_MAX;								// 使用可能なトレイの数
	u8	startPos  = pos;
	u8	startTray = tray;


	// 開放済みのトレイを検索
	for( int i=0; i<openMax; i++ ){
		if( tray >= openMax ){
			tray = 0;
		}
		for( int j=pos; j<TRAY_POKE_MAX; j++ ){
			if( CheckPokemon( tray, j, tmp, true ) == false ){
				pos = j;
				return true;
			}
		}
    // kujiraGFBTS[2602] ボックスの空き位置検索の不具合修正
		pos = 0;	// 開始トレイ以外のトレイが開始位置より前の位置を検索しないため修正

		tray++;
	}
	// 初期トレイの指定位置より前の部分を検索
	tray = startTray;
	for( int i=0; i<startPos; i++ ){
		if( CheckPokemon( tray, i, tmp, true ) == false ){
			pos = i;
			return true;
		}
	}
	// 未開放のトレイを検索
	for( int i=openMax; i<trayMax; i++ ){
		for( int j=0; j<TRAY_POKE_MAX; j++ ){
			if( CheckPokemon( i, j, tmp, true ) == false ){
				tray = i;
				pos  = j;
				return true;
			}
		}
	}

	GFL_ASSERT( 0 );
	return false;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		空き位置を取得（トレイと位置指定）
 *
 * @param		tray	トレイ番号
 * @param		pos		位置
 * @param		heap	ヒープ
 *
 * @retval	"true = 空きあり"
 * @retval	"false = 空きなし"
 */
//------------------------------------------------------------------------------------------
bool BoxPokemon::GetSpacePos( u32 & tray, u32 & pos, gfl2::heap::HeapBase * heap )
{
	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	bool flg = BoxPokemon::GetSpacePos( tray, pos, tmp );
	GFL_DELETE tmp;
	return flg;
}

//------------------------------------------------------------------------------------------
/**
 * @brief		トレイ開放
 *
 * @param		heap	ヒープ
 *
 * @return	開放されているトレイの数
 */
//------------------------------------------------------------------------------------------
u32 BoxPokemon::UpdateTrayMax( gfl2::heap::HeapBase * heap )
{
	u32	trayMax = sv_box->GetTrayMax();

	if( trayMax >= BOX::TRAY_NUM_4TH ){
		return trayMax;
	}

	pml::pokepara::CoreParam * tmp = GFL_NEW_LOW( heap ) pml::pokepara::CoreParam( heap );
	u32	i;
	for( i=0; i<BOX::TRAY_NUM_4TH; i++ ){
		if( GetPokemonCount( i, tmp ) == 0 ){
			break;
		}
	}
	GFL_DELETE tmp;

	if( i >= trayMax ){
		if( i >= BOX::TRAY_NUM_3RD )
    {
      sv_box->SetTrayMax( BOX::TRAY_MAX );
      return BOX::TRAY_MAX;
		}
    else if( i >= BOX::TRAY_NUM_2ND )
    {
			sv_box->SetTrayMax( BOX::TRAY_NUM_3RD );
			return BOX::TRAY_NUM_3RD;
		}
    else
    {
			sv_box->SetTrayMax( BOX::TRAY_NUM_2ND );
			return BOX::TRAY_NUM_2ND;
		}
	}

	return trayMax;
}


GFL_NAMESPACE_END(Savedata)
