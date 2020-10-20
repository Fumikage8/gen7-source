#ifndef __GFL2_SINGLETON_H__
#define __GFL2_SINGLETON_H__
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		gfl2_Singleton.h
 *	@brief  シングルトンクラス
 *	@author	Koji Kawada
 *	@date		2010.10.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

// CTR_SDK

// NW4C

// gflib
#include <debug/include/gfl2_Assert.h>
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>


namespace gfl2 {
namespace base {

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------
// シングルトンクラス
template <typename T> class Singleton
{
  GFL_FORBID_COPY_AND_ASSIGN(Singleton);

public:

  ///  Type of the singleton object
  typedef T ObjectType;

  //-----------------------------------------------------------------------------
  /*
   *  @brief         コンストラクタ
   */
  //-----------------------------------------------------------------------------
  inline Singleton(void)
  {
    GFL_ASSERT_STOP(!m_instance);
    m_instance = static_cast<T*>(this);
  }
  //-----------------------------------------------------------------------------
  /*
   *  @brief         デストラクタ
   */
  //-----------------------------------------------------------------------------
  inline ~Singleton()  // 継承注意
  {
    GFL_ASSERT_STOP(m_instance);
    m_instance = 0;
  }

  //-----------------------------------------------------------------------------
  /*
   *  @brief         インスタンスを取得
   *
   *  @retval        T*    シングルトンクラスのインスタンス
   */
  //-----------------------------------------------------------------------------
  static inline T* GetInstance(void)
  {
    return m_instance;
  }

private:
  static T* m_instance;  // インスタンス
};

//-----------------------------------------------------------------------------
/**
 *					クラスのstaticメンバ変数
*/
//-----------------------------------------------------------------------------
// シングルトンクラスのstaticメンバ変数
template <typename T> T* Singleton<T>::m_instance = 0;

}  // namespace base
}  // namespace gfl2




//-----------------------------------------------------------------------------
/**
 *					Singletonクラスアクセス専用クラス（DLL経由でのアクセスのため）
*/
//-----------------------------------------------------------------------------
namespace gfl2 {
namespace base {

#if GFL_RO_ENABLE
  //---------------------------------------------------------------------------
  // DLL有効時
  //---------------------------------------------------------------------------
  #ifndef SINGLETON_EXPORT
  #define SINGLETON_EXPORT
  #endif

  template <typename T> class SINGLETON_EXPORT SingletonAccessor
  {
    public:
      static T* GetInstance();
  };

  /**
  * @brief SINGLETON用プロトタイプ宣言
  * 下記マクロを、singleton継承クラスのヘッダファイル内でコールすること
  */
  #define SINGLETON_INSTANCE_DECLARATION(classname)                         \
      template<> classname::ObjectType*                                     \
        gfl2::base::SingletonAccessor<classname::ObjectType>::GetInstance();

  /**
  * @brief SINGLETON用実体化コード
  * 下記マクロを、singleton継承クラスのソースコード内でコールすること
  */
  #define SINGLETON_INSTANCE_DEFINITION(classname)                         \
      template<> classname::ObjectType*                                    \
        gfl2::base::SingletonAccessor<classname::ObjectType>::GetInstance() \
      {                                                                    \
          return classname::GetInstance();                                 \
      }                                                                    
#else



  //---------------------------------------------------------------------------
  // DLL無効時
  //---------------------------------------------------------------------------
  template <typename T> class SingletonAccessor{};

  #define SINGLETON_INSTANCE_DECLARATION(classname)
  //
  #define SINGLETON_INSTANCE_DEFINITION(classname)

#endif

}  // namespace base
}  // namespace gfl2

//-----------------------------------------------------------------------------
/**
 *					マクロ定義
*/
//-----------------------------------------------------------------------------
// シングルトンクラスのインスタンスを取得
#if GFL_RO_ENABLE
  #define GFL_SINGLETON_INSTANCE(classname) (gfl2::base::SingletonAccessor<classname>::GetInstance())
#else
  #define GFL_SINGLETON_INSTANCE(classname) (classname::GetInstance())
#endif



#endif // __GFL2_SINGLETON_H__
