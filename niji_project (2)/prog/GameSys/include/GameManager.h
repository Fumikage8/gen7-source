#ifndef __GAMEMANAGER_H__
#define __GAMEMANAGER_H__
//===================================================================
/**
 * @file    GameManager.h
 * @brief   ゲームで全体を通して管理が必要なクラスのインスタンスを持つクラス
 * @author  k.ohno
 * @date    10.12.22
*/
//===================================================================
#pragma once

#include <base/include/gfl2_Singleton.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <ui/include/gfl2_UI.h>
#include <ro/include/gfl2_RoManager.h>

// gfl2
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_types.h>   // for displayNo

#include "GameSys/include/GameFrameMode.h"
#include "GameSys/include/GameEventManager.h"
#include <NetStatic/NetLib/include/NetDefine.h>

#include <debug/include/gfl2_DebugTick.h>

//-----------------------------------------------------------------------------
// クラス前方参照宣言
//-----------------------------------------------------------------------------
namespace GameSys {
  class GameProcManager;
  class GameManager;
  class GameData;
  class PlayTimeManager;
}

namespace ExtSavedata {
  class NijiExtSaveSystem;
}

namespace AppLib{
  namespace Fade{
    class AppFade;
  }
}


GFL_NAMESPACE_BEGIN( Field );
GFL_NAMESPACE_BEGIN( SubScreen );
class FieldSubScreen;
GFL_NAMESPACE_END( SubScreen );
GFL_NAMESPACE_END( Field );

   
namespace Field{
  class FieldSystem;
  class Fieldmap;
  class FieldResident;

  namespace RotomPower{
    class RotomPowerManager;
  }

  namespace FieldWindow{
    class FieldWindowManager;
  }
  namespace FieldScript{
    class FieldScriptSystem;
  }
}

namespace System {
  namespace Skybox {
    class Skybox;
  }
}

namespace BattleInst {
  class BattleInst;
  class BattleInstManager;
}

GFL_NAMESPACE_BEGIN( app );
GFL_NAMESPACE_BEGIN( ui );
class UIViewManager;
GFL_NAMESPACE_END( ui );
GFL_NAMESPACE_END( app );

GFL_NAMESPACE_BEGIN( print );
class GrammaticalAttributeProvider;
GFL_NAMESPACE_END( print );

GFL_NAMESPACE_BEGIN( gfl2 )
GFL_NAMESPACE_BEGIN( lyt )
class DrawingLytWkManager;
GFL_NAMESPACE_END( lyt )
GFL_NAMESPACE_END( gfl2 )


#if PM_DEBUG
#if GFL_DEBUG
#define GAME_MANAGER_DEBUG_TICK_ENABLE 1
#else // GFL_DEBUG
#define GAME_MANAGER_DEBUG_TICK_ENABLE 0
#endif // GFL_DEBUG
#else // PM_DEBUG
#define GAME_MANAGER_DEBUG_TICK_ENABLE 0
#endif // PM_DEBUG



//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
namespace GameSys {

class GameManager : public gfl2::base::Singleton<GameManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(GameManager); //コピーコンストラクタ＋代入禁止

  public:

  // nijiのユニークＩＤを指定
  // SDカードは共用なため、片方のユニークＩＤである必要がある。
	  static const u32 APLICATION_UNIQUE_ID = 0x01B50; // アプリケーション固有ID momiji-s

  static const u8 BUFFER_CLEAR_TARGET_BIT_COLOR = 0x01;
  static const u8 BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL = 0x01 << 1;

  static const f32 PARALLAX_DEPTH_RANGE_ONE; //視差1(立体視標準)

  struct BUFFER_CLEAR_SETTING
  {
    gfl2::gfx::Color color;   
    f32 depth;
    u8 stencil;
    u8 clearTarget;    // クリアを行う対象バッファ、BUFFER_CLEAR_TARGET_BIT〜の論理和
  };


