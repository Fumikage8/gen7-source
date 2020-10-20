//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		poke_tool.cpp
 *	@brief  ポケモンパラムプロジェクト側処理
 *	@author	Toru=Nagihashi
 *	@date		2012.06.01
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <niji_conv_header/poke_lib/item/itemsym.h>

#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeToolPokePlace.h>
#include <Savedata/include/ZukanSave.h>
#include <GameSys/include/GameManager.h>
#include <System/include/PokemonLanguage.h>

GFL_NAMESPACE_BEGIN(PokeTool)

/**
 * @brief SimpleParamをCoreParamから取得する
 *
 * @param param SimpleParam受け取り
 * @param core_param ポケモンパラム
 */
void GetSimpleParam( SimpleParam *param, const pml::pokepara::CoreParam *core_param )
{
  param->monsNo   = core_param->GetMonsNo();
  param->formNo   = core_param->GetFormNo();
  param->sex      = core_param->GetSex();
  param->isRare   = core_param->IsRare();
  param->isEgg    = core_param->IsEgg( pml::pokepara::CHECK_BOTH_EGG ); //描画用なので、両方タマゴとして表示するはず
  param->perRand  = core_param->GetPersonalRnd();
}


/**
 * @brief 描画用ポケモンパラメータを比較する
 *
 * @param a 比較A
 * @param b 比較B
 *
 * @return trueならば同じ falseならば異なる
 */
bool CompareSimpleParam( const SimpleParam &a, const SimpleParam &b )
{
  if(  a.monsNo == b.monsNo 
    && a.formNo == b.formNo 
    && a.sex    == b.sex
    && a.isRare == b.isRare
    && a.isEgg  == b.isEgg
    && a.perRand == b.perRand )
  {
    return true;
  }
  return false;
}

/**
 * @brief 演出用の重さカテゴリの取得。(ひこうタイプ・特性ふゆう・ふうせん持ちはNONE)
 *
 * @param core_param ポケモンパラム
 */
PokeEffWeight CheckPokemonEffectWeight( const pml::pokepara::CoreParam *cp )
{
  bool isFast = cp->StartFastMode();
  if( cp->GetType1() == POKETYPE_HIKOU ||
      cp->GetType2() == POKETYPE_HIKOU ||
      cp->GetTokuseiNo() == TOKUSEI_HUYUU ||
      cp->GetItem() == ITEM_HUUSEN )
  {
    cp->EndFastMode(isFast);
    return POKE_EFF_WEIGHT_NONE;
  }

  pml::personal::LoadPersonalData( cp->GetMonsNo() , cp->GetFormNo() ); 
  const u32 weight = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_weight );
  
  cp->EndFastMode(isFast);
  
  if( weight >= 2000 )
  {
    return POKE_EFF_WEIGHT_HEAVY;
  }
  else
  if( weight >= 1000 )
  {
    return POKE_EFF_WEIGHT_LIGHT_HEAVY;
  }
  else
  if( weight >= 500 )
  {
    return POKE_EFF_WEIGHT_MIDDLE;
  }
  return POKE_EFF_WEIGHT_LIGHT;

}

/**
 * @brief カロス地方で捕まえたポケモンかどうか
 *        （ステータスやBOXなどでのカロス地方捕獲マークにて使用）
 *
 * @param core_param  コアパラム
 * 
 */
//IsCalos無効になっているので消しておく
//bool IsCalosCaptured( const pml::pokepara::CoreParam *core_param )
//{
//  return false;
//}

/**
 * @brief 表示する性別を取得
 *
 * @param core_param  コアパラム
 * 
 * @retval "pml::SEX_MALE = ♂を表示"
 * @retval "pml::SEX_FEMALE = ♀を表示"
 * @retval "pml::SEX_UNNON = 性別を表示しない"
 */
pml::Sex GetDrawSex( const pml::pokepara::CoreParam *core_param )
{
  bool   flag          = core_param->StartFastMode(); // 高速モード
  MonsNo mons          = core_param->GetMonsNo();
  bool   is_egg        = core_param->IsEgg(pml::pokepara::CHECK_BOTH_EGG);
  bool   have_nickname = core_param->HaveNickName();
  pml::Sex sex         = core_param->GetSex();
  core_param->EndFastMode( flag );

  // 下の関数で判定
  return GetDrawSex( mons, sex, have_nickname, is_egg );
}

/** 
 * @brief CoreParamで判定できない時用の性別表示判定関数
 *
 * @param mons         ポケモンNO
 * @param sex          性別
 * @param haveNickName ニックネームフラグ
 * @param isEgg        タマゴフラグ（CHECK_BOTH_EGGで渡す事）
 *
 * @retval "pml::SEX_MALE = ♂を表示"
 * @retval "pml::SEX_FEMALE = ♀を表示"
 * @retval "pml::SEX_UNNON = 性別を表示しない"
 */
pml::Sex GetDrawSex( MonsNo mons, pml::Sex sex, bool haveNickName, bool isEgg )
{
  // タマゴなので性別無し扱い
  if( isEgg ){
    return pml::SEX_UNKNOWN;
  }

  // ニドランはニックネームフラグを確認した上で性別非表示判定を行う
  if( haveNickName == false ){
    if( mons == MONSNO_NIDORAN_M || mons == MONSNO_NIDORAN_F ){
      return pml::SEX_UNKNOWN;
    }
  }

  // そのままの性別を返す
  return sex;
}


