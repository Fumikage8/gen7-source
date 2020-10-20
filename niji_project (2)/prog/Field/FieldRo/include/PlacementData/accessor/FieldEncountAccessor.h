/**
 *  GAME FREAK inc.
 *
 *  @file   FieldEncountAccessor.h
 *  @brief  エンカウント配置データアクセサ
 *  @author saita_kazuki
 *  @date   2015.05.31
 */

#if !defined(__FIELD_ENCOUNT_DATA_ACCESSOR_H__)
#define __FIELD_ENCOUNT_DATA_ACCESSOR_H__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_Assert.h>

GFL_NAMESPACE_BEGIN( Field );

class FieldEncountAccessor
{
  // コピー禁止クラスにする
  GFL_FORBID_COPY_AND_ASSIGN( FieldEncountAccessor );

public:

  static const u32 TARGET_ATTRIBUTE_BITS_SIZE           = 128;                                              ///< 対象アトリビュートビットフィールドのビットサイズ
  static const u32 TARGET_ATTRIBUTE_BITS_DIVISION_SIZE  = TARGET_ATTRIBUTE_BITS_SIZE / sizeof(u32);         ///< 対象アトリビュートビットフィールドの分割ビットサイズ
  static const u32 TARGET_ATTRIBUTE_ARRAY_SIZE          = (TARGET_ATTRIBUTE_BITS_SIZE / 8) / sizeof(u32);   ///< 対象アトリビュートビットフィールドのu32型の配列サイズ

  // 配置情報
  struct EncountData
  {
    u32 type;                                         // 識別子
    u32 dataID;                                       // エンカウントテーブルデータID
    u32 targetAttrBits[TARGET_ATTRIBUTE_ARRAY_SIZE];  // 対象アトリビュートビットフィールド u64 * 2 (128bit)
    u32 collisionOffset;                              // コリジョンオフセット
  }; // struct EncountData

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // 配置情報（Zone単位）
  struct EncountDataHeader
  {
    u32           num;
    EncountData   data[];
  }; // struct EncountDataHeader
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP

public:
  /**
   *  @brief  コンストラクタ
   */
  FieldEncountAccessor( void );

  /**
   *  @brief  デストラクタ
   */
  ~FieldEncountAccessor( void );

  /**
   *  @brief  初期化
   */
  void Initialize( const void *data );

  /**
   *  @brief  最大数取得
   */
  u32 GetDataMax( void ) const;

  /**
   *  @brief  情報取得
   */
  const EncountData *GetData( u32 index ) const;

  /**
   *  @brief  コリジョン情報の取得
   */
  const void *GetCollisionData( const EncountData &data ) const;

private:

  const EncountDataHeader *m_pData;

}; // class FieldEncountAccessor

GFL_NAMESPACE_END( Field );

#endif // !defined(__FIELD_ENCOUNT_DATA_ACCESSOR_H__)
