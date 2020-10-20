//======================================================================
/**
 * @file KisekaeDataBase.cpp
 * @date 2015/10/18 11:58:21
 * @author uchida_yuto
 * @brief きせかえデータベース（roプロジェクトなので注意）
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#include "KisekaeData/include/KisekaeDataBase.h"

//  nijiのインクルード
#include "System/include/DressUp.h"
#include "System/include/PokemonVersion.h"

//  メッセージのインクルード
#include <niji_conv_header/message/msg_kisekae.h>
#include <niji_conv_header/message/msg_kisekae_item_name.h>
#include <niji_conv_header/message/msg_kisekae_color.h>

//  リソースのインクルード
#include <arc_index/kisekae_icon.gaix>

//  きせかえのcdatのインクルード
//  cdatファイルをインクルードする前に事前にインクルードするファイルが必要
//  なぜならcdatファイルの構造体内でenum/defineの定義名を参照しているから
/*
  //  着せ替えのアイテムid
  "System/include/DressUp.h"
  //  テキスト
  <niji_conv_header/message/msg_kisekae.h>
  //  アイコンのテクスチャーid
  <arc_index/kisekae_icon.gaix>
*/

#include "niji_conv_header/app/shop_kisekae/data/GoodsData_bag.cdat"
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_acceye.cdat"
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_bottoms.cdat"
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_hat.cdat"
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_acchair.cdat"
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_legs.cdat"
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_shoes.cdat"
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_tops.cdat"

//  ショップのcdatデータ
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Moon_shop_data_00.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Moon_shop_data_01.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Moon_shop_data_02.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Moon_shop_data_03.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Moon_shop_data_04.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Moon_shop_data_05.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Moon_shop_data_06.cdat"

#include "niji_conv_header/app/shop_kisekae/data/ShopData_Sun_shop_data_00.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Sun_shop_data_01.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Sun_shop_data_02.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Sun_shop_data_03.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Sun_shop_data_04.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Sun_shop_data_05.cdat"
#include "niji_conv_header/app/shop_kisekae/data/ShopData_Sun_shop_data_06.cdat"

