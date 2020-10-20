/**
* @brief  フィールドデバッグ表示制御フラグの定義
* @file   FieldDebugTypes.h
* @author ikuta_junya
* @data   2015.03.08
*/

// ・概要
// このファイルを直接includeしないでください、FieldDebugTypes.hをincludeして使用します。
// このファイルに定義を追加すると、[R+Xメニュー]-[Field]-[DrawSwitch]メニューに項目が追加され、整数値の操作が出来るようになります

// ・定義のフォーマット
// DEFINE( 定義名, 初期値(整数), 最大値(整数) )
// 定義名はField::debug::DrawSwitch型(enum型)の内部に定義されます

// ・参照方法
// #include <**/FieldDebugTypes.h>
// Field::debug::FieldDebugTypes::s_DrawSwitchTable[ Field::debug::FieldDebugTypes::定義名 ]


DEFINE( DRAW_SWITCH_EDGE_MODE, 1, 3 )      //  0:無し 1:ID 2:法線 3:ID+法線
DEFINE( FRAME_LATE_MODE, 0, 2 )            //  0:30FPS 1:20FPS 2:15FPS
DEFINE( MODEL_TRAVERSE_MODE, 1, 2 )        //  0:行列全更新 1:ダーティチェックを行って更新
DEFINE( ENABLE_PLAYER_VISIBLE   , 1, 1 )
DEFINE( ENABLE_PLAYER_ANIMAION , 1, 1 )
DEFINE( ENABLE_PLAYER_COLLISION , 0, 1 )
DEFINE( ENABLE_PLAYER_IK, 0, 1 )
DEFINE( ENABLE_POKE_RIDE , 0, 1 )          // ポケライドにのっているか
DEFINE( WORK_TICK_PRINT_AVE_COUNT , 20, 20 )    // TickPrintAve平均値の計測時間
DEFINE( DRAW_SWITCH_SKYBOX, 1, 1 )                // 天球
DEFINE( DRAW_SWITCH_TERRAIN_HIGH, 1, 1 )          // ハイモデル領域の地形
DEFINE( DRAW_SWITCH_TERRAIN_MIDDLE, 1, 1 )        // ミドルモデル領域の地形
DEFINE( DRAW_SWITCH_TERRAIN_DISTANT_VIEW, 1, 1 )  // 遠景領域の地形
DEFINE( DRAW_SWITCH_TEST_KISEKAE, 1, 1 )   // 着せ替え検証用
DEFINE( DRAW_SWITCH_TEST_RIM, 1, 1 )       // RIM検証用
DEFINE( DRAW_SWITCH_TEST_KENTAUROS, 1, 7 ) // ポケライド検証用
DEFINE( DRAW_SWITCH_TEST_NPC_1500, 1, 7 )  // 1500ポリゴンNPCモデル（Llモデル）
DEFINE( DRAW_SWITCH_TEST_NPC_2000, 1, 7 )  // 2000ポリゴンNPCモデル（Lhモデル）
DEFINE( DRAW_SWITCH_TEST_NPC_1500_02, 1, 7 )  // 1500ポリゴンNPCモデル（Llモデル）
DEFINE( DRAW_SWITCH_TEST_NPC_2000_02, 1, 7 )  // 2000ポリゴンNPCモデル（Lhモデル）
DEFINE( DRAW_SWITCH_TEST_NPC_1500_03, 1, 7 )  // 1500ポリゴンNPCモデル（Llモデル）
DEFINE( DRAW_SWITCH_TEST_NPC_2000_03, 1, 7 )  // 2000ポリゴンNPCモデル（Lhモデル）
DEFINE( DRAW_SWITCH_TEST_NPC_1500_04, 1, 7 )  // 1500ポリゴンNPCモデル（Llモデル）
DEFINE( DRAW_SWITCH_TEST_NPC_2000_04, 1, 7 )  // 2000ポリゴンNPCモデル（Lhモデル）
DEFINE( DRAW_SWITCH_ENCOUNT_HIGH, 0, 1 )   // 高負荷エンカウントモデル表示
DEFINE( DRAW_SWITCH_ENCOUNT_MIDDLE, 0, 1 ) // 普通負荷エンカウントモデル表示
DEFINE( DRAW_SWITCH_ENCOUNT_GROUND, 0, 1 ) // エンカウントモデル確認用の地形を表示するか
DEFINE( DRAW_SWITCH_SHADOW_PRI, 0, 1 )     // テクスチャなしの影モデル表示
DEFINE( DRAW_SWITCH_SHADOW_TEX, 0, 1 )     // テクスチャでの影モデル表示
DEFINE( DRAW_SWITCH_SHADOW_TEXANM, 0, 1 )     // テクスチャでの影モデル表示
DEFINE( DRAW_SWITCH_WEATHER, 0, 2 )        // 天候 0:表示OFF、1:パーティクルを表示、2:モデルを表示
DEFINE( DRAW_SWITCH_FOOT_SPRAY, 0, 2 )     // 足元エフェクト 0:表示OFF、1:パーティクルを表示、2:モデルを表示
DEFINE( DRAW_SWITCH_MAP_00_00 , 1, 1 )     // テストマップ左上
DEFINE( DRAW_SWITCH_MAP_00_01 , 1, 1 )     // テストマップ右上
DEFINE( DRAW_SWITCH_MAP_01_00 , 1, 1 )     // テストマップ左下
DEFINE( DRAW_SWITCH_MAP_01_01 , 1, 1 )     // テストマップ右下
DEFINE( DRAW_SWITCH_EDGE_TEST_00, 0, 1 )   // IDカラーエッジ（表情マテリアルが2つ）
DEFINE( DRAW_SWITCH_EDGE_TEST_01, 0, 1 )   // 頂点カラーエッジ（表情マテリアルが2つ）
DEFINE( DRAW_SWITCH_MAT_TEST_00, 0, 1 )    // 表情マテリアルが2つ（edge_test_00と同じ物）
DEFINE( DRAW_SWITCH_MAT_TEST_01, 0, 1 )    // 1マテリアルでUVセットが2つ、テクスチャにアルファチャンネルが含まれる
DEFINE( DRAW_SWITCH_MAT_TEST_00_PLUS, 0, 1 )    // DRAW_SWITCH_MAT_TEST_00の周囲に同じものを複数体だす
DEFINE( DRAW_SWITCH_MAT_TEST_01_PLUS, 0, 1 )    // DRAW_SWITCH_MAT_TEST_01の周囲に同じものを複数体だす
DEFINE( DRAW_SWITCH_DOWN_DISPLAY, 0, 1 )      //  0:上画面に表示 1:下画面に表示
DEFINE( DRAW_SWITCH_DEBUG_FRUSTUM_DRAW, 0, 1 )//  視錐台の表示
DEFINE( DRAW_SWITCH_DEBUG_AABB_DRAW, 0, 1 )//  AABBの表示
