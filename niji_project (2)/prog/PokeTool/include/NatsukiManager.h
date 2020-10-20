//=============================================================================
/**
 * @brief  ポケモンのなつき度更新管理
 * @file   NatsukiManager.h
 * @author tamada
 * @date   2012.06.09
 */
//=============================================================================
#ifndef __NATSUKIMANAGER_H__
#define __NATSUKIMANAGER_H__

#pragma once

#include <pml/include/pmlib.h>
#include <pml/include/item/item.h>
#include <pml/include/item/item_manager.h>

///
namespace PokeTool {


    //------------------------------------------------------------------------
    /**
     * @brief なつき度更新のタイプ指定
     */
    //------------------------------------------------------------------------
    enum NatsukiType{
      NATSUKI_TYPE_LEVELUP = 0,       ///<レベルアップ
      NATSUKI_TYPE_USE_ITEM,          ///<アイテム使用（クラス外で直接指定することはない）
      NATSUKI_TYPE_BOSS_BATTLE,       ///<ボス戦闘
      NATSUKI_TYPE_TSUREARUKI,        ///<連れ歩き（クラス外で直接指定することはない）
      NATSUKI_TYPE_HINSHI,            ///<瀕死
      NATSUKI_TYPE_LEVEL30_HINSHI,    ///<レベル差30以上の瀕死
      NATSUKI_TYPE_RESORT_HOTSPA,     ///<ぽかぽかリゾート
    };

    //------------------------------------------------------------------------
    /**
     * @brief なつき度更新マネジャー
     */
    //------------------------------------------------------------------------
    class NatsukiManager 
    { 
      GFL_FORBID_COPY_AND_ASSIGN(NatsukiManager); //コピーコンストラクタ＋代入禁止


      public:

        //---------------------------------------------------------------------
        // コンストラクタ・デストラクタ
        //---------------------------------------------------------------------
        /**
         * @brief コンストラクタ
         */
        NatsukiManager( void );

        /**
         * @brief デストラクタ
         */
        ~NatsukiManager();


        //---------------------------------------------------------------------
        // なつき度更新処理
        //---------------------------------------------------------------------
        static void Calc( pml::pokepara::CoreParam * pp, NatsukiType type, u16 place_id );
        static void CalcWhenWalk( pml::pokepara::CoreParam * pp, u16 place_id );
        static void CalcWhenUseItem( pml::pokepara::CoreParam * pp, u16 place_id, const item::ITEM_DATA * item_data );
        static void CalcWhenUseItem( pml::pokepara::CoreParam * pp, u16 place_id, u16 itemno, itemman::ITEM_MANAGER * itemMan );
        static void CalcWhenUseItem( pml::pokepara::CoreParam * pp, u16 place_id, u16 itemno, gfl2::heap::HeapBase * heap );
        static void CalcWhenResortHotSpa( pml::pokepara::CoreParam * pp, u16 place_id, s8 val );

      private:
        static void CalcCore( pml::pokepara::CoreParam * pp, u16 place_id, const s8 * table );
        static bool IsNatsukiUpItem( u32 itemno );

    };



}  // namespace PokeTool


#endif  // __PML_NATSUKIMANAGER_H__
