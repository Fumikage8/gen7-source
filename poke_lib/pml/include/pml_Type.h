//==============================================================================
/**
 * @brief  基本的な型の宣言
 * @file   pml_Type.h
 * @author obata_toshihiro
 * @date   2010.12.21
 */
//==============================================================================
#if !defined( __PML_TYPE_H__ )
#define __PML_TYPE_H__
#pragma once

#include <stddef.h>
#include <types/include/gfl2_Typedef.h>
#include <pml/include/poketype_def.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include <niji_conv_header/poke_lib/monsnum_def.h>
#include <niji_conv_header/poke_lib/wazano_def.h>
#include <niji_conv_header/poke_lib/tokusei_def.h>
#include <niji_conv_header/poke_lib/item/itemsym.h>
#include <niji_conv_header/poke_lib/item/itemequip.h>
#include <niji_conv_header/poke_lib/item/itemtype_def.h>
#include <niji_conv_header/poke_lib/item/itempocket_def.h>



namespace pml {

  typedef wchar_t STRCODE;  ///< 文字
  typedef u8 PokeType;      ///< ポケモンのタイプ
  typedef u8 FormNo;        ///< フォルムNo.
  typedef u16 ItemNo;       ///< 道具No.

}




/**
 * 名前空間移行のための一時的な定義
 * @attention 本来はpml::FormNoに統一するべき
 */
typedef u8 FormNo;        // フォルムNo.


#endif  // __PML_TYPE_H__

