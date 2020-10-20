//======================================================================
/**
 * @file	PokeModel.h
 * @brief	niji ポケモンモデル
 * @author	ariizumi
 * @data	15/05/07
 */
//======================================================================
#if !defined( __POKE_MODEL_H__ )
#define __POKE_MODEL_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <math/include/gfl2_math.h>
#include <util/include/gfl2_GLHeapAllocator.h>
#include <util/include/gfl2_FixedSizeContainer.h>

#include <RenderingEngine/include/Renderer/Util/gfl2_ColorShaderTextureRenderPath.h>
#include <RenderingEngine/include/SceneGraph/Resource/gfl2_ResourceManager.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_InstanceCreator.h>

#include <model/include/gfl2_PokemonModel.h>
#include <PokeTool/include/PokeTool.h>
#include <PokeTool/include/PokeModelPokeFinderMotion.h>

GFL_NAMESPACE_BEGIN(PokeTool)

class PokeModelSystem;
struct EffectNodeData;
class PokeAnimReplace;

static const u32 POKEMODEL_VRAM_SIZE = 0x60000;
//マテリアル名
static const int POKE_MATNAME_LEN = 32;

typedef enum
{
  MODEL_ANIMETYPE_BATTLE,
  MODEL_ANIMETYPE_KAWAIGARI,
  MODEL_ANIMETYPE_FIELD,
  MODEL_ANIMETYPE_POKE_FINDER,
  MODEL_ANIMETYPE_APP,  // APP用モーション(実際にはMODEL_ANIMETYPE_BATTLEかMODEL_ANIMETYPE_KAWAIGARIを割り当てます)

  MODEL_ANIMETYPE_MAX,
}MODEL_ANIMETYPE;

