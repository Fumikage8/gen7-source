#if !defined( __BTL_DEBUGPRINT_H__ )
#define __BTL_DEBUGPRINT_H__
//=============================================================================================
/**
 * @file  btl_debug_print.h
 * @brief ポケモンWB バトルシステム デバッグPrint機能
 * @author  taya
 *
 * @date  2010.01.07  作成
 */
//=============================================================================================
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <debug/include/gfl2_DebugPrint.h>

GFL_NAMESPACE_BEGIN(btl)

#if( ( defined(DEBUG_ONLY_FOR_ariizumi_nobuhiko) & 0 ) || defined(DEBUG_ONLY_FOR_obata_toshihiro ) )
#define USE_BTLV_PRINT 1
#define BTL_PRINT GFL_PRINT
#else
#define USE_BTLV_PRINT 0
#define BTL_PRINT(...) //
#endif

  typedef enum {
    BTL_PRINTTYPE_UNKNOWN,    ///< タイプ未確定
    BTL_PRINTTYPE_SERVER,     ///< タイプサーバー
    BTL_PRINTTYPE_CLIENT,     ///< タイプサーバー以外
    BTL_PRINTTYPE_STANDALONE, ///< タイプスタンドアロン実行
  }BtlPrintType;

  /**
   *  文字列リテラルID
   */
  typedef enum {
    DBGSTR_SETUP_DONE = 0,
    DBGSTR_DEBUGFLAG_BIT,
    DBGSTR_ADAPTER_SEND_START,
    DBGSTR_ADAPTER_RECV_DONE,
    DBGSTR_ADAPTER_Create,
    DBGSTR_ADAPTER_RECVED_A_CLIENT,
    DBGSTR_MAIN_PokeConGetByPos,
    DBGSTR_MAIN_CheckHPByLvup,
    DBGSTR_MAIN_PerappVoiceAdded,
    DBGSTR_MAIN_PerappVoiceComplete,
    DBGSTR_MAIN_MultiAITrainer_SeqStart,
    DBGSTR_MAIN_MultiAITrainer_SendDone,
    DBGSTR_MAIN_CheckResultStart,
    DBGSTR_MAIN_Result_CommError,
    DBGSTR_MAIN_Result_Capture,
    DBGSTR_MAIN_Result_Escape,
    DBGSTR_MAIN_Result_RestCnt,
    DBGSTR_MAIN_Result_ServerCalc,

    DBGSTR_CLIENT_RETURN_CMD_START,
    DBGSTR_CLIENT_RETURN_CMD_DONE,
    DBGSTR_CLIENT_SelActPokeDead,
    DBGSTR_CLIENT_SelectActionSkip,
    DBGSTR_CLIENT_SelectActionRoot,
    DBGSTR_CLIENT_SelectActionStart,
    DBGSTR_CLIENT_SelectActionDone,
    DBGSTR_CLIENT_SelectActionBacktoRoot,
    DBGSTR_CLIENT_SelectAction_Pokemon,
    DBGSTR_CLIENT_SelectAction_Fight,
    DBGSTR_CLIENT_SelectChangePoke,
    DBGSTR_CLIENT_SelectChangePokeCancel,
    DBGSTR_CLIENT_ReturnSeqDone,
    DBGSTR_CLIENT_WazaLockInfo,
    DBGSTR_CLIENT_NoMorePuttablePoke,
    DBGSTR_CLIENT_NotDeadMember,
    DBGSTR_CLIENT_ChangePokeCmdInfo,
    DBGSTR_CLIENT_NumChangePokeBegin,
    DBGSTR_CLIENT_NumChangePokeResult,
    DBGSTR_CLIENT_PokeSelCnt,
    DBGSTR_CLIENT_PokeChangeIdx,
    DBGSTR_CLIENT_ForbidEscape_Kagefumi_Chk,
    DBGSTR_CLIENT_ForbidEscape_Kagefumi_Enable,
    DBGSTR_CLIENT_ForbidEscape_Arijigoku_Chk,
    DBGSTR_CLIENT_ForbidEscape_Arijigoku_Enable,
    DBGSTR_CLIENT_ForbidEscape_Jiryoku_Chk,
    DBGSTR_CLIENT_ForbidEscape_Jiryoku_Enable,
    DBGSTR_CLIENT_UpdateWazaProcResult,
    DBGSTR_CLIENT_CmdLimitTimeEnable,
    DBGSTR_CLIENT_CmdLimitTimeOver,
    DBGSTR_CLIENT_ForcePokeChange,
    DBGSTR_CLIENT_UpdateEnemyBaseHP,
    DBGSTR_CLIENT_ForceQuitByTimeLimit,
    DBGSTR_CLIENT_StartCmd,
    DBGSTR_CLIENT_RecvedQuitCmd,
    DBGSTR_CLIENT_ReplyToQuitCmd,
    DBGSTR_CLIENT_StartRotAct,
    DBGSTR_CLIENT_EndRotAct,
    DBGSTR_CLIENT_ReadRecAct,
    DBGSTR_CLIENT_ReadRecTimerOver,
    DBGSTR_CLIENT_ReadRecError,
    DBGSTR_CLIENT_ReadRecAct_Fight,
    DBGSTR_CLIENT_ReadRecAct_Change,
    DBGSTR_CLIENT_ReadRecAct_Move,
    DBGSTR_CLIENT_UnknownServerCmd,
    DBGSTR_CLIENT_RecPlayerBlackOut,
    DBGSTR_CLIENT_RecPlay_ChapterSkipped,
    DBGSTR_CLIENT_CHGAI_UkeTokStart,
    DBGSTR_CLIENT_CHGAI_UkeTokFind,
    DBGSTR_CLIENT_CHGAI_UkeTokDecide,
    DBGSTR_CLIENT_CHGAI_PreSortList,
    DBGSTR_CLIENT_CHGAI_AfterSortList,
    DBGSTR_CLIENT_CHGAI_SortListVal,
    DBGSTR_CLIENT_CHGAI_HOROBI,
    DBGSTR_CLIENT_CHGAI_FusigiNaMamori,
    DBGSTR_CLIENT_CHGAI_NoEffWaza,
    DBGSTR_CLIENT_CHGAI_Kodawari,
    DBGSTR_CLIENT_CHGAI_UkeTok,
    DBGSTR_CLIENT_CHGAI_SizenKaifuku,
    DBGSTR_CLIENT_CHGAI_WazaEff,
    DBGSTR_CLIENT_ROT_R,
    DBGSTR_CLIENT_ROT_L,
    DBGSTR_CLIENT_ROT_Determine,
    DBGSTR_CLIENT_AI_PutPokeStart,
    DBGSTR_CLIENT_AI_PutPokeDecide,
    DBGSTR_CLIENT_CoverRotateDecide,
    DBGSTR_CLIENT_HPCheckByLvup,
    DBGSTR_CLIENT_OP_HPPlus,
    DBGSTR_CLIENT_OP_MemberIn,
    DBGSTR_CLIENT_StartWazaTargetSel,
    DBGSTR_CLIENT_MoveAction,
    DBGSTR_CLIENT_StartWazaEffect,

    DBGSTR_BPP_NemuriWakeCheck,
    DBGSTR_BPP_WazaLinkOff,
    DBGSTR_BPP_WazaRecord,
    DBGSTR_BPP_BatonTouch,

    DBGSTR_MAIN_SwapPartyMember,
    DBGSTR_MAIN_PartyDataNotifyComplete,
    DBGSTR_MAIN_AIPartyDataSendComplete,
    DBGSTR_MAIN_SendAIPartyStart,
    DBGSTR_MAIN_SendAIPartyParam,
    DBGSTR_MAIN_RecvedMultiAITrainer,
    DBGSTR_MAIN_GetFrontPokeData,
    DBGSTR_MAIN_GetFrontPokeDataResult,
    DBGSTR_MAIN_CommError,
    DBGSTR_MAIN_Illusion1st,
    DBGSTR_MAIN_Illusion2nd,

    DBGSTR_NET_Startup,
    DBGSTR_NET_DecideClientID,
    DBGSTR_NET_SendSucceed,
    DBGSTR_NET_RecvedServerParam,
    DBGSTR_NET_RecvedPartyData,
    DBGSTR_NET_PartyDataComplete,
    DBGSTR_NET_PerappVoiceComplete,
    DBGSTR_NET_SendCmdDone,
    DBGSTR_NET_RecvedAllClientsData,
    DBGSTR_NET_ReturnToServerTrying,
    DBGSTR_NET_RecvedClientData,
    DBGSTR_NET_RecvedServerVersion,
    DBGSTR_NET_ServerDetermine,
    DBGSTR_NET_SendAITrainerData,
    DBGSTR_NET_RecvAITrainerData,
    DBGSTR_NET_AIPartyInfo,
    DBGSTR_NET_CreateAIPartyRecvBuffer,
    DBGSTR_NET_RecvedAIPartyData,
    DBGSTR_NET_PrappVoiceGetBufDisable,
    DBGSTR_NET_PrappVoiceGetBufEnable,
    DBGSTR_NET_PerappVoiceCheckRaw,
    DBGSTR_NET_PerappVoiceRecvedEnable,
    DBGSTR_NET_PerappVoiceRecvedDisable,

    DBGSTR_HEM_Push,
    DBGSTR_HEM_Pop,
    DBGSTR_HEM_PushWork,
    DBGSTR_HEM_PophWork,

    DBGSTR_CALCDMG_BaseDamage,
    DBGSTR_CALCDMG_WazaParam,
    DBGSTR_CALCDMG_RangeHosei,
    DBGSTR_CALCDMG_WeatherHosei,
    DBGSTR_CALCDMG_RAW_DAMAGE,
    DBGSTR_CALCDMG_Critical,
    DBGSTR_CALCDMG_RandomHosei,
    DBGSTR_CALCDMG_TypeMatchHosei,
    DBGSTR_CALCDMG_TypeAffInfo,
    DBGSTR_CALCDMG_DamageResult,
    DBGSTR_CALCDMG_DamageMarume,
    DBGSTR_STR_StdStrID,
    DBGSTR_STR_SetStrID,
    DBGSTR_STR_StdStrInfo,
    DBGSTR_STR_Arg,
    DBGSTR_STR_SetPoke1,
    DBGSTR_STR_SetPoke2,
    DBGSTR_PRINT_BufIdx,
    DBGSTR_STR_TagSetNumber,
    DBGSTR_STR_SetPokeNickname,
    DBGSTR_STR_SetPokeNicknameTruth,
    DBGSTR_STR_SetTagWazaName,
    DBGSTR_STR_SetItemName,
    DBGSTR_SVFL_AtkPower,
    DBGSTR_SVFL_DefGuard,
    DBGSTR_SVFL_WazaPower,
    DBGSTR_SVFL_UncategoryWazaInfo,
    DBGSTR_SVFL_QueWritePtr,
    DBGSTR_SVFL_ActOrderStart,
    DBGSTR_SVFL_ActOrderEnd,
    DBGSTR_SVFL_ActOrder_Fight,
    DBGSTR_SVFL_ActOrder_Item,
    DBGSTR_SVFL_ActOrder_Change,
    DBGSTR_SVFL_ActOrder_Escape,
    DBGSTR_SVFL_ActOrder_Dead,
    DBGSTR_SVFL_ActOrder_Rotation,
    DBGSTR_SVFL_CorrectTarget_Info,
    DBGSTR_SVFL_CorrectResult,
    DBGSTR_SVFL_CorrectHitFarOn,
    DBGSTR_SVFL_CorrectHitFarOff,
    DBGSTR_SVFL_CorrectTargetNum,
    DBGSTR_SVFL_CorrectTargetDiff,
    DBGSTR_SVFL_CorrectTargetHP,
    DBGSTR_SVFL_ClientPokeStillAlive,
    DBGSTR_SVFL_ClientPokeDeadAll,
    DBGSTR_SVFL_PosEffAdd,
    DBGSTR_SVFL_PosEffDupFail,
    DBGSTR_SVFL_HitCheckInfo1,
    DBGSTR_SVFL_HitCheckInfo2,
    DBGSTR_SVFL_HitCheckInfo3,
    DBGSTR_SVFL_HitCheckInfo4,
    DBGSTR_SVFL_StartAfterPokeChange,
    DBGSTR_SVFL_PokeChangeRootInfo,
    DBGSTR_SVFL_UseItemCall,
    DBGSTR_SVFL_ChangePokeReqInfo,
    DBGSTR_SVFL_TurnStart_Result,
    DBGSTR_SVFL_ActOrderMainStart,
    DBGSTR_SVFL_ActOrderMainDropOut,
    DBGSTR_SVFL_ExpCalc_Base,
    DBGSTR_SVFL_ExpCalc_MetInfo,
    DBGSTR_SVFL_ExpCalc_DivideInfo,
    DBGSTR_SVFL_ExpCalc_Result,
    DBGSTR_SVFL_ExpAdjustCalc,
    DBGSTR_SVFL_RecDataSendComped,
    DBGSTR_SVFL_SendServerCmd,
    DBGSTR_SVFL_AllClientCmdPlayComplete,
    DBGSTR_SVFL_GotoQuit,
    DBGSTR_SVFL_ReqWazaCallActOrder,
    DBGSTR_SVFL_WazaExeFail_1,
    DBGSTR_SVFL_WazaExeFail_2,
    DBGSTR_SVFL_HandEx_AddSick,
    DBGSTR_SVFL_CombiWazaCheck,
    DBGSTR_SVFL_CombiWazaFound,
    DBGSTR_SVFL_CombiDecide,
    DBGSTR_SVFL_StartAfterPokeIn,
    DBGSTR_SVFL_AfterPokeIn_Alive,
    DBGSTR_SVFL_AfterPokeIn_Dead,
    DBGSTR_SVFL_PokeDead,
    DBGSTR_SVFL_AddSickFailCode,
    DBGSTR_SVFL_AddSickFixed,
    DBGSTR_SVFL_WazaExeStart,
    DBGSTR_SVFL_ActionSortInfo,
    DBGSTR_SVFL_ActionSortRotation,
    DBGSTR_SVFL_ActionSortGen,
    DBGSTR_SVFL_TrainerItemTarget,
    DBGSTR_SVFL_DoubleTargetIntr,
    DBGSTR_SVFL_DoubleTargetRegister,
    DBGSTR_SVFL_DeadAlready,
    DBGSTR_SVFL_DeadDiffLevelCheck,
    DBGSTR_SVFL_HandExRelive,
    DBGSTR_SVFL_PutWazaEffect,
    DBGSTR_SVFL_WazaDmgCmd,
    DBGSTR_SVFL_ActIntr,
    DBGSTR_SVFL_RelivePokeStart,
    DBGSTR_SVFL_ReliveEndIn,
    DBGSTR_SVFL_ReliveEndIgnore,
    DBGSTR_SVFL_RankEffCore,
    DBGSTR_SVFL_IntrTargetPoke,
    DBGSTR_SVFL_CheckItemReaction,
    DBGSTR_SVFL_MagicCoatStart,
    DBGSTR_SVFL_MagicCoatReflect,
    DBGSTR_SVFL_YokodoriDetermine,
    DBGSTR_SVFL_YokodoriExe,
    DBGSTR_SVFL_YokodoriInfo,
    DBGSTR_SVFL_HandExRoot,
    DBGSTR_SVFL_HandExContFail,
    DBGSTR_SVFL_HandExContDead,
    DBGSTR_SVFL_HandExSetResult,
    DBGSTR_SVFL_HandExTokChangeAdd,
    DBGSTR_SVFL_HandExTokChangeEventCall,
    DBGSTR_SVFL_HandExTokChangeEventCallEnd,
    DBGSTR_SVFL_MemberInEventBegin,
    DBGSTR_SVFL_MemberInEventEnd,
    DBGSTR_SVFS_RegTargetDouble,
    DBGSTR_SVFS_Result_RestCnt,
    DBGSTR_SVFS_LastPokeID,
    DBGSTR_SVFL_DeadPokeCount,
    DBGSTR_SVFL_HPRatio,

    DBGSTR_CALC_EscCheckMyTeam,
    DBGSTR_CALC_EscInfoAdd,
    DBGSTR_CALC_EscCheckEnemy,
    DBGSTR_CALC_EscResultBoth1,
    DBGSTR_CALC_EscResultBoth2,
    DBGSTR_CALC_EscResultLose,
    DBGSTR_CALC_EscResultWin,

    DBGSTR_SC_PutCmd,
    DBGSTR_SC_ReservedPos,
    DBGSTR_SC_WriteReservedPos,
    DBGSTR_SC_ReservedInfo,
    DBGSTR_SC_PutMsgParam,
    DBGSTR_SC_ReadMsgParam,
    DBGSTR_SC_PutMsg_SE,
    DBGSTR_SC_ReadMsg_SE,
    DBGSTR_SC_ArgsEqual,
    DBGSTR_SC_WPEqual,
    DBGSTR_SC_RPEqual,
    DBGSTR_SC_ReadCmd,
    DBGSTR_SC_ReserveCmd,

    DBGSTR_SERVER_SendShooterChargeCmd,
    DBGSTR_SERVER_ShooterChargeCmdDoneAll,
    DBGSTR_SERVER_SendActionSelectCmd,
    DBGSTR_SERVER_ActionSelectDoneAll,
    DBGSTR_SERVER_FlowResult,
    DBGSTR_SV_ChangePokeOnTheTurn,
    DBGSTR_SV_PokeInReqForEmptyPos,
    DBGSTR_SV_StartChangePokeInfo,
    DBGSTR_SV_SendQuitACmad,
    DBGSTR_SV_ReplyQuitACmad,
    DBGSTR_SV_RestoreAction,
    DBGSTR_SV_CmdCheckOK,
    DBGSTR_SV_CmdCheckNG,
    DBGSTR_SV_SendBtlInRecord,
    DBGSTR_SV_SendActRecord,
    DBGSTR_SV_SendActRecordFailed,
    DBGSTR_SV_ExitBattle,
    DBGSTR_SV_CommError,

    DBGSTR_EVENT_AddFactorInfo,
    DBGSTR_EVENT_RmvFactorCalling,
    DBGSTR_EVENT_CallFactorStart,
    DBGSTR_EVENT_CheckFactorStart,
    DBGSTR_EVENT_CheckFactorTbl,
    DBGSTR_EVENT_CallFactorEnd,
    DBGSTR_EVENT_SkipByNewComerFlg,
    DBGSTR_EVENT_SkipByCallingFlg,
    DBGSTR_EVENT_SkipByStackLayer,
    DBGSTR_EVENT_ClearNewComFlag,
    DBGSTR_EVENT_SkipByCheckHandler,
    DBGSTR_EVENT_NextFactorInfo,
    DBGSTR_EVENT_NextFactorInfo2,
    DBGSTR_EVENT_AddFactor_Case0,
    DBGSTR_EVENT_AddFactor_Case1,
    DBGSTR_EVENT_AddFactor_Case2,
    DBGSTR_EVENT_AddFactor_Case3,
    DBGSTR_EV_AddFactor,
    DBGSTR_EV_DelFactor,
    DBGSTR_EV_DelReserveFactor,
    DBGSTR_EV_LinkInfoHeader,
    DBGSTR_EV_LinkInfoFooder,
    DBGSTR_EV_LinkPtr,
    DBGSTR_EV_LinkEmpty,
    DBGSTR_EVAR_Push,
    DBGSTR_EVAR_Pop,

    DBGSTR_SIDE_AddFirst,
    DBGSTR_SIDE_NoMoreAdd,
    DBGSTR_PSET_Copy,
    DBGSTR_PSET_Add,
    DBGSTR_PSET_Remove,

    DBGSTR_FIDLD_FuinCheck,
    DBGSTR_FIELD_FuinHit,

    DBGSTR_SICK_ContProc,

    DBGSTR_POKESET_RemoveDeadPoke,

    DBGSTR_VCORE_PokeListStart,
    DBGSTR_VCORE_SelPokeEnd,
    DBGSTR_VCORE_SelPokeEnd_Sel,
    DBGSTR_VCORE_SelPokeEnd_Unsel,
    DBGSTR_VCORE_PluralDamageInfo,
    DBGSTR_VCORE_PluralDamagePoke,
    DBGSTR_SCU_RelivePokeAct,
    DBGSTR_SCD_DecideTargetPos,
    DBGSTR_SCD_TargetHitPosConvert,
    DBGSTR_SCD_TargetHitPosFix,

    DBGSTR_POSPOKE_Out,
    DBGSTR_POSPOKE_In,
    DBGSTR_DEADREC_Add,

    DBGSTR_Item_PinchReactItem,
    DBGSTR_Item_PinchReactOn,
    DBGSTR_HANDWAZA_CombiWazaExe,
    DBGSTR_HANDWAZA_AlreadyRegistered,
    DBGSTR_HANDWAZA_YokodoriDecide,
    DBGSTR_HANDITEM_OujaCheck,
    DBGSTR_HANDITEM_OujaEffective,
    DBGSTR_HANDTOK_TRACE_Add,
    DBGSTR_HANDTOK_PressureIn,

    DBGSTR_REC_ReadActStart,
    DBGSTR_REC_ReadActSkip,
    DBGSTR_REC_SeekClient,
    DBGSTR_REC_ReadActParam,
    DBGSTR_REC_CheckMatchClient,
    DBGSTR_REC_ReadTimeOverCmd,
    DBGSTR_REC_TimeOverCmdWrite,
    DBGSTR_REC_CheckReadComp,


    DBGSTR_line,
    DBGSTR_csv,
    DBGSTR_done,
    DBGSTR_LF,

  }BtlDebugStrID;

  //--------------------------------------------------------------------------
  /**
   *  文字列ID指定方式
   */
  //--------------------------------------------------------------------------
  enum {
    PRINT_CHANNEL_PRESSURE = 0,
    PRINT_CHANNEL_EVENTSYS = 4,
    PRINT_CHANNEL_HEMSYS = 3,
    PRINT_CHANNEL_RECTOOL = 4,
  };

  #if PM_DEBUG
