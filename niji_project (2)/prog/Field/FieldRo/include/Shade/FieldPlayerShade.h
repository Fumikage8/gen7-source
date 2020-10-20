//======================================================================
/**
 * @file FieldPlayerShade.h
 * @date 2016/11/29 12:57:04
 * @author kawazoe_shinichi
 * @brief フィールドプレイヤー用陰影関連
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================


#if !defined __FIELD_PLAYER_SHADE_H_INCLUDED__
#define __FIELD_PLAYER_SHADE_H_INCLUDED__
#pragma once

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(MoveModel)
class FieldMoveModelPlayer;
GFL_NAMESPACE_END(MoveModel)

GFL_NAMESPACE_BEGIN(Shade);
class FieldShadeAreaManager;

//-------------------------------------------------------------------------
/// @class FieldPlayerShade
/// @brief フィールドプレイヤー用陰影クラス
//-------------------------------------------------------------------------
class FieldPlayerShade
  {
    GFL_FORBID_COPY_AND_ASSIGN(FieldPlayerShade);

  public:

    //-------------------------------------------------------------------------
    /// @brief フィールドプレイヤー用陰影クラス初期化設定
    //-------------------------------------------------------------------------
    struct InitDesc
    {
      gfl2::heap::HeapBase* pHeap;            //!< ヒープ
      void*                 pShadeAreaData;   //!< 影領域データ

    };

    //-------------------------------------------------------------------------
    /// @brief  コンストラクタ
    //-------------------------------------------------------------------------
    FieldPlayerShade();
    //-------------------------------------------------------------------------
    /// @brief  デストラクタ
    //-------------------------------------------------------------------------
    ~FieldPlayerShade();


    //-------------------------------------------------------------------------
    /// @brief  初期化処理
    /// @param desc   初期化設定データ
    //-------------------------------------------------------------------------
    void Initialize(const InitDesc& desc);

    //-------------------------------------------------------------------------
    /// @brief  終了処理
    //-------------------------------------------------------------------------
    void Terminate(void);



    //-------------------------------------------------------------------------
    /// @brief  更新処理
    /// @param pPlayer  プレイヤー動作モデル
    //-------------------------------------------------------------------------
    void Update(Field::MoveModel::FieldMoveModelPlayer* pPlayer);

    //-------------------------------------------------------------------------
    /// @brief  色変更
    /// @param pPlayer  プレイヤー動作モデル
    //-------------------------------------------------------------------------
    void ChangeColor(Field::MoveModel::FieldMoveModelPlayer* pPlayer);

    //-------------------------------------------------------------------------
    /// @brief  指定した動作モデルに色を反映
    /// @param pMoveModel   動作モデル
    /// @param vColor       反映させる色
    //-------------------------------------------------------------------------
    void ApplyColorForMoveModel(Field::MoveModel::FieldMoveModel* pMoveModel, gfl2::math::Vector3 vColor);

    //-------------------------------------------------------------------------
    /// @brief  指定した動作モデルに影領域を反映（領域内に進入している場合はその領域の影色を反映）
    /// @param pMoveModel   動作モデル
    //-------------------------------------------------------------------------
    void ApplyShadeAreaForMoveModel(Field::MoveModel::FieldMoveModel* pMoveModel);


  private:
    //-------------------------------------------------------------------------
    /// @brief  現在のコンスタントカラーを取得する
    /// @param pPlayer  プレイヤー動作モデル
    /// @return コンスタントカラー
    //-------------------------------------------------------------------------
    gfl2::math::Vector3 GetCurrentConstantColor(Field::MoveModel::FieldMoveModelPlayer* pPlayer) const;

  private:
    gfl2::heap::HeapBase* m_pHeap;  //!< ヒープ

    FieldShadeAreaManager* m_pShadeAreaManager; //!< 陰影領域管理

    bool m_isFirstFrameForFieldmap; //!< Fieldmap開始直後のフレームかどうか
    bool m_isColorChanging;         //!< 色が変化中かどうか

    s32 m_TargetAreaIndex;          //!< 現在重なっている領域のID（重なっていない場合は『-1』）
    gfl2::math::Vector3 m_vStartColor;  //!< 変化前色
    gfl2::math::Vector3 m_vEndColor;    //!< 変化後色

    u32 m_CurrentFrame;             //!< 現在の経過フレーム
    u32 m_DoFrame;                  //!< 補間にかける処理フレーム

    bool m_isRidingForBeforeFrame;  //!< 前フレームにライド状態だったかどうか
    
  };

GFL_NAMESPACE_END(Shade)
GFL_NAMESPACE_END(Field)

#endif // __FIELD_PLAYER_SHADE_H_INCLUDED__