#if !defined(EVOLUTIONPROCPARAM_H_INCLUDED)
#define EVOLUTIONPROCPARAM_H_INCLUDED
#pragma once
//==============================================================================
/**
 * @file   EvolutionProcParam.h
 * @date   2015/08/28 Fri. 19:31:13
 * @author muto_yu
 * @brief  
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//==============================================================================


// =============================================================================
/**/
//==============================================================================
#include  <types/include/gfl2_Typedef.h>
#include  <macro/include/gfl2_Macros.h>
#include  <pml/include/pokepara/pml_PokemonParam.h>
#include  <pml/include/pokepara/pml_PokemonParamType.h>
#include  "niji_conv_header/poke_lib/wazano_def.h"

/*  poketool  */
#include  "PokeTool/include/PokeToolPokeGet.h"

/*  save  */
#include  "Savedata/include/MyItemSave.h"


namespace NetApp    {
namespace Evolution {
// =============================================================================
/**/
//==============================================================================
/*
*/
enum  BootMode
{
  BOOTMODE_None,
  BOOTMODE_Evolve,      /**<  進化から    */
  BOOTMODE_CheckWaza,   /**<  技から      */

  BOOTMODE_
};


/*
  proc呼び出し結果
*/
enum  APP_RESULT
{
  APPRESULT_Initial,          /**<  初期値          */
  APPRESULT_Cancel,           /**<  キャンセル終了  */
  APPRESULT_CallZukan,        /**<  図鑑登録要求    */
  APPRESULT_CallWazaWasure,   /**<  技忘れ要求      */
  APPRESULT_Exit,             /**<  終了            */

  APPRESULT_
};



/**  進化の要因  */
enum  Reason
{
  REASON_LevelUp,   /**<  レベルアップによる  */
  REASON_Trade,     /**<  通信交換            */
  REASON_Item,      /**<  アイテム使用        */

  REASON_
};


//==============================================================================
/**
  *  @class  APP_PARAM
  *  @brief  進化Proc引数及び結果
  *  @note   関数の実装はNetStatic側
  *  @date   2015/09/24 Thu. 14:34:56
  */
//==============================================================================
typedef struct APP_PARAM
{
  BootMode                        m_BootMode;       /**<  [in]      どこから開始するか                  */

  /*  result  */
  APP_RESULT                      m_Result;         /**<  [out] */

  /**  進化の要因  */
  Reason                          m_Reason;         /**<  [in]  */

  /*  進化関連  */
  pml::pokepara::PokemonParam*    m_pTargetPokemon; /**<  [in/out]    対象となるポケモンのパラメータ      */
  MonsNo                          m_MonsNoEvolve;   /**<  [in]        進化先MonsNo                        */
  u32                             m_EvolveRootNum;  /**<  [in]        進化Root                            */

  /*  所持品関連  */
  pml::PokeParty*                 m_pPokeParty;                 /**<  [in/out]  現在の手持ちポケモン（ヌケニン発生確認用、また発生時には追加される）            */
  Savedata::MyItem*               m_pMyItem;                    /**<  [in/out]  手持ちのアイテム（ヌケニン発生確認用、また発生時には条件アイテムが消費される）  */
  u32                             m_PokePartyReserveNum;        /**<  [in]  手持ちの空きが外部で予約されている数（ヌケニン発生確認用、手持ちの空きがこの数より多ければ合格）  */

  /*  技関連  */
  u32                                       m_PersonalWazaCheckCount;     /**<  (internal)  覚える技のうち、進化習得技であるかを確認した数  */
  WazaNo                                    m_OldWaza;                    /**<  (internal)  覚えるために忘れる技                            */
  u32                                       m_OldWazaIndex;               /**<  (internal)  覚えるために忘れる技index                       */
  WazaNo                                    m_NewWaza;                    /**<  (internal)  新しく覚える（た）技                            */
  u32                                       m_FindWazaIndex;              /**<  (internal)  覚える技の検出を継続するためのIndex             */
  pml::pokepara::CoreParam::WazaLearnWork   m_WazaWork;                   /**<  (internal)  習得チェック済みの技をフィルタするためのワーク  */
  bool                                      m_IsWazaWasureSequence;       /**<  (internal)  技忘れシーケンス中である                        */

  /*  ctor  */
  APP_PARAM(void)
    : m_BootMode(BOOTMODE_None)
  /*  result  */
    , m_Result(APPRESULT_Initial)
  /*  進化の要因  */
    , m_Reason(REASON_LevelUp)
  /*  進化関連  */
    , m_pTargetPokemon(NULL)
    , m_MonsNoEvolve(MONSNO_HUSIGISOU)
    , m_EvolveRootNum(0)
  /*  所持品関連  */
    , m_pPokeParty(NULL)
    , m_pMyItem(NULL)
    , m_PokePartyReserveNum(0)
  /*  技関連  */
    , m_PersonalWazaCheckCount(0)
    , m_OldWaza(WAZANO_NULL)
    , m_OldWazaIndex(0)
    , m_NewWaza(WAZANO_NULL)
    , m_FindWazaIndex(0)
    , m_WazaWork()
    , m_IsWazaWasureSequence(false)
  {
  }

  /*  funcs  */
  void  Setup_FromZukan(void);
  void  Setup_FromWazaWasure(const u32 oldWazaIndex);
  void  Setup_FromWazaWasureCancel(void);

  bool  IsReturnFromOtherApp(void) const      {return (m_BootMode == BOOTMODE_CheckWaza);}                            /**<  他アプリからの戻りであるか  */

  /*  */
  void  SetWazaWasureSequence(const bool isWazaWasureSequence)  {m_IsWazaWasureSequence = isWazaWasureSequence;}
  bool  IsWazaWasureSequence(void) const                        {return m_IsWazaWasureSequence;}                      /**<  技忘れシーケンス中であるか（新規に覚える技の候補があり、技の空きがなく、且つ「忘れる」が選択されている状態）  */

  void  DoEvolve(gfl2::heap::HeapBase* pTempHeapBaseForNukenin);
  bool  IsEvolutionCancelable(void) const {return (m_Reason == REASON_LevelUp);}
  PokeTool::PokeGetUtil::CallerType::Tag GetZukanCallerType(void) const  {
    return ((m_Reason == REASON_Trade) ? PokeTool::PokeGetUtil::CallerType::SHINKA_NET : PokeTool::PokeGetUtil::CallerType::SHINKA);
  }

  enum  WazaResult
  {
    WAZARESULT_None,      /**<  終了        */
    WAZARESULT_Learned,   /**<  覚えた      */
    WAZARESULT_Full,      /**<  空きがない  */
    
    WAZARESULT_
  };
  WazaResult  NextWaza(void);
  void  ForgetAndLearnWaza(void);
  bool  IsForgotWaza(void) const;
  bool  IsForgetCancel(void) const;
  bool  IsAbandonedForget(void) const;
  void  ResetWaza(void) {m_NewWaza = m_OldWaza = WAZANO_NULL; m_OldWazaIndex = 0; SetWazaWasureSequence(false);}      /**<  技覚えシーケンス開始のため状態をリセット    */

  static bool IsAlolaSpecies(const pml::pokepara::CoreParam& rCoreParam);

}APP_PARAM;




} /*  namespace Evolution */
} /*  namespace NetApp    */
#endif  /*  #if !defined(EVOLUTIONPROCPARAM_H_INCLUDED)  */

