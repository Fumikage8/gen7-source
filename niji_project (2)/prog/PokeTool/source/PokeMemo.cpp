//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		PokeMemo.cpp
 *	@brief  トレーナーメモ設定クラス
 *	@author	Toru=Nagihashi
 *	@date		2012.05.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_Str.h>
#include <pml/include/pmlib.h>
#include <Savedata/include/MyStatus.h>
#include <PokeTool/include/PokeToolPokePlace.h>
#include <PokeTool/include/PokeMemo.h>
#include "GameSys/include/NijiTime.h"

#include <niji_conv_header/poke_lib/item/itemsym.h>
#include <Savedata/include/ZukanSave.h>
#include <GameSys/include/GameManager.h>
#include <System/include/PokemonLanguage.h>
#include <System/include/PokemonVersion.h>


GFL_NAMESPACE_BEGIN(PokeTool)

//----------------------------------------------------------------------------
/**
 *	@brief  ポケモンへのメモ設定
 */
//-----------------------------------------------------------------------------
/**
 *  @brief 捕獲で捕まえた場合のポケメモを設定
 *  @param  param           ポケモンパラム
 *  @param  mystatus        マイステータス
 *  @param  place_no    捕まえた場所の地名メッセージ番号
 *  @param  temporary_heap  テンポラリ用ヒープ
 */
void PokeMemo::SetFromCapture( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, u32 place_no, gfl2::heap::HeapBase *temporary_heap )
{
  //タマゴは捕獲できない
  if( param->IsEgg(pml::pokepara::CHECK_BOTH_EGG) )
  {
    GFL_ASSERT( 0 );  //@fix
    return;
  }

  bool fast_mode = param->StartFastMode();
  
  PokeMemo::ClearPlaceTime( param, DATA_TYPE_EGG_TAKEN );
  PokeMemo::SetPlaceTime( param, place_no, DATA_TYPE_FIRST_CONTACT );
  PokeMemo::SetMyStatus( param, mystatus, temporary_heap );
  PokeMemo::SetGetLevel( param );
  PokeMemo::SetVersion( param );

  param->EndFastMode( fast_mode );
}

/**
 *  @brief ゲーム内交換した場合のポケメモを設定
 *  @param  param           ポケモンパラム
 */
void PokeMemo::SetFromInnerTrade( pml::pokepara::CoreParam * param )
{
  //タマゴは捕獲できない
  if( param->IsEgg(pml::pokepara::CHECK_BOTH_EGG) )
  {
    GFL_ASSERT( 0 );  //@fix
    return;
  }
  
  bool fast_mode = param->StartFastMode();

  //ゲーム内交換はポケモン取得に該当するのでCONTACT
  PokeMemo::ClearPlaceTime( param, DATA_TYPE_EGG_TAKEN );
  PokeMemo::SetPlaceTime( param, PokePlace::PLACE_NO_SPECIAL_INNER_TRADE, DATA_TYPE_FIRST_CONTACT );
  
  PokeMemo::SetGetLevel( param );
  PokeMemo::SetVersion( param );

  param->EndFastMode( fast_mode );
}
/**
 *  @brief 人と交換した場合のポケメモを設定
 *  @param  param           ポケモンパラム
 */
void PokeMemo::SetFromOuterTrade( pml::pokepara::CoreParam * param )
{
  if( param->IsEgg(pml::pokepara::CHECK_BOTH_EGG) )
  {
    //タマゴだったら、タマゴ入手時間だけ取得、
    //親情報はセットしない
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_OUTER_TRADE, DATA_TYPE_FIRST_CONTACT );
  }
  else
  {
    //なにもしない
  }
}

/**
 *  @brief タマゴから孵化した場合のポケメモを設定
 *  @param  param           ポケモンパラム
 *  @param  mystatus        マイステータス
 *  @param  place_no           捕まえた場所の地名メッセージ番号
 *  @param  temporary_heap  テンポラリ用ヒープ
 */
void PokeMemo::SetFromEggBirth( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, u32 place_no, gfl2::heap::HeapBase *temporary_heap )
{
  bool fast_mode = param->StartFastMode();

  if( mystatus->IsMyPokemon( *param ) )
  {
    PokeMemo::SetPlaceTime( param, place_no, DATA_TYPE_FIRST_CONTACT );
  }
  else
  {
    PokeMemo::CopyPlaceTime( param );
    
    //孵化後のポケモンでもバージョン表記をするため、
    //コピー後の場所１に地方名をいれる
    // PokeMemo::SetVersionPlace( param );
    // →バンクからタマゴが来ないので、バンクからきたタマゴが孵化したと目印をつけるための地名１へのバージョン名いれるものはなくす
    
    PokeMemo::SetPlaceTime( param, place_no, DATA_TYPE_FIRST_CONTACT );
  }
  PokeMemo::SetMyStatus( param, mystatus, temporary_heap );
  PokeMemo::SetVersion( param );

  param->EndFastMode( fast_mode );
  

}

