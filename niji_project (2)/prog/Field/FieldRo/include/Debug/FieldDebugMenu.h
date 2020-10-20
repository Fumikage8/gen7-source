/**
* @brief  フィールドデバッグメニュー
* @file   FieldDebugMenu.h
* @author ikuta_junya
* @data   2015.03.08
*/

#if !defined( __FIELDDEBUGMENU_H_INCLUDED__ )
#define __FIELDDEBUGMENU_H_INCLUDED__ 
#pragma once

#if PM_DEBUG

// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <math/include/gfl2_Easing.h>
#include <str/include/gfl2_MsgData.h>

#include "arc_index/ScriptDebugSymbol.gaix" //for GARC_ScriptDebugSymbol_DATA_NUM


// field
#include "Field/FieldDebug/include/FieldDebugEditorStruct.h"

// 前方宣言
namespace Field { namespace MoveModel {
  class FieldMoveModelManager;
}}

GFL_NAMESPACE_BEGIN( GameSys );
class GameManager;
GFL_NAMESPACE_END( GameSys );

GFL_NAMESPACE_BEGIN( gfl2 );
GFL_NAMESPACE_BEGIN( debug );
class DebugWinFunc;
class DebugWinGroup;
class DebugWinItem;
GFL_NAMESPACE_END( debug );
GFL_NAMESPACE_END( gfl2 );

GFL_NAMESPACE_BEGIN( Field );
class EnvManager;
class TrafficManager;
class FieldItemManager;

GFL_NAMESPACE_BEGIN( Effect)
class EffectManager;
GFL_NAMESPACE_END( Effect )
GFL_NAMESPACE_BEGIN( weather)
class WeatherControl;
GFL_NAMESPACE_END( weather )
GFL_NAMESPACE_BEGIN( Debug );

class DebugMapJump;
class DebugMenuCameraGroup;
class DebugTraffic;
class DebugSignal;
class DebugFade;
class DebugEffect;
class DebugTrialModel;
class DebugEncount;
class DebugGimmickEncount;
class DebugPokeFinder;
class DebugDispInfo;
class DebugWeather;
class DebugFishing;
class DebugCharaLayoutEditor;
class DebugEventCameraEditor;
class DebugStaticModel;
class DebugFieldDemo;
class DebugSodateya;
class DebugBattleInst;
class DebugRotomPower;
class FlagWorkImpl;
class FlagWorkSearchImpl;
class DebugMantain;

GFL_NAMESPACE_BEGIN( Menu );
class Terrain;
GFL_NAMESPACE_END( Menu );

/**
* @brief フィールドデバッグメニュー
*/
class FieldDebugMenu
{
  /** @brief  コピー禁止クラスにする */
  GFL_FORBID_COPY_AND_ASSIGN( FieldDebugMenu );

public:
  
  static const u32 DRAW_TAG_TERRAIN_HIGH_MODEL_SIZE = 4;
  struct BLOCK_INFO
  {
    s32 x;
    s32 z;
    s32 drawTagNum;
  };
  
  /**
  * @brief コンストラクタ
  * フィールドデバッグメニューの構築を実行します
  *
  * @param  pHeap         メニュー構築に使用するヒープ
  * @param  pGameManager  ゲーム管理
  *
  * @return 無し
  */ 
  FieldDebugMenu(
    gfl2::heap::HeapBase* pHeap, Field::MoveModel::FieldMoveModelManager* pFieldMoveModelManager,
    Field::EnvManager* pEnvManager, Field::Effect::EffectManager *pEffectManager,
    Field::FieldItemManager* pItemManager, Field::weather::WeatherControl *pWeatherControl, TrafficManager *pTrafficManager 
    );

  /**
  * @brief デストラクタ
  *
  * @return 無し
  */
  virtual ~FieldDebugMenu( void );

  // DebugWinに渡す情報
  struct USER_WORK{
    FieldDebugMenu  *pThis;
    u32             labelId;    // 文字列のラベル値
    u32             datId;
  };

  //
  // 取得系
  //

  /**
  * @brief Field関連デバッグメニューの親グループを取得する
  *
  * @return グループ
  */
  gfl2::debug::DebugWinGroup* GetFieldRootGroup( void ) { return m_pFieldRootGroup; }
  gfl2::debug::DebugWinGroup* GetCameraRootGroup( void );

