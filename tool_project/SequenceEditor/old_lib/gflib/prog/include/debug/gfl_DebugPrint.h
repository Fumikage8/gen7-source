#ifndef __GFL_DEBUGPRINT_H__
#define __GFL_DEBUGPRINT_H__
//======================================================================
/**
 * @file	gfl_DebugPrint.h
 * @brief	デバッグ出力
 * @author	ariizumi
 * @data	10/10/13
 */
//======================================================================

#pragma once

namespace gfl
{
namespace debug
{

//Printに設定するフラグ
typedef enum
{
  //出力レベル
  PRINT_FLAG_LEVEL1 = 1<<0, //デフォルト出力される
  PRINT_FLAG_LEVEL2 = 1<<1, //各カテゴリで任意
  PRINT_FLAG_LEVEL3 = 1<<2, //各カテゴリで任意
  PRINT_FLAG_LEVEL4 = 1<<3, //各カテゴリで任意
  
  //出力カテゴリ
  PRINT_FLAG_CATEGORY_FS     = 1<<4,
  PRINT_FLAG_CATEGORY_G2D    = 1<<5,
  PRINT_FLAG_CATEGORY_G3D    = 1<<6,
  PRINT_FLAG_CATEGORY_COMM   = 1<<7,
  PRINT_FLAG_CATEGORY_UI     = 1<<8,
  PRINT_FLAG_CATEGORY_DEVICE = 1<<9,
  PRINT_FLAG_CATEGORY_STRING = 1<<10,
  PRINT_FLAG_CATEGORY_HEAP   = 1<<11,
  PRINT_FLAG_CATEGORY_DEBUG   = 1<<12,

  PRINT_FLAG_USER   = 1<<13  //以降ユーザーカテゴリ
}PrintFlag;

/*
//ユーザーカテゴリ作成例
enum
{
  XY_PRINT_FLAG_SYSTEM = gfl::debug::PRINT_FLAG_USER<<0,
  XY_PRINT_FLAG_APP    = gfl::debug::PRINT_FLAG_USER<<1,
  XY_PRINT_FLAG_COMM   = gfl::debug::PRINT_FLAG_USER<<2,
}

//ユーザー定義例

//grepで検索できるようにGFL_PRINT_EXをマクロに入れるのは避けてください。
//フラグをソースの頭で定義し、使用してください。
#define PRINT_FLG1 (gfl::debug::PRINT_FLAG_LEVEL1|gfl::debug::PRINT_FLAG_CATEGORY_HEAP)
#define PRINT_FLG2 (gfl::debug::PRINT_FLAG_LEVEL2|gfl::debug::PRINT_FLAG_CATEGORY_HEAP)

GFL_PRINT_EX( PRINT_FLG1 , "Heap Create[size %x]\n",size );
GFL_PRINT_EX( PRINT_FLG2 , "[addr %x][Least mem%x]\n",addr,GetAllocatebleSize() );
*/


  
#if GFL_DEBUG_PRINT
  //----------------------------------------------------------------------
  /**
   * @brief   フラグの初期化 GFL_PRINT_FLAG_LEVEL1が設定されます
   */
  //----------------------------------------------------------------------
  extern void ResetPrintFlag();
  //----------------------------------------------------------------------
  /**
   * @brief   フラグのトグル
   */
  //----------------------------------------------------------------------
  extern bool TogglePrintFlg(void);
  //----------------------------------------------------------------------
  /**
   * @brief   フラグの設定(上書き)
   *
   * @param[in]    flag フラグビット
   */
  //----------------------------------------------------------------------
  extern void SetPrintFlag( const int flag );

  //----------------------------------------------------------------------
  /**
   * @brief   フラグの設定(追加)
   *
   * @param[in]    flag フラグビット
   */
  //----------------------------------------------------------------------
  extern void AddPrintFlag( const int flag );

  //----------------------------------------------------------------------
  /**
   * @brief   フラグの設定(削除)
   *
   * @param[in]    flag フラグビット
   */
  //----------------------------------------------------------------------
  extern void SubPrintFlag( const int flag );

