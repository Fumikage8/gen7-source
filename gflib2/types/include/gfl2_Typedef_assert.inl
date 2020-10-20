#pragma once
#ifndef GFL2_TYPEDEF_ASSERT_INL
#define GFL2_TYPEDEF_ASSERT_INL
//===========================================================================
/**
 * @file    gfl2_Typedef_assert.inl
 * @brief   gfl2_Typdef.hの定義検証用アサート処理
 * @author  tamada@gamefreak.co.jp
 * @date    2016.01.26
 */
//===========================================================================


//---------------------------------------------------------------------------------------
/**
 * @note gfl2_Typedef.hの定義が意図通りか確認するための関数
 * STATIC_ASSERTをヘッダに記述したくない一方で
 * STATIC_ASSERTが関数外にかけないのでダミーの関数を定義
 */
//---------------------------------------------------------------------------------------
static void StaticAssertGfl2TypeDef( void )
{
  STATIC_ASSERT(sizeof(s8)    == 1 );
  STATIC_ASSERT(sizeof(s16)   == 2 );
  STATIC_ASSERT(sizeof(s32)   == 4 );
  STATIC_ASSERT(sizeof(s64)   == 8 );

  STATIC_ASSERT(sizeof(u8)    == 1 );
  STATIC_ASSERT(sizeof(u16)   == 2 );
  STATIC_ASSERT(sizeof(u32)   == 4 );
  STATIC_ASSERT(sizeof(u64)   == 8 );

//  STATIC_ASSERT(sizeof(f16)   == 2); // niji 未使用
  STATIC_ASSERT(sizeof(f32)   == 4 );
  STATIC_ASSERT(sizeof(f64)   == 8 );

  STATIC_ASSERT(sizeof(uptr)  == sizeof(void*) );

  STATIC_ASSERT(sizeof(b8)    == 1);
  STATIC_ASSERT(sizeof(b32)   == 4);

  STATIC_ASSERT(sizeof(bit8)  == 1);
  STATIC_ASSERT(sizeof(bit32) == 4);
  STATIC_ASSERT(sizeof(bit64) == 8);

  STATIC_ASSERT(sizeof(c8)    == 1);
  STATIC_ASSERT(sizeof(c16)   == 2);
}



#endif //GFL2_TYPEDEF_ASSERT_INL