  /// 描画スキップモード
  enum SkipDrawMode
  {
    SKIP_DRAWMODE_NONE,           ///<　スキップなし
    SKIP_DRAWMODE_CPU_OVERFLOW,   ///<　CPUがオーバーしたときに２フレーム分の描画をスキップ(リカバーリー率が高い）
    SKIP_DRAWMODE_FPS_HALF,       ///<　常にFPSを半分にする。 
    SKIP_DRAWMODE_CPU_OVERFLOW_LIGHT, ///< CPUがオーバーしたときに1フレーム分の描画をスキップ(リカバリー率は低い)
    SKIP_DRAWMODE_MAX
  };

  private:
  //gfl2::heap::HeapBase *m_systemHeap;
  gfl2::heap::HeapBase *m_deviceHeap;
  gfl2::heap::HeapBase *m_procCellHeap;

  ///動作フレーム管理用
  FrameMode  m_ReqFrameMode;  // 動作モードリクエスト情報
  FrameMode  m_FrameMode;     // 動作モード
  s32        m_VSyncCount;    // 垂直同期回数制御

  // レンダリングのパラメータ
  f32        m_parallaxDepthRange;  // 立体視の視差の補正値

  ///ここに重要なインスタンスを書いてください
  GameProcManager*            m_procManager;
  bool                        m_fatalEvent;   ///< FatalEvent発生

  app::ui::UIViewManager* m_pUIViewManager;
  GameEventManager*           m_GameEventManager;
  GameData*                   m_GameData;
  PlayTimeManager*            m_PlayTimeManager;          ///< プレイ時間マネージャ
  Field::Fieldmap                         *m_pFieldmap;   ///< フィールドマップワーク.
  Field::FieldResident                    *m_pFieldResident;   ///< フィールドresident
  Field::FieldScript::FieldScriptSystem   *m_FieldScriptSystem;

  print::GrammaticalAttributeProvider* m_pGrammaticalAttributeProvider;  ///< 文法対応モジュール

  System::Skybox::Skybox*              m_pSkybox;                        ///< 天球
  BattleInst::BattleInstManager*       m_pBattleInstManager;             ///< バトル施設マネージャー


  //
  SkipDrawMode mEnableSkipDraw;      ///<
  b8 mIsSwapBuffer;        ///<
  b8 m_bVSyncSkip;
  b8 padding[2];
  b32 mIsSkipDraw;          ///<
  u32 mGpuRunSkipCount; ///< 連続でGPUコマンドの実行をスキップした回数 @fix GFNMCat[663]

  Field::RotomPower::RotomPowerManager* m_pRotomPowerManager;        ///<ロトムパワー効果時間マネージャー

#if PM_DEBUG
  bool mDebugKeyDisable;
#endif

  public:

  //----------------------------------------------------------------------------
  /**
   *  @brief    コンストラクタ
   *  @param    dHeap デバイスヒープ
   */
  //-----------------------------------------------------------------------------
  GameManager(gfl2::heap::HeapBase *dHeap);
  //-----------------------------------------------------------------------------
  /**
  *     @brief デストラクタ
  */
  //-----------------------------------------------------------------------------
  ~GameManager();

