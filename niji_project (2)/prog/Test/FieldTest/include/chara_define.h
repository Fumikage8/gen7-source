#pragma once
//=================================================================================================
/**
 *  @file  chara_define.h
 *  @brief  niji用キャラ指定定義
 *  @author  tamada@gamefreak.co.jp
 *  @date  2014.05.21
 *
 *  nijiにおいては、バトル用・フィールド用で同一のキャラモデルを使用するため、統一した定義を用意する。
 */
//=================================================================================================

namespace chara {

  //-------------------------------------------------------------------------------------------------
  /**
  *  @enum
  *  @brief キャラクター指定IDの定義
  */
  //-------------------------------------------------------------------------------------------------
  enum RSC_ID
  {
    RSCID_HERO,
    RSCID_PIKACYU,
    RSCID_MAP_01_KABE,
    RSCID_MAP_01_YUKA,
    RSCID_MAP00_00_00,   // 左上
    RSCID_MAP00_00_01,   // 右上 
    RSCID_MAP00_01_00,   // 左下
    RSCID_MAP00_01_01,   // 右下
    RSCID_MAP_TEST_HEIGHT,  // 4ブロック分の高さ
    RSCID_MAP00_TEST,   // 
    RSCID_ATTACH_OBJ_TSURIZAO,

    // 処理負荷計測用キャラモデル
    RSCID_EDGE_TEST_00,   // IDカラーエッジ（表情マテリアルが2つ）
    RSCID_EDGE_TEST_01,   // 頂点カラーエッジ（表情マテリアルが2つ）
    RSCID_MAT_TEST_00,   // 表情マテリアルが2つ（edge_test_00と同じ物）
    RSCID_MAT_TEST_01,   // 1マテリアルでUVセットが2つ、テクスチャにアルファチャンネルが含まれる

    RSCID_TEST_KISEKAE,   // 着せ替え検証用
    RSCID_TEST_KENTAUROS, // ポケライド検証用
    RSCID_TEST_NPC_1500,  // 1500ポリゴンNPCモデル（Llモデル）
    RSCID_TEST_NPC_2000,  // 2000ポリゴンNPCモデル（Lhモデル）
    RSCID_TEST_RIM,       // リムライト検証用

    RSCID_NUM
  };

  //-------------------------------------------------------------------------------------------------
  /**
  *  @brief キャラのモーション指定ID
  *  現状、全キャラ同じIDで指定することを想定している
  */
  //-------------------------------------------------------------------------------------------------
  enum 
  {
    MOTION_ID_WAIT = 0,
    MOTION_ID_WALK,
    MOTION_ID_RUN,
    MOTION_ID_WALK_FAST,
    MOTION_ID_TSURI_START,
    MOTION_ID_MAX
  };

}; //end of namespace chara
