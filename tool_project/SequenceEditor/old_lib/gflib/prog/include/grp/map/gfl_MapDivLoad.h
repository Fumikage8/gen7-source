#ifndef __GFL_MAPDIVLOAD_H__
#define __GFL_MAPDIVLOAD_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl_MapDivLoad.h
 *	@brief  マップブロック分割読み込み
 *	@author	tomoya takahashi
 *	@date		2011.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include <gfl_Macros.h>
#include <gfl_Fs.h>

namespace gfl{
namespace grp{
namespace map{

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
class DivLoad
{
  GFL_FORBID_COPY_AND_ASSIGN( DivLoad );

public:
  enum Mode
  {
    MODE_PEACE_LOAD,      // オフセットずらしでの、分割読み込み
    MODE_THREAD_LOAD,     // THREADを使用しての読み込み

    MODE_MAX,     // システム内で使用
  };

public:

  //-----------------------------------------------------------------------------
  /**
   *      コンストラクタ・デストラクタ
   */
  //-----------------------------------------------------------------------------
  DivLoad( void );
  ~DivLoad();

  //----------------------------------------------------------------------------
  /**
   *	@brief  初期化
   *
   *	@param	p_buff      バッファ
   *	@param	buffSize    バッファサイズ
   *	@param  p_tmp_heap  スレッド動作に必要なテンポラリワーク確保用
   *	@param  mode        動作モード
   *	@param  priority    スレッド優先順位
   */
  //-----------------------------------------------------------------------------
  void Initialize( void* p_buff, u32 buffSize, gfl::heap::HeapBase* p_tmp_heap, Mode mode, gfl::base::Thread::Priority priority );

  //----------------------------------------------------------------------------
  /**
   *	@brief  破棄
   */
  //-----------------------------------------------------------------------------
  void Finalize( void );


  //----------------------------------------------------------------------------
  /**
   *	@brief  ファイル読み込み開始
   *
   *	@param	  p_arc         アーカイブファイル
   *	@param    dataID        データID
   *	@param    one_load_size 1回の読み込みサイズ
   */
  //-----------------------------------------------------------------------------
  void StartFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size );

  //----------------------------------------------------------------------------
  /**
   *	@brief  圧縮の掛かったファイルの読み込み開始
   *
   *	@param	p_arc         アーカイブファイル
   *	@param  dataID        データＩＤ
   *	@param  one_load_size 一度に読み込むサイズ
   *	@param  gfl::heap::HeapBase* p_tmp_heap テンポラリバッファ生成用ヒープ
   */
  //-----------------------------------------------------------------------------
  void StartCompFileLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ファイル読み込み1回分実行
   *
   *  @retval true    ファイル読み込み完了
   *  @retval false   ファイル読み込み途中
   */
  //-----------------------------------------------------------------------------
  b32 ContinueFileLoad( void );

  //----------------------------------------------------------------------------
  /**
   *	@brief  ファイル読み込み　リセット
   */
  //-----------------------------------------------------------------------------
  bool ResetFileLoad( void );
  


  //----------------------------------------------------------------------------
  /**
   *	@brief  読み込み中かチェック
   *
   *	@retval true    読み込み中
   *	@retval false   読み込んでない
   */
  //-----------------------------------------------------------------------------
  b32 IsLoading( void ) const;

  //----------------------------------------------------------------------------
  /**
   *	@brief  1つ前に読み込んだファイルサイズを返す。
   */
  //-----------------------------------------------------------------------------
  inline s32 GetLoadFileSize( void ) const { return m_FileSize; }

private:

  Mode              m_Mode;
  gfl::base::Thread::Priority m_Priority;
  b32               m_CompFlag;

  void*             m_pLoadBuff;
  void*             m_pBuff;
  u32               m_BuffSize;
  gfl::fs::ArcFile* m_pArcFile;
  u32               m_DataID;
  u32               m_OneLoadSize;
  u32               m_MemoryOffset;
  u32               m_FileOffset;
  u32               m_FileSize;

  gfl::heap::HeapBase* m_pTmpHeap;


private:

  // 分割読み込み
  void startFilePieceLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size );
  void startFilePieceLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap );
  b32 continueFilePieceLoad(void);
  void resetFilePieceLoad(void);
  b32 isLoadingFilePieceLoad(void) const;

  // スレッド読み込み
  void startFileSyncLoad( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size );
  void startFileSyncLoadComp( gfl::fs::ArcFile* p_arc, u32 dataID, u32 one_load_size, gfl::heap::HeapBase* p_tmp_heap );
  b32 continueFileSyncLoad(void);
  bool resetFileSyncLoad(void);
  b32 isLoadingFileSyncLoad(void) const;
  
};



} //namespace map{
} //namespace grp{  
} //namespace gfl{



#endif // __GFL_MAPDIVLOAD_H__