////このモーションはモーションの種類をポケファインダー"以外"にした時専用の定義です。
////ポケファインダーの場合はPokeModelPokeFinderMotion.hのMODEL_ANIME_POKE_FINDERを使って下さい。
/// アニメーションタイプ定義
typedef enum
{
  ///バトル用モーション
  MODEL_ANIME_BT_WAIT_A = 0,  ///待機A
  MODEL_ANIME_BT_WAIT_B,      ///待機B
  MODEL_ANIME_BT_WAIT_C,      ///待機C
  MODEL_ANIME_LAND_A,         ///登場1(飛び出し)
  MODEL_ANIME_LAND_B,         ///登場1(落下ループ)
  MODEL_ANIME_LAND_C,         ///登場1(着地)
  MODEL_ANIME_LAND,           ///登場2
  MODEL_ANIME_APPEAL,         ///メガシンカアピール
  MODEL_ANIME_BUTURI1,        ///物理攻撃１
  MODEL_ANIME_BUTURI2,        ///物理攻撃２
  MODEL_ANIME_BUTURI3,        ///物理攻撃３
  MODEL_ANIME_BUTURI4,        ///物理攻撃４
  MODEL_ANIME_TOKUSHU1,       ///特殊攻撃１
  MODEL_ANIME_TOKUSHU2,       ///特殊攻撃２
  MODEL_ANIME_TOKUSHU3,       ///特殊攻撃３
  MODEL_ANIME_TOKUSHU4,       ///特殊攻撃４
  MODEL_ANIME_DAMAGE_S,       ///ダメージ
  MODEL_ANIME_DOWN,           ///ダウン

  MODEL_ANIME_BATTLE_NUM,

  ///可愛がり用モーション
  MODEL_ANIME_KW_WAIT = 0,    ///待機
  MODEL_ANIME_RESPOND,        ///反応する
  MODEL_ANIME_TURN_A,         ///振り向き１
  MODEL_ANIME_TURN_B,         ///振り向き２
  MODEL_ANIME_DROWSE_A,       ///うたたね(開始)
  MODEL_ANIME_DROWSE_B,       ///うたたね(ループ)
  MODEL_ANIME_DROWSE_C,       ///うたたね(起きる)
  MODEL_ANIME_SLEEP_A,        ///ねむる(ループ)
  MODEL_ANIME_SLEEP_B,        ///ねむる(目を覚ます)
  MODEL_ANIME_HATE,           ///いやがる
  MODEL_ANIME_QUESTION,       ///わからない
  MODEL_ANIME_HAPPY_A,        ///よろこぶ１
  MODEL_ANIME_HAPPY_B,        ///よろこぶ２
  MODEL_ANIME_HAPPY_C,        ///よろこぶ３
  MODEL_ANIME_MOVE_A,         ///くせ１
  MODEL_ANIME_MOVE_B,         ///くせ２
  MODEL_ANIME_MOVE_C,         ///くせ３
  MODEL_ANIME_MOVE_D,         ///くせ４
  MODEL_ANIME_LONELY,         ///さみしがる
  MODEL_ANIME_PLAY_A,         ///かまってほしい１
  MODEL_ANIME_PLAY_B,         ///かまってほしい２
  MODEL_ANIME_MAD,            ///怒る
  MODEL_ANIME_EAT_A,          ///かぶりつく(開始)
  MODEL_ANIME_EAT_B,          ///かぶりつく(ループ)
  MODEL_ANIME_EAT_C,          ///かぶりつく(終了)
  MODEL_ANIME_TOUCH,          ///タッチ

  MODEL_ANIME_KAWAIGARI_NUM,

  MODEL_ANIME_FI_WAIT_A = 0, //移動専用待機01
  MODEL_ANIME_FI_WAIT_B, //移動専用待機02（ランダム再生）
  MODEL_ANIME_WALK01, //歩き
  MODEL_ANIME_RUN01, //走り
  MODEL_ANIME_MERGE_WALK01, //(待機＞歩き＞待機)
  MODEL_ANIME_WAIT_WALK01, //待機＞歩きへ移行
  MODEL_ANIME_WALK_WAIT01, //歩き＞待機へ移行
  MODEL_ANIME_MERGE_RUN01, //(待機＞走り＞待機)
  MODEL_ANIME_WAIT_RUN01, //待機＞走りへ移行
  MODEL_ANIME_RUN_WAIT01, //走り＞待機へ移行
  MODEL_ANIME_MERGE_MOVE01, //(歩き＞走り＞歩き)
  MODEL_ANIME_WALK_RUN01, //歩き＞走りへ移行
  MODEL_ANIME_RUN_WALK01, //走り＞歩きへ移行

  MODEL_ANIME_FIELD_NUM,

  MODEL_ANIME_MAX = MODEL_ANIME_POKE_FINDER_NUM, 
  MODEL_ANIME_ERROR = -1,

  ///APP用モーション
  MODEL_ANIME_APP_WAIT = 0,  ///待機(実際にはMODEL_ANIME_BT_WAIT_AかMODEL_ANIME_KW_WAITが割り当てられます)
}MODEL_ANIME;

///モデルのノードです。これ以外にも角とか尻尾とかもありますが、
///定数化(初期化時に高速アクセス設定をする)のは以下のものです。
///これ以外は文字列で個々にアクセスしてください。
///複数ある場合は基本的に右左か中右左の順になっています。
typedef enum
{
  MODEL_NODE_HEAD,      ///頭(0～複数
  MODEL_NODE_OVERHEAD,  ///頭の上(単一・必須)
  MODEL_NODE_EYE,       ///目(0～複数
  MODEL_NODE_MOUTH,     ///口(0～複数
  MODEL_NODE_HORN,      ///角(0～複数
  MODEL_NODE_CENTER,    ///中心(単一・必須)
  MODEL_NODE_FRONT,     ///前面(単一・必須)
  MODEL_NODE_HAND,      ///手(0～複数
  MODEL_NODE_TAIL,      ///尾(0～複数
  MODEL_NODE_FOOT,      ///足(0～複数
  MODEL_NODE_ATTACK01,  ///物理攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_ATTACK02,  ///物理攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_ATTACK03,  ///物理攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_ATTACK04,  ///物理攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_SHOOT01,   ///特殊攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_SHOOT02,   ///特殊攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_SHOOT03,   ///特殊攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_SHOOT04,   ///特殊攻撃点(対応するモーションがあれば1～複数
  MODEL_NODE_WIDEUSE,   ///その他用(0～複数
  MODEL_NODE_MAX,
}MODEL_NODE;
#define MODEL_NODE_NUM_MAX (8)