//  色染めショップのcdatデータ
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_00.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_01.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_02.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_03.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_04.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_05.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_06.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_07.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_08.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_09.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_10.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_11.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_12.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_13.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_14.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_15.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_16.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_17.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_18.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_19.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_20.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_21.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_22.cdat"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_color_shop_23.cdat"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(Kisekae)

  //  きせかえのカテゴリーデータ
  struct LOCAL_GOODS_CATEGORY_DATA
  {
    Savedata::Fashion::Category category;
    const table_data_GoodsData* pData;
    u32 dataNum;
  };

  //  ショップのデータ
  struct LOCAL_SHOPS_DATA_SUN
  {
    const table_data_ShopData_Sun*  pData;
    u32 dataNum;
  };

  struct LOCAL_SHOPS_DATA_MOON
  {
    const table_data_ShopData_Moon*  pData;
    u32 dataNum;
  };

  //  色染めショップのデータ
  struct LOCAL_COLOR_SHOPS_DATA
  {
    const table_data_DyeingData* pData;
    u32 dataNum;
  };

  //  ショップのデータ一覧(月版)
  static const LOCAL_SHOPS_DATA_MOON s_aShopsDataMoon[ KISEKAE_SHOP_ID_MAX ]  =
  {
    { ShopData_Moon_shop_data_00::table_shop_data_00_array, GFL_NELEMS( ShopData_Moon_shop_data_00::table_shop_data_00_array ) },
    { ShopData_Moon_shop_data_01::table_shop_data_01_array, GFL_NELEMS( ShopData_Moon_shop_data_01::table_shop_data_01_array ) },
    { ShopData_Moon_shop_data_02::table_shop_data_02_array, GFL_NELEMS( ShopData_Moon_shop_data_02::table_shop_data_02_array ) },
    { ShopData_Moon_shop_data_03::table_shop_data_03_array, GFL_NELEMS( ShopData_Moon_shop_data_03::table_shop_data_03_array ) },
    { ShopData_Moon_shop_data_04::table_shop_data_04_array, GFL_NELEMS( ShopData_Moon_shop_data_04::table_shop_data_04_array ) },
    { ShopData_Moon_shop_data_05::table_shop_data_05_array, GFL_NELEMS( ShopData_Moon_shop_data_05::table_shop_data_05_array ) },
    { ShopData_Moon_shop_data_06::table_shop_data_06_array, GFL_NELEMS( ShopData_Moon_shop_data_06::table_shop_data_06_array ) },
  };

  //  ショップのデータ一覧(太陽版)
  static const LOCAL_SHOPS_DATA_SUN s_aShopsDataSun[ KISEKAE_SHOP_ID_MAX ]  =
  {
    { ShopData_Sun_shop_data_00::table_shop_data_00_array, GFL_NELEMS( ShopData_Sun_shop_data_00::table_shop_data_00_array ) },
    { ShopData_Sun_shop_data_01::table_shop_data_01_array, GFL_NELEMS( ShopData_Sun_shop_data_01::table_shop_data_01_array ) },
    { ShopData_Sun_shop_data_02::table_shop_data_02_array, GFL_NELEMS( ShopData_Sun_shop_data_02::table_shop_data_02_array ) },
    { ShopData_Sun_shop_data_03::table_shop_data_03_array, GFL_NELEMS( ShopData_Sun_shop_data_03::table_shop_data_03_array ) },
    { ShopData_Sun_shop_data_04::table_shop_data_04_array, GFL_NELEMS( ShopData_Sun_shop_data_04::table_shop_data_04_array ) },
    { ShopData_Sun_shop_data_05::table_shop_data_05_array, GFL_NELEMS( ShopData_Sun_shop_data_05::table_shop_data_05_array ) },
    { ShopData_Sun_shop_data_06::table_shop_data_06_array, GFL_NELEMS( ShopData_Sun_shop_data_06::table_shop_data_06_array ) },
  };

  //  色染めショップのデータ一覧
  static const LOCAL_COLOR_SHOPS_DATA s_aColorShopsData[ KISEKAE_COLOR_SHOP_ID_MAX ] =
  {
    //  0
    { DyeingData_color_shop_00::table_color_shop_00_array, GFL_NELEMS( DyeingData_color_shop_00::table_color_shop_00_array ) },
    //  1
    { DyeingData_color_shop_01::table_color_shop_01_array, GFL_NELEMS( DyeingData_color_shop_01::table_color_shop_01_array ) },
    //  2
    { DyeingData_color_shop_02::table_color_shop_02_array, GFL_NELEMS( DyeingData_color_shop_02::table_color_shop_02_array ) },
    //  3
    { DyeingData_color_shop_03::table_color_shop_03_array, GFL_NELEMS( DyeingData_color_shop_03::table_color_shop_03_array ) },
    //  4
    { DyeingData_color_shop_04::table_color_shop_04_array, GFL_NELEMS( DyeingData_color_shop_04::table_color_shop_04_array ) },
    //  5
    { DyeingData_color_shop_05::table_color_shop_05_array, GFL_NELEMS( DyeingData_color_shop_05::table_color_shop_05_array ) },
    //  6
    { DyeingData_color_shop_06::table_color_shop_06_array, GFL_NELEMS( DyeingData_color_shop_06::table_color_shop_06_array ) },
    //  7
    { DyeingData_color_shop_07::table_color_shop_07_array, GFL_NELEMS( DyeingData_color_shop_07::table_color_shop_07_array ) },
    //  8
    { DyeingData_color_shop_08::table_color_shop_08_array, GFL_NELEMS( DyeingData_color_shop_08::table_color_shop_08_array ) },
    //  9
    { DyeingData_color_shop_09::table_color_shop_09_array, GFL_NELEMS( DyeingData_color_shop_09::table_color_shop_09_array ) },
    //  10
    { DyeingData_color_shop_10::table_color_shop_10_array, GFL_NELEMS( DyeingData_color_shop_10::table_color_shop_10_array ) },
    //  11
    { DyeingData_color_shop_11::table_color_shop_11_array, GFL_NELEMS( DyeingData_color_shop_11::table_color_shop_11_array ) },
    //  12
    { DyeingData_color_shop_12::table_color_shop_12_array, GFL_NELEMS( DyeingData_color_shop_12::table_color_shop_12_array ) },
    //  13
    { DyeingData_color_shop_13::table_color_shop_13_array, GFL_NELEMS( DyeingData_color_shop_13::table_color_shop_13_array ) },
    //  14
    { DyeingData_color_shop_14::table_color_shop_14_array, GFL_NELEMS( DyeingData_color_shop_14::table_color_shop_14_array ) },
    //  15
    { DyeingData_color_shop_15::table_color_shop_15_array, GFL_NELEMS( DyeingData_color_shop_15::table_color_shop_15_array ) },
    //  16
    { DyeingData_color_shop_16::table_color_shop_16_array, GFL_NELEMS( DyeingData_color_shop_16::table_color_shop_16_array ) },
    //  17
    { DyeingData_color_shop_17::table_color_shop_17_array, GFL_NELEMS( DyeingData_color_shop_17::table_color_shop_17_array ) },
    //  18
    { DyeingData_color_shop_18::table_color_shop_18_array, GFL_NELEMS( DyeingData_color_shop_18::table_color_shop_18_array ) },
    //  19
    { DyeingData_color_shop_19::table_color_shop_19_array, GFL_NELEMS( DyeingData_color_shop_19::table_color_shop_19_array ) },
    //  20
    { DyeingData_color_shop_20::table_color_shop_20_array, GFL_NELEMS( DyeingData_color_shop_20::table_color_shop_20_array ) },
    //  21
    { DyeingData_color_shop_21::table_color_shop_21_array, GFL_NELEMS( DyeingData_color_shop_21::table_color_shop_21_array ) },
    //  22
    { DyeingData_color_shop_22::table_color_shop_22_array, GFL_NELEMS( DyeingData_color_shop_22::table_color_shop_22_array ) },
    //  23
    { DyeingData_color_shop_23::table_color_shop_23_array, GFL_NELEMS( DyeingData_color_shop_23::table_color_shop_23_array ) },
  };

  //  きせかえデータ取得
  static const LOCAL_GOODS_CATEGORY_DATA* _GetKisekaeLocakData( const Savedata::Fashion::Category category )
  {
    //  きせかえのカテゴリーデータ一覧
    static const LOCAL_GOODS_CATEGORY_DATA s_aGoodsCategoryData[ Savedata::Fashion::CATEGORY_MAX ] =
    {
  //    CATEGORY_HAT,                    // 帽子
        {  Savedata::Fashion::CATEGORY_HAT, GoodsData_hat::table_hat_array, GFL_NELEMS( GoodsData_hat::table_hat_array ) },

  //    CATEGORY_ACCHAIR,                //  かみかざり
        {  Savedata::Fashion::CATEGORY_ACCHAIR, GoodsData_acchair::table_acchair_array, GFL_NELEMS( GoodsData_acchair::table_acchair_array ) },

  //    CATEGORY_ACCEYE,                 //  めがね
        {  Savedata::Fashion::CATEGORY_ACCEYE, GoodsData_acceye::table_acceye_array, GFL_NELEMS( GoodsData_acceye::table_acceye_array ) },

  //    CATEGORY_TOPS,                   // トップス
        {  Savedata::Fashion::CATEGORY_TOPS, GoodsData_tops::table_tops_array, GFL_NELEMS( GoodsData_tops::table_tops_array ) },

  //    CATEGORY_BOTTOMS,                // ボトムス
        {  Savedata::Fashion::CATEGORY_BOTTOMS, GoodsData_bottoms::table_bottoms_array, GFL_NELEMS( GoodsData_bottoms::table_bottoms_array ) },

  //    CATEGORY_LEGS,                   // レッグ
        {  Savedata::Fashion::CATEGORY_LEGS, GoodsData_legs::table_legs_array, GFL_NELEMS( GoodsData_legs::table_legs_array ) },

  //    CATEGORY_SHOES,                  // シューズ
        {  Savedata::Fashion::CATEGORY_SHOES, GoodsData_shoes::table_shoes_array, GFL_NELEMS( GoodsData_shoes::table_shoes_array ) },

  //    CATEGORY_BAG,                    // バッグ
        {  Savedata::Fashion::CATEGORY_BAG, GoodsData_bag::table_bag_array, GFL_NELEMS( GoodsData_bag::table_bag_array ) },
    };

    u32 loopNum = GFL_NELEMS( s_aGoodsCategoryData );
    for( u32 i = 0; i < loopNum; ++i )
    {
      if( s_aGoodsCategoryData[ i ].category == category )
      {
        return  &s_aGoodsCategoryData[ i ];
      }
    }

    return  NULL;
  }

  //  きせかえデータをカテゴリーごとに取得
  const table_data_GoodsData* KisekaeDataBase::GetGoodsData( const Savedata::Fashion::Category category, const u32 idx )
  {
    if( Savedata::Fashion::CATEGORY_MAX <= category )
    {
      GFL_ASSERT_MSG( 0, "カテゴリ指定に問題があります。category(%d)\n", category );
      return  NULL;
    }

    const LOCAL_GOODS_CATEGORY_DATA*  pGoodsData = _GetKisekaeLocakData( category );
    if( pGoodsData->dataNum <= idx )
    {
      GFL_ASSERT_MSG( 0, "カテゴリ指定に問題はありませんが、カテゴリーデータ内のidx値がデータ数を超えています。category(%d), idx(%d), dataNum(%d)\n", category, idx, pGoodsData->dataNum );
      return  NULL;
    }

    return  &pGoodsData->pData[ idx ];
  }

  //  着せ替えデータをカテゴリー/noから取得
  const table_data_GoodsData* KisekaeDataBase::GetGoodsDataSearchNo( const Savedata::Fashion::Category category, const u32 no )
  {
    if( Savedata::Fashion::CATEGORY_MAX <= category )
    {
      GFL_ASSERT_MSG( 0, "カテゴリ指定に問題があります。category(%d)\n", category );
      return  NULL;
    }

    const LOCAL_GOODS_CATEGORY_DATA*  pGoodsData = _GetKisekaeLocakData( category );
    if( pGoodsData->dataNum <= no )
    {
      GFL_ASSERT_MSG( 0, "カテゴリ指定に問題はありませんが、カテゴリーデータ内にno値がデータ数を超えています。category(%d), no(%d), dataNum(%d)\n", category, no, pGoodsData->dataNum );
      return  NULL;
    }

    for( u32 i = 0; i < pGoodsData->dataNum; ++i )
    {
      if( pGoodsData->pData[ i ].no == (s32)no )
      {
        return &pGoodsData->pData[ no ];
      }
    }
    
    return  NULL;
  }

  //  性別/カテゴリ/アイテムidからきせかえデータ取得
  const table_data_GoodsData*  KisekaeDataBase::GetGoodsDataSearchItemId( const poke_3d::model::DressUpParam::Sex sex, const Savedata::Fashion::Category category, const u32 itemId )
  {
    if( Savedata::Fashion::CATEGORY_MAX <= category )
    {
      GFL_ASSERT_MSG( 0, "カテゴリ指定に問題があります。category(%d)\n", category );
      return  NULL;
    }

    const LOCAL_GOODS_CATEGORY_DATA*  pGoodsData = _GetKisekaeLocakData( category );
    for( u32 idx = 0; idx < pGoodsData->dataNum; ++idx )
    {
      u32 chkItemId = pGoodsData->pData[ idx ].item_id;
      if( pGoodsData->pData[ idx ].sex == sex )
      {
        if( chkItemId == itemId )
        {
          //  ヒット！
          return  &pGoodsData->pData[ idx ];
        }
      }
      else
      {
        continue;
      }
    }

    return  NULL;
  }

  //  カテゴリーごとの着せ替えデータを取得
  u32 KisekaeDataBase::GetGoodsDataNum( const Savedata::Fashion::Category category )
  {
    if( Savedata::Fashion::CATEGORY_MAX <= category )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    const LOCAL_GOODS_CATEGORY_DATA*  pGoodsData = _GetKisekaeLocakData( category );
    return pGoodsData->dataNum;
  }

  //  ショップの購入データを取得
  const KisekaeDataBase::SHOP_DATA KisekaeDataBase::GetShopData( const KisekaeShopID id, const u32 dataIdx )
  {
    SHOP_DATA shopData;

    u32 listNum = GetShopsDataNum( id );
    if( listNum <= 0 )
    {
      return  shopData;
    }
    else
    if( listNum <= dataIdx )
    {
      GFL_ASSERT( 0 );
      return  shopData;
    }

    //  アイテム名を取得
    const char* pItemId = NULL;
    if( GET_VERSION() == VERSION_SUN2 )
    {
      pItemId = s_aShopsDataSun[ id ].pData[ dataIdx ].item_id;
    }
    else
    {
      pItemId = s_aShopsDataMoon[ id ].pData[ dataIdx ].item_id;
    }

    // 全カテゴリーの名前チェックをする
    {
      for( u32 i = 0; i < Savedata::Fashion::CATEGORY_MAX; ++i )
      {
        u32 goodsNum  = GetGoodsDataNum( (Savedata::Fashion::Category)i );
        for( u32 goods_i = 0; goods_i < goodsNum; ++goods_i )
        {
          const table_data_GoodsData* pGoogdData  = GetGoodsData( (Savedata::Fashion::Category)i, goods_i );
          //  名前チェック
          if( pGoogdData->data_id != NULL )
          {
            if( std::strcmp( pGoogdData->data_id, pItemId ) == 0 )
            {
              //  ヒット!
              shopData.category   = (Savedata::Fashion::Category)i;
              shopData.listIdx    = goods_i;
              shopData.pGoodsData = pGoogdData;

              return  shopData;
            }
          }
        }
      }
    }
    
    //  ヒットしないのはおかしい
    GFL_ASSERT( 0 );
    return  shopData;
  }

  //  ショップの購入リスト数を取得
  u32 KisekaeDataBase::GetShopsDataNum( const KisekaeShopID id )
  {
    if( GET_VERSION() == VERSION_SUN2 )
    {
      if( GFL_NELEMS( s_aShopsDataSun ) <= id )
      {
        GFL_ASSERT( 0 );
        return  0;
      }

      return s_aShopsDataSun[ id ].dataNum;
    }
    else
    {
      if( GFL_NELEMS( s_aShopsDataMoon ) <= id )
      {
        GFL_ASSERT( 0 );
        return  0;
      }

      return s_aShopsDataMoon[ id ].dataNum;
    }
  }

  //  いろそめショップidと色idから値段を取得
  const table_data_DyeingData* KisekaeDataBase::GetColorShopDataSearchColorId( const u32 shopId, const u32 colorId )
  {
    if( GFL_NELEMS( s_aColorShopsData ) <= shopId )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    const LOCAL_COLOR_SHOPS_DATA* pCurrentData  = &s_aColorShopsData[ shopId ];
    for( u32 i = 0; i < pCurrentData->dataNum; ++i )
    {
      if( pCurrentData->pData[ i ].color_id == (s32)colorId )
      {
        return  &pCurrentData->pData[ i ];
      }
    }

    return  NULL;
  }

  //  指定したidのいろそめショップのデータ数取得
  u32 KisekaeDataBase::GetColorShopDataNum( const u32 shopId )
  {
    if( GFL_NELEMS( s_aColorShopsData ) <= shopId )
    {
      GFL_ASSERT( 0 );
      return  0;
    }

    const LOCAL_COLOR_SHOPS_DATA* pCurrentData  = &s_aColorShopsData[ shopId ];
    return  pCurrentData->dataNum;
  }

  //  指定したidのいろそめショップからリストidxを指定データ取得
  const table_data_DyeingData* KisekaeDataBase::GetColorShopData( const u32 shopId, const u32 listIdx )
  {
    if( GFL_NELEMS( s_aColorShopsData ) <= shopId )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    const LOCAL_COLOR_SHOPS_DATA* pCurrentData  = &s_aColorShopsData[ shopId ];
    if( pCurrentData->dataNum <= listIdx )
    {
      GFL_ASSERT( 0 );
      return  NULL;
    }

    return  &pCurrentData->pData[ listIdx ];
  }

  // 色idに対応するメッセージラベルを取得
  u32 KisekaeDataBase::GetColorMsgIdForColorId( const u32 colorId )
  {
    static const u32 colorMsgIdTable[ DRESSUP_COLOR_ID_CLOTH_COUNT ] =
    {
      kisekae_color_01_01,			// DRESSUP_COLOR_ID_CLOTH_WH 	ホワイト
      kisekae_color_01_02,			// DRESSUP_COLOR_ID_CLOTH_BK 	ブラック
      kisekae_color_01_05,			// DRESSUP_COLOR_ID_CLOTH_RD 	レッド
      kisekae_color_01_12,			// DRESSUP_COLOR_ID_CLOTH_YE 	イエロー
      kisekae_color_01_10,			// DRESSUP_COLOR_ID_CLOTH_GR 	グリーン
      kisekae_color_01_06,			// DRESSUP_COLOR_ID_CLOTH_BL 	ブルー
      kisekae_color_01_11,			// DRESSUP_COLOR_ID_CLOTH_OR 	オレンジ
      kisekae_color_01_09,			// DRESSUP_COLOR_ID_CLOTH_NB 	ネイビー
      kisekae_color_01_08,			// DRESSUP_COLOR_ID_CLOTH_PP 	パープル
      kisekae_color_01_07,			// DRESSUP_COLOR_ID_CLOTH_PN 	ピンク
      kisekae_color_01_03,			// DRESSUP_COLOR_ID_CLOTH_GL 	グレー
      kisekae_color_01_04,			// DRESSUP_COLOR_ID_CLOTH_BE 	ベージュ
      kisekae_color_03_01,			// DRESSUP_COLOR_ID_CLOTH_RD1	"レッド染め１ （パステル）"
      kisekae_color_02_01,			// DRESSUP_COLOR_ID_CLOTH_RD2	"レッド染め２ （ビビット）"
      kisekae_color_04_01,			// DRESSUP_COLOR_ID_CLOTH_RD3	"レッド染め３ （ダーク）"
      kisekae_color_03_08,			// DRESSUP_COLOR_ID_CLOTH_YE1	"イエロー染め１ （パステル）"
      kisekae_color_02_08,			// DRESSUP_COLOR_ID_CLOTH_YE2	"イエロー染め２ （ビビット）"
      kisekae_color_04_08,			// DRESSUP_COLOR_ID_CLOTH_YE3	"イエロー染め３ （ダーク）"
      kisekae_color_03_06,			// DRESSUP_COLOR_ID_CLOTH_GR1	"グリーン染め１ （パステル）"
      kisekae_color_02_06,			// DRESSUP_COLOR_ID_CLOTH_GR2	"グリーン染め２ （ビビット）"
      kisekae_color_04_06,			// DRESSUP_COLOR_ID_CLOTH_GR3	"グリーン染め３ （ダーク）"
      kisekae_color_03_02,			// DRESSUP_COLOR_ID_CLOTH_BL1	"ブルー染め１ （パステル）"
      kisekae_color_02_02,			// DRESSUP_COLOR_ID_CLOTH_BL2	"ブルー染め２ （ビビット）"
      kisekae_color_04_02,			// DRESSUP_COLOR_ID_CLOTH_BL3	"ブルー染め３ （ダーク）"
      kisekae_color_03_07,			// DRESSUP_COLOR_ID_CLOTH_OR1	"オレンジ染め１ （パステル）"
      kisekae_color_02_07,			// DRESSUP_COLOR_ID_CLOTH_OR2	"オレンジ染め２ （ビビット）"
      kisekae_color_04_07,			// DRESSUP_COLOR_ID_CLOTH_OR3	"オレンジ染め３ （ダーク）"
      kisekae_color_03_05,			// DRESSUP_COLOR_ID_CLOTH_NB1	"ネイビー染め１ （パステル）"
      kisekae_color_02_05,			// DRESSUP_COLOR_ID_CLOTH_NB2	"ネイビー染め２ （ビビット）"
      kisekae_color_04_05,			// DRESSUP_COLOR_ID_CLOTH_NB3	"ネイビー染め３ （ダーク）"
      kisekae_color_03_04,			// DRESSUP_COLOR_ID_CLOTH_PP1	"パープル染め１ （パステル）"
      kisekae_color_02_04,			// DRESSUP_COLOR_ID_CLOTH_PP2	"パープル染め２ （ビビット）"
      kisekae_color_04_04,			// DRESSUP_COLOR_ID_CLOTH_PP3	"パープル染め３ （ダーク）"
      kisekae_color_03_03,			// DRESSUP_COLOR_ID_CLOTH_PN1	"ピンク染め１ （パステル）"
      kisekae_color_02_03,			// DRESSUP_COLOR_ID_CLOTH_PN2	"ピンク染め２ （ビビット）"
      kisekae_color_04_03,			// DRESSUP_COLOR_ID_CLOTH_PN3	"ピンク染め３ （ダーク）"
      kisekae_color_06_03,			// DRESSUP_COLOR_ID_CLOTH_LG 	染め用ライトグレー
    };

    if( colorId < DRESSUP_COLOR_ID_CLOTH_COUNT )
    {
      return colorMsgIdTable[ colorId ];
    }

    GFL_ASSERT_MSG( 0, "%d", colorId );
    return kisekae_color_01_01;
  }

GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)
