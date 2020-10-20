//======================================================================
/**
 * @file FieldDropItemDataAccessor.h
 * @date 2015/08/31 18:09:04
 * @author saita_kazuki
 * @brief ドロップアイテムデータアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_DROP_ITEM_DATA_ACCESSOR_H_INCLUDED__
#define __FIELD_DROP_ITEM_DATA_ACCESSOR_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

GFL_NAMESPACE_BEGIN( Field )
GFL_NAMESPACE_BEGIN( DropItem )

/**
 * @class ドロップアイテムデータアクセサー
 */
class DataAccessor
{
  GFL_FORBID_COPY_AND_ASSIGN( DataAccessor );

public:

  static const u32 TABLE_ITEM_NUM = 3;                  ///< 1テーブル内のアイテム数
  static const u32 DROP_ITEM_INVALID_ID = 0xFFFFFFFF;   ///< 無効値

  /**
   * @brief アイテムデータ
   */
  struct ItemData
  {
    u16   itemID;
    u8    prob;
    u8    padding[1];
  };

  /**
   * @brief テーブルデータ
   */
  struct TableData
  {
    ItemData  data[ TABLE_ITEM_NUM ];
  };

  /**
   * @brief コンストラクタ
   *  @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
   */
  DataAccessor();
  DataAccessor( void* pData);

  /**
   * @brief デストラクタ
   */
  virtual ~DataAccessor();

  /**
   *  @brief データセット
   *  @param pData ドロップアイテムバイナリデータ。FieldResidentから取得
   */
  void SetData( void* pData);

  /**
   * @brief データ取得
   * @param id テーブルID
   * @return 指定したIDのテーブルデータ
   * @note 不正なID指定をした場合はNULL
   */
  TableData* GetData( u32 id) const ;

private:

  /**
   * @brief ヘッダーデータ
   */
  struct HeaderData
  {
    u32   num;
  };

GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  /**
   * @brief バイナリ全体
   */
  struct AllData
  {
    HeaderData header;
    TableData  data[];
  };

GFL_WARNING_WIN32_POP

private:

  AllData*  m_pData;

};

GFL_NAMESPACE_END( DropItem )
GFL_NAMESPACE_END( Field )

#endif // __FIELD_DROP_ITEM_DATA_ACCESSOR_H_INCLUDED__