/**
 * @brief 表示する図鑑マークを取得
 * @param core_param ポケモン
 * @param zukan 図鑑セーブデータ
 * @param extend  拡張データ
 * @return ZukanMarkDisplay列挙
 */
/*
誰も使っていないのでコメントアウト
DrawZukanMark GetDrawZukanMark( const pml::pokepara::CoreParam * core_param, const Savedata::ZukanData * zukan, const PokeTool::ExtendData *extend )
{
  if( zukan->GetZenkokuZukanFlag() )
  {
    return ZUKAN_MARK_INVISIBLE;
  }
  else
  {
    if( extend->IsExistLocalZukan( core_param ) )
    {
      poke_tool::ExtendData::LocalArea local = extend->GetLocalArea( core_param );
      return static_cast<DrawZukanMark>( static_cast<u32>(ZUKAN_MARK_LOCAL_A) + local-1 );
    }
    else
    {
      return ZUKAN_MARK_INVISIBLE;
    }
  }
}
*/

/**
 * @brief 表示する図鑑番号を取得(全国図鑑番号orアローラ図鑑番号(nijiではアローラ図鑑番号しか返さない))
 * @param core_param ポケモン
 * @param zukan 図鑑セーブデータ
 * @param extend  拡張データ
 * @return ZUKAN_NUMBER_UNKNOUWNならば???、それ以外ならばその数値を表示すべき
 */
s32 GetDrawZukanNumber( const pml::pokepara::CoreParam * core_param, const Savedata::ZukanData * zukan, const PokeTool::ExtendData *extend )
{
  if( zukan->GetZenkokuZukanFlag() )
  {
    return core_param->GetMonsNo();
  }
  else
  {
    if( extend->IsExistLocalZukan( core_param, PokeTool::ExtendData::LOCAL_AREA_A ) )
    {
      return extend->GetLocalNumber( core_param, PokeTool::ExtendData::LOCAL_AREA_A );
    }
    else
    {
      return ZUKAN_NUMBER_UNKNOUWN;
    }
  }
}

/**
 * @brief ポケモンをアプリで表示するとき可愛がりのモーションを使うか？
 *
 * @param core_param ポケモン
 * 
 * @return bool trueなら可愛がりモーションを使う
 */
bool CheckUsingKawaigariInApp( const pml::pokepara::CoreParam * core_param )
{
  return CheckUsingKawaigariInApp( core_param->GetMonsNo() , core_param->GetFormNo() );
}

bool CheckUsingKawaigariInApp( MonsNo monsNo , pml::FormNo formNo )
{
  //////////////////////////////////////////////////////////////
  //
  // 図鑑で似たような判定関数をつくっています。
  // 図鑑ではゼルネアスのフォルム1でバトルモーションを出すため、
  // この関数は使えませんでした。
  // この関数の中を変更したら図鑑にもお知らせ下さい。
  //
  //////////////////////////////////////////////////////////////
  
  if( monsNo == MONSNO_ZERUNEASU )
  {
    return true;
  }
  
  if( monsNo == MONSNO_KYUREMU &&
     (formNo == FORMNO_KYUREMU_WHITE || formNo == FORMNO_KYUREMU_BLACK ))
  {
    return true;
  } 
  return false;
}


/**
 * @brief Miiversse投稿可能なポケモンか？
 *
 * @param core_param ポケモン
 * 
 * @return bool falseだったらMiiverse投稿をふさぐ
 */
bool CheckCanPostMiiverse( const pml::PokeParty *party )
{
  bool enable = true;
  for( u32 i=0;i<party->GetMemberCount();i++ )
  {
    if( CheckCanPostMiiverse(party->GetMemberPointerConst(i)) == false )
    {
      enable = false;
      break;
    }
  }
  return enable;
}

bool CheckCanPostMiiverse( const pml::pokepara::CoreParam * core_param )
{
  return CheckCanPostMiiverse( core_param->GetMonsNo() , core_param->GetFormNo() );
}

bool CheckCanPostMiiverse( MonsNo monsNo , pml::FormNo formNo )
{
  if( monsNo == MONSNO_HUUPA || 
      monsNo == MONSNO_BORUKENION )
  {
    return false;
  }
  return true;
}

/**
 * @func    GetDrawID
 * @brief   表示用ID取得
 * @date    2015.11.26
 *
 * @param   cp    CoreParam
 * @param   id    ID格納場所
 *
 * @retval  true  = アローラのポケモン   6桁表示
 * @retval  false = それ以外             5桁表示
 */
bool GetDrawID( const pml::pokepara::CoreParam * cp, u32 * id )
{
  if( PokePlace::GetVersionSixID(cp->GetCassetteVersion()) != false )
  {
    *id = cp->GetID() % 1000000;
    return true;
  }
  else
  {
    *id = cp->GetID() & 0xffff;
    return false;
  }
}


GFL_NAMESPACE_END(PokeTool)
