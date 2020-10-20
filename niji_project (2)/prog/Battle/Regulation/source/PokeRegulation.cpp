//==============================================================================
/**
 * @file  PokeRegulation.cpp
 * @brief ポケモンのレギュレーション検査用プログラム
 * @author  ichiraku_katsuhiko
 * @data  2015/07/21, 21:21
 */
// =============================================================================
#include <debug/include/gfl2_DebugPrint.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include <thread/include/gfl2_ThreadStatic.h>

// mine
#include "Battle/Regulation/include/PokeRegulation.h"
#include "Battle/Regulation/include/RegulationScriptChecker.h"

// validation
#include "NetStatic/NetLib/include/Validation/PokemonValidation.h"

//==========================================================================
//=========================================================================

//GFL_NAMESPACE_BEGIN( Battle )

// コンストラクタ
PokeRegulation::PokeRegulation( gfl2::heap::HeapBase* pHeap ) :
    m_pHeap( pHeap )
   ,m_pRegulation( NULL )
{
}

// デストラクタ
PokeRegulation::~PokeRegulation( void )
{
}

//--------------------------------------------------------------------------
/**
 * @brief   伝説ポケモンかどうかチェック
 * @param   monsno  モンスター番号
 * @param   formno  フォルム番号
 * @retval true:伝説
 * @retval  false:非伝説
 */
