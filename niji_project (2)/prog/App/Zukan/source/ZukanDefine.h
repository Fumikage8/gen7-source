#if !defined(NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_H_INCLUDED)
#define NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ZukanDefine.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2015.06.10
 @brief   図鑑の定義
 */
//==============================================================================


////////////////////////////////////////////////////////////////////////////////
//
// 開発中のデバッグモード
//
////////////////////////////////////////////////////////////////////////////////

// これが1のとき、ポケモンモデルのモデル違い一覧をデバッグ出力する。
#define  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_POKE_MODEL_ALL    ((PM_DEBUG)&&(0))  // 必ず0でコミットすること

// これが1のとき、図鑑のメモリ使用状況をデバッグ出力する。
#define  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_PRINT_DUMP_MEM          ((PM_DEBUG)&&(0))  // 必ず0でコミットすること

// これが1のとき、図鑑のデバッグ情報を保持する。
#define  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_INFO_ENABLE             ((PM_DEBUG)&&(0))  // 必ず0でコミットすること

// これが1のとき、「分布データがFixしておらずASSERTに引っ掛かる」というのを回避する。
#define  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_MAP_IGNORE_ASSERT       (0)  // 製品では必ず0でコミットだが、開発中は1でいい。

// これが1のとき、その他開発向けmodeが有効になる
#define  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_OTHER_DEV_MODE_ENABLE    ((PM_DEBUG)&&(1))  // PM_DEBUGの0/1に合わせて無効/有効が切り替わればいい

// これが1のとき、図鑑登録画面でポケモンを切り替えられる
#define  NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_DEBUG_REGISTER_POKE_CHANGE_ENABLE    ((PM_DEBUG)&&(0))  // 必ず0でコミットすること



////////////////////////////////////////////////////////////////////////////////
//
// 検討中の機能
//
////////////////////////////////////////////////////////////////////////////////

// これが1のとき、図鑑のキラカードが有効になる。
#define NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_PRISM_SHEET_ENABLE             (1)  // 採用されれば1、採用されなければ0  →  採用されたので必ず1でコミットすること




GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(zukan)




////////////////////////////////////////////////////////////////////////////////
//
// オブジェクトサイズ ZUKAN_OBJ_SIZE_
//
////////////////////////////////////////////////////////////////////////////////

static const s32 ZUKAN_OBJ_SIZE_HEAP_MANAGE   =   0x100;  // ヒープ管理領域のサイズ
static const s32 ZUKAN_OBJ_SIZE_IMPL          = 0x10000;  // implのサイズ


////////////////////////////////////////////////////////////////////////////////
//
// メモリサイズ ZUKAN_MEM_SIZE_
//
////////////////////////////////////////////////////////////////////////////////

/*
HEAPID_APP_DEVICE
    +-- PROC_HEAP
        +-- IMPL_HEAP
            +-- COMMON_HEAP
            +-- INSTANCE_HEAP
                +-- 「画面ごとに使用するメモリ」
    +-- PROC_DEV
        +-- IMPL_DEV
            +-- INPL_INSTANCE_DEV
            +-- DATA_DEV
            +-- FRAME_MANAGER_DEV
            +-- VRAM_ALLOCATOR_DEV
            +-- COMMON_DEV
            +-- LANG_MSG_DATA_DEV
            +-- LANG_FONT_DEV
            +-- INSTANCE_DEV
                +-- 「画面ごとに使用するメモリ」
    +-- STR_INPUT_DEV
*/



// ZUKAN_MEM_SIZE_VRAM_ALLOCATOR_DEV_MEM用のプラス分
#if defined(GF_PLATFORM_CTR)
static const s32 ZUKAN_MEM_SIZE_PLUS_FOR_WIN32  = 0;
#elif defined(GF_PLATFORM_WIN32)
static const s32 ZUKAN_MEM_SIZE_PLUS_FOR_WIN32  = 0x80000+ZUKAN_OBJ_SIZE_HEAP_MANAGE;  // Win32ではメモリがVRAMではなくヒープから取られる。(512*256*4)の画像1枚分プラスしておく。
#endif  // GF_PLATFORM_



// procのメモリサイズ ZUKAN_MEM_SIZE_PROC_
// 親はHEAPID_APP_DEVICE、HEAPID_APP_DEVICE（どちらもデバイスメモリ）
// 一番大元のメモリ
static const s32 ZUKAN_MEM_SIZE_PROC_HEAP_MEM   =    0x1000;
static const s32 ZUKAN_MEM_SIZE_PROC_DEV_MEM    = 0x19B2B00;

static const s32 ZUKAN_REGISTER_MEM_SIZE_PROC_HEAP_MEM =    0x1000;
static const s32 ZUKAN_REGISTER_MEM_SIZE_PROC_DEV_MEM  = 0x14DB700;