  /**
  * @brief 地形関連メニュー管理クラスを取得する
  *
  * @return メニュー管理クラス
  */
  Field::Debug::Menu::Terrain* GetTerrainMenu( void );

  /**
   *  @brief  ファインダーデバッグ取得
   */
  Field::Debug::DebugPokeFinder *GetDebugPokeFinder( void ){ return m_pPokeFinder; }

  // DebugInfo
  Field::Debug::DebugDispInfo *GetDebugDispInfo( void ){ return m_pDispInfo; }

  /**
   *  @brief  立体視パラメータを取得する
   */
  inline f32 GetDepthLevel( void ) const { return m_fDepthLevel; }
  inline f32 GetFactor( void ) const { return m_fFactor; }
  f32 *GetDepthLevelPtr( void ){ return &m_fDepthLevel; }
  f32 *GetFactorPtr( void ){ return &m_fFactor; }

  //! ヒープ取得
  gfl2::heap::HeapBase *GetHeap( void ){ return m_pHeap; }
  //! 動作モデルマネージャー取得
  Field::MoveModel::FieldMoveModelManager* GetMoveModelManager( void ){ return m_pFieldMoveModelManager; }
  // 環境マネージャー取得
  Field::EnvManager* GetEnvManager( void ){ return m_pEnvManager; }
  USER_WORK* GetUserWorkTimeCtrl( void ){ return m_pUserWorkTimeControlArray; }
  Field::FieldItemManager* GetItemManager( void ){ return m_pItemManager; }
  u32 GetEventWorkTableSize( s32 idx ){ return m_EventWorkTableSize[idx]; }
  u32 GetEventWorkTotalTableSize( void )
  { 
    u32 size = 0;
    for( u32 i = 0; i < GARC_ScriptDebugSymbol_DATA_NUM; i++ )
    {
      size += m_EventWorkTableSize[ i ];
    }
    return size;
  }

  // カリング取得
  inline b32 isUseCullingForMoveModel  ( void ) { return (b32)m_isUseCullingForMoveModel;   }
  inline b32 isUseCullingForStaticModel( void ) { return (b32)m_isUseCullingForStaticModel; }

  // ピヨリ易さ取得
  inline b32 isUsePiyoriMaxValue( void ) const { return (b32)m_isUsePiyoriMaxValue; }
  inline u32 GetPiyoriMaxValue  ( void ) const { return m_piyoriMaxValue; }
  inline u32 GetPiyoriAngle  ( void ) const { return m_piyoriAngle; }

  // ジャンプ角度取得
  inline b32 isUseJumpAngle( void ) const { return (b32)m_isUseJumpAngle; }
  inline u32 GetJumpAngle  ( void ) const { return m_jumpAngle; }

  // 旋回速度取得
  inline b32 isUseRunTurnSpeed( void ) const { return (b32)m_isUseRunTurnSpeed; }
  inline u32 GetRunTurnSpeed  ( void ) const { return m_runTurnSpeed; }

  // 坂道減衰率取得
  inline b32 isUseSlopeDampingRate( void ) const { return (b32)m_isUseSlopeDampingRate; }
  inline f32 GetSlopeDampingRate  ( void ) const { return m_slopeDampingRate; }

  // 段差ジャンプをおっとっとにするか
  inline b32 isUseOttotto( void ) const { return (b32)m_isUseOttotto; }

  // カイリキのデバッグ情報を取得
  inline b32 IsUseKairikyDebugValue( void ) const { return (b32)m_isUseKairikyDebugValue; }
  inline f32 GetKairikyAngle       ( void ) const { return m_kairikyAngle; }
  inline f32 GetKairikyDistance    ( void ) const { return m_kairikyDistance; }
  inline u32 GetKairikyInterpFrame ( void ) const { return m_kairikyInterpFrame; }

  inline u32 GetWaitSlopeFrame ( void ) const { return m_waitSlopeFrame; }  

  inline f32 GetWallStopAngle      ( void ) const { return m_wallStopAngle;      }
  inline f32 GetRideOffAngle       ( void ) const { return m_rideOffAngle;       }

  inline b32 IsUseTranierEyeParam        ( void ) const { return (b32)m_isUseTranierEyeParam; }
  inline f32 GetTrainerEyeCylinderOffsetY( void ) const { return m_trainerEyeCylinderOffsetY; }
  inline f32 GetTrainerEyeCylinderHeight ( void ) const { return m_trainerEyeCylinderHeight; }

