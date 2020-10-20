//これはマクロ展開用なのでインクルードガード不要です。
//#if !defined( __HEAP_DEFINE_H__ )
//#define __HEAP_DEFINE_H__
//======================================================================
/**
 * @file  heap_define.h
 * @brief HEAPID定義の元となる定義
 * @author  ariizumi
 * @data  10/10/20
 */
//======================================================================

//HEAP_DEFINEと言うマクロを読み込み先で用意しenum展開します。

//常駐ヒープ
HEAP_DEFINE( HEAPID_DEVICE )
HEAP_DEFINE( HEAPID_DEBUG )   //デバッグ起動時のみ確保
HEAP_DEFINE( HEAPID_DEBUG_DEVICE )   //デバッグ起動時のみ確保

//マクロを無効化する
#undef  HEAP_DEFINE

//これはマクロ展開用なのでインクルードガード不要です。
//#endif // __HEAP_DEFINE_H__
