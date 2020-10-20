//==============================================================================
/**
 * @file        ScriptFuncSetAppCall.h
 * @brief       アプリケーションコール系 
 * @author      N.Takeda
 * @date        2015/04/13(月) 18:52:41
 */
//==============================================================================

#if !defined(__SCRIPTFUNCSETAPPCALL_H__) // 重複定義防止
#define __SCRIPTFUNCSETAPPCALL_H__   // 重複定義防止マクロ
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>

#include "pawn/include/gfl2_Pawn.h"


//前方宣言
namespace GameSys{
  class GameManager;
}
namespace Field{
  namespace Camera{
    class CameraManager;
  }
  namespace FieldScript{
    class FieldScriptSystem;
  }
}

/// @brief フィールド
GFL_NAMESPACE_BEGIN( Field )
/// @brief スクリプト
GFL_NAMESPACE_BEGIN( FieldScript )

//------------------------------------------------------------------------------
/**
 * @class       FuncSetAppCall
 * @brief       
 * @author      N.Takeda
 * @date        2015/04/13(月) 20:58:43
 */
//------------------------------------------------------------------------------
class FuncSetAppCall {
  GFL_FORBID_COPY_AND_ASSIGN(FuncSetAppCall);

public:
  /// 命令テーブルセットを取得.
  static const GFL_PAWN_FUNC_TABLE* GetTable(void);
  
private:
  // スクリプトに渡す関数
  static cell AMX_NATIVE_CALL CallDemo_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL MoviePlay_(AMX *, const cell * );
  
  static cell AMX_NATIVE_CALL CallCaptureDemo_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallZenryokuWazaDemo_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallEggDemoEvent( AMX *, const cell * );

  static cell AMX_NATIVE_CALL CallPokemonBox_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallBoxFieldTrade( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallPokePartyNameInput_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallPokeSelect_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ResultPokeSelect_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallPokeWazaApp_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL WaitPokeWazaApp_(AMX *, const cell * );

  static cell AMX_NATIVE_CALL CallWazaOmoidashi_(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL ResultWazaOmoidashi_(AMX* amx, const cell * ptr);

  static cell AMX_NATIVE_CALL CallFriendlyShopBuy_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallFriendlyShopSell_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallKisekaeShopBuy_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallKisekaeFittingShop_(AMX *, const cell *);

  static cell AMX_NATIVE_CALL CallSuperTrainingApp_(AMX* amx, const cell * ptr);
  static cell AMX_NATIVE_CALL ResultSuperTrainingApp_(AMX* amx, const cell * ptr);

  //ジョインフェスタ系列
  static cell AMX_NATIVE_CALL CallAttractionList_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL ResultAttractionList_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallAttractionResult_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallJoinFestaPlayerDetail_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallJoinFestaPlayerListSelectForScript_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallGameSync_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallBtlVideoWifi_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallGlobalAttraction_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallKisekaeShopJoinFesta_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL ResultKisekaeShopJoinFesta_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallFesCircleNameInput_( AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallBFCPokeStatus_Mine_(AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL CallBFCPokeStatus_Lottery_(AMX * amx, const cell * ptr );
  static cell AMX_NATIVE_CALL CallBFCTrainerSelect_(AMX * amx, const cell * ptr );


  //NPCとのポケモン交換（Commonでは上手くいかなかったのでこちらに移動）
  static cell AMX_NATIVE_CALL PokeTradeLoad( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokeTradeIsLoaded( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokeTradeExec( AMX *, const cell * );
  static cell AMX_NATIVE_CALL PokeTradeFin( AMX *, const cell * );

  static cell AMX_NATIVE_CALL CallDendouiriDemo_(AMX *, const cell * );
  static cell AMX_NATIVE_CALL CallGameClearEvent_(AMX *, const cell * );

  // バトル施設関連
  static cell AMX_NATIVE_CALL CallRegulationSelectApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL CallTeamSelectApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL GetResultTeamSelectApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL CallPokeListApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL CallNpcSelectApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL CallQuickMatchApp_( AMX*, const cell* );
  static cell AMX_NATIVE_CALL BinstBattleRecorder_( AMX *amx, const cell * ptr );

  static cell AMX_NATIVE_CALL CallStampApp_( AMX*, const cell* );

  //配達員
  static cell AMX_NATIVE_CALL CallPostmanPokemonReceive_( AMX*, const cell* );

  //ジガルデ生成
  static cell AMX_NATIVE_CALL CallZigarudeUnionFromCell_( AMX*, const cell* );

  //マンタインサーフ
  static cell AMX_NATIVE_CALL CallMantainSurf_(AMX*, const cell*);
  //ルナソルダイブ
  static cell AMX_NATIVE_CALL CallLunaSolDive_(AMX*, const cell*);
  //ファインダースタジオ(写真を撮る)
  static cell AMX_NATIVE_CALL CallFinderStudioCapture_(AMX*, const cell*); 

  // バトルレコーダー呼び出し
  static cell AMX_NATIVE_CALL CallBfesBattleRecorder_(AMX*, const cell*);

  // スタッフロール アプリ呼び出し
  static cell AMX_NATIVE_CALL CallStaffrollApp_(AMX*, const cell*);

  //マイアルバム
  static cell AMX_NATIVE_CALL CallMyAlbum_(AMX*, const cell*);

  // PhotoView
  static cell AMX_NATIVE_CALL FuncSetAppCall::CallPhotoView_(AMX * amx, const cell * ptr );

  private:
  static bool WaitFunc_PokePartyNameInput( FieldScriptSystem* p_sys, u32 label, void* p_work );
  static bool WaitFunc_FesCircleNameInput( FieldScriptSystem* p_sys, u32 label, void* p_work );
  static bool WaitFunc_JoinFestaPlayerListSelectForScript( FieldScriptSystem* p_sys, u32 label, void* p_work );
  static bool WaitFunc_PokemonBox( FieldScriptSystem* p_sys, u32 label, void* p_work );
  static bool WaitFunc_BoxFieldTrade( FieldScriptSystem* p_sys, u32 label, void* p_work );
  static bool WaitFunc_PokemonReceive( FieldScriptSystem* p_sys, u32 label, void* p_work );
  static bool WaitFunc_CallPokeStatus( FieldScriptSystem* p_sys, u32 label, void* _pWork );
  enum {
    CALL_POKESTATUS_LOTTERY_NO_CREATE = 0,
    CALL_POKESTATUS_LOTTERY_CREATE,
  };
  static bool WaitFunc_CallPokeStatus_Lottery( FieldScriptSystem* p_sys, u32 label, void* _pWork );
  static bool WaitFunc_CallBFCTrainerSelect(FieldScriptSystem* p_sys, u32 label, void* _pWork );
  static bool WaitFunc_MantainSurf(FieldScriptSystem* p_sys, u32 label, void* _pWork);
  static bool WaitFunc_LunaSolDive(FieldScriptSystem* p_sys, u32 label, void* _pWork);
  static bool WaitFunc_BfesBattleRecorder(FieldScriptSystem* p_sys, u32 label, void* _pWork);
  static bool WaitFunc_CallPhotoView( FieldScriptSystem* p_sys, u32 label, void* _pWork );
};


GFL_NAMESPACE_END( FieldScript )
GFL_NAMESPACE_END( Field )

#endif // __SCRIPTFUNCSETAPPCALL_H__ 重複定義防止

