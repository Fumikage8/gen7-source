//======================================================================
/**
 * @file	btlv_object_chara.h
 * @brief	バトル描画 キャラモデル
 * @author	ariizumi
 * @data	15/09/03
 */
//======================================================================
#if !defined( __BTLV_OBJECT_CHARA__H__ )
#define __BTLV_OBJECT_CHARA__H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <math/include/gfl2_math.h>

#include "niji_conv_header/battle/trainer_extra_data_struct.h"

#include <DemoLib/Object/include/Object_Chara.h>

#include "btlv_local_def.h"


GFL_NAMESPACE_BEGIN(trainer)
  class TrainerTypeData;
GFL_NAMESPACE_END(trainer)


GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;

enum
{
  TRAINER_ANIME_TOUJYOU1,
  TRAINER_ANIME_BALL1,
  TRAINER_ANIME_ORDER1,
  TRAINER_ANIME_LOSE1,
  TRAINER_ANIME_WAIT1,
  TRAINER_ANIME_WAIT2,
  TRAINER_ANIME_TOUJYOU2,
  TRAINER_ANIME_BALL2,
  TRAINER_ANIME_LOSE2,
  TRAINER_ANIME_BALL3_2,
  TRAINER_ANIME_MULTIBALL01,
  TRAINER_ANIME_TOUJYOU03,
  TRAINER_ANIME_BALL3,
  TRAINER_ANIME_LOSE3,
  TRAINER_ANIME_ZNORMAL01,
  TRAINER_ANIME_MULTIBALL00,


  //以下選択モーションなので飛び版
  TRAINER_ANIME_SELECT_BALL1  = 0x00010000,
  TRAINER_ANIME_SELECT_WAIT1  = 0x00010001,
  TRAINER_ANIME_SELECT_MULTI1 = 0x00010002,
  TRAINER_ANIME_SELECT_BALL2  = 0x00020000,
  TRAINER_ANIME_SELECT_WAIT2  = 0x00020001,
  TRAINER_ANIME_SELECT_MULTI2 = 0x00020002,
  TRAINER_ANIME_SELECT_BALL3  = 0x00030000,
  TRAINER_ANIME_SELECT_WAIT3  = 0x00030001,
  TRAINER_ANIME_SELECT_MULTI3 = 0x00030002,
  TRAINER_ANIME_SELECT_BALL4  = 0x00040000,
  TRAINER_ANIME_SELECT_WAIT4  = 0x00040001,
  TRAINER_ANIME_SELECT_MULTI4 = 0x00040002,
  TRAINER_ANIME_SELECT_BALL5  = 0x00050000,
  TRAINER_ANIME_SELECT_WAIT5  = 0x00050001,
  TRAINER_ANIME_SELECT_MULTI5 = 0x00050002,
  TRAINER_ANIME_SELECT_BALL6  = 0x00060000,
  TRAINER_ANIME_SELECT_WAIT6  = 0x00060001,
  TRAINER_ANIME_SELECT_MULTI6 = 0x00060002,
  TRAINER_ANIME_SELECT_BALL7  = 0x00070000,
  TRAINER_ANIME_SELECT_WAIT7  = 0x00070001,
  TRAINER_ANIME_SELECT_MULTI7 = 0x00070002,
  TRAINER_ANIME_SELECT_BALL8  = 0x00080000,
  TRAINER_ANIME_SELECT_WAIT8  = 0x00080001,
  TRAINER_ANIME_SELECT_MULTI8 = 0x00080002,
  TRAINER_ANIME_SELECT_BALL9  = 0x000B0000,
  TRAINER_ANIME_SELECT_WAIT9  = 0x000B0001,
  TRAINER_ANIME_SELECT_MULTI9 = 0x000B0002
};

class BtlvObject_Chara:public DemoLib::Obj::Object_Chara
{
public:
  BtlvObject_Chara(BattleViewSystem *btlvCore,int idx,DemoLib::Obj::ObjectRenderPipeLine *pipe,poke_3d::model::CharaModelFactory *charaFactory,poke_3d::model::DressUpModelResourceManager *dressUpModelResourceManager)
    :Object_Chara(pipe,charaFactory,dressUpModelResourceManager)
    ,mIdx(idx)
    ,mBtlFesIdx(-1)
    ,mBtlvCore(btlvCore)
    ,mIsLoad(false)
    ,mEnableWaitB(false)
    ,mIsDressUpAnime(false)
    ,mShadowModel(pipe,charaFactory,dressUpModelResourceManager)
    ,mAsyncSetupStep(0)
    {}

