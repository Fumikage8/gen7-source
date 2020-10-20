//=============================================================================
/**
 * @brief  ポケモンパラメータ 生成システム
 * @file   pml_PokemonParamFactory.cpp
 * @author obata_toshihiro
 * @date   2011.01.05
 */
//=============================================================================
#include <heap/include/gfl2_Heap.h>
#include <debug/include/gfl2_Assert.h>
#include <util/include/gfl2_std_string.h>

#include "pml/include/pml_Local.h"
#include "pml/include/pml_Type.h"
#include "pml/include/pml_PokeParty.h"
#include "pml/include/pokepara/pml_PokemonParam.h"
#include "pml/include/pokepara/pml_PokemonParamFactory.h"
#include "./pml_PokemonParamLocal.h"
#include "./pml_PokemonParamAccessor.h"
#include "./pml_PokemonParamCalcTool.h"

#include "niji_conv_header/poke_lib/item/monsball_id.h"
#include <pml/include/system/pml_Library.h>

namespace pml {
  namespace pokepara { 


    //-------------------------------------------------------------------------
    /**
     * @brief コアデータを生成する
     * @param heap データの確保に使用するヒープ
     * @return ゼロクリアされたコアデータ
     */
    //-------------------------------------------------------------------------
    CoreData* Factory::CreateCoreData( gfl2::heap::HeapBase* heap )
    {
      CoreData* core_data = AllocCoreData( heap );
      ClearCoreData( core_data );
      return core_data;
    }

    /**
     * @brief コアデータのメモリを確保する
     * @param heap  使用するヒープ
     */
    CoreData* Factory::AllocCoreData( gfl2::heap::HeapBase* heap )
    {
      return static_cast<CoreData*>( GflHeapAllocMemory( heap, sizeof(CoreData) ) );
    }

