﻿//======================================================================
/**
 * @file DendouDemoFrame.h
 * @date 2016/03/01 18:03:23
 * @author  nagihashi_toru(移植), Hiroyuki Nakamura
 * @brief メイン処理 dendou_demo_main の移植。niji に合わせて Frame化
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined( __DENDOU_DEMO_MAIN_H__ )
#define __DENDOU_DEMO_MAIN_H__

#include <AppLib/include/Frame/AppFrameCellSuper.h>
#include <pml/include/pml_PokeParty.h>
#include <PokeTool/include/PokeTool.h>

#if PM_DEBUG
//#include "dendou_demo_debug.h"
#endif // PM_DEBUG

namespace gfl2 { namespace fs { class AsyncFileManager; } }
namespace app { namespace dendou_demo { struct APP_PARAM; class View; } }
namespace GameSys { class GameManager; }
namespace app { namespace util { class AppRenderingManager; } }
namespace app { namespace util { class Heap; } }
namespace Savedata { class MyStatus; }

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::dendou_demo
 * @brief			殿堂入りデモ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace dendou_demo {

//--------------------------------------------------------------------------------------------
/**
 * @brief	ループシーンの配置データ
 */
//--------------------------------------------------------------------------------------------
typedef struct {
	u16	poke_id;				//!< 配置ポケモンID
	u16	put_pos;				//!< 配置位置
	f32	put_frame;			//!< 配置フレーム
	f32	vanish_frame;		//!< 非表示フレーム
  f32 motion_frame;   //!< モーションフレーム
  f32 spot_frame;   //!< スポット切り替えフレーム（４体の時のみ）
  u32 next_index;     //!< 次のIndex
}LOOP_PUT_DATA;

//--------------------------------------------------------------------------------------------
/**
 * @brief	サブワーク（他画面遷移時に解放される）
 */
//--------------------------------------------------------------------------------------------
typedef struct {
	View * pView;		//!< グラフィックワーク
	const pml::pokepara::PokemonParam * pp[pml::PokeParty::MAX_MEMBERS];	//!< ポケモンデータ
	u32	pokeMax;																													//!< ポケモン数
	u32	loopPokePutID;	//!< ループシーンのポケモン配置位置ID
  bool canEndKey;     //!< 終了キー入力有効値

	LOOP_PUT_DATA loopPutData[4];		//!< ループシーンの配置データ

}SUB_WORK;

//--------------------------------------------------------------------------------------------
/**
 * @brief	メインワーク
 */
//--------------------------------------------------------------------------------------------
struct MAIN_WORK{
	APP_PARAM * prm;		//!< 外部設定パラメータ

	// ゲームマネージャ関連
	GameSys::GameManager * gameManager;				//!< ゲームマネージャ
  gfl2::fs::AsyncFileManager * pAsyncFileManager;     //!< ファイルリードマネージャ
  app::util::AppRenderingManager * pRenderingManager; //!< レンダリングパイプライン

	int	mainSeq;			//!< メインシーケンス
	int	nextSeq;			//!< メインシーケンス遷移用シーケンス
	int	fadeNextSeq;	//!< フェード後に遷移するシーケンス
	int	subSeq;				//!< サブシーケンス

  MAIN_WORK() : prm(NULL), gameManager(NULL), pAsyncFileManager(NULL), pRenderingManager(NULL), mainSeq(0), nextSeq(0), fadeNextSeq(0), subSeq(0)
  {}
};


//--------------------------------------------------------------------------------------------
/**
 * @brief	メインシステム
 */
//--------------------------------------------------------------------------------------------
class Frame : public applib::frame::CellSuper {