  //読み込み開始
  virtual void StartLoad(void);
  //この２個はwarnning対策
  virtual void StartLoad(const DemoLib::Obj::Object_Chara::LOAD_PARAM &param){GFL_ASSERT_MSG(0,"こっちを使うな。\n");};
  virtual void StartLoad(gfl2::fs::AsyncFileManager *fileMng,const DemoLib::Obj::Object_GfMdl::LOAD_PARAM &param){GFL_ASSERT_MSG(0,"こっちを使うな。\n");};
  virtual bool IsFinishLoad(void);
  bool AsyncSetup(void);
  void CreateModel(void);

  virtual void ChangeAnimation( u32 animeIdx ){ChangeAnimationRand(animeIdx,false);}
  //warning対策で、別名関数にしておく
  void ChangeAnimationRand( u32 animeIdx , bool isRand = false );
  virtual void ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot=0 );
  virtual void ChangeAnimationResourceDressUp( void *buf, gfl2::gfx::IGLAllocator *allocator  );

  void SetEnableWaitB(bool flg){ mEnableWaitB = flg;}
  bool IsFinishWaitB(void);
  int GetThrowType(void);
  int GetDynamicAnimeType(void);

  void SetIsDressUpAnime(bool flg){ mIsDressUpAnime = flg;}
  bool GetIsDressUpAnime(void){return mIsDressUpAnime;}
  bool GetIsPlayResourceMotion(void){return mIsPlayResourceMotion;}

  u16 GetTrainerType(void){return mTrainerType;}
  void SetCharaModelId(u32 id ){mCharaModelId = id;}
  u32 GetCharaModelId(void){return mCharaModelId;}

  DemoLib::Obj::Object_Chara* GetShadowModel(void){return &mShadowModel;}
  
  //影のための継承用
  virtual void SetPosition(const gfl2::math::Vector3 pos );
  virtual void SetPosition(const f32 x,const f32 y,const f32 z);
  virtual void Delete(void);
  //表示系
  virtual void SetVisible(const bool flg);
  virtual bool GetVisible(void)const;
  void SetShadowVisible(const bool flg);
  //ObjectRenderPipeLine経由で追加する
  virtual void AddRenderPath(int opt = 0);
  virtual void RemoveRenderPath(void);

  //通信時子だとClientIDが若干変わるので
  u8 GetClientID(void);

  void SetAutoBlink(bool flg);

  //ライトをエフェクトで操作するか？
  void SetUseEffectLight(bool flg){mUseEffectLight = flg;}
  bool IsUseEffectLight(void)const{return mUseEffectLight;}
  void SetEffectLightDir(const gfl2::math::Vector3 &dir){mEffLightDir = dir;}
  const gfl2::math::Vector3& GetEffectLightDir(void)const{return mEffLightDir;}

  //カメラとの当たり判定用
  bool CheckHitCamera(const gfl2::math::Vector3 &pos,f32 ofsSize);
  void UpdateCulling(void);

  //ノード座標取得のための強制更新
  void UpdatePositionForce(void);

  TrainerExtraInfoStruct* GetTrainerData(void){return mTrainerData;}

  void SetIndex(int idx){ mIdx = idx; }
  int GetIndex(void){ return mIdx; }
  void SetBattleFesIndex(int idx){ mBtlFesIdx = idx; }
  int GetBattleFesIndex(void){ return mBtlFesIdx; }

#if PM_DEBUG
  virtual bool* GetVisiblePointer(void){return &mIsVisibleChara;}
  void ReloadData(void);
#endif
protected:
  //IsEnableはチェック済み。
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);

private:
  //表示状態の更新
  void UpdateVisible(void);

  BattleViewSystem *mBtlvCore;
  int mIdx; //インデック
  int mBtlFesIdx; //フェストレーナーIdx

  //セットアップ関連
  u16 mTrainerType;
  u32 mCharaModelId;
  bool mIsDressUp;

  int mAttachModelId;

  //制御関連
  bool mIsLoad;
  bool mIsSetupDressMnager;
  bool mEnableWaitB;
  bool mIsDressUpAnime;
  bool mIsPlayResourceMotion;
  int mDynamicAnimeType;

  int mAsyncSetupStep;
  
  //ライトをエフェクトで操作するか？
  bool mUseEffectLight;
  gfl2::math::Vector3 mEffLightDir;
  
  DemoLib::Obj::Object_Chara mShadowModel;

  trainer::TrainerTypeData *mTrTypeData;
  TrainerExtraInfoStruct *mTrainerData;

    //本来はマイステータス(ライド衣装用にキープしておく)
  poke_3d::model::DressUpParam mTempDressParam;
  poke_3d::model::DressUpModelResourceManager *mDressUpModelResourceManager;

  bool mIsVisibleChara;    //ポケモンの表示
  bool mCamHitErace;  //カメラヒットの表示状態

};

GFL_NAMESPACE_END(btl)


#endif  //__BTLV_OBJECT_CHARA__H__