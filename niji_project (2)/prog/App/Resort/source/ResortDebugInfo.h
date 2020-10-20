#if !defined(NIJI_PROJECT_APP_RESORT_RESORTDEBUGINFO_H_INCLUDED)
#define NIJI_PROJECT_APP_RESORT_RESORTDEBUGINFO_H_INCLUDED
#pragma once




#define  NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE  (PM_DEBUG&&(0))  // コミットするときは必ず0で




#if  NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE


//==============================================================================
/**
 @file    ResortDebugInfo.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.03.25
 @brief   ポケリゾートのデバッグに役立つ情報を扱うクラス
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)


//! @brief  ポケリゾートのデバッグに役立つ情報を扱うクラス
class ResortDebugInfo
{
  GFL_FORBID_COPY_AND_ASSIGN(ResortDebugInfo);
public:
  enum
  {
    /* a 000 */ TIME_PHASE_RESORT_PROC_CONSTRUCTOR_START ,
    /* b 001 */ TIME_PHASE_RESORT_PROC_DEBUG_WIN_START ,
    /* b 002 */ TIME_PHASE_RESORT_PROC_DEBUG_WIN_END ,
    /* a 003 */ TIME_PHASE_RESORT_DATA_MANAGER_START ,
    /* a 004 */ TIME_PHASE_RESORT_DATA_MANAGER_RESORT_PACK_LOAD_END ,
    /* a 005 */ TIME_PHASE_RESORT_DATA_MANAGER_POKEICON_OPEN_LOAD_END ,
    /* b 006 */ TIME_PHASE_RESORT_PROC_RENDERING_MANAGER_CREATE_START ,
    /* b 007 */ TIME_PHASE_RESORT_PROC_RENDERING_MANAGER_CREATE_END ,
    /* a 008 */ TIME_PHASE_RESORT_PROC_FILE_OPEN_START ,
    /* a 009 */ TIME_PHASE_RESORT_PROC_FILE_LOAD_END ,
    /* b 010 */ TIME_PHASE_RESORT_PROC_ISLANDGROUND_LV_PACK_LOAD_START ,
    /* b 011 */ TIME_PHASE_RESORT_PROC_ISLANDGROUND_LV_PACK_LOAD_END ,
    /* a 012 */ TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_OPEN_START ,
    /* a 013 */ TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_OPEN_END ,
    /* b 014 */ TIME_PHASE_RESORT_PROC_FILE_SYNC_OPEN_START ,
    /* b 015 */ TIME_PHASE_RESORT_PROC_FILE_SYNC_CLOSE_END ,
    /* a 016 */ TIME_PHASE_RESORT_FRAME_FILE_OPEN_START ,
    /* a 017 */ TIME_PHASE_RESORT_FRAME_FILE_CLOSE_END ,
    /* b 018 */ TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_LOAD_START ,
    /* b 019 */ TIME_PHASE_RESORT_DRAW_POKEICON_3D_FILE_LOAD_END ,
    /* a 020 */ TIME_PHASE_RESORT_CHARA_FILE_OPEN_START ,
    /* a 021 */ TIME_PHASE_RESORT_CHARA_FILE_OPEN_END ,
    /* b 022 */ TIME_PHASE_RESORT_CHARA_FILE_LOAD_START ,
    /* b 023 */ TIME_PHASE_RESORT_CHARA_FILE_LOAD_END ,
    /* a 024 */ TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_OPEN_START ,
    /* a 025 */ TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_OPEN_END ,
    /* b 026 */ TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_LOAD_START ,
    /* b 027 */ TIME_PHASE_RESORT_POKE_MODEL_SYSTEM_FILE_LOAD_END ,
    /* a 028 */ TIME_PHASE_RESORT_INIT_LOGIC_INIT_START ,
    /* a 029 */ TIME_PHASE_RESORT_INIT_LOGIC_INIT_END ,
    /* b 030 */ TIME_PHASE_RESORT_FADE_IN_START ,
    /* a 031 */ TIME_PHASE_RESORT_STEP_A_00 ,
    /* a 032 */ TIME_PHASE_RESORT_STEP_A_01 ,
    /* a 033 */ TIME_PHASE_RESORT_STEP_A_02 ,
    /* a 034 */ TIME_PHASE_RESORT_STEP_A_03 ,
    /* a 035 */ TIME_PHASE_RESORT_STEP_A_04 ,
    /* a 036 */ TIME_PHASE_RESORT_STEP_A_05 ,
    /* a 037 */ TIME_PHASE_RESORT_STEP_A_06 ,
    /* a 038 */ TIME_PHASE_RESORT_STEP_A_07 ,
    /* a 039 */ TIME_PHASE_RESORT_STEP_A_08 ,
    /* a 040 */ TIME_PHASE_RESORT_STEP_A_09 ,
    /* b 041 */ TIME_PHASE_RESORT_STEP_B_00 ,
    /* b 042 */ TIME_PHASE_RESORT_STEP_B_01 ,
    /* b 043 */ TIME_PHASE_RESORT_STEP_B_02 ,
    /* b 044 */ TIME_PHASE_RESORT_STEP_B_03 ,
    /* b 045 */ TIME_PHASE_RESORT_STEP_B_04 ,
    /* b 046 */ TIME_PHASE_RESORT_STEP_B_05 ,
    /* b 047 */ TIME_PHASE_RESORT_STEP_B_06 ,
    /* b 048 */ TIME_PHASE_RESORT_STEP_B_07 ,
    /* b 049 */ TIME_PHASE_RESORT_STEP_B_08 ,
    /* b 050 */ TIME_PHASE_RESORT_STEP_B_09 ,
    /* b 051 */ TIME_PHASE_RESORT_STEP_B_10 ,
    /* b 052 */ TIME_PHASE_RESORT_STEP_B_11 ,
    /* b 053 */ TIME_PHASE_RESORT_STEP_B_12 ,
    /* b 054 */ TIME_PHASE_RESORT_STEP_B_13 ,
    /* b 055 */ TIME_PHASE_RESORT_STEP_B_14 ,
    /* b 056 */ TIME_PHASE_RESORT_STEP_B_15 ,
    /* b 057 */ TIME_PHASE_RESORT_STEP_B_16 ,
    /* b 058 */ TIME_PHASE_RESORT_STEP_B_17 ,
    /* b 059 */ TIME_PHASE_RESORT_STEP_B_18 ,
    /* b 060 */ TIME_PHASE_RESORT_STEP_B_19 ,
    /* c 061 */ TIME_PHASE_RESORT_STEP_C_00 ,
    /* c 062 */ TIME_PHASE_RESORT_STEP_C_01 ,
    /* c 063 */ TIME_PHASE_RESORT_STEP_C_02 ,
    /* c 064 */ TIME_PHASE_RESORT_STEP_C_03 ,
    /* c 065 */ TIME_PHASE_RESORT_STEP_C_04 ,
    /* c 066 */ TIME_PHASE_RESORT_STEP_C_05 ,
    /* c 067 */ TIME_PHASE_RESORT_STEP_C_06 ,
    /* c 068 */ TIME_PHASE_RESORT_STEP_C_07 ,
    /* c 069 */ TIME_PHASE_RESORT_STEP_C_08 ,
    /* c 070 */ TIME_PHASE_RESORT_STEP_C_09 ,
    /* c 071 */ TIME_PHASE_RESORT_STEP_C_10 ,
    /* c 072 */ TIME_PHASE_RESORT_STEP_C_11 ,
    /* c 073 */ TIME_PHASE_RESORT_STEP_C_12 ,
    /* c 074 */ TIME_PHASE_RESORT_STEP_C_13 ,
    /* c 075 */ TIME_PHASE_RESORT_STEP_C_14 ,
    /* c 076 */ TIME_PHASE_RESORT_STEP_C_15 ,
    /* c 077 */ TIME_PHASE_RESORT_STEP_C_16 ,
    /* c 078 */ TIME_PHASE_RESORT_STEP_C_17 ,
    /* c 079 */ TIME_PHASE_RESORT_STEP_C_18 ,
    /* c 080 */ TIME_PHASE_RESORT_STEP_C_19 ,
    /* c 081 */ TIME_PHASE_RESORT_STEP_C_20 ,
    /* c 082 */ TIME_PHASE_RESORT_STEP_C_21 ,
    /* c 083 */ TIME_PHASE_RESORT_STEP_C_22 ,
    /* c 084 */ TIME_PHASE_RESORT_STEP_C_23 ,
    /* c 085 */ TIME_PHASE_RESORT_STEP_C_24 ,
    /* c 086 */ TIME_PHASE_RESORT_STEP_C_25 ,
    /* c 087 */ TIME_PHASE_RESORT_STEP_C_26 ,
    /* c 088 */ TIME_PHASE_RESORT_STEP_C_27 ,
    /* c 089 */ TIME_PHASE_RESORT_STEP_C_28 ,
    /* c 090 */ TIME_PHASE_RESORT_STEP_C_29 ,
    /* c 091 */ TIME_PHASE_RESORT_STEP_C_30 ,
    /* c 092 */ TIME_PHASE_RESORT_STEP_C_31 ,
    /* c 093 */ TIME_PHASE_RESORT_STEP_C_32 ,
    /* c 094 */ TIME_PHASE_RESORT_STEP_C_33 ,
    /* c 095 */ TIME_PHASE_RESORT_STEP_C_34 ,
    /* c 096 */ TIME_PHASE_RESORT_STEP_C_35 ,
    /* c 097 */ TIME_PHASE_RESORT_STEP_C_36 ,
    /* c 098 */ TIME_PHASE_RESORT_STEP_C_37 ,
    /* c 099 */ TIME_PHASE_RESORT_STEP_C_38 ,
    /* c 100 */ TIME_PHASE_RESORT_STEP_C_39 ,
    /* d 101 */ TIME_PHASE_RESORT_STEP_D_00 ,
    /* d 102 */ TIME_PHASE_RESORT_STEP_D_01 ,
    /* d 103 */ TIME_PHASE_RESORT_STEP_D_02 ,
    /* d 104 */ TIME_PHASE_RESORT_STEP_D_03 ,
    /* d 105 */ TIME_PHASE_RESORT_STEP_D_04 ,
    /* d 106 */ TIME_PHASE_RESORT_STEP_D_05 ,
    /* d 107 */ TIME_PHASE_RESORT_STEP_D_06 ,
    /* d 108 */ TIME_PHASE_RESORT_STEP_D_07 ,
    /* d 109 */ TIME_PHASE_RESORT_STEP_D_08 ,
    /* d 110 */ TIME_PHASE_RESORT_STEP_D_09 ,
    /* e 111 */ TIME_PHASE_RESORT_STEP_E_00 ,
    /* e 112 */ TIME_PHASE_RESORT_STEP_E_01 ,
    /* e 113 */ TIME_PHASE_RESORT_STEP_E_02 ,
    /* e 114 */ TIME_PHASE_RESORT_STEP_E_03 ,
    /* e 115 */ TIME_PHASE_RESORT_STEP_E_04 ,
    /* e 116 */ TIME_PHASE_RESORT_STEP_E_05 ,
    /* e 117 */ TIME_PHASE_RESORT_STEP_E_06 ,
    /* e 118 */ TIME_PHASE_RESORT_STEP_E_07 ,
    /* e 119 */ TIME_PHASE_RESORT_STEP_E_08 ,
    /* e 120 */ TIME_PHASE_RESORT_STEP_E_09 ,

    TIME_PHASE_RESORT_MAX ,                                                               
  };

  enum
  {
    HEAP_DEF_RESORT_00,
    HEAP_DEF_RESORT_01,
    HEAP_DEF_RESORT_02,
    HEAP_DEF_RESORT_03,
    HEAP_DEF_RESORT_04,
    HEAP_DEF_RESORT_05,
    HEAP_DEF_RESORT_06,
    HEAP_DEF_RESORT_07,
    HEAP_DEF_RESORT_08,
    HEAP_DEF_RESORT_09,
    HEAP_DEF_RESORT_10,
    HEAP_DEF_RESORT_11,
    HEAP_DEF_RESORT_12,
    HEAP_DEF_RESORT_13,
    HEAP_DEF_RESORT_14,
    HEAP_DEF_RESORT_15,
    HEAP_DEF_RESORT_16,
    HEAP_DEF_RESORT_17,
    HEAP_DEF_RESORT_18,
    HEAP_DEF_RESORT_19,

    HEAP_DEF_RESORT_MAX,
  };

