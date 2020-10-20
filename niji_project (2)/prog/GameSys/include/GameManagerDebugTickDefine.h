/**
* @brief  GameManager用DebugTick定義
* @file   GameManagerDebugTickDefine.h
* @author saita_kazuki
* @data   2015.03.25
*/

// ・概要
// このファイルを直接includeしないでください、GameManager.hでincludeして使用します。
// このファイルに定義を追加すると、[R+Xメニュー]-[GameManager Tick View]メニューに項目が追加され、Tick値を見ることができます。

// ・使い方
// １、このファイルに定義を追加する
// ２、↓のようにTick計測を有効にする (登録してある全ての定義の計測が有効になる)
//     GAME_MANAGER_DEBUG_TICK_SET_FLAG( true)
// ３、↓のように計測処理を組み込む
//     GAME_MANAGER_DEBUG_TICK_START( GameManager::TickDefine::定義名);
//     計測したい処理
//     GAME_MANAGER_DEBUG_TICK_END( GameManager::TickDefine::定義名);

DEFINE( UPDATE,              "Update" )         // DoFrameAction()内、isUpDate==true時の処理
DEFINE( UPDATE_EVENT,        "-Event" )         // UpdateEventProcess()
DEFINE( UPDATE_PROC,         "-Proc" )          // UpdateProcManager()
DEFINE( UPDATE_SOUND,        "-SoundUpdate" )   // Sound::Update()
DEFINE( UPDATE_NEXT,         "UpdateNext" )     // DoFrameAction()内、isUpDate==false時の処理(1/30モード時)
DEFINE( DRAW,                "Draw" )           // DrawProcManager()内、gfl2::gfx::GFGL::Finish()直前まで
DEFINE( DRAW_PRE,            "-Pre" )           // m_procManager->PreDraw();
DEFINE( DRAW_LEFT,           "-Left" )          // 左画面全体(上画面)
DEFINE( DRAW_LEFT_PROC,      " -Proc" )         // GameProc::Draw( LEFT )
DEFINE( DRAW_LEFT_DEBUG,     " -Debug" )        // DebugWin_Draw( LEFT )
DEFINE( DRAW_RIGHT,          "-Right" )         // 右画面全体(立体視)
DEFINE( DRAW_RIGHT_PROC,     " -Proc" )         // GameProc::Draw( RIGHT )
DEFINE( DRAW_RIGHT_DEBUG,    " -Debug" )        // DebugWin_Draw( RIGHT )
DEFINE( DRAW_DOWN,           "-Down" )          // 下画面全体
DEFINE( DRAW_DOWN_PROC,      " -Proc" )         // GameProc::Draw( DOWN )
DEFINE( DRAW_DOWN_DEBUG,     " -Debug" )        // DebugWin_Draw( DOWN )