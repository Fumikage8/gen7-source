//=============================================================================
/**
 * @brief 全力技システム
 * @file pml_AuraWazaSystem.cpp
 * @author obata_toshihiro
 * @date 2015.05.27
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include "niji_conv_header/poke_lib/item/itemsym.h"
#include <pml/include/personal/pml_PersonalSystem.h>
#include <pml/include/waza/pml_WazaDataSystem.h>
#include <pml/include/waza/pml_ZenryokuWazaSystem.h>
#include <pml/include/pml_PokePara.h>


namespace pml {
  namespace waza { 


    namespace {


      // タイプ別、汎用全力技を使うための道具
      static const ItemNo GENERAL_ZENRYOKUWAZA_ITEMS[ POKETYPE_MAX ] = 
      {
        /* [ POKETYPE_NORMAL  ] */ ITEM_NOOMARUBIIZU_1,
        /* [ POKETYPE_KAKUTOU ] */ ITEM_KAKUTOUBIIZU_1,
        /* [ POKETYPE_HIKOU   ] */ ITEM_HIKOUBIIZU_1,
        /* [ POKETYPE_DOKU    ] */ ITEM_DOKUBIIZU_1,
        /* [ POKETYPE_JIMEN   ] */ ITEM_ZIMENBIIZU_1,
        /* [ POKETYPE_IWA     ] */ ITEM_IWABIIZU_1,
        /* [ POKETYPE_MUSHI   ] */ ITEM_MUSIBIIZU_1,
        /* [ POKETYPE_GHOST   ] */ ITEM_GOOSUTOBIIZU_1,
        /* [ POKETYPE_HAGANE  ] */ ITEM_HAGANEBIIZU_1,
        /* [ POKETYPE_HONOO   ] */ ITEM_HONOOBIIZU_1,
        /* [ POKETYPE_MIZU    ] */ ITEM_MIZUBIIZU_1,
        /* [ POKETYPE_KUSA    ] */ ITEM_KUSABIIZU_1,
        /* [ POKETYPE_DENKI   ] */ ITEM_DENKIBIIZU_1,
        /* [ POKETYPE_ESPER   ] */ ITEM_ESUPAABIIZU_1,
        /* [ POKETYPE_KOORI   ] */ ITEM_KOORIBIIZU_1,
        /* [ POKETYPE_DRAGON  ] */ ITEM_DORAGONBIIZU_1,
        /* [ POKETYPE_AKU     ] */ ITEM_AKUBIIZU_1,
        /* [ POKETYPE_FAIRY   ] */ ITEM_FEARIIBIIZU_1,
      };


      /**
       * @brief 全力技の情報
       */
      struct ZenryokuWazaDesc
      {
        WazaNo           wazaNo;           // 全力技の技No.( 全力技に変換できない場合 WAZANO_NULL )
        ZenryokuWazaKind zenryokuWazaKind; // 全力技の種類 ( 全力技に変換できない場合 ZENRYOKUWAZA_KIND_NULL )
      };
    
      /**
       * @brief 全力技の情報を取得する
       *
       * @param[in]  source  全力技の元
       * @param[out] desc    全力技の情報
       */
      void GetZenryokuWazaDesc( const ZenryokuWazaSystem::ZenryokuWazaSource& source, ZenryokuWazaDesc* desc )
      {
        if( source.wazaNo == WAZANO_NULL )
        {
          desc->wazaNo = WAZANO_NULL;
          desc->zenryokuWazaKind = ZENRYOKUWAZA_KIND_NULL;
          return;
        }

        // 専用全力技
        pml::personal::LoadPersonalData( source.monsNo, source.formNo );
        const ItemNo specialZenryokuWazaItem   = static_cast<ItemNo>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_zenryoku_waza_item ) );
        const WazaNo specialZenryokuWazaSource = static_cast<WazaNo>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_zenryoku_waza_before ) );
        if( ( specialZenryokuWazaItem   == source.itemNo ) &&
            ( specialZenryokuWazaSource == source.wazaNo ) )
        {
          desc->wazaNo = static_cast<WazaNo>( pml::personal::GetPersonalParam( pml::personal::PARAM_ID_zenryoku_waza_after ) );
          desc->zenryokuWazaKind = ZENRYOKUWAZA_KIND_SPECIAL;
          return;
        }

        // 汎用全力技
        // ブースト技
        const PokeType wazaType = pml::wazadata::GetType( source.wazaNo );
        const ItemNo generalPiece = ZenryokuWazaSystem::GetItemToUseGeneralZenryokuWaza( wazaType );
        if( source.itemNo == generalPiece ) 
        {
          const WazaNo zenryokuWazaNo = pml::wazadata::GetZenryokuWazaNo( source.wazaNo );

          // 元技に「全力技」が設定されているなら、汎用全力技
          if( zenryokuWazaNo != WAZANO_NULL ) 
          {
            desc->wazaNo = zenryokuWazaNo;
            desc->zenryokuWazaKind = ZENRYOKUWAZA_KIND_GENERAL;
            return;
          }

          // 元技に「全力技」が設定されていない非ダメージ技なら、ブースト技
          if( ( zenryokuWazaNo == WAZANO_NULL ) &&
             !( pml::wazadata::IsDamage( source.wazaNo ) ) ) 
          {
             desc->wazaNo = source.wazaNo;
             desc->zenryokuWazaKind = ZENRYOKUWAZA_KIND_BOOST;
             return;
          }

          // 例外：一部の派生技をブースト技として扱う
          // ダメージ技として登録されている派生技は、派生後の技を全力技として実行するため、派生技自体はブースト技として扱う
          if( ( source.wazaNo == WAZANO_SAKIDORI ) ||
              ( source.wazaNo == WAZANO_WHEZAABOORU ) )
          {
            desc->wazaNo = source.wazaNo;
            desc->zenryokuWazaKind = ZENRYOKUWAZA_KIND_BOOST;
            return;
          }
        }

        desc->wazaNo = WAZANO_NULL;
        desc->zenryokuWazaKind = ZENRYOKUWAZA_KIND_NULL;
      }


    } // namespace
    


    //---------------------------------------------------------------------
    /**
     * @brief 汎用全力技を使用するための道具を取得するzenryokuWazaKind
     *
     * @param wazaType  技のタイプ
     *
     * @retval ITEM_DUMMY_DATA  対応する道具が存在しない場合( 引数が不正値の場合 )
     */
    //---------------------------------------------------------------------
    ItemNo ZenryokuWazaSystem::GetItemToUseGeneralZenryokuWaza( PokeType wazaType )
    {
      if( POKETYPE_MAX <= wazaType ) {
        GFL_ASSERT(0); // 引数が不正値
        return ITEM_DUMMY_DATA;
      }
      return GENERAL_ZENRYOKUWAZA_ITEMS[ wazaType ];
    }


    //---------------------------------------------------------------------
    /**
     * @brief 全力技を使用する条件を満たしているか？
     *
     * @param[in]  source  全力技の元
     *
     * @retval true   全力技を使用する条件を満たしている
     * @retval false  全力技を使用する条件を満たしていない
     *
     * @note 専用全力技の条件：　　元技、道具、種族　の組み合わせが一致している
     * @note 汎用全力技の条件：　　元技、道具　　　　の組み合わせが一致している( 元技がダメージ技の場合 )
     * @note 全力ブーストの条件：　元技、道具　　　　の組み合わせが一致している( 元技が非ダメージ技の場合 )
     */
    //---------------------------------------------------------------------
    bool ZenryokuWazaSystem::IsZenryokuWazaEnable( const ZenryokuWazaSource& source )
    {
      ZenryokuWazaDesc desc;
      GetZenryokuWazaDesc( source, &desc );
      return ( desc.zenryokuWazaKind != ZENRYOKUWAZA_KIND_NULL );
    }

    //---------------------------------------------------------------------
    /**
     * @brief 全力技を使用する条件を満たしているか？
     *
     * @param[in]  pp     ポケモンパラメータ
     * @param[in]  item   アイテム番号
     *
     * @retval true   全力技を使用する条件を満たしている
     * @retval false  全力技を使用する条件を満たしていない
     *
     * @note 専用全力技の条件：　　元技、道具、種族　の組み合わせが一致している
     * @note 汎用全力技の条件：　　元技、道具　　　　の組み合わせが一致している( 元技がダメージ技の場合 )
     * @note 全力ブーストの条件：　元技、道具　　　　の組み合わせが一致している( 元技が非ダメージ技の場合 )
     */
    //---------------------------------------------------------------------
    bool ZenryokuWazaSystem::IsZenryokuWazaEnable( const pml::pokepara::PokemonParam * pp, ItemNo item )
    {
      ZenryokuWazaSource src;
      src.monsNo = pp->GetMonsNo();
      src.formNo = pp->GetFormNo();
      src.itemNo = item;
      for( u32 i=0; i<pml::MAX_WAZA_NUM; i++ )
      {
        src.wazaNo = pp->GetWazaNo( i );
        if( IsZenryokuWazaEnable( src ) != false )
        {
          return true;
        }
      }
      return false;
    }

    //---------------------------------------------------------------------
    /**
     * @brief 全力技の種類を取得する
     *
     * @param[in]  source  全力技の元
     *
     * @return 全力技の種類
     * @retval ZENRYOKUWAZA_KIND_NULL  全力技に変換できない場合
     */
    //---------------------------------------------------------------------
    ZenryokuWazaKind ZenryokuWazaSystem::GetZenryokuWazaKind( const ZenryokuWazaSource& source )
    {
      ZenryokuWazaDesc desc;
      GetZenryokuWazaDesc( source, &desc );
      return desc.zenryokuWazaKind;
    }

    //---------------------------------------------------------------------
    /**
     * @brief 全力技の技No.を取得する
     *
     * @param[in]  source  全力技の元
     * 
     * @return 全力技の技No.
     * @retval WAZANO_NULL  全力技に変換できない場合
     */
    //---------------------------------------------------------------------
    WazaNo ZenryokuWazaSystem::GetZenryokuWazaNo( const ZenryokuWazaSource& source )
    {
      ZenryokuWazaDesc desc;
      GetZenryokuWazaDesc( source, &desc );
      return desc.wazaNo;
    }


  } // namespace waza
} // namespace pml