//--------------------------------------------------------------------------
bool PokeRegulation::CheckLegend( u16 monsno, u8 formno )
{
  static const u16 legend_monsno[] = {
    MONSNO_MYUUTUU,   // ミュウツー
    MONSNO_MYUU,      // ミュウ
    MONSNO_RUGIA,     // ルギア
    MONSNO_HOUOU,     // ホウオウ
    MONSNO_SEREBHI,   // セレビィ
    MONSNO_KAIOOGA,   // カイオーガ
    MONSNO_GURAADON,  // グラードン
    MONSNO_REKKUUZA,  // レックウザ
    MONSNO_ZIRAATI,   // ジラーチ
    MONSNO_DEOKISISU, // デオキシス
    MONSNO_DHIARUGA,  // ディアルガ
    MONSNO_PARUKIA,   // パルキア
    MONSNO_GIRATHINA, // ギラティナ
    MONSNO_FIONE,     // フィオネ
    MONSNO_MANAFI,    // マナフィ
    MONSNO_DAAKURAI,  // ダークライ
    MONSNO_SHEIMI,    // シェイミ
    MONSNO_ARUSEUSU,  // アルセウス
    MONSNO_BIKUTHINI, // ビクティニ
    MONSNO_RESIRAMU,  // レシラム
    MONSNO_ZEKUROMU,  // ゼクロム
    MONSNO_KYUREMU,   // キュレム
    MONSNO_KERUDHIO,  // ケルディオ
    MONSNO_MEROETTA,  // メロエッタ
    MONSNO_GENOSEKUTO,// ゲノセクト
    MONSNO_ZERUNEASU, // エックス
    MONSNO_IBERUTARU, // ワイ
    MONSNO_ZIGARUDE,  // ゼット
    MONSNO_DHIANSII,  // ピンクダイヤ
    MONSNO_HUUPA,     // スチーム
    MONSNO_BORUKENION,// マジン
    // フラエッテ破壊フォルム
    MONSNO_BEBII,     // ベビー
    MONSNO_MAYU,      // まゆ
    MONSNO_NIKKOU,    // にっこう
    MONSNO_GEKKOU,    // げっこう
    MONSNO_PURIZUMU,  // プリズムドラゴン（アッパー伝説）
    MONSNO_MABOROSI16,// マボロシ16
    MONSNO_MABOROSI17,// マボロシ17

    // momiji追加
    MONSNO_MABOROSI18,// マボロシ17
  };

  // フラエッテのみフォルムで伝説になるので例外
  if( monsno == MONSNO_HURAETTE )
  {
    // 破壊フォルム
    if( formno == FORMNO_HURAETTE_HAKAI	){
      ICHI_PRINT("フラエッテ破壊フォルムだった\n");
      return true;
    }else{
      return false;
    }
  }

  for( int i=0; i<GFL_NELEMS(legend_monsno); i++ )
  {
    if( legend_monsno[i] == monsno )
    {
      ICHI_PRINT("伝説ポケモンだった %d %d \n", i, monsno);
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief   準伝説ポケモンかどうかチェック
 * @param   monsno  モンスター番号
 * @retval  true:準伝説
 * @retval  false:非準伝説
 */
//--------------------------------------------------------------------------
bool PokeRegulation::CheckSubLegend( u16 monsno )
{
  static const u16 sub_legend_monsno[] = {
    MONSNO_HURIIZAA,    // フリーザー
    MONSNO_SANDAA,      // サンダー
    MONSNO_FAIYAA,      // ファイアー
    MONSNO_RAIKOU,      // ライコウ
    MONSNO_ENTEI,       // エンテイ
    MONSNO_SUIKUN,      // スイクン
    MONSNO_REZIROKKU,   // レジロック
    MONSNO_REZIAISU,    // レジアイス
    MONSNO_REZISUTIRU,  // レジスチル
    MONSNO_RATHIASU,    // ラティアス
    MONSNO_RATHIOSU,    // ラティオス
    MONSNO_YUKUSII,     // ユクシー
    MONSNO_EMURITTO,    // エムリット
    MONSNO_AGUNOMU,     // アグノム
    MONSNO_HIIDORAN,    // ヒードラン
    MONSNO_REZIGIGASU,  // レジギガス
    MONSNO_KURESERIA,   // クレセリア
    MONSNO_KOBARUON,    // コバルオン
    MONSNO_TERAKION,    // テラキオン
    MONSNO_BIRIZION,    // ビリジオン
    MONSNO_TORUNEROSU,  // トルネロス
    MONSNO_BORUTOROSU,  // ボルトロス
    MONSNO_RANDOROSU,   // ランドロス
    MONSNO_GURIPUSU1,   // タイプ：ヌル
    MONSNO_GURIPUSU2,   // シルヴィディ
    MONSNO_THIKIUSI,    // ティキうし
    MONSNO_THIKISAKANA, // ティキさかな
    MONSNO_THIKITORI,   // ティキとり
    MONSNO_THIKITYOU,   // ティキちょう
    MONSNO_BIISUTOr,    // ウルトラビースト・クラゲ・代表に寄生
    MONSNO_BIISUTOy,    // ウルトラビースト・黄・マッスルモスキート
    MONSNO_BIISUTOb,    // ウルトラビースト・超綺麗なゴキブリ
    MONSNO_BIISUTOg,    // ウルトラビースト・緑・イルミネーション
    MONSNO_BIISUTOp,    // ウルトラビースト・紫
    MONSNO_BIISUTOi,    // ウルトラビースト・藍・紙
    MONSNO_BIISUTOo,    // レインボービースト・橙・クライマクス・グリードラ

    // momiji追加
    MONSNO_BIISUTOd1,   // ベベノム
    MONSNO_BIISUTOd2,   // アーゴヨン
    MONSNO_BIISUTOs,    // ツンデツンデ
    MONSNO_BIISUTOh,    // ズガドーン
  };

  for( int i=0; i<GFL_NELEMS(sub_legend_monsno); i++ )
  {
    if( sub_legend_monsno[i] == monsno )
    {
      ICHI_PRINT("準伝説ポケモンだった %d %d \n", i, monsno);
      return true;
    }
  }
  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 指定したレベルにポケモンパラメータを補正する
 * @param pPokePara   対象ポケモン
 * @param level       補正レベル
 */
//--------------------------------------------------------------------------
void PokeRegulation::MakeLevelRevise( pml::pokepara::PokemonParam *pPokePara, u32 level )
{
  MonsNo monsno = pPokePara->GetMonsNo();
  FormNo formno = pPokePara->GetFormNo();
  pml::personal::LoadGrowTable( monsno, formno );
  u32 exp = pml::personal::GetMinExp( level );
  pPokePara->SetExp( exp );
}

//--------------------------------------------------------------------------
/**
 * @brief   ポケパーティをレギュレーションにそったレベル補正をかける
 * @param   pPokeParty   ポケパーティ
 */
//--------------------------------------------------------------------------
void PokeRegulation::PokePartyLevelRevise( pml::PokeParty *pPokeParty, u32 maxLevel )
{
  // ポケパーティ数分
  for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
  {
    pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );

    // タマゴでない場合のみ処理
    if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
    {
      const u32 level = pp->GetLevel();

      // 以上補正なので、補正レベル以上なら補正レベルにする
      if( level > maxLevel )
      {
        ICHI_PRINT("レベル %dに補正\n", maxLevel );
        // 指定したレベルにパラメータを補正する
        MakeLevelRevise( pp, maxLevel );
      }
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief niji互換対応ポケモンチェック（=momiji追加なのでnijiに送ってはいけない）
 * @param monsno   対象ポケモンのMonsNo
 * @param formno   対象ポケモンのFormNo
 * @retval  true:互換対応ポケモンである
 * @retval  false:互換対応ポケモンじゃない
 */
//--------------------------------------------------------------------------
bool PokeRegulation::CheckNijiCompatibleMonsNo( u16 monsNo, u8 formNo )
{
  // momiji追加ポケモンチェック
  static const u32 momiji_new_monsno[] = {
    MONSNO_BIISUTOd1,     // 毒ビースト1
    MONSNO_BIISUTOd2,     // 毒ビースト2
    MONSNO_BIISUTOs,      // sun用ビースト：イシガキ
    MONSNO_BIISUTOh,      // moon用ビースト：ハナビ
    MONSNO_MABOROSI18,    // M21。映画配信用。
  };

  for( int i = 0; i < GFL_NELEMS( momiji_new_monsno ); i++ )
  {
    if( momiji_new_monsno[i] == monsNo )
    {
      GFL_PRINT("momiji新ポケだった %d monsno:%d \n", i, monsNo);
      return true;
    }
  }

  // momiji追加フォルムチェック
  // ネクロズマのフォルムが３つ追加された
  if( monsNo == MONSNO_PURIZUMU )
  {
    if( formNo == FORMNO_PURIZUMU_FORM_S ||
        formNo == FORMNO_PURIZUMU_FORM_L ||
        formNo == FORMNO_PURIZUMU_FORM_N )
    {
      GFL_PRINT("momiji追加ネクロズマ monsno:%d formno:%d \n", monsNo, formNo);
      return true;
    }
  }
  // 2017映画配信ピカチュウ
  if( monsNo == MONSNO_PIKATYUU && formNo == FORMNO_PIKATYUU_SATOPIKA20 ) { return true; }
  // アブリボン　ヌシフォルム
  if( monsNo == MONSNO_ABU2 && formNo == FORMNO_ABU2_NUSHI ) { return true; }
  // オニシズクモ　ヌシフォルム
  if( monsNo == MONSNO_MIZUGUMO2 && formNo == FORMNO_MIZUGUMO2_NUSHI ) { return true; }
  // トゲデマル　ヌシフォルム
  if( monsNo == MONSNO_NIZITYUU && formNo == FORMNO_NIZITYUU_NUSHI ) { return true; }
  // アローラガラガラ　ヌシフォルム
  if( monsNo == MONSNO_GARAGARA && formNo == FORMNO_GARAGARA_NUSHI ) { return true; }
  // イワンコ　新フォルム
  if( monsNo == MONSNO_OOKAMI1 && formNo == FORMNO_OOKAMI1_TWILIGHT ) { return true; }
  // ルガルガン　新フォルム
  if( monsNo == MONSNO_OOKAMI2 && formNo == FORMNO_OOKAMI2_TWILIGHT ) { return true; }

  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief niji互換対応ポケモンチェック（=momiji追加なのでnijiに送ってはいけない）
 * @param pPokePara   対象ポケモン
 * @retval  true:互換対応ポケモンである
 * @retval  false:互換対応ポケモンじゃない
 */
//--------------------------------------------------------------------------
bool PokeRegulation::CheckNijiCompatible( pml::pokepara::CoreParam *pPokePara )
{
  // // タマゴチェック。手持ちだとありえる。
  // if( pPokePara->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == true )
  //{
  //   return false; // タマゴは互換対応ポケモンではない
  //}
  //
  // タマゴも中を見て弾く必要がある。

  //====================================================
  // 
  // ポケモンチェック
  //
  //====================================================
  if( CheckNijiCompatibleMonsNo( pPokePara->GetMonsNo(), pPokePara->GetFormNo() ) )
  {
    return true;
  }
  

  //====================================================
  // 
  // 新技チェック
  //
  //====================================================
  // nijiの技番号MAX以上があるかどうか
  for( u8 i=0; i<pPokePara->GetWazaCount(); i++ )
  {
    u32 waza = static_cast<u32>(pPokePara->GetWazaNo(i));
    if( waza > WAZANO_SATOPIKASENYOUZENRYOKU )
    {
      GFL_PRINT("momiji新わざだった %d\n", waza);
      return true;
    }
  }
  

  //====================================================
  // 
  // 新アイテムチェック
  //
  //====================================================
  if( pPokePara->GetItem() > ITEM_FEARIIMEMORI )
  {
    GFL_PRINT( "momiji新アイテムだった %d\n", pPokePara->GetItem() );
    return true;
  }

  return false;
}

//--------------------------------------------------------------------------
/**
 * @brief 検査用のレギュレーションをセット
 * @param   pRegulation レギュレーション
 */
//--------------------------------------------------------------------------
void PokeRegulation::SetRegulation( Regulation *pRegulation )
{
  if( m_pRegulation != NULL )
  {
    GFL_ASSERT_MSG( 0, "既にレギュレーションデータが存在します！"); // @check
    return;
  }
  m_pRegulation = pRegulation;
}

/**
 * @brief バトルチーム用のポケパーティ検査
 * @param[in]   pPokeParty        検査するポケモンパーティ
 * @param[out]  violationDetails  違反内容詳細
 * @retval true:正常
 * @retval false:異常 (違反内容詳細はviolationDetailsに格納されている）
 */
bool PokeRegulation::CheckPokePartyForBtlTeam( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails )
{
  gfl2::std::MemClear( &violationDetails, sizeof(ViolationDetails) ); 

  // レギュレーションがないので不明なエラー
  if( m_pRegulation == NULL )
  {
    GFL_ASSERT_MSG(0, "レギュレーションがNULL"); // @check
    // 適当なメッセージを出す為立てておく
    violationDetails.isViolationExScript = true;
    return false;
  }

  // 処理上タマゴがいないパーティの方が都合がよいので、そうする
  pml::PokeParty* tmpParty = GFL_NEW( m_pHeap->GetLowerHandle() ) pml::PokeParty( m_pHeap->GetLowerHandle() );
  tmpParty->Clear();
  for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
  {
    pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );
    // タマゴでないなら追加
    if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
    {
      tmpParty->AddMember( *pp );
    }
  }

  // memo:バトルチームでチェックするパラメータ
  /*
   * バトルチーム下限
   * バトルチーム上限
   * 参加レベル
   * 同じポケモン
   * 同じアイテム
   * 禁止ポケモン
   * 禁止破滅の花のフラエッテ
   * 禁止特定フォルムポケモン
   * 禁止わざ
   * 禁止どうぐ
   * 必須ポケモン（いるかどうかだけ）
   * 産地限定
   * QRバトルチーム
   * 拡張スクリプト
   *
   * ※タマゴはチェックしてはいけない（手持ちにはいるので）
   *   ↑の処理でタマゴを省く事にした
   */

  // パーティ検査
  bool bCheckPokePartyResult = CheckPokeParty( pPokeParty, tmpParty, violationDetails );



  // 拡張レギュレーションチェック
  bool bCheckPokePartyExScriptResult = CheckPokePartyExScript( tmpParty, violationDetails, RegulationScriptChecker::MODE_BATTLE_TEAM );

  GFL_DELETE tmpParty;

  //ICHI_PRINT("CheckPokePartyForBtlTeam retBit %x\n", ret);

  bool ret = ( bCheckPokePartyResult && bCheckPokePartyExScriptResult ) ? true : false;

  return ret;
}

/**
 * @brief 参加選択用のポケパーティ検査
 * @param[in]   pPokeParty   検査するポケモンパーティ
 * @return 参加ポケモン選択画面のチェックの検査戻り値
 */
PokeRegulation::PokelistCheckReturn PokeRegulation::CheckPokePartyForPokelist( pml::PokeParty* pPokeParty )
{
  // memo:参加選択でチェックするパラメータ
  /*
   * 選出ポケモン下限
   * 選出ポケモン上限
   * 伝説参加上限
   * 必須ポケモン（最終チェック）
   * 拡張スクリプト
   *
   * タマゴ（あり得ないが不正対策としてチェックする必須がある。）
   */

  // レギュレーションがないので不明なエラー
  if( m_pRegulation == NULL )
  {
    GFL_ASSERT_MSG(0, "レギュレーションがNULL"); // @check
    // 適当なメッセージを出す為立てておく
    return RET_EX_SCRIPT;
  }

  // 選出ポケモン上限チェック
  if( pPokeParty->GetMemberCount() > m_pRegulation->GetValueParam( Regulation::SELECT_NUM_HI ) )
  {
    GFL_PRINT("CheckPokePartyForPokelist RET_NUM_FAILED \n");
    return RET_NUM_FAILED;
  }
  // 選出ポケモン下限チェック
  if( pPokeParty->GetMemberCount() < m_pRegulation->GetValueParam( Regulation::SELECT_NUM_LO ) )
  {
    GFL_PRINT("CheckPokePartyForPokelist RET_NUM_FAILED \n");
    return RET_NUM_FAILED;
  }
  //// 伝説参加上限チェック
  //if( CheckLegendNum( pPokeParty ) )
  //{
  //  GFL_PRINT("CheckPokePartyForPokelist RET_OVER_LEGEND \n");
  //  return RET_OVER_LEGEND;
  //}

  // 必須ポケモンチェック
  if( CheckMustPokeForParty( pPokeParty ) == false )
  {
    GFL_PRINT("CheckPokePartyForPokelist RET_NO_MASTPOKE \n");
    return RET_NO_MASTPOKE;
  }

  // タマゴチェック
  if( pPokeParty->GetMemberCountEx( pml::PokeParty::POKE_COUNT_TYPE_BOTH_EGG ) > 0 )
  {
    // エラーメッセージが用意されていないので（仕様ではあり得ない為）、適当な戻り値返す
    GFL_PRINT("CheckPokePartyForPokelist RET_EX_SCRIPT egg \n");
    return RET_EX_SCRIPT;
  }

  // memo:必須ポケモンが解決できないので、削除。TPC様同意済み
  // 拡張レギュレーションチェック
  /*ViolationDetails result;
  if( !CheckPokePartyExScript( pPokeParty , result, RegulationScriptChecker::MODE_SELECT_POKEMON ) )
  {
    GFL_PRINT("CheckPokePartyForPokelist RET_EX_SCRIPT \n");
    return RET_EX_SCRIPT;
  }*/

  return RET_OK;
}

/**
 * @brief 参加選択の制限時間なくなった時の参加自動選択
 * @param[in]   pPokeParty   ポケモンパーティ
 * @param[in]   joinPokeIndex  参加ポケモンインデックス配列（要素：順番、値：プレートのインデックス）　-1で参加していない
 * @param[out]  retJoinPokeIndex  自動選択後の参加ポケモンインデックス配列（要素：順番、値：プレートのインデックス）
 */
void PokeRegulation::SetJoinOrderTimeOutForPokelist( pml::PokeParty* pPokeParty, const s8* joinPokeIndex, s8* retJoinPokeIndex )
{
  // レギュレーションがない
  if( m_pRegulation == NULL )
  {
    GFL_ASSERT_MSG(0, "レギュレーションがNULL"); // @check
    return;
  }

  //! 選出ポケモン下限
  const u32 selectNumLo = m_pRegulation->GetValueParam( Regulation::SELECT_NUM_LO );
  //! 選出ポケモン上限
  const u32 selectNumHi = m_pRegulation->GetValueParam( Regulation::SELECT_NUM_HI );

  ICHI_PRINT(">ICHI NumLo:%d NumHi:%d \n", selectNumLo, selectNumHi );

  // 選択パーティ
  pml::PokeParty selectParty( m_pHeap->GetLowerHandle() );

  // 一旦戻り設定
  gfl2::std::MemCopy( joinPokeIndex, retJoinPokeIndex, sizeof(s8)*pml::PokeParty::MAX_MEMBERS );

  // 選択されていたパーティを作る
  {
    selectParty.Clear();
    for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
    {
      if( joinPokeIndex[i] != -1 )
      {
        pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( joinPokeIndex[i] );
        // タマゴでない場合のみ処理
        if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
        {
          selectParty.AddMember( *pp );
        }
      }
    }
    ICHI_PRINT(">ICHI 選択済みポケモン数:%d \n", selectParty.GetMemberCount() );
  }

  // 必須ポケモン条件を満たしているかチェック
  bool isMustPokeOk = CheckMustPokeForParty( &selectParty );
    
  // 必須ポケモンが選ばれるように自動選択
  if( isMustPokeOk == false )
  {
    // 必須ポケモン
    pml::PokeParty mustParty( m_pHeap->GetLowerHandle() );
    b8 mustJoinPokeFlag[pml::PokeParty::MAX_MEMBERS];
    for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; ++i ){ mustJoinPokeFlag[i]=false; }

    // 1.追加しなければいけない必須ポケモンを洗い出す
    // 2.選択パーティに追加しなければいけない必須ポケモン数分の空きを作る
    // 3.必須ポケモンを選択パーティに追加する

    // 1.追加しなければいけない必須ポケモンを洗い出す
    {
      mustParty.Clear();

      for( u8 mustPokeIdx=0; mustPokeIdx<Regulation::MUST_POKE_NUM; mustPokeIdx++ )
      {
        u16 mustPoke     = m_pRegulation->GetParamMustPokeAND( mustPokeIdx );
        u8  mustPokeForm = m_pRegulation->GetParamMustPokeFormAND( mustPokeIdx );

        // 設定されていなければ見ない
        if( mustPoke != 0 )
        {
          // 選択パーティの中にいたら入れない
          bool isSelected = false;
          for( u8 i=0; i<selectParty.GetMemberCount(); i++ )
          {
            pml::pokepara::PokemonParam *pp = selectParty.GetMemberPointer( i );
            if( CheckMustPokeForPokeParam( pp, mustPoke, mustPokeForm ) == true )
            {
              isSelected = true;// 既に選択済み
              break;
            }
          }

          // 選択パーティにいなかったので追加
          if( isSelected == false )
          {
            // フルパーティの中にいるはずなのでコピーする
            for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
            {
              pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );
              if( CheckMustPokeForPokeParam( pp, mustPoke, mustPokeForm ) == true )
              {
                mustJoinPokeFlag[ i ] = true;
                mustParty.AddMember( *pp );
                break;
              }
            }
          }
        }

        // OR必須
        mustPoke     = m_pRegulation->GetParamMustPokeOR( mustPokeIdx );
        mustPokeForm = m_pRegulation->GetParamMustPokeFormOR( mustPokeIdx );
        if( mustPoke != 0 )
        {
          // Or必須は1匹のみでOK
          mustPoke     = m_pRegulation->GetParamMustPokeOR( mustPokeIdx );
          mustPokeForm = m_pRegulation->GetParamMustPokeFormOR( mustPokeIdx );

          // 上のCheckMustPokeForPartyに引っかからなかったので、選択パーティにはいない
          // フルパーティの中にいるはずなのでコピーする
          for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
          {
            pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );
            if( CheckMustPokeForPokeParam( pp, mustPoke, mustPokeForm ) == true )
            {
              mustJoinPokeFlag[ i ] = true;
              mustParty.AddMember( *pp );
              break;
            }
          }
          break;
        }
      } // for
      ICHI_PRINT(">ICHI 必須ポケモン数:%d \n", mustParty.GetMemberCount() );
    }

    // 2.選択パーティに追加しなければいけない必須ポケモン数分の空きを作る
    {
      // 上までの処理により選択パーティと必須パーティで被っているのはいない

      // 空きを作る必須があるかチェック
      if( (selectParty.GetMemberCount() + mustParty.GetMemberCount() ) > selectNumHi )
      {
        // 選択パーティから必須ポケモン以外を削除
        int deletePokeNum = (selectParty.GetMemberCount() + mustParty.GetMemberCount() ) - selectNumHi; 

        for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
        {
          if( retJoinPokeIndex[i] != -1 )
          {
            pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( retJoinPokeIndex[i] );
            if( IsContainMustPoke( pp ) == false )
            {
              // 参加配列から消しておく
              retJoinPokeIndex[i] = -1;

              deletePokeNum--;
              if( deletePokeNum == 0 ){ break; }
            }
          }
        }// for
      }
    }

    // 3.必須ポケモンを選択パーティに追加する
    {
      // memo:mustJoinPokeIndexにはフルパーティを回して、要素をインデックスで調べる trueで追加する必須ポケである
      for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
      {
        if( mustJoinPokeFlag[i] == true )
        {
          bool checkFlag = false;
          // 選択リストの中で空いているとこに入れる
          for( u8 j=0; j<pml::PokeParty::MAX_MEMBERS; j++ )
          {
            if( retJoinPokeIndex[j] == -1 )
            {
              retJoinPokeIndex[j] = i;
              checkFlag = true;
              break;
            }
          }
          GFL_ASSERT(checkFlag);
        }
      }
      // 中抜けあったら詰める
      for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
      {
        if( retJoinPokeIndex[i] == -1 )
        {
          for( u8 j=i; j<pml::PokeParty::MAX_MEMBERS-1; j++ )
          {
            retJoinPokeIndex[j] = retJoinPokeIndex[j+1];
          }
        }
      }
    }
  }

  // 選択済みパーティで足りない場合は、埋める
  int selectCount = 0;

  // ↓ARMCC4.1によるコンパイラ不具合により、ループ展開しています。
  if( retJoinPokeIndex[0] != -1 ) { selectCount++; }
  if( retJoinPokeIndex[1] != -1 ) { selectCount++; }
  if( retJoinPokeIndex[2] != -1 ) { selectCount++; }
  if( retJoinPokeIndex[3] != -1 ) { selectCount++; }
  if( retJoinPokeIndex[4] != -1 ) { selectCount++; }
  if( retJoinPokeIndex[5] != -1 ) { selectCount++; }

  if( selectCount < selectNumLo )
  {
    // 最低限必要な匹数
    const u8 addPokeNum = selectNumLo - selectCount;

    // フルパーティから被らないように追加
    for( u8 addCount=0; addCount<addPokeNum; addCount++ )
    {
      s8 nonSelectIndex = -1;
      for( u8 pokeIndex=0; pokeIndex<pml::PokeParty::MAX_MEMBERS; pokeIndex++ )
      {
        pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( pokeIndex );
        // タマゴでない場合のみ処理
        if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
        {
          continue;
        }

        bool isFound = false;
        for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
        {
          if( retJoinPokeIndex[i] == pokeIndex )
          {
            isFound = true;
            break;
          }
        }
        // 既に選択されているリストで見つからなかったら、被らず追加出来る
        if( isFound == false )
        {
          nonSelectIndex = pokeIndex;
          break;
        }
      }
      GFL_ASSERT(nonSelectIndex != -1);
      retJoinPokeIndex[ selectCount+addCount ] = nonSelectIndex;
    }
  }

#if PM_DEBUG
  // ホントにレギュレーション問題ないかチェック
  {
    // 選択パーティ
    pml::PokeParty checkParty( m_pHeap->GetLowerHandle() );

    // 選択されていたパーティを作る
    {
      checkParty.Clear();
      for( u8 i=0; i<pml::PokeParty::MAX_MEMBERS; i++ )
      {
        if( retJoinPokeIndex[i] != -1 )
        {
          pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( retJoinPokeIndex[i] );
          checkParty.AddMember( *pp );
        }
      }
      ICHI_PRINT(">ICHI 自動選択後のチェック \n" );
      PokelistCheckReturn ret = CheckPokePartyForPokelist( &checkParty );
      GFL_ASSERT( ret == RET_OK );
    }
  }
#endif

}

//--------------------------------------------------------------------------
/**
 * @brief   ポケパーティをレギュレーションにそったレベル補正をかける
 * @param   pPokeParty   ポケパーティ
 */
//--------------------------------------------------------------------------
void PokeRegulation::ModifyLevelPokeParty( pml::PokeParty *pPokeParty )
{
  // レギュレーションがないので不明なエラー
  if( m_pRegulation == NULL )
  {
    GFL_ASSERT_MSG(0, "レギュレーションがNULL"); // @check
    return;
  }

  // ポケパーティ数分
  for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
  {
    pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );

    // タマゴでない場合のみ処理
    if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
    {
      const u32 level = pp->GetLevel();
      const u32 modLv = ModifyLevelCalc( level );

      // 補正をかける
      if( level != modLv )
      {
        ICHI_PRINT("レベル %dに補正\n", modLv );
        // 指定したレベルにパラメータを補正する
        MakeLevelRevise( pp, modLv );
      }
    }
  }
}

//--------------------------------------------------------------------------
/**
 * @brief   ポケパーティをレギュレーションにそったニックネーム補正をかける
 * @param   pPokeParty   ポケパーティ
 */
//--------------------------------------------------------------------------
void PokeRegulation::ModifyNickName( pml::PokeParty *pPokeParty )
{
  // レギュレーションがないので不明なエラー
  if( m_pRegulation == NULL )
  {
    GFL_ASSERT_MSG(0, "レギュレーションがNULL"); // @check
    return;
  }

  if( m_pRegulation->GetBoolParam( Regulation::NICKNAME ) == false )
  {
    for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
    {
      pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );
      // タマゴでない場合のみ処理
      if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
      {
        pp->SetDefaultNickName();
      }
    }
  }
  else
  {
    // ニックネームそのままの時は、名前の不正チェック
    for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
    {
      pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );
      // タマゴでない場合のみ処理
      if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == false )
      {
#if defined(GF_PLATFORM_CTR)
        NetLib::Validation::PokemonValidation::ReplaceMonsName( pp );
#endif
      }
    }
  }
}

