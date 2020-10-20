#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANPROCPARAM_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANPROCPARAM_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanProcParam.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑のProcに渡す引数
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>

// poke_libのインクルード
#include <pml/include/pmlib.h>


// 前方宣言
GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(event)
class ZukanRegisterEventParam;
GFL_NAMESPACE_END(event)
GFL_NAMESPACE_END(app)




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)


//==============================================================================
/**
 @class     ZukanProcParam
 @brief     図鑑のProcに渡す引数
 */
//==============================================================================
class ZukanProcParam
{
public:
  //! @brief  呼び出し者
  enum Caller
  {
    CALLER_ZUKAN_EVENT,                             //!< 図鑑イベントが呼び出した
    CALLER_ZUKAN_REGISTER_EVENT,                    //!< 図鑑登録イベントが呼び出した
    CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST,    //!< 図鑑登録イベントが呼び出した(ポケモンリストの後に)
    CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS,  //!< 図鑑登録イベントが呼び出した(ポケモンステータスの後に)
  };
  //! @brief  戻り値タイプ
  enum ReturnType 
  {
    RETURN_TYPE_END,                //!< 終了
    RETURN_TYPE_NEXT_POKE_LIST,     //!< 次にポケモンリストに行って欲しい
    RETURN_TYPE_NEXT_POKE_STATUS,   //!< 次にポケモンステータスに行って欲しい
  };
  //! @brief  戻り値タイプがRETURN_TYPE_NEXT_POKE_STATUSのときに使用する値
  enum ReturnValNextPokeStatus
  {
    RETURN_VAL_NEXT_POKE_STATUS_NEW,    //!< 新規追加ポケモンのポケモンステータス
    RETURN_VAL_NEXT_POKE_STATUS_PARTY,  //!< 手持ちのポケモンステータス
  };

public:
  Caller                                caller;                   //!< [in]     呼び出し者
  app::event::ZukanRegisterEventParam*  zukanRegisterEventParam;  //!< [in,out] 図鑑登録の呼び出しEventに渡された引数
                                                                  //            callerが
                                                                  //            CALLER_ZUKAN_REGISTER_EVENT,
                                                                  //            CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LIST,
                                                                  //            CALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_STATUS
                                                                  //            のときのみ使用する
  u32                                   pokeListSelectPos;        //!< [in]     ポケモンリストの戻り値
                                                                  //            callerがCALLER_ZUKAN_REGISTER_EVENT_AFTER_POKE_LISTのときのみ使用する
  ReturnType                            returnType;               //!< [out]    戻り値タイプ
  s32                                   returnVal;                //!< [out]    戻り値タイプに応じた値
                                                                  //            returnTypeが
                                                                  //            RETURN_TYPE_NEXT_POKE_STATUS(ReturnValNextPokeStatus)
                                                                  //            のときに使用する

public:
  ZukanProcParam(void)
    : caller(CALLER_ZUKAN_EVENT),
      zukanRegisterEventParam(NULL),
      pokeListSelectPos(pml::PokeParty::MEMBER_INDEX_ERROR),
      returnType(RETURN_TYPE_END),
      returnVal(0)
  {}
};


GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANPROCPARAM_H_INCLUDED

