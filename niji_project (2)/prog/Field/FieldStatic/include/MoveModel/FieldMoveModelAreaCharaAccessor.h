//======================================================================
/**
 * @file FieldMoveModelAreaCharaAccessor.h
 * @date 2015/09/04 22:20:02
 * @author takahashi_tomoya
 * @brief 動作モデル　エリアリソースパックアクセサー
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FIELD_MOVEMODEL_AREA_CHARA_ACCESSOR_H__
#define __FIELD_MOVEMODEL_AREA_CHARA_ACCESSOR_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <fs/include/gfl2_BinLinkerAccessor.h>

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)


  /**
   * @class FieldMoveModelAreaCharaAccessor
   * @brief フィールド動作モデル　エリアキャラアクセサー
   */
  class FieldMoveModelAreaCharaAccessor
  {
      GFL_FORBID_COPY_AND_ASSIGN(FieldMoveModelAreaCharaAccessor);

  public:
    /**
     *  @brief コンストラクタ
     */
    FieldMoveModelAreaCharaAccessor();

    /**
     *  @brief デストラクタ
     */
    virtual ~FieldMoveModelAreaCharaAccessor();

    /**
     *  @brief 初期化
     */
    void Initialize( gfl2::fs::BinLinkerAccessor* pAccessor );

    /**
     *  @brief 破棄
     */
    void Terminate();

    /**
     *  @brief データ数の取得
     */
    s32 GetDataNum() const;

    /**
     *  @brief キャラクターIDの取得
     */
    u32 GetCharacterID( u32 index ) const;

    /**
     *  @brief　キャラクターリソースの取得
     */
    void * GetCharacterResource( u32 index ) const;


  private:
    //! 変数へのコメント
    gfl2::fs::BinLinkerAccessor* m_pAccessor;
  };

GFL_NAMESPACE_END(MoveModel)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_MOVEMODEL_AREA_CHARA_ACCESSOR_H__