/**
 *  @brief 過去作から持ってきた場合のポケメモを設定
 *  @param  param           ポケモンパラム
 */
void PokeMemo::SetFromPastTranslate( pml::pokepara::CoreParam * param )
{
  //なにもしない
}

/**
 *  @brief 配布で取得した場合のポケメモを設定  ->version、mystatus,EventFalgは外で設定してください。MEMORIES_CAPTURED_LEVELも外で設定して下さい。
 *  @param  param           ポケモンパラム
 *  @param  place_no           場所（ふしぎな贈り物等に入っている場所）
 *  @param  year            年（ふしぎな贈り物等に入っている年）
 *  @param  month           月（ふしぎな贈り物等に入っている月）
 *  @param  day             日（ふしぎな贈り物等に入っている日）
 */
void PokeMemo::SetFromDistribution( pml::pokepara::CoreParam * param, u32 place_no, u32 year, u32 month, u32 day )
{
  bool fast_mode = param->StartFastMode();
  
  //PokeMemo::SetGetLevel( param );  // @fix NMCat[3890] 【ふしぎなおくりもの】捕獲レベルが反映されない  →paramは捕獲レベル設定済みなので改めてセットしないようにコメントアウト。

  if( param->IsEgg(pml::pokepara::CHECK_BOTH_EGG) )
  {
    PokeMemo::SetPlaceTime( param, place_no, year, month, day, DATA_TYPE_EGG_TAKEN );
  }
  else
  {
    PokeMemo::SetPlaceTime( param, place_no, year, month, day, DATA_TYPE_FIRST_CONTACT );
  }

  param->EndFastMode( fast_mode );
}
/**
 *  @brief 自分のタマゴを貰った場合のポケメモを設定(育て屋、ゲーム内イベント)
 *  @param  param           ポケモンパラム
 *  @param  mystatus        マイステータス
 *  @param  place_no    捕まえた場所の地名メッセージ番号
 *  @param  temporary_heap  テンポラリ用ヒープ
 */
void PokeMemo::SetFromEggTaken( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, u32 place_no, gfl2::heap::HeapBase *temporary_heap )
{
  bool fast_mode = param->StartFastMode();

  PokeMemo::SetPlaceTime( param, place_no, DATA_TYPE_EGG_TAKEN );
  PokeMemo::ClearPlaceTime( param, DATA_TYPE_FIRST_CONTACT );
  PokeMemo::SetMyStatus( param, mystatus, temporary_heap );
  PokeMemo::SetVersion( param );
  param->EndFastMode( fast_mode );
}

/**
 * @brief 3DSDLAppにて貰った場合のポケメモを設定 ->version、mystatusは外で設定してください。MEMORIES_CAPTURED_LEVELも外で設定して下さい。
 *  @param  param           ポケモンパラム
 *  @param  place_no        場所（配布データに入っている場所）
 *  @param  year            年（配布データに入っている年）
 *  @param  month           月（配布データに入っている月）
 *  @param  day             日（配布データに入っている日）
 */
void PokeMemo::SetFrom3dsDlApp( pml::pokepara::CoreParam * param, u32 year, u32 month, u32 day )
{
  //PokeMemo::SetGetLevel( param );  // @fix NMCat[3890] 【ふしぎなおくりもの】捕獲レベルが反映されない  →paramは捕獲レベル設定済みなので改めてセットしないようにコメントアウト。こちらはnijiでは使われていないが一緒に修正しておく。

  //外部配信と同じで場所が固定なだけ
  PokeMemo::SetFromDistribution( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_3DSLINK, year, month, day );
}

//----------------------------------------------------------------------------
/**
 *	@brief  private
 */
//----------------------------------------------------------------------------
/**
 *  @brief 場所と時間を設定（現在の時刻を設定）
 *  @param  param           ポケモンパラム
 *  @param  place_no           場所
 *  @param  type            設定箇所
 */
void PokeMemo::SetPlaceTime( pml::pokepara::CoreParam * param, u32 place_no, DataType type )
{
  GameSys::DeviceDate date_time;

	PokeMemo::SetPlaceTime( param, place_no, date_time.GetYear()%100, date_time.GetMonth(), date_time.GetDay(), type );
}
/**
 *  @brief 場所と時間を設定（時刻を外部指定）
 *  @param  param           ポケモンパラム
 *  @param  place_no           場所
 *  @param  year            年
 *  @param  month           月
 *  @param  day             日
 *  @param  type            設定箇所
 */
void PokeMemo::SetPlaceTime( pml::pokepara::CoreParam * param, u32 place_no, u32 year, u32 month, u32 day, DataType type )
{
  if( type == DATA_TYPE_EGG_TAKEN )
  {
    param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_PLACE, place_no );
    param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_YEAR, year%100 );
    param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_MONTH, month );
    param->SetMemories( pml::pokepara::MEMORIES_EGG_TAKEN_DAY, day );
  }
  else
  {
    param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE, place_no );
    param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR, year%100 );
    param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH, month );
    param->SetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY, day );
  }
}