//==========================================================================
/**
 *    private関数
 */
//==========================================================================
/**
 * @brief ポケパーティ検査
 * @param[in]   pFullPokeParty      チェックに渡されたフルパーティ
 * @param[in]   pRejectEggPokeParty フルパーティからタマゴを除外したパーティ
 * @param[out]  violationDetails  違反内容詳細
 * @retval true:正常
 * @retval false:異常 (違反内容詳細はviolationDetailsに格納されている）
 */
bool PokeRegulation::CheckPokeParty( pml::PokeParty* pFullPokeParty, pml::PokeParty* pRejectEggPokeParty, ViolationDetails& violationDetails )
{
  // 戻り値
  bool retOk = true;

  // ポケモン単体のチェック
  if( CheckPokeParaForParty( pFullPokeParty, violationDetails ) == false )
  {// 異常
    retOk = false;
  }

  // バトルチーム上限チェック
  if( pRejectEggPokeParty->GetMemberCount() > m_pRegulation->GetValueParam( Regulation::TEAM_NUM_HI ) )
  {
    violationDetails.isViolationNumHi = true;
    retOk = false;
  }
  // バトルチーム下限チェック
  if( pRejectEggPokeParty->GetMemberCount() < m_pRegulation->GetValueParam( Regulation::TEAM_NUM_LO ) )
  {
    violationDetails.isViolationNumLo = true;
    retOk = false;
  }

  // 伝説参加上限チェック
  if( CheckLegendNum( pRejectEggPokeParty ) )
  {
    violationDetails.isViolationNumLegend = true;
    retOk = false;
  }

  //-----------------------------------------------------
  // 同じポケモン・アイテムチェック
  //-----------------------------------------------------
  if( CheckBothPoke( pRejectEggPokeParty, violationDetails ) )
  {
    retOk = false;
  }
  if( CheckBothItem( pRejectEggPokeParty, violationDetails ) )
  {
    retOk = false;
  }

  // 必須ポケモンチェック
  if( CheckMustPokeForParty( pRejectEggPokeParty ) == false )
  {
    violationDetails.isViolationMustPoke = true;
    retOk = false;
  }

  return retOk;
}