  inline u32 GetTsumadukiCnt             ( void ) const { return m_tsumadukiCnt; }
  inline u32 GetTsumadukiAcceptFrame     ( void ) const { return m_tsumadukiAcceptFrame; }

  inline f32 GetPlayerSpeedRateForWalk   ( void ){ return m_playerSpeedRateForWalk;    }
  inline f32 GetPlayerSpeedRateForRun    ( void ){ return m_playerSpeedRateForRun;     }
  inline f32 GetKentarosSpeedRateForWalk ( void ){ return m_kentarosSpeedRateForWalk;  }
  inline f32 GetKentarosSpeedRateForRun  ( void ){ return m_kentarosSpeedRateForRun;   }
  inline f32 GetSamehaderSpeedRateForWalk( void ){ return m_samehaderSpeedRateForWalk; }
  inline f32 GetSamehaderSpeedRateForRun ( void ){ return m_samehaderSpeedRateForRun;  }
  inline f32 GetMuurandSpeedRateForWalk  ( void ){ return m_muurandSpeedRateForWalk;   }
  inline f32 GetMuurandSpeedRateForRun   ( void ){ return m_muurandSpeedRateForRun;    }
  inline f32 GetRoba2SpeedRateForWalk    ( void ){ return m_roba2SpeedRateForWalk;     }
  inline f32 GetRoba2SpeedRateForRun     ( void ){ return m_roba2SpeedRateForRun;      }
  inline f32 GetKairikySpeedRateForWalk  ( void ){ return m_kairikySpeedRateForWalk;   }
  inline f32 GetKairikySpeedRateForRun   ( void ){ return m_kairikySpeedRateForRun;    }
  inline f32 GetLaplaceSpeedRateForWalk  ( void ){ return m_laplaceSpeedRateForWalk;   }
  inline f32 GetLaplaceSpeedRateForRun   ( void ){ return m_laplaceSpeedRateForRun;    }

  inline u32 IsUsePlayerTurnRate           ( void ){ return m_isUsePlayerTurnRate;        }
  inline u32 GetPlayerTurnRateForDefault   ( void ){ return m_playerTurnRateForDefault;   }
  inline u32 GetPlayerTurnRateForRun       ( void ){ return m_playerTurnRateForRun;       }
  inline u32 GetKentarosTurnRateForDefault ( void ){ return m_kentarosTurnRateForDefault; }
  inline u32 GetKentarosTurnRateForRun     ( void ){ return m_kentarosTurnRateForRun;     }
  inline u32 GetSamehaderTurnRateForDefault( void ){ return m_samehaderTurnRateForDefault;}
  inline u32 GetSamehaderTurnRateForRun    ( void ){ return m_samehaderTurnRateForRun;    }
  inline u32 GetMuurandTurnRateForDefault  ( void ){ return m_muurandTurnRateForDefault;  }
  inline u32 GetMuurandTurnRateForRun      ( void ){ return m_muurandTurnRateForRun;      }
  inline u32 GetRoba2TurnRateForDefault    ( void ){ return m_roba2TurnRateForDefault;    }
  inline u32 GetRoba2TurnRateForRun        ( void ){ return m_roba2TurnRateForRun;        }
  inline u32 GetKairikyTurnRateForDefault  ( void ){ return m_kairikyTurnRateForDefault;  }
  inline u32 GetKairikyTurnRateForRun      ( void ){ return m_kairikyTurnRateForRun;      }
  inline u32 GetLaplaceTurnRateForDefault  ( void ){ return m_laplaceTurnRateForDefault;  }
  inline u32 GetLaplaceTurnRateForRun      ( void ){ return m_laplaceTurnRateForRun;      }

  inline f32 GetKentarosSuddenStopRate ( void ){ return m_kentarosSuddenStopRate; }
  inline f32 GetSamehaderSuddenStopRate( void ){ return m_samehaderSuddenStopRate;}
  inline f32 GetMuurandSuddenStopRate  ( void ){ return m_muurandSuddenStopRate;  }
  inline f32 GetRoba2SuddenStopRate    ( void ){ return m_roba2SuddenStopRate;    }
  inline f32 GetKairikySuddenStopRate  ( void ){ return m_kairikySuddenStopRate;  }
  inline f32 GetLaplaceSuddenStopRate  ( void ){ return m_laplaceSuddenStopRate;  }