    /**
     * @brief コアデータをゼロクリアする
     * @param coreData  クリア対象のコアデータ
     */
    void Factory::ClearCoreData( CoreData* coreData )
    {
      gfl2::std::MemFill( coreData, 0, sizeof(CoreData) );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コアデータを生成する
     * @param heap データの確保に使用するヒープ
     * @param monsno ポケモンNo.
     * @param level  ポケモンのレベル
     * @param id     ポケモンのIDNo.
     * @return 指定したパラメータを持つコアデータ( 暗号化済み )
     */
    //-------------------------------------------------------------------------
    CoreData* Factory::CreateCoreData( gfl2::heap::HeapBase* heap, MonsNo monsno, u16 level, u64 id )
    {
      InitialSpec spec;
      spec.monsno = monsno;
      spec.level  = level;
      spec.id     = id;
      return CreateCoreData( heap, spec );
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コアデータを生成する
     * @param heap データの確保に使用するヒープ
     * @param spec ポケモンの初期設定
     * @return 指定したパラメータを持つコアデータ( 暗号化済み )
     */
    //-------------------------------------------------------------------------
    CoreData* Factory::CreateCoreData( gfl2::heap::HeapBase* heap, const InitialSpec& spec )
    {
      CoreData* core_data = AllocCoreData( heap );
      SetupCoreData( core_data, spec );
      return core_data;
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief コアデータをセットアップする
     * @param coreData  セットアップ対象のコアデータ
     * @param spec      ポケモンの初期設定
     */
    //-------------------------------------------------------------------------
    void Factory::SetupCoreData( CoreData* coreData, const InitialSpec& spec )
    {
      ClearCoreData( coreData );

      InitialSpec fix_spec;
      FixInitSpec( &fix_spec, spec );
      InitCoreData( coreData, fix_spec );
    }

    /**
     * @brief あいまいな初期設定を解決する
     * @param[out] fix_spec あいまいさを解決した設定
     * @param[in]  spec     あいまいさを含む設定
     */
    void Factory::FixInitSpec( InitialSpec* fix_spec, const InitialSpec& spec )
    { 
      gfl2::std::MemCopy( &spec, fix_spec, sizeof(InitialSpec) );

      // 個性乱数
      if( fix_spec->personal_rnd == INIT_SPEC_PERSONALRND_RANDOM ) {
        fix_spec->personal_rnd = pml::local::Random::GetValue();
      }

      // IDNo.
      if( fix_spec->id == INIT_SPEC_ID_RANDOM ) {
        fix_spec->id = pml::local::Random::GetValue();
      } 

      // レア乱数
      switch( fix_spec->rare_rnd ) 
      {
      case INIT_SPEC_RARE_FALSE:
        fix_spec->rare_rnd = CalcTool::CorrectColorRndForNormal( static_cast<u32>(fix_spec->id), pml::local::Random::GetValue() );
        break; 
      case INIT_SPEC_RARE_TRUE:
        fix_spec->rare_rnd = CalcTool::CorrectColorRndForRare( static_cast<u32>(fix_spec->id), pml::local::Random::GetValue() );
        break; 
      case INIT_SPEC_RARE_RANDOM:
        for( int i = 0; i < fix_spec->rare_try_count; ++i ){
          fix_spec->rare_rnd = pml::local::Random::GetValue();
          if( CalcTool::IsRareColor( static_cast<u32>(fix_spec->id), static_cast<u32>(fix_spec->rare_rnd) ) ) break;  //レアになったら止める
        }
        break; 
      }

      // レベル
      if( MAX_POKE_LEVEL < fix_spec->level ) {
        fix_spec->level = MAX_POKE_LEVEL;
      }

      // 個体値
      {
        u8 i = 0;

        //V指定カウントチェック
        if( fix_spec->talentVNum > INIT_SPEC_TALENT_V_NUM_NONE ){
          
          if( fix_spec->talentVNum > (POWER_NUM-1) ){
            GFL_ASSERT(0); //@check iwasawa 許容するのはランダム5Vまで
            fix_spec->talentVNum = 0; //不正な値だったら0に戻す。POWER_NUM>=6なら直接指定すればいい
          }
          for( i=0; i<POWER_NUM; i++ ){
            fix_spec->talentPower[i] = INIT_SPEC_TALENT_RANDOM; //一度全部ランダムにリセット
          }
          for( i = 0;i < fix_spec->talentVNum; i++ ){
            u8 idx;
            do{
              idx = pml::local::Random::GetValue( POWER_NUM ); 
            }while( fix_spec->talentPower[idx] != INIT_SPEC_TALENT_RANDOM );
            fix_spec->talentPower[idx] = MAX_TALENT_POWER;  //V設定
          }
        }
        for(  i=0; i<POWER_NUM; i++ )
        {
          if( fix_spec->talentPower[i] == INIT_SPEC_TALENT_RANDOM ) { 
            fix_spec->talentPower[i] = pml::local::Random::GetValue( MAX_TALENT_POWER + 1 ); 
          }
        }
      }

      // 特性
      if( fix_spec->tokusei_index == INIT_SPEC_TOKUSEI_1_OR_2 ) {
        fix_spec->tokusei_index = pml::local::Random::GetValue(2);
      }
      else if ( fix_spec->tokusei_index == INIT_SPEC_TOKUSEI_1_OR_2_OR_3 ) {
        fix_spec->tokusei_index = pml::local::Random::GetValue(3);
      }

      // 性格
      if( fix_spec->seikaku == INIT_SPEC_SEIKAKU_RANDOM ) {
        fix_spec->seikaku = pml::local::Random::GetValue( SEIKAKU_NUM );
      }

      // フォルム
      if( fix_spec->formno == INIT_SPEC_FORM_RANDOM ) {
        //メガ進化フォルムを持っているかどうか
        pml::personal::LoadMegaEvolveTable( fix_spec->monsno );

        //メガフォルムを持つかと、通常のフォルム違いを持つかは排他的
        if( pml::personal::GetMegaEvolveRouteNum() == 0 ){
          //ランダムが有効なのな通常フォルム違いを持つ場合のみ
          personal::LoadPersonalData( fix_spec->monsno, 0 );
          u8 form_max = personal::GetPersonalParam( personal::PARAM_ID_form_max );
          fix_spec->formno = pml::local::Random::GetValue( form_max );
        }else{
          //メガフォルムを持つ場合はFORM_RANDOMは無効になり、必ずフォルム0が入る
          fix_spec->formno = 0;
        }
      }

      // 性別
      if( fix_spec->sex == INIT_SPEC_SEX_BOTH ) {
        fix_spec->sex = GetSexAtRandomInPersonalDistribution( fix_spec->monsno, fix_spec->formno );
      }
      fix_spec->sex = CalcTool::GetCorrectSexInPersonalData( 
          fix_spec->monsno, fix_spec->formno, static_cast<Sex>( fix_spec->sex ) );

      // なつき度
      if( fix_spec->familiarity == INIT_SPEC_FAMILIARITY_DEFAULT ) {
        personal::LoadPersonalData( fix_spec->monsno, fix_spec->formno );
        fix_spec->familiarity = personal::GetPersonalParam( personal::PARAM_ID_initial_familiarity );
      }
    }

    /**
     * @brief コアデータを初期化する
     * @param[out] core_data 初期化するコアデータ( 復号化 )
     * @param[in]  spec      パラメータの初期設定
     */
    void Factory::InitCoreData( CoreData* core_data, const InitialSpec& spec )
    {
      Accessor accessor;
      accessor.AttachDecodedData( core_data, NULL ); // ここで暗号化される

      if( spec.monsno == MONSNO_NULL ) { 
        return; 
      } 

      // 個性乱数
      // 個性乱数が構造体の並びを決定するので、必ず先に確定させる
      accessor.SetPersonalRnd( static_cast<u32>(spec.personal_rnd) );

      accessor.SetID( static_cast<u32>(spec.id) );                            // IDNo.  
      accessor.SetColorRnd( static_cast<u32>(spec.rare_rnd) );                // レア乱数
      accessor.SetMonsNo( spec.monsno );             // モンスターNo.
      accessor.SetFormNo( spec.formno );             // フォルムNo.
      accessor.SetLevel( static_cast<u8>(spec.level) );                       // レベル
      accessor.SetSeikaku( spec.seikaku );           // 性格
      accessor.SetSex( spec.sex );                   // 性別
      accessor.SetGetLevel( static_cast<u8>(spec.level) );                    // 捕獲時のレベル
      accessor.SetCassetteVersion( 0 );              // カセットバージョン->project側で設定するため0
      accessor.SetFamiliarity( spec.familiarity );                            // なつき度
      accessor.SetLangId( Library::m_setting->langId );                       //言語設定（DS世代以前ではcountry_codeだった）
      accessor.SetCountryCode( Library::m_setting->countryCode );             // 国・地域コード（3DS新規項目）
      accessor.SetCountryRegionCode( Library::m_setting->countryRegionCode ); // 上記内区分コード
      accessor.SetHWRegionCode( Library::m_setting->hwRegionCode );           // 3DS本体リージョンコード

      // 個体値
      accessor.SetTalentHp( static_cast<u8>(spec.talentPower[ POWER_HP ]) );        
      accessor.SetTalentAtk( static_cast<u8>(spec.talentPower[ POWER_ATK ]) );
      accessor.SetTalentDef( static_cast<u8>(spec.talentPower[ POWER_DEF ]) );
      accessor.SetTalentSpAtk( static_cast<u8>(spec.talentPower[ POWER_SPATK ]) );
      accessor.SetTalentSpDef( static_cast<u8>(spec.talentPower[ POWER_SPDEF ]) );
      accessor.SetTalentAgi( static_cast<u8>(spec.talentPower[ POWER_AGI ]) );

      // 特性
      TokuseiNo tokusei_no = CalcTool::GetTokuseiNo( spec.monsno, spec.formno, spec.tokusei_index ); 
      accessor.SetTokuseiNo( tokusei_no );
      accessor.SetTokusei1Flag( spec.tokusei_index == 0 );
      accessor.SetTokusei2Flag( spec.tokusei_index == 1 );
      accessor.SetTokusei3Flag( spec.tokusei_index == 2 ); 

      // 経験値は成長曲線グループにより最小値が異なる
      {
        pml::personal::LoadGrowTable( spec.monsno, spec.formno );
        u32 exp = pml::personal::GetMinExp( static_cast<u8>(spec.level) );
        accessor.SetExp( exp ); 
      }
    }

    /**
     * @brief パーソナルデータの性別分布に基づいて, 性別をランダムに決定する
     *
     * @param monsno  モンスターNo.
     * @param formno  フォルムNo.
     *
     * @retval SEX_MALE     ♂しか存在しない or ♂♀両方存在する場合
     * @retval SEX_FEMALE   ♀しか存在しない or ♂♀両方存在する場合
     * @retval SEX_UNKNOWN  性別不明の場合
     */
    Sex Factory::GetSexAtRandomInPersonalDistribution( MonsNo monsno, FormNo formno )
    {
      // 性別ベクトルを取得
      personal::LoadPersonalData( monsno, formno );
      u32 sex_vector = personal::GetPersonalParam( personal::PARAM_ID_sex );

      // 性別固定のケース
      switch( sex_vector ) {
      case personal::SEX_VECTOR_ONLY_MALE:   return SEX_MALE;
      case personal::SEX_VECTOR_ONLY_FEMALE: return SEX_FEMALE;
      case personal::SEX_VECTOR_UNKNOWN:     return SEX_UNKNOWN;
      }

      // ♂♀両方が存在する場合
      u32 range = personal::SEX_VECTOR_RANDOM_MAX - personal::SEX_VECTOR_RANDOM_MIN;
      u32 rnd = pml::local::Random::GetValue( range );
      rnd += personal::SEX_VECTOR_RANDOM_MIN;  // [SEX_VECTOR_RND_MIN, SEX_VECTOR_MAX]

      if( rnd < sex_vector )
      {
        return SEX_FEMALE;
      }
      else
      {
        return SEX_MALE;
      }
    }

    //-------------------------------------------------------------------------
    /**
     * @brief コアデータを破棄する
     */
    //-------------------------------------------------------------------------
    void Factory::DeleteCoreData( CoreData* core_data )
    {
      GflHeapFreeMemory( core_data );
    }





    //-------------------------------------------------------------------------
    /**
     * @brief 算出データを生成する
     * @param heap         データの確保に使用するヒープ
     * @param personal_rnd 関連付けるコアパラメータの個性乱数
     * @return ゼロクリアされた算出データ
     */
    //-------------------------------------------------------------------------
    CalcData* Factory::CreateCalcData( gfl2::heap::HeapBase* heap, u32 personal_rnd )
    {
      CalcData* calc_data = static_cast<CalcData*>( GflHeapAllocMemory( heap, sizeof(CalcData) ) );
      gfl2::std::MemFill( calc_data, 0, sizeof(CalcData) );
      Encoder::Encode( calc_data, sizeof(CalcData), personal_rnd );
      return calc_data;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief 算出データを破棄する
     */
    //-------------------------------------------------------------------------
    void Factory::DeleteCalcData( CalcData* calc_data )
    {
      GflHeapFreeMemory( calc_data );
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 「ヌケニン」を作成する
     * @param heap    ヌケニン作成に使用するヒープ
     * @param tutinin ヌケニンのベースとなる「ツチニン」or「テッカニン」
     * @return 作成したヌケニン
     */
    //-------------------------------------------------------------------------
    PokemonParam* Factory::CreateNukenin( gfl2::heap::HeapBase* heap, PokemonParam* tutinin )
    {
      GFL_ASSERT(  //@check iwasawa NULLを返す方が恐ろしいのでこのまま 
          ( tutinin->GetMonsNo() == MONSNO_TUTININ ) ||
          ( tutinin->GetMonsNo() == MONSNO_TEKKANIN ) );

      PokemonParam* nukenin = GFL_NEW( heap ) PokemonParam( heap, *tutinin );
      nukenin->ChangeMonsNo( MONSNO_NUKENIN, 0 );
      nukenin->RecoverSick();
      nukenin->RemoveItem();
      nukenin->SetMemories( MEMORIES_CAPTURED_BALL, BALLID_MONSUTAABOORU );
      nukenin->RemoveAllBoxMark();
      nukenin->RemoveAllRibbon();
      nukenin->SetDefaultNickName();
      //スパーリング関連パラメータをクリア
      nukenin->SetTrainingBadge1( 0 );
      nukenin->SetTrainingBadge2( 0 );
      return nukenin;
    }



  } // namespace pokepara
} // namespace pml