///目と口のパターン
typedef enum
{
  EYETYPE_OPEN = 0,    ///通常
  EYETYPE_HALFOPEN,    ///半目
  EYETYPE_CLOSE,       ///通常閉じ
  EYETYPE_CLOSE2,      ///ぎゅっと閉じる
  EYETYPE_ANGRY,       ///怒り
  EYETYPE_SMILE,       ///ニコ目
  EYETYPE_SAD,         ///悲しみ
  EYETYPE_OTHER,       ///その他
  EYETYPE_MAX,         ///目のパターンの総数

  MOUTHTYPE_CLOSE1 = 0,  ///閉じ口
  MOUTHTYPE_OPEN_S,      ///開き 小
  MOUTHTYPE_OPEN_L,      ///開き 大
  MOUTHTYPE_CLOSE2,      ///閉じ ニュートラル
  MOUTHTYPE_OPEN_TRI,    ///開き △
  MOUTHTYPE_CLOSE3,      ///閉じ 逆U字
  MOUTHTYPE_OTHER1,      ///その他
  MOUTHTYPE_OTHER2,      ///その他
  MOUTHTYPE_MAX,         ///口のパターンの総数
}FACE_TYPE;

typedef enum
{
  POKE_SIZE_S,
  POKE_SIZE_M,
  POKE_SIZE_L,
  POKE_SIZE_NUM,
}POKE_SIZE;

typedef enum
{
  POKE_ENV_NONE,    //朝昼
  POKE_ENV_EVENING, //夕
  POKE_ENV_NIGHT,   //夜
  POKE_ENV_CAVE,    //洞窟

  POKE_ENV_MAX,
}POKE_ENV_TYPE;

///設定ファイルのデータ(バイナリを直に入れるので並び注意)
struct PokeSettingData
{
  s32 cmHeight;
  s32 adjustHeight;
  s32 fieldAdjustHeight;
  u32 size;
  f32 minX;
  f32 minY;
  f32 minZ;
  f32 maxX;
  f32 maxY;
  f32 maxZ;

  f32 camSize;
  f32 camYOffset;
  f32 camRotate;
  f32 camRotateRightOfs;
  f32 camXRotate;

  s16 waitBRate;
  s16 waitCRate;

  // 目の使用フラグ(バイナリデータの関係でs16)
  s16 eyeUsedFlags[EYETYPE_MAX];

  // 目の使用フラグ(バイナリデータの関係でs16)
  s16 mouthUsedFlags[MOUTHTYPE_MAX];
};

static const int POKE_ANIME_SLOT_EYE_NUM = 3;
static const int POKE_ANIME_SLOT_MOUTH_NUM = 3;

class PokeModel :public poke_3d::model::PokemonModel
{
  // コピーコンストラクタ＋代入演算子禁止
  GFL_FORBID_COPY_AND_ASSIGN(PokeModel);
public:
  ///生成時オプション
  struct SetupOption
  {
    ///メインのヒープ
    gfl2::heap::HeapBase *dataHeap;
    ///ファイル非同期用の作業用ヒープ
    gfl2::heap::HeapBase *workHeap; //後方確保推奨
    ///VRAMアドレス
    void* vramAddr;

    MODEL_ANIMETYPE  animeType;

    //影の有無
    bool useShadow;
    //IDモデルの使用
    bool useIdModel;
    SetupOption()
      :dataHeap(NULL)
      ,workHeap(NULL)
      ,vramAddr(NULL)
      ,animeType(MODEL_ANIMETYPE_BATTLE)
      ,useShadow(false)
      ,useIdModel(false)
    {
    }
  };

