//======================================================================
/**
 * @file DendouDemoFrame.cpp
 * @date 2016/03/01 18:03:35
 * @author  nagihashi_toru(移植), Hiroyuki Nakamura
 * @brief メイン処理 dendou_demo_main の移植。niji に合わせて Frame化
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
// niji
#include <base/include/gfl2_Singleton.h>
#include <GameSys/include/GameManager.h>
#include <Sound/include/Sound.h>
#include <AppLib/include/Util/app_util_Common.h>
#include <GameSys/include/GameData.h>
#include <GameSys/include/GameManager.h>
#include <AppLib/include/Util/app_util_heap.h>
#include <Fade/include/gfl2_FadeManager.h>
#include <Fade/include/gfl2_FadeTypes.h>

// mine
#include "DendouDemoFrame.h"
#include "DendouDemoView.h"
#include "App/DendouDemo/include/DendouDemoProc.h"
#include "DendouDemoDebug.h"

//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @namespace	app::dendou_demo
 * @brief			殿堂入りデモ
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
namespace app {
namespace dendou_demo {


//============================================================================================
//	定数定義
//============================================================================================
#if PM_DEBUG
//#define	__ASSERT_CHECK_130328__
#endif

namespace
{

//! メインシーケンス定義
enum {
	MAINSEQ_INIT = 0,		//!< 初期化
	MAINSEQ_RELEASE,		//!< 解放
	MAINSEQ_FADE,				//!< フェード待ち
	MAINSEQ_LOOP,				//!< ループシーン処理
  MAINSEQ_STAMP,      //!< スタンプシーン
	MAINSEQ_END					//!< 終了定義
};

typedef int (Frame::*pSeqFunc)(void);	//!< メインシーケンス型

static const u32 DENDOU_DEMO_FADE_SYNC = app::util::DEFAULT_FADE_SYNC;		//!< フェード速度


//! メインシーケンス関数テーブル
static const pSeqFunc MainSeq[] = {
	&Frame::MainSeq_Init,
	&Frame::MainSeq_Release,
	&Frame::MainSeq_Fade,
	&Frame::MainSeq_Loop,
	&Frame::MainSeq_Stamp,
};

static const u32 LOOP_DATA_POKE_NONE = 0xff;			//!< ループデータ：配置ポケモンなし
static const f32 LOOP_DATA_FRAME_NONE = 2000.0f;	//!< ループデータ：フレーム系データなし

//! ループデータ：ポケモンが１匹のとき
static const LOOP_PUT_DATA LoopPutData1[] =
{
	{ LOOP_DATA_POKE_NONE, 1, LOOP_DATA_FRAME_NONE, LOOP_DATA_FRAME_NONE }, 
	{ LOOP_DATA_POKE_NONE, 0, 395.0f, 138.0f, 15.0f,  258.0f, 1 },
	{ LOOP_DATA_POKE_NONE, 3, LOOP_DATA_FRAME_NONE, LOOP_DATA_FRAME_NONE },
	{ LOOP_DATA_POKE_NONE, 2, LOOP_DATA_FRAME_NONE, LOOP_DATA_FRAME_NONE },
};
//! ループデータ：ポケモンが２匹のとき
static const LOOP_PUT_DATA LoopPutData2[] =
{
	{ LOOP_DATA_POKE_NONE, 1, LOOP_DATA_FRAME_NONE, LOOP_DATA_FRAME_NONE },
	{ LOOP_DATA_POKE_NONE, 0, 395.0f, 138.0f, 15.0f,  258.0f, 3 },
	{ LOOP_DATA_POKE_NONE, 3, LOOP_DATA_FRAME_NONE, LOOP_DATA_FRAME_NONE },
	{ LOOP_DATA_POKE_NONE, 2, 145.0f, 388.0f, 260.0f, 498.0f, 1 },
};
//! ループデータ：ポケモンが３匹のとき
static const LOOP_PUT_DATA LoopPutData3[] =
{
	{ LOOP_DATA_POKE_NONE, 1, 20.0f, 263.0f, 132.0f, 0.0f, 2 },    //50/3 = 166毎
	{ LOOP_DATA_POKE_NONE, 6, 352.0f, 96.0f, 474.0f, 0.0f, 0  },
	{ LOOP_DATA_POKE_NONE, 7, 186.0f, 431.0f, 303.0f, 0.0f, 1 },
	{ LOOP_DATA_POKE_NONE, 2, LOOP_DATA_FRAME_NONE, LOOP_DATA_FRAME_NONE },
};
//! ループデータ：ポケモンが４匹以上のとき
static const LOOP_PUT_DATA LoopPutData4[] =
{
	{ LOOP_DATA_POKE_NONE, 1, 20.0f, 263.0f, 132.0f,  383.0f, 3 },    //１ループ500 = 125毎
	{ LOOP_DATA_POKE_NONE, 0, 395.0f, 138.0f, 15.0f,  258.0f, 0 },   //カメラ回転的には 0,1,3,2…と動く
	{ LOOP_DATA_POKE_NONE, 3, 270.0f, 13.0f, 390.0f,  133.0f, 1 },
	{ LOOP_DATA_POKE_NONE, 2, 145.0f, 388.0f, 260.0f, 498.0f, 2 },
};

//! ループデータテーブル
static const LOOP_PUT_DATA * LoopPutDataTable[] = {
	LoopPutData1,
	LoopPutData2,
	LoopPutData3,
	LoopPutData4,
};
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		コンストラクタ
 *
 * @param		prm		外部設定パラメータ
 */
