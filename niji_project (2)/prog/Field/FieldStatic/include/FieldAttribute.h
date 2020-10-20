//===================================================================
/**
 * @file    FieldAttribute.h
 * @brief   フィールドアトリビュート
 * @author  saita_kazuki
 * @date    2015.05.28
 */
//===================================================================

#if !defined( FIELD_ATTRIBUTE_H_INCLUDED )
#define FIELD_ATTRIBUTE_H_INCLUDED
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

// conv_header
#include <niji_conv_header/field/attribute/attribute_id.h>

GFL_NAMESPACE_BEGIN( Field )

typedef u32 Attr;                           ///< アトリビュート型
const Attr ATTR_ERROR = ATTRIBUTE_ERROR;    ///< アトリビュートエラー値

/**
 * @class フィールドアトリビュートクラス
 */
class Attribute
{
  /**
   * @briefコピーコンストラクタ＋代入禁止
   */
  GFL_FORBID_COPY_AND_ASSIGN( Attribute );

public:

  /**
   * @brief コンストラクタ
   */
  Attribute(){}

  /**
   * @brief デストラクタ
   */
  virtual ~Attribute(){}

  /**
   * @brief 初期化
   * @param pParameterTableData バイナリデータ
   * @note FieldResidentから取得してセットしてください
   */
  static void Initialize( void* pParameterTableData);

  /**
   * @brief アトリビュート数の取得
   */
  static u32 GetAttributeNum();

  /**
   * @brief 水アトリビュートかチェック
   */
  static bool IsWater( Attr attr );

  /**
   *  @brief 草むらアトリビュートかチェック
   */
  static bool IsGrass( Attr attr );

  /**
   *  @brief 海アトリビュートかチェック
   */
  static bool IsSea( Attr attr );

  /**
   *  @brief ロバ２しか入れない地面アトリビュートチェック
   */
  static bool IsRoba2Ground( Attr attr );

  /**
   * @brief 戦闘背景近景ID取得
   */
  static u8 GetBattleNearBGID( Attr attr, u32 pattern);

  /**
   * @brief 戦闘エフェクトID取得
   */
  static u8 GetBattleEffectID( Attr attr);

  /**
   * @brief 有効なアトリビュートか
   * @retval true 有効なアトリビュート(attribute_id.hに定義がある)
   * @retval false それ以外
   */
  static bool IsEnableAttribute( Attr attr);

private:

  // ヘッダー
  struct Header
  {
    u32   num;                  ///< アトリビュート数
    u32   battleBGNearTypeNum;  ///< 近景パターンの数
  };

  // データ
  struct Core
  {
    u16   padding1 : 16;
    u8    battleEffectID;
    u8    padding2[1];

    u32   battleBGNearTypeAddrOffset; ///< データ先頭から近景IDへのアドレスオフセット
  };

//C4200を抑止したい場合
GFL_WARNING_WIN32_PUSH
GFL_WARNING_WIN32_IGNORE(4200)  //C4200を抑止

  // パラメーターテーブル
  struct AttributeParameterTable
  {
    Header    header;
    Core      core[];
  };
//C4200を抑止したい場合
GFL_WARNING_WIN32_POP


private:

  static const Core* GetCoreData( Attr attr);

private:

  static AttributeParameterTable*   m_pParameterTable;

};

GFL_NAMESPACE_END( Field )

#endif // FIELD_ATTRIBUTE_H_INCLUDED