  PokeModel();
  ~PokeModel();

  //生成・削除
  //----------------------------------------------------------------------------
  /**
   *  @brief  PokeModel生成
   *
   *  @param  const SimpleParam *param                    シンプルパラム
   *  @param  const PokeModel::SetupOption *opt = NULL    セットアップオプション
   *
   *  ※この関数は使わずPokeModelSysからのCreateModel○○関数を使ってください。
   */
  //-----------------------------------------------------------------------------
  void CreateSimple( PokeModelSystem *modelSys , const SimpleParam *paramTemp , const SetupOption &opt ,gfl2::heap::HeapBase *dataHeap , void *vramAddr);

  //----------------------------------------------------------------------------
  /**
   *  @brief  非同期読み完了待ち
   *
   *  @return bool 読み込みが完了したらtrue
   */
  //-----------------------------------------------------------------------------
  bool IsFinishLoad(void);

  //----------------------------------------------------------------------------
  /**
   *  @brief  非同期ロード後の生成処理
   *
   *  IsFinishLoadからtrueが帰ってきたら読んでください。
   */
  //-----------------------------------------------------------------------------
  void Create( void );

  //----------------------------------------------------------------------------
  /**
   *  @brief  削除チェック(どちらを呼んでも一緒です。
   */
  //-----------------------------------------------------------------------------
  bool CanDelete( void );           //開放がDeleteなので名前をあわせるよう
  virtual bool CanDestroy() const;  //継承用

  /**
   * @brief 解放チェック有効無効設定
   */
  virtual void SetDestroyCheckEnable(bool enable);

  //----------------------------------------------------------------------------
  /**
   *  @brief  削除
   */
  //-----------------------------------------------------------------------------
  void Delete( void );

  void Update(void);
  void UpdateAfter(void);

  void SetupColorShader(void);
  void CleanColorShader(void);  //※開放１フレ前に呼ぶこと！

  /**
   * @brief カラーシェーダの色の設定
   * @param col カラーシェーダで適用する色
   * @param executeFrame 実行するフレーム
   * @note executeFrameは基本1. 処理落ち発生の際に, 確実に実行してほしいフレーム数を指定する
   */
  void SetShaderColor(const gfl2::math::Vector4& col, s32 executeFrame = 1);
  const gfl2::math::Vector4 GetShaderColor(void)const{return mShaderColor;}

  //アニメーション関係
  // @value forceReset モーション番号が同じだった場合リセットをかけるか？
  void ChangeAnimation( MODEL_ANIME type , bool forceReset = false );       //推奨
  void ChangeAnimation( MODEL_ANIME_POKE_FINDER type , bool forceReset = false );       //推奨
  void ChangeAnimationDirect( int type , bool forceReset  = false ); //非推奨
  void ChangeAnimationSmooth( MODEL_ANIME type , int smoothFrame, bool forceReset = false );       //推奨
  void ChangeAnimationSmooth( MODEL_ANIME_POKE_FINDER type , int smoothFrame, bool forceReset = false );       //推奨
  //リソース渡し再生(全力技・デモ等外部でモーションを持つ場合)
  //戻す際は普通にChangeAnimationしてください。
  void ChangeAnimationResource( gfl2::renderingengine::scenegraph::resource::ResourceNode *anmRes,int aniSlot=0 );

  //ランダム再生時に再生して良いモーションか？(待機で始まり、待機で戻るか？)
  bool CheckCanOneshotMotion( MODEL_ANIME type );

  bool IsAvailableAnimation( MODEL_ANIME type );
  bool IsAvailableAnimation( MODEL_ANIME_POKE_FINDER type );
  bool IsAvailableAnimationDirect( int type );
  MODEL_ANIME GetAnimationType(void);
  MODEL_ANIME_POKE_FINDER GetAnimationType_PokeFinder(void);  //戻り値だけ違う関数は作れなかったので、これだけ名前分ける

