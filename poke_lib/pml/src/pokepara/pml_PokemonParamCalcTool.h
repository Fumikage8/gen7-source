//=======================================================================================
/**
 * @brief  ポケモンパラメータの計算
 * @file   pml_PokemonParamCalcTool.h
 * @author obata_toshihiro
 * @date   2010.12.27
 */
//=======================================================================================
#ifndef __PML_POKEMONPARAMCALCTOOL_H__
#define __PML_POKEMONPARAMCALCTOOL_H__
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

namespace pml {
  namespace pokepara { 


    class CalcTool 
    {
      GFL_FORBID_COPY_AND_ASSIGN( CalcTool ); 

      public:

      //---------------------------------------------------------------------------------
      // パラメータ
      //---------------------------------------------------------------------------------
      static u8 CalcLevel( MonsNo monsno, FormNo formno, u32 exp );                  // レベル
      static u16 CalcMaxHp( MonsNo monsno, u8 level, u16 base, u16 rnd, u16 exp );   // 最大HP
      static u16 CalcAtk( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku );   // こうげき
      static u16 CalcDef( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku );   // ぼうぎょ
      static u16 CalcSpAtk( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku ); // とくこう
      static u16 CalcSpDef( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku ); // とくぼう
      static u16 CalcAgi( u8 level, u16 base, u16 rnd, u16 exp, Seikaku seikaku );   // すばやさ

      //---------------------------------------------------------------------------------
      // 色違い
      //---------------------------------------------------------------------------------
      static bool IsRareColor( u32 id, u32 rnd );            // 色違いかどうかを調べる
      static u32 CorrectColorRndForNormal( u32 id, u32 rnd );    // 非色違いになるように調整する
      static u32 CorrectColorRndForRare( u32 id, u32 rnd );  // 色違いになるように調整する

      //---------------------------------------------------------------------------------
      // 性別
      //---------------------------------------------------------------------------------
      static Sex GetCorrectSexInPersonalData( MonsNo monsno, FormNo formno, Sex both_case );

      //---------------------------------------------------------------------------------
      // 特性
      //---------------------------------------------------------------------------------
      static TokuseiNo GetTokuseiNo( MonsNo monsno, FormNo formno, u8 tokusei_index );

      //---------------------------------------------------------------------------------
      // めざめるパワー
      //---------------------------------------------------------------------------------
      static PokeType CalcMezamerupawaaType( u8 hp, u8 atk, u8 def, u8 agi, u8 spatk, u8 spdef );
      static u32 CalcMezamerupawaaPower( u8 hp, u8 atk, u8 def, u8 agi, u8 spatk, u8 spdef ); 

      //---------------------------------------------------------------------------------
      // 味の好み
      //---------------------------------------------------------------------------------
      static TasteJudge JudgeTaste( Seikaku seikaku, Taste taste ); 

      //---------------------------------------------------------------------------------
      // ロトム専用
      //---------------------------------------------------------------------------------
      static WazaNo GetRotomuWazaNo( FormNo formno ); 

      //---------------------------------------------------------------------------------
      // アルセウス専用
      //---------------------------------------------------------------------------------
      static PokeType GetAruseusuType( u32 itemno ); 

      //---------------------------------------------------------------------------------
      // グリプス２専用
      //---------------------------------------------------------------------------------
      static PokeType GetGuripusu2Type( u32 itemno ); 

      //-----------------------------------------------------------------------------------
      //  所持どうぐからフォルムNoを取得
      //-----------------------------------------------------------------------------------
      static bool DecideFormNoFromHoldItem( MonsNo monsno, u32 hold_itemno, FormNo* formno );

      //-----------------------------------------------------------------------------------
      //  性格からパワー乱数変化値を取得
      //-----------------------------------------------------------------------------------
      static s8 GetPowerTransformBySeikaku( u16 seikaku, PowerID power_id );
      private:
      static u16 CalcCorrectedPowerBySeikaku( u16 value, u16 seikaku, PowerID power_id );
    };


  } // namespace pokepara
} // namespace pml 


#endif//__PML_POKEMONPARAMCALCTOOL_H__
