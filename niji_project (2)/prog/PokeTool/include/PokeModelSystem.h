//======================================================================
/**
 * @file	PokeModelSystem.h
 * @brief	niji ポケモンモデルシステム
 * @author	ariizumi
 * @data	15/05/07
 */
//======================================================================
#if !defined( __POKE_MODEL_SYSTEM_H__ )
#define __POKE_MODEL_SYSTEM_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <heap/include/gfl2_Heap.h>
#include <fs/include/gfl2_Fs.h>
#include <util/include/gfl2_List.h>

#include <util/include/gfl2_FixedSizeContainer.h>
#include <RenderingEngine/include/Renderer/Util/gfl2_ColorShaderTextureRenderPath.h>

#include "System/include/nijiAllocator.h"

#include <PokeTool/include/PokeModel.h>

GFL_NAMESPACE_BEGIN(PokeTool)

//カラーシェーダー描画後のViewportのリセットだけを行うパス
class ViewportResetPath: public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
{
public:
	ViewportResetPath();
	virtual ~ViewportResetPath();

private:
	virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
};


class PokeModelSystem
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(PokeModelSystem);
public:

  //管理Heapの作成でサイズ計算に使用する
  struct HeapOption
  {
    MODEL_ANIMETYPE animeType;
    bool useIdModel;      //タッチ用IDモデルを使うか？
    bool useShadow;       //影を使うか？
    bool useColorShader;  //カラーシェーダーを使うか？(VRAM確保)

    int allocSize;  //サイズ強制指定用

    gfl2::gfx::Pool vramPool; //VRAM確保先

    HeapOption()
      :animeType(MODEL_ANIMETYPE_BATTLE)
      ,useIdModel(false)
      ,useShadow(false)
      ,useColorShader(false)
      ,allocSize(0)
      ,vramPool(gfl2::gfx::Pool::Default)
    {}
  };

  //GARCの中身の管理データ
  typedef struct
  {
    u16 mDataTop;
    u8  mDataNum;
    u8  mFlgData;
  }POKE_MNG_DATA;

  //POKE_MNG_DATA.mFlgData用(ポケモンの種類ごと)
  static const u8 POKE_MNG_FLG_DATA_ENABLE = 1;       //仮データではない場合
  static const u8 POKE_MNG_FLG_EXIST_FEMALE = 2;      //雌雄差が存在する
  static const u8 POKE_MNG_FLG_EXIST_FORM_CHANGE = 4; //フォルム違いが存在する

  typedef struct
  {
    u8 mFlgData;
    u8 mSubFrom;
  }POKE_FLG_DATA;

  //POKE_FLG_DATA.mFlgData用(ポケモン1体(フォルム毎)ごと)
  static const u8 POKE_DATA_FLG_SHARE_ANIME = 1;
  static const u8 POKE_DATA_FLG_SHARE_TEX = 2;
  static const u8 POKE_DATA_FLG_SHARE_MODEL = 4;
  //----------------------------------------------------------------------------
  /**
   *  @brief  コンストラクタ
   */
  //-----------------------------------------------------------------------------
  PokeModelSystem();

  //----------------------------------------------------------------------------
  /**
   *  @brief  デストラクタ
   */
  //-----------------------------------------------------------------------------
  ~PokeModelSystem();

  //----------------------------------------------------------------------------
  /**
   *  @brief  システムの初期化(非同期版)
   *
   *  @param  gfl2::heap::HeapBase *heap              デバイスヒープ
   *  @param  gfl2::heap::HeapBase *workHeap          ファイル読み込み用Heap
   *  @param  gfl2::fs::AsyncFileManager* readMng     非同期読み込み用マネージャ
   *  @param  int                                     モデル管理個数
   *
   *  InitSystemAsyncをした後、IsFinishInitSystemAsync を待ってから、CreateSystemを読んでください。
   */
  //-----------------------------------------------------------------------------
  void InitSystemAsync( gfl2::heap::HeapBase *heap , gfl2::heap::HeapBase *workHeap , gfl2::fs::AsyncFileManager* fileMng , int modelNum );

  //----------------------------------------------------------------------------
  /**
   *  @brief  システムの初期化待ち(非同期版)
   */
  //-----------------------------------------------------------------------------
  bool IsFinishInitSystemAsync(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  システムの作成
   *          IsFinishInitSystemAsyncの後に呼んでください。
   */
  //-----------------------------------------------------------------------------
  void CreateSystem(gfl2::gfx::IGLAllocator *allocator,bool useColorShader,bool useIdMode);
  //並び順の対応。上を使って下さい。
  void CreateSystem(bool useColorShader,gfl2::gfx::IGLAllocator *allocator){CreateSystem(allocator,useColorShader,false);}

  //----------------------------------------------------------------------------
  /**
   *  @brief  システムの開放(非同期版)
   */
  //-----------------------------------------------------------------------------
  void TermSystemAsync(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  システムの開放(非同期版)
   */
  //-----------------------------------------------------------------------------
  bool IsFinishTermSystemAsync(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeModelの各種更新
   */
  //-----------------------------------------------------------------------------
  void Update(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeModelの各種更新
   */
  //-----------------------------------------------------------------------------
  void Draw(gfl2::gfx::CtrDisplayNo displayNo);

  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeModel生成(簡易パラメータ版)
   *
   *  @param  const SimpleParam *param                    シンプルパラム
   *  @param  const PokeModel::SetupOption *opt           セットアップオプション
   *  @return PokeModel*                                  ポケモンモデル
   */
  //-----------------------------------------------------------------------------
  PokeModel* CreateModelSimple( const SimpleParam *param , const PokeModel::SetupOption &opt );

  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeModel生成(PokeParam版)
   *
   *  @param  const pml::pokepara::PokemonParam *pp       ポケモンパラム
   *  @param  const PokeModel::SetupOption *opt           セットアップオプション
   *  @return PokeModel*                                  ポケモンモデル
   */
  //-----------------------------------------------------------------------------
  PokeModel* CreateModelPP( const pml::pokepara::PokemonParam *pp , const PokeModel::SetupOption &opt );

  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeModel破棄
   */
  //-----------------------------------------------------------------------------
  void DeleteModel( PokeModel *mdl );

  //----------------------------------------------------------------------------
  /**
   *  @brief  管理用ヒープの作成
   *  @param  gfl2::heap::HeapBase *heap  管理領域＋ポケモン用ヒープの確保用
   *  @param  HeapOption *heapOption      作成用パラメータ
   */
  //-----------------------------------------------------------------------------
  void CreatePokeModelHeap( gfl2::heap::HeapBase *heap , const HeapOption *heapOption );

  //----------------------------------------------------------------------------
  /**
   *  @brief  管理用ヒープの削除
   */
  //-----------------------------------------------------------------------------
  void DeletePokeModelHeap(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  管理用ヒープの取得(デバッグ用
   */
  //-----------------------------------------------------------------------------
#if PM_DEBUG
  gfl2::heap::HeapBase* GetPokeModelHeap(int idx){return mModelHeapArr[idx];}
#endif //PM_DEBUG

  //----------------------------------------------------------------------------
  /**
   *  @brief  ポケモンモデルの表示に必要なヒープサイズの計算
   */
  //-----------------------------------------------------------------------------
  u32 CalcPokeModelHeapSize( const HeapOption *heapOption );

  gfl2::fs::AsyncFileManager *GetAsyncFileManager(void){return mFileReadManager;}

  gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath* CreatePokeShaderPath(gfl2::heap::HeapBase *heap);
  void DeletePokeShaderPath(gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath *path);

  int GetPokeShaderPathNum(void)const;
  gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath* GetPokeShaderPath(u32 idx);

  gfl2::renderingengine::scenegraph::resource::ShaderResourceNode* GetIdTextureResourceNode(void){return mIdTextureShaderResourceNode;}

  //管理データがらみ
  //ポケモン管理データの取得
  const POKE_MNG_DATA *GetMngData( int monsNo );
  //ポケモン管理フラグの取得
  const POKE_FLG_DATA *GetFlgData( int monsNo , int formNo , int sex );
  //ポケモンデータインデックスの取得
  int GetDataIdx( int monsNo , int formNo , int sex );

private:

  gfl2::fs::AsyncFileManager* mFileReadManager;
  gfl2::heap::HeapBase *mHeap;
  gfl2::heap::HeapBase *mWorkHeap;
  System::nijiAllocator *mGLAllocator;  //Vram確保用

  int mModelNum;
  gfl2::util::List<PokeModel*> *mList;

  bool mIsEnable;
  bool mIsSetupShader;

  //カラーシェーダーパス用
  void *mPokeShaderResourceBuf;
  gfl2::fs::BinLinkerAccessor mPokeShaderResourceBin;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mPokeShaderResourceNode;
  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* mPokeShaderModel;

  gfl2::util::FixedSizeContainer<gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath*> mPokeShaderRenderPaths;

  ViewportResetPath *mViewportResetPath;

  //IDテクスチャ用
  void *mIdTextureShaderResourceBuf;
  gfl2::fs::BinLinkerAccessor mIdTextureShaderResourceBin;
  gfl2::renderingengine::scenegraph::resource::ShaderResourceNode *mIdTextureShaderResourceNode;

  //管理ヒープ
  gfl2::heap::HeapBase **mModelHeapArr;
  void **mModelVramArr;
  bool  *mIsUseHeap;

  //GARCの中身の管理データ
  //Bufの先頭にMonsNo分(コンバート時)のPOKE_MNG_DATAが並んでいる。
  //その後ろにモデル毎のPOKE_FLG_DATAが並んでいる。
  void          *mPokeMngDataBuf;
  POKE_MNG_DATA *mPokeMngData;
  POKE_FLG_DATA *mPokeFlgData;
};

GFL_NAMESPACE_END(PokeTool)


#endif //__POKE_MODEL_SYSTEM_H__