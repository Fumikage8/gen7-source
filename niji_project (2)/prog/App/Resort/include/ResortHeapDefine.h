#if !defined(NIJI_PROJECT_APP_RESORT_RESORTHEAPDEFINE_H_INCLUDED)
#define NIJI_PROJECT_APP_RESORT_RESORTHEAPDEFINE_H_INCLUDED
#pragma once


//==============================================================================
/**
 @file    ResortHeapDefine.h
 @author  kawada_koji@gamefreak.co.jp
 @date    2016.04.16
 @brief   ポケリゾートのヒープ定義
 */
//==============================================================================


// gfl2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>


GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Resort)


class ResortHeapDefine
{
public: 
  // ファイル
  static const s32 RESORT_FILE_SIZE          = 0x600000;  // resort_pack.pack  // 親はHEAPID_APP_DEVICE
  static const s32 POKEICON_FILE_SIZE        =  0x7D000;  // PokeIcon3D.garc  // 親はHEAPID_APP_DEVICE
  static const s32 UNCOMP_WORK_SIZE          = 0x1EE000;  //NMCat[1784]対処前0x201000;  // 圧縮ファイルを読み込むためのワークヒープ。1個ずつしか載せられないサイズ。resrot_pack.pack.comp, islandground_島_Lv_pack.pack.comp, ポケモン3Dモデル(ポケモン3Dモデルの圧縮ファイルには1.5MBほど必要)  // 親はHEAPID_APP_DEVICE

  static const s32 ISLANDGROUND_LV_FILE_SIZE = 0x1ED000;  //NMCat[1784]対処前0x200000;  // islandground_島_Lv_pack.pack  // 親はHEAPID_APP_DEVICE

  // ポケモンアイコン3D
  static const s32 POKEICON_3D_TEX_SIZE     =   0x1400;  // ポケモンアイコン3D 1個分のサイズ  // 【親はPOKEICON_3D_TEX_ALL_SIZE】
  static const s32 POKEICON_3D_TEX_ALL_SIZE =  0x36800;  // = POKEICON_3D_TEX_SIZE * 21 * 2 + 0x2000  // ポケモンアイコン3D 1度に出る合計サイズ + 一時ワークヒープ  // 親はHEAPID_APP_DEVICE

  // 管理
  static const s32 FRAME_MANAGER_SIZE        =    0xFA0;  // フレームマネージャが使用するヒープ  // 親はHEAPID_APP_DEVICE
  static const s32 DEBUG_WIN_SIZE            =  0x25000;  // デバッグウィンドウが使用するヒープ  // 親はHEAPID_APP_DEVICE
  static const s32 ROOT_HEAP_SIZE            = 0xF02000;  //福島さん大野さんNMCat[1784]暫定対処(0xEDD000 + 0x10000);  // 大物ヒープをここからつくる。ヒープ作成に必要な小物インスタンスの作成もこれを使って行う。  // 親はHEAPID_APP_DEVICE

  // PROC内で使用
  static const s32 PROC_HEAP_SIZE       =   0x3000;  //整理前0x4000;  // 【親はROOT_HEAP_SIZE】
  static const s32 PROC_DEV_SIZE        = 0x3EBB00;  //整理前0x8B8000;  //ギリ0x6E1000←元々0x8CA000←大元0x900000;  // 【親はROOT_HEAP_SIZE】
          // ResortProcのmpAppHeapはResortIslandDrawでも使われる。mpAppHeapは、常時存在するものを生成し終わってから、生成/破棄を繰り返す島モデルで使われる。
  
  // ResortInitLogic内で使用
  static const s32 LOGIC_HEAP_SIZE       =    0x980;  // 【親はROOT_HEAP_SIZE】
  static const s32 LOGIC_TEMP_HEAP_SIZE  =   0x2000;  // 小物用一時ヒープ。確保したらすぐ解放するようなものにしか使わない。  // 【親はROOT_HEAP_SIZE】

  // ポケモン3Dモデルシステム、ポケモン3Dモデル用
  static const s32 POKEMODEL_HEAP_SIZE  =  0x400000;  // 【親はROOT_HEAP_SIZE】

  // FRAME内で使用
  static const s32 FRAME_HEAP_SIZE =  0x10000;  //整理前0x300000;  //ギリ0x213000;  // 【親はROOT_HEAP_SIZE】
  static const s32 FRAME_DEV_SIZE  = 0x2A7060;  //キャラがいないとき0x67000;//整理前0x320000;  //ギリ0x29D000←元々0x800000;  // 【親はROOT_HEAP_SIZE】

  // レイアウトで使用
  static const s32 LAYOUT_HEAP_SIZE   = 0x2E0000;  //整理前0x340000;  // 【親はROOT_HEAP_SIZE】
  static const s32 LAYOUT_DEV_SIZE    =  0xDFFA0;  //キャラがいるとき0x320000;  //整理前0x3C0000;  // 【親はROOT_HEAP_SIZE】

  // アイコンで使用
  static const s32 ICON_2D_HEAP_SIZE  = 0x1000;  // 【親はROOT_HEAP_SIZE】
  static const s32 ICON_2D_DEV_SIZE   = 0x96000;  //福島さん大野さんNMCat[1784]暫定対処0x81000;  //0x70000;//0x81000;  //整理前0x80000;  // 【親はROOT_HEAP_SIZE】
};


GFL_NAMESPACE_END(Resort)
GFL_NAMESPACE_END(App)


#endif  // NIJI_PROJECT_APP_RESORT_RESORTHEAPDEFINE_H_INCLUDED