  void SetAnimationIsLoop( bool flg );
  bool GetAnimationIsLoop(void);
 
  //内部で置き換えているのでここでMODEL_ANIMETYPE_APPは帰ってこない
  MODEL_ANIMETYPE GetModelAnimeType(void) const { return mSetupOpt.animeType; }

  bool IsEnable(void)const{return mIsEnable;}

  //エフェクトノードが有効かチェック
  bool IsEnableEffectNode(MODEL_NODE node , int idx );
  //エフェクトノードの座標を取得
  // @return 取得成功(失敗してもモデルの座標を返します。)
  bool GetEffectNodePosition(MODEL_NODE node , int idx , gfl2::math::Vector3 *ret );

  //@berif キーアニメーションのタイミング取得(バトルの攻撃タイミング先調べ用)
  //@param MODEL_ANIME type    モーション種類
  //@param const char* keyName キーの名前(AK_EffectStart01)
  //@return -1 で該当なし
  s32 GetKeyTiming( MODEL_ANIME type ,  const char* keyName );

  //gfl2_BaseModel.h より抜粋

  //SRT各種
  void SetPosition(const gfl2::math::Vector3& position){ mBasePosition = position; mIsUpdate = true;}
  void SetPosition(f32 positionX, f32 positionY, f32 positionZ){ mBasePosition.Set(positionX,positionY,positionZ);mIsUpdate = true;}
  const gfl2::math::Vector3& GetPosition() const{return mBasePosition;}
  
  //void SetRotation(const gfl2::math::Vector3& rotationRad, gfl2::math::EulerRotation::RotationOrder rotationOrder = gfl2::math::EulerRotation::RotationOrderZYX);
  //void SetRotation(f32 rotationRadX, f32 rotationRadY, f32 rotationRadZ, gfl2::math::EulerRotation::RotationOrder rotationOrder = gfl2::math::EulerRotation::RotationOrderZYX);
  //gfl2::math::Vector3 GetRotation() const;
  //AdjustHeightの関係上こちらで処理をする
  void SetScale(const gfl2::math::Vector3& scale){ mBaseScale = scale; mIsUpdate = true;}
  void SetScale(f32 scaleX, f32 scaleY, f32 scaleZ){ mBaseScale.Set(scaleX,scaleY,scaleZ);mIsUpdate = true;}
  const gfl2::math::Vector3& GetScale() const{return mBaseScale;}
  //1.0で1m
  void SetAdjustHeight( f32 height );
  //見た目の高さ
  const f32 GetAdjustHeight(void)const{return mAdjustHeight;}
  //設定上の高さ
  const f32 GetBaseHeight(void)const{return mBaseHeight;}
  
  const f32 GetAdjustScale(void)const{return mAdjustScale;}
 

  const gfl2::math::Vector3& GetOffset(void)const{return mOffset;}
  void SetOffset(const gfl2::math::Vector3& pos ){mOffset = pos;mIsUpdate = true;}
  void SetOffset(const f32 x,const f32 y,const f32 z){mOffset.Set(x,y,z);mIsUpdate = true;}

  gfl2::math::Vector3 GetMergePosition(void) const { gfl2::math::Vector3 pos=GetPosition(); return (pos+mOffset); }


  //表示切替(影の制御のため継承
  virtual void SetVisible(b32 flag);

  //Visible
  //void SetVisible(b32 flag);
  //b32 IsVisible() const;

  // モデルインスタンスノード取得
  //gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode()

  //複数体表示時のIDエッジのオフセット指定
  void SetStencilReferenceOffset(u8 ofs);

  //影モデル取得
  poke_3d::model::PokemonModel* GetShadowModel(void){return mShadowModel;}
  //歪み用モデル取得(NULLなら存在無し・実態はメインのモデルと同等
  poke_3d::model::PokemonModel* GetDistortionModel(void);

  //IDモデル取得
  bool IsAvailableIdModel(void);  //Create後に確認可能
  poke_3d::model::PokemonModel* GetIdModel(void){return mIdModel;}

