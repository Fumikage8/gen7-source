#ifndef __GFL_DEBUGBASECLASSMANAGER_H__
#define __GFL_DEBUGBASECLASSMANAGER_H__
//=============================================================================
/**
 * @file	  gfl_DebugBaseClassManager.h
 * @brief	  デバッグ用基底クラス
            デバッグの為にクラス名を文字列で管理します
            デバッグ用のため扱いやすさを最優先するため静的になっています
 * @author  k.ohno
 * @date	  2011.3.6
 */
//=============================================================================
#pragma once



#if GFL_DEBUG

namespace gfl{
namespace debug{

class DebugBaseClassManager{
  GFL_FORBID_COPY_AND_ASSIGN(DebugBaseClassManager); //コピーコンストラクタ＋代入禁止


public:
  static const char CLASSNAME_SIZE = 24;  ///< 名前を保存するバッファ

private:

  
  struct ClassData{
    void* Next;       ///< 次のポインタ
    void* ClassPtr;   ///< クラスのアドレス
    unsigned long count;  ///< 確保順番
    char Name[CLASSNAME_SIZE];  ///< クラス名
  } ;

  static ClassData* mpClassDataStart; ///< クラスデータの先頭のアドレス
  static ClassData* mpClassDataEnd;   ///< クラスデータの最後のアドレス
  static unsigned long mCount;               ///< カウンター
  static gfl::heap::HeapBase *mpDebugHeap;   ///< デバッグ用ヒープアドレス

public:

  //----------------------------------------------------------------------------
  /**
   *	@brief    初期化する
   *  @param    debugHeap デバッグヒープクラスポインタ
   */
  //-----------------------------------------------------------------------------
  static void Initialize( gfl::heap::HeapBase *debugHeap );
  //----------------------------------------------------------------------------
  /**
   *	@brief    クラスを記録する
   *  @param    name クラス名
   */
  //-----------------------------------------------------------------------------
  static void AddClass(const char* name,void* classAddr);
  //----------------------------------------------------------------------------
  /**
   *	@brief    クラス名を得る
   *  @return   クラス名
   */
  //-----------------------------------------------------------------------------
  static const char* GetClassName(void* classAddr);
  //----------------------------------------------------------------------------
  /**
   *	@brief    クラスを消す
   *  @return   クラスアドレス
   */
  //-----------------------------------------------------------------------------
  static void RemoveClass(void* classAddr);

  //----------------------------------------------------------------------------
  /**
   *	@brief    開放する
   */
  //-----------------------------------------------------------------------------
  static void Finalize( void );

  
};  // class DebugBaseClassManager

}  //namespace debug
}  //namespace gfl

#endif //GFL_DEBUG

#endif //__GFL_DEBUGBASECLASSMANAGER_H__

