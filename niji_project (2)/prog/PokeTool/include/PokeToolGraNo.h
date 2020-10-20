//=============================================================================
/**
 * @file  PokeToolGraNo.h
 * @author  obata_toshihiro
 * @date    2012.10.31
 */
//=============================================================================
#if !defined  __POKE_TOOL_GRANO_H__
#define  __POKE_TOOL_GRANO_H__

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN( PokeTool )



//------------------------------------------------------------------------
/**
 * @brief モンスターNo. --> グラフィックNo.
 * @param[in] monsno  モンスターNo.
 * @return グラフィックナンバー( 開発No. )
 */
//------------------------------------------------------------------------
extern u16 MonsNo2GraNo( u16 monsno );



GFL_NAMESPACE_END( PokeTool );

#endif  //  __POKE_TOOL_GRANO_H__
