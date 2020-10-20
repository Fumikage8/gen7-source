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
///#include <ro/include/gfl2_RoManager.h>@saito不要

// gfl2
#include <gfx/include/gfl2_Color.h>
#include <gfx/include/gfl2_types.h>   // for displayNo @todo アプリ用に定義をきる可能性がある 150217 ikuta_junya

namespace gfl2 {
  namespace lyt {
    class LayoutRenderPath;  // @todo 今だけLayoutを使えるようにしておく  kawada_koji_20150209
  }
}

#include "GameSystem/include/GameFrameMode.h"
#include "GameSystem/include/GameEventManager.h"

//-----------------------------------------------------------------------------
// クラス前方参照宣言
//-----------------------------------------------------------------------------
namespace GameSystem {
  class GameProcManager;
  class GameManager;
  class GameData;
}

namespace Field{
  class FieldSystem;
  class Fieldmap;
  class FieldResident;
/**
  namespace FieldScript{
    class FieldScriptSystem;
  }
*///@saito不要
}

/**
GFL_NAMESPACE_BEGIN( print );
class GrammaticalAttributeProvider;
GFL_NAMESPACE_END( print );
*/
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
namespace GameSystem {

class GameManager : public gfl2::base::Singleton<GameManager>
{
  GFL_FORBID_COPY_AND_ASSIGN(GameManager); //コピーコンストラクタ＋代入禁止

  public:

  /*
     任天堂様から提供を受けている情報
     ---
     製品コード          ：CTR-P-AKJA
     タイトルコード      ：0x00055D01
     ユニークID          ：0x0055D

     製品コード          ：CTR-P-AK2A
     タイトルコード      ：0x00055E00
     ユニークID          ：0x0055E
     ---
  */
  // XのユニークＩＤを指定 @todo
  // SDカードは共用なため、片方のユニークＩＤである必要がある。
  static const u32 APLICATION_UNIQUE_ID = 0x0055D; // アプリケーション固有ID

  static const u8 BUFFER_CLEAR_TARGET_BIT_COLOR = 0x01;
  static const u8 BUFFER_CLEAR_TARGET_BIT_DEPTH_STENCIL = 0x01 << 1;

  struct BUFFER_CLEAR_SETTING
  {
    gfl2::gfx::Color color;   
    f32 depth;
    u8 stencil;
    u8 clearTarget;    // クリアを行う対象バッファ、BUFFER_CLEAR_TARGET_BIT～の論理和
  };


  private:
  gfl2::heap::HeapBase *m_systemHeap;
  gfl2::heap::HeapBase *m_deviceHeap;
  gfl2::heap::HeapBase *m_procCellHeap;

  ///動作フレーム管理用
  GameSystem::FrameMode  m_ReqFrameMode;  // 動作モードリクエスト情報
  GameSystem::FrameMode  m_FrameMode;     // 動作モード
  u8         m_FrameCount;    // フレームカウント 0のときtopFrame 1のときnextFrame
  s32        m_VSyncCount;    // 垂直同期回数制御

  // レンダリングのパラメータ
  f32        m_parallaxDepthRange;  // 立体視の視差の補正値

  ///ここに重要なインスタンスを書いてください
  GameSystem::GameProcManager*            m_procManager;
  bool                        m_fatalEvent;   ///< FatalEvent発生

  GameSystem::GameEventManager*           m_GameEventManager;
  GameData*                               m_GameData;
  Field::Fieldmap                         *m_pFieldmap;
  Field::FieldResident                    *m_pFieldResident;
  ///Field::FieldScript::FieldScriptSystem* m_FieldScriptSystem;@saito不要

  ///print::GrammaticalAttributeProvider* m_pGrammaticalAttributeProvider;  ///< 文法対応モジュール@saito不要

#if GFL_DEBUG
  bool mDebugKeyDisable;
#endif

	bool mEndRequest;

  public:

  //----------------------------------------------------------------------------
  /**
   *	@brief    コンストラクタ
   *  @param    sHeap システムヒープ
   *  @param    dHeap デバイスヒープ
   */
  //-----------------------------------------------------------------------------
  GameManager(gfl2::heap::HeapBase *sHeap, gfl2::heap::HeapBase *dHeap);
  //-----------------------------------------------------------------------------
  /**
  *     @brief デストラクタ
  */
  //-----------------------------------------------------------------------------
  ~GameManager();

  //----------------------------------------------------------------------------
  /**
   *	@brief    初期化
   */
  //-----------------------------------------------------------------------------
  void Initialize(void);
  //----------------------------------------------------------------------------
  /**
   *	@brief    メイン処理
   *  @return   処理が終了する場合false 続行中true
   */
  //-----------------------------------------------------------------------------
  bool Process(void);

	void EndRequest()
	{
		mEndRequest = true;
	}

	bool IsEndRequest()
	{
		return mEndRequest;
	}

  //----------------------------------------------------------------------------
  /**
   *	@brief    開放処理
   */
  //-----------------------------------------------------------------------------
  void Finalize(void);

  //----------------------------------------------------------------------------
  /**
   *	@brief    動作フレーム変更リクエスト
   */
  //-----------------------------------------------------------------------------
  void RequestFrameMode( GameSystem::FrameMode mode );

