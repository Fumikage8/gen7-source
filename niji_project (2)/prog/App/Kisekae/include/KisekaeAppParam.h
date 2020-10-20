// ============================================================================
/*
 * @file KisekaeAppParam.h
 * @brief 着せ替えアプリを起動する為のパラメータです。
 * @date 2015.10.08
 * @author uchida_yuto
 */
// ============================================================================
#if !defined( KISEKAE_APPPARAM_H_INCLUDE )
#define KISEKAE_APPPARAM_H_INCLUDE
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_macros.h>
#include <util/include/gfl2_std_string.h>

//  nijiのインクルード
#include "KisekaeData/include/KisekaeShopID.h"

//  色定義
#include "niji_conv_header/battle/dressup/dressup_color_id.h"

#include <model/include/gfl2_DressUpParam.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Kisekae)

  //  イベント起動タイプ
  enum EventRunType
  {
    //  きせかえショップ
    EVENT_RUN_TYPE_SHOP = 0,

    //  きせかえをする
    EVENT_RUN_TYPE_DRESS_UP,

    //  色染め
    EVENT_RUN_TYPE_COLOR_SHOP,

    //  ファインダースタジオアプリの着せ替え
    EVENT_RUN_TYPE_FINDER_STUDIO,

    EVENT_RUN_TYPE_NONE,
  };

  enum
  {
    //  色染めショップの全色を選択できるようにする
    ALL_COLOR_SELECT  = -1,
  };

  //  呼び出し側が設定するパラメータ
  struct APP_PARAM
  {
    APP_PARAM()
    {
      runType = EVENT_RUN_TYPE_NONE;

      gfl2::std::MemClear( &typeData, sizeof( typeData ) );
    }

    //  イベント起動設定
    EventRunType runType;
    //  着せ替えデータ(ファインダースタジオ内で着ている格好を渡す)
    poke_3d::model::DressUpParam inputDressUpParam;

    union TYPE_DATA
    {
      //  runType == EVENT_RUN_TYPE_SHOP であれば有効な変数群
      struct  SHOP
      {
        //  ショップid
        app::util::Kisekae::KisekaeShopID shopId;
      } shop;

      //  runType == EVENT_RUN_TYPE_COLOR_SHOP であれば有効な変数群
      struct COLOR_SHOP
      {
        //  色染めショップid
        u32 shopId;

        //  染める色id
        s32 colorId;

        //  きのみ割引にする
        bool  bUseKinomiDiscount;

      } colorShop;

    } typeData;

    struct OUT_PUT
    {
      OUT_PUT()
      {
        bKinomiChangeColor  = false;
      }

      //  木の実で色染めした時にフラグを立てる（デフォルト色である白とライトグレー時にはfalseになる）
      bool  bKinomiChangeColor;

      poke_3d::model::DressUpParam dressUpParam;
    } output;
  };

GFL_NAMESPACE_END(Kisekae)
GFL_NAMESPACE_END(App)

#endif // KISEKAE_APPPARAM_H_INCLUDE
