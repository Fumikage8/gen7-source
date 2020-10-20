#if !defined(SYSTEM_DRESSUP_H)
#define SYSTEM_DRESSUP_H

#include <types/include/gfl2_Typedef.h>
#include <model/include/gfl2_DressUpParam.h>
#include <model/include/gfl2_CharaModelFactory.h>
#include <math/include/gfl2_math_random.h>

// 着せ替えのヘッダーは全てbattleの方を使う
#include <niji_conv_header/battle/dressup/dressup_color_id.h>

#include <niji_conv_header/battle/dressup/dressup_hero_acceye_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_bag_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_bngl_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_bottoms_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_face_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_hair_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_hat_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_legs_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_shoes_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_tops_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_hero_situ_item_id.h>

#include <niji_conv_header/battle/dressup/dressup_heroine_acceye_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_acchair_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_bag_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_bngl_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_bottoms_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_face_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_hair_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_hat_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_legs_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_shoes_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_tops_item_id.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_situ_item_id.h>

#include <niji_conv_header/battle/dressup/dressup_hero_acceye_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_bag_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_bngl_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_bottoms_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_face_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_hair_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_hat_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_legs_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_shoes_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_tops_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_hero_situ_item_id_start_end.h>

#include <niji_conv_header/battle/dressup/dressup_heroine_acceye_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_acchair_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_bag_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_bngl_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_bottoms_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_face_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_hair_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_hat_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_legs_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_shoes_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_tops_item_id_start_end.h>
#include <niji_conv_header/battle/dressup/dressup_heroine_situ_item_id_start_end.h>

namespace System {
  
/**
 * @brief 着せ替え系の共通処理
 */
class DressUp
{
public:

  // バトルヒープサイズ定義
  static const s32 BATTLE_HEAP_SIZE_ANIM      = 0x00030000; // アニメーション用ヒープサイズ：バトル
  static const s32 BATTLE_HEAP_SIZE_MODEL     = 0x001B0000; // モデル用ヒープサイズ：バトル
  static const s32 BATTLE_HEAP_SIZE_INSTANCE  = 0x00030000; // インスタンス用ヒープサイズ：バトル
  static const s32 BATTLE_HEAP_SIZE_ALLOCATOR = 0x00040000; // NijiAllocator用ヒープサイズ：バトル
  // 17/02/28 計測
  // animHeap      : 0x000178D8
  // modelHeap     : 0x00176EC8
  // instanceHeap  : 0x00014CCC
  // allocatorHeap : 0x00029358

  // フィールドヒープサイズ定義
  static const s32 FIELD_HEAP_SIZE_ANIM       = 0x001C0000; // アニメーション用ヒープサイズ：フィールド
  static const s32 FIELD_HEAP_SIZE_MODEL      = 0x00140000; // モデル用ヒープサイズ：フィールド
  static const s32 FIELD_HEAP_SIZE_INSTANCE   = 0x00040000; // インスタンス用ヒープサイズ：フィールド
  static const s32 FIELD_HEAP_SIZE_ALLOCATOR  = 0x00040000; // NijiAllocator用ヒープサイズ：フィールド
  // 17/02/28 計測
  // animHeap      : 0x00165F5C
  // modelHeap     : 0x000DE06C
  // instanceHeap  : 0x00025F4C
  // allocatorHeap : 0x0002E65C

  /*
    ■ HEAP_SIZE_ANIM を必要とする関数
    CharaModelFactory::LoadModelAsync()

    ■ HEAP_SIZE_MODEL を必要とする関数
    DressUpModelResourceManager::Initialize()

    ■ HEAP_SIZE_INSTANCE を必要とする関数
    CharaModelFactory::CreateDressUpModel()

    ■ HEAP_SIZE_ALLOCATOR を必要とする関数
    CharaModelFactory::SetupModel()
    CharaModelFactory::SetupDynamicAnimation()
    CharaModelFactory::CreateDressUpModel()
    DressUpModelResourceManager::SetupDressUpParts()
    DressUpModelResourceManager::SetupDressUpDynamicAnimation()
  */

public:

  /**
   * @brief バトル用のArcIdリストを取得
   */
  static s32* GetDressUpArcIdListBattle();

  /**
   * @brief フィールド用のArcIdリストを取得
   */
  static s32* GetDressUpArcIdListField();