/*
 * @brief ポケモンパーティを渡してポケモン単体チェック
 * @param[in]   pPokeParty        検査するポケモンパーティ
 * @param[out]  violationDetails  違反内容詳細
 * @retval true:正常
 * @retval false:異常 (違反内容詳細はviolationDetailsに格納されている）
 */
bool PokeRegulation::CheckPokeParaForParty( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails )
{
  // 戻り値
  bool retOk = true;

  // 配列のインデックス
  u8 levelIdx = 0;
  u8 vetoPokeIdx = 0;
  u8 vetoItemIdx = 0;
  u8 vetoWazaIdx = 0;

  // ポケモン単体チェック
  for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
  {
    pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );

    // タマゴは除外
    if( pp->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == true )
    {
      continue;
    }

    //-----------------------------------------------------
    // ポケモン単体のレギュレーションチェック
    //-----------------------------------------------------
    // 違反があったらそのビットを立てる
    int retBit = CheckPokePara( pp );

    // タマゴの場合は以降の処理を無視。
    if( retBit & FAILED_BIT_EGG )
    {
      retOk = false;
      continue;
    }

    // タマゴだった場合は、ポケモンパラメータを取得してはいけない
    const u32 monsNo = pp->GetMonsNo();
    const u16 itemNo = pp->GetItem();
    ICHI_PRINT("%d : monsno %d itemno %d \n", i, monsNo, itemNo );

    if( retBit != FAILED_BIT_NONE )
    {
      retOk = false;

      //======================================================
      /**
       *		エラーの種類みて詳細を保存しておく
       */
      //======================================================
      // レベル違反
      if( retBit & FAILED_BIT_LEVEL )
      {
        violationDetails.isViolationLevel = true;
        violationDetails.level[ levelIdx++ ] = monsNo;
        violationDetails.levelIndex[ i ] = true;
      }
      // 禁止ポケモン
      if( retBit & FAILED_BIT_VETO_POKE )
      {
        violationDetails.isViolationVetoPoke = true;
        violationDetails.vetoPoke[ vetoPokeIdx++ ] = monsNo;
        violationDetails.vetoPokeIndex[ i ] = true;
      }
      // 禁止アイテム
      if( retBit & FAILED_BIT_VETO_ITEM )
      {
        violationDetails.isViolationVetoItem = true;
        violationDetails.vetoItem[ vetoItemIdx++ ] = itemNo;
        violationDetails.vetoItemIndex[ i ] = true;
      }
      // 禁止わざ
      if( retBit & FAILED_BIT_VETO_WAZA )
      {
        violationDetails.isViolationVetoWaza = true;
        violationDetails.vetoWaza[ vetoWazaIdx++ ] = monsNo;
        violationDetails.vetoWazaIndex[ i ] = true;
      }
    }
  }

  return retOk;
}