//    #define BTL_PRINT_SYSTEM_ENABLE
  #endif


  #ifdef BTL_PRINT_SYSTEM_ENABLE
    void BTL_UTIL_PRINTSYS_Init( void );
    void BTL_UTIL_PRINTSYS_Quit( void );
    void BTL_DEBUGPRINT_Ctrl( void );
    void BTL_UTIL_SetPrintType( BtlPrintType type );
    void BTL_UTIL_Printf( const char* filename, int line, u32 channel, u32 strID, ... );
    void BTL_UTIL_PrintfSimple( const char* filename, u32 channel, u32 strID, ... );
    void BTL_UTIL_DumpPrintf( const char* caption, const void* data, u32 size );
    #define BTL_N_Printf( ... ) BTL_UTIL_Printf( __FILE__, __LINE__, 1, __VA_ARGS__ )
    #define BTL_N_PrintfEx( ch, ... )  if( ch!=0 ){ BTL_UTIL_Printf( __FILE__, __LINE__, ch, __VA_ARGS__ ); }
    #define BTL_N_PrintfSimple( ... ) BTL_UTIL_PrintfSimple( __FILE__, 1, __VA_ARGS__ )
    #define BTL_N_PrintfSimpleEx( ch, ... ) if( ch!=0 ){ BTL_UTIL_PrintfSimple( __FILE__, ch, __VA_ARGS__ ); }

  #else // #ifdef BTL_PRINT_SYSTEM_ENABLE
    #define BTL_UTIL_PRINTSYS_Init()         /* */
    #define BTL_UTIL_PRINTSYS_Quit()         /* */
    #define BTL_DEBUGPRINT_Ctrl()            /* */
    #define BTL_UTIL_SetPrintType(t)         /* */
    #define BTL_N_Printf( ... )              /* */
    #define BTL_N_PrintfEx( flg, ... )       /* */
    #define BTL_N_PrintfSimple( ... )        /* */
    #define BTL_N_PrintfSimpleEx( flg, ... ) /* */
  #endif  // #ifdef BTL_PRINT_SYSTEM_ENABLE

GFL_NAMESPACE_END(btl)



#endif // __BTL_DEBUGPRINT_H__
