/* 
 * File:   gfl_Macro.h
 * Author: s-kimura
 *
 * Created on 2010/09/18, 15:05
 */

#ifndef GFL_MACRO_H
#define	GFL_MACRO_H
#pragma once

// =============================================================================
// マクロ
// =============================================================================
/**
 *   main 関数を定義
 * @param  無し
 * @remark windows の場合は main 関数が変なので定義をかますことで切り替えられるようにした。
 * ゲームプラットフォームの場合は int main() に、
 * その他の場合は int main(int argc, char *argv[]) に展開される
 */
#ifdef GFL_PLATFORM_WINDOWS // === windows
#include <tchar.h>
#ifdef GFL_PLATFORM_WINDOWS_CONSOLE
#define GFL_MAIN() int _tmain(int argc, _TCHAR* argv[])
#else
#define GFL_MAIN() int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#endif // GFL_PLATFORM_WINDOWS_CONSOLE
#elif defined(GFL_PLATFORM_XENON) // === xenon
#include <tchar.h>
#define GFL_MAIN() void __cdecl main()
#elif defined(GFL_PLATFORM_3DS) // == 3ds
#define GFL_MAIN() void nnMain(void)
#elif defined(GFL_PLATFORM_GAME_CONSOLE) // == game console
#define GFL_MAIN() int main() // avoid warning
#else // === other
#define GFL_MAIN() int main(int argc, char *argv[])
#endif

/**
 *   main 関数の返り値
 * @param  val  返り値
 * @return 返り値
 * @remark Ｘｅｎｏｎの main 関数は返り値を持たないのでこんなになりました。
 */
#if defined(GFL_PLATFORM_XENON) || defined(GFL_PLATFORM_3DS)
#define GFL_MAIN_RETURN(val)
#elif defined(_CRTDBG_MAP_ALLOC)
#define GFL_MAIN_RETURN(val) _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF|_CRTDBG_LEAK_CHECK_DF); return (val)
#else
#define GFL_MAIN_RETURN(val) return (val)
#endif

/**
 *   ワーニングを回避
 * @param  val  使用しない変数
 * @remark 使用しない変数があるというワーニングを回避させるため
 */
#define GFL_AVOID_WARNING(val) static_cast<void>(val)


/**
 *   リリース時のワーニングを回避
 * @param  val  使用しない変数
 * @remark デバッグ時には使用しているが、リリース時には使用しない変数があるというワーニングを回避させるため
 */
#ifdef GFL_DEBUG
#define GFL_AVOID_RELEASE_WARNING(val)
#else
#define GFL_AVOID_RELEASE_WARNING GFL_AVOID_WARNING
#endif

/**
 *   メイン関数の引数のワーニングを回避
 * @param  無し
 */
#ifdef GFL_PLATFORM_GAME_CONSOLE
#define GFL_NO_QT
#define GFL_AVOID_ARG_WARNING()
#elif defined(GFL_PLATFORM_WINDOWS_WINDOWS)
#define GFL_AVOID_ARG_WARNING() \
UNREFERENCED_PARAMETER( hPrevInstance );\
UNREFERENCED_PARAMETER( lpCmdLine )
#else
#define GFL_AVOID_ARG_WARNING() \
	static_cast<void>(argc);	\
	static_cast<void>(argv)
#endif

/**
 *   コンストアトリビュート設定
 * @param  無し
 */
#if defined(GFL_COMPILER_GCC) || defined(GFL_COMPILER_CW)
#define GFL_ATTR_CONST	__attribute__((const))
#elif defined(GFL_COMPILER_VC)
#define GFL_ATTR_CONST
#else
#define GFL_ATTR_CONST
#endif

/**
 *   アラインメントを指定した変数の宣言
 * @param  val 宣言したい変数
 * @param  aln アラインメントバイト数
 * @remark 使用例　￥u16 GFL_ATTR_ALIGN(u16_val,8);
 * この例では u16_val という変数を８バイトアラインメントで宣言
 * オート変数に対しては無効
 */
#if defined(GFL_COMPILER_GCC) || defined(GFL_COMPILER_CW) || defined(GFL_COMPILER_ARMCC)
#define GFL_ALIGN(aln)	__attribute__((aligned(aln)))
#define GFL_ATTR_ALIGN(val,aln)	val __attribute__((aligned(aln)))
#elif defined(GFL_COMPILER_VC)
#define GFL_ALIGN(aln)	__declspec(align(aln))
#define GFL_ATTR_ALIGN(val,aln)	__declspec(align(aln)) val
#else
#define GFL_ALIGN(aln) val
#define GFL_ATTR_ALIGN(val,aln) val
#endif

/**
 *   val を type でアクセスする
 * @param  type 型
 * @param  val 変数のポインター
 * @remark 例えば１バイトのデータ４つにS32でアクセスしたい場合に使う。
 * エンディアンが違うと挙動が変わるので注意が必要。
 */
#define GFL_ACCESS_TYPE(type,val) (*(reinterpret_cast<type*>(val)))

/**
 *   ビット制御
 * @param  val 立てたいビット
 */
#define GFL_BIT(val) (1<<(val))

/**
 *  クラスのメンバー変数のオフセットを計算
 * @param  name クラス名
 * @param  para メンバー変数名
 * @remark 本当は０でもいいけどワーニングが出るので０ｘ１００
 */
#define GFL_OFFSET_OF(name,para)  (reinterpret_cast<size_t>(&((name*)0x100)->para) - 0x100)

/**
 *  クラスのメンバー変数のオフセットのポインタを取得
 * @param  ptr クラスポインタ
 * @param  type 取得したい型
 * @param  ofs オフレット値　事前に GFL_OFFSET_OF で取得しておく
 */
#define GFL_GET_POINTER_BY_OFFSET(ptr,type,ofs) ((type*)((size_t)ptr + ofs))

/**
 *  メートルを単位として設定 Meter
 * @param x メートル
 * @remark ゲーム内の単位系がメートルなのかセンチなのかを意識させないようにマクロをかませる
 */
#if UNIT_METER
#define METER(x) (x)
#else
#define METER(x) ((x)*100.0f)
#endif

/**
 *  センチメートルを単位として設定 UnitMeter
 * @param x センチメートル
 */
#if UNIT_METER
#define CENTI(x) (x*0.01f)
#else
#define CENTI(x) (x)
#endif

#define GFL_SECOND(x) (GFL_FPS*x)

/** 深いインデントを回避するため */
#define GFL_NAMESPACE_BEGIN(ns) namespace ns {
#define GFL_NAMESPACE_END(ns) }

#endif	/* GFL_MACRO_H */

