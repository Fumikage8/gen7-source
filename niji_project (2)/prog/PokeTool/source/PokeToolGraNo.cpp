//=============================================================================
/**
 * @file  PokeToolGraNo.cpp
 * @author  obata_toshihiro
 * @date    2012.10.31
 */
//=============================================================================
#include <debug/include/gfl2_Assert.h>
#include <niji_conv_header/poke_lib/monsno_def.h>
#include "PokeTool/include/PokeToolGraNo.h"

GFL_NAMESPACE_BEGIN( PokeTool )

#include <niji_conv_header/poke_lib/monsno2grano.cdat>


//------------------------------------------------------------------------
/**
 * @brief モンスターNo. --> グラフィックNo.
 * @param[in] monsno  モンスターNo.
 * @return グラフィックナンバー( 開発No. )
 */
//------------------------------------------------------------------------
u16 MonsNo2GraNo( u16 monsno )
{
  GFL_ASSERT_STOP( monsno <= MONSNO_END );
  return POKEGRA_ARRAY[ monsno ];
}


GFL_NAMESPACE_END( PokeTool )