  //----------------------------------------------------------------------
  /**
   * @brief   ログ出力
   *
   * @param[in]    flag フラグビット
   * @param[in]    fmt  出力(可変引数対応)
   */
  //----------------------------------------------------------------------
  extern void PrintCore( const int flag , const char* fmt , ... );

#define GFL_PRINT_EX(flag,...) gfl::debug::PrintCore(flag,__VA_ARGS__)
#define GFL_PRINT(...) GFL_PRINT_EX(0,__VA_ARGS__)


#else //GFL_DEBUG_PRINT
#define GFL_PRINT_EX(flag, ...) ((void)0)
#define GFL_PRINT( ...)      ((void)0)
void ResetPrintFlag();
bool TogglePrintFlg(void);
void SetPrintFlag( const int flag );
void AddPrintFlag( const int flag );
void SubPrintFlag( const int flag ); 
#endif//GFL_DEBUG_PRINT


//----------------------------------------------------------------------
//ユーザ別プリント
//----------------------------------------------------------------------
typedef enum {
  AUTHOR_FREE = 0,  ///< これを指定すると全環境で出力される

  AUTHOR_OHNO,
  AUTHOR_TAYA,
  AUTHOR_TOMOYA,
  AUTHOR_ARIIZUMI,
  AUTHOR_OBATA,
  AUTHOR_KAWADA,
  AUTHOR_ISHIGURO,
  AUTHOR_KIMURA,
  AUTHOR_NAKAHIRO,
  AUTHOR_HOSAKA,
  AUTHOR_SOGABE,
  AUTHOR_NOZOMU,
  AUTHOR_SUZUKI,
  AUTHOR_TAMADA,
  AUTHOR_MORI,
  AUTHOR_IWASAWA,
  AUTHOR_NAGIHASHI,
  AUTHOR_ICHIRAKU,
  AUTHOR_KOUSAKA,
  AUTHOR_ONOUE,
  AUTHOR_ITO,
  AUTHOR_IKUTA,
  AUTHOR_SAITA,

  AUTHOR_OHHIRA,
  AUTHOR_KATO,
  AUTHOR_TAKEBE,


  AUTHOR_IGNORE    ///< これを指定すると出力されない
}AUTHOR_ID;

#if defined DEBUG_ONLY_FOR_taya
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_TAYA)
#elif defined DEBUG_ONLY_FOR_ohno_katsumi
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_OHNO)
#elif defined DEBUG_ONLY_FOR_ariizumi_nobuhiko
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_ARIIZUMI)
#elif defined DEBUG_ONLY_FOR_takahashi_tomoya
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_TOMOYA)
#elif defined DEBUG_ONLY_FOR_obata_toshihiro
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_OBATA)
#elif defined DEBUG_ONLY_FOR_kawada_koji
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_KAWADA)
#elif defined DEBUG_ONLY_FOR_ishiguro_masateru
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_ISHIGURO)
#elif defined DEBUG_ONLY_FOR_kimura_shin
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_KIMURA)
#elif defined DEBUG_ONLY_FOR_hiro_nakamura
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_NAKAHIRO)
#elif defined DEBUG_ONLY_FOR_hosaka_genya
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_HOSAKA)
#elif defined DEBUG_ONLY_FOR_sogabe
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_SOGABE)
#elif defined DEBUG_ONLY_FOR_saito_nozomu
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_NOZOMU)
#elif defined DEBUG_ONLY_FOR_suzuki_hisanao
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_SUZUKI)
#elif defined DEBUG_ONLY_FOR_tamada
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_TAMADA)
#elif defined DEBUG_ONLY_FOR_mori_akito
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_MORI)
#elif defined DEBUG_ONLY_FOR_iwasawa
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_IWASAWA)
#elif defined DEBUG_ONLY_FOR_nagihashi_toru
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_NAGIHASHI)
#elif defined DEBUG_ONLY_FOR_ichiraku_katsuhiko
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_ICHIRAKU)
#elif defined DEBUG_ONLY_FOR_onoue_masayuki
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_ONOUE)
#elif defined DEBUG_ONLY_FOR_ito_isao
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_ITO)
#elif defined DEBUG_ONLY_FOR_kousaka_shin
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_KOUSAKA)
#elif defined DEBUG_ONLY_FOR_ikuta_junya
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_IKUTA)
#elif defined DEBUG_ONLY_FOR_saita_kazuki
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_SAITA)
#elif defined DEBUG_ONLY_FOR_ohhira_syo
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_OHHIRA)
#elif defined DEBUG_ONLY_FOR_kato_tatsuya
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_KATO)
#elif defined DEBUG_ONLY_FOR_takebe_makoto
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_TAKEBE)
#else
#define DEFINED_AUTHOR_ID (gfl::debug::AUTHOR_FREE)
#endif


