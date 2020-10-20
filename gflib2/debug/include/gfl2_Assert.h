#ifndef __GFL_ASSERT_H__
#define __GFL_ASSERT_H__
//===========================================================================
/**
 * @file    gfl_assert.h
 * @brief   アサート処理
 * @author  GAMEFREAK inc.
 * @date    2010.09.09
 */
//===========================================================================

#pragma once
#include <stdio.h>  //NULL
#include <types/include/gfl2_Typedef.h>  // for b32

#if GFL_ENABLE_ASSERT

namespace gfl2 {
namespace debug {

  /* ASSERT情報出力をユーザ定義するための関数型定義 */
  typedef void (*GFL_ASSERT_DispInitFunc)(void);
  typedef void (*GFL_ASSERT_DispMainFunc)( const char* );
  typedef void (*GFL_ASSERT_DispFinishFunc)(void);

  /**
  * @brief   GFL_ASSERTのコールバック関数、停止処理とユーザー通知部分の処理を記述する
  * @note    この関数を抜けた場合、そのままプログラムが継続するようなっています、
  *          isStop==falseの際は何らか操作により処理が継続するようにしてください、
  *          isStop==trueの際はこの関数の内部で必ず停止するように処理を記述してください
  *
  * @param   pDispBuffer  表示する文章
  * @param   isStop       _STOP系ならtrue、_STOP以外(継続可能)ならfalse
  *
  * @return  無し
  */
  typedef void (*GFL_ASSERT_LoopFunc)(const char* buff, b32 isStop );

  extern void GFLassert( const char * filename, unsigned int line_no, b32 isStop, const char * exp );
  extern void GFLassertMsg( const char * filename, unsigned int line_no, b32 isStop, const char * fmt, ... );
  extern void GFLassertStop( const char * filename, unsigned int line_no, const char * exp );
  extern void GFLassertMsgStop( const char * filename, unsigned int line_no, const char * fmt, ... );
  extern void GFL_ASSERT_SetDisplayFunc( GFL_ASSERT_DispInitFunc initFunc, GFL_ASSERT_DispMainFunc mainFunc, GFL_ASSERT_DispFinishFunc finishFunc );
  extern void GFL_ASSERT_SetLoopFunc( GFL_ASSERT_LoopFunc loopFunc );

  extern void SetAssertEnable( bool isEnable );
  extern bool GetAssertEnable( void );
} /* debug */
} /* gfl */

//#define GFL_ASSERT(exp)  ( (exp)? ((void)0): GFLIB::dbg::assert( __FILE__, __LINE__, #exp ) )

//#define GFL_ASSERT_MSG(exp, ...) ( (exp)? ((void)0): GFLIB::dbg::assert( __FILE__, __LINE__, __VA_ARGS__ ) )

//#define GFL_PANIC(...) ( NN_LOG( __VA_ARGS__ ); nn::dbg::Panic() )


// 製品版では、ヒットしないアサート
// アサートをケアするコードをこの後に書く必要がある。
// 静的ソースコード解析ツールを動かす場合には、止める記述を書いてはいけない。製品版状態にしておくこと。

#ifdef __KLOCWORK__

#define GFL_ASSERT(exp) if(!(exp)){abort();}

#define GFL_ASSERT_MSG(exp, x) GFL_ASSERT(exp)
#define GFL_ASSERT_MSG(exp, x, y) GFL_ASSERT(exp)
#define GFL_ASSERT_MSG(exp, x, y, z) GFL_ASSERT(exp)
#define GFL_ASSERT_MSG(exp, x, y, z, xx) GFL_ASSERT(exp)
#define GFL_ASSERT_MSG(exp, x, y, z, xx, yy) GFL_ASSERT(exp)
#define GFL_ASSERT_MSG(exp, x, y, z, xx, yy, zz) GFL_ASSERT(exp)

//#define GFL_ASSERT_MSG(exp, ...) if(!(exp)){exit();}

#else

#define GFL_ASSERT(exp) ((exp)? ((void)0) : gfl2::debug::GFLassert( __FILE__, __LINE__, false, #exp ))
#define GFL_ASSERT_MSG(exp, ...) ( (exp)? ((void)0): gfl2::debug::GFLassertMsg( __FILE__, __LINE__, false, __VA_ARGS__ ) )

#endif


