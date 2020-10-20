//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
*	GAME FREAK inc.
*
*	@file		FieldSodateya.cpp
*	@brief  育て屋チェック
*	@author iwasawa
*	@date		2012.10.02
* @date   2015.12.12 niji用に移植 onoue_masayuki
*
*/
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "Field/FieldStatic/include/Sodateya/FieldSodateya.h"

// poke_memop
#include "PokeTool/include/PokeMemo.h"

// poke_tool
#include "PokeTool/include/PokeToolForm.h"
#include "PokeTool/include/PokeToolPokeMemory.h"
#include "PokeTool/include/PokeToolPokePlace.h"

// SodateyaLoveLevel
#include "niji_reference_files/script/FieldPawnTypes.h"

namespace Field
{

  //-----------------------------------------------------------------------------
  /**
  *					定数宣言
  */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  /**
  *					クラス宣言
  */
  //-----------------------------------------------------------------------------

  //-----------------------------------------------------------------------------
  /**
  *					実装
  */
  //-----------------------------------------------------------------------------

  // コンストラクタ
  FieldSodateya::FieldSodateya(Fieldmap* p_fieldmap) :
    m_pFieldmap(p_fieldmap)
  {
    m_pGameManager = m_pFieldmap->GetGameManager();
    m_pGameData    = m_pGameManager->GetGameData();
    m_pSodateyaSave = m_pGameData->GetSodateyaSave();
    m_heap = gfl2::heap::Manager::GetHeapByHeapId( HEAPID_EVENT )->GetLowerHandle();
  }

  // デストラクタ
  FieldSodateya::~FieldSodateya()
  {
  }

  //========================================================================================
  // ■両親の相性チェック
  //========================================================================================

