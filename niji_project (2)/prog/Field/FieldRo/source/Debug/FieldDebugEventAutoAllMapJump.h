#if PM_DEBUG
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugEventAutoAllMapJump.cpp
 *	@brief  自動全マップジャンプ
 *	@author	tomoya takahashi
 *	@date		2015.06.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#if !defined( __FIELD_DEBUG_EVENT_AUTO_ALLMAPJUMP_H__ )
#define __FIELD_DEBUG_EVENT_AUTO_ALLMAPJUMP_H__
#pragma once


#include "Field/FieldStatic/include/Event/EventMapJump.h"
#include "Field/FieldStatic/include/Debug/DebugSymbolNameLoader.h"


GFL_NAMESPACE_BEGIN(Field);


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
/**
 *  @class 自動全マップジャンプ
 */
class DebugEventAutoAllMapJump : public GameSys::GameEvent
{
  GFL_FORBID_COPY_AND_ASSIGN(DebugEventAutoAllMapJump); //コピーコンストラクタ＋代入禁止

public:

  enum Mode
  {
    MODE_ALL = 0,     // 全てのマップ
    MODE_ONLY_WORLD,  // 指定ワールドに含まれるマップ 
    MODE_RANDOM,      // RANDOMマップジャンプ
    MODE_APP_RETURN,  // アプリケーションにジャンプして帰ってくるを繰り返しながらのマップジャンプ
    MODE_MAX
  };

    /** @brief  初期化設定 */
  class InitializeDescription
  {
  public:

    InitializeDescription( void ) :
       m_Mode( MODE_ALL ),
       m_StartZoneID( static_cast<ZoneID>( 0 ) ),
       m_WorldID( static_cast<WorldID>( 0 ) ),
       m_bBCancel(false),
       m_bHeapInfoSave(false),
       m_bBlockHeapInfoSave(false)
    {
    }

    ~InitializeDescription( void )
    {
    }

    Mode                          m_Mode;         // ジャンプ型式
    ZoneID                        m_StartZoneID;  // 開始ゾーンID
    WorldID                       m_WorldID;      // 対象ワールド( MODE_ONLY_WORLDのみ使用 )
    u32                           m_bBCancel;     // BChangeの有効無効
    b32                           m_bHeapInfoSave;    // ヒープ状況のHIOファイル出力を有効にするかのオプションです
    b32                           m_bBlockHeapInfoSave;
  };



public:
  DebugEventAutoAllMapJump( gfl2::heap::HeapBase* heap );
  virtual ~DebugEventAutoAllMapJump();

  inline void SetStartZoneID( ZoneID zoneID ) { m_ZoneID = zoneID; }
  void SetDesc( const InitializeDescription& rDesc );


  //--------------------------------------------------------------
  /**
   * @brief   イベントを動かしてよいかどうかの判定関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @retval  true  起動してよい
   * @retval  false 起動しない
   */
  //--------------------------------------------------------------
  virtual bool BootChk(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   bootchkがtrueの場合にすぐ呼び出される初期化関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void InitFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   * @return  GameSys::GMEVENT_RESULT  イベント制御関数の戻り値
   */
  //--------------------------------------------------------------
  virtual GameSys::GMEVENT_RESULT MainFunc(GameSys::GameManager* gmgr);

  //--------------------------------------------------------------
  /**
   * @brief   呼び出される関数 純粋仮想関数
   * @param   gmgr       ゲームマネージャークラス
   */
  //--------------------------------------------------------------
  virtual void EndFunc(GameSys::GameManager* gmgr);

private:

  enum JumpType
  {
    JUMPTYPE_SKIP,    // スキップ
    JUMPTYPE_NORMAL,  // ノーマル
    JUMPTYPE_JF,      // JFジャンプ
  };


  enum AppReturnType
  {
    APPRETURN_KEEP,
    APPRETURN_NORMAL,
    APPRETURN_JF,
    APPRETURN_MAX,
  };

private:

  //--------------------------------------------------------------
  /**
   * @brief   対象ゾーンにジャンプできるかチェック
   * @param   zoneId 対象ゾーン
   */
  //--------------------------------------------------------------
  JumpType IsJump(GameSys::GameManager* gmgr, ZoneID zoneId ) const;


  InitializeDescription m_InitDesc;

  ZoneID m_ZoneID;
  s32 m_CountUp;
  JumpType m_JumpType;
  gfl2::heap::HeapBase* m_pHeap;


  u32 m_MinTotalFreeHeapSize;
  ZoneID m_MinTotalFreeHeapSizeZoneId;
  u32 m_MinAreaResourceFreeSize;
  ZoneID m_MinAreaResourceFreeSizeZoneId;


  u32 m_AppReturnCount;
  u32 m_AppReturnType;

  u32 m_MapJumpCount;

private:

  class HeapInfoSaver;
  HeapInfoSaver* m_pHeapInfoSaver;
  Field::Debug::SymbolNameLoader* m_pZoneNameLoader;
  Field::Debug::SymbolNameLoader* m_pAreaNameLoader;
  Field::Debug::SymbolNameLoader* m_pWorldNameLoader;

  u32 m_LowestFreeSizeZoneID;   // モジュール読込チェックにて最も空きサイズの少なかったゾーンのID
  s32 m_LowestFreeSize;         // モジュール読込チェックにて最も少ない空きサイズ

};


GFL_NAMESPACE_END(Field);

#endif	// __FIELD_DEBUG_EVENT_AUTO_ALLMAPJUMP_H__


#endif // PM_DEBUG