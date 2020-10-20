//==============================================================================
/**
 * @file poke_tool_Form.cpp
 * @brief フォルムチェンジ処理関連ツール
 * @date  2012.06.15
 * @author  tamada@gamefreak.co.jp
 */
//==============================================================================

//#include <gflib.h>
#include "pml/include/pmlib.h"

#include "PokeTool/include/PokeToolForm.h"

#include "GameSys/include/TimeZone.h"


namespace PokeTool {

namespace form {

 /**
  * @brief 実体の宣言をここに移動
  */
 const u32 TRIMING_KEEP_DAYS = 5;
 const u32 HUUPA_KEEP_DAYS = 3;


//======================================================================
//======================================================================
//------------------------------------------------------------------------
/**
 * @brief  ケルディオフォルムチェンジ
 * @param my_party
 */
//------------------------------------------------------------------------
void UpdateKerudhioForm( GameSys::GameData * gamedata )
{
  pml::PokeParty * my_party = gamedata->GetPlayerParty();
  int max = my_party->GetMemberCount();
  for ( int i = 0; i < max; i ++ )
  {
    pml::pokepara::PokemonParam * pp = my_party->GetMemberPointer(i);

    UpdateKerudhioFormPP(gamedata,pp);
  }
}

//------------------------------------------------------------------------
/**
 * @brief  ケルディオフォルムチェンジ PokemonParam版
 * @param  PokemonParam*
 */
//------------------------------------------------------------------------
void UpdateKerudhioFormPP( GameSys::GameData * gamedata, pml::pokepara::PokemonParam * pp )
{
  if ( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ){
    return;
  }
  if ( pp->GetMonsNo() == MONSNO_KERUDHIO
       && pp->GetFormNo() == FORMNO_KERUDHIO_SWORD
       && pp->HaveWaza( WAZANO_SINPINOTURUGI) == false )
  {
    //ソードフォルム（かくごのすがた）でしんぴのつるぎを持っていない場合は
    //フォルムをノーマルに戻す
    pp->ChangeFormNo( FORMNO_KERUDHIO_NORMAL );

    // 図鑑登録「見た」
    gamedata->GetZukanData()->SetPokeSee( *pp );
  }
}


//------------------------------------------------------------------------
/**
 * @brief 該当する時間に夜が含まれるか？
 * @param now
 * @param passed_minutes
 * @return bool 夜が含まれる
 */
//------------------------------------------------------------------------
static bool IsPassedNight( const gfl2::system::Date *now, u32 passed_minutes )
{
  int now_hour = now->GetHour();
  if ( passed_minutes > 60 * 24 ) return true;  //24時間経過していたら何も考えずに戻す

  int night_start = GameSys::TimeZone::ConvertTimeZoneToStartHour( GameSys::TimeZone::NIGHT );
  int morning_start = GameSys::TimeZone::ConvertTimeZoneToStartHour( GameSys::TimeZone::MORNING );
  if ( now_hour < morning_start || now_hour >= night_start )
  { //今がNGだったら戻す
    return true;
  }


  // 1つ前の時間を求める。
  // 経過時間が12時間以上で、1つ前の時間が昼だった場合も下ろす必要がある。
  if( passed_minutes >= (12 * 60) )
  {
    s32 before_hour = ( now_hour * 60 + now->GetMinute() - (s32)passed_minutes ) / 60;
    while( before_hour < 0 )
    {
      before_hour += 24;
    }

    // before_hourも、
    if ( before_hour < morning_start || before_hour >= night_start )
    { //今がNGだったら戻す
      return true;
    }
  }

  return false;
}

//------------------------------------------------------------------------
/**
 * 
 * @param my_party
 * @param now
 * @param passed_minutes
 */
//------------------------------------------------------------------------
void UpdateSheimiForm( pml::PokeParty * my_party, const gfl2::system::Date *now, u32 passed_minutes )
{
  if ( IsPassedNight( now, passed_minutes ) == true )
  {
    ChangeSheimiFormNormal( my_party );
  }
}

//------------------------------------------------------------------------
/**
 * @brief シェイミのフォルムを元に戻す
 * @param my_party
 * 
 * シェイミは時間でフォルムが変わる都合上、どの入手ルートでも必ずノーマルフォルムで初回登録されるため
 * ゲーム内の時間チェックetcでスカイからノーマルに戻す本関数で、ノーマルを見た登録しなくても大丈夫
 * 
 * 直してもいいのだが、gamedataを引数にとっていないので時期的にnijiではこのままにする by iwasawa 160608
 * 
 */
//------------------------------------------------------------------------
void ChangeSheimiFormNormal( pml::PokeParty * my_party )
{
  int max = my_party->GetMemberCount();
  for ( int i = 0; i < max; i ++ )
  {
    pml::pokepara::PokemonParam * pp = my_party->GetMemberPointer(i);
    if ( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ) continue;
    if ( pp->GetMonsNo() == MONSNO_SHEIMI && pp->GetFormNo() == FORMNO_SHEIMI_SKY )
    {
      pp->ChangeFormNo(  FORMNO_SHEIMI_LAND );
    }
  }
}

//------------------------------------------------------------------------
/**
 * @brief プードル(トリミアン)とフーパのフォルム更新処理
 * @param my_party
 * @param passed_days
 */
//------------------------------------------------------------------------
void UpdatePoodleForm( pml::PokeParty * my_party, u32 passed_days )
{
  //内部で持つカウントの上限が255なのでそれ以内に丸める
  if ( passed_days > 255 ) passed_days = 255;

  int max = my_party->GetMemberCount();
  for ( int i = 0; i < max; i ++ )
  {
    pml::pokepara::PokemonParam * pp = my_party->GetMemberPointer(i);
    MonsNo monsno = pp->GetMonsNo();
    FormNo formno = pp->GetFormNo();

    if ( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ) continue;

    //トリミアンorフーパだった時チェック
    if ( ( monsno == MONSNO_TORIMIAN && formno != FORMNO_TORIMIAN_NORMAL ) ||
         ( monsno == MONSNO_HUUPA    && formno != FORMNO_HUUPA_NORMAL ))
    {
      //経過日数カウント
      int keep = pp->TORIMIAN_GetKeepDayCount();
      int keep_max = pp->TORIMIAN_GetKeepMaxDayCount();  //@fix BTS6417
      keep += passed_days;
      if ( keep > 255 ) keep = 255;
      pp->TORIMIAN_SetKeepDayCount( keep );
      if ( keep > keep_max )
      {
        pp->TORIMIAN_SetKeepMaxDayCount( keep );
      }
      //体毛カウントを減らす
      u32 hair_count = pp->TORIMIAN_GetBodyHairCount();
      if ( hair_count > passed_days )
      {
        hair_count -= passed_days;
      }
      else
      {
        hair_count = 0;
      }
      pp->TORIMIAN_SetBodyHairCount( hair_count );

      if ( hair_count == 0 )
      {
        // GFBTS2302/BTS6412 ノーマルフォルム戻すときにはカウンタもクリア
        if( monsno == MONSNO_HUUPA ){
          SetHuupaForm( pp, FORMNO_HUUPA_NORMAL );
        }else{
          SetPoodleForm( pp, FORMNO_TORIMIAN_NORMAL );
        }
      }
    }
  }
}

//------------------------------------------------------------------------
/**
 * @brief プードルポケモンのフォルムをセット(トリミアン)
 * @param pp
 * @param formno
 * 
 * トリミアンは各種入手ルート、また、手持ちにはいる時に、必ずノーマルフォルムで登録される
 * 
 * ノーマルに戻す本関数で、ノーマルを見た登録しなくても大丈夫
 * ノーマル以外に変える場所では呼び出し元で登録している
 * 
 * ここで見た登録するよう直した方が直感的だが、gamedataを引数にとっていないので時期的にnijiではこのままにする by iwasawa 160608
 */
//------------------------------------------------------------------------
void SetPoodleForm( pml::pokepara::CoreParam * pp, pml::FormNo formno )
{
  if( pp->GetMonsNo() != MONSNO_TORIMIAN ){
    return; //トリミアン以外だったら何もしない
  }
  if ( formno >= FORMNO_TORIMIAN_MAX )
  {
    GFL_ASSERT(0);
    formno = FORMNO_TORIMIAN_NORMAL;
  }
  pp->ChangeFormNo( formno );
  if ( formno == FORMNO_TORIMIAN_NORMAL )
  {
    // GFBTS2302/BTS6412 ノーマルフォルム戻すときにはカウンタもクリア
    pp->TORIMIAN_SetKeepDayCount( 0 );   // 経過日数クリア
    pp->TORIMIAN_SetBodyHairCount( 0 );  //体毛カウントクリア
  }
  else
  { //　ノーマルフォルム以外の場合は
    pp->TORIMIAN_SetBodyHairCount( TRIMING_KEEP_DAYS );
  }
}


void SetPoodleForm( pml::pokepara::PokemonParam * pp, pml::FormNo formno )
{
  pml::pokepara::CoreParam *cp = pp;
  SetPoodleForm( cp, formno );
}

//------------------------------------------------------------------------
/**
 * @brief フーパのフォルムをセット
 * @param pp
 * @param formno
 * 
 * フ－パは各種入手ルート、また、手持ちにはいる時に、必ずノーマルフォルムで登録される
 * 
 * ノーマルに戻す本関数で、ノーマルを見た登録しなくても大丈夫
 * ノーマル以外に変える場所では呼び出し元で登録している
 * 
 * ここで見た登録するよう直した方が直感的だが、gamedataを引数にとっていないので時期的にnijiではこのままにする by iwasawa 160608
 */
//------------------------------------------------------------------------
void SetHuupaForm( pml::pokepara::CoreParam * pp, pml::FormNo formno )
{
  if( pp->GetMonsNo() != MONSNO_HUUPA ){
    return; //フーパ以外だったら何もしない
  }
  if ( formno >= FORMNO_HUUPA_MAX )
  {
    GFL_ASSERT(0);
    formno = FORMNO_HUUPA_NORMAL;
  }
  pp->ChangeFormNo( formno );
  if ( formno == FORMNO_HUUPA_NORMAL )
  {
    // GFBTS2302/BTS6412 ノーマルフォルム戻すときにはカウンタもクリア
    pp->TORIMIAN_SetKeepDayCount( 0 );   // 経過日数クリア
    pp->TORIMIAN_SetBodyHairCount( 0 );  //体毛カウントクリア
  }
  else
  { //　ノーマルフォルム以外の場合は
    pp->TORIMIAN_SetBodyHairCount( HUUPA_KEEP_DAYS );
  }
}
void SetHuupaForm( pml::pokepara::PokemonParam * pp, pml::FormNo formno )
{
  pml::pokepara::CoreParam *cp = pp;
  SetHuupaForm( cp, formno );
}

//------------------------------------------------------------------------
/**
 * @brief ミノムッチのフォルムを地形で更新する
 * @param[out]  p_gamedata    ゲーム進行データ
 * @param[in]   fightIndex    バトルに参加したかどうか？のフラグ列
 * @param[in]   form_no       地形から導出したフォルムナンバー
 *
 * @note  2013.05.18 BTS6803【シーケンス：ミノムッチのフォルム違いが登録されるタイミングが前作と相違している】対応のため、引数を変更した
 */
//------------------------------------------------------------------------
void UpdateMinomuttiForm( GameSys::GameData * p_gamedata,
    const u8 * fightIndex, u8 form_no )
{
  pml::PokeParty * my_party = p_gamedata->GetPlayerParty();
  int count = my_party->GetMemberCount();

  for ( int i = 0; i < count; ++ i )
  {
    if ( fightIndex[i] )
    {
      pml::pokepara::PokemonParam * pp;
      pp = my_party->GetMemberPointer( i );
      if ( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) ) continue;
      if ( pp->GetMonsNo() == MONSNO_MINOMUTTI )
      {
        pp->ChangeFormNo( static_cast<pml::FormNo>(form_no) );
        //BTS6803
        p_gamedata->GetZukanData()->SetPokeSee( *pp );
      }
    }
  }
}


//------------------------------------------------------------------------
/**
 * @brief     ボックスでのフォルム解除処理
 * @param     pp  CoreParamへのポインタ
 * @return     true = フォルムを強制セットした
 */
//------------------------------------------------------------------------
bool UpdateFormForBox( pml::pokepara::CoreParam * pp )
{
  switch ( pp->GetMonsNo() )
  {
  case MONSNO_SHEIMI:
    //シェイミはボックスに入れたらランドフォルムに強制セット
    if( pp->GetFormNo() != FORMNO_SHEIMI_LAND )
    {
      pp->ChangeFormNo( FORMNO_SHEIMI_LAND );
      return true;
    }
    break;
  case MONSNO_TORIMIAN:
    //トリミアンはボックスに入れたらノーマルフォルムに強制セット
    if( pp->GetFormNo() != FORMNO_TORIMIAN_NORMAL )
    {
      pp->ChangeFormNo( FORMNO_TORIMIAN_NORMAL );
      pp->TORIMIAN_SetKeepDayCount( 0 );
      pp->TORIMIAN_SetBodyHairCount( 0 );
      return true;
    }
    break;
  case MONSNO_HUUPA:
    //フーパはボックスに入れたらノーマルフォルムに強制セット
    if( pp->GetFormNo() != FORMNO_HUUPA_NORMAL )
    {
      SetHuupaForm( pp, FORMNO_HUUPA_NORMAL ); // ノーマルフォルムに戻す
      return true;
    }
    break;
  }
  return false;
}

//------------------------------------------------------------------------
/**
 * @brief     ジガルデ生成処理
 * 
 * PokemonParamをnewするので、呼び出し元で開放してください
 * ジガルデ生成で消費されるセル数のチェックや処理は外部でやる
 */
//------------------------------------------------------------------------
pml::pokepara::PokemonParam * ZigarudeMakeFromCell( GameSys::GameData* p_gdata, FormNo formNo, u32 level, gfl2::heap::HeapBase* p_heap )
{
  pml::pokepara::PokemonParam* p_pp = NULL;

  pml::pokepara::InitialSpec spec;
  spec.monsno = MONSNO_ZIGARUDE;
  spec.formno = formNo;
  spec.level  = level;
  spec.id      = p_gdata->GetPlayerStatus()->GetID();
  spec.rare_rnd  = pml::pokepara::INIT_SPEC_RARE_FALSE; //ノーマルカラー固定
  spec.talentVNum  = 3; //3V指定

  {
    //シンクロ反映
    pml::PokeParty* party = p_gdata->GetPlayerParty();
    pml::pokepara::PokemonParam* p_mypoke;
    int  idx;

    //タマゴでない先頭ポケモン(瀕死は可)のパラメータチェック
    idx = party->GetMemberTopIndex(pml::PokeParty::POKE_SEARCH_TYPE_NOT_EGG);
    if( idx != pml::PokeParty::MEMBER_INDEX_ERROR ){

      p_mypoke = party->GetMemberPointer(idx);
      if( p_mypoke->GetTokuseiNo() == TOKUSEI_SINKURO ){
        spec.seikaku = p_mypoke->GetSeikaku();
      }
    }
  }

  p_pp = GFL_NEW(p_heap)  pml::pokepara::PokemonParam( p_heap, spec );

  //モンスターボールセット
  p_pp->SetGetBall( item::BALLID_MONSUTAABOORU );

  //バージョン設定
  p_pp->SetCassetteVersion( System::GetVersion() );

  return p_pp;
}

//------------------------------------------------------------------------
/**
 * @brief バトル専用フォルムをノーマルに戻す
 * @param my_party  手持ちポケモンパーティー
 * @note  改造対策用
 */
//------------------------------------------------------------------------
void NormalizeBattleForm( pml::PokeParty * my_party )
{
  const int max = my_party->GetMemberCount();
  for ( int i = 0; i < max; i ++ )
  {
    pml::pokepara::PokemonParam * pp = my_party->GetMemberPointer(i);

    // ※メガのフォルムは、PokeToolMega.cppのNormalizeMegaFormでやっている

    // 対象は以下
    // ・サトシゲッコウガ
    // ・ジガルデパーフェクトフォルム
    // ・光ドラゴン


    // サトシゲッコウガ
    if( pp->GetMonsNo() == MONSNO_GEKKOUGA &&
        pp->GetFormNo() == FORMNO_GEKKOUGA_SATOSHI )
    {
      pp->ChangeFormNo( FORMNO_GEKKOUGA_NORMAL );
    }

    // ジガルデパーフェクトフォルム
    if( pp->GetMonsNo() == MONSNO_ZIGARUDE &&
        pp->GetFormNo() == FORMNO_ZIGARUDE_100PER )
    {
      pp->ChangeFormNo( FORMNO_ZIGARUDE_50PER );
    }

    // 光ドラゴン
    if( pp->GetMonsNo() == MONSNO_PURIZUMU &&
        pp->GetFormNo() == FORMNO_PURIZUMU_FORM_N )
    {
      pp->ChangeFormNo( FORMNO_PURIZUMU_NORMAL );
    }
  }
}


}  //namespace form 
} //namespace PokeTool