// 製品版でも、処理がストップするアサート
// 静的ソースコード解析ツールを動かす場合には、止める記述を書く必要がある
#ifdef	__KLOCWORK__

#define	GFL_ASSERT_STOP(exp)	if(!(exp)){abort();} //(exp)? ((void)0): abort();

#define	GFL_ASSERT_STOP_MSG(exp, x)	GFL_ASSERT_STOP(exp)
#define	GFL_ASSERT_STOP_MSG(exp, x, y)	GFL_ASSERT_STOP(exp)
#define	GFL_ASSERT_STOP_MSG(exp, x, y, z)	GFL_ASSERT_STOP(exp)
#define	GFL_ASSERT_STOP_MSG(exp, x, y, z, xx)	GFL_ASSERT_STOP(exp)
#define	GFL_ASSERT_STOP_MSG(exp, x, y, z, xx, yy)	GFL_ASSERT_STOP(exp)
#define	GFL_ASSERT_STOP_MSG(exp, x, y, z, xx, yy, zz)	GFL_ASSERT_STOP(exp)


//#define GFL_ASSERT_STOP_MSG(exp, ... ) if(!(exp)){exit();}

#else   //__KLOCWORK__

#define GFL_ASSERT_STOP(exp) ((exp)? ((void)0) : gfl2::debug::GFLassertStop( __FILE__, __LINE__, #exp ))
#define GFL_ASSERT_STOP_MSG(exp, ...) ( (exp)? ((void)0): gfl2::debug::GFLassertMsgStop( __FILE__, __LINE__, __VA_ARGS__ ) )

#endif  //__KLOCWORK__

#define GFL_PANIC  NN_PANIC

#else   // GFL_ENABLE_ASSERT


namespace gfl2 {
namespace debug {

  /**
  * @brief   GFL_ASSERTのコールバック関数、停止処理とユーザー通知部分の処理を記述する
  * @note    この関数を抜けた場合、そのままプログラムが継続するようなっています、
  *          isStop==falseの際は何らか操作により処理が継続するようにしてください、
  *          isStop==trueの際はこの関数の内部で必ず停止するように処理を記述してください
  *
  * @param   pDispBuffer  表示する文章
  * @param   isStop       _STOP系ならtrue、_STOP以外(継続可能)ならfalse
  *
  * @return  無し
  */
  typedef void (*GFL_ASSERT_LoopFunc)(const char* buff, b32 isStop );

  extern void GFLassert( const char * filename, unsigned int line_no, b32 isStop, const char * exp );
  extern void GFLassertMsg( const char * filename, unsigned int line_no, b32 isStop, const char * fmt, ... );
  extern void GFLassertStop( const char * filename, unsigned int line_no, const char * exp );
  extern void GFLassertMsgStop( const char * filename, unsigned int line_no, const char * fmt, ... );
  extern void GFL_ASSERT_SetLoopFunc( GFL_ASSERT_LoopFunc loopFunc );

} /* debug */
} /* gfl */
#if 0
#define GFL_ASSERT(exp)      ((void)0);
#define GFL_ASSERT_MSG(exp, ...) ((void)0);
#define GFL_ASSERT_STOP(exp)      if( !(exp) ){nn::dbg::Panic();}
#define GFL_ASSERT_STOP_MSG(exp, ...) if( !(exp) ){nn::dbg::Panic();}
#define GFL_PANIC(...)       nn::dbg::Panic()
#else
#define GFL_ASSERT(exp)      if( !(exp) ){ gfl2::debug::GFLassert(NULL, 0, 0, false); }
#define GFL_ASSERT_MSG(exp, ...) if( !(exp) ){ gfl2::debug::GFLassert(NULL, 0, 0, false); }
#define GFL_ASSERT_STOP(exp)      if( !(exp) ){ gfl2::debug::GFLassertStop(NULL, 0, 0); }
#define GFL_ASSERT_STOP_MSG(exp, ...) if( !(exp) ){ gfl2::debug::GFLassertStop(NULL, 0, 0); }
#define GFL_PANIC(...)       gfl2::debug::GFLassertStop(NULL, 0, 0); }
#endif


#endif  // GFL_ENABLE_ASSERT

// NW4CのASSERT
#define GFL_GL_ASSERT NW_GL_ASSERT

#endif // __GFL_ASSERT_H__
