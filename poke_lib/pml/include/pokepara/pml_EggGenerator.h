//=============================================================================
/**
 * @brief  ポケモンのタマゴ生成
 * @file   pml_EggGenerator.h
 * @author obata
 * @date   2012.04.12
 */
//=============================================================================
#ifndef __PML_EGGGENERATOR_H__
#define __PML_EGGGENERATOR_H__
#pragma once

#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <str/include/gfl2_StrBuf.h>
#include <math/include/gfl2_math_random.h>

#include "niji_conv_header/poke_lib/monsno_def.h"

#include <niji_conv_header/poke_lib/wazano_def.h>


namespace pml {
  /**
   * @namespace pml::pokepara
   */
  namespace pokepara {

    class CoreParam;
    class PokemonParam;
    struct Parents;
    class EggParam;
    class EggWazaData;


    //---------------------------------------------------------------------
    /**
     * @class EggGenerator
     * @brief ポケモンのタマゴ生成クラス
     */
    //---------------------------------------------------------------------
    class EggGenerator
    {
      public:

        //---------------------------------------------------------------------
        /**
         * @brief 準備
         * @param heap 作業領域として使用するヒープ
         */
        //---------------------------------------------------------------------
        static void Initialize( gfl2::heap::HeapBase* heap );

        //---------------------------------------------------------------------
        /**
         * @brief 後片付け
         */
        //---------------------------------------------------------------------
        static void Finalize( void );


        //---------------------------------------------------------------------
        /**
         * @struct  Setting
         * @brief   生成設定データ
         */
        //---------------------------------------------------------------------
        struct Setting
        {
          u32 IDNo;
          const gfl2::str::StrBuf* parentName;
          bool haveItem_HIKARUOMAMORI; ///<「ひかるおもまり」を持っているか？
          //@attention 乱数をセットする方式に変更
          gfl2::math::Random* pRandom;  ///< 乱数
          u8  realRegionFormNo;        ///< リアル地域ポケモンのフォルム指定
        };

        //---------------------------------------------------------------------
        /**
         * @brief タマゴを作成する
         * @param heap      タマゴ生成に使用するヒープ
         * @param poke1     親ポケモン1
         * @param poke2     親ポケモン2
         * @param setting   設定
         */
        //---------------------------------------------------------------------
        static PokemonParam* CreateEgg( 
            gfl2::heap::HeapBase* heap,
            const CoreParam* poke1, 
            const CoreParam* poke2, 
            const Setting& setting );

        //---------------------------------------------------------------------
        /**
         * @brief タマゴを作成する
         * @param heap      タマゴ生成に使用するヒープ
         * @param poke1     親ポケモン1
         * @param poke2     親ポケモン2
         * @param setting   設定
         */
        //---------------------------------------------------------------------
        static CoreParam* CreateEgg_CoreParam( 
            gfl2::heap::HeapBase* heap,
            const CoreParam* poke1, 
            const CoreParam* poke2, 
            const Setting& setting );


      private:

        static void DecideParents( Parents*, const CoreParam*, const CoreParam* );
        static void CorrectParents_SEX( Parents* );
        static void CorrectParents_METAMON( Parents* );
        static void ReverseParents( Parents* );
        static const CoreParam* GetBasePokemon_forMonsNo( const Parents& );
        static const CoreParam* GetBasePokemon_forForm( const Parents& );
        static void DecideEggParams( gfl2::heap::HeapBase*, EggParam*, const Parents&, const Setting& );
        static void DecideEggParam_MonsNo_BASIC( EggParam*, const Parents& );
        static void DecideEggParam_MonsNo_NIDORAN( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_MonsNo_BARUBIITO( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_MonsNo_FIONE( EggParam*, const Parents& );
        static void DecideEggParam_MonsNo_BABY( EggParam*, const Parents& );
        static void DecideEggParam_Form_BASIC( EggParam*, const Parents& );
        static void DecideEggParam_Form_MOTHER( EggParam*, const Parents& );
        static void DecideEggParam_Form_REGION( EggParam*, const Parents&, const Setting& );
        static void DecideEggParam_Form_PERSONAL_REGION( gfl2::heap::HeapBase* , EggParam* , const Parents& );
        static void DecideEggParam_Sex( EggParam* egg, MonsNo monsno, FormNo formno, gfl2::math::Random* pRandom );
        static void DecideEggParam_Seikaku_BASIC( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_Seikaku_KAWARAZUNOISI( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_TokuseiIndex( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_BirthCount( EggParam*);
        static void DecideDerivedTalentPower_AKAIITO( EggParam*, const Parents& );
        static void DecideDerivedTalentPower_ITEM( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideDerivedTalentPower_RANDOM( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_TalentPower_RANDOM( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_TalentPower_DERIVE( EggParam*, const Parents& );
        static void DecideEggParam_PersonalRnd( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static void DecideEggParam_RareRnd( EggParam* egg, const Parents& parents, bool have_HIKARUOMAMORI, gfl2::math::Random* pRandom );
        static void DecideEggParam_Waza_DEFAULT( EggParam*);
        static void PushWaza_to_EggParam( EggParam*, WazaNo wazano );
        static void PushEggWaza_to_EggParam( EggParam* egg, const CoreParam* parent, const EggWazaData& eggWazaData );
        static void DecideEggParam_Waza_EGG( gfl2::heap::HeapBase*, EggParam*, const Parents& );
        static void DecideEggParam_Waza_PARENTS( EggParam*, const Parents& );
        static void DecideEggParam_Waza_PITYUU( EggParam*, const Parents& );
        static void DecideEggParam_Waza_MACHINE( EggParam*, const Parents& );
        static void DecideEggParam_BallID( EggParam* egg, const Parents& parents, gfl2::math::Random* pRandom );
        static bool IsWazaMachineAbleToUse( MonsNo monsno, FormNo formno, u16 itemno );
        static void SetupEggParam( gfl2::heap::HeapBase*, CoreParam*, const EggParam&, const Parents&, const Setting& );
        static bool IsCurrentRegionFormNo( gfl2::heap::HeapBase* heap, const CoreParam* coreParam );
    };

  } //namespace pokepara
} //namespace pml

#endif //__PML_EGGGENERATOR_H__