  //----------------------------------------------------------------------------------------
  /**
  * @brief 預けてある二匹の相性をチェック
  */
  //----------------------------------------------------------------------------------------
  SodateyaLoveLevel FieldSodateya::LoveCheck( Savedata::SodateyaID id )
  {
    SodateyaLoveLevel level = SODATEYA_LOVE_WORST;
    if( m_pSodateyaSave->PokeNumGet( id ) < Savedata::SODATEYA_POKE_MAX ){
      return SODATEYA_LOVE_WORST; //2匹いない
    }
    pml::pokepara::PokemonParam* pp1 = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );
    pml::pokepara::PokemonParam* pp2 = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );

    //育て屋ポケモンパラメータ取得
    m_pSodateyaSave->PokemonParamGet( id, 0, pp1 );
    m_pSodateyaSave->PokemonParamGet( id, 1, pp2 );

    level = this->LoveCheckCore( pp1, pp2 );

    GFL_SAFE_DELETE( pp2);
    GFL_SAFE_DELETE( pp1);
    return level;
  }

  //相性チェックコア private
  SodateyaLoveLevel FieldSodateya::LoveCheckCore( pml::pokepara::PokemonParam* pp1,pml::pokepara::PokemonParam* pp2 )
  {
    int i,j;
    LoveCheckParam  prm[Savedata::SODATEYA_POKE_MAX];

    prm[0].pp = pp1;
    prm[1].pp = pp2;

    //まずは必要なパラメータ取得
    for( i = 0;i < Savedata::SODATEYA_POKE_MAX;++i){
      FormNo  form  = prm[i].pp->GetFormNo();
      prm[i].monsno = prm[i].pp->GetMonsNo();
      prm[i].id     = prm[i].pp->GetID();
      prm[i].sex    = prm[i].pp->GetSex();

      pml::personal::LoadPersonalData( prm[i].monsno, form );
      prm[i].group[0] = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_egg_group1 );
      prm[i].group[1] = pml::personal::GetPersonalParam( pml::personal::PARAM_ID_egg_group2 );
    }

    //グループ無生殖チェック
    for( i = 0; i < Savedata::SODATEYA_POKE_MAX; ++i ){
      //ひとつでも「無生殖」ならタマゴはできない
      for( j = 0; j < EGG_GROUP_MAX; ++j ){
        if( prm[i].group[j] == pml::personal::EGG_GROUP_MUSEISYOKU ) return SODATEYA_LOVE_WORST;
      }
    }

    //==========================
    //メタモンチェック
    if( prm[0].monsno == MONSNO_METAMON &&
      prm[1].monsno == MONSNO_METAMON ){
        //両方メタモンは×
        return SODATEYA_LOVE_WORST;
    }
    else if( prm[0].monsno == MONSNO_METAMON ||
      prm[1].monsno == MONSNO_METAMON ){
        //片方メタモンなら普通
        return this->LoveCheckCoreSame( prm );
    }
    //==========================
    //性別チェック
    if( prm[0].sex == prm[1].sex ||
      prm[0].sex == pml::SEX_UNKNOWN ||
      prm[1].sex == pml::SEX_UNKNOWN ){
        //同性または片方が性別なしだと×
        return SODATEYA_LOVE_WORST;
    }

    //==========================
    //無生殖以外のグループチェック
    if( prm[0].group[0] == prm[1].group[0] ||
      prm[0].group[0] == prm[1].group[1] ||
      prm[0].group[1] == prm[1].group[0] ||
      prm[0].group[1] == prm[1].group[1]){

        //いずれかのグループが合致すればOK
        return this->LoveCheckCoreSame( prm );
    }
    return SODATEYA_LOVE_WORST;
  }

  //----------------------------------------------------------------------------------------
  /**
  * @brief 指定したポケモンの相性を求める (同じタマゴグループの場合)
  *
  * [ルール]
  *  同種 and 異ID then ◎
  *  同種 and 同ID then ○
  *  異種 and 異ID then ○
  　 *  異種 and 同ID then △
    *
    * @return 相性
    *
    * enum SodateyaLoveLevel{
    *   SODATEYA_LOVE_GOOD,   //「とっても良いようじゃ」
    *   SODATEYA_LOVE_NORMAL, //「まずまずのようじゃ」
    *   SODATEYA_LOVE_BAD,    //「それほど　良くはないがなぁ」
    *   SODATEYA_LOVE_WORST,  //「違うポケモンと遊んでおるがなぁ」
    * };
    */
  //----------------------------------------------------------------------------------------
  SodateyaLoveLevel FieldSodateya::LoveCheckCoreSame( LoveCheckParam* prm )
  {
    int diff = 0;

    //ポケモンの種類が異なると相性ダウン
    diff += ( prm[0].monsno != prm[1].monsno );
    //ポケモンのIDが同じだと相性ダウン
    diff += ( prm[0].id == prm[1].id );

    return static_cast<SodateyaLoveLevel>(SODATEYA_LOVE_GOOD+diff);
  }

  //========================================================================================
  // ■タマゴの受け渡し
  //========================================================================================

  //----------------------------------------------------------------------------------------
  /**
  * @brief タマゴを生成して渡す(PokemonParamを生成して返すので呼び出し元で開放すること)
  * 
  * @retval NULL 受け取れなかった
  * @retval PokemonParam*
  * 
  */
  //----------------------------------------------------------------------------------------
  pml::pokepara::PokemonParam* FieldSodateya::EggAdopt( Savedata::SodateyaID id )
  {
    pml::PokeParty* party = m_pGameData->GetPlayerParty();
    pml::pokepara::PokemonParam* pp_egg = NULL;

    if( (party->GetMemberCount() >= pml::PokeParty::MAX_MEMBERS) &&
        (m_pGameData->GetBoxPokemon()->GetSpaceCountAll( m_heap )==0)){
      GFL_ASSERT(0);
      return NULL; //手持ちもボックスもいっぱいで受け取れない
    }
    pml::pokepara::PokemonParam* pp1 = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );
    pml::pokepara::PokemonParam* pp2 = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );

    //育て屋ポケモンパラメータ取得
    m_pSodateyaSave->PokemonParamGet( id, 0, pp1 );
    m_pSodateyaSave->PokemonParamGet( id, 1, pp2 );

    //タマゴ生成
    {
      pml::pokepara::EggGenerator::Setting setting = {0};
      Savedata::MyStatus*  my_status  = m_pGameData->GetPlayerStatus();
      gfl2::str::StrBuf  playerName( pml::PERSON_NAME_BUFFER_SIZE, m_heap );
      my_status->GetNameString( &playerName );

      gfl2::math::Random rand;
      rand.Initialize( m_pSodateyaSave->GetEggCreateRandState( id ) );

      //タマゴ生成用パラメータ初期化
      setting.IDNo = my_status->GetID();
      setting.parentName = &playerName;
      setting.haveItem_HIKARUOMAMORI = m_pGameData->GetMyItem()->Check( ITEM_HIKARUOMAMORI, 1 );
      setting.pRandom = &rand;
      setting.realRegionFormNo = m_pGameData->GetRealRegionFormNo();

      pp_egg = pml::pokepara::EggGenerator::CreateEgg( m_heap, pp1, pp2, setting );

      // 乱数を更新してから、保存
      rand.Next();
      m_pSodateyaSave->SetEggCreateRandState( id, rand.SaveState() );

#if PM_DEBUG
      // 強制レア
      //if (FieldDebug::DebugWildEncRareForce) {
        //pp_egg->SetRare();
      //}
#endif

#if 0
      //ポケメモ設定
      PokeTool::PokeMemo::SetFromEggTaken( pp_egg, my_status, PokeTool::PokePlace::PLACE_NO_PERSON_SODATEYA, m_heap );

      //メンバに追加
      party->AddMember( *pp_egg );
#endif
      //ポケモンダイアリー生成
      PokeDiary::Set( PDID_COUNT_SODATEYA_TAMAGO );

      //タマゴ保持ワーククリア
      m_pSodateyaSave->EggClear( id );
    }
    GFL_SAFE_DELETE(pp2);
    GFL_SAFE_DELETE(pp1);
    return pp_egg;  //受け取るタマゴのパラメータ
  }

  //----------------------------------------------------------------------------------------
  /**
  * @brief 一歩ごとの育てる処理
  */
  //----------------------------------------------------------------------------------------
  void FieldSodateya::PokeBreed(void)
  {
    static const u8 per_tbl[][2] = {
      //通常確率、まるいおまもり補正
      { LOVE_LV_GOOD,   LOVE_LV_GOOD_EX },
      { LOVE_LV_NORMAL, LOVE_LV_NORMAL_EX },
      { LOVE_LV_BAD,    LOVE_LV_BAD_EX },
      { LOVE_LV_WORST,  LOVE_LV_WORST },
    };

    //産卵チェック歩数チェック
    if( m_pGameData->SodateyaWalkCounterInc() <= EGG_CHECK_INTERVAL ){
      return; //歩数がまだ
    }

    //歩数カウンターリセット
    m_pGameData->SodateyaWalkCounterReset();
    u32 patten = m_pGameData->GetMyItem()->Check( ITEM_MARUIOMAMORI, 1 );

    for( int i = 0; i < Savedata::SODATEYA_ID_MAX; i++ ){
      //もうある
      if( m_pSodateyaSave->IsEggExist( static_cast<Savedata::SodateyaID>(i) ) != false) {
        continue;
      }

      //100基準でタマゴ発生チェック
      SodateyaLoveLevel level = this->LoveCheck( static_cast<Savedata::SodateyaID>(i) );

      if( System::GflUse::GetPublicRand( 100 ) < per_tbl[level][patten] ){
        m_pSodateyaSave->EggSet( static_cast<Savedata::SodateyaID>(i) );
      }
    }
  }

  //========================================================================================
  // ■ポケモンの受け渡し
  //========================================================================================
  //----------------------------------------------------------------------------------------
  /**
  * @brief ポケモンを預ける
  *
  * @param party_idx 預けるポケモンのindex
  */
  //----------------------------------------------------------------------------------------
  void FieldSodateya::PokeLeave( Savedata::SodateyaID id, u32 party_pos )
  {
    pml::PokeParty* party = m_pGameData->GetPlayerParty();
    pml::pokepara::PokemonParam* pp = party->GetMemberPointer( party_pos );
    int idx = m_pSodateyaSave->PokeNumGet( id );

    //全快させる
    pp->RecoverAll();

    //シェイミのフォルムをもとに戻す処理
    if( pp->GetMonsNo() == MONSNO_SHEIMI ){
      pp->ChangeFormNo( FORMNO_SHEIMI_LAND );
    }
    //トリミアンのフォルムをもとに戻す処理
    if( pp->GetMonsNo() == MONSNO_TORIMIAN ){
      PokeTool::form::SetPoodleForm( pp, FORMNO_TORIMIAN_NORMAL );
    }
    //フーパのフォルムをもとに戻す処理
    if( pp->GetMonsNo() == MONSNO_HUUPA ){
      PokeTool::form::SetHuupaForm( pp, FORMNO_HUUPA_NORMAL );
    }

    //ポケモンダイアリー生成
    //PokeDiary::Set( PDID_COUNT_SODATEYA_LEAVE, pp->GetMonsNo(), pp->GetLevel() );//PDID_COUNT_SODATEYA_LEAVEは削除された

    if( idx > 0 ){
      //他に預けられているポケモンがいる場合に「思い出」作成

      //預け済みのもう一匹のパラメータ取得
      pml::pokepara::PokemonParam* pp1 = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );
      m_pSodateyaSave->PokemonParamGet( id, 0, pp1 );

      //新たに預けるポケモンに思い出セット
      //PokeTool::PokeMemory::Set( PokeTool::PokeMemory::EVENT_SODATEYA, pp1->GetMonsNo(), pp, m_pGameData );

      { //BTS[1592]：預け済みの一匹に思い出をセットして、セーブ枠のポケモンパラメータのみを上書き
        //PokeTool::PokeMemory::Set( PokeTool::PokeMemory::EVENT_SODATEYA, pp->GetMonsNo(), pp1, m_pGameData );
        m_pSodateyaSave->PokemonLeave( id, 0, pp1 );
      }

      GFL_SAFE_DELETE( pp1);
    }
    m_pSodateyaSave->PokemonLeave( id, idx, party->GetMemberPointer(party_pos) );

    //預けたポケモンを手持ちから削除
    party->RemoveMember( party_pos );

    //レコード追加
    Savedata::IncRecord(Savedata::Record::RECID_SODATEYA_CNT);
  }

  //----------------------------------------------------------------------------------------
  /**
  * @brief ポケモンを引き取る
  *
  * @param idx 引き取るポケモンのindex
  */
  //----------------------------------------------------------------------------------------
  void FieldSodateya::PokeTakeBack( Savedata::SodateyaID id, u32 idx )
  {
    pml::PokeParty* party = m_pGameData->GetPlayerParty();
    pml::pokepara::PokemonParam* pp = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );
    int before_lv = 0;

    //育て屋ワークからデータを抜き出して削除
    m_pSodateyaSave->PokemonTakeBack( id, idx, pp );

    //預けていたポケモンを手持ちに戻す
    party->AddMember( *pp );

    //成長の結果ケルディオのしんぴの剣が消えたかもしれないのでチェック
    PokeTool::form::UpdateKerudhioForm( m_pGameData );

    //シェイミやケルディオなど、預けた結果フォルムが変わったかもしれないので図鑑登録
    m_pGameData->GetZukanData()->SetPokeGet( *pp );

    //ポケモンダイアリー生成
    //pokediary::Set( PDID_COUNT_SODATEYA_RECEIVE, pp->GetMonsNo(), after_lv, (after_lv-before_lv) );

    GFL_SAFE_DELETE( pp);
  }

  //----------------------------------------------------------------------------------------
  /**
  * @brief 預けているポケモンの引き取りメニューリスト用ワードセット
  *
  * BTS[996] ニックネーム付き二ドラン♂♀の表記修正
  */
  //----------------------------------------------------------------------------------------
  pml::Sex FieldSodateya::PokeTakeBackMenuWordset( print::WordSet* wset, u32 buf_id, Savedata::SodateyaID id, u32 idx )
  {
    // [fix]GFCHECK[8230] 未使用変数を削除
    pml::pokepara::PokemonParam* pp = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );
    u8  level = 0;
    pml::Sex sex;

    //育て屋ワークからデータを抜き出す
    m_pSodateyaSave->PokemonParamGet( id, idx, pp );
    level = pp->GetLevel();
    sex = pp->GetSex();

    //名前
    wset->RegisterPokeNickName( buf_id, pp );
    //レベル
    wset->RegisterNumber( buf_id+1, level, 3, print::NUM_DISP_LEFT, print::NUM_CODE_DEFAULT );
    //表示の性別コードを決定
    sex = PokeTool::GetDrawSex( pp );
    GFL_SAFE_DELETE( pp );

    return sex;
  }

  //----------------------------------------------------------------------------------------
  /**
  * @brief 預けているポケモンのニックネームをワードセット展開
  */
  //----------------------------------------------------------------------------------------
  void FieldSodateya::PokeNameWordset( print::WordSet* wset, u32 buf_id, Savedata::SodateyaID id, u32 idx )
  {
    pml::pokepara::PokemonParam* pp = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );

    //育て屋ワークからデータを抜き出す
    m_pSodateyaSave->PokemonParamGet( id, idx, pp );
    wset->RegisterPokeNickName( buf_id, pp );

    GFL_SAFE_DELETE( pp );
  }

  //==================================================================
  //非公開関数群
  //==================================================================

  //====================================================================
  //以下デバッグ用関数群