// implのメモリ ZUKAN_MEM_SIZE_IMPL_
// 親はZUKAN_MEM_SIZE_PROC_
// proc内で確保する以外のものは全てここから確保する
// ZUKAN_MEM_SIZE_PROC_めいっぱい確保する
static const s32 ZUKAN_MEM_SIZE_IMPL_HEAP_MEM   = 0x68000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;  // m_heapMemではなくm_devMemからヒープメモリを確保することにした。
static const s32 ZUKAN_MEM_SIZE_IMPL_DEV_MEM    = ZUKAN_MEM_SIZE_PROC_DEV_MEM - ( ZUKAN_MEM_SIZE_IMPL_HEAP_MEM + ZUKAN_OBJ_SIZE_HEAP_MANAGE ) - ZUKAN_OBJ_SIZE_IMPL - ZUKAN_OBJ_SIZE_HEAP_MANAGE;

static const s32 ZUKAN_REGISTER_MEM_SIZE_IMPL_HEAP_MEM   = 0x68000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;  // m_heapMemではなくm_devMemからヒープメモリを確保することにした。
static const s32 ZUKAN_REGISTER_MEM_SIZE_IMPL_DEV_MEM    = ZUKAN_REGISTER_MEM_SIZE_PROC_DEV_MEM - ( ZUKAN_REGISTER_MEM_SIZE_IMPL_HEAP_MEM + ZUKAN_OBJ_SIZE_HEAP_MANAGE ) - ZUKAN_OBJ_SIZE_IMPL - ZUKAN_OBJ_SIZE_HEAP_MANAGE;



// データのメモリ ZUKAN_MEM_SIZE_DATA_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_IMPL_INSTANCE_DEV_MEM   = 0x2000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;

// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_DATA_DEV_MEM           = 0x992500 - ZUKAN_OBJ_SIZE_HEAP_MANAGE - ZUKAN_MEM_SIZE_PLUS_FOR_WIN32;  // Win32版ではここのメモリが一番空くだろうから、ここから画像1枚分もらっておく。
static const s32 ZUKAN_REGISTER_MEM_SIZE_DATA_DEV_MEM  = 0x6F0000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE - ZUKAN_MEM_SIZE_PLUS_FOR_WIN32;

// frame_managerのメモリ ZUKAN_MEM_SIZE_FRAME_MANAGER_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_FRAME_MANAGER_DEV_MEM   = 0x1100 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;

// VRAMアロケータのメモリ ZUKAN_MEM_SIZE_VRAM_ALLOCATOR_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_VRAM_ALLOCATOR_DEV_MEM   = 0x800 - ZUKAN_OBJ_SIZE_HEAP_MANAGE + ZUKAN_MEM_SIZE_PLUS_FOR_WIN32;  // メモリはVRAMから取られるから少なくてよい。

// コンテンツのメモリ ZUKAN_MEM_SIZE_CONTENTS_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_CONTENTS_DEV_MEM   = 0x4D000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;

// 共通物のメモリ ZUKAN_MEM_SIZE_COMMON_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_COMMON_HEAP_MEM         =  0x20000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;
static const s32 ZUKAN_MEM_SIZE_COMMON_DEV_MEM          = 0x828100 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;  // ZUKAN_MEM_SIZE_POKEMODEL_COMP_DEV_MEMを減らしたら、ここに足して下さい。

// ポケモン3Dモデルの圧縮ファイルを読み込むのに使うメモリ ZUKAN_MEM_SIZE_POKEMODEL_COMP_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_POKEMODEL_COMP_DEV_MEM  = 0x201000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;

// 言語別MsgDataのメモリ ZUKAN_MEM_SIZE_LANG_MSG_DATA_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_LANG_MSG_DATA_DEV_MEM           = 0x10000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;
static const s32 ZUKAN_REGISTER_MEM_SIZE_LANG_MSG_DATA_DEV_MEM  = 0;  // REGISTERでは使わない

// 言語別Fontのメモリ ZUKAN_MEM_SIZE_LANG_FONT_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_LANG_FONT_DEV_MEM           = 0x1000 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;
static const s32 ZUKAN_REGISTER_MEM_SIZE_LANG_FONT_DEV_MEM  = 0;  // REGISTERでは使わない

// インスタンスのメモリ ZUKAN_MEM_SIZE_INSTANCE_
// 親はZUKAN_MEM_SIZE_IMPL_
static const s32 ZUKAN_MEM_SIZE_INSTANCE_HEAP_MEM          =  0x47800 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;
static const s32 ZUKAN_MEM_SIZE_INSTANCE_DEV_MEM           = 0x51CC00 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;
static const s32 ZUKAN_REGISTER_MEM_SIZE_INSTANCE_DEV_MEM  = 0x2F8D00 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;