//--------------------------------------------------------------------------------------------
Frame::Frame() :
  mainWork(),
	subWork( NULL ),
  heap( NULL ),
  drawFrame(0),
  counter(0)
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		デストラクタ
 */
//--------------------------------------------------------------------------------------------
Frame::~Frame()
{
}


//--------------------------------------------------------------------------------------------
/**
* @brief		設定
*/
//--------------------------------------------------------------------------------------------
void Frame::Setup( APP_PARAM * prm, app::util::Heap * heap, GameSys::GameManager * man, app::util::AppRenderingManager * renderingManager )
{
  GFL_ASSERT( prm->pPokeParty != NULL );

  this->heap = heap;
	mainWork.prm = prm;
	mainWork.mainSeq = MAINSEQ_INIT;
  mainWork.pRenderingManager = renderingManager;

  SetGameManager(man);
}

//--------------------------------------------------------------------------------------------
/**
* @brief		描画できるか
*/
//--------------------------------------------------------------------------------------------
bool Frame::CanDraw(void) const
{
  return drawFrame == 0;
}

//------------------------------------------------------------------
/**
 * @brief    PROCの初期化を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result Frame::InitFunc( void )
{
  return applib::frame::RES_FINISH;
}

//------------------------------------------------------------------
/**
 * @brief    PROCのメイン更新を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
 */
//------------------------------------------------------------------
applib::frame::Result Frame::UpdateFunc( void )
{
  if( Update() == UPDATE_FINISH )
  {
    return applib::frame::RES_FINISH;
  }
  return applib::frame::RES_CONTINUE;
}

//------------------------------------------------------------------
/**
 * @brief    PROCのメイン描画を行う関数
 *          純粋仮想ではありません。
 */
//------------------------------------------------------------------
void Frame::DrawFunc( gfl2::gfx::CtrDisplayNo no )
{
  Draw(no);
}

//------------------------------------------------------------------
/**
 * @brief    PROCの終了を行う関数
 * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
 */