  inline f32 GetPlayerStartTurnRate    ( void ) const { return m_playerStartTurnRate;     }
  inline u32 GetPlayerStartTurnFrame   ( void ) const { return m_playerStartTurnFrame;    }
  inline f32 GetKentarosStartTurnRate  ( void ) const { return m_kentarosStartTurnRate;   }
  inline u32 GetKentarosStartTurnFrame ( void ) const { return m_kentarosStartTurnFrame;  }
  inline f32 GetSamehaderStartTurnRate ( void ) const { return m_samehaderStartTurnRate;  }
  inline u32 GetSamehaderStartTurnFrame( void ) const { return m_samehaderStartTurnFrame; }
  inline f32 GetMuurandStartTurnRate   ( void ) const { return m_muurandStartTurnRate;    }
  inline u32 GetMuurandStartTurnFrame  ( void ) const { return m_muurandStartTurnFrame;   }
  inline f32 GetRoba2StartTurnRate     ( void ) const { return m_roba2StartTurnRate;      }
  inline u32 GetRoba2StartTurnFrame    ( void ) const { return m_roba2StartTurnFrame;     }
  inline f32 GetKairikyStartTurnRate   ( void ) const { return m_kairikyStartTurnRate;    }
  inline u32 GetKairikyStartTurnFrame  ( void ) const { return m_kairikyStartTurnFrame;   }
  inline f32 GetLaplaceStartTurnRate   ( void ) const { return m_laplaceStartTurnRate;    }
  inline u32 GetLaplaceStartTurnFrame  ( void ) const { return m_laplaceStartTurnFrame;   }
  
  inline s32 GetPokeIndexMakePokemonIndex( void ) const { return m_pokeIndexMake_pokemonIndexInParty; }
  inline u8 GetPokeIndexMakeLevelBand( void ) const { return m_pokeIndexMake_levelBand; }
  inline u16 GetPokemonIndexMakeRequestKind( void ) const { return m_pokeIndexMake_requestKind; }
  inline u8 GetPokemonIndexMakeRequestGender( void ) const { return m_pokeIndexMake_requestGender; }
  inline u8 GetPokemonIndexMakeRequestLevelBand( void ) const { return m_pokeIndexMake_requestLevelBand; }
  inline u8 GetPokemonIndexMakeRequestNoLegend( void ) const { return m_pokeIndexMake_requestNoLegend; }
  inline u8 GetPokemonIndexMakeSearchDisable( void ) const { return m_pokeIndexMake_searchDisable; }
  inline u64 GetPokemonIndexMakeUniqId( void ) const { return m_pokeIndexMake_uniqId; }
  inline u64 GetPokemonIndexMakeDataStoreId( void ) const { return m_pokeIndexMake_dataStoreId; }
  inline u32 GetPokemonIndexMakePrincibalId( void ) const { return m_pokeIndexMake_principalId; }
  inline u16 GetPokemonIndexMakeMessageId( void ) const { return m_pokeIndexMake_messageId; }
  inline s32 GetPokemonIndexMakeMethod( void ) const { return m_pokeIndexMake_method; }

  inline b32  IsEnableAdjustGround                ( void )                                  { return (m_enableAdjustGround >= 1) ? true:false;                         }
  inline void SetPerformanceForAllScene           ( s32 performanceForAllScene            ) { m_performanceForAllScene            = performanceForAllScene;            }
  inline void SetPerformanceForMoveModelScene     ( s32 performanceForMoveModelScene      ) { m_performanceForMoveModelScene      = performanceForMoveModelScene;      }
  inline void SetPerformanceForStaticModelScene   ( s32 performanceForStaticModelScene    ) { m_performanceForStaticModelScene    = performanceForStaticModelScene;    }
  inline void SetPerformanceForTerrainScene       ( s32 performanceForTerrainScene        ) { m_performanceForTerrainScene        = performanceForTerrainScene;        }
  inline void SetPerformanceForNaminoriScene      ( s32 performanceForNaminoriScene       ) { m_performanceForNaminoriScene       = performanceForNaminoriScene;       }
  inline void SetPerformanceForRideScene          ( s32 performanceForRideScene           ) { m_performanceForRideScene           = performanceForRideScene;           }
  inline void SetPerformanceForRoba2Scene         ( s32 performanceForRoba2Scene          ) { m_performanceForRoba2Scene          = performanceForRoba2Scene;          }
  inline void SetPerformanceForAdjustGround       ( s32 performanceForAdjustGround        ) { m_performanceForAdjustGround        = performanceForAdjustGround;        }
  inline void SetPerformanceForCpuDraw            ( s32 performanceForCpuDraw             ) { m_performanceForCpuDraw             = performanceForCpuDraw;             }
  inline void SetPerformanceForZone               ( s32 performanceForZone                ) { m_performanceForZone                = performanceForZone;                }
  inline void SetPerformanceForTraverse           ( s32 performanceForTraverse            ) { m_performanceForTraverse            = performanceForTraverse;            }
  inline void SetPerformanceForMoveModelPrevUpdate( s32 performanceForMoveModelPrevUpdate ) { m_performanceForMoveModelPrevUpdate = performanceForMoveModelPrevUpdate; }
  inline void SetPerformanceForMoveModelPostUpdate( s32 performanceForMoveModelPostUpdate ) { m_performanceForMoveModelPostUpdate = performanceForMoveModelPostUpdate; }

