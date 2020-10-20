//=============================================================================
/**
 * @brief 技データ
 * @file pml_WazaDataSystem.h
 * @author obata_toshihiro
 * @date 2011.05.06
 */
//=============================================================================
#ifndef __PML_WAZADATASYSTEM_H__
#define __PML_WAZADATASYSTEM_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <str/include/gfl2_StrBuf.h>
#include <str/include/gfl2_MsgData.h>
#include <pml/include/pml_Type.h>
#include <pml/include/wazasick_def.h>
#include <pml/include/waza/pml_WazaDataConst.h>

namespace pml {
  namespace wazadata { 

    class WazaData;

    extern void Initialize( gfl2::heap::HeapBase* heap );
    extern void Finalize( void );
    //extern void GetWazaName( WazaNo wazano, gfl2::str::StrBuf* strbuf );
    //extern void GetWazaInfo( WazaNo wazano, gfl2::str::StrBuf* strbuf );

    extern void LoadWazaData( WazaNo wazano, WazaData* buffer );
    extern int GetParam( WazaNo wazano, ParamID paramID );
    extern bool GetFlag( WazaNo wazano, WazaFlag flag );

    //---------------------------------------------------------------------
    /**
     * @brief 技のMaxPP値を取得する
     * @param wazano  技No.
     * @param upCnt  「マックスアップ」を使用した回数
     */
    //---------------------------------------------------------------------
    u8 GetMaxPP( WazaNo wazano, u8 upCnt );

    //---------------------------------------------------------------------
    /**
     * @brief 技の威力値を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    u16 GetPower( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技のタイプを取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    PokeType GetType( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技のダメージタイプを取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    WazaDamageType GetDamageType( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技のカテゴリを取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    WazaCategory GetCategory( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技の必中フラグを取得する。
     * @note つばめがえしなどがTRUEを返す。
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    bool IsAlwaysHit( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技の必ずクリティカルフラグを取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    bool IsMustCritical( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技のダメージワザかどうかを取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    bool IsDamage( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技のクリティカルランクを取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    u8 GetCriticalRank( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技の天候効果を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    WazaWeather GetWeather( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 技の状態異常効果を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    WazaSick GetSick( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 状態異常継続パラメータ取得を取得する
     * @param wazano  技No.
     * @retval SICKCONT_PARAM
     */
    //---------------------------------------------------------------------
    SICKCONT_PARAM GetSickCont( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief ランク効果の種類数を取得する
     * @param[in]  wazano  ワザID
     * @param[out] volume  ランク効果の程度( +ならアップ, -ならダウン, 戻り値==RANKEFF_NULLの時のみ0 )
     * @retval WazaRankEffect  ランク効果ID
     */
    //---------------------------------------------------------------------
    u8 GetRankEffectCount( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief ランク効果を取得する
     * @param[in]  wazano  ワザID
     * @param[in]  idx     何番目のランク効果か？( 0〜 )
     * @param[out] volume  ランク効果の程度( +ならアップ, -ならダウン, 戻り値==RANKEFF_NULLの時のみ0 )
     * @retval WazaRankEffect ランク効果ID
     */
    //---------------------------------------------------------------------
    WazaRankEffect  GetRankEffect( WazaNo wazano, u32 idx, int* volume );

    //---------------------------------------------------------------------
    /**
     * @brief 全力技として使用した場合の、変化後の技の技No.を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    WazaNo GetZenryokuWazaNo( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 全力技として使用した場合の、変化後の技の威力値を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    u16 GetZenryokuWazaPower( WazaNo wazano );

    //---------------------------------------------------------------------
    /**
     * @brief 全力技として使用した場合の、追加効果を取得する
     * @param wazano  技No.
     */
    //---------------------------------------------------------------------
    ZenryokuWazaEffect GetZenryokuWazaEffect( WazaNo wazano );


  } // namespace wazadata
} // namespace pml

#endif // __PML_WAZADATA_SYSTEM_H__