#if PM_DEBUG

  //----------------------------------------------------------------------------------------
  /**
  * @brief デバッグ：育て屋ポケモンパラメータ取得(使い終わったら呼び出し側がDeleteすること)
  *
  * @param   make_code DEBUG_PP_MAKE_POKE1他
  * @return  取得したポケモンパラメータ
  */
  //----------------------------------------------------------------------------------------
  pml::pokepara::PokemonParam* FieldSodateya::DebugPokeParaMake( Savedata::SodateyaID id, int make_code, gfl2::heap::HeapBase* m_heap )
  {
    int poke_num = m_pSodateyaSave->PokeNumGet( id );

    if( make_code  < DEBUG_PP_MAKE_EGG ){

      if( poke_num <= make_code ){
        return NULL;
      }
      pml::pokepara::PokemonParam* pp = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );

      //育て屋ポケモンパラメータ取得
      m_pSodateyaSave->PokemonParamGet( id, make_code, pp );
      return pp;
    }
    //タマゴ生成モード
    if( this->LoveCheck( id ) == SODATEYA_LOVE_WORST ){
      return NULL;
    }
    //タマゴ生成
    {
      pml::pokepara::EggGenerator::Setting setting = {0};
      pml::pokepara::PokemonParam* pp_egg = NULL;
      Savedata::MyStatus*  my_status  = m_pGameData->GetPlayerStatus();
      gfl2::str::StrBuf  playerName( pml::PERSON_NAME_BUFFER_SIZE, m_heap );
      my_status->GetNameString( &playerName );

      gfl2::math::Random rand;
      rand.Initialize( m_pSodateyaSave->GetEggCreateRandState( id ) );

      pml::pokepara::PokemonParam* pp1 = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );
      pml::pokepara::PokemonParam* pp2 = GFL_NEW( m_heap ) pml::pokepara::PokemonParam( m_heap );

      //育て屋ポケモンパラメータ取得
      m_pSodateyaSave->PokemonParamGet( id, 0, pp1 );
      m_pSodateyaSave->PokemonParamGet( id, 1, pp2 );

      //タマゴ生成用パラメータ初期化
      setting.IDNo = my_status->GetID();
      setting.parentName = &playerName;
      setting.haveItem_HIKARUOMAMORI = m_pGameData->GetMyItem()->Check( ITEM_HIKARUOMAMORI, 1 );
      setting.pRandom = &rand;
      setting.realRegionFormNo = m_pGameData->GetRealRegionFormNo();

      pp_egg = pml::pokepara::EggGenerator::CreateEgg( m_heap, pp1, pp2, setting );
      
      // 乱数を更新してから、保存
      rand.Next();
      m_pSodateyaSave->SetEggCreateRandState( id, rand.SaveState() );

