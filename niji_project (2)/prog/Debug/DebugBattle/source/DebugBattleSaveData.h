#if PM_DEBUG
//=============================================================================
/**
 * @file    DebugBattleSaveData.h
 * @brief   デバッグバトルProc用セーブデータ
 * @author  obata_toshihiro
 * @data    2015.02.25
 * @note    sango_projectから移植
 */
//=============================================================================
#if !defined( DEBUG_BATTLE_SAVEDATA_H_INCLUDED )
#define DEBUG_BATTLE_SAVEDATA_H_INCLUDED 

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <pml/include/pmlib.h>
#include <Battle/include/battle_DefaultPowerUp.h>
#include <Battle/include/battle_SetupParam_Reinforce.h>
#include "./DebugBattleCommon.h"

GFL_NAMESPACE_BEGIN(debug)
GFL_NAMESPACE_BEGIN(battle)


class DebugBattleSaveData
{
  GFL_FORBID_COPY_AND_ASSIGN( DebugBattleSaveData );

public:
  enum {
    SLOT_MAX = 5,
  };

  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   *
   * @param pHeap  使用するヒープ
   */
  //--------------------------------------------------------------------------
  DebugBattleSaveData( gfl2::heap::HeapBase* pHeap );
  
  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  ~DebugBattleSaveData();

  /**
   *  セーブ開始
   */
  void StartSave( u32 slotIdx );

  /**
   *  セーブ終了待ち：trueで終了
   */
  bool WaitSave( void );

  /**
   *  ロード開始
   */
  void StartLoad( u32 slotIdx );
  /**
   *  ロード終了待ち：trueで終了
   */
  bool WaitLoad( bool* bSucceed );

  //---------------------------------------------------------------------------
  /**
   * @brief ポケモンパラメータを取得する
   *
   * @param[out] pDst ポケモンパラメータの格納先
   * @param      idx  取得するポケモンパラメータのインデックス
   */
  //---------------------------------------------------------------------------
  void GetPokeParam( pml::pokepara::PokemonParam* pDst, u32 idx );

  //---------------------------------------------------------------------------
  /**
   * @brief ポケモンパラメータを設定する
   *
   * @param pSrc ポケモンパラメータ
   * @param idx  設定先インデックス
   */
  //---------------------------------------------------------------------------
  void SetPokeParam( const pml::pokepara::PokemonParam* pSrc, u32 idx );


  void GetDefaultPowerUpDesc( u32 index, DefaultPowerUpDesc* dest );
  void SetDefaultPowerUpDesc( u32 index, const DefaultPowerUpDesc& desc );

  u8   GetParam( DBItemID itemID );
  void SetParam( DBItemID item, int value );


  // 録画データのセーブ・ロード
  bool  SaveRecordData( u32 idx, const void* data, u32 dataSize );
  bool  LoadRecordData( u32 idx, void* dst, u32 dataSize );



private:
  gfl2::heap::HeapBase* m_heap;
  gfl2::fs::Result      m_fileResult;
  u8*                   m_pokeData;
  DefaultPowerUpDesc    m_defalutPowerUpDesc[ debug::battle::POKEPARAM_NUM ];
  char                  m_datFilePath[ 64 ];

  enum {
    BITFLAGS_ARY_SIZE = 1 + ((ITEM_BITFLAG_COUNT-1) / 8),
  };

  struct {
    u8  btlMode;
    u8  msgSpeed;
    u8  changeRule;
    u8  stampCount;
    u8  commMode;
    u8  bEffectEnable;
    u8  recMode;
    u8  recIdx;
    u8  time_game_minute;
    u8  time_game_sec;
    u8  time_client_minute;
    u8  time_client_sec;
    u8  time_command;
    u8  bitFlags[ BITFLAGS_ARY_SIZE ];
    u8  bBtlHouseMode;
    u8  weather;
    u8  nearBackGround;
    u8  farBackGround;
    u8  bgm;
    u8  reinforceCond[ MAX_REINFORCE_POKE_NUM ];
    u8  reinforceCount[ MAX_REINFORCE_POKE_NUM ];
  }m_param;

  void  setupDatFilePath( char* dst, u32 slotIdx );
  void  setupRecFilePath( char* dst, u32 slotIdx );
  void  setupFilePathCore( char* dst, u32 idx, const char* baseFilePath );

};  // class DebugBattleSaveData


GFL_NAMESPACE_END(battle)
GFL_NAMESPACE_END(debug)

#endif // DEBUG_BATTLE_SAVEDATA_H_INCLUDED
#endif // PM_DEBUG