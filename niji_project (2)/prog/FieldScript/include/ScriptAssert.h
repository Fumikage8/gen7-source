/*
 *  @file   ScriptAssert.h
 *  @brief  イベントスクリプトのmake対象者のみに出るアサートマクロ定義
 *  @author Miyuki Iwasawa
 *
 *  @note 対象者は手動で追加
 */

#if !defined( __HSCRIPTASSERTH__ )
#define __HSCRIPTASSERTH__

#pragma once

#include <debug/include/gfl2_Assert.h>

class ScriptAssert{
public:
  static int ScriptAssertEnableFlag;  ///<これが有効だとSCRIPT_ASSERTが有効になる
};

#if defined(DEBUG_ONLY_FOR_takahashi_tomoya) || \
    defined(DEBUG_ONLY_FOR_suginaka_katsunori) || \
    defined(DEBUG_ONLY_FOR_takeda_nobutaka) || \
    defined(DEBUG_ONLY_FOR_ohhira_syo ) || \
    defined(DEBUG_ONLY_FOR_saito_nozomu ) || \
    defined(DEBUG_ONLY_FOR_kanamaru_masanori ) || \
    defined(DEBUG_ONLY_FOR_ikuta_junya ) || \
    defined(DEBUG_ONLY_FOR_saita_kazuki ) || \
    defined(DEBUG_ONLY_FOR_ikeuchi_yuya ) || \
    defined(DEBUG_ONLY_FOR_miyachi_soichi ) || \
    defined(DEBUG_ONLY_FOR_tamagawa_satoshi ) || \
    defined(DEBUG_ONLY_FOR_watanabe_yuya ) || \
    defined(DEBUG_ONLY_FOR_muranaka_ryota ) || \
    defined(DEBUG_ONLY_FOR_kojima_yoshikazu ) || \
    defined(DEBUG_ONLY_FOR_hosaka_genya ) || \
    defined(DEBUG_ONLY_FOR_asaba_akinori ) || \
    defined(DEBUG_ONLY_FOR_onoue_masayuki ) || \
    defined(DEBUG_ONLY_FOR_iwasawa )

  //#define SCRIPT_ASSERT_ENABLE
#endif

#if !defined(SCRIPT_ASSERT_ENABLE)
  #define SCRIPT_ASSERT_ENABLE  //デバッグ後期に全環境でSCRIPT_ASSERT有効
#endif

#ifdef SCRIPT_ASSERT_ENABLE
  #define SCRIPT_ASSERT(check) { \
    if(ScriptAssert::ScriptAssertEnableFlag){ \
      if (!check) { GFL_RELEASE_PRINT("[[SCRIPT_ASSERT]]\n"); } GFL_ASSERT(check);  \
    }else{ \
      if((!check)) GFL_RELEASE_PRINT("[[SCRIPT_WARNING]]]\n%s line %d %s\n", __FILE__, __LINE__, #check ); \
    } \
  }
  #define SCRIPT_ASSERT_MSG(check,...) { \
    if(ScriptAssert::ScriptAssertEnableFlag){ \
      if (!check) { GFL_RELEASE_PRINT("[[SCRIPT_ASSERT]]\n"); } GFL_ASSERT_MSG(check,__VA_ARGS__);  \
    }else{ \
      if((!check)) GFL_RELEASE_PRINT("[[SCRIPT_WARNING]]]\n%s line %d %s\n", __FILE__, __LINE__, #check ); \
    } \
  }
#else
  #define SCRIPT_ASSERT(check) { if((!check)) GFL_RELEASE_PRINT("[[SCRIPT_WARNING]]]\n%s line %d %s\n", __FILE__, __LINE__, #check );}
  #define SCRIPT_ASSERT_MSG(check,...) { if((!check)) GFL_RELEASE_PRINT("[[SCRIPT_WARNING]]\n"); GFL_PRINT(__VA_ARGS__);}
#endif

#define SCRIPT_WARNING_MSG(check,...) { if((!check)) GFL_RELEASE_PRINT("[[SCRIPT_WARNING]]\n"); GFL_PRINT(__VA_ARGS__);}

#endif		// __HSCRIPTASSERTH__