  /**
   * @brief 男主人公のデフォルトの着せ替えパラメータ取得
   */
  static void GetDressUpParamDefaultHero(poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief 女主人公のデフォルトの着せ替えパラメータ取得
   */
  static void GetDressUpParamDefaultHeroine(poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief デフォルトの着せ替えパラメータ取得：性別は引数で受け取る
   */
  static void GetDressUpParamDefault(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   *  @brief 男主人公　ライドスーツの着せ替えパラメータ取得
   *  @param  is_land   true:陸用  false:海用
   */
  static void GetDressUpParamRideHero(bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   *  @brief 女主人公　ライドスーツの着せ替えパラメータ取得
   *  @param  is_land   true:陸用  false:海用
   */
  static void GetDressUpParamRideHeroine(bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   *  @brief 男主人公　宇宙服の着せ替えパラメータ取得
   */
  static void GetDressUpParamSpaceHero(poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   *  @brief 女主人公　宇宙服の着せ替えパラメータ取得
   */
  static void GetDressUpParamSpaceHeroine(poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief ライドスーツの着せ替えパラメータ取得：性別は引数で受け取る
   *  @param  is_land   true:陸用  false:海用
   */
  static void GetDressUpParamRide(u32 sex, bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief ライドスーツの着せ替えパラメータ取得：性別は引数で受け取る
   */
  static void GetDressUpParamSpace(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief ライドスーツの着せ替えパラメータセットアップ：性別は引数で受け取る、Face以外
   *  @param  is_land   true:陸用  false:海用
   */
  static void OverwrideDressUpParamRide(u32 sex, bool is_land, poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief 宇宙服の着せ替えパラメータセットアップ：性別は引数で受け取る、Face以外
   */
  static void OverwrideDressUpParamSpace(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief 男主人公のデフォルトの着せ替えパラメータ取得(きせかえではずすことができるのはすべて-1にしてきせかえではずすことができない値だけ残す)
   */
  static void GetDressUpParamDefaultIndispensableHero(poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief 女主人公のデフォルトの着せ替えパラメータ取得(きせかえではずすことができるのはすべて-1にしてきせかえではずすことができない値だけ残す)
   */
  static void GetDressUpParamDefaultIndispensableHeroine(poke_3d::model::DressUpParam* out_pDressUpParam);

  /**
   * @brief デフォルトの着せ替えパラメータ取得(きせかえではずすことができるのはすべて-1にしてきせかえではずすことができない値だけ残す)
   */
  static void GetDressUpParamDefaultIndispensable(u32 sex, poke_3d::model::DressUpParam* out_pDressUpParam);

#if PM_DEBUG
  /**
   * @brief ランダムで着せ替えパラメータ取得：性別は引数で受け取る
   */
  static void Debug_GetDressUpParamRandom(u32 sex, const poke_3d::model::DressUpModelResourceManagerCore* pDressUpModelResourceManager, poke_3d::model::DressUpParam* out_pDressUpParam, gfl2::math::SFMTRandom* pSFMTRandom = NULL);

  /**
  * @brief 処理負荷の高い着せ替えパラメータ取得（女主人公）
  */
  static void Debug_GetDressUpParamTopPfm(poke_3d::model::DressUpParam* out_pDressUpParam);

#endif
};

// セーブデータのビット
#define DRESSUP_PARAM_SAVE_DATA_BIT_VERSION 2                                                // バージョン情報

#define DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_SEX 1 //BODY_CATEGORY_SEX,                    // 性別
#define DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_BODY_COLOR 5 //BODY_CATEGORY_BODY_COLOR,      // 体の色
#define DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_EYE_COLOR 5 //BODY_CATEGORY_EYE_COLOR,        // 目の色
#define DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_HAIR_COLOR 5 //BODY_CATEGORY_HAIR_COLOR,      // 髪の色
#define DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_LIP_COLOR 5//BODY_CATEGORY_FEMALE_LIP_COLOR,  // リップの色

#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAIR 6 //ITEM_CATEGORY_HAIR_STYLE,            // 髪型
#define DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_00 3 // パディング00
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCEYE 9 //ITEM_CATEGORY_ACCEYE,              // アイアクセサリ
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCHAIR 9 //ITEM_CATEGORY_ACCHAIR,            // ヘアアクセサリ
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BAG 10 //ITEM_CATEGORY_BAG,                    // バッグ
#define DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_01 4 // パディング01
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BOTTOMS 10 //ITEM_CATEGORY_BOTTOMS,            // ボトムス
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAT 10 //ITEM_CATEGORY_HAT,                    // 帽子
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_LEGS 10 //ITEM_CATEGORY_LEGS,                  // レッグ
#define DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_02 2 // パディング02
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_SHOES 10 //ITEM_CATEGORY_SHOES,                // シューズ
#define DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_TOPS 10 //ITEM_CATEGORY_TOPS,                  // トップス
#define DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_03 8 // パディング03

#define DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_ZRING 1 // hasZRing,                           // Zリングを所持している
#define DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_BAGCHARM 1 // hasBagCharm,                     // 島巡りの証を所持している
#define DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_NIJI 1 // isRomVersionNiji,                    // ROMバージョンがnijiか。MMCat[337]対処用
#define DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_04 1 // パディング04

/**
 * @brief 着せ替え用パラメータ：セーブ用
 */
struct DressUpParamSaveData
{
  enum DressUpVersion
  {
    DRESSUP_VERSION_NIJI,                                                                    // 着せ替えバージョン：niji
    DRESSUP_VERSION_RESERVED_0,                                                              // 着せ替えバージョン：予約０
    DRESSUP_VERSION_RESERVED_1,                                                              // 着せ替えバージョン：予約１
    DRESSUP_VERSION_RESERVED_2,                                                              // 着せ替えバージョン：予約２

    DRESSUP_VERSION_COUNT                                                                    // 着せ替えバージョン：総数
  };

#if PM_DEBUG
  static u32 s_debugVersion;                                                                 // デバッグ用のセーブデータバージョン
#endif

  u32 version : DRESSUP_PARAM_SAVE_DATA_BIT_VERSION;                                         // バージョン情報

  u32 bodyParamSex : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_SEX;                             // 性別
  u32 bodyParamBodyColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_BODY_COLOR;                // 体の色
  u32 bodyParamEyeColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_EYE_COLOR;                  // 目の色
  u32 bodyParamHairColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_HAIR_COLOR;                // 髪の色
  u32 bodyParamLipColor : DRESSUP_PARAM_SAVE_DATA_BIT_BODY_PARAM_LIP_COLOR;                  // リップの色

  u32 itemParamHair : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAIR;                           // 髪型
  u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_00; // パディング00
  u32 itemParamAccEye : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCEYE;                       // アイアクセサリ
  u32 itemParamAccHair : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_ACCHAIR;                     // ヘアアクセサリ
  u32 itemParamBag : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BAG;                             // バッグ
  u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_01; // パディング01
  u32 itemParamBottoms : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_BOTTOMS;                     // ボトムス
  u32 itemParamHat : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_HAT;                             // 帽子
  u32 itemParamLegs : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_LEGS;                           // レッグ
  u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_02; // パディング02
  u32 itemParamShoes : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_SHOES;                         // シューズ
  u32 itemParamTops : DRESSUP_PARAM_SAVE_DATA_BIT_ITEM_PARAM_TOPS;                           // トップス
  u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_03; // パディング03

  u32 hasZRing : DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_ZRING;                               // Zリングを所持している
  u32 hasBagCharm : DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_BAGCHARM;                         // 島巡りの証を所持している
  u32 isRomVersionNiji : DRESSUP_PARAM_SAVE_DATA_BIT_FLAG_PARAM_NIJI;                        // ROMバージョンがnijiか。MMCat[337]対処用
  u32 : DRESSUP_PARAM_SAVE_DATA_BIT_PADDING_04; // パディング04

  // コンストラクタ
  DressUpParamSaveData();

  // デストラクタ
  ~DressUpParamSaveData();

  // 着せ替え用パラメータから変換
  static void ConvertFromDressUpParam(DressUpParamSaveData* out_pSaveData, const poke_3d::model::DressUpParam& dressUpParam);

  // 着せ替え用パラメータに変換
  static void ConvertToDressUpParam(poke_3d::model::DressUpParam* out_pDressUpParam, const DressUpParamSaveData& saveData);

  // バングルをnijiのものに上書き(MMCat[337]用に作成）
  static void OverrideNijiBngl(poke_3d::model::DressUpParam* out_pDressUpParam);
};


}

#endif