//--------------------------------------------------------------------------
/**
 * @breif   ポケモンがレギュレーションに適合しているかどうか調べる
 * @param   pPokePara  ポケパラ
 * @return 条件違反だったFAILED_BITのビットがたっている状態で返す
 */
//--------------------------------------------------------------------------
int PokeRegulation::CheckPokePara( pml::pokepara::PokemonParam* pPokePara )
{
  // たまご参戦不可
  if( pPokePara->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) == true )
  {
    // タマゴは検査対象から外しているはずなのでこない
    GFL_ASSERT_MSG(0, "タマゴだった\n");
    return FAILED_BIT_EGG;
  }

  u16 mons = static_cast<u16>(pPokePara->GetMonsNo());
  u16 item = pPokePara->GetItem();
  int retBit = FAILED_BIT_NONE;

  // レギュレーションがないので不明なエラー
  if( m_pRegulation == NULL )
  {
    GFL_ASSERT_MSG(0, "レギュレーションがNULL"); // @check
    return FAILED_BIT_UNKNOWN;
  }

  // レベル制限チェック
  {
    u32 baseLevel = m_pRegulation->GetValueParam( Regulation::LEVEL );
    u32 pokeLevel = pPokePara->GetLevel();

    // レベル制限がある場合検査
    switch( m_pRegulation->GetLevelRangeType() ){
    case Regulation::LEVEL_RANGE_OVER: // 以上
      if( baseLevel > pokeLevel ){
        retBit |= FAILED_BIT_LEVEL;
      }
      break;
    case Regulation::LEVEL_RANGE_LESS: // 以下
      if( baseLevel < pokeLevel ){
        retBit |= FAILED_BIT_LEVEL;
      }
      break;
    }
  }

  // 禁止ポケモンかどうか
  if( m_pRegulation->CheckParamBit( Regulation::VETO_POKE_BIT, mons ) )
  {
    GFL_PRINT("禁止ポケだった %d\n", mons);
    retBit |= FAILED_BIT_VETO_POKE;
  }

  // 禁止アイテムかどうか
  if( m_pRegulation->CheckParamBit( Regulation::VETO_ITEM_BIT, item ) )
  {
    GFL_PRINT("禁止アイテムだった %d\n", item);
    retBit |= FAILED_BIT_VETO_ITEM;
  }

  // 禁止わざがあるかどうか
  for( u8 i=0; i<pPokePara->GetWazaCount(); i++ )
  {
    u32 waza = static_cast<u32>(pPokePara->GetWazaNo(i));
    if( m_pRegulation->CheckParamBit( Regulation::VETO_WAZA_BIT, waza ) )
    {
      GFL_PRINT("禁止わざだった %d\n", waza);
      retBit |= FAILED_BIT_VETO_WAZA;
    }
  }

  // 産地限定
  {
    u32 version = pPokePara->GetCassetteVersion(); // ROMバージョン取得

    if( m_pRegulation->CheckParamProducingArea(version) == false )
    {
      GFL_PRINT("産地違いだった %d\n", version);
      retBit |= FAILED_BIT_VETO_POKE;
    }
  }

  // 破滅の花のフラエッテ
  if( m_pRegulation->GetBoolParam( Regulation::HURAETTE_HAKAI ) )
  {
    if( mons == MONSNO_HURAETTE && pPokePara->GetFormNo() == FORMNO_HURAETTE_HAKAI )
    {
      GFL_PRINT("破滅の花フラエッテだった\n" );
      retBit |= FAILED_BIT_VETO_POKE;
    }
  }

  // 禁止特定フォルムポケモン
  if( m_pRegulation->IsSpecificVetoPoke() )
  {
    if( m_pRegulation->CheckParamSpecifcVetoPoke( mons, pPokePara->GetFormNo() ) )
    {
      GFL_PRINT("禁止特定フォルムポケモンだった %d %d\n", mons, pPokePara->GetFormNo() );
      retBit |= FAILED_BIT_VETO_POKE;
    }
  }

  ICHI_PRINT("MonsNo %d -> %d \n", mons, retBit);

  return retBit;
}