  //フェイシャル制御(Idx=-1で全部)
  void ResetFaceAnime_Eye( int eyeIdx = -1 );
  void ResetFaceAnime_Mouth( int mouthIdx = -1 );
  void SetFaceAnime_Eye( int type , int eyeIdx = -1 , bool isBlink = false );
  void SetFaceAnime_Mouth( int type , int mouthIdx = -1 );
  //内部用
  void SetFaceAnime( int type , int slot , bool isBlink = false );
  void UpdateFaceAnime(void);

  void SetAutoBlink(bool flg);

  //振り向き制御が可能か(mcnfが無いと初期化失敗するので、落ちる
  bool IsEnableInterest() const{return (mMcnfResNode?true:false);}


  void SetEnvColor(POKE_ENV_TYPE type);

  //モーションを加味したボックスの取得
  bool GetMotionCollision( gfl2::math::AABB *box );
  bool GetMotionCollision( gfl2::math::AABB *box , MODEL_ANIME type );
  bool GetMotionCollisionDirect( gfl2::math::AABB *box , int type = -1);
  
  //サイズカテゴリの取得
  POKE_SIZE GetPokeSize(void)const;

  const PokeSettingData* GetSettingData(void){return &mPokeSettingData;}
  const void* GetFullPowerDisableMaterialData(int *retSize);

  //特殊フォルムチェンジ
  void NoLoadFormChange(pml::FormNo formNo);

  //開放時のHeap比較用
  gfl2::heap::HeapBase* GetDataHeap(void)const{return mSetupOpt.dataHeap;}

  // --------------------------------------------------------------------------
  /**
   * @brief  メガゲンガーシェーダー照り返し部分の開始位置設定
   * @param  startY       メガゲンガーシェーダー照り返し部分の開始位置（ワールド座標Ｙ）
   */
  // --------------------------------------------------------------------------
  void SetMegaGangarEffectState( f32 startY );

  // 表情補間の有効無効を設定
  // @note サクラビスやテールナーなど表情補間に問題があるポケモンは関数内で強制的に補間を無効にされる
  void SetFaceAnimeSmoothEnable(bool enable);

  // 目の状態の取得
  int GetEyeState(u32 slot);
  // 口の状態の取得
  int GetMouseState(u32 slot);

  /// @brief ループアニメーションフレームのセット
  /// @note POKE_ANIME_SLOT_LOOP1のフレームを変更するときはこの関数を使用する
  void SetLoop1AnimationFrame(f32 frame);

private:
  void DataLoadFunc( void **buf , int dataIdx );

  void CreateMcnf(void);
  void CreateModel(void);
  void CreateAnime(bool createEffNode = true);
  void CreateIdModel(void);

  /**
   * @brief アニメーション関連のステータスのリセット
   * @note アニメーション最初の設定時, 切替時にコールバック設定とフラグリセット
   */
  void ResetAnimationStatus();

  /**
   * @brief アニメーションの切替の必要があるかの判定
   * @param type アニメーションの種類
   * @param forceReset 力のリセットの必要あり/なし
   * @return true/false 変更の必要あり/なし
   */
  b32 IsNecessaryToChangeAnimation(int type, bool forceReset);

  //自動目パチ
  void UpdateBlink(void);

  /**
   * @brief ループモーションのコールバック制御の更新
   */
  void UpdateKeyAnimationCallBack();

  ///読み込んだBINファイルの種類
  enum
  {
    BINPACK_COMMON,   ///共通(モデル  )
    BINPACK_TEXTURE,  ///テクスチャ
    BINPACK_ID,       ///IDテクスチャ
    BINPACK_ANIME,    ///モーション
    BINPACK_SETTING,  ///設定データ

    BINPACK_ANIME_EX, ///ギルガルドとかフォルムチェンジ用

    BINPACK_MAX
  };

  PokeModelSystem *mPokeModelSys;

  SetupOption mSetupOpt;
  SimpleParam mSimpleParam;
  gfl2::util::GLHeapAllocator *mGLAllocator;