    GFL_FORBID_COPY_AND_ASSIGN( Frame );
public:
	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		コンストラクタ
	 */
	//--------------------------------------------------------------------------------------------
	Frame(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		デストラクタ
	 */
	//--------------------------------------------------------------------------------------------
	virtual ~Frame();

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		設定
	 */
	//--------------------------------------------------------------------------------------------
  void Setup( APP_PARAM * prm, app::util::Heap * heap, GameSys::GameManager * man, app::util::AppRenderingManager * renderingManager );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		描画できるか
	 */
	//--------------------------------------------------------------------------------------------
  bool CanDraw(void) const;
  //以下CellSuper部分
  //------------------------------------------------------------------
  /**
   * @brief    PROCの初期化を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result InitFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief    PROCのメイン更新を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result UpdateFunc( void );

  //------------------------------------------------------------------
  /**
   * @brief    PROCのメイン描画を行う関数
   *          純粋仮想ではありません。
   */
  //------------------------------------------------------------------
  virtual void DrawFunc( gfl2::gfx::CtrDisplayNo no );

  //------------------------------------------------------------------
  /**
   * @brief    PROCの終了を行う関数
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //------------------------------------------------------------------
  virtual applib::frame::Result EndFunc( void );

private:
  //以下本来のFrame
	//! アップデートの戻り値
	enum {
		UPDATE_CONTINUE = 0,	//!< 継続
		UPDATE_FINISH,				//!< 終了
	};

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		ゲームマネージャ関連取得
	 *
	 * @param		man		GameProcManager
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void SetGameManager( GameSys::GameManager * man );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		アップデート処理
	 *
	 * @param		none
	 *
	 * @return	処理結果
	 */
	//--------------------------------------------------------------------------------------------
	int Update(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		描画処理
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void Draw( gfl2::gfx::CtrDisplayNo no );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		サブワーク作成
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void CreateSubWork(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		サブワーク削除
	 *
	 * @param		none
	 *
	 * @retval	"true = 完了"
	 * @retval	"false = それ以外"
	 */
	//--------------------------------------------------------------------------------------------
	bool DeleteSubWork(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		フェードインセット
	 *
	 * @param		next		フェード後のシーケンス
	 *
	 * @return	MAINSEQ_FADE
	 */
	//--------------------------------------------------------------------------------------------
	int SetFadeIn( int next );

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		フェードアウトセット
	 *
	 * @param		next		フェード後のシーケンス
	 *
	 * @return	MAINSEQ_FADE
	 */
	//--------------------------------------------------------------------------------------------
	int SetFadeOut( int next );


	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		ポケモンデータ作成
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void CreatePokemonData(void);

#if PM_DEBUG
	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		ポケモンデータ削除（デバッグ用）
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void DeletePokemonData(void);
#endif //PM_DEBUG

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		ループシーンのモデル配置データ作成
	 *
	 * @param		none
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void MakeLoopPutData(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		ループシーンのモデル配置アップデート
	 *
	 * @param		frame		現在のフレーム
	 *
	 * @return	none
	 */
	//--------------------------------------------------------------------------------------------
	void UpdateLoopPutData( f32 frame );


public:
	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		メインシーケンス：初期化
	 *
	 * @param		none
	 *
	 * @return	次のシーケンス
	 *
	 *	定義：MAINSEQ_INIT
	 */
	//--------------------------------------------------------------------------------------------
	int MainSeq_Init(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		メインシーケンス：解放
	 *	
	 * @param		none
	 *
	 * @return	次のシーケンス
	 *
	 *	定義：MAINSEQ_RELEASE
	 */
	//--------------------------------------------------------------------------------------------
	int MainSeq_Release(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		メインシーケンス：フェード待ち
	 *
	 * @param		none
	 *
	 * @return	次のシーケンス
	 *
	 *	定義：MAINSEQ_FADE
	 */
	//--------------------------------------------------------------------------------------------
	int MainSeq_Fade(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		メインシーケンス：メイン処理
	 *
	 * @param		none
	 *
	 * @return	次のシーケンス
	 *
	 *	定義：MAINSEQ_MAIN
	 */
	//--------------------------------------------------------------------------------------------
	int MainSeq_Main(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		メインシーケンス：ループシーン処理
	 *
	 * @param		none
	 *
	 * @return	次のシーケンス
	 *
	 *	定義：MAINSEQ_LOOP
	 */
	//--------------------------------------------------------------------------------------------
	int MainSeq_Loop(void);

	//--------------------------------------------------------------------------------------------
	/**
	 * @brief		メインシーケンス：スタンプシーン処理
	 *
	 * @param		none
	 *
	 * @return	次のシーケンス
	 *
	 *	定義：MAINSEQ_STAMP
	 */
	//--------------------------------------------------------------------------------------------
	int MainSeq_Stamp(void);

private:
	MAIN_WORK	mainWork;			//!< メインワーク
	SUB_WORK * subWork;			//!< サブワーク
	app::util::Heap *	heap;		//!< ヒープ
  u32 drawFrame;
  u32 counter;
};

}		// dendou_demo
}		// app

#endif	// __DENDOU_DEMO_MAIN_H__

