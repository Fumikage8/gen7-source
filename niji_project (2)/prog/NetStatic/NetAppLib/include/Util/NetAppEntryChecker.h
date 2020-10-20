// ============================================================================
/*
 * @file NetAppEntryChecker.h
 * @brief 参加チェック系のユーティリティ
 * @date 2016.02.08
 * @author endo_akira
 */
// ============================================================================
#if !defined( NETAPP_ENTRY_CHECKER_H_INCLUDE )
#define NETAPP_ENTRY_CHECKER_H_INCLUDE
#pragma once

#include <macro/include/gfl2_macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(NetAppLib)
GFL_NAMESPACE_BEGIN(Util)


class NetAppEntryChecker
{
  GFL_FORBID_COPY_AND_ASSIGN(NetAppEntryChecker);

public:

  NetAppEntryChecker();
  virtual~NetAppEntryChecker();

  //! @brief  手持ちとボックスにポケモンが２匹以上いるかいないか
  //! @retval true : ２匹以上いる
  //! @retval false : ２匹以上いない
  static bool CheckPokemonCountForTrade( gfl2::heap::HeapBase* pHeap );

  //! @brief  手持ちとボックスにポケモンが２匹以上いるかいないか
  //! @retval true : ２匹以上いる
  //! @retval false : ２匹以上いない
  static bool CheckPokemonCountForDoubleBattle( gfl2::heap::HeapBase* pHeap );

  //! @brief  交換ペナルティ状態か(通信交換切断ペナルティ状態か)
  //! @retval true : ペナルティを受けてない
  //! @retval false : ペナルティを受けてる
  static bool CheckTradePenalty();

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief GTSが起動出来るかチェック
   *        
   *        GTSにあずけていて、手持ち・BOXがFULLだと起動できない        
   *        
   * @retval true:起動出来る
   * @retval false:起動出来ない
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool CheckFreeSpaceForGtsPlay( gfl2::heap::HeapBase* pHeap );

  /* ----------------------------------------------------------------------------------------- */
  /**
   * @brief GTSが起動出来るかチェック
   *        
   *        GTSにあずけていて、手持ち・BOXに１匹いれば起動できる
   *        
   * @retval true:起動出来る
   * @retval false:起動出来ない
   */
  /* ----------------------------------------------------------------------------------------- */
  static bool CheckPokemonCountForGtsPlay( gfl2::heap::HeapBase* pHeap );

};


GFL_NAMESPACE_END(Util)
GFL_NAMESPACE_END(NetAppLib)

#endif // NETAPP_ENTRY_CHECKER_H_INCLUDE