  void *mDataBuf[BINPACK_MAX];
  gfl2::fs::BinLinkerAccessor mBinData[BINPACK_MAX];

  int  mAsyncBufNum;
  void **mAsyncBuf[BINPACK_MAX];

  gfl2::renderingengine::scenegraph::resource::ResourceNode *mModelResNode;
  
  //IDモデル
  poke_3d::model::PokemonModel *mIdModel;
  int mIdTexNum;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mIdModelResNode;
  gfl2::renderingengine::scenegraph::resource::TextureResourceNode **mIdTexResNode;

  //影モデル関係
  poke_3d::model::PokemonModel *mShadowModel;
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mShadowModelResNode;
  s32* mShadowMaterialIndexList;

  //mcnfファイル
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mMcnfResNode;

  //アニメーション関係
  int mAnimeNum;
  gfl2::renderingengine::scenegraph::resource::ResourceNode **mAnimeResNode;
  bool mIsPlayAnime;
  bool mIsLoopAnime;
  bool mIsFaceAnimSmoothEnable;
  int mPlayAnimeType;

  //フェイシャル、ループの数
  static const int POKE_OVERRIDE_ANIME_NUM = gfl2::animation::OverridePartsType::NumberOf;
  static const int POKE_ANIME_PATTERN_NUM = poke_3d::model::PokemonModel::LOOP_ANIMATION_BIT_PATTERN_NUM;
  //フェイシャル・ループ制御
  gfl2::renderingengine::scenegraph::resource::ResourceNode *mOverrideAnimeResNode[POKE_OVERRIDE_ANIME_NUM];

  /**
   * 各種ループアニメーションの再生フレーム
   * 再生フレームを計算するのは, コールバックで返ってくるAK_PartsSkel01, AK_PartsMaterial01, AK_PartsVisible01の3つが対象
   * サウンドのキーに関してはGFModelViewerで使用しているが, バトルはコールバックを使用せずにサウンドの再生フレームを計算している
   */
  f32 mPartsLoopAnimeFrame[POKE_ANIME_PATTERN_NUM];

  /**
   * 置換対象となるアニメーションの総数
   * ループアニメはPokemonModelで管理しているのでループアニメを除外して計上
   */
  static const int POKE_REPLACE_ANIM_NUM = gfl2::animation::OverridePartsType::Loop01;
  PokeAnimReplace* mpFaceReplaceList[POKE_REPLACE_ANIM_NUM];
  bool mActiveEyeAnime[POKE_ANIME_SLOT_EYE_NUM]; //目パチ抑制用
  //自動目パチ
  bool mBlinkEnable;
  int  mBlinkCnt;
  u8   mBlinkState;
  // 目・口の状態
  int mEyeMouseState[POKE_ANIME_SLOT_EYE_NUM + POKE_ANIME_SLOT_MOUTH_NUM];

  EffectNodeData *mEffectNodeData;
  int            mEffectNodeDataNum;
  gfl2::renderingengine::scenegraph::instance::LocatorInstanceNode *mEffectNode[MODEL_NODE_MAX][MODEL_NODE_NUM_MAX];

  gfl2::math::Vector4 mShaderColor;

  PokeSettingData mPokeSettingData;
  bool mIsEnable;

  gfl2::util::FixedSizeContainer<gfl2::renderingengine::renderer::util::ColorShaderTextureRenderPath*,poke_3d::model::PokemonModel::COLORTEXTURE_MAX> mPokeShaderRenderPaths;

  //とりあえずはポジション、スケールの更新にのみ使用
  bool mIsUpdate;
  gfl2::math::Vector3 mBaseScale;
  f32 mAdjustScale;

  gfl2::math::Vector3 mBasePosition;
  gfl2::math::Vector3 mOffset;

  f32 mBaseHeight;
  f32 mAdjustHeight;
};

GFL_NAMESPACE_END(PokeTool)


#endif //__POKE_MODEL_H__
