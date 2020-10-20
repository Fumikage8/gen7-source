//======================================================================
/**
 * @file	PokeModelLocal.h
 * @brief	niji ポケモンモデル用ローカル定義
 * @author	ariizumi
 * @data	15/05/07
 */
//======================================================================
#if !defined( __POKE_MODEL_LOCAL_H__ )
#define __POKE_MODEL_LOCAL_H__
#pragma once

#include <util/include/gfl2_GLHeapAllocator.h>

GFL_NAMESPACE_BEGIN(PokeTool)

//デフォルトAdjustHeight計算用
#define POKE_BASE_ADJUST_RATE (0.43f)

//BINの種類
enum
{
  BINTYPE_COMMON,     //モデル、フェイシャル
  BINTYPE_TEX,        //通常テクスチャ
  BINTYPE_TEX_RARE,   //色違いテクスチャ
  BINTYPE_ID_TEX,     //可愛がりタッチ判定テクスチャ
  BINTYPE_BATTLE,     //バトルモーション
  BINTYPE_KAWAIGARI,  //可愛がりモーション
  BINTYPE_FIELD,      //フィールドモーション
  BINTYPE_POKE_FINDER,  //ファインダーモーション
  BINTYPE_SETTING,    //設定データ

  BINTYPE_MAX,
};

//各種BINの中身
enum
{
  //coパック
  BINDATA_CO_MODEL = 0,
  BINDATA_CO_SHADOW_MODEL,
  BINDATA_CO_SHADER_PACK,
  BINDATA_CO_SHADOW_SHADER_PACK,
  BINDATA_CO_FP_DISABLE_SETTING,  //全力シェーダー例外リソース

  //idパック
  BINDATA_ID_MAT_SETTING = 0,
  //以降テクスチャ

  //テクスチャはパックがテクスチャ枚数によって可変。全部テクスチャ
  //アニメーションもパックによって可変。全部gfbmot
  //ただフェイシャル、ノード、ロケーターが入っているので、アニメ最大数からのオフセットを描いておく
  BINDATA_OFS_ANM_MCNF = 0,
  BINDATA_OFS_ANM_EYE01,
  BINDATA_OFS_ANM_EYE02,
  BINDATA_OFS_ANM_EYE03,
  BINDATA_OFS_ANM_MOUTH01,
  BINDATA_OFS_ANM_MOUTH02,
  BINDATA_OFS_ANM_MOUTH03,
  BINDATA_OFS_ANM_LOOP01,
  BINDATA_OFS_ANM_LOOP02,
  BINDATA_OFS_ANM_LOOP03,
  BINDATA_OFS_ANM_LOOP04,
  BINDATA_OFS_ANM_EFFECT_NODE,
  BINDATA_OFS_ANM_PD_LOCATER,


  //settingパック
  BINDATA_SETTING_SETTING = 0,
  BINDATA_SETTING_RARE,
};

//アニメーションスロット
enum
{
  POKE_ANIME_SLOT_MAIN1,
  POKE_ANIME_SLOT_MAIN2,
  POKE_ANIME_SLOT_EYE1,
  POKE_ANIME_SLOT_EYE2,
  POKE_ANIME_SLOT_EYE3,
  POKE_ANIME_SLOT_MOUTH1,
  POKE_ANIME_SLOT_MOUTH2,
  POKE_ANIME_SLOT_MOUTH3,
  POKE_ANIME_SLOT_LOOP1,
  POKE_ANIME_SLOT_LOOP2,
  POKE_ANIME_SLOT_LOOP3,
  POKE_ANIME_SLOT_LOOP4,

  POKE_ANIME_SLOT_NUM,

  //変数の配列に必要だったのでPokeModel.hに移動
  //POKE_ANIME_SLOT_EYE_NUM = 3,
  //POKE_ANIME_SLOT_MOUTH_NUM = 3,
};

///EffectNodeのデータ
struct EffectNodeData
{
  char parentNode[32];
  f32  ofsX;
  f32  ofsY;
  f32  ofsZ;
  u8   nodeType;
  u8   nodeIdx;
  u8   pad[2];
};

//VRAM固定アロケーター
//スタック式アロケーター、ポケモンのVRAMは固定サイズ(0x60000)
class PokeModelAllocator : public gfl2::util::GLHeapAllocator
{
  GFL_FORBID_COPY_AND_ASSIGN(PokeModelAllocator); //コピーコンストラクタ＋代入禁止
public:
  PokeModelAllocator( gfl2::heap::HeapBase *devHeap , void* vramTop , u32 vramSize);
  virtual ~PokeModelAllocator();

  virtual void* LocalMemoryMalloc( gfl2::gfx::Pool allocation, u32 size, u32 alignment ){ return LocalMemoryMalloc(size,alignment); }
  virtual void* LocalMemoryMalloc(u32 size, u32 alignment);
  virtual void LocalMemoryFree(void* const address);
private:
  void *mVramTop;
  u32 mVramOffset;
  u32 mVramSize;
};


GFL_NAMESPACE_END(PokeTool)


#endif //__POKE_MODEL_LOCAL_H__