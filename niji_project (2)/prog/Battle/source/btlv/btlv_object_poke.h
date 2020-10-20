//======================================================================
/**
 * @file	btlv_object_poke.h
 * @brief	バトル描画 ポケモンモデル
 * @author	ariizumi
 * @data	15/06/09
 */
//======================================================================
#if !defined( __BTLV_OBJECT_POKE__H__ )
#define __BTLV_OBJECT_POKE__H__
#pragma once

#include <macro/include/gfl2_macros.h>
#include <math/include/gfl2_math.h>

#include "niji_conv_header/battle/poke_extra_data_struct.h"
#include <DemoLib/Object/include/Object_Poke.h>

#include "btlv_local_def.h"

GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;

class BtlvObject_PokeModel:public DemoLib::Obj::Object_Poke
{
public:
  //コンストラクタ
  BtlvObject_PokeModel(BattleViewSystem *btlvCore,DemoLib::Obj::ObjectRenderPipeLine *pipe,BtlvPos pos,int idx);
  //デストラクタ
  virtual ~BtlvObject_PokeModel();

  void StartLoad_Battle(void);
  void StartLoad_Battle(BtlvPos target);  //変身用
  //ライト番号設定のため継承
  virtual void Create(gfl2::heap::HeapBase *heap,gfl2::gfx::IGLAllocator *allocator,bool useShader);
  virtual void Delete(void);

  //ObjectRenderPipeLine経由で追加する
  //optにライト番号渡したいので追加
  virtual void AddRenderPath(int opt = 0);
  //全力シェーダーのフォロー用に追加
  virtual void RemoveRenderPath(void);

  //ポケモンは反転するとまずい
  virtual void SetIsFlip(const bool flg){mIsFlip = false;}

  virtual void ChangeAnimationDirect( int type , bool forceReset = false );
  void StartWaitAnime( bool isForce , bool isRand );

  void GetNodePosition_Sequence( int node , gfl2::math::Vector3 *pos );
  void UpdatePositionForce(void);
  f32 GetCamAdjustHeight(void);
  f32 GetAdjustHeight(void);
  bool GetCollision(gfl2::math::AABB *box);

  //SequenceEditorからの値をMODEL_NODEに変換
  void GetFollowNode( int seqNode , PokeTool::MODEL_NODE *retNode , int *retNodeIdx );

  //攻撃モーションのキーの位置を取得
  int GetAttackKeyFrame(PokeTool::MODEL_ANIME type);

   //見た目のMonsNo・FormNoの取得
  int GetMonsNo(void)const{return mMonsNo;}
  int GetFormNo(void)const{return mFormNo;}

  int GetTikiType(void)const;
  
  //サイズの取得(内部で持っており、死んでも前回のサイズが残るようにする
  PokeTool::POKE_SIZE GetSize(void){return mSize;}

  //全力技のシェーダーか？
  void AddFPShader(bool isZenryoku);
  void RemoveFpShader(void);
  bool IsEnableFPShader(void)const{return (mFpShaderCnt>0?true:false);}
  bool IsFPZenryoku(void){return mIsFPZeryoku;}

  DemoLib::Obj::Object_Chara* GetMigawariModel(void)const{return mMigawariModel;}

  //HitBack処理用
  void SetHitBackFlg(bool flg){mIsHitBack = flg;}
  bool GetHitBackFlg(void)const{return mIsHitBack;}

  //表示系
  virtual void SetVisible(const bool flg);
  virtual void SetShadowVisible(const bool flg);
  virtual bool GetVisible(void)const;     //身代わり用に継承
  bool IsVisibleMigawari(void){return mIsVisibleMigawari;}
  bool IsVisibleTame(void){return mIsVisibleTame;}
  void SetVisibleMigawari(bool flg);
  void SetVisibleTame(bool flg);
  void SetVisibleDigudaStone(bool flg);
  void SetEnableFloat(bool flg);

  //全力技用の一時非表示
  void SetHideMigawariZenryoku(bool flg){mIsHideMigawariZenryoku = flg;}
  bool IsHideMigawariZenryoku(void){return mIsHideMigawariZenryoku;}

  //ピンチ音制御
  bool GetIsPlayPinchSe(void){return mIsPlayPinchSe;}
  void SetIsPlayPinchSe(bool flg){mIsPlayPinchSe = flg;}

  void SetEnableWaitB(bool flg);

  void NoLoadFormChange(void);

  const PokeExtraInfoStruct* GetPokeInfoData(void){return mPokeInfoData;}

  void SetIndex(int idx){mIdx = idx;}
  int GetIndex(void){return mIdx;}
  
