/**
* @brief  フィールド用DebugTick定義
* @file   FieldDebugTickDefine.h
* @author saita_kazuki
* @data   2015.03.24
*/

// ・概要
// このファイルを直接includeしないでください、FieldDebugTypes.hをincludeして使用します。
// このファイルに定義を追加すると、[R+Xメニュー]-[Field]-[Tick View]メニューに項目が追加され、Tick値を見ることができます。

// ・使い方
// １、このファイルに定義を追加する
// ２、↓のようにTick計測を有効にする (登録してある全ての定義の計測が有効になる)
//     FIELD_DEBUG_TICK_SET_FLAG( true)
// ３、↓のように計測処理を組み込む
//     FIELD_DEBUG_TICK_START( Field::debug::FieldDebugTypes::定義名);
//     計測したい処理
//     FIELD_DEBUG_TICK_END( Field::debug::FieldDebugTypes::定義名);


// FieldTestProc::UpdateFunc
DEFINE( UPDATE,                 "Update" )              // 更新全体
DEFINE( DEBUG_UPDATE,           "-Debug" )              // デバッグ処理
DEFINE( MMDL_UPDATE,            "-MoveModel" )          // 動作モデルのUpdateAllModels
DEFINE( TRAVERSE_MDL_FAST,      "-TraversModelFast" )   // SceneGraphManager::TraverseModelFast
DEFINE( COLLISION,              "-Collision" )          // コリジョン
DEFINE( CAMERA_UPDATE,          "-Camera" )             // カメラ更新
DEFINE( GRASS_UPDATE,           "-Grass" )              // エンカウント草更新
DEFINE( SHADOW_UPDATE,          "-Shadow" )             // 影更新
DEFINE( EFFECT_UPDATE,          "-Effect" )             // エフェクト更新
DEFINE( TRRAIN_UPDATE,          "-Trrain" )             // 地形更新
DEFINE( PRE_DRAW,               "PreDraw" )             // PreDrawFunc()
// DrawFunc
DEFINE( DRAW_LEFT,              "DrawLeft" )            // 描画上画面
DEFINE( PIPELINE_UPDATE,        "-Update" )             // RenderingPipeLine::Update
DEFINE( PIPELINE_UPDATE_EDEGE_MAP,    " -Edge" )        // EdgeMapSceneRenderPath::Update
DEFINE( PIPELINE_UPDATE_MAIN,         " -Main" )        // MainRenderPath::Update
DEFINE( PIPELINE_UPDATE_MAIN_SET_DRAWABLE,         "  -SetDrawable" )       // MainRenderPath::Update内、表示対象Node更新
DEFINE( PIPELINE_UPDATE_MAIN_DRAW_UPDATE,         "  -DrawManager" )        // DrawManager::Update
DEFINE( PIPELINE_UPDATE_OUTLINE_POST, " -Post" )        // OutLinePostRenderPath::Update
DEFINE( PIPELINE_UPDATE_EFFECT,       " -Effect" )      // gfl2::Effect::EffectRenderPath::Update
DEFINE( PIPELINE_UPDATE_LAYOUT,       " -Layout" )      // gfl2::lyt::LayoutRenderPath::Update
DEFINE( PIPELINE_UPDATE_UTILTEXT,     " -UtilText" )    // UtilTextRenderPath

DEFINE( PIPELINE_EXECUTE,       "-Execute" )            // EdgeMapSceneRenderPath::Execute
DEFINE( EXE_EDEGE_MAP,          " -Edge" )              // EdgeMapSceneRenderPath::Execute
DEFINE( EXE_MAIN,               " -Main" )              // MainRenderPath::Execute
DEFINE( EXE_OUTLINE_POST,       " -Post" )              // OutLinePostRenderPath::Execute
DEFINE( EXE_EFFECT,             " -Effect" )            // gfl2::Effect::EffectRenderPath::Execute
DEFINE( EXE_LAYOUT,             " -Layout" )            // gfl2::lyt::LayoutRenderPath::Execute
DEFINE( EXE_UTILTEXT,           " -UtilText" )          // UtilTextRenderPath