  inline void SetDrawTagNumForAllScene( s32 drawTagNumForAllScene ) { m_drawTagNumForAllScene = drawTagNumForAllScene; }
  inline void SetDrawTagNumForTerrainHighModel( s32 drawTagNum ) { m_drawTagNumForTerrainHighModel = drawTagNum; }
  inline void SetDrawTagNumForTerrainHighModelIndex( u32 index, s32 x, s32 z, s32 drawTagNum ) { m_drawTagNumTableForTerrainHighModel[index].drawTagNum = drawTagNum; m_drawTagNumTableForTerrainHighModel[index].x = x, m_drawTagNumTableForTerrainHighModel[index].z = z; }
  inline void SetDrawTagNumForTerrainMiddleModel( s32 drawTagNumForTerrainMiddleModel ) { m_drawTagNumForTerrainMiddleModel = drawTagNumForTerrainMiddleModel; }
  inline void SetDrawTagNumForTerrainDistantViewModel( s32 drawTagNumForTerrainDistantViewModel ) { m_drawTagNumForTerrainDistantViewModel = drawTagNumForTerrainDistantViewModel; }

  // 往来設定
  void SetupTrafficManager( TrafficManager *mgr );
  
  inline gfl2::str::MsgData* GetMessageData( void ) { return m_pMsgData; }

  static gfl2::debug::DebugWinFunc* DebugWin_AddItemValueEaseFunc( gfl2::debug::DebugWinGroup *parent , gfl2::heap::HeapBase *heap , const wchar_t *name , gfl2::math::Easing::EaseFunc* target );

  void DebugTestMenu_FlagWork( gfl2::heap::HeapBase *heap, gfl2::debug::DebugWinGroup* parent );
  void DebugTestMenu_ClearFlagWork( gfl2::heap::HeapBase *heap );


  inline DebugMapJump* GetDebugMapJump() { return m_pMapJump; }

#if defined(GF_ENABLE_DEBUG_EDITOR)
  DebugCharaLayoutEditor* GetDebugCharaLayoutEditor( void ){ return m_pCharaLayoutEditor; }
  DebugEventCameraEditor* GetDebugEventCameraEditor( void ){ return m_pEventCameraEditor; }
#endif // GF_PLATFORM_WIN32

  // フラグワークページ関連
  void SetFlagWorkPtr( gfl2::debug::DebugWinGroup *pGroup, s32 index );
  void FreeFlagWorkPtr( s32 index );
  static b32 CreateFlagWorkItemForSearch( void *userWork, gfl2::debug::DebugWinItem *item, s32 index, gfl2::str::StrBuf* pStrBuf, size_t breakHeapFreeSize, s32* pHitCountUp = NULL );
  FlagWorkImpl* GetFlagWorkImpl( void );
  FlagWorkSearchImpl* GetFlagWorkSearchImpl( void );

private:

  class Impl;
  Impl* m_pImpl;
  
