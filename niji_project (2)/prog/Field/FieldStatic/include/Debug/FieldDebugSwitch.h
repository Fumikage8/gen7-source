/**
* @brief  フィールドデバッグ制御フラグの定義
* @file   FieldDebugSwitch.h
* @author ikuta_junya
* @data   2015.03.08
*/

// ・概要
// このファイルを直接includeしないでください、FieldDebugTypes.hをincludeして使用します。
// このファイルに定義を追加すると、[R+Xメニュー]-[Field]-[Switch]メニューに項目が追加され、整数値の操作が出来るようになります

// ・定義のフォーマット
// DEFINE( 定義名, 初期値(整数), 最大値(整数) )
// 定義名はField::debug::Switch型(enum型)の内部に定義されます

// ・デバッグメニューへの登録
// FieldDebugSwitchGroup.hの各小グループに登録する事で、デバッグメニュー([DebugWinTop]-[Field]-[デバッグフラグ操作)で操作が出来るようになります

// ・参照方法
// #include <**/FieldDebugTypes.h>
// FIELD_DEBUG_SWITCH( Debug::DebugTypes::Switch::定義名 )
// FIELD_DEBUG_SWITCH_SET( Debug::DebugTypes::Switch::定義名, 値 )

DEFINE( INFO_LEVEL, 0, 1 )       // プリント出力レベル
DEFINE( TERRAIN, 1, 1 )          // 地形表示制御 0:OFF 1:ON
DEFINE( TERRAIN_LOD, 1, 1 )      // 地形ブロックLODモデルの表示制御 0:OFF 1:ON
DEFINE( TERRAIN_HIGH, 1, 1 )     // 地形ブロックHighモデルの表示制御 0:OFF 1:ON
DEFINE( TERRAIN_DISTANT_VIEW, 1, 1 )  // 地形遠景モデルの表示制御 0:OFF 1:ON
DEFINE( TERRAIN_BG_PARTS, 1, 1 )      // BGパーツの表示制御 0:OFF 1:ON
DEFINE( ENABLE_TERRAIN_CULLING, 1, 1 )   // 地形のカリングの制御 0:OFF 1:ON
DEFINE( ENABLE_WALL_COLLISION, 1, 1 )    // 壁コリジョン処理の制御 0:OFF 1:ON
DEFINE( ENABLE_ID_EDGE, 1, 1 )           // IDエッジ有効無効制御
DEFINE( ENABLE_NORMAL_EDGE, 1, 1 )       // 法線エッジ有効無効制御
DEFINE( UPDATE_TERRAIN_ANIM_LOCAL, 1, 1 )     // 地形ローカルアニメーションフレーム更新、有効無効制御
DEFINE( UPDATE_TERRAIN_ANIM_ONE_DAY, 1, 1 )   // 地形一日アニメーションフレーム更新、有効無効制御
#if 0
DEFINE( TERRAIN_ONLY, 0, 1 )      
DEFINE( TERRAIN_ONLY_HIGH_X, 0, 20 )      
DEFINE( TERRAIN_ONLY_HIGH_Z, 0, 20 )      
DEFINE( TERRAIN_ONLY_LOD_X, 0, 20 )      
DEFINE( TERRAIN_ONLY_LOD_Z, 0, 20 )      
#endif
DEFINE( ENCOUNT_THROUGH, 0, 1 )           // エンカウントスルー(バトルには行かず必ず勝ちにする) 0:OFF 1:ON
DEFINE( ENABLE_WILD_ENCOUNT, 1, 1 )       // 野生エンカウント有効無効制御
DEFINE( ENABLE_TRAINER_ENCOUNT, 1, 1 )    // 視線トレーナーエンカウント有効無効制御
DEFINE( ENABLE_GIMMICK_ENCOUNT, 1, 1 )    // ギミックエンカウント有効無効制御
DEFINE( ENABLE_SCENESCRIPT, 1, 1 )   // シーンスクリプト起動 0:OFF 1:ON

DEFINE( VISIBLE_TERRAIN_COLLISION, 0, 1 )  // 地形コリジョン（床面）モデル表示 0:OFF 1:ON
DEFINE( VISIBLE_WALL_COLLISION, 0, 1 )  // 地形コリジョン（壁面）モデル表示 0:OFF 1:ON
DEFINE( VISIBLE_WALL_COLLISION_NAMINORI, 0, 1 )  // 地形コリジョン（波乗り）モデル表示 0:OFF 1:ON
DEFINE( VISIBLE_WALL_COLLISION_ROBA2, 0, 1 )     // 地形コリジョン（地面馬）モデル表示 0:OFF 1:ON
DEFINE( VISIBLE_WALL_COLLISION_RIDE, 0, 1 )      // 地形コリジョン（ライド）モデル表示 0:OFF 1:ON

