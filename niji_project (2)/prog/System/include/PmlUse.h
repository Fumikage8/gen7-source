//======================================================================
/**
 *  GAME FREAK inc.
 *
 *  @file   pml_use.h
 *  @brief  PMLibを動作させるための初期化処理
 *  @date   2011.04.21
 *
 */
//======================================================================
#ifndef __PML_USE_H__
#define __PML_USE_H__

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_heap.h>

GFL_NAMESPACE_BEGIN(System)
GFL_NAMESPACE_BEGIN(PmlUse)

    class NijiEvolveManager;


    /**
     * @brief ローカルヒープを生成
     * @param heap  親ヒープ
     */
    extern void CreateLocalHeap( gfl2::heap::HeapBase* heap );

    /**
     * @brief ローカルヒープを削除
     */
    extern void DeleteLocalHeap( void );

    /**
     * @brief PML 初期化処理
     */
    extern void Initialize( void ); 

    /**
     * @brief PML 終了処理
     */
    extern void Finalize( void );

    /**
     * @brief 進化マネージャを取得する
     */
    extern const NijiEvolveManager* GetEvolveManager( void );



GFL_NAMESPACE_END(PmlUse)
GFL_NAMESPACE_END(System)



#endif  // __PML_USE_H__
