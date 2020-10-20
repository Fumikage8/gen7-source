//=============================================================================
/**
 * @brief kujira用ポケモン進化マネージャ
 * @file XYEvolveManager.cpp
 * @author obata
 * @date 2011.11.21
 */
//=============================================================================
#include "niji_conv_header/poke_lib/item/itemsym.h"
#include "NijiEvolveManager.h"

GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(PmlUse)


  //---------------------------------------------------------------------------
  /**
   * @brief コンストラクタ
   */
  //---------------------------------------------------------------------------
  NijiEvolveManager::NijiEvolveManager( void ) :
    pml::pokepara::EvolveManager()
  {
  }

  //---------------------------------------------------------------------------
  /**
   * @brief デストラクタ
   */
  //---------------------------------------------------------------------------
  NijiEvolveManager::~NijiEvolveManager()
  {
  }






  //---------------------------------------------------------------------------
  /**
   * @brief 指定された道具が「かわらずのいし」かどうかを調べる
   * @param item  チェック対象の道具
   * @retval true   item は「かわらずのいし」
   * @retval false  item は「かわらずのいし」でない
   */
  //---------------------------------------------------------------------------
  bool NijiEvolveManager::CheckItem_KAWARAZUNOISHI( u32 item ) const 
  { 
    return ( item == ITEM_KAWARAZUNOISI );
  }

GFL_NAMESPACE_END(PmlUse)
GFL_NAMESPACE_END(System)
