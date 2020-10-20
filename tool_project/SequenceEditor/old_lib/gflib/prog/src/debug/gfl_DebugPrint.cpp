//======================================================================
/**
 * @file	gfl_DebugPrint.cpp
 * @brief	デバッグ出力
 * @author	ariizumi
 * @data	10/10/13
 */
//======================================================================

#include <gflib.h>
#include <debug/gfl_DebugPrint.h>

//プラットフォームごとの定義
#include <nn/dbg.h>
//プラットフォームごとの定義 ここまで



namespace gfl
{
namespace debug
{


#if GFL_DEBUG_PRINT

  static const int defaultPrintFlag = PRINT_FLAG_LEVEL1;
  
  static u16 printFlag = defaultPrintFlag;
  static u16 printEnable = 1;

  //----------------------------------------------------------------------
  /**
   * @brief   フラグの初期化 GFL_PRINT_FLAG_LEVEL1が設定されます
   */
  //----------------------------------------------------------------------
  void ResetPrintFlag()
  {
    printFlag = defaultPrintFlag;
  }
  
  //----------------------------------------------------------------------
  /**
   * @brief   フラグの設定(上書き)
   *
   * @param[in]    flag フラグビット
   */
  //----------------------------------------------------------------------
  void SetPrintFlag( const int flag )
  {
    printFlag = flag;
  }
  bool TogglePrintFlg(void)
  {
    printEnable = 1 - printEnable;
    return printEnable;
  }

  
  //----------------------------------------------------------------------
  /**
   * @brief   フラグの設定(追加)
   *
   * @param[in]    flag フラグビット
   */
  //----------------------------------------------------------------------
  void AddPrintFlag( const int flag )
  {
    printFlag |= flag;
  }
  
  //----------------------------------------------------------------------
  /**
   * @brief   フラグの設定(削除)
   *
   * @param[in]    flag フラグビット
   */
  //----------------------------------------------------------------------
  void SubPrintFlag( const int flag )
  {
    printFlag -= (printFlag&flag);
  }
  
  //----------------------------------------------------------------------
  /**
   * @brief   ログ出力
   *
   * @param[in]    flag フラグビット
   * @param[in]    fmt  出力(可変引数対応)
   */
  //----------------------------------------------------------------------
  void PrintCore( const int flag , const char *fmt, ... )
  {
    if(!printEnable){
      return;
    }
    if( flag == (printFlag&flag) )
    {
      va_list vlist;
      va_start(vlist, fmt);

      //プラットフォームごとの処理
      
      //可変引数が渡せないので内部関数使用
      (void)nn::dbg::detail::VPrintf(fmt,vlist);

      //プラットフォームごとの処理 ここまで

      va_end(vlist);
    }
  }

  void PrintCore2( const char* fmt , ... )
  {
    if(!printEnable){
      return;
    }
    va_list vlist;
    va_start(vlist, fmt);

    //プラットフォームごとの処理
    
    //可変引数が渡せないので内部関数使用
    (void)nn::dbg::detail::VPrintf(fmt,vlist);

    //プラットフォームごとの処理 ここまで

    va_end(vlist);
  }
  
  //==============================================================================================
  /**
   * 特定ユーザの環境でコンパイルした場合のみ動作するPrintf関数
   *
   * @param   fmt       OS_TPrintfに渡す書式付き文字列
   *
   */
  //==============================================================================================
  void UserPrint( const char * fmt, ... )
  {
    if(!printEnable){
      return;
    }
    //判定はヘッダでやらないとgflibのmakeした人のが出てしまう・・・
    va_list vlist;
    va_start(vlist, fmt);

    //プラットフォームごとの処理
    
    //可変引数が渡せないので内部関数使用
    (void)nn::dbg::detail::VPrintf(fmt,vlist);

    //プラットフォームごとの処理 ここまで

    va_end(vlist);
  }



#else  //GFL_DEBUG_PRINT
void ResetPrintFlag(){}
bool TogglePrintFlg(void){ return true; }
void SetPrintFlag( const int flag ){GFL_UNUSED(flag);}
void AddPrintFlag( const int flag ){GFL_UNUSED(flag);}
void SubPrintFlag( const int flag ){GFL_UNUSED(flag);}
void UserPrint( const char * fmt, ... ){GFL_UNUSED( fmt );}
#endif  //GFL_DEBUG_PRINT


}  // namespace debug
}  // namespace gfl