  //----------------------------------------------------------------------------
  /**
   *  @brief    初期化
   */
  //-----------------------------------------------------------------------------
  void Initialize(void);
  //----------------------------------------------------------------------------
  /**
   *  @brief    メイン処理
   *  @return   処理が終了する場合false 続行中true
   */
  //-----------------------------------------------------------------------------
  bool Process(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief    GPU処理の実行待ち
   */
  //-----------------------------------------------------------------------------
  void WaitGpuCommandDone();

  //----------------------------------------------------------------------------
  /**
   *  @brief    VSyncWait
   */
  //-----------------------------------------------------------------------------
  void VSyncWait(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief    GpuCommand?
s
   */
  //-----------------------------------------------------------------------------
  void RunCommandList(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief    開放処理
   */
  //-----------------------------------------------------------------------------
  void Finalize(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief    動作フレーム変更リクエスト
   */
  //-----------------------------------------------------------------------------
  void RequestFrameMode( FrameMode mode );

  //----------------------------------------------------------------------------
  /**
   *  @brief    現在の動作フレーム取得
   *
   *  @return   動作フレーム識別子
   */
  //-----------------------------------------------------------------------------
  FrameMode GetFrameMode( void ) const { return m_FrameMode; }

  //----------------------------------------------------------------------------
  /**
   *  @brief    レンダリングのパラメータにデフォルト値を設定する。
   */
  //-----------------------------------------------------------------------------
  void SetDefaultRenderingParameter( void );
  //----------------------------------------------------------------------------
  /**
   *  @brief    レンダリングのパラメータを設定する。
   *            設定し直すまでこの値が使用され続けます。
   *
   *  @param    parallaxDepthRange  立体視の視差の補正値
   *                                 (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
   *                                  gfl2::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
   */
  //-----------------------------------------------------------------------------
  void SetRenderingParameter( const f32 parallaxDepthRange );
  //----------------------------------------------------------------------------
  /**
   *  @brief    レンダリングのパラメータを取得する。
   *
   *  @param[out]    parallaxDepthRange  立体視の視差の補正値
   *                                      (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
   *                                       gfl2::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
   */
  //-----------------------------------------------------------------------------
  void GetRenderingParameter( f32* parallaxDepthRange ) const;

  // @fix SBTS[2501] : 【表示：特定のデモ中に立体視をONにすると右目用映像に不具合が発生する】
  //----------------------------------------------------------------------------
  /**
   *  @brief  右目がレンダリングされるか？
   *
   *  @retval true  レンダリングされる
   *  @retval false レンダリングされない
   */
  //-----------------------------------------------------------------------------
  bool IsUpperRightRendering( void ) const;

  /**
   *  @brief  フィールドシステムを取得(ないこともある)
   *
   *  @return フィールドシステム(NULLの場合ない)
   */
  Field::FieldSystem* GetFieldSystem( void ){ return m_pFieldSystem; }

  /**
  * @brief  フィールドシステムを設定
  *
  * @param  pFieldSystem  フィールドシステム
  *
  * @retun  無し
  */
  void SetFieldSystem( Field::FieldSystem* pFieldSystem ){ m_pFieldSystem = pFieldSystem; }


  //DrawSkip
  //------------------------------------------------------------------
  /**
   * @brief   Drawスキップモードの設定の設定  CPUオーバー版
   * @param  bool
   * @note  CPUがオーバーしていたら、次のDrawを飛ばす処理です。
   */
  //------------------------------------------------------------------
  bool SetEnableSkipDrawMode(SkipDrawMode flg, bool is_assert = true)
  {
    if( (GetIsSkipDraw() == false) || (flg == SKIP_DRAWMODE_NONE) )
    {
      mEnableSkipDraw = flg;
      return true;
    }
    else
    {
      if( is_assert )
      {
        GFL_ASSERT( GetIsSkipDraw() == false );
      }
      return false;
    }
  }


  //------------------------------------------------------------------
  /**
   * @brief   Drawスキップモードの設定の取得
   * @return  bool
   */
  //------------------------------------------------------------------
  SkipDrawMode EnableSkipDrawMode(void){return mEnableSkipDraw;}

  //------------------------------------------------------------------
  /**
   * @brief   このフレームのDrawがスキップされるか？
   * @return  bool
   */
  //------------------------------------------------------------------
  b32 GetIsSkipDraw(void){return mIsSkipDraw;}

  //------------------------------------------------------------------
  /**
   * @brief   GPUがスキップされたカウントの取得(描画されたら0)
   * @return  u32
   */
  //------------------------------------------------------------------
  u32 GetGpuRunSkipCount(void)const{return mGpuRunSkipCount;}

  //------------------------------------------------------------------
  /**
   * @brief   暗転期間高速化のためのVSyncSkipFlag
   */
  //------------------------------------------------------------------
  void SetVSyncSkipFlag( b32 flag ) { m_bVSyncSkip = flag; }

  public:

  //-----------------------------------------------------------------------------
  /**
   * @brief ファイル読み込みマネージャを取得する
   */
  //-----------------------------------------------------------------------------
  gfl2::fs::AsyncFileManager* GetAsyncFileManager( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief UI デバイスマネージャを取得する
   */
  //-----------------------------------------------------------------------------
  gfl2::ui::DeviceManager* GetUiDeviceManager( void ) const;

  //----------------------------------------------------------------------------
  /**
  * @brief 拡張セーブデータ管理システムの取得
  */
  //----------------------------------------------------------------------------
  ExtSavedata::NijiExtSaveSystem * GetNijiExtSaveSystem( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief プロセスマネージャを取得する
   */
  //-----------------------------------------------------------------------------
  inline GameProcManager* GetProcManager( void ) const { return m_procManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief 現在動作しているプロセスがあるかチェック
   */
  //-----------------------------------------------------------------------------
  bool IsProcessExists( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief ゲームイベントマネージャの取得
   */
  //-----------------------------------------------------------------------------
  inline GameEventManager* GetGameEventManager( void ) const { return m_GameEventManager; }

  //-----------------------------------------------------------------------------
  /**
   * @brief 現在動作しているイベントがあるかチェック
   */
  //-----------------------------------------------------------------------------
  bool IsEventExists( void ) const;

  //-----------------------------------------------------------------------------
  /**
   * @brief ゲームデータを取得
   */
  //-----------------------------------------------------------------------------
  inline GameData* GetGameData( void ) const { return m_GameData; }

  //-----------------------------------------------------------------------------
  /**
   * @brief プレイ時間マネージャの取得
   */
  //-----------------------------------------------------------------------------
  inline PlayTimeManager* GetPlayTimeManager( void ) const { return m_PlayTimeManager; };

  //----------------------------------------------------------------------------
  /**
   *  @brief  フィールドマップワーク　のセッター・ゲッター
   *  *FieldmapProcがセットする。
   */
  //-----------------------------------------------------------------------------
  inline void SetFieldmap( Field::Fieldmap* p_fieldmap ) { m_pFieldmap = p_fieldmap; }
  inline Field::Fieldmap* GetFieldmap( void ) const { return m_pFieldmap; }

  //----------------------------------------------------------------------------
  /**
   *  @brief  フィールドresident
   */
  //-----------------------------------------------------------------------------
  inline void SetFieldResident( Field::FieldResident* p_fieldresi ) { m_pFieldResident = p_fieldresi; }
  inline Field::FieldResident* GetFieldResident( void ) const { return m_pFieldResident; }

  //-----------------------------------------------------------------------------
  /**
   * @brief フィールドスクリプトシステムの取得
   */
  //-----------------------------------------------------------------------------
  inline Field::FieldScript::FieldScriptSystem* GetFieldScriptSystem( void ) const { return m_FieldScriptSystem; }

  //-----------------------------------------------------------------------------
  /**
   * @brief 天球の取得
   */
  //-----------------------------------------------------------------------------
  inline System::Skybox::Skybox* GetSkybox( void ) const { return m_pSkybox; }
  
  //-----------------------------------------------------------------------------
  /**
   * @brief バトル施設マネージャーのsetter/getter
   */
  //-----------------------------------------------------------------------------
  inline void SetBattleInstManager( BattleInst::BattleInstManager* pInstManager ) { m_pBattleInstManager = pInstManager; }
  inline BattleInst::BattleInstManager* GetBattleInstManager( void ) const { return m_pBattleInstManager; }

  //--------------------------------------------------------------------------
  /**
   *  @brief  FatalErrorイベント起動フラグをセット
   *
   *  @note   この関数を呼ぶと、フィールドのイベントチェックで、
   *           最優先でFatalErrorEventが呼ばれます。
   *           必ずフィールドのイベントチェックが呼ばれることを保証してから使用！！！
   */
  //--------------------------------------------------------------------------
  void SetFatalErrorEvent( void ){ m_fatalEvent = true; }

  //--------------------------------------------------------------------------
  /**
   *  @brief  FatalErrorイベント起動フラグを取得
   *
   *  @retval true:起動する
   *  @retval false:起動しない
   */
  //--------------------------------------------------------------------------
  bool GetFatalErrorEvent( void ) const{ return m_fatalEvent; }


  // バッファクリア設定

  /**
   *  @brief  指定画面のクリア設定を設定する
   *
   *  @param  display   対象画面
   *  @param  rSetting  クリア設定
   *
   *  @return 無し
   */
  void SetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, const BUFFER_CLEAR_SETTING& rSetting );

  /**
   *  @brief  指定画面のクリア設定を取得する
   *
   *  @param  display   対象画面
   *  @param  pOut      格納先
   *
   *  @return 成否
   */
  b32 GetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, BUFFER_CLEAR_SETTING* pOut ) const;
  void ChangeFrameControl( void );             // フレームコントロールのモード変更


  /**
   *  @brief  フィールドサブスクリーン管理インターフェイスのGET ※FIELD_RESIDENTが消えても必要だから
   *  @return 無し
   */
  inline Field::SubScreen::FieldSubScreen* GetSubScreen(void){ return m_pFieldSubScreen; }

  /**
  *  @brief  ロトムパワー効果時間マネージャー取得
  *  @return Field::RotomPower::RotomPowerManager*
  */
  inline Field::RotomPower::RotomPowerManager* GetRotomPowerManager() { return m_pRotomPowerManager; }


#if PM_DEBUG

  //-----------------------------------------------------------------------------
  /**
  * @brief UIログセーブ開始
  */
  //-----------------------------------------------------------------------------
  void DebugUiDeviceStartLogRec( void );

  //-----------------------------------------------------------------------------
  /**
  * @brief UIログセーブ終了
  */
  //-----------------------------------------------------------------------------
  void DebugUiDeviceEndLogRec( void );
  bool DebugUiDeviceIsLogRec( void ) const;

  //-----------------------------------------------------------------------------
  /**
  * @brief UIログ再生開始
  */
  //-----------------------------------------------------------------------------
  void DebugUiDeviceStartPlayLog( bool is_loop = true );

  //-----------------------------------------------------------------------------
  /**
  * @brief UIログ再生終了
  */
  //-----------------------------------------------------------------------------
  void DebugUiDeviceEndPlayLog( void );
  bool DebugUiDeviceIsPlayLog( void ) const;


  //-----------------------------------------------------------------------------
  /**
  * @brief UIログ再生の一時停止設定
  */
  //-----------------------------------------------------------------------------
  void DebugUiDeviceSoftwareControlEnabel( bool isEnable );

  //--------------------------------------------------------------------------
  /**
  * @brief  UIログをファイル出力
  *
  *  @param  idx   ログファイルインデックス
  */
  //--------------------------------------------------------------------------
  void DebugUiDeviceOutputLogRec( const u32 idx );

  //--------------------------------------------------------------------------
  /**
  * @brief  UIログをファイルから読み込む
  *
  *  @param  idx   ログファイルインデックス
  *
  *  @retval true  成功
  *  @retval false 失敗
  */
  //--------------------------------------------------------------------------
  bool DebugUiDeviceLoadLogRec( const u32 idx );

#endif


  //==========================================================================/
  //以下、private
  private:

  //通信プロセス
  void NetworkProcess(void);
  // イベント処理
  void UpdateEventProcess( void );

  // プロセスマネージャの制御
  void CreateProcManager( void ); // プロセスマネージャを生成する
  void DeleteProcManager( void ); // プロセスマネージャを破棄する
  bool UpdateProcManager( void ); // プロセスマネージャの更新処理
  bool NextFrameUpdateProcManager( void );     // プロセスマネージャの更新処理
  void DrawProcManager( void );                // プロセスマネージャの描画処理

  // フレーム管理
  void ClearFrameControl( void );              // フレーム管理の初期化
  bool IsFrameControl( FrameMode mode ) const; // 今のモードチェック
  bool UpdateFrameControl( void );             // フレームコントロール更新
  void RequestFrameControl( FrameMode mode );  // フレームコントロールのモード変更リクエスト
  bool DoFrameAction( void );                  // １フレームで行うアクション

  
  s32 GetFrameControlVSyncCount( FrameMode mode ) const;  // ???[?h?ł̖SyncWait?̉񐔍
  s32 CalcVSyncCount( s32 vcount, s32 add, b32 * p_overflow ) const; // VSyncCount?̃J?E???g?A?b?v?@?I?[?o?[?t???[?à?F?b?N???遂

  // 
  void checkDrawSkip(b32 isVsyncOverflow);


  //------------------------------------------------------------------
  /**
   * @brief   
   * @param  bool
   */
  //------------------------------------------------------------------
  void SetIsSkipDraw(bool flg){mIsSkipDraw=flg;}

  void SetIsSwapBuffer(bool flg){mIsSwapBuffer=flg;}
  b32 GetIsSwapBuffer(void)const{return mIsSwapBuffer;}

  bool isPause( void );

#if PM_DEBUG
  // アプリケーションキー入力無効状態
  inline bool IsDebugKeyDisable( void ) const { return mDebugKeyDisable; }
#endif

private:

  Field::FieldSystem* m_pFieldSystem;

  //下画面接続部
  Field::SubScreen::FieldSubScreen* m_pFieldSubScreen;


  BUFFER_CLEAR_SETTING m_BufferClearSettingTable[ gfl2::gfx::CtrDisplayNo::NumberOf ];   // バッファクリア設定



#if PM_DEBUG
public:

  // GameManager用DebugTick定義
  struct TickDefine
  {
    enum Index
    {
#define DEFINE(x,y) x,
#include "GameManagerDebugTickDefine.h"
#undef DEFINE
      MAX,
    };
  };

  /**
  * @brief DebugTick定義から定義名を取得する
  *
  * @param index DebugTickの定義値
  *
  * @return 定義名(不正な場合はNULL)
  */ 
  static const c8* GetTickLabel( TickDefine::Index index );

#if GAME_MANAGER_DEBUG_TICK_ENABLE
  static gfl2::debug::DebugTickDump s_TickWork;   // Tick計測ワーク
  static b32 s_TickResetRequest;
#endif // GAME_MANAGER_DEBUG_TICK_ENABLE

#endif // PM_DEBUG

  //フェード
  AppLib::Fade::AppFade* m_pAppFade;

  
  // 描画中レイアウトワークのマネージャ
public:
  //! @brief  描画中レイアウトワークのマネージャを取得する
  gfl2::lyt::DrawingLytWkManager* GetDrawingLytWkManager(void) const;
#if PM_DEBUG
  //! @brief  DrawingLytWkManagerへのLytWkのAddを可能とするか否か
  void SetDrawingLytWkManagerAdditionEnabled(bool enable);
  bool IsDrawingLytWkManagerAdditionEnabled(void) const;
#endif  // PM_DEBUG
private:


#if PM_DEBUG
  //! @brief  起動時設定ファイルの設定をゲーム設定に適用する
  void ApplyStartSettingAccessorToGameSetting(void);
#endif  // PM_DEBUG


  // @fix GFNMCat[3490] 【図鑑】検索画面から戻れなくなる  // デバッグ機能のつもりでPM_DEBUGで括っていたが、デバッグ用途以外で使用している箇所があったので本機能に昇格。PM_DEBUGを外した。
  gfl2::lyt::DrawingLytWkManager* m_pDrawingLytWkManager;
  bool                            m_DrawingLytWkManagerAdditionIsEnabled;

  //
  // メンバ変数
  //

  //
  // デバッグメンバ変数
  //
#if PM_DEBUG
  // ログセーブ終了のR+Xをログに含めない為の差フレーム数
  static const u32 DEBUG_UI_LOG_END_DIFF  = 12;
  // ログファイル出力の最大インデックス
  static const u32 DEBUG_UI_LOG_FILE_MAX  = 20;
  enum{
    DEBUG_UI_LOG_BUTTON = 0,
    DEBUG_UI_LOG_BUTTON_EMU, // nijiで定義追加、CTRで通常版の結果にスティックエミュレートによるキー入力が入っていた(ついでにボタンも修正)
    DEBUG_UI_LOG_CROSS,
    DEBUG_UI_LOG_CROSS_EMU, // nijiで定義追加、CTRで通常版の結果にスティックエミュレートによるキー入力が入っていた
    DEBUG_UI_LOG_STICK,
    DEBUG_UI_LOG_TOUCH,

    /*
    DEBUG_UI_LOG_ACCELEROMETER,
    DEBUG_UI_LOG_GYROSCOPE,
    */

    DEBUG_UI_LOG_MAX,
  };
  // 各デバイスのログのバッファサイズ
  // メモ：基準は0x114E
  static const u32 DEBUG_UI_LOG_BUFFER_MAX[ DEBUG_UI_LOG_MAX ];


  void * mDebugUiDevice[DEBUG_UI_LOG_MAX];

#endif

};

} // namespace GameSys

/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( GameSys::GameManager );


//----------------------------------------------------------------------------
// GameManager DebugTick計測定義
//
// ・使い方
// １、GameManagerDebugTickDefine.hに定義を追加する
// ２、↓のようにTick計測を有効にする (登録してある全ての定義の計測が有効になる)
//     GAME_MANAGER_DEBUG_TICK_SET_FLAG( true)
// ３、↓のように計測処理を組み込む
//     GAME_MANAGER_DEBUG_TICK_START( GameManager::定義名);
//     計測したい処理
//     GAME_MANAGER_DEBUG_TICK_END( GameManager::定義名);
//----------------------------------------------------------------------------
#if GAME_MANAGER_DEBUG_TICK_ENABLE

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ機能のON・OFF
 *  @param  flag    ダンプ機能フラグ　true:ON   false:OFF
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_SET_FLAG(flag) GameSys::GameManager::s_TickWork.SetFlag( (flag) )

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ情報のリセット
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_RESET() GameSys::GameManager::s_TickWork.ResetTickDump()

//----------------------------------------------------------------------------
/**
 *  @brief  チック計測開始
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_START(index) GameSys::GameManager::s_TickWork.StartTickDump( (index), GameManager::GetTickLabel(index) )

//----------------------------------------------------------------------------
/**
 *  @brief  チック計測終了
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_END(index) GameSys::GameManager::s_TickWork.EndTickDump( (index) )

//----------------------------------------------------------------------------
/**
 *  @brief  ダンプ
 *  @param  index   格納先インデックス
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_DUMP_ALL() GameSys::GameManager::s_TickWork.PrintAll()
#define GAME_MANAGER_DEBUG_TICK_DUMP(index) GameSys::GameManager::s_TickWork.Print( (index) )

//----------------------------------------------------------------------------
/**
 *  @brief  μ秒を取得
 *  @param  index インデックス
 *  @retval μ秒 
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_US_GET(index) GameSys::GameManager::s_TickWork.GetTickNowUs((index))

//----------------------------------------------------------------------------
/**
 *  @brief  平均μ秒を取得
 *  @param  index インデックス
 *  @retval 平均μ秒
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_US_AVE_GET(index) GameSys::GameManager::s_TickWork.GetTickAveUs((index))

//----------------------------------------------------------------------------
/**
 *  @brief  最大μ秒を取得
 *  @param  index インデックス
 *  @retval 最大μ秒
 */
//-----------------------------------------------------------------------------
#define GAME_MANAGER_DEBUG_TICK_US_MAX_GET(index) GameSys::GameManager::s_TickWork.GetTickMaxUs((index))

#else // GAME_MANAGER_DEBUG_TICK_ENABLE

#define GAME_MANAGER_DEBUG_TICK_SET_FLAG(flag) /**/
#define GAME_MANAGER_DEBUG_TICK_RESET() /**/
#define GAME_MANAGER_DEBUG_TICK_START(index) /**/
#define GAME_MANAGER_DEBUG_TICK_END(index) /**/
#define GAME_MANAGER_DEBUG_TICK_DUMP_ALL() /**/
#define GAME_MANAGER_DEBUG_TICK_DUMP(index) /**/
#define GAME_MANAGER_DEBUG_TICK_US_GET(index) /**/
#define GAME_MANAGER_DEBUG_TICK_US_AVE_GET(index) /**/
#define GAME_MANAGER_DEBUG_TICK_US_MAX_GET(index) /**/

#endif // GAME_MANAGER_DEBUG_TICK_ENABLE



#endif //__GAMEMANAGER_H__