  void SetSick( PokeSick sick );
  PokeSick GetSick(void)const{return mSick;}
  f32 UpdateSickMotion(void);

  //目の上書き
  void SetOverrideEyeType(int type);
  void ResetOverrideEyeType(void);

  //振り向き系
  void UpdateLookAt(void);  //TraverseModelFast の後
  void SetEnableLookAt(bool flg);
  void SetLookAtPos(gfl2::math::Vector3 *pos);  //NULLでなし

  //カメラとの当たり判定用
  bool CheckHitCamera(const gfl2::math::Vector3 &pos,f32 ofsSize);
  void UpdateCulling(void);

  //ライト番号再設定
  int GetLightNo(void){return mLightNo;}
  //ライトをエフェクトで操作するか？
  void SetUseEffectLight(bool flg){mUseEffectLight = flg;}
  bool IsUseEffectLight(void)const{return mUseEffectLight;}
  void SetEffectLightDir(const gfl2::math::Vector3 &dir){mEffLightDir = dir;}
  const gfl2::math::Vector3& GetEffectLightDir(void)const{return mEffLightDir;}
  // @fix MMCat[359] 通常色のメタモンが色違いの「ズガドーン/ビーストH」に「へんしん」した状態で「ビックリヘッド/はなびせんよう」を使用すると技演出が通常色のものになる
  bool IsRare(void)const{return mIsRare;}

#if PM_DEBUG
  virtual bool* GetVisiblePointer(void){return &mIsVisiblePoke;}
  void ReloadData(void);
#endif //PM_DEBUG
protected:
  //こっちはUpdateCoreの後に呼ばれる。座標反映後の更新はこちらで。
  virtual void UpdateAlwaysAfterCore(int step=1);
  //IsUpdateが立っていたら来る関数。
  //IsEnableはチェック済みなので座標更新はこちらで。
  virtual void UpdateCore(int step=1);
  //毎フレーム呼ばれる
  virtual void UpdateAlways(int step=1);

private:
  //表示状態の更新
  void UpdateVisible(void);
  //フェイシャル更新
  void UpdateFaceAnime(void);
  //みがわり用の補正倍率取得
  f32 GetMigawariAdjustHeight(void);
  //身代わり専用の座標計算処理
  void GetNodePosition_Migawari(int node , gfl2::math::Vector3 *ret );

  BattleViewSystem *mBtlvCore;
  int mIdx;
  int mMonsNo;
  int mFormNo;
  int mLightNo; //これはMoveMenberなどで変わらない

  DemoLib::Obj::Object_Chara *mMigawariModel;
  DemoLib::Obj::Object_GfMdl *mDigudaStoneModel;
  f32 mDigudaStoneScaleWidth;
  f32 mDigudaStoneScaleHeight;

  bool mIsVisiblePoke;    //ポケモンの表示
  bool mIsVisibleShadow;  //影の表示状態
  bool mIsVisibleTame;    //ためでの表示状態
  bool mIsVisibleMigawari;  //身代わりの表示状態
  bool mIsVisibleDigudaStone; //ディグダ岩の表示状態
  bool mCamHitErace;  //カメラヒットの表示状態
  bool mEnableFloat;  //浮き禁止ポケモンの浮き許可
  bool mEnableWiatB;
  // @fix MMCat[359] 通常色のメタモンが色違いの「ズガドーン/ビーストH」に「へんしん」した状態で「ビックリヘッド/はなびせんよう」を使用すると技演出が通常色のものになる
  bool mIsRare;

  bool mIsHideMigawariZenryoku;  //Z技による一時的身代わり非表示

  //全力シェーダーがかかっているか？
  //複数回かかるのでCnt式
  int mFpShaderCnt;
  bool mIsFPZeryoku;  //全力シェーダーか？ぬし、UBシェーダーか？

  //HitBack処理用
  bool mIsHitBack;

  //振り向き制御
  bool mEnableLookAt;

  //フェイシャル上書き
  int mOverrideEyeType;

  //ピンチ音制御
  bool mIsPlayPinchSe;

  //ライトをエフェクトで操作するか？
  bool mUseEffectLight;
  gfl2::math::Vector3 mEffLightDir;

  PokeExtraInfoStruct *mPokeInfoData;

  PokeTool::POKE_SIZE mSize;
  PokeSick mSick;
#ifdef DEBUG_ONLY_FOR_ariizumi_nobuhiko
  void *mDhigudaStoneDummy;
#endif //DEBUG_ONLY_FOR_ariizumi_nobuhiko
};

GFL_NAMESPACE_END(btl)


#endif  //__BTLV_OBJECT_POKE__H__