DEFINE( VISIBLE_PLACEMENT_COLLISION, 0, 1 )  // 配置物コリジョン モデル表示 0:OFF 1:ON
DEFINE( VISIBLE_EVENT_POS_COLLISION, 0, 1 )  // POSイベントコリジョン表示 0:OFF 1:ON
DEFINE( VISIBLE_EVENT_INTRPOS_COLLISION, 0, 1 )  // POSイベントコリジョン表示 0:OFF 1:ON
DEFINE( VISIBLE_EVENT_TALK_COLLISION, 0, 1 )  // 会話コリジョン表示 0:OFF 1:ON
DEFINE( VISIBLE_EVENT_PUSH_COLLISION, 0, 1 )  // 地形コリジョン表示 0:OFF 1:ON
DEFINE( VISIBLE_ENCOUNT_COLLISION, 0, 1 )  // エンカウントコリジョンモデル表示 0:OFF 1:ON
DEFINE( VISIBLE_TRAINER_EYE_COLLISION, 0, 1 )  // トレーナー視界コリジョンモデル表示 0:OFF 1:ON
DEFINE( VISIBLE_FOCUS_EYE_COLLISON, 0, 1 )     // TRフォーカス用コリジョンモデル表示 0:OFF 1:ON
DEFINE( VISIBLE_FOCUS_EYE_KEEP_COLLISON, 0, 1 ) // TRフォーカス用コリジョン(継続KEEP用)モデル表示 0:OFF 1:ON
DEFINE( VISIBLE_CAMERA_AREA_COLLISION, 0, 1 )  // カメラエリアコリジョンモデル表示 0:OFF 1:ON
DEFINE( VISIBLE_CAMERA_EYE_COLLISION, 0, 1 )  // カメラエリアコリジョン消去対象コリジョン表示 0:OFF 1:ON
DEFINE( VISIBLE_ENV_SOUND_COLLISION, 0, 1 )  // 環境音コリジョンモデル表示 0:OFF 1:ON
DEFINE( VISIBLE_GIMMICK_ENCOUNT_ACTION_COLLISION, 0, 1 )  // ギミックエンカウントアクション用コリジョンモデル表示 0:OFF 1:ON
DEFINE( VISIBLE_GIMMICK_ENCOUNT_ENCOUNT_COLLISION, 0, 1 )  // ギミックエンカウントエンカウント用コリジョンモデル表示 0:OFF 1:ON
DEFINE( VISIBLE_EVENT_POKEFINDER, 0, 1 ) // ポケファインダーコリジョン表示0:OFF 1:ON

DEFINE( GIMMICK_ENCOUNT_LOTTERY, 1, 2 )       // ギミックエンカウント抽選 0:出現しない 1:通常 2:必ず出現

DEFINE( ENABLE_CAMERA_FOCUS_TEST, 0, 1 )         // カメラフォーカスのテスト設定反映を有効化
DEFINE( DRAW_CAMERA_AREA_DATA, 0, 1 )            // カメラエリア設定表示制御
DEFINE( ENABLE_DUMP_BLOCK_INFO, 1, 1 )           // 地形ブロック情報Dump制御
#if DEBUG_ONLY_FOR_ikuta_junya
DEFINE( ENABLE_DUMP_BLOCK_INFO_FOR_SCRIPT, 1, 1 )   // 地形ブロック情報Dump制御
#else
DEFINE( ENABLE_DUMP_BLOCK_INFO_FOR_SCRIPT, 0, 1 )   // 地形ブロック情報Dump制御
#endif
DEFINE( ENABLE_POKEFINDER_LIGHT, 0, 1 )
DEFINE( ENABLE_POKEFINDER_VAL, 0, 360 )
DEFINE( ENABLE_POKEFINDER_HOL, 0, 360 )
DEFINE( ENABLE_POKEFINDER_TRIAL_CHECK, 0, 1 )
DEFINE( ENABLE_POKEFINDER_TRIAL_SCREEN, 0, 200 )
DEFINE( ENABLE_POKEFINDER_TRIAL_OUT, 0, 100 )
DEFINE( ENABLE_POKEFINDER_TRIAL_LENGTH, 0, 1000 )

#if defined(GF_PLATFORM_WIN32)
DEFINE( ENABLE_EV_CAMERA_RECORD, 0, 1 )
#endif  //#if defined(GF_PLATFORM_WIN32)
DEFINE( VISIBLE_SKYBOX, 0, 1 )
DEFINE( ENCGRASS_ON_LENGTH, 50, 107 )
DEFINE( VISIBLE_CAMERA_COMPOSITION_ASSIST, 0, 1 )    // カメラ構図アシスト
DEFINE( CHECK_VALID_GROUND, 1, 1 )    // 表世界チェックの有無
#if defined(GF_PLATFORM_WIN32)
DEFINE( ENABLE_DEBUG_PERFORMANCE_INFO_DRAW, 0, 1 )    // デバッグPERFORMANCE情報表示
#elif defined(GF_PLATFORM_CTR)
DEFINE( ENABLE_DEBUG_PERFORMANCE_INFO_DRAW, 1, 1 )    // デバッグPERFORMANCE情報表示
#endif
DEFINE( RERIVE_ITEM_PER, 30, 100 )  // 隠しアイテムの出現パーセンテージ