/**
 *  @brief 場所と時間を消す
 *  @param  param           ポケモンパラム
 *  @param  type            設定箇所
 */
void PokeMemo::ClearPlaceTime( pml::pokepara::CoreParam * param, DataType type )
{
  PokeMemo::SetPlaceTime( param, 0, 0, 0, 0, type );
}
/**
 *  @brief 場所と時間をCONTACTからEGG_TAGKEにへコピーする
 *  @param  param           ポケモンパラム
 *  @param  src_type        コピー元
 *  
 *  @note 孵化のときにしか使わない
 */
void PokeMemo::CopyPlaceTime( pml::pokepara::CoreParam * param )
{
  u32 place_no = param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_PLACE );
  u32 year  = param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_YEAR );
  u32 month = param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_MONTH );
  u32 day   = param->GetMemories( pml::pokepara::MEMORIES_FIRST_CONTACT_DAY );
  
  PokeMemo::SetPlaceTime( param, place_no, year, month, day, DATA_TYPE_EGG_TAKEN );
}

/**
 *  @brief 場所と時間をsrc_typeからもう片方へコピーする
 *  @param  param             ポケモンパラム
 *  @param  mystatus          設定するまいステータス
 *  @param  temporary_heap    テンポラリ用ヒープ
 */
void PokeMemo::SetMyStatus( pml::pokepara::CoreParam * param, const Savedata::MyStatus *mystatus, gfl2::heap::HeapBase *temporary_heap )
{
  bool fast_mode = param->StartFastMode();

  gfl2::str::StrBuf *tmp_buf = GFL_NEW(temporary_heap->GetLowerHandle()) gfl2::str::StrBuf( 32, temporary_heap->GetLowerHandle() );
  param->SetID( mystatus->GetID() );
  mystatus->GetNameString( tmp_buf );
  param->SetParentName( tmp_buf );
  param->SetParentSex( static_cast<pml::Sex>( mystatus->GetSex() ) );
  param->EndFastMode( fast_mode );

  GFL_SAFE_DELETE( tmp_buf );
}
/**
 *  @brief 現在のレベルを捕まえた時のレベルに設定する。
 *  @param  param
 */
void PokeMemo::SetGetLevel( pml::pokepara::CoreParam * param )
{
  param->SetMemories( pml::pokepara::MEMORIES_CAPTURED_LEVEL, param->GetLevel() );
}
/**
 *  @brief ロムバージョンを設定する
 *  @param  param   ポケモンパラム
 */
void PokeMemo::SetVersion( pml::pokepara::CoreParam * param )
{
  param->SetCassetteVersion( System::GetVersion() );
}

/**
 * @brief バージョンを場所１にいれる
 *
 * @param param
 */
/* // @note 未使用のためコメントアウト 2016/01/06 by nakahiro
void PokeMemo::SetVersionPlace( pml::pokepara::CoreParam * param )
{
  //場所１に地方名をセットする
  switch( param->GetCassetteVersion() )
  {
  case VERSION_SAPPHIRE: ///< AGBサファイア
  case VERSION_RUBY:     ///< AGBルビー
  case VERSION_EMERALD:  ///< AGBエメラルド
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_HOUEN, DATA_TYPE_EGG_TAKEN );
    break;
  case VERSION_RED:      ///< AGBファイアレッド
  case VERSION_GREEN:    ///< AGBリーフグリーン
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_KANTOU, DATA_TYPE_EGG_TAKEN );
    break;
  case VERSION_GOLD:     ///< DSハートゴールド
  case VERSION_SILVER:   ///< DSソウルシルバー
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_JYOUTO, DATA_TYPE_EGG_TAKEN );
    break;
  case VERSION_DIAMOND:  ///< DSダイヤモンド
  case VERSION_PEARL:    ///< DSパール
  case VERSION_PLATINUM: ///< DSプラチナ
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_SHINOU, DATA_TYPE_EGG_TAKEN );
    break;
  case VERSION_COLOSSEUM:///< GCコロシアム
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_FAR_PLACE, DATA_TYPE_EGG_TAKEN );
    break;
  case VERSION_WHITE:    ///< DSホワイト
  case VERSION_BLACK:    ///< DSブラック
  case VERSION_W2:       ///< DSホワイト２
  case VERSION_B2:       ///< DSブラック２
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_ISSHU, DATA_TYPE_EGG_TAKEN );
    break;
  case VERSION_X:
  case VERSION_Y:
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_KAROSU, DATA_TYPE_EGG_TAKEN );
    break;

  case VERSION_CTR_SAPPHIRE:
  case VERSION_CTR_RUBY:
    //なにもしない
    break;
    
  default:
    PokeMemo::SetPlaceTime( param, PokeTool::PokePlace::PLACE_NO_SPECIAL_UNKNOWN, DATA_TYPE_EGG_TAKEN );
  }
}
*/

GFL_NAMESPACE_END(PokeTool)
