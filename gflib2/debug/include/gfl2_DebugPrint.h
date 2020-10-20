#ifndef __GFL_DEBUGPRINT_H__
#define __GFL_DEBUGPRINT_H__
//======================================================================
/**
 * @file	  gfl_debug_print.h
 * @brief	  デバッグ出力
 * @author	ariizumi
 * @data	  10/10/13
 */
//======================================================================

#pragma once

namespace gfl2
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



#define GFL_PRINT_EX(flag,...) gfl2::debug::PrintCore(flag,__VA_ARGS__)
#define GFL_PRINT(...) GFL_PRINT_EX(0,__VA_ARGS__)

#define NN_LOG(...)		GFL_PRINT_EX(0,__VA_ARGS__)
#define GFL_WARNING(...)		GFL_PRINT_EX(0,__VA_ARGS__)


#else //GFL_DEBUG_PRINT

#define NN_LOG( ...)      ((void)0)
#define GFL_WARNING( ...)      ((void)0)

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
#if defined( DEBUG_ONLY_FOR_taya )
#define   TAYA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   TAYA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ohno_katsumi )
#define   OHNO_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   OHNO_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ariizumi_nobuhiko )
#define   ARIIZUMI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ARIIZUMI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_takahashi_tomoya )
#define   TOMOYA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   TOMOYA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_obata_toshihiro )
#define   OBATA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   OBATA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_kawada_koji )
#define   KAWADA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   KAWADA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ishiguro_masateru )
#define   ISHIGURO_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ISHIGURO_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_nakamura )
#define   NAKAHIRO_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   NAKAHIRO_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_saito_nozomu )
#define   NOZOMU_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   NOZOMU_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_tamada )
#define   TAMADA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   TAMADA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_iwasawa )
#define   IWASAWA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   IWASAWA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ichiraku_katsuhiko )
#define   ICHI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ICHI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_kousaka_shin )
#define   KOUSAKA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   KOUSAKA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ikuta_junya )
#define   IKUTA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   IKUTA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_uematsu_naoya )
#define   UEMATSU_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   UEMATSU_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_takebe_makoto )
#define   TAKEBE_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   TAKEBE_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_watanabe )
#define   WATANABE_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   WATANABE_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_gomi_akihiro )
#define   GOMI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   GOMI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_saita_kazuki )
#define   SAITA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   SAITA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_kanamaru_masanori )
#define   KANAMARU_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   KANAMARU_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_takeda_nobutaka )
#define   TAKEDA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   TAKEDA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ikeuchi_yuya )
#define   IKEUCHI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   IKEUCHI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_fukushima_yuya )
#define   FUKUSHIMA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   FUKUSHIMA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_uchida_yuto )
#define   UCHIDA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   UCHIDA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_endo_akira )
#define   ENDO_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ENDO_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_yam_nathanael )
#define   YAM_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   YAM_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_muto_yu )
#define   MUTO_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   MUTO_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_araki_syo )
#define   ARAKI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ARAKI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_miyachi_soichi )
#define   MIYACHI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   MIYACHI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ikawa_hiroki )
#define   IKAWA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   IKAWA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_anai_shigeo )
#define   ANAI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ANAI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_kiryu__morihiko )
#define   KIRYU_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   KIRYU_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_kijima_satoshi )
#define   KIJIMA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   KIJIMA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_nagihashi_toru )
#define   NAGI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   NAGI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_ohhira_syo )
#define   OHHIRA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   OHHIRA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_onoue_masayuki )
#define   ONOUE_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ONOUE_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_fang_yicheng )
#define   HOU_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   HOU_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_arai_takashi )
#define   ARAI_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   ARAI_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_kawazoe_shinichi )
#define   KAWAZOE_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   KAWAZOE_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_harada_takumi )
#define   HARADA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   HARADA_PRINT( ... )      /* */
#endif
//=======================================
#if defined( DEBUG_ONLY_FOR_munakata_kai )
#define   MUNAKATA_PRINT( ... )      gfl2::debug::UserPrint(__VA_ARGS__);
#else
#define   MUNAKATA_PRINT( ... )      /* */
#endif
//=======================================
#if GFL_DEBUG

//----------------------------------------------------------------------
/**
* @brief   ログ出力(RELEASEでも動作する版)
* @note    一時確認の使用を想定しているもの、使用箇所は基本的にコミットしないでください
* @note    【コミットしても問題ないケース】
* @note    ・デバッグメニューの先で特定の操作を行った際に通るシーケンス
* @note    ・ゲーム起動時のシステム使用量
*
* @param[in]    fmt  出力(可変引数対応)
*/
//----------------------------------------------------------------------
void ReleasePrintCore( const char* pszFormat , ... );

#define GFL_RELEASE_PRINT(...) gfl2::debug::ReleasePrintCore(__VA_ARGS__)

#else
#define GFL_RELEASE_PRINT(...) ((void)0)
#endif



}//namespace debug
}//namespace gfl2
#endif // __GFL_DEBUGPRINT_H__