#if PM_DEBUG
      // 強制レア
      //if (FieldDebug::DebugWildEncRareForce) {
        //pp_egg->SetRare();
      //}
#endif

      //ポケメモ設定
      PokeTool::PokeMemo::SetFromEggTaken( pp_egg, my_status, PokeTool::PokePlace::PLACE_NO_PERSON_SODATEYA, m_heap );

      GFL_SAFE_DELETE( pp2 );
      GFL_SAFE_DELETE( pp1);
      return pp_egg;
    }
  }

  //----------------------------------------------------------------------------------------
  /**
  * @brief デバッグ：育て屋ポケモンパラメータ書き戻し
  *
  * @param   make_code DEBUG_PP_MAKE_POKE1他
  * @return  セットできたか
  */
  //----------------------------------------------------------------------------------------
  bool FieldSodateya::DebugPokeParaSet( Savedata::SodateyaID id, int make_code, pml::pokepara::PokemonParam* pp )
  {
    int poke_num = m_pSodateyaSave->PokeNumGet( id );

    if( make_code  < DEBUG_PP_MAKE_EGG ){
      if( poke_num <= make_code ){
        return false;
      }
      //全快させる
      pp->RecoverAll();

      //シェイミのフォルムをもとに戻す処理
      if( pp->GetMonsNo() == MONSNO_SHEIMI ){
        pp->ChangeFormNo( FORMNO_SHEIMI_LAND );
      }
      //トリミアンのフォルムをもとに戻す処理
      if( pp->GetMonsNo() == MONSNO_TORIMIAN ){
        pp->ChangeFormNo( FORMNO_TORIMIAN_NORMAL );
      }
      //フーパのフォルムをもとに戻す処理
      if( pp->GetMonsNo() == MONSNO_HUUPA ){
        PokeTool::form::SetHuupaForm( pp, FORMNO_HUUPA_NORMAL );
      }
      m_pSodateyaSave->PokemonLeave( id, make_code, pp );
      return true;
    }
    //タマゴモード
    return false;
  }

#endif  //PM_DEBUG
  //ここまでデバッグ用関数群
  //====================================================================

} // namespace field
