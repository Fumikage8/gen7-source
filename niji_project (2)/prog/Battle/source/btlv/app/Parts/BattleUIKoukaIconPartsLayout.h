//=============================================================================
/**
 * @file BattleUIKoukaIconPartsLayout.h
 * @brief パーツレイアウト効果アイコンの制御
 * @author yuto_uchida
 * @date 2015.04.10
 */
//=============================================================================
#if !defined( __BTL_UI_KOUKA_ICON_PARTS_LAYOUT_H__ )
#define __BTL_UI_KOUKA_ICON_PARTS_LAYOUT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <layout/include/gfl2_LytSys.h>

#include "Battle/source/btl_common.h"

GFL_NAMESPACE_BEGIN( app )

//  前方宣言
GFL_NAMESPACE_BEGIN( util )
  class G2DUtil;
GFL_NAMESPACE_END( util )

GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( btl )
GFL_NAMESPACE_BEGIN( app )
GFL_NAMESPACE_BEGIN( parts )

//@memo  各ボタン項目のグラフィックフォントはレイアウトデータで対応

  /**
    * @brief 効果アイコンパーツレイアウトの制御クラス
    * @note
    *    レイアウトファイル「Common_ICN_009.flyt」のパーツの制御をしている
   */
  class BattleUIKoukaIconPartsLayout
  {
    GFL_FORBID_COPY_AND_ASSIGN( BattleUIKoukaIconPartsLayout );  // コピーを禁止

    public:
      //  クラス初期化時に必要なパラメータ
      typedef struct
      {
        //  パーツの細かい設定を行うのに必要
        ::app::util::G2DUtil*    p_g2d_util;

        //  パーツが配置されているLayoutId
        s32                     layout_id;

        //  パーツペイン
        gfl2::lyt::LytParts*    p_parts_pane;

        //  有効化アニメidx(アニメ名「active」)
        s32 active_anime_idx;

        //  無効化化アニメidx(アニメ名「passive」)
        s32 passive_anime_idx;

        //  効果なしアニメidx(アニメ名「kouka_02」)
        s32 kouka_none_anime_idx;

        //  効果いまひとつアニメidx(アニメ名「kouka_01」)
        s32 kouka_imahitotsu_anime_idx;

        //  効果ばつぐんアニメidx(アニメ名「kouka_00」)
        s32 kouka_batsugun_anime_idx;

        //  効果無記名アニメ名(アニメ名「kouka_03」)
        s32 kouka_normal_anime_idx;

      } INIT_PARAM;

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  コンストラクタ
        */
      //-----------------------------------------------------------------------------------------
      BattleUIKoukaIconPartsLayout( const INIT_PARAM& param );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  デストラクタ
        */
      //-----------------------------------------------------------------------------------------
      virtual ~BattleUIKoukaIconPartsLayout();

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  表示設定
        */
      //-----------------------------------------------------------------------------------------
      void SetVisible( const bool bFlg );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  効果設定
        */
      //-----------------------------------------------------------------------------------------
      void SetType( const BtlTypeAffAbout type );

    private:
      //  変数宣言一覧
      INIT_PARAM  m_initParam;
  };

GFL_NAMESPACE_END( parts )
GFL_NAMESPACE_END( app )
GFL_NAMESPACE_END( btl )

#endif // __BTL_UI_KOUKA_ICON_PARTS_LAYOUT_H__ 
