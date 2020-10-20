#ifndef __GFL2_MACROS_H__
#define __GFL2_MACROS_H__
//===========================================================================
/**
 * @file    gfl_Macros.h
 * @brief   GAMEFREAK Library / Utility Macros
 * @author  GAMEFREAK inc.
 * @date    2010.10.29
 */
//===========================================================================
#pragma once

//-----------------------------------------------------------------------------
/**
 *          マクロ定義
*/
//-----------------------------------------------------------------------------

/// 使用しない引き数に対して使うマクロ（#pragma unusedの代わり）
#define GFL_UNUSED(a) static_cast<void>(a)

/*
 *  warning C4800:
 *  'u8' : ブール値を 'true' または 'false' に強制的に設定します (警告の処理)
 *  を回避するためのマクロ
 *
 *  bool型へのキャストを明示的に行うのに使う
 */
#define GFL_BOOL_CAST(a) (static_cast<bool>((a)!=0))

///コピーコンストラクタ＋代入演算子を禁止する
#define GFL_FORBID_COPY_AND_ASSIGN(TypeName)  \
  private: \
    TypeName( const TypeName& ); \
    void operator= (const TypeName&)

/// 配列の要素数
#define GFL_NELEMS(ary) (sizeof(ary)/sizeof(ary[0]))
#define elementof(ary) (sizeof(ary)/sizeof(ary[0]))

/// enumのサイズを DWORD （４バイト）に統一する
#define  ENUM_FORCE_DWORD(name) name##_FORCE_DWORD = 0x7FFFFFFF
// enumのサイズ固定用
#define ENUM_FORCE_U32  _enum_force_u32 = 0xFFFFFFFF


/// スタティックアサート
#define STATIC_ASSERT(expr) { extern char dummy[(expr) ? 1 : -1]; }


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
#if GFL_DEBUG
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
 * @remark 使用例　¥u16 GFL_ATTR_ALIGN(u16_val,8);
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

/*
 * 警告抑止用マクロ
 * 利用はリーダーの承認＆許可制
 */
#ifdef GF_PLATFORM_WIN32
  #define GFL_PRAGMA(X) __pragma(X)
  #define GFL_WARNING_PUSH GFL_PRAGMA(warning(push))
  #define GFL_WARNING_POP GFL_PRAGMA(warning(pop))
#elif GF_PLATFORM_CTR
  #define GFL_PRAGMA(X) _Pragma(#X)
  #define GFL_WARNING_PUSH GFL_PRAGMA(push)
  #define GFL_WARNING_POP GFL_PRAGMA(pop)
#endif


#ifdef GF_PLATFORM_WIN32
  #define GFL_PRAGMA_WIN32(a) GFL_PRAGMA(a)
  #define GFL_WARNING_WIN32_PUSH GFL_PRAGMA(warning(push))
  #define GFL_WARNING_WIN32_POP GFL_PRAGMA(warning(pop))
  #define GFL_WARNING_WIN32_IGNORE(a) GFL_PRAGMA(warning(disable:a))
  #define GFL_WARNING_WIN32_DEFAULT(a) GFL_PRAGMA(warning(default:a))
#else
  //win32以外では全てelseを意図
  #define GFL_PRAGMA_WIN32(a)
  #define GFL_WARNING_WIN32_PUSH
  #define GFL_WARNING_WIN32_POP
  #define GFL_WARNING_WIN32_IGNORE(a)
  #define GFL_WARNING_WIN32_DEFAULT(a)
#endif

#ifdef GF_PLATFORM_CTR
  #define GFL_PRAGMA_CTR(a) GFL_PRAGMA(X)
  #define GFL_PRAGMA_CTR_STR(a) _Pragma(a)
  #define GFL_WARNING_CTR_PUSH GFL_PRAGMA(push)
  #define GFL_WARNING_CTR_POP GFL_PRAGMA(pop)
  #define GFL_WARNING_CTR_IGNORE(a)  GFL_PRAGMA(diag_suppress a)
  #define GFL_WARNING_CTR_DEFAULT(a) GFL_PRAGMA(diag_default a)
  #define GFL_WARNING_CTR_STR(a) GFL_PRAGMA_CTR_STR(a)
#else
  //ctr以外では全てelseを意図
  #define GFL_PRAGMA_CTR(a)
  #define GFL_PRAGMA_CTR_STR(a)
  #define GFL_WARNING_CTR_PUSH
  #define GFL_WARNING_CTR_POP
  #define GFL_WARNING_CTR_IGNORE(a)
  #define GFL_WARNING_CTR_DEFAULT(a)
  #define GFL_WARNING_CTR_STR(a)
#endif

/// for(;;)の無限ループを書きたいときに使う
#define ever  (;;)  //for(;;) -> for ever{ ... } のように使います

/*
 * do while(0) を書きたいときに使う
 * C4127, arm #236 条件式が定数の警告を回避
 * do{
 * }ONCE;
 */
#ifdef GF_PLATFORM_WIN32
#define ONCE \
  GFL_WARNING_PUSH \
  GFL_WARNING_WIN32_IGNORE(4127) \
  while(0) \
  GFL_WARNING_POP
#endif
#ifdef GF_PLATFORM_CTR
#define ONCE \
  GFL_WARNING_PUSH \
  GFL_WARNING_CTR_IGNORE(236) \
  while(0) \
  GFL_WARNING_POP
#endif


#endif // __GFL2_MACROS_H__