DEFINE( ENABLE_BATTLE_HIO, 0, 1 ) //バトルでHIOを有効にする
DEFINE( ENABLE_BATTLE_RARE_EFFECT, 0, 1 ) //バトルでレアエフェクトを強制再生する
DEFINE( ENABLE_BATTLE_WEAK_ENEMY, 0, 1 ) //バトルで敵を弱くする

DEFINE( VISIBLE_STATICMODEL, 1, 1 )
DEFINE( VISIBLE_MOVEMODEL, 1, 1 )
DEFINE( VISIBLE_PLAYER, 1, 1 )
DEFINE( COLLISION_OPACITY, 1, 1 )

DEFINE( VISIBLE_WHERE_IS_THIS, 0, 1 ) // フィールドデバッグ情報表示

DEFINE( VISIBLE_ENVSOUND_INFO, 0, 1 ) // 環境音 情報表示

DEFINE( BATTLE_INTRUDE_FORCE_CALL, 0, 1 )     // バトル( 必ず助けを呼ぶ )
DEFINE( BATTLE_INTRUDE_FORCE_APPEAR, 0, 1 )   // バトル( 必ず助けが来る )

DEFINE( ENABLE_ENTRANCE_FADE_CUT, 0, 1 ) // エントランス演出のフェード処理をカットする。
DEFINE( GM_SKIP_DRAWMODE_SET, 5, 5 ) // フィールドデバッグ情報表示

DEFINE( ENCOUNT_POKE_COLOR_RARE, 0, 1 ) // エンカウントポケモンを必ずレアカラーにする
DEFINE( VISIBLE_BAG, 1, 1 ) // 主人公のカバンを表示
DEFINE( VISIBLE_TRAFFIC_PATH, 0, 1 ) // 往来パス表示
DEFINE( COL_DARW_ONLY_ACTIVE, 0, 1 )    // アクティブなアクターのみ表示
DEFINE( ENABLE_SKYBOX_ALWAYS_UPDATE, 0, 1 ) // 天球の毎フレーム更新有効化

DEFINE( ENABLE_DROPITEM_RND_NUM, 0, 100 ) // ドロップアイテムの乱数値固定化
DEFINE( VISIBLE_CAMERA_TARGET, 0, 1 ) // ターゲット座標表示
DEFINE( CAMERA_STOP, 0, 1 ) // カメラ停止

DEFINE( VISIBLE_CAMERA_TARGET_POSITION, 0, 1 ) // ターゲット座標
DEFINE( TERRAIN_DATA_CHECK, 0, 1 ) // 地形データのチェック有効化

DEFINE( ENC_EFF_FLASH_SYNC_ENABLE, 0, 1 ) // 戦闘導入演出フラッシュのデバッグON/OFF
DEFINE( ENC_EFF_FLASH_SYNC, 8, 20 ) // 戦闘導入演出フラッシュの合計時間
DEFINE( ENC_EFF_FLASH_NUM, 2, 10 ) // 戦闘導入演出フラッシュの回数
DEFINE( ENC_EFF_FLASH_INTERVAL, 2, 20 ) // 戦闘導入演出フラッシュの間隔
DEFINE( ENC_EFF_FLASH_INTERVAL_REVERSE, 1, 1 ) // 戦闘導入演出フラッシュの間隔をマイナスにする
DEFINE( ENC_EFF_FLASH_COLOR, 205, 255 ) // 戦闘導入演出フラッシュの色

DEFINE( ENCOUNT_LOTTERY_INDEX, 0, 10 ) // エンカウント抽選ポケモンを指定番号にする 0は無効
DEFINE( ENABLE_MEMORY_DANGLING_CHECK, 0, 1 ) // メモリのダングリングチェックをするかどうか。

DEFINE( ENABLE_EXTHEAP_CHECK, 1, 1 ) // EXTヒープのイベント終了時メモリLEAKチェック
DEFINE( CAMERA_CHECK_FUTURE_MAX, 0, 1 )     // カメラめり込み確認用、未来予知MAX固定
DEFINE( CAMERA_CHECK_FORCE_MUURAND, 0, 1 )  // カメラめり込み確認用、ムーランド固定
DEFINE( CAMERA_CHECK_DIR_FIX, 0, 1 )        // カメラめり込み確認用、方向固定
DEFINE( ENABLE_MOVEMODEL_INTEREST, 1, 1 )   // 動作モデルの注視処理
DEFINE( ENABLE_UNREGIST_ANIM_STOP, 1, 1 )   // UnRegistされているNPCのアニメ停止有効化