#if GFL_DEBUG_PRINT
//==============================================================================================
/**
 * 特定ユーザの環境でコンパイルした場合のみ動作するPrintf関数
 *
 * @param   fmt       OS_TPrintfに渡す書式付き文字列
 *
 */
//==============================================================================================
extern void UserPrint( const char * fmt, ...);
#else //GFL_DEBUG

extern void UserPrint( const char * fmt, ... );

#endif  //GFL_DEBUG

//-----------------------------------------------------------
// かんたん記述用マクロ
//-----------------------------------------------------------
#define   TAYA_PRINT( ... )      if(gfl::debug::AUTHOR_TAYA     ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
//#define   OHNO_PRINT( ...)       if((gfl::debug::AUTHOR_OHNO     ==DEFINED_AUTHOR_ID)||(gfl::debug::AUTHOR_SUZUKI   ==DEFINED_AUTHOR_ID)){gfl::debug::UserPrint(__VA_ARGS__);}
#define   OHNO_PRINT( ...)       if((gfl::debug::AUTHOR_OHNO     ==DEFINED_AUTHOR_ID)){gfl::debug::UserPrint(__VA_ARGS__);}
#define   TOMOYA_PRINT( ... )    if(gfl::debug::AUTHOR_TOMOYA   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   ARIIZUMI_PRINT( ...)   if(gfl::debug::AUTHOR_ARIIZUMI ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   OBATA_PRINT( ... )     if(gfl::debug::AUTHOR_OBATA    ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   KAWADA_PRINT( ... )    if(gfl::debug::AUTHOR_KAWADA   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   ISHIGURO_PRINT( ... )  if(gfl::debug::AUTHOR_ISHIGURO ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   KIMURA_PRINT( ... )    if(gfl::debug::AUTHOR_KIMURA   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   NAKAHIRO_PRINT( ... )  if(gfl::debug::AUTHOR_NAKAHIRO ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   HOSAKA_PRINT( ... )    if(gfl::debug::AUTHOR_HOSAKA   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   SOGABE_PRINT( ... )    if(gfl::debug::AUTHOR_SOGABE   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   NOZOMU_PRINT( ... )    if(gfl::debug::AUTHOR_NOZOMU   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   SUZUKI_PRINT( ... )    if(gfl::debug::AUTHOR_SUZUKI   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   TAMADA_PRINT( ... )    if(gfl::debug::AUTHOR_TAMADA   ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   MORI_PRINT( ... )      if(gfl::debug::AUTHOR_MORI     ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   IWASAWA_PRINT( ... )   if(gfl::debug::AUTHOR_IWASAWA  ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   KOUSAKA_PRINT( ... )   if(gfl::debug::AUTHOR_KOUSAKA  ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   NAGI_PRINT( ... ) if(gfl::debug::AUTHOR_NAGIHASHI==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   ICHI_PRINT( ... )  if(gfl::debug::AUTHOR_ICHIRAKU ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   ONOUE_PRINT( ... )     if(gfl::debug::AUTHOR_ONOUE    ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   ITO_PRINT( ... )       if(gfl::debug::AUTHOR_ITO      ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   IKUTA_PRINT( ... )       if(gfl::debug::AUTHOR_IKUTA      ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   SAITA_PRINT( ... )     if(gfl::debug::AUTHOR_SAITA    ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   OHHIRA_PRINT( ... )     if(gfl::debug::AUTHOR_OHHIRA    ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   KATO_PRINT( ... )     if(gfl::debug::AUTHOR_KATO    ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}
#define   TAKEBE_PRINT( ... )     if(gfl::debug::AUTHOR_TAKEBE    ==DEFINED_AUTHOR_ID){gfl::debug::UserPrint(__VA_ARGS__);}

}//namespace debug
}//namespace gfl
#endif // __GFL_DEBUGPRINT_H__