/**
 * @brief   伝説参加上限チェック
 * @param[in]   checkMonsNo   チェックするポケモン番号
 * @return  trueで伝説酸化数上限オーバー
 */
bool PokeRegulation::CheckLegendNum( pml::PokeParty* pPokeParty )
{
  if( m_pRegulation->GetValueParam( Regulation::LEGEND_NUM_HI ) > 0 )
  {
    u8 legendNum = 0;

    for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
    {
      pml::pokepara::PokemonParam *pp = pPokeParty->GetMemberPointer( i );
      u32 monsNo = pp->GetMonsNo();
      u8  formNo = pp->GetFormNo();

      // 伝説チェック
      if( CheckLegend( monsNo, formNo ) == true )
      {
        legendNum++;
      }
    }

    if( legendNum > m_pRegulation->GetValueParam( Regulation::LEGEND_NUM_HI ) )
    {
      return true;
    }
  }
  return false;
}

/**
 * @brief   同じポケモンチェック
 * @param[in]   pPokeParty        検査するポケモンパーティ
 * @param[out]  violationDetails  違反内容詳細
 * @return  trueで同じポケモンがいた
 */
bool PokeRegulation::CheckBothPoke( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails ) const
{
  if( !m_pRegulation->GetBoolParam( Regulation::BOTH_POKE ) )
  {
    // 無効な場合チェックしない
    return false;
  }

  const u32 pokePartyCount = pPokeParty->GetMemberCount(); 
  u8 bothPokeIdx = 0;

  for( u32 i=0; i<pokePartyCount-1; i++ )
  {
    const pml::pokepara::PokemonParam *pPokeParaA = pPokeParty->GetMemberPointerConst( i );
    for( u32 j=i+1; j<pokePartyCount; j++ )
    {
      const pml::pokepara::PokemonParam *pPokeParaB = pPokeParty->GetMemberPointerConst( j );

      // 一致チェック
      if( pPokeParaA && (pPokeParaA->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false) &&
          pPokeParaB && (pPokeParaB->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false) &&
          pPokeParaA->GetMonsNo() == pPokeParaB->GetMonsNo() )
      {
        violationDetails.isViolationBothPoke = true;

        // 同じポケモン番号が既に登録されていたら、登録しない
        {
          const u32 checkMonsNo = pPokeParaA->GetMonsNo();

          bool isSame = false;
          for( u32 k=0; k<bothPokeIdx; k++ )
          {
            if( violationDetails.bothPoke[k] == checkMonsNo )
            {
              isSame = true;
              break;
            }
          }

          if( isSame == false )
          {
            ICHI_PRINT(">REG bothPoke[%d] = Monsno:%d \n", bothPokeIdx, checkMonsNo );
            violationDetails.bothPoke[ bothPokeIdx++ ] = checkMonsNo;
          }
        }

        // このMonsNoはチェックしたので次へ進む
        continue;
      }
    }
  }

  return ( bothPokeIdx != 0 );
}

