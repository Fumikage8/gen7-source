#ifndef __GFL_DEBUGBASECLASS_H__
#define __GFL_DEBUGBASECLASS_H__
//=============================================================================
/**
 * @file	  gfl_DebugBaseClass.h
 * @brief	  デバッグ用基底クラス
            デバッグの為にクラス名を文字列で管理します
 * @author  k.ohno
 * @date	  2011.3.6
 */
//=============================================================================
#pragma once



#if GFL_DEBUG

namespace gfl{
namespace debug{

class DebugBaseClass{
  GFL_FORBID_COPY_AND_ASSIGN(DebugBaseClass); //コピーコンストラクタ＋代入禁止

public:
  DebugBaseClass(void){};
  virtual ~DebugBaseClass(){};

  //----------------------------------------------------------------------------
  /**
   *	@brief    クラスを登録する
   *  @param    name クラス名
   */
  //-----------------------------------------------------------------------------
  void AddClass(const char* name);
  //----------------------------------------------------------------------------
  /**
   *	@brief    クラス名を得る
   *  @return   クラス名
   */
  //-----------------------------------------------------------------------------
  const char* GetClassName(void);
  //----------------------------------------------------------------------------
  /**
   *	@brief    クラスを廃棄する
   *  @param    name クラス名
   */
  //-----------------------------------------------------------------------------
  void DeleteClass(const char* name);



  

};


}  //debug
}  //gfl

#endif //GFL_DEBUG


//クラス宣言のマクロ

//#if GFL_DEBUG
//#define CLASS_DECLARATION( className ) class className : public gfl::debug::DebugBaseClass
//#else //GFL_DEBUG
//#define CLASS_DECLARATION( className ) class className
//#endif //GFL_DEBUG




#endif //__GFL_DEBUGBASECLASS_H__

