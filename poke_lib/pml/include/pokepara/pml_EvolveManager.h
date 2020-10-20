//=============================================================================
/**
 * @brief  ポケモンの進化
 * @file   pml_EvolveManager.h
 * @author obata_toshihiro
 * @date   2011.01.28
 */
//=============================================================================
#ifndef __PML_EVOLVESYSTEM_H__
#define __PML_EVOLVESYSTEM_H__
#pragma once

#include <pml/include/pml_Type.h>


///
namespace pml {
  /**
   * @namespace pml::pokepara
   */
  namespace pokepara { 

    class CoreParam;


    //------------------------------------------------------------------------
    /**
     * @brief 進化マネジャー
     * @attention  フォルム違いでの進化テーブル違いへの対応はしていない
     */
    //------------------------------------------------------------------------
    class EvolveManager 
    { 
      public:
        static const u16 EVOLVE_FAMILIARITY = 220;  ///< 進化に必要な「なつき度」

        static const u16 EVOLVE_FRIENDLY    =  50;  ///< 進化に必要な「なかよし度」


        //---------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        EvolveManager( void );

        /**
         * @brief デストラクタ
         */
        virtual ~EvolveManager();



        //---------------------------------------------------------------------
        // 進化チェック
        //---------------------------------------------------------------------
        /**
         * @brief レベルアップによる進化後のモンスターNo.を取得する
         * @param poke  チェック対象のポケモン
         * @retval 進化後のモンスターNo.
         * @note 進化できない場合は, 現在のモンスターNo.を返す
         */
        MonsNo GetEvolvedMonsNo_byLevelUp( const CoreParam* poke, const pml::PokeParty * party, const EvolveSituation * situation, u32 * root_num ) const;

        /**
         * @brief アイテムを使用することによって進化できるかどうかを調べる
         * @param poke      チェック対象のポケモン
         * @param use_item  使用する道具
         * @retval 進化後のモンスターNo.
         * @note 進化できない場合は, 現在のモンスターNo.を返す
         */
        MonsNo GetEvolvedMonsNo_byItem( const CoreParam* poke, const EvolveSituation * situation, u32 use_item, u32 * root_num ) const;

        /**
         * @brief 通信交換で進化できるかどうかを調べる
         * @param poke       チェック対象のポケモン
         * @param pair_poke  交換相手のポケモン
         * @retval 進化後のモンスターNo.
         * @note 進化できない場合は, 現在のモンスターNo.を返す
         */
        MonsNo GetEvolvedMonsNo_byTrade(
            const CoreParam* poke, const CoreParam* pair_poke, u32 * root_num = NULL ) const;

        /**
         * @brief メガ進化後のフォルムNo.を取得する
         * @param poke  チェック対象のポケモン
         * @return メガ進化後のフォルムNo.
         * @note メガ進化できない場合は, 現在のフォルムNo.を返す
         */
        FormNo GetMegaEvolvedFormNo( const CoreParam* poke ) const;

        /**
         * @brief 進化する余地があるかどうかのチェック
         * @param[in] poke  チェック対象のポケモン
         * @return    bool  trueのとき、進化ルートを持っている
         */
        bool HaveEvolutionRoot( const CoreParam* poke ) const;


        /**
         * @brief   メガ進化対応アイテムかどうかの判定
         * @param[in] monsno  対象となるポケモンのナンバー
         * @param[in] itemno  どうぐID
         * @return  bool  trueのとき、メガ進化条件にあてはまるアイテム
         */
        bool IsMegaEvolveItem( MonsNo monsno, u16 itemno ) const;

      protected:
        //---------------------------------------------------------------------
        // ゲーム状況のチェック
        //---------------------------------------------------------------------
        // このクラスを継承し, 以下のメンバ関数をオーバーライドしてください.
        // 派生クラスのインスタンスを pml::System に渡してください.  

        ///////////// ここから ////////////////////////////////////////////////


        /**
         * @brief 指定された道具が「かわらずのいし」かどうかを調べる
         * @param item  チェック対象の道具
         * @retval true   item は「かわらずのいし」
         * @retval false  item は「かわらずのいし」でない
         */
        virtual bool CheckItem_KAWARAZUNOISHI( u32 item ) const { return false; }


        ///////////// ここまでをオーバーライドしてください ////////////////////
        





      private:

        struct EvolveData {
          personal::EvolveCond cond;  // 進化条件
          u16 cond_value;             // 進化条件についての満たすべき値
          MonsNo next_monsno;         // 進化後のモンスターNo.
          u8 enable_level;            // 進化可能なレベル
        };


        bool IsSatisfyEvolveCondition_LEVELUP( 
            const pml::pokepara::CoreParam* poke, 
            const pml::PokeParty * party,
            const EvolveSituation * situation,
            const EvolveData& evolve_data ) const;

        bool IsSatisfyEvolveCondition_ITEM( 
            const pml::pokepara::CoreParam* poke,
            const EvolveSituation * situation,
            u32 use_item, 
            const EvolveData& evolve_data ) const;

        bool IsSatisfyEvolveCondition_TRADE( 
            const pml::pokepara::CoreParam* poke, 
            const pml::pokepara::CoreParam* pair_poke,
            const EvolveData& evolve_data ) const;
        
        bool IsSatisfyEvolveCondition_MEGA( 
            u16 itemno,  const CoreParam* poke ,
            pml::personal::MegaEvolveCond cond, u16 param ) const;

    };



  }  // namespace pokepara
}  // namespace pml


#endif  // __PML_EVOLVESYSTEM_H__