/**
 * @brief   同じアイテムチェック
 * @param[in]   pPokeParty        検査するポケモンパーティ
 * @param[out]  violationDetails  違反内容詳細
 * @return  trueで同じアイテムがあった
 */
bool PokeRegulation::CheckBothItem( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails ) const
{
  if( !m_pRegulation->GetBoolParam( Regulation::BOTH_ITEM ) )
  {
    // 無効な場合チェックしない
    return false;
  }

  const u32 pokePartyCount = pPokeParty->GetMemberCount(); 
  u8 bothItemIdx = 0;

  for( u32 i=0; i<pokePartyCount-1; i++ )
  {
    const pml::pokepara::PokemonParam *pPokeParaA = pPokeParty->GetMemberPointerConst( i );
    for( u32 j=i+1; j<pokePartyCount; j++ )
    {
      const pml::pokepara::PokemonParam *pPokeParaB = pPokeParty->GetMemberPointerConst( j );

      // 一致チェック
      if( pPokeParaA && (pPokeParaA->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false) &&
          pPokeParaB && (pPokeParaB->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == false) &&
          pPokeParaA->GetItem() != 0 && 
          pPokeParaA->GetItem() == pPokeParaB->GetItem() )
      {

        violationDetails.isViolationBothItem = true;

        // 同じアイテム番号が既に登録されていたら、登録しない
        {
          const u32 checkItemNo = pPokeParaA->GetItem();

          bool isSame = false;
          for( u32 k=0; k<bothItemIdx; k++ )
          {
            if( violationDetails.bothItem[k] == checkItemNo )
            {
              isSame = true;
              break;
            }
          }

          if( isSame == false )
          {
            ICHI_PRINT(">REG bothPoke[%d] = Itemno:%d \n", bothItemIdx, checkItemNo );
            violationDetails.bothItem[ bothItemIdx++ ] = checkItemNo;
          }
        }

        // このItemNoはチェックしたので次へ進む
        continue;
      }
    }
  }

  return ( bothItemIdx != 0);
}

/**
 * @brief   必須ポケモンチェック（パーティ）
 * @param[in]   pPokeParty        検査するポケモンパーティ
 * @return  falseで必須ポケモンがいなかった
 */