  //----------------------------------------------------------------------------
  /**
   *	@brief    レンダリングのパラメータにデフォルト値を設定する。
   */
  //-----------------------------------------------------------------------------
  void SetDefaultRenderingParameter( void );
  //----------------------------------------------------------------------------
  /**
   *	@brief    レンダリングのパラメータを設定する。
   *	          設定し直すまでこの値が使用され続けます。
   *
   *  @param    parallaxDepthRange  立体視の視差の補正値
   *	                               (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
   *                                  gfl2::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
   */
  //-----------------------------------------------------------------------------
  void SetRenderingParameter( const f32 parallaxDepthRange );
  //----------------------------------------------------------------------------
  /**
   *	@brief    レンダリングのパラメータを取得する。
   *
   *  @param[out]    parallaxDepthRange  立体視の視差の補正値
   *	                                    (gfl2::grp::GraphicsSystem::PARALLAX_DEPTH_RANGE_か
   *                                       gfl2::grp::g3d::StereoCameraのGetDepthRange関数の戻り値を渡して下さい。)
   */
  //-----------------------------------------------------------------------------
  void GetRenderingParameter( f32* parallaxDepthRange ) const;

  // @fix SBTS[2501] : 【表示：特定のデモ中に立体視をONにすると右目用映像に不具合が発生する】
  //----------------------------------------------------------------------------
  /**
   *	@brief  右目がレンダリングされるか？
   *
   *	@retval true  レンダリングされる
   *	@retval false レンダリングされない
   */
  //-----------------------------------------------------------------------------
  bool IsUpperRightRendering( void ) const;

  /**
   *	@brief  フィールドシステムを取得(ないこともある)
   *
   *	@return フィールドシステム(NULLの場合ない)
   */
  Field::FieldSystem* GetFieldSystem( void ){ return m_pFieldSystem; }

  /**
  *	@brief  フィールドシステムを設定
  *
  * @param  pFieldSystem  フィールドシステム
  *
  *	@retun  無し
  */
  void SetFieldSystem( Field::FieldSystem* pFieldSystem ){ m_pFieldSystem = pFieldSystem; }

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

  //-----------------------------------------------------------------------------
  /**
   * @brief プロセスマネージャを取得する
   */
  //-----------------------------------------------------------------------------
  inline GameSystem::GameProcManager* GetProcManager( void ) const { return m_procManager; }

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
  inline GameSystem::GameEventManager* GetGameEventManager( void ) const { return m_GameEventManager; }

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
  ///inline Field::FieldScript::FieldScriptSystem* GetFieldScriptSystem( void ) const { return m_FieldScriptSystem; }@saito不要

  //--------------------------------------------------------------------------
  /**
   *	@brief  FatalErrorイベント起動フラグをセット
   *
   *  @note   この関数を呼ぶと、フィールドのイベントチェックで、
   *           最優先でFatalErrorEventが呼ばれます。
   *           必ずフィールドのイベントチェックが呼ばれることを保証してから使用！！！
   */
  //--------------------------------------------------------------------------
  void SetFatalErrorEvent( void ){ m_fatalEvent = true; }

  //--------------------------------------------------------------------------
  /**
   *	@brief  FatalErrorイベント起動フラグを取得
   *
   *  @retval true:起動する
   *  @retval false:起動しない
   */
  //--------------------------------------------------------------------------
  bool GetFatalErrorEvent( void ) const{ return m_fatalEvent; }


  // バッファクリア設定

  /**
   *	@brief  指定画面のクリア設定を設定する
   *
   *  @param  display   対象画面
   *  @param  rSetting  クリア設定
   *
   *  @return 無し
   */
  void SetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, const BUFFER_CLEAR_SETTING& rSetting );

  /**
   *	@brief  指定画面のクリア設定を取得する
   *
   *  @param  display   対象画面
   *  @param  pOut      格納先
   *
   *  @return 成否
   */
  b32 GetBufferClearSetting( gfl2::gfx::CtrDisplayNo display, BUFFER_CLEAR_SETTING* pOut ) const;



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
  bool IsFrameControl( GameSystem::FrameMode mode ) const; // 今のモードチェック
  bool UpdateFrameControl( void );             // フレームコントロール更新
  void ChangeFrameControl( void );             // フレームコントロールのモード変更
  void RequestFrameControl( GameSystem::FrameMode mode );  // フレームコントロールのモード変更リクエスト
  bool DoFrameAction( void );                  // １フレームで行うアクション

  // ポーズ処理
  bool isPause( void );

#if GFL_DEBUG
  // UIログ再生の一時停止設定
  void DebugUiDeviceSoftwareControlEnabel( bool isEnable );
  // アプリケーションキー入力無効状態
  inline bool IsDebugKeyDisable( void ) const { return mDebugKeyDisable; }
#endif

  //! @name  Layoutサンプルのための今だけの処理  @todo 今だけLayoutを使えるようにしておく  kawada_koji_20150209
  //@{
private:
  gfl2::lyt::LayoutRenderPath* m_pLayoutRenderPath;


  Field::FieldSystem* m_pFieldSystem;


  BUFFER_CLEAR_SETTING m_BufferClearSettingTable[ gfl2::gfx::CtrDisplayNo::NumberOf ];   // バッファクリア設定

public:
  void SetLayoutRenderPath(gfl2::lyt::LayoutRenderPath* pLayoutRenderPath) { m_pLayoutRenderPath = pLayoutRenderPath; }
  gfl2::lyt::LayoutRenderPath* GetLayoutRenderPath(void) const { return m_pLayoutRenderPath; }
  //@}

};

} // namespace GameSystem

/** ---------------------------------------------------------------------
//! @brief  SINGLETONの前方参照宣言
// ---------------------------------------------------------------------*/
SINGLETON_INSTANCE_DECLARATION( GameSystem::GameManager );



#endif //__GAMEMANAGER_H__
