//======================================================================
/**
 * @file SoundTestProc.h
 * @date 2015/10/15 17:40:55
 * @author saito_nozomu
 * @brief サウンドテスト用プロセス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================
#if PM_DEBUG

#if !defined __SOUND_TEST_PROC_H_INCLUDED__
#define __SOUND_TEST_PROC_H_INCLUDED__
//#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <sound/include/gfl2_SoundHeap.h>

#include <System/include/nijiAllocator.h>
#include <Print/include/WordSet.h>
#include <GameSys/include/GameProc.h>

#ifdef  GF_PLATFORM_CTR
#include <gflnet2/include/p2p/gflnet2_NetGame.h>
#endif

#include "Debug/SoundTest/include/SoundContainer.h"

#include "Debug/SoundTest/include/SoundTestDefine.h"

//typedef MyRenderingPipeLine LayoutRenderingPipeLine;

#ifdef GF_HIO_SOUND_EDITOR_ENABLE
GFL_NAMESPACE_BEGIN(Sound)
class HioSoundEditor;
GFL_NAMESPACE_END(Sound)
#endif //GF_HIO_SOUND_EDITOR_ENABLE

GFL_NAMESPACE_BEGIN(gfl2)
GFL_NAMESPACE_BEGIN(util)
class DrawUtilText;
class UtilTextRenderingPipeLine;
GFL_NAMESPACE_END(util)

GFL_NAMESPACE_BEGIN(lyt)
class LytSys;
GFL_NAMESPACE_END(lyt)

GFL_NAMESPACE_END(gfl2)

GFL_NAMESPACE_BEGIN(print)
class WordSet;
GFL_NAMESPACE_END(print)

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(sound)

class UpdaterBase;

/**
 * @class SoundTestProc
 * @brief サウンドテストプロセスクラス
 */
class SoundTestProc : public GameSys::GameProc
{
  GFL_FORBID_COPY_AND_ASSIGN( SoundTestProc );
  
public:
  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  SoundTestProc();

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  ~SoundTestProc();

  //---------------------------------------------------------------------------
  /**
   * @brief   PROCの初期化を行う関数
   * @param   プロセス管理マネージャー
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事でUpdateFuncに進む
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result InitFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief   PROCの終了を行う関数
   * @param   プロセス管理マネージャー
   * @return  Result 動作が終了すると RES_FINISHを返してもらう事で終わったと解釈する
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result EndFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief	  PROCのメイン更新を行う関数
   * @param   プロセス管理マネージャー
   * @return	Result 動作が終了すると RES_FINISHを返してもらう事でEndFuncに進む
   */
  //---------------------------------------------------------------------------
  virtual gfl2::proc::Result UpdateFunc(gfl2::proc::Manager* pManager);

  //---------------------------------------------------------------------------
  /**
   * @brief	PROCの描画処理
   * @param pManager   プロセス管理マネージャー
   * @param displayNo  描画対象画面
   * 登録したPROCによる、フレームバッファへの描画を実行します。
   * ディスプレイバッファの切り替え、ディスプレイバッファへの転送は呼び出し元が行います。
   */
  //---------------------------------------------------------------------------
  virtual void DrawFunc( gfl2::proc::Manager* pManager, gfl2::gfx::CtrDisplayNo displayNo );

private:
  //---------------------------------------------------------------------------
  /**
   * @brief	更新関数
   * @return bool false 終了 true 継続
   */
  //---------------------------------------------------------------------------
  bool Update();

  //---------------------------------------------------------------------------
  /**
   * @brief	プロセス開始時の初期化処理
   */
  //---------------------------------------------------------------------------
  void Enter();

  // ヒープ関係
	gfl2::heap::NwAllocator* m_heap_alloc;      //!ヒープ
	gfl2::heap::NwAllocator* m_device_alloc;    //ヒープ!

  System::nijiAllocator*    m_pDeviceHeapAllocator; //!アロケーター

  int m_page;                             //!現在のコントロールページ

#ifdef GF_HIO_SOUND_EDITOR_ENABLE
  Sound::HioSoundEditor* m_pSoundEditor;  //!HIOサウンドエディット機能用クラス
#endif //GF_HIO_SOUND_EDITOR_ENABLE

  SoundContainer m_container;           //!サウンドコンテナクラス
  gfl2::sound::SoundHeap* m_sound_heap; //!サウンドヒープ
  
	gfl2::str::MsgData* m_msg_sound;    //!サウンドメッセージ
	gfl2::str::MsgData* m_msg_voice;    //!鳴き声メッセージ
  gfl2::str::MsgData* m_msg_debug;    //!デバッグメッセージ
	gfl2::str::StrBuf* m_str;           //!汎用メッセージバッファ
	gfl2::str::StrBuf* m_str_search;    //!検索メッセージ用バッファ
  print::WordSet*     m_pWordSet;     //!ワードセット
	

  UpdaterBase * m_pUpdater;                 //! 現在制御している更新クラスへのポインタ
  UpdaterBase * m_pUpdaterList[PAGE_MAX];   //!更新クラスリスト


  // ここからView関連
  gfl2::util::UtilTextRenderingPipeLine*		m_pRenderingPipeLine; //!レンダリングパイプライン
	gfl2::util::DrawUtilText* m_pDrawUtilText;                      //!テキスト描画ユーティリティ

  gfl2::lyt::LytSys* m_pLytSys;                                   //!レイアウトシステム
};  // class SoundTestProc




GFL_NAMESPACE_END(sound)
GFL_NAMESPACE_END(debug)

#endif // __SOUND_TEST_PROC_H_INCLUDED__
#endif // PM_DEBUG

