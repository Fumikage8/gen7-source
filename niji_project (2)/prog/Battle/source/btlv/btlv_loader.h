//======================================================================
/**
 * @file btlv_loader.h
 * @date 2015/12/08 15:35:38
 * @author ariizumi_nobuhiko
 * @brief バトル描画の開始時リソース読み込みの仕組み
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BTLV_LOADER_H_INCLUDED__
#define __BTLV_LOADER_H_INCLUDED__
#pragma once

#include <macro/include/gfl2_Macros.h>

GFL_NAMESPACE_BEGIN(GameSys)
class DllReadThread;
GFL_NAMESPACE_END(GameSys)

GFL_NAMESPACE_BEGIN(btl)

class BattleViewSystem;
class BtlvObject_PokeModel;
class BtlvObject_Chara;

//読み込み管理の基底
class BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderBase);

public:

  BtlvLoaderBase(BattleViewSystem *core)
    :mBtlvCore(core)
    ,mIsLoad(false)
    {}
  virtual ~BtlvLoaderBase(){}

  //読み込み開始
  virtual void StartLoad(void){}
  //読み込み中の更新
  virtual void LoadUpdate(void){}
  //読み込み完了チェック
  virtual bool IsFinishLoad(void){return true;}
  //読み込み後生成
  virtual void Create(void){}

  //読み込み中フラグ
  bool GetIsLoad(void)const{return mIsLoad;}
  void SetIsLoad(bool flg){mIsLoad = flg;}
protected:
  BattleViewSystem *mBtlvCore;
  bool mIsLoad;
};

//------------------------------------------------------
//ポケモン用
class BtlvLoaderPoke :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderPoke);

public:
  BtlvLoaderPoke(BattleViewSystem *core,BtlvObject_PokeModel *poke,int idx);
  virtual ~BtlvLoaderPoke();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  BtlvObject_PokeModel *mPoke;
  int mIdx;
};
//------------------------------------------------------
//トレーナー用
class BtlvLoaderTrainer :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderTrainer);

public:
  BtlvLoaderTrainer(BattleViewSystem *core,BtlvObject_Chara *mdl,int idx);
  virtual ~BtlvLoaderTrainer();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
protected:
  BtlvObject_Chara *mMdl;
  int mIdx;
};
//------------------------------------------------------
//トレーナー用
class BtlvLoaderTrainerAsyncSetup :public BtlvLoaderTrainer
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderTrainerAsyncSetup);

public:
  BtlvLoaderTrainerAsyncSetup(BattleViewSystem *core,BtlvObject_Chara *mdl,int idx);
  virtual ~BtlvLoaderTrainerAsyncSetup();

  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
};
//------------------------------------------------------
//背景用
class BtlvLoaderBg :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderBg);

public:
  BtlvLoaderBg(BattleViewSystem *core);
  virtual ~BtlvLoaderBg();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み中の更新
  virtual void LoadUpdate(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  int mIdx;
};
//------------------------------------------------------
//キャラ用影
class BtlvLoaderCharaShadow :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderCharaShadow);

public:
  BtlvLoaderCharaShadow(BattleViewSystem *core);
  virtual ~BtlvLoaderCharaShadow();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  int mIdx;
};

//------------------------------------------------------
//エフェクト用モデル
class BtlvLoaderEffectModel :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderEffectModel);

public:
  BtlvLoaderEffectModel(BattleViewSystem *core,BtlvObject_Gfmdl *mdl,int type);
  virtual ~BtlvLoaderEffectModel();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  BtlvObject_Gfmdl *mMdl;
  int mIdx;
  int mType;
};

//------------------------------------------------------
//攻撃モーションテーブル
//ポケモン用追加情報テーブル
//トレーナー用追加情報テーブル
class BtlvLoaderBattleData :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderBattleData);

public:
  BtlvLoaderBattleData(BattleViewSystem *core,void **buf,gfl2::fs::BinLinkerAccessor *binLinker);
  virtual ~BtlvLoaderBattleData();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  gfl2::fs::BinLinkerAccessor *mBinLinker;
  void **mBuf;
};

//------------------------------------------------------
//汎用エフェクトリソース
class BtlvLoaderEffectResource:public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderEffectResource);

public:
  BtlvLoaderEffectResource(BattleViewSystem *core,void **buf, u32 datId);
  virtual ~BtlvLoaderEffectResource();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  void **mBuf;
  u32 mDatId;

};

//------------------------------------------------------
//DLL読み替えよう
class BtlvLoaderDll :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderDll);

public:
  BtlvLoaderDll(BattleViewSystem *core);
  virtual ~BtlvLoaderDll();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み中の更新
  virtual void LoadUpdate(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  int mIdx;

  static const int DLL_NUM = 2;
  void *mDllBuf[DLL_NUM];
  size_t mDllSize[DLL_NUM];

  GameSys::DllReadThread *mDllReadThread;
};

//------------------------------------------------------
//初期天候
class BtlvLoaderWeather:public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderWeather);

public:
  BtlvLoaderWeather(BattleViewSystem *core);
  virtual ~BtlvLoaderWeather();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  void **mBuf;
  u32 mDatId;

};

//------------------------------------------------------
//UIメッセージ読み用
class BtlvLoaderUiMessage:public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderUiMessage);

public:
  BtlvLoaderUiMessage(BattleViewSystem *core);
  virtual ~BtlvLoaderUiMessage();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み中の更新
  virtual void LoadUpdate(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
};

//------------------------------------------------------
//バトルフォント用(一旦未使用
class BtlvLoaderBattleFont :public BtlvLoaderBase
{
  GFL_FORBID_COPY_AND_ASSIGN(BtlvLoaderBattleFont);

public:
  BtlvLoaderBattleFont(BattleViewSystem *core);
  virtual ~BtlvLoaderBattleFont();

  //読み込み開始
  virtual void StartLoad(void);
  //読み込み中の更新
  virtual void LoadUpdate(void);
  //読み込み完了チェック
  virtual bool IsFinishLoad(void);
  //読み込み後生成
  virtual void Create(void);
private:
  int mIdx;
};
GFL_NAMESPACE_END(btl)

#endif // __BTLV_LOADER_H_INCLUDED__
