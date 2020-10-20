//=============================================================================
/**
 * @brief  ポケモンの進化
 * @file   pml_EvolveManager.cpp
 * @author obata_toshihiro
 * @date   2011.01.28
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>

#include <pml/include/pml_Const.h>
#include <pml/include/pml_Type.h>
#include <pml/include/pml_Item.h>
#include <pml/include/pml_System.h>
#include <pml/include/pml_Personal.h>
#include <pml/include/pml_PokeParty.h>
#include <pml/include/pokepara/pml_PokemonCoreParam.h>
#include <pml/include/pokepara/pml_EvolveManager.h>


namespace pml { 
  namespace pokepara { 


    //-------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-------------------------------------------------------------------------
    EvolveManager::EvolveManager( void )
    {
    } 

    //-------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-------------------------------------------------------------------------
    EvolveManager::~EvolveManager()
    {
    }




    //-------------------------------------------------------------------------
    /**
     * @brief 進化する余地があるかどうかのチェック
     * @param[in] poke  チェック対象のポケモン
     * @return    bool  trueのとき、進化ルートを持っている
     */
    //-------------------------------------------------------------------------
    bool EvolveManager::HaveEvolutionRoot( const CoreParam* poke ) const
    {
      MonsNo current_monsno = poke->GetMonsNo(); 
      FormNo current_formno = poke->GetFormNo();
      // 進化テーブルを読み込む
      pml::personal::LoadEvolutionTable( current_monsno, current_formno );  
      return pml::personal::GetEvolutionRouteNum() != 0;
    }

    //-------------------------------------------------------------------------
    /**
     * @brief レベルアップによる進化後のモンスターNo.を取得する
     * @param poke  チェック対象のポケモン
     * @retval 進化後のモンスターNo.
     * @note 進化できない場合は, 現在のモンスターNo.を返す
     */
    //-------------------------------------------------------------------------
    MonsNo EvolveManager::GetEvolvedMonsNo_byLevelUp( const CoreParam* poke, const pml::PokeParty * party, const EvolveSituation * situation, u32 * root_num ) const
    { 
      MonsNo current_monsno = poke->GetMonsNo(); 
      FormNo current_formno = poke->GetFormNo();

      //「かわらずのいし」を持っていたら進化しない
      u32 itemno = poke->GetItem();
      if( this->CheckItem_KAWARAZUNOISHI( itemno ) ) {
        return current_monsno;
      }

      // 進化テーブルを読み込む
      pml::personal::LoadEvolutionTable( current_monsno, current_formno );  

      u8 evolve_route_num = pml::personal::GetEvolutionRouteNum();
      for( u32 i=0; i<evolve_route_num; i++ )
      {
        EvolveData evolve_data;
        evolve_data.cond = pml::personal::GetEvolutionCondition(i);
        evolve_data.cond_value = pml::personal::GetEvolutionParam(i);
        evolve_data.next_monsno = pml::personal::GetEvolvedMonsNo(i);
        evolve_data.enable_level = pml::personal::GetEvolveEnableLevel(i);

        bool evolve_enable = this->IsSatisfyEvolveCondition_LEVELUP( poke, party, situation, evolve_data );
        if( evolve_enable ) {
          if (root_num) *root_num = i;
          return evolve_data.next_monsno;
        }
      }
      return current_monsno;
    }

    /**
     * @brief ポケモンと進化データを照らし合わせ, 進化条件を満たしているかどうかを調べる
     * @param poke         チェックするポケモン
     * @param party
     * @param situation
     * @param evolve_data  進化データ
     * @retval true   進化条件を満たしている
     * @retval false  進化条件を満たしていない
     */
    bool EvolveManager::IsSatisfyEvolveCondition_LEVELUP( 
            const pml::pokepara::CoreParam* poke, 
            const pml::PokeParty * party,
            const EvolveSituation * situation,
            const EvolveData& evolve_data ) const
    {
      // 必要レベルを満たしていない
      if( poke->GetLevel() < evolve_data.enable_level )
      {
        return false;
      }

      u32 familiarity = poke->CompareOwnerInfo(&situation->owner_info) ? poke->GetFamiliarity() : poke->GetOthersFamiliarity();
      switch( evolve_data.cond ) 
      {
      // なつき度が高い状態でレベルアップ
      case pml::personal::EVOLVE_COND_FAMILIARITY_HIGH:  
        return ( EVOLVE_FAMILIARITY <= familiarity );

      // なつき度が高い状態で朝昼にレベルアップ
      case pml::personal::EVOLVE_COND_FAMILIARITY_HIGH_NOON:  
        return ( ( EVOLVE_FAMILIARITY <= familiarity ) && ( situation->is_morning_or_noon ) );

      // なつき度が高い状態で夜間にレベルアップ
      case pml::personal::EVOLVE_COND_FAMILIARITY_HIGH_NIGHT:  
        return ( ( EVOLVE_FAMILIARITY <= familiarity ) && ( situation->is_night ) );

      // 規定レベルに達していれば進化
      case pml::personal::EVOLVE_COND_LEVELUP:  
        return true;

      // ( こうげき > ぼうぎょ )の状態でレベルアップ
      case pml::personal::EVOLVE_COND_SPECIAL_POW:  
        {
          u16 atk = poke->GetPower( pokepara::POWER_ATK );
          u16 def = poke->GetPower( pokepara::POWER_DEF );
          return ( def < atk );
        }

      // ( こうげき == ぼうぎょ )の状態でレベルアップ
      case pml::personal::EVOLVE_COND_SPECIAL_EVEN:  
        {
          u16 atk = poke->GetPower( pokepara::POWER_ATK );
          u16 def = poke->GetPower( pokepara::POWER_DEF );
          return ( atk == def );
        }

      // ( こうげき < ぼうぎょ )の状態でレベルアップ
      case pml::personal::EVOLVE_COND_SPECIAL_DEF:  
        {
          u16 atk = poke->GetPower( pokepara::POWER_ATK );
          u16 def = poke->GetPower( pokepara::POWER_DEF );
          return ( atk < def );
        }

      // 個性乱数の上位16ビットを10で割った余りが5未満
      case pml::personal::EVOLVE_COND_SPECIAL_RND_EVEN:		
        {
          u32 personal_rnd = poke->GetPersonalRnd() >> 16;
          return ( ( personal_rnd % 10 ) < 5 );
        }

      // 個性乱数の上位16ビットを10で割った余りが5以上
      case pml::personal::EVOLVE_COND_SPECIAL_RND_ODD:		
        {
          u32 personal_rnd = poke->GetPersonalRnd() >> 16;
          return ( 5 <= ( personal_rnd % 10 ) );
        }

      // 手持ちに空きがあれば、進化しつつ、カラを残す( テッカニン専用 )
      case pml::personal::EVOLVE_COND_SPECIAL_LEVELUP:		
        return true;

      // 手持ちに空きがあり,「モンスターボール」を持っている状態でレベルアップ( ヌケニン専用 )
      case pml::personal::EVOLVE_COND_SPECIAL_NUKENIN:  
        GFL_ASSERT_STOP(0); //この条件は使用されることがない
        return false;

      //「うつくしさ」が一定値以上の状態でレベルアップ
      case pml::personal::EVOLVE_COND_SPECIAL_BEAUTIFUL:  
        return ( evolve_data.cond_value <= poke->GetCondition( pml::pokepara::CONDITION_BEAUTIFUL ) );

      // 特定のアイテムを持たせた状態で朝昼にレベルアップ
      case pml::personal::EVOLVE_COND_SOUBI_NOON:  
        return ( ( poke->GetItem() == evolve_data.cond_value ) && ( situation->is_morning_or_noon ) ); 

      // 特定のアイテムを持たせた状態で夜間にレベルアップ
      case pml::personal::EVOLVE_COND_SOUBI_NIGHT:  
        return ( ( poke->GetItem() == evolve_data.cond_value ) && ( situation->is_night ) );

      // 特定の技を習得した状態でレベルアップ
      case pml::personal::EVOLVE_COND_WAZA:  
        for( u32 idx=0; idx<pml::MAX_WAZA_NUM; idx++ ) {
          WazaNo wazano = poke->GetWazaNo( idx );
          if( wazano == evolve_data.cond_value ) {
            return true;
          }
        }
        break;

      // 手持ちに特定のポケモンがいる状態でレベルアップ
      case pml::personal::EVOLVE_COND_POKEMON:  
        return party->CheckPokeExist(static_cast<MonsNo>( evolve_data.cond_value ) );

      // ♂がレベルアップ
      case pml::personal::EVOLVE_COND_MALE:  
        return ( poke->GetSex() == pml::SEX_MALE );

      // ♀がレベルアップ
      case pml::personal::EVOLVE_COND_FEMALE:  
        return ( poke->GetSex() == pml::SEX_FEMALE );

      //「特殊磁場」でレベルアップ
      case pml::personal::EVOLVE_COND_PLACE_MAGNETIC:  
        return situation->is_magnetic_field;

      //「苔生した岩」付近でレベルアップ
      case pml::personal::EVOLVE_COND_PLACE_KOKE:  
        return situation->is_moss_field;

      //「凍結した岩」付近でレベルアップ
      case pml::personal::EVOLVE_COND_PLACE_ICE:  
        return situation->is_ice_field;

      //「雪山」でレベルアップ
      case pml::personal::EVOLVE_COND_PLACE_SNOW_MOUNTAIN:  
        return situation->is_snow_mountain;

      //「でんきどうくつ」でレベルアップ
      //case pml::personal::EVOLVE_COND_PLACE_DENKIDOUKUTSU:  
      //  return situation->is_magnetic_field;

      // 3DSをひっくり返している状態でレベルアップ
      case pml::personal::EVOLVE_COND_DEVICE_TURNOVER:
        return ( situation->is_device_turned_over );

      // なかよし度が高い状態でレベルアップ
      case pml::personal::EVOLVE_COND_FRIENDLY:
        {
          u32 friendly = poke->CompareOwnerInfo(&situation->owner_info) ? poke->GetFriendship() : poke->GetOthersFriendship();
          return ( EVOLVE_FRIENDLY <= friendly );
        }

      // なかよし度が高い状態で特定タイプの技を持ってレベルアップ
      case pml::personal::EVOLVE_COND_FRIENDLY_WAZATYPE:
        {
          u32 friendly = poke->CompareOwnerInfo(&situation->owner_info) ? poke->GetFriendship() : poke->GetOthersFriendship();
          if ( EVOLVE_FRIENDLY <= friendly ) {
            for( u32 idx=0; idx<pml::MAX_WAZA_NUM; idx++ ) {
              WazaNo wazano = poke->GetWazaNo( idx );
              PokeType wazatype = pml::wazadata::GetType( wazano );
              if( wazatype == evolve_data.cond_value ) {
                return true;
              }
            }
          }
        }
        break;
        
      // 悪タイプのポケモンと一緒にいるときにレベルアップ
      case pml::personal::EVOLVE_COND_WITH_POKETYPE_AKU:
        {
          u32 max = party->GetMemberCount();
          PokeType type = POKETYPE_AKU;
          for (u32 idx=0; idx<max; idx++ ){
            const PokemonParam * pp = party->GetMemberPointerConst(idx);
            if ( pp->IsEgg(CHECK_BOTH_EGG) == false && ( pp->GetType1() == type || pp->GetType2() == type )) {
              return true;
            }
          }
        }
        break;

      // 天候が雨のときにレベルアップ
      case pml::personal::EVOLVE_COND_WEATHER_RAIN:
        return ( situation->is_rain );

      // 朝昼にレベルアップ
      case pml::personal::EVOLVE_COND_NOON:
        return ( situation->is_morning_or_noon );

      // 夜にレベルアップ
      case pml::personal::EVOLVE_COND_NIGHT:
        return ( situation->is_night );

      // ♀がレベルアップ（進化後フォルムも変化）
      case pml::personal::EVOLVE_COND_FEMALE_FORMCHANGE:
        return ( poke->GetSex() == pml::SEX_FEMALE );

      // 特定のカセットバージョンでレベルアップ
      case pml::personal::EVOLVE_COND_CASSETTE_VERSION:
        return ( pml::Library::m_setting->cassetteVersion == evolve_data.cond_value );

      // 特定のカセットバージョンで、朝昼にレベルアップ
      case pml::personal::EVOLVE_COND_CASSETTE_VERSION_NOON:
        return ( ( pml::Library::m_setting->cassetteVersion == evolve_data.cond_value ) &&
                 ( situation->is_morning_or_noon ) );

      // 特定のカセットバージョンで、夜にレベルアップ
      case pml::personal::EVOLVE_COND_CASSETTE_VERSION_NIGHT:
        return ( ( pml::Library::m_setting->cassetteVersion == evolve_data.cond_value ) &&
                 ( situation->is_night ) );

        // 夜にレベルアップ
      case pml::personal::EVOLVE_COND_EVENING:
        return (situation->is_evening);

      // ウルトラスペースでレベルアップ
      case pml::personal::EVOLVE_COND_PLACE_ULTRA_SPACE:
        return (situation->is_ultra_space);
      }


      // 進化できない
      return false; 
    }






    //-------------------------------------------------------------------------
    /**
     * @brief アイテムを使用することによって進化できるかどうかを調べる
     * @param poke      チェック対象のポケモン
     * @param use_item  使用する道具
     * @retval 進化後のモンスターNo.
     * @note 進化できない場合は, 現在のモンスターNo.を返す
     */
    //-------------------------------------------------------------------------
    MonsNo EvolveManager::GetEvolvedMonsNo_byItem( const CoreParam* poke, const EvolveSituation * situation, u32 use_item, u32 * root_num ) const
    {
      MonsNo current_monsno = poke->GetMonsNo(); 
      FormNo current_formno = poke->GetFormNo();

      //「かわらずのいし」を持っていたら進化しない
      u32 itemno = poke->GetItem();
      if( this->CheckItem_KAWARAZUNOISHI( itemno ) ) {
        return current_monsno;
      }
      if ( current_monsno == MONSNO_HURAETTE && poke->GetFormNo() == FORMNO_HURAETTE_HAKAI )
      {
        return current_monsno;
      }

      // 進化テーブルを読み込む
      pml::personal::LoadEvolutionTable( current_monsno, current_formno );  


      u8 evolve_route_num = pml::personal::GetEvolutionRouteNum();
      for( u32 i=0; i<evolve_route_num; i++ )
      {
        EvolveData evolve_data;
        evolve_data.cond = pml::personal::GetEvolutionCondition(i);
        evolve_data.cond_value = pml::personal::GetEvolutionParam(i);
        evolve_data.next_monsno = pml::personal::GetEvolvedMonsNo(i);
        evolve_data.enable_level = pml::personal::GetEvolveEnableLevel(i);

        bool evolve_enable = IsSatisfyEvolveCondition_ITEM( poke, situation, use_item, evolve_data ); 
        if( evolve_enable ) {
          if (root_num) *root_num = i;
          return evolve_data.next_monsno;
        }
      }
      return current_monsno;
    } 

    /**
     * @brief ポケモンと進化データを照らし合わせ, 進化条件を満たしているかどうかを調べる
     * @param poke         チェックするポケモン
     * @param situation    状況（時間帯と場所）
     * @param use_item     使用する道具
     * @param evolve_data  進化データ
     * @retval true   進化条件を満たしている
     * @retval false  進化条件を満たしていない
     */
    bool EvolveManager::IsSatisfyEvolveCondition_ITEM( 
        const pml::pokepara::CoreParam* poke, 
        const EvolveSituation * situation,
        u32 use_item, 
        const EvolveData& evolve_data ) const
    {
      switch( evolve_data.cond ) 
      {
      case pml::personal::EVOLVE_COND_ITEM:  // 特定のアイテムを使用する
        return ( use_item == evolve_data.cond_value );

      case pml::personal::EVOLVE_COND_ITEM_MALE:  // ♂に特定のアイテムを使用する
        return ( 
            ( use_item == evolve_data.cond_value ) && 
            ( poke->GetSex() == pml::SEX_MALE ) );

      case pml::personal::EVOLVE_COND_ITEM_FEMALE:  // ♀に特定のアイテムを使用する
        return ( 
            ( use_item == evolve_data.cond_value ) && 
            ( poke->GetSex() == pml::SEX_FEMALE ) );

      case pml::personal::EVOLVE_COND_PLACE_ULTRA_SPACE_ITEM:  // 「ウルトラスペース」で特定のアイテムを使用する
        return (
            (use_item == evolve_data.cond_value) && 
            (situation->is_ultra_space) );
      } 

      return false;  // 進化できない
    }






    //-------------------------------------------------------------------------
    /**
     * @brief 通信交換で進化できるかどうかを調べる
     * @param poke       チェック対象のポケモン
     * @param pair_poke  交換相手のポケモン
     * @retval 進化後のモンスターNo.
     * @note 進化できない場合は, 現在のモンスターNo.を返す
     */
    //-------------------------------------------------------------------------
    MonsNo EvolveManager::GetEvolvedMonsNo_byTrade(
        const CoreParam* poke, const CoreParam* pair_poke, u32 * root_num ) const
    {
      MonsNo current_monsno = poke->GetMonsNo(); 
      FormNo current_formno = poke->GetFormNo();

      //タマゴの場合は進化しない
      if( poke->IsEgg(CHECK_BOTH_EGG) ) return current_monsno;

      //「かわらずのいし」を持っていたら進化しない
      u32 itemno = poke->GetItem();
      if( this->CheckItem_KAWARAZUNOISHI( itemno ) && (current_monsno!= MONSNO_YUNGERAA) ) {
        return current_monsno;
      }

      // 進化テーブルを読み込む
      pml::personal::LoadEvolutionTable( current_monsno, current_formno );  
      u8 evolve_route_num = pml::personal::GetEvolutionRouteNum();
      for( u32 i=0; i<evolve_route_num; i++ )
      {
        EvolveData evolve_data;
        evolve_data.cond = pml::personal::GetEvolutionCondition(i);
        evolve_data.cond_value = pml::personal::GetEvolutionParam(i);
        evolve_data.next_monsno = pml::personal::GetEvolvedMonsNo(i);
        evolve_data.enable_level = pml::personal::GetEvolveEnableLevel(i);

        bool evolve_enable = IsSatisfyEvolveCondition_TRADE( poke, pair_poke, evolve_data );
        if( evolve_enable ) {
          *root_num = i;
          //NN_LOG("root_num %d\n",i);
          return evolve_data.next_monsno;
        } 
      }
      return current_monsno;
    } 

    /**
     * @brief ポケモンと進化データを照らし合わせ, 進化条件を満たしているかどうかを調べる
     * @param poke         チェックするポケモン
     * @param pair_poke    交換相手のポケモン
     * @param evolve_data  進化データ
     * @param use_item     使用する道具
     * @retval true   進化条件を満たしている
     * @retval false  進化条件を満たしていない
     */
    bool EvolveManager::IsSatisfyEvolveCondition_TRADE( 
        const pml::pokepara::CoreParam* poke, 
        const pml::pokepara::CoreParam* pair_poke,
        const EvolveData& evolve_data ) const
    {
      switch( evolve_data.cond ) 
      {
      case pml::personal::EVOLVE_COND_TUUSHIN:  // 通信交換で無条件進化
        return true;

      case pml::personal::EVOLVE_COND_TUUSHIN_ITEM:  // 特定のアイテムを持った状態で通信交換
        return ( poke->GetItem() == evolve_data.cond_value );

      case pml::personal::EVOLVE_COND_TUUSHIN_YUUGOU:  // 特定の組み合わせで通信交換
        if( pair_poke == NULL ) return false;
        //BTS2096 片方が「かわらずの石」を持っていても進化する：の対応
        if( this->CheckItem_KAWARAZUNOISHI(pair_poke->GetItem()) ) return false;
        //相手がタマゴの場合も融合進化できない
        if( pair_poke->IsEgg(CHECK_BOTH_EGG) ) return false;
        // チョボマキ × カブルモ なら融合進化
        if( ( ( poke->GetMonsNo() == MONSNO_TYOBOMAKI ) && ( pair_poke->GetMonsNo() == MONSNO_KABURUMO ) ) ||
            ( ( poke->GetMonsNo() == MONSNO_KABURUMO ) && ( pair_poke->GetMonsNo() == MONSNO_TYOBOMAKI ) ) ) {
          return true;
        }
        break;
      }

      return false;  // 進化できない
    }






    //-------------------------------------------------------------------------
    /**
     * @brief メガ進化後のフォルムNo.を取得する
     * @param poke  チェック対象のポケモン
     * @return メガ進化後のフォルムNo.
     * @note メガ進化できない場合は, 現在のフォルムNo.を返す
     */
    //-------------------------------------------------------------------------
    FormNo EvolveManager::GetMegaEvolvedFormNo( const CoreParam* poke ) const
    {
      FormNo current_formno = poke->GetFormNo();

      // 既に進化済み
      if( poke->IsMegaEvolved() ) {
        return current_formno;
      }

      // メガ進化テーブルを読み込む
      MonsNo monsno = poke->GetMonsNo(); 
      pml::personal::LoadMegaEvolveTable( monsno );  

      u8 evolve_route_num = pml::personal::GetMegaEvolveRouteNum();
      for( u32 i=0; i<evolve_route_num; i++ )
      {
        FormNo next_formno = pml::personal::GetMegaEvolvedFormNo(i);
        pml::personal::MegaEvolveCond cond = pml::personal::GetMegaEvolveCondition(i); 
        u16 param = pml::personal::GetMegaEvolveParam( i );

        bool evolve_enable = IsSatisfyEvolveCondition_MEGA( poke->GetItem(), poke , cond, param );
        if( evolve_enable ) {
          return next_formno;
        } 
      }
      return current_formno;
    }

    /**
     * @brief ポケモンと進化データを照らし合わせ, 進化条件を満たしているかどうかを調べる
     * @param itemno  保持しているアイテム
     * @param poke    CoreParam(レックウザ用に追加
     * @param cond    メガ進化条件
     * @param param   メガ進化条件についてのパラメータ
     * @retval true   進化条件を満たしている
     * @retval false  進化条件を満たしていない
     *
     * レックウザは、技「ガリョウテンセイ」を覚えているなら、メガ進化できる
     * ただし、道具「ピース」を持っているなら、メガ進化できない
     */
    bool EvolveManager::IsSatisfyEvolveCondition_MEGA( 
        u16 itemno,  const CoreParam* poke ,
        pml::personal::MegaEvolveCond cond, u16 param ) const
    {
      switch( cond )
      {
      case pml::personal::MEGA_EVOLVE_COND_ITEM:  // メガ進化専用アイテムを所持
        return ( itemno == param );

      case pml::personal::MEGA_EVOLVE_COND_REKKUUZA:  // レックウザ特殊
        if( poke == NULL )
        {
          GFL_ASSERT(0);
          return false;
        }
        if( item::ITEM_CheckPiece( itemno ) )
        {
          return false;
        }
        return ( poke->HaveWaza( WAZANO_GARYOUTENSEI ) );
      }

      return false;  // 進化できない
    }


    /**
     * @brief   メガ進化対応アイテムかどうかの判定
     * @param[in] monsno  対象となるポケモンのナンバー
     * @param[in] itemno  どうぐID
     * @return  bool  trueのとき、メガ進化条件にあてはまるアイテム
     */
    bool EvolveManager::IsMegaEvolveItem( MonsNo monsno, u16 itemno ) const
    {
      // メガ進化テーブルを読み込む
      pml::personal::LoadMegaEvolveTable( monsno );  

      u8 evolve_route_num = pml::personal::GetMegaEvolveRouteNum();
      for( u32 i=0; i<evolve_route_num; i++ )
      {
        //FormNo next_formno = pml::personal::GetMegaEvolvedFormNo(i);
        pml::personal::MegaEvolveCond cond = pml::personal::GetMegaEvolveCondition(i); 
        u16 param = pml::personal::GetMegaEvolveParam( i );
        if( cond == pml::personal::MEGA_EVOLVE_COND_ITEM
            && IsSatisfyEvolveCondition_MEGA( itemno, NULL,cond, param ) )
        {
          return true;
        } 
      }
      return false;
    }



  } // namespace pokepara
} // namespace pml 