//------------------------------------------------------------------
applib::frame::Result Frame::EndFunc( void )
{
  return applib::frame::RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ゲームマネージャ関連取得
 *
 * @param		man		GameProcManager
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Frame::SetGameManager( GameSys::GameManager * man )
{
	MAIN_WORK * mwk = &mainWork;

	mwk->gameManager      = man;
  mwk->pAsyncFileManager = mwk->gameManager->GetAsyncFileManager();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アップデート処理
 *
 * @param		none
 *
 * @return	処理結果
 */
//--------------------------------------------------------------------------------------------
int Frame::Update(void)
{
	MAIN_WORK * mwk = &mainWork;

	mwk->mainSeq = (this->*MainSeq[mwk->mainSeq])();

	if( mwk->mainSeq == MAINSEQ_END ){
		return UPDATE_FINISH;
	}

	SUB_WORK * swk = subWork;

	if( swk != NULL ){
		swk->pView->Update();
	}

	return UPDATE_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		描画処理
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Frame::Draw( gfl2::gfx::CtrDisplayNo no )
{
	SUB_WORK * swk = subWork;

	if( swk == NULL ){
		return;
	}

	swk->pView->Draw( no );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		サブワーク作成
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Frame::CreateSubWork(void)
{
#ifdef __ASSERT_CHECK_130328__
	GFL_PRINT( "■↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓■\n" );
	gfl2::debug::AddPrintFlag( gfl2::debug::PRINT_FLAG_LEVEL1|gfl2::debug::PRINT_FLAG_CATEGORY_HEAP );
#endif // __ASSERT_CHECK_130328__

	subWork = GFL_NEW( heap->GetSystemHeap() )	SUB_WORK;

	subWork->pView = GFL_NEW( heap->GetSystemHeap() ) View( heap, mainWork.pAsyncFileManager, mainWork.pRenderingManager );
	subWork->pView->Initialize();

	CreatePokemonData();
}

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
bool Frame::DeleteSubWork(void)
{
  if( drawFrame == 0 )
  {
    drawFrame ++;
    return false;
  }

	if( subWork->pView->Finalize() == false ){
		return false;
	}

	GFL_DELETE subWork->pView;

	//GFL_DELETE subWork->ui;
	GFL_DELETE subWork;
	subWork = NULL;

#ifdef __ASSERT_CHECK_130328__
	gfl2::debug::ResetPrintFlag();
	GFL_PRINT( "■↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑■\n\n" );
#endif // __ASSERT_CHECK_130328__

	return true;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		フェードインセット
 *
 * @param		next		フェード後のシーケンス
 *
 * @return	MAINSEQ_FADE
 */
//--------------------------------------------------------------------------------------------
int Frame::SetFadeIn( int next )
{
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  pFadeManager->RequestIn(gfl2::Fade::DISP_UPPER, gfl2::Fade::FADE_TYPE_ALPHA, DENDOU_DEMO_FADE_SYNC );

	mainWork.fadeNextSeq = next;
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		フェードアウトセット
 *
 * @param		next		フェード後のシーケンス
 *
 * @return	MAINSEQ_FADE
 */
//--------------------------------------------------------------------------------------------
int Frame::SetFadeOut( int next )
{
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
  pFadeManager->RequestDefaultOut(gfl2::Fade::DISP_UPPER);

  Sound::EndAppBGMReq( Sound::BGM_FADE_MIDDLE, Sound::BGM_FADE_FAST );

	mainWork.fadeNextSeq = next;
	return MAINSEQ_FADE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンデータ作成
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Frame::CreatePokemonData(void)
{
	MAIN_WORK * mwk = &mainWork;
	SUB_WORK * swk = subWork;

	for( u32 i=0; i<mwk->prm->pPokeParty->GetMemberCount(); i++ ){
		const pml::pokepara::PokemonParam * pp = mwk->prm->pPokeParty->GetMemberPointerConst( i );
		if( pp->IsEgg(pml::pokepara::CHECK_BOTH_EGG) == true ){
			continue;
		}
		swk->pp[swk->pokeMax] = pp;
		swk->pokeMax++;
	}
}

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
int Frame::MainSeq_Init(void)
{
	if( subWork == NULL ){
		CreateSubWork();
	}

  enum
  {
    SEQ_LOAD,
#ifdef DENDOU_DEMO_PLAYER_READ_WAIT_ENABLE
    SEQ_SETUP_PLAYER,
#endif
    SEQ_CREATE_DUMMY_POKE,
    SEQ_DELETE_DUMMY_POKE,
    SEQ_CREATE_POKE,
    SEQ_WAIT_POKE,
  };

	MAIN_WORK * mwk = &mainWork;
	SUB_WORK * swk = subWork;

	switch( mwk->subSeq ){
	case SEQ_LOAD:
		if( swk->pView->CheckFileRead( mwk->prm->pMyStatus ) == true ){
			break;
		}
    if( swk->pView->Setup() == true) {
      break;
    }
		swk->pView->CreatePlayerModel( mwk->prm->pMyStatus );
#ifndef DENDOU_DEMO_PLAYER_READ_WAIT_ENABLE
			swk->pView->CreateDummyPokeModel();
#endif
		mwk->subSeq++;
		break;

#ifdef DENDOU_DEMO_PLAYER_READ_WAIT_ENABLE
	case SEQ_SETUP_PLAYER:
		if( swk->pView->SetupPlayerModel() == true ){
			swk->pView->CreateDummyPokeModel();
			mwk->subSeq++;
		}
		break;
#endif

	case SEQ_CREATE_DUMMY_POKE:
		if( swk->pView->SetDummyPokeModelParam() == false ){
			break;
		}
		mwk->subSeq++;
    //fall

	case SEQ_DELETE_DUMMY_POKE:
		swk->pView->CreatePokeModel( swk->pp, swk->pokeMax );
	  mwk->subSeq++;
    //fall

	case SEQ_CREATE_POKE:
		if( subWork->pView->IsPokeModelCreateAll(swk->pokeMax) == true ){
      swk->pView->SetVisiblePlayerModel( true );
	    mwk->subSeq++;
		}
		break;
  case SEQ_WAIT_POKE:
    MakeLoopPutData();
    swk->pView->SetPlayCameraAnime( true );
    subWork->pView->StartCameraAnime();
    swk->pView->StartParticle();
    swk->pView->SetPlayerEndMotion();
    mwk->subSeq = 0;
    return SetFadeIn( MAINSEQ_LOOP );
	}

	return MAINSEQ_INIT;
}

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
int Frame::MainSeq_Release(void)
{
	if( DeleteSubWork() == true ){
#if PM_DEBUG
    if( GFL_SINGLETON_INSTANCE(Debug)->IsLoop() )
    {
      return MAINSEQ_INIT;
    }
#endif
		return mainWork.nextSeq;
	}
	return MAINSEQ_RELEASE;
}

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
int Frame::MainSeq_Fade(void)
{
  gfl2::Fade::FadeManager*  pFadeManager  = GFL_SINGLETON_INSTANCE( gfl2::Fade::FadeManager );
	if( pFadeManager->IsEnd(gfl2::Fade::DISP_UPPER) == true ){
		return mainWork.fadeNextSeq;
	}
	return MAINSEQ_FADE;
}

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
int Frame::MainSeq_Loop(void)
{
	MAIN_WORK * mwk = &mainWork;
	SUB_WORK * swk = subWork;

  enum
  {
    SEQ_WAIT,
    SEQ_START,
    SEQ_WAIT_ANIM,
    SEQ_LOOP,
  };

	u32	frame = static_cast<u32>( swk->pView->GetCameraFrame() );

	switch( mwk->subSeq ){
  case SEQ_WAIT:
    if( counter++ > 0 ) { // 数フレーム待ってから開始していたが、末処理をなくした
      mwk->subSeq = SEQ_START;
    }
    break;
	case SEQ_START:
    if( mwk->prm->isFirstClear ) {
      swk->pView->SetLytAnime(View::ANM_FIRST);
    } else {
      swk->pView->SetLytAnime(View::ANM_DIFENSE);
      swk->canEndKey  = true; //防衛の場合、すぐに終了できる
    }
    swk->pView->SetLytAnime(View::ANM_TELOP_IN);

    mwk->subSeq = SEQ_WAIT_ANIM;
    //fall through
  case SEQ_WAIT_ANIM:
    if ( swk->pView->CheckLytAnime(View::ANM_TELOP_IN) ) {
      break;
    } else {
      swk->pView->SetLytAnime(View::ANM_TELOP_KEEP);
      mwk->subSeq = SEQ_LOOP;
      //fall through
    }

	case SEQ_LOOP:
		if( swk->canEndKey ){
      bool isEnd = swk->pView->UpdateUI() == true;
#if PM_DEBUG
      isEnd |= GFL_SINGLETON_INSTANCE(Debug)->IsLoop();
      if( isEnd )
      {
        GFL_SINGLETON_INSTANCE(Debug)->UpdatePokemonToAging();
      }
#endif
			if( isEnd ){
        if( mwk->prm->isFirstClear ) {
          //初回はスタンプへ
          mwk->subSeq = 0;
          return MAINSEQ_STAMP;
        }
        else
        {
          //防衛は終了
          mwk->nextSeq = MAINSEQ_END;
          mwk->subSeq = 0;
          return SetFadeOut( MAINSEQ_RELEASE );
        }
			}
		}
		break;
	}

  if( swk->pView->IsCameraLastFrame() ){
    swk->canEndKey = true;
  }
  UpdateLoopPutData( frame );

	return MAINSEQ_LOOP;
}


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
int Frame::MainSeq_Stamp(void)
{
	MAIN_WORK * mwk = &mainWork;
	SUB_WORK * swk = subWork;

  enum
  {
    SEQ_STAMP,
    SEQ_STAMP_WAIT,
  };

	u32	frame = static_cast<u32>( swk->pView->GetCameraFrame() );

	switch( mwk->subSeq ){
  case SEQ_STAMP:
    NAGI_PRINT("STAMP_START");
    swk->pView->SetLytAnime( View::ANM_STAMP );
    Sound::PlaySE( SEQ_SE_FLD_DAISHIRENTASSEI_HANKO );
    mwk->subSeq = SEQ_STAMP_WAIT;
    //fall through

  case SEQ_STAMP_WAIT:
    if( swk->pView->CheckLytAnime( View::ANM_STAMP ) == false)
    {
			if( swk->pView->UpdateUI() ){
				mwk->nextSeq = MAINSEQ_END;
        mwk->subSeq = 0;
				return SetFadeOut( MAINSEQ_RELEASE );
			}
    }
    break;
  }

  UpdateLoopPutData( frame );

	return MAINSEQ_STAMP;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ループシーンのモデル配置データ作成
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Frame::MakeLoopPutData(void)
{
	SUB_WORK * swk = subWork;

	u32	index = swk->pokeMax - 1;
	if( index >= GFL_NELEMS(LoopPutDataTable) ){
		index = GFL_NELEMS(LoopPutDataTable) - 1;
	}

  NAGI_PRINT("UseLoopData[%d]\n",index);
	gfl2::std::MemCopy( LoopPutDataTable[index], swk->loopPutData, sizeof(LOOP_PUT_DATA)*4 );

	// 先頭のポケモンを入れる
  swk->loopPutData[1].poke_id = 0;
  swk->pView->PutLoopPokemon( 0, swk->loopPutData[1].put_pos );
	swk->loopPokePutID = 1;

  //スポットの設定もしてしまう
  {
    //初期位置
    swk->pView->SetupSpot(0, swk->loopPutData[1].put_pos );

    //それぞれ次を受け取る
    int next = swk->loopPutData[1].next_index;
    for (int i = 1; i <= index; ++i )
    {
      swk->pView->SetupSpot(i, swk->loopPutData[next].put_pos );
      next = swk->loopPutData[next].next_index;
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ループシーンのモデル配置アップデート
 *
 * @param		frame		現在のフレーム
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void Frame::UpdateLoopPutData( f32 frame )
{
	SUB_WORK * swk = subWork;

	for( int i=0; i<4; i++ ){
		// 配置
		if( frame == swk->loopPutData[i].put_frame ){
			if( swk->loopPutData[i].poke_id == LOOP_DATA_POKE_NONE ){
				if( swk->loopPokePutID >= swk->pokeMax ){
					swk->loopPokePutID = 0;
				}
				swk->loopPutData[i].poke_id = swk->loopPokePutID;
        NAGI_PRINT( "Visible frame[%f] index[%d] putPos[%d] pokeId[%d] monsNo[%d]\n", frame, i, swk->loopPutData[i].put_pos, swk->loopPutData[i].poke_id, swk->pp[swk->loopPutData[i].poke_id]->GetMonsNo() );
				swk->pView->PutLoopPokemon( swk->loopPutData[i].poke_id, swk->loopPutData[i].put_pos );
				swk->loopPokePutID++;
			}
		}
		// 非表示
		if( frame == swk->loopPutData[i].vanish_frame ){
			if( swk->loopPutData[i].poke_id != LOOP_DATA_POKE_NONE ){
        NAGI_PRINT( "Invisible frame[%f] index[%d] putPos[%d] pokeId[%d] monsNo[%d]\n", frame, i, swk->loopPutData[i].put_pos, swk->loopPutData[i].poke_id, swk->pp[swk->loopPutData[i].poke_id]->GetMonsNo() );
				swk->pView->SetVisiblePokeMondel( swk->loopPutData[i].poke_id, false );

        //スポット切り替え予約
        if( swk->pokeMax -1 >= GFL_NELEMS(LoopPutDataTable) )
        {
          int next = swk->loopPutData[i].next_index;
          u32 pos = swk->loopPutData[i].put_pos;
          u32 prev_id = swk->loopPutData[i].poke_id;
          u32 next_id = (swk->loopPutData[i].poke_id+4)%swk->pokeMax;//一周後のデータなので+4
          swk->pView->ReserveChangeSpot( pos, prev_id,  next_id );
          NAGI_PRINT( "Spot frame[%f] prevId[%d] nextId[%d] pos[%d]\n", frame, prev_id, next_id, pos );
        }

				swk->loopPutData[i].poke_id = LOOP_DATA_POKE_NONE;
			}
		}
    //モーション
		if( frame == swk->loopPutData[i].motion_frame ){
			if( swk->loopPutData[i].poke_id != LOOP_DATA_POKE_NONE ){
        NAGI_PRINT( "Motion frame[%f] index[%d] putPos[%d] pokeId[%d] monsNo[%d]\n", frame, i, swk->loopPutData[i].put_pos, swk->loopPutData[i].poke_id, swk->pp[swk->loopPutData[i].poke_id]->GetMonsNo() );
				swk->pView->StartPokeMotion( swk->loopPutData[i].poke_id );
			}
		}

      //スポットの切り替えは４体以上の時のみ行う
    if( frame == swk->loopPutData[i].spot_frame )
    {
      if( swk->pokeMax -1 >= GFL_NELEMS(LoopPutDataTable) )
      {
        swk->pView->ChangeSpot();
        NAGI_PRINT( "ChangeSpot frame[%f]\n", frame );
      }
    }
	}
}


}		// dendou_demo
}		// app

