//=============================================================================
/**
 * @brief  ポケモンパーソナルシステム
 * @file   pml_PersonalSystem.h
 * @author obata_toshihiro
 * @date   2010.12.20
 */
//=============================================================================
#ifndef __PML_PERSONALSYSTEM_H__
#define __PML_PERSONALSYSTEM_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_StrBuf.h>
#include <pml/include/pml_Type.h>
#include <pml/include/personal/pml_WazaOboeData.h>
#include "./pml_PersonalConst.h"

/**
 * @namespace pml
 * @brief ポケモンライブラリ全体の名前空間
 */
namespace pml { 
  /**
   * @namespace pml::personal
   * @brief ポケモンパーソナルデータを扱う名前空間
   */
  namespace personal {

    class PersonalData;
    class WazaOboeData;
    class GrowTable;
    class EvolutionTable;
    class MegaEvolveTable;

    extern void Initialize( gfl2::heap::HeapBase* heap );
    extern void Finalize( void );

    extern u16 GetMonsName( STRCODE* buffer, MonsNo monsno );
    extern u16 GetMonsName( gfl2::str::StrBuf* buffer, MonsNo monsno );

    //-------------------------------------------------------------------------
    // パーソナルデータ
    //-------------------------------------------------------------------------
    extern void LoadPersonalData( MonsNo monsno, FormNo formno );
    extern void LoadPersonalData( MonsNo monsno, FormNo formno, PersonalData* buffer );
    extern u32 GetPersonalParam( ParamID paramID );         // パーソナルパラメータ
    extern SexType GetPersonalSexType( void );              // 性別タイプを取得する
    extern TokuseiPattern GetTokuseiPattern( void );        // 持っている特性のパターンを返す
    extern bool CheckPersonalWazaMachine( u16 machineNo );  // 技マシンで技を覚えられるか？
    extern bool CheckPersonalWazaOshie(u16 oshieNo);      // 技教えで技を覚えられるか？
    extern bool CheckPersonalBpWazaOshie(u16 wazaNo);      // 技教えで技を覚えられるか？

    //-------------------------------------------------------------------------
    // 技覚え
    //-------------------------------------------------------------------------
    extern void LoadWazaOboeData( MonsNo monsno, FormNo formno );
    extern void LoadWazaOboeData( MonsNo monsno, FormNo formno, WazaOboeData* buffer );
    extern void StartAsyncReadWazaOboe( MonsNo monsno, FormNo formno, gfl2::heap::HeapBase * p_temp_heap );
    extern bool WaitAsyncReadFinishWazaOboe( void );
    extern u8 GetWazaOboeNum( void );                                     // 覚える技の数
    extern u16 GetWazaOboeLevel( u16 learn_index );                       // 技を習得するレベル
    extern WazaNo GetWazaOboeWazaNo( u16 learn_index );                   // 技No.
    extern WazaOboeData::OboeWazaKind GetWazaOboeKind( u16 learn_index ); // 覚え技の種類

    //-------------------------------------------------------------------------
    // 成長曲線
    //-------------------------------------------------------------------------
    extern void LoadGrowTable( u32 grow_type );
    extern void LoadGrowTable( u32 grow_type, GrowTable* buffer );
    extern void LoadGrowTable( MonsNo monsno, FormNo formno );
    extern void LoadGrowTable( MonsNo monsno, FormNo formno, GrowTable* buffer );
    extern u32 GetMinExp( u8 level );  // 指定したレベルになるための最小経験値

    //-------------------------------------------------------------------------
    // 進化
    //-------------------------------------------------------------------------
    extern void LoadEvolutionTable( MonsNo monsno, FormNo formno );
    extern void LoadEvolutionTable( MonsNo monsno, FormNo formno, EvolutionTable* buffer );
    extern u8 GetEvolutionRouteNum( void );                        // 進化経路の数
    extern EvolveCond GetEvolutionCondition( u8 route_index );     // 進化の条件
    extern EvolveCond GetEvolutionCondition( MonsNo next_monsno ); // 進化の条件
    extern u16 GetEvolutionParam( u8 route_index );                // 進化条件に関するパラメータ
    extern MonsNo GetEvolvedMonsNo( u8 route_index );              // 進化後のモンスターNo.
    extern FormNo GetEvolvedFormNo( u8 route_index );              // 進化後のフォルムNo.
    extern bool IsEvolvedFormNoSpecified( u8 route_index );        // 進化後のフォルムNo.が指定されているか？
    extern u8 GetEvolveEnableLevel( u8 route_index );              // 進化可能なレベル

    //-------------------------------------------------------------------------
    // メガ進化
    //-------------------------------------------------------------------------
    extern void LoadMegaEvolveTable( MonsNo monsno );
    extern void LoadMegaEvolveTable( MonsNo monsno, MegaEvolveTable* buffer );
    extern u8 GetMegaEvolveRouteNum( void );                                     // 進化経路の数
    extern MegaEvolveCond GetMegaEvolveCondition( u8 route_index );              // 進化の条件
    extern MegaEvolveCond GetMegaEvolveCondition( MonsNo next_monsno );          // 進化の条件
    extern u16 GetMegaEvolveParam( u8 route_index );                             // 進化条件に関するパラメータ
    extern FormNo GetMegaEvolvedFormNo( u8 route_index );                        // 進化後のモンスターNo.
    extern bool CheckMegaForm( MonsNo monsno, FormNo formno );                   // メガ進化後のフォルムか？

    //-------------------------------------------------------------------------
    // ゲンシカイキ
    //-------------------------------------------------------------------------
    extern bool CheckGensiKaiki( MonsNo monsno, FormNo formno );  // ゲンシカイキ後のフォルムか？

    //-------------------------------------------------------------------------
    // ウルトラバースト
    //-------------------------------------------------------------------------
    extern bool CheckUltraBurst(const MonsNo monsno, const FormNo formno);  // ウルトラバースト後のフォルムか？

    //-------------------------------------------------------------------------
    // 種ポケモン
    //-------------------------------------------------------------------------
    extern MonsNo GetSeedMonsNo( MonsNo monsno );  // 種ポケモンのモンスターNo.

    //-------------------------------------------------------------------------
    // メテノ専用
    //-------------------------------------------------------------------------
    extern bool   METENO_IsShellForm( FormNo formno );         // 殻つきフォルムか？
    extern bool   METENO_IsCoreForm( FormNo formno );          // コアフォルムか？
    extern FormNo METENO_GetShellFormNo( FormNo coreFormNo );  // 対応する殻つきフォルムを取得する
    extern FormNo METENO_GetCoreFormNo( FormNo shellFormNo );  // 対応するコアフォルムを取得する


  } // namespace personal
} // namespace pml 

#endif //__PML_PERSONALSYSTEM_H__