public:
  static ResortDebugInfo s_ResortDebugInfo;

public:
  ResortDebugInfo(void);
  virtual ~ResortDebugInfo() {}
  void Init(void);
  void SetCurrTime(u32 index);
  void SetCurrTimeOnce(u32 index);
  s64 GetTime(u32 index) const;
  void SetHeap(u32 index, gfl2::heap::HeapBase* heap, const char* heapName);  // 無効にするときはNULLを渡して下さい。  // heapに対してはDumpしかしません。
  void SetUserVal(s32 val);
  s32  GetUserVal(void) const;
  void PrintTime(void) const;
  void ReleasePrintTime(void) const;
  void PrintHeap(void) const;
  void Update(void);

private:
  static const u32 TIME_NUM_MAX = 128;
  static const u32 HEAP_NUM_MAX = 32;

private:
  s64                    m_time[TIME_NUM_MAX];  // [micro second]
  gfl2::heap::HeapBase*  m_heap[HEAP_NUM_MAX];  // heapに対してはDumpしかしません。
  const char*            m_heapName[HEAP_NUM_MAX];
  s32                    m_userVal;
};


GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif  // NIJI_PROJECT_APP_RESORT_DEBUG_INFO_ENABLE


#endif  // NIJI_PROJECT_APP_RESORT_RESORTDEBUGINFO_H_INCLUDED

