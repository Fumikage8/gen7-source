//=============================================================================
/**
 * @file    ShopDef.h
 * @brief   ショップ購入画面汎用定義
 * @author  Hiroyuki Nakamura
 * @date    2015.10.08
 */
//=============================================================================
#if !defined( SHOP_DEF_H_INCLUDED )
#define SHOP_DEF_H_INCLUDED

#pragma once

// gflib
#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_heap.h>



GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Shop)
struct APP_PARAM;


//! ショップタイプ
enum ShopType
{
  TYPE_FR_SHOP = 0,   //!< フレンドリィショップ
  TYPE_BP_SHOP,       //!< バトルポイント
  TYPE_FC_ITEM,       //!< フェスコイン：土産物
  TYPE_FC_ACTION,     //!< フェスコイン：行動系
  TYPE_BP_WAZA,       //!< バトルポイント：技
  TYPE_MAX            //!< ショップタイプ最大値
};

//! 商品データ
struct SHOP_ITEM
{
  u32 id;       //!< 商品ID
  u32 price;    //!< 値段

  // TYPE_FC_ITEM用
  u16 item;     //!< 道具
  u16 num;      //!< 個数
};


//==============================================================================
/**
  *  @class  IShopFeatureDelegate
  *  @brief  Shop機能の一部を外部に移譲するためのオブジェクト
  *  @note   FesCircle用
  *  @date   2015/11/09 Mon. 15:14:56
  */
//==============================================================================
class IShopFeatureDelegate
{
public:
  //------------------------------------------------------------------
  /**
    *  @brief    SHOP_ITEM配列（商品リスト）の構築
    */
  //------------------------------------------------------------------
  virtual void  ShopFeatureDelegate_CreateShopItemList(SHOP_ITEM** ppShopItemList, u32& rShopItemNum, gfl2::heap::HeapBase* pHeapBase, const u32 tableIndex) = 0;

  //------------------------------------------------------------------
  /**
    *  @brief    結果を処理する
    */
  //------------------------------------------------------------------
  virtual void  ShopFeatureDelegate_HandlingResult(const APP_PARAM& rAppParam) = 0;


  /** dtor  */
  virtual ~IShopFeatureDelegate(){}
};



GFL_NAMESPACE_END(Shop)
GFL_NAMESPACE_END(App)

#endif  // SHOP_DEF_H_INCLUDED