bool PokeRegulation::CheckMustPokeForParty( pml::PokeParty* pPokeParty ) const
{
  bool isOk = true;

  // AND条件の必須ポケモン
  {
    for( u8 mustPokeIdx=0; mustPokeIdx<Regulation::MUST_POKE_NUM; mustPokeIdx++ )
    {
      u16 mustPoke = m_pRegulation->GetParamMustPokeAND( mustPokeIdx );
      u8  mustPokeForm = m_pRegulation->GetParamMustPokeFormAND( mustPokeIdx );

      // 設定されていなければ見ない
      if( mustPoke != 0 )
      {
        bool isFound = false;
        // パーティの中にいるかチェック
        for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
        {
          if( CheckMustPokeForPokeParam( pPokeParty->GetMemberPointer(i), mustPoke, mustPokeForm ) == true )
          {
            isFound = true;
            break;
          }
        }

        if( isFound == false )
        {
          // AND条件は一匹でもいなかった場合ダメ
          isOk = false;
          GFL_PRINT(">Regulation AndMustPoke NOT FOUND %d %d \n", mustPoke, mustPokeForm );
          break;// これ以上見ても意味ないので見ない
        }
      }
    }
  }

  // OR条件の必須ポケモン
  {
    bool mustPokeCheck = false;
    u8 mustPokeCount = 0;
    for( u8 mustPokeIdx=0; mustPokeIdx<Regulation::MUST_POKE_NUM; mustPokeIdx++ )
    {
      u16 mustPoke = m_pRegulation->GetParamMustPokeOR( mustPokeIdx );
      u8  mustPokeForm = m_pRegulation->GetParamMustPokeFormOR( mustPokeIdx );

      // 設定されていなければ見ない
      if( mustPoke != 0 )
      {
        mustPokeCheck = true;
        // パーティの中にいるかチェック
        for( u8 i=0; i<pPokeParty->GetMemberCount(); i++ )
        {
          if( CheckMustPokeForPokeParam( pPokeParty->GetMemberPointer(i), mustPoke, mustPokeForm ) == true )
          {
            mustPokeCount++;
          }
        }
      }
    }
    // OR条件は一匹もいない場合ダメ
    if( mustPokeCheck && mustPokeCount == 0 )
    {
      isOk = false;
      GFL_PRINT(">Regulation OrMustPoke NOT FOUND \n");
    }
  }
  return isOk;
}

/**
 * @brief   必須ポケモンチェック(ポケモン単体）
 * @param[in]   pPokemonParam   検査するポケモン
 * @param[in]   mustPoke        必須ポケモン種族No
 * @param[in]   mustPokeForm    必須ポケモンFormNo
 * @retval      true:必須ポケモンだった
 * @retval      false:必須ポケモンじゃない
 */
bool PokeRegulation::CheckMustPokeForPokeParam( pml::pokepara::PokemonParam* pPokemonParam, u16 mustPoke, u8 mustPokeForm ) const
{
  // タマゴチェック
  if( pPokemonParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
  {
    return false;
  }

  const u32 monsNo = pPokemonParam->GetMonsNo();
  const u8  formNo = pPokemonParam->GetFormNo();

  // 必須ポケモンチェック
  if( monsNo == mustPoke )
  {
    // 全フォルムOKか　指定フォルムか
    if( mustPokeForm == Regulation::ALL_FORM_SELECT ||
        mustPokeForm == formNo )
    {
      // いた
      ICHI_PRINT("必須ポケモン いた mons:%d form:%d\n", monsNo, formNo );
      return true;
    }
  }
  return false;
}

/**
 * @brief   必須ポケモンに含まれているか(ポケモン単体）AndとOrは区別しない
 * @param[in]   pPokemonParam   検査するポケモン
 * @retval      true:必須ポケモンに含まれている
 * @retval      false:必須ポケモンに含まれていない
 */
bool PokeRegulation::IsContainMustPoke( pml::pokepara::PokemonParam* pPokemonParam ) const
{
  // タマゴチェック
  if( pPokemonParam->IsEgg( pml::pokepara::CHECK_BOTH_EGG ) )
  {
    return false;
  }

  u16 mustPoke;
  u8  mustPokeForm;
  for( u8 mustPokeIdx=0; mustPokeIdx<Regulation::MUST_POKE_NUM; mustPokeIdx++ )
  {
    // AND条件の必須ポケモン
    mustPoke = m_pRegulation->GetParamMustPokeAND( mustPokeIdx );
    mustPokeForm = m_pRegulation->GetParamMustPokeFormAND( mustPokeIdx );
    if( CheckMustPokeForPokeParam( pPokemonParam, mustPoke, mustPokeForm ) == true )
    {
      return true; // 必須ポケモンいたら即終了
    }

    // OR条件の必須ポケモン
    mustPoke = m_pRegulation->GetParamMustPokeOR( mustPokeIdx );
    mustPokeForm = m_pRegulation->GetParamMustPokeFormOR( mustPokeIdx );
    if( CheckMustPokeForPokeParam( pPokemonParam, mustPoke, mustPokeForm ) == true )
    {
      return true; // 必須ポケモンいたら即終了
    }
  }
  return false;
}

/**
 * @brief   レベルにレギュレーションにそったレベル補正をかける
 * @param   level   元レベル
 * @return 補正後レベル
 */
u32 PokeRegulation::ModifyLevelCalc( u32 level ) const
{
  const u32 modify_level = m_pRegulation->GetValueParam( Regulation::LEVEL );

  switch( m_pRegulation->GetLevelRangeType() ){
  // 以上補正   補正レベル以上ならば補正レベルにする（50補正で51なら50）
  case Regulation::LEVEL_RANGE_DRAG_DOWN:
    // 以上補正なので、補正レベル以上なら補正レベルにする
    if( level > modify_level )
    {
      ICHI_PRINT("以上補正\n");
      return modify_level;
    }
    break;

  // 全補正
  case Regulation::LEVEL_RANGE_SAME:
    // 全補正なので全員行う
    ICHI_PRINT("全補正\n");
    return modify_level;

  // 以下補正  補正レベル以下ならば補正レベルにする(50補正で49なら50)
  case Regulation::LEVEL_RANGE_PULL_UP:
    // 以下補正なので補正レベル以下ならば補正レベルにする
    if( level < modify_level )
    {
      ICHI_PRINT("以下補正\n");
      return modify_level;
    }
    break;

  default:
    // それ以外は何もしない
    break;
  }

  return level;
}

/**
  * @brief ポケパーティを拡張スクリプトで検査
  * @param[in]   pPokeParty        検査するポケモンパーティ
  * @param[out]  violationDetails  違反内容詳細
  * @retval true:正常
  * @retval false:異常 (違反内容詳細はviolationDetailsに格納されている）
  */
bool PokeRegulation::CheckPokePartyExScript( pml::PokeParty* pPokeParty, ViolationDetails& violationDetails, RegulationScriptChecker::Mode eMode )
{
  RegulationScriptChecker checker( m_pHeap );
  checker.CheckStart( m_pRegulation, pPokeParty, eMode );
  while( checker.Update() )
  {
    gfl2::thread::ThreadStatic::CurrentSleep( 1 );
  }

  if( checker.GetResultEntry() == RegulationScriptChecker::RESULT_ENTRY_OK )
  {
    return true;
  }
  else
  {
    violationDetails.isViolationExScript = true;
    return false;
  }
}


//GFL_NAMESPACE_END( Battle )

