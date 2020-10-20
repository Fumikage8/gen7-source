//======================================================================
/**
 * @file KisekaeDataBase.h
 * @date 2015/10/18 11:57:55
 * @author uchida_yuto
 * @brief きせかえのデータベース
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __KISEKAE_DATA_BASE_HEADER_H_INCLUDED__
#define __KISEKAE_DATA_BASE_HEADER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include "Savedata/include/FashionSave.h"

#include "KisekaeData/include/KisekaeShopID.h"

//  データ構造体
#include "niji_conv_header/app/shop_kisekae/data/GoodsData_CommonStructFormat.h"
#include "niji_conv_header/app/shop_kisekae/data/DyeingData_CommonStructFormat.h"

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)
GFL_NAMESPACE_BEGIN(Kisekae)
  /**
   * @class KisekaeDataBase
   * @brief きせかえのデータベース
   */
  class KisekaeDataBase
  {
    GFL_FORBID_COPY_AND_ASSIGN(KisekaeDataBase);
  public:
    struct SHOP_DATA
    {
      SHOP_DATA()
      {
        category    = Savedata::Fashion::CATEGORY_MAX;
        pGoodsData  = NULL;
        listIdx     = 0;
      }

      Savedata::Fashion::Category category;
      const table_data_GoodsData* pGoodsData;

      u32   listIdx;
    };

    //  きせかえデータをカテゴリーごとに取得
    static const table_data_GoodsData* GetGoodsData( const Savedata::Fashion::Category category, const u32 idx );

    //  着せ替えデータをカテゴリー/noから取得
    static const table_data_GoodsData* GetGoodsDataSearchNo( const Savedata::Fashion::Category category, const u32 no );

    //  性別/カテゴリ/アイテムidからきせかえデータ取得
    static const table_data_GoodsData*  GetGoodsDataSearchItemId( const poke_3d::model::DressUpParam::Sex sex, const Savedata::Fashion::Category category, const u32 itemId );

    //  きせかえのカテゴリごとのリストデータ総数を取得
    static u32 GetGoodsDataNum( const Savedata::Fashion::Category category );


    //  販売データリスト取得
    //  サイズは小さいのでデータコピーしても平気
    static const SHOP_DATA GetShopData( const KisekaeShopID id, const u32 dataIdx );
    static u32 GetShopsDataNum( const KisekaeShopID id );

    //  いろそめショップidと色idからデータを取得
    static const table_data_DyeingData* GetColorShopDataSearchColorId( const u32 shopId, const u32 colorId );

    //  指定したidのいろそめショップのデータ数取得
    static u32 GetColorShopDataNum( const u32 shopId );

    //  指定したidのいろそめショップからリストidxを指定データ取得
    static const table_data_DyeingData* GetColorShopData( const u32 shopId, const u32 listIdx );

    // 色idに対応するメッセージラベルを取得
    static u32 GetColorMsgIdForColorId( const u32 colorId );

  };

GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

#endif // __KISEKAE_DATA_BASE_HEADER_H_INCLUDED__