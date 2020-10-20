#pragma once
//=================================================================================================
/**
*  @file  movemodel_define.h
*  @brief
*  @author  tamada@gamefreak.co.jp
*  @date  2014.03.13
*/
//=================================================================================================
#include "../include/chara_define.h"

namespace fld{ namespace mmodel {

  class MoveModel;

  //----------------------------------------------------------------------------------------
  //! @brief MoveModel用のID
  //----------------------------------------------------------------------------------------
  enum MMDL_ID
  {
    MMDL_NONE  = 0,
    MMDL_PLAYER,
    MMDL_NPC_0,
    MMDL_NPC_1,
    MMDL_NPC_2,
    MMDL_CITY_KABE,
    MMDL_CITY_YUKA,
    MMDL_MAP_00_00,
    MMDL_MAP_00_01,
    MMDL_MAP_01_00,
    MMDL_MAP_01_01,
    MMDL_ATTACH_0,

    MMDL_EDGE_TEST_00,   // IDカラーエッジ（表情マテリアルが2つ）
    MMDL_EDGE_TEST_01,   // 頂点カラーエッジ（表情マテリアルが2つ）
    MMDL_MAT_TEST_00,   // 表情マテリアルが2つ（edge_test_00と同じ物）
    MMDL_MAT_TEST_01,   // 1マテリアルでUVセットが2つ、テクスチャにアルファチャンネルが含まれる

    MMDL_MAT_TEST_00_001,
    MMDL_MAT_TEST_00_002,
    MMDL_MAT_TEST_00_003,
    MMDL_MAT_TEST_00_004,

    MMDL_MAT_TEST_01_001,
    MMDL_MAT_TEST_01_002,
    MMDL_MAT_TEST_01_003,
    MMDL_MAT_TEST_01_004,

    MMDL_TEST_KISEKAE,   // 着せ替え検証用
    MMDL_TEST_KENTAUROS, // ポケライド検証用
    MMDL_TEST_NPC_1500,  // 1500ポリゴンNPCモデル（Llモデル）
    MMDL_TEST_NPC_2000,  // 2000ポリゴンNPCモデル（Lhモデル）
    MMDL_TEST_RIM,       // リム確認用モデル
    MMDL_TEST_NPC_1500_01,  // 1500ポリゴンNPCモデル（Llモデル）
    MMDL_TEST_NPC_2000_01,  // 2000ポリゴンNPCモデル（Lhモデル）
    MMDL_TEST_NPC_1500_02,  // 1500ポリゴンNPCモデル（Llモデル）
    MMDL_TEST_NPC_2000_02,  // 2000ポリゴンNPCモデル（Lhモデル）
    MMDL_TEST_NPC_1500_03,  // 1500ポリゴンNPCモデル（Llモデル）
    MMDL_TEST_NPC_2000_03,  // 2000ポリゴンNPCモデル（Lhモデル）
    MMDL_TEST_NPC_1500_04,  // 1500ポリゴンNPCモデル（Llモデル）
    MMDL_TEST_NPC_2000_04,  // 2000ポリゴンNPCモデル（Lhモデル）

    MMDL_MAX
  };

  //----------------------------------------------------------------------------------------
  //! @brief MoveCode用のID
  //----------------------------------------------------------------------------------------
  enum MOVE_CODE
  {
    MOVE_CODE_NONE,
    MOVE_CODE_PLAYER,
    MOVE_CODE_NPC,
    MOVE_CODE_ATTACH_PLAYER,
    MOVE_CODE_NUM
  };

  //----------------------------------------------------------------------------------------
  //! @brief MoveCodeの各インスタンスが保持するワーク
  //----------------------------------------------------------------------------------------
  enum
  {
    MOVECODE_WORK_SIZE = 64
  };

  //----------------------------------------------------------------------------------------
  //! @brief 動作コード処理　共通ワーク
  //----------------------------------------------------------------------------------------
  struct MoveCodeWork
  {
    MoveModel*  p_model;                    // 動作モデルワーク
    u32         mv_code;                    // 動作の種類
    u32         work[MOVECODE_WORK_SIZE/4]; // フリーに使える共有ワーク
  };

  //----------------------------------------------------------------------------------------
  //! @brief セーブされない状態　動作ビット
  //----------------------------------------------------------------------------------------
  enum MoveBit
  {
    MOVEBIT_NON       = (0),      ///<無し
    MOVEBIT_ACMD      = (1<<9),   ///<アクションコマンド中
    MOVEBIT_ACMD_END  = (1<<10),  ///<アクションコマンド終了
  } ;

  //----------------------------------------------------------------------------------------
  //! @brief MoveModelのリクエストデータ
  //----------------------------------------------------------------------------------------
  struct MoveModelHeader
  {
    MMDL_ID         mmdl_id;
    MOVE_CODE       move_code;
    chara::RSC_ID   rsc_id;
  };

}; //end of namespace mmodel
}; //end of namespace fld