  gfl2::heap::HeapBase*       m_pHeap;            // メニュー構築に使用するヒープ
  gfl2::debug::DebugWinGroup* m_pFieldRootGroup;  // フィールド関連メニューのルート
  gfl2::debug::DebugWinGroup* m_pScriptGroup;
  gfl2::debug::DebugWinGroup* m_pFlagWorks;
  gfl2::debug::DebugWinGroup* m_pFlagWork[ GARC_ScriptDebugSymbol_DATA_NUM ];
  gfl2::debug::DebugWinGroup* m_pTimeGroup;
  gfl2::debug::DebugWinItem*  m_pItemItem;
  gfl2::debug::DebugWinGroup* m_pPokemonGroup;
  gfl2::debug::DebugWinGroup* m_pRibbonGrop;
  gfl2::debug::DebugWinGroup* m_pPokemonValidateGroup;
  gfl2::debug::DebugWinGroup* m_pPokemonMakeIndexGroup;
  gfl2::debug::DebugWinGroup* m_pRotom;

  Field::MoveModel::FieldMoveModelManager* m_pFieldMoveModelManager;  // 動作モデルマネージャー
  Field::EnvManager*                       m_pEnvManager;             // 環境マネージャー

  DebugMapJump* m_pMapJump; // マップジャンプのデバッグメニュー

  DebugMenuCameraGroup*       m_pCameraGroup;     // カメラグループ

  u32 m_EventWorkTableSize[ GARC_ScriptDebugSymbol_DATA_NUM ]; //


  USER_WORK*          m_pUserWorkTimeControlArray;  // 時間コントロール項目に持たせる個別情報
  USER_WORK*          m_pUserWorkPokeDiaryCount;    // ポケダイアリーCOUNT
  USER_WORK*          m_pUserWorkRibbonArray;       //  リボン書き換え用

  DebugTraffic                  *m_pTraffic;
  DebugSignal                   *m_pSignal;
  DebugFade                     *m_pFade;
  DebugEffect                   *m_pEffect;
  DebugTrialModel               *m_pTrialModel;
  DebugEncount                  *m_pEncount;
  DebugGimmickEncount           *m_pGimmickEncount;
  DebugPokeFinder               *m_pPokeFinder;
  DebugDispInfo                 *m_pDispInfo;
  DebugWeather                  *m_pWeather;
  DebugFishing                  *m_pFishing;
  DebugSodateya                 *m_pSodateya;
  DebugRotomPower               *m_pRotomPower;
  DebugMantain                  *m_pMantainRecord;

#if defined(GF_ENABLE_DEBUG_EDITOR)
  DebugCharaLayoutEditor        *m_pCharaLayoutEditor;
  DebugEventCameraEditor        *m_pEventCameraEditor;
#endif // GF_PLATFORM_WIN32
  DebugStaticModel              *m_pStaticModel;
  DebugFieldDemo                *m_pFieldDemo;
  DebugBattleInst               *m_pBattleInst;

  f32 m_fDepthLevel;
  f32 m_fFactor;

  u32 m_isUseCullingForMoveModel;
  u32 m_isUseCullingForStaticModel;
  u32 m_isUsePiyoriMaxValue;
  u32 m_piyoriMaxValue;
  u32 m_piyoriAngle;
  u32 m_isUseJumpAngle;
  u32 m_jumpAngle;
  u32 m_isUseRunTurnSpeed;
  u32 m_runTurnSpeed;
  u32 m_isUseSlopeDampingRate;
  f32 m_slopeDampingRate;
  u32 m_isUseOttotto;
  u32 m_isUseKairikyDebugValue;
  f32 m_kairikyAngle;
  f32 m_kairikyDistance;
  u32 m_kairikyInterpFrame;
  u32 m_waitSlopeFrame;
  f32 m_wallStopAngle;
  f32 m_rideOffAngle;
  u32 m_isUseTranierEyeParam;
  f32 m_trainerEyeCylinderOffsetY;
  f32 m_trainerEyeCylinderHeight;
  u32 m_tsumadukiCnt;
  u32 m_tsumadukiAcceptFrame;

  f32 m_playerSpeedRateForWalk;
  f32 m_playerSpeedRateForRun;
  f32 m_kentarosSpeedRateForWalk;
  f32 m_kentarosSpeedRateForRun;
  f32 m_samehaderSpeedRateForWalk;
  f32 m_samehaderSpeedRateForRun;
  f32 m_muurandSpeedRateForWalk;
  f32 m_muurandSpeedRateForRun;
  f32 m_roba2SpeedRateForWalk;
  f32 m_roba2SpeedRateForRun;
  f32 m_kairikySpeedRateForWalk;
  f32 m_kairikySpeedRateForRun;
  f32 m_laplaceSpeedRateForWalk;
  f32 m_laplaceSpeedRateForRun;