// デバッグ用のメモリ ZUKAN_MEM_SIZE_DEBUG_
// 親はHEAPID_APP_DEVICE
static const s32 ZUKAN_MEM_SIZE_DEBUG_DEV_MEM   = 0x6C00 - ZUKAN_OBJ_SIZE_HEAP_MANAGE;  // デバッグ用メモリが空いているならそちらから取りたい。


// 余り
//ZUKAN_MEM_SIZE_IMPL_HEAP_MEMの余り(ZUKAN_MEM_SIZE_IMPL_HEAP_MEMから引く)
//      0x800 = 0x68000 - (0x20000+0x47800)
//ZUKAN_MEM_SIZE_IMPL_DEV_MEMの余り((ZUKAN_MEM_SIZE_PROC_DEV_MEM-ZUKAN_MEM_SIZE_IMPL_HEAP_MEM)から引く)
//    0x11000 = 0x19B2B00 - 0x68000 - (0x2000+0x992500+0x1100+0x800+0x4D000+0x828100+0x201000+0x10000+0x1000+0x51CC00)
//
//ZUKAN_REGISTER_MEM_SIZE_IMPL_HEAP_MEMの余り(ZUKAN_REGISTER_MEM_SIZE_IMPL_HEAP_MEMから引く)
//      0x800 = 0x68000 - (0x20000+0x47800)
//ZUKAN_REGISTER_MEM_SIZE_IMPL_DEV_MEMの余り((ZUKAN_REGISTER_MEM_SIZE_PROC_DEV_MEM-ZUKAN_REGISTER_MEM_SIZE_IMPL_HEAP_MEM)から引く)
//    0x11000 = 0x14DB700 - 0x68000 - (0x2000+0x6F0000+0x1100+0x800+0x4D000+0x828100+0x201000+      0+     0+0x2F8D00)



// 「図鑑を残したまま起動するもので使用するメモリ」
// StrInputのメモリ(Proc起動ではなくFrame起動)
//     niji_project/prog/App/StrInput/source/StrInputFrame.cpp  // #define STRINPUT_SYS_HEAP_SIZE ( 0x10300 )
//     niji_project/prog/App/StrInput/source/StrInputDraw.cpp   // static const u32 STRINPUT_APP_HEAP_SIZE = 768 * 1024;  // static const u32 STRINPUT_DEVICE_HEAP_SIZE  = 4 * 1024 * 1024;
//     親はHEAPID_APP_DEVICE
static const s32 ZUKAN_MEM_SIZE_STR_INPUT_DEV_MEM  = 0x4D0300;  // = 0x10300 + 0xC0000 + 0x400000;



////////////////////////////////////////////////////////////////////////////////
//
// 「画面ごとに使用するメモリ」のメモリサイズ ZUKAN_MEM_SIZE_
//
////////////////////////////////////////////////////////////////////////////////
// ポケアイコンのテクスチャのメモリ ZUKAN_MEM_SIZE_POKE_ICON_TEX_BUF_
static const s32 ZUKAN_MEM_SIZE_POKE_ICON_TEX_BUF_TEMP_MEM   = 0x1000;  // 一時的に使用するメモリのサイズ。m_heapMemではなくm_devMemからヒープメモリを確保することにした。
static const s32 ZUKAN_MEM_SIZE_POKE_ICON_TEX_BUF_DEV_MEM    = 0x1280;  // ポケアイコンのテクスチャの1個分に必要なメモリサイズ 


////////////////////////////////////////////////////////////////////////////////
//
// 定数 ZUKAN_CONST_
//
////////////////////////////////////////////////////////////////////////////////

// app::util::G2DUtilで作成するLytWkのIDで無効とみなす値。図鑑内でしか使わない。app::util::G2DUtilに実際に渡したりはしない。
static const u32 ZUKAN_CONST_G2DUTIL_LYT_WK_ID_NULL = 0xFFFF;
// app::tool::PaneListから渡されるpane_indexで無効とみなす値。図鑑内でしか使わない。app::tool::PaneListに実際に渡したりはしない。
static const u32 ZUKAN_CONST_PANE_LIST_PANE_INDEX_NULL = 0xFFFF;

// 無限ループ
static const u32 ZUKAN_CONST_ENDLESS_LOOP_COUNT_MAX_LONG = 2147483647-1;  // ループがこの回数を越えたら止める。




GFL_NAMESPACE_END(zukan)
GFL_NAMESPACE_END(app)


#endif  // NIJI_PROJECT_APP_ZUKAN_ZUKANDEFINE_H_INCLUDED

