//============================================================================================
/**
 * @file		FashionSave.h
 * @brief		きせかえセーブデータ
 * @author	uchida_yuto
 * @date		15.10.09
 *
 * namespace	savedata::
 */
//============================================================================================

#ifndef __SV_FASHION_H__
#define __SV_FASHION_H__

#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <util/include/gfl2_std_string.h>

//  nijiのインクルード
#include "model/include/gfl2_DressUpParam.h"
#include "System/include/DressUp.h"

#include "SaveDataInterface.h"

GFL_NAMESPACE_BEGIN(Savedata)

//  きせかえ購入セーブデータ
class Fashion : public SaveDataInterface
{
  GFL_FORBID_COPY_AND_ASSIGN(Fashion); // コピーコンストラクタ＋代入禁止
  
public:
  // １カテゴリーのきせかえアイテム最大数
  // セーブデータ内でのカテゴリー定義
  // @caution niji_reference_fiels/script/FieldPawnTypes.h に定義の複製があります
  enum Category
  {
    CATEGORY_TOPS,                   // トップス
    CATEGORY_BOTTOMS,                // ボトムス
    CATEGORY_LEGS,                   // レッグ
    CATEGORY_SHOES,                  // シューズ
    CATEGORY_BAG,                    // バッグ
    CATEGORY_HAT,                    // 帽子
    CATEGORY_ACCEYE,                 //  めがね
    CATEGORY_ACCHAIR,                //  かみかざり
    CATEGORY_MAX,
  };

  enum
  {
    COLOR_NONE_CODE  = -1,
  };

  //  各カテゴリーのアイテム最大数
  static const u32 FASHION_ARRAY_NUM = 512;

  //  SavedateのカテゴリーからDressUpParamのカテゴリーに変換
  static poke_3d::model::DressUpParam::ItemCategory ConvDressUpParamItemCategoryFromtSavedataCategory( const Category category );
  //  DressUpParamのカテゴリーからSavedateのカテゴリーに変換
  static Category ConvSavedataCategoryFromDressUpParamItemCategory( const poke_3d::model::DressUpParam::ItemCategory category );

  //  きせかえパラメータが変更しているか
  static bool IsChangeDressUpParam( const poke_3d::model::DressUpParam& rChkParam, const poke_3d::model::DressUpParam& rOrgParam );

private:
  //  きせかえアイテムデータ(1byte)
  struct ItemData
  {    
    u8 bHave     :1;
    u8 bNew      :1;
    u8 dummy     :6;
  };

  //! セーブデータに該当する構造体
  //  16kbyteを確保
  struct FashionData
  {

    // 
    // 着せ替え最大数更新日：2017/03/08　ichiraku
    // 

    //  所持している着せ替えアイテムリスト
    //  帽子 momijiは最大で女が99
    ItemData  aHatData[ 120 ];
    //  髪アクセ momijiは最大で女が37
    ItemData  aAcchairData[ 68 ];
    //  顔アクセ momijiは最大が男女とも21
    ItemData  aAcceyeData[ 30 ];
    //  トップス momijiは最大が女で436
    ItemData  aTopsData[ 450 ];
    //  ボトムス momijiは最大が女で252
    ItemData  aBottomsData[ 280 ];
    //  足 momijiは最大が女で126
    ItemData  aLegsData[ 158 ];
    //  靴 momijiは最大が女で169
    ItemData  aShoesData[ 197 ];
    //  バッグ momijiは最大が女で146
    ItemData  aBagData[ 177 ];

    //  使用サイズは1480byte

    //  セーブ変更する機会があればこの確保にする
    //  5000以下のpaddingにする
    //  u8  padding[ 6664 - 1480 ]; 

    //  用意した16kbyteから実際に使うサイズを引く
    u8  padding[ 5184 ];  //  16kbyte - 11200byte
  };

public:
  // con,des
  Fashion();
  ~Fashion();
  
public:
  //-----------------------------------------------------------------------------
  /**
   *  @brief    読み込んだデータをセットする関数
    * @param    pData   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void SetData(void* pData)
  {
    gfl2::std::MemCopy(pData, reinterpret_cast<void*>(&m_data), sizeof( FashionData ));
  }
  //-----------------------------------------------------------------------------
  /**
   *  @brief    データの先頭ポインタを得る関数
    * @return   先頭のポインタ
   */
  //-----------------------------------------------------------------------------
  virtual void* GetData(void)
  {
    return reinterpret_cast<void*>(&m_data);
  }

  //-----------------------------------------------------------------------------
  /**
   *  @brief    データサイズ
   *  @retval   バイト
   */
  //-----------------------------------------------------------------------------
  virtual size_t GetDataSize(void){ return sizeof( FashionData ); }
  
  //-----------------------------------------------------------------------------
  /**
   * @brief セーブデータのクリア
   * @param heap　クリアに使用するテンポラリヒープ
   */
  //-----------------------------------------------------------------------------
  virtual void Clear( gfl2::heap::HeapBase * heap );

public:
  //! 初期の服に所持フラグを立てる ゲーム開始後、性別確定時に呼ぶ
  void InitializeDefaultFashionItem(bool is_female);

  //  カテゴリーの配列アドレスを取得
  ItemData* GetCagoryItmeData( const Category type );
  //  カテゴリーの配列の個数取得
  u32 GetCagoryItmeDataNum( const Category type );

  //! 既に衣類を所持しているかどうか
  bool IsOwned(u32 type, int id);
  //  新規か
  bool IsNew(u32 type, int id);

  //! 衣類を追加
  bool Add(u32 type, int id);
  //! 衣類の追加を削除
  void Del(u32 type, int id );
  //! 衣類の新規追加設定をクリア
  void ClearNewFlag(u32 type, int id);

  
  //! 所持着せ替えアイテムの総数を取得
  u32 GetHaveItemTotalNum();

private:
  
  struct CategoryItemData
  {
    Category  category;
    ItemData* pItemData;
    u32       dataNum;
  };

  //  カテゴリデータ取得
  CategoryItemData* _GetCategoryItmeData( u32 type );

  ItemData* _GetItmeDataSearchId( u32 type, int id );

  FashionData m_data;


  // ライブラリのアイテムカテゴリーとセーブのカテゴリの対応表
  static const s32 CATEGORY_CONV_TABLE[ CATEGORY_MAX + 1 ][ 2 ];
};

GFL_NAMESPACE_END(savedata)

#endif //__SV_FASHION_H__