  u32 m_isUsePlayerTurnRate;
  u32 m_playerTurnRateForDefault;
  u32 m_playerTurnRateForRun;
  u32 m_kentarosTurnRateForDefault;
  u32 m_kentarosTurnRateForRun;
  u32 m_samehaderTurnRateForDefault;
  u32 m_samehaderTurnRateForRun;
  u32 m_muurandTurnRateForDefault;
  u32 m_muurandTurnRateForRun;
  u32 m_roba2TurnRateForDefault;
  u32 m_roba2TurnRateForRun;
  u32 m_kairikyTurnRateForDefault;
  u32 m_kairikyTurnRateForRun;
  u32 m_laplaceTurnRateForDefault;
  u32 m_laplaceTurnRateForRun;

  u32 m_playerStartTurnRate;
  u32 m_playerStartTurnFrame;
  u32 m_kentarosStartTurnRate;
  u32 m_kentarosStartTurnFrame;
  u32 m_samehaderStartTurnRate;
  u32 m_samehaderStartTurnFrame;
  u32 m_muurandStartTurnRate;
  u32 m_muurandStartTurnFrame;
  u32 m_roba2StartTurnRate;
  u32 m_roba2StartTurnFrame;
  u32 m_kairikyStartTurnRate;
  u32 m_kairikyStartTurnFrame;
  u32 m_laplaceStartTurnRate;
  u32 m_laplaceStartTurnFrame;

  f32 m_kentarosSuddenStopRate;
  f32 m_samehaderSuddenStopRate;
  f32 m_muurandSuddenStopRate;
  f32 m_roba2SuddenStopRate;
  f32 m_kairikySuddenStopRate;
  f32 m_laplaceSuddenStopRate;

  s32 m_enableAdjustGround;
  s32 m_performanceForAllScene;
  s32 m_performanceForMoveModelScene;
  s32 m_performanceForStaticModelScene;
  s32 m_performanceForNaminoriScene;
  s32 m_performanceForRideScene;
  s32 m_performanceForRoba2Scene;
  s32 m_performanceForTerrainScene;
  s32 m_performanceForAdjustGround;
  s32 m_performanceForCpuDraw;
  s32 m_performanceForZone;
  s32 m_performanceForTraverse;
  s32 m_performanceForMoveModelPrevUpdate;
  s32 m_performanceForMoveModelPostUpdate;

  s32 m_drawTagNumForAllScene;

  s32 m_pokeIndexMake_pokemonIndexInParty;
  u8 m_pokeIndexMake_levelBand;
  u16 m_pokeIndexMake_requestKind;
  u8 m_pokeIndexMake_requestGender;
  u8 m_pokeIndexMake_requestLevelBand;
  u8 m_pokeIndexMake_requestNoLegend;
  u8 m_pokeIndexMake_searchDisable;
  u64 m_pokeIndexMake_uniqId;
  u64 m_pokeIndexMake_dataStoreId;
  u32 m_pokeIndexMake_principalId;
  u16 m_pokeIndexMake_messageId;
  s32 m_pokeIndexMake_method;

  BLOCK_INFO m_drawTagNumTableForTerrainHighModel[ DRAW_TAG_TERRAIN_HIGH_MODEL_SIZE ];
  s32 m_drawTagNumForTerrainHighModel;
  s32 m_drawTagNumForTerrainMiddleModel;
  s32 m_drawTagNumForTerrainDistantViewModel;

  gfl2::str::MsgData*           m_pMsgData;
  gfl2::str::MsgData*           m_pRibbonMsgData;

  Field::FieldItemManager* m_pItemManager;

public:

  u32 m_diaryRomCode;   /**< 生成データのロムバージョン*/
  u32 m_diaryType;      /**< ダイアリー種別*/
  u32 m_diaryId;        /**< ダイアリーID（種別内分類）*/
  u32 m_diaryData[3];   /**< 固有データ */

private:
  void DebugTestMenu_Script( gfl2::heap::HeapBase *heap );

  int m_partySaveSlot;   // 手持ちセーブのスロット
  int m_boxSaveSlot;   // Boxセーブのスロット
  
};  // class FieldDebugMenu

GFL_NAMESPACE_END( Debug );
GFL_NAMESPACE_END( Field );

#endif // if PM_DEBUG
#endif // __FIELDDEBUGMENU_H_INCLUDED__
