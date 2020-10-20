//=============================================================================
/**
 * @file    btl_ACTION_ORDER_WORK.cpp
 * @brief   アクション優先順記録構造体
 * @author  obata_toshihiro
 * @date    2015.05.28
 */
//=============================================================================
#include <util/include/gfl2_std_string.h>

#include "./btl_ACTION_ORDER_WORK.h"
#include "./btl_InsertWazaInfoStorage.h"

namespace btl {

  

  //---------------------------------------------------------------------------
  /**
   * @brief アクションの詳細情報をクリアする
   */
  //---------------------------------------------------------------------------
  void ActionDesc_Clear( ActionDesc* desc )
  {
    gfl2::std::MemClear( desc, sizeof(ActionDesc) );
    desc->insertInfoID = InsertWazaInfoStorage::ID_NULL;
  }

  //---------------------------------------------------------------------------
  /**
   * @brief 使用する技の技No.を取得する
   * @retval WAZANO_NULL  技を使用しない場合
   */
  //---------------------------------------------------------------------------
  WazaID ActOrder_GetWazaID( const ACTION_ORDER_WORK* wk )
  {
    if( wk->action.gen.cmd == BTL_ACTION_FIGHT )
    {
      return BTL_ACTION_GetWazaID( &(wk->action), wk->bpp );
    }
    return WAZANO_NULL;
  }



} // namespace btl