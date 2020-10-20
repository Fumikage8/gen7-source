//======================================================================
/**
 * @file FinderManager.h
 * @date 2016/02/01 11:33:04
 * @author miyachi_soichi
 * @brief ファインダー管理者
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __FINDER_MANAGER_H_INCLUDED__
#define __FINDER_MANAGER_H_INCLUDED__
#pragma once
// gfl2
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
// GameSys
#include "GameSys/include/GameManager.h"
// UI
#include "App/Finder/include/FinderAppParam.h"
// Param
#include "Field/FieldRo/include/TrialModel/FieldFinderModel_define.h"
#include "App/FinderMenu/include/FinderMenuAppParam.h"

// 前方宣言
GFL_NAMESPACE_BEGIN(System)
class nijiAllocator;
GFL_NAMESPACE_END(System)
GFL_NAMESPACE_BEGIN(PokeTool)
class KawaigariParamOptionCoreManager;
GFL_NAMESPACE_END(PokeTool)
GFL_NAMESPACE_BEGIN(App)
GFL_NAMESPACE_BEGIN(Finder)
class FinderProc;
GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(App)
GFL_NAMESPACE_BEGIN(Field)
class FieldPokeFinderActor;
GFL_NAMESPACE_BEGIN(Camera)
class CameraManager;
GFL_NAMESPACE_BEGIN(Controller)
class ControllerTypePokeFinderForPlacement;
GFL_NAMESPACE_END(Controller)
GFL_NAMESPACE_END(Camera)
GFL_NAMESPACE_BEGIN(TrialModel)
class TrialModelFactory;
class FinderModel;
GFL_NAMESPACE_END(TrialModel)
GFL_NAMESPACE_BEGIN(weather)
class WeatherControl;
GFL_NAMESPACE_END(weather)
#if PM_DEBUG
GFL_NAMESPACE_BEGIN(Debug)
class DebugPokeFinder;
GFL_NAMESPACE_END(Debug)
GFL_NAMESPACE_BEGIN(Finder)
class DebugInfo;
GFL_NAMESPACE_END(Finder)
class DebugInfoWin;
#endif
GFL_NAMESPACE_END(Field)

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Finder)

/**
 *  @class  Manager
 *  @brief  ファインダー管理者
 */
class Manager
{
  GFL_FORBID_COPY_AND_ASSIGN(Manager);

public:
  /**
   *  @brief  シチュエーション種別
   */
  enum SITUATION
  {
    SITUATION_SINGLE,
    SITUATION_DOUBLE,
    SITUATION_OTHER,
  };
  /**
   *  @brief  コメント用方向
   */
  enum ROT_DIR
  {
    ROT_DIR_FRONT,
    ROT_DIR_SIDE,
    ROT_DIR_BACK,
  };
  /**
   *  @brief  終わり方
   */
  enum EndResult
  {
    EndResult_None,     //!< 終了していない
    EndResult_Cancel,   //!< キャンセル
    EndResult_Over,     //!< ６まい
    EndResult_Trial,    //!< 試練達成
  };
  /**
   *  @brief  初期化構造体
   */
  struct Desc
  {
    gfl2::heap::HeapBase        *pHeap;
    gfl2::heap::HeapBase        *pHeap_FileRead;
    gfl2::heap::HeapBase        *pHeap_Dof;
    App::FinderMenu::APP_PARAM  *pFinderMenuParam;
    u32                         nSpotID;
    s32                         nSubjectID;
    s32                         nTrialMonsNo;

#if PM_DEBUG
    Debug::DebugPokeFinder      *pDebug;
    Field::DebugInfoWin         *pDebugInfoWin;
#endif
  };
  /**
   *  撮影時の状況
   */
  struct SituationData
  {
    u32                 score;
    bool                inScreen[Field::TrialModel::FINDER_POKE_MODEL_MAX];
    f32                 scr_score[Field::TrialModel::FINDER_POKE_MODEL_MAX];
    f32                 scr_out[Field::TrialModel::FINDER_POKE_MODEL_MAX];
    f32                 rot[Field::TrialModel::FINDER_POKE_MODEL_MAX];  //!< 向き
    u32                 eRot[Field::TrialModel::FINDER_POKE_MODEL_MAX];
    gfl2::math::Vector2 min[Field::TrialModel::FINDER_POKE_MODEL_MAX];
    gfl2::math::Vector2 max[Field::TrialModel::FINDER_POKE_MODEL_MAX];
  };
  /**
   *  コメントソート用
   */
  struct CommentData
  {
    u32 score;
    s32 msgID;
    s32 priority;
  };
public:
  /**
   *  @brief  コンストラクタ
   */
  Manager( void );

  /**
   *  @brief  デストラクタ
   */
  ~Manager( void ){}

  /**
   *  @brief  初期化
   */
  void Initialize( const Desc &desc );

  /**
   *  @brief  初期化完了待ち
   */
  bool IsInitialized( void );

  /**
   *  @brief  終了
   */
  void Finalize( void );

  /**
   *  @brief  終了完了待ち
   */
  bool IsFinalized( void );

  /**
   *  @brief  実行
   */
  void Execute( void );

  /**
   *  @brief  更新処理
   */
  void Update( GameSys::GameManager *pGameManager );

  /**
   *  @brief  終了状態の取得
   */
  EndResult GetEndResult( void ){ return m_eResult; }

private:
  /**
   *  @brief  初期化更新
   */
  void update_Initialize( GameSys::GameManager *pGameManager );

  /**
   *  @brief  更新
   */
  void update_Execute( GameSys::GameManager *pGameManager );

  /**
   *  @brief  終了更新
   */
  void update_Finalize( GameSys::GameManager *pGameManager );

  /**
   *  @brief  garcOpen
   */
  void openFile( GameSys::GameManager *pGameManager );

  /**
   *  @brief  garcOpenWait
   */
  bool isFinishOpenFile( GameSys::GameManager *pGameManager );

  /**
   *  @brief  garcLoad
   */
  void loadFile( GameSys::GameManager *pGameManager );

  /**
   *  @brief  garcLoadWait
   */
  bool isFinishLoadFile( GameSys::GameManager *pGameManager );

  /**
   *  @brief  garcClose
   */
  void closeFile( GameSys::GameManager *pGameManager );

  /**
   *  @brief  garcCloseWait
   */
  bool isFinishCloseFile( GameSys::GameManager *pGameManager );

  /**
   *  @brief  カワイガリデータ作成
   */
  void createKawaigari( GameSys::GameManager *pGameManager );

  /**
   *  @brief  カワイガリデータ作成待ち
   */
  bool isLoadedKawaigari( GameSys::GameManager *pGameManager );

  /**
   *  @brief  データ作成
   */
  void createData( GameSys::GameManager *pGameManager );

  /**
   *  @brief  予約判定
   */
  bool checkSpotReserve( GameSys::GameManager *pGameManager );

  /**
   *  @brief  配列番号からデータを取得する
   */
  bool setupSpotModelData( u32 instance_index, s32 data_index );

  /**
   *  @brief  抽選処理
   */
  void execLottery( GameSys::GameManager *pGameManager );
  
  /**
   *  @brief  カワイガリデータ破棄
   */
  void deleteKawaigari( GameSys::GameManager *pGameManager );

  /**
   *  @brief  セーブデータからデータを受け取る
   */
  void setDataFromSavedata( GameSys::GameManager *pGameManager );

  /**
   *  @brief  カラーバッファ作成
   */
  void createColorBuffer( void );

  /**
   *  @brief  メニュー作成
   */
  void createMenu( GameSys::GameManager *pGameManager );

  /**
   *  @brief  メニュー作成待ち
   */
  bool isCreateMenu( GameSys::GameManager *pGameManager );

  /**
   *  @brief  ポケモンモデルシステムの作成
   */
  void createPokeSystem( GameSys::GameManager *pGameManager );

  /**
   *  @brief  ポケモンモデルを生成する
   */
  void createPokeModel( GameSys::GameManager *pGameManager );

  /**
   *  @brief  ポケモンモデル作成待ち
   */
  bool isCreatePokeModel( void );

  /**
   *  @brief  ポケモンモデルセットアップ
   */
  void setupPokeModel( void );

  /**
   *  @brief  描画変更
   */
  void startGraphic( GameSys::GameManager *pGameManager );

  /**
   *  @brief  ポケモンの更新
   */
  void updatePokeModel( void );

  /**
   *  @brief  メニューパラメータの更新
   */
  void updateMenu( void );

  /**
   *  @brief  ポケモンを呼ぶ
   */
  void requestPokeCall( void );

  /**
   *  @brief  ピントの更新
   */
  void updateFocus( GameSys::GameManager *pGameManager );

  /**
   *  @brief  BGMの更新
   */
  void updateBgmVolume( GameSys::GameManager *pGameManager );

  /**
   *  @brief  スナップショット準備
   */
  void setupSafeSnapshot( void );

  /**
   *  @brief  安全に撮影できるか判定
   */
  bool isSafeSnapshot( void );

  /**
   *  @brief  スナップショットのリクエスト
   */
  void requestSnapshot( GameSys::GameManager *pGameManager );

  /**
   *  @brief  スナップショットの完了待ち
   */
  bool isFinishSnapshot( void );

  /**
   *  @brief  Jpeg作成
   */
  void createJpeg( GameSys::GameManager *pGameManager );

  /**
   *  @brief  評価
   */
  void calcValue( GameSys::GameManager *pGameManager );

  /**
   *  @brief  基礎計算式：ポケモン
   *  @param  idx   計算するポケモンの配列番号
   */
  u32 calcBaseScore( u32 idx );

  /**
   *  @brief  状況を設定
   */
  bool calcSituation( u32 idx );

  /**
   *  @brief  画面占有率の計算
   */
  f32 calcPokeScreen( u32 idx );

  /**
   *  @brief  はみ出具合の計算
   */
  f32 calcPokeOut( u32 idx );

  /**
   *  @brief  シチュエーション：単体
   *  @param  idx     計算するポケモンの配列番号
   *  @param  pValue  コメント格納先
   */
  u32 calcSituationSingleScore( u32 idx, Field::TrialModel::FinderPhotoValue *pValue );

  /**
   *  @brief  シチュエーション：複合
   *  @param  pValue  コメント格納先
   */
  u32 calcSituationDoubleScore( Field::TrialModel::FinderPhotoValue *pValue );

  /**
   *  @brief  シチュエーション：その他
   *  @param  pValue  コメント格納先
   */
  u32 calcSituationOtherScore( Field::TrialModel::FinderPhotoValue *pValue );

  /**
   *  @brief  コメントデータの追加
   */
  u32 addComment( u32 sit_type, u32 idx, bool bLimit );
  void addComment( CommentData &data, bool bLimit );

  /**
   *  @brief  終了チェック
   */
  void checkEnd( void );

  /**
   *  @brief  ポケモンの動きを再開する
   */
  void requestResume( void );

  /**
   *  @brief  BGMボリュームのリセット
   */
  void resetBgmVolume( GameSys::GameManager *pGameManager );

  /**
   *  @brief  描画変更
   */
  void endGraphic1( GameSys::GameManager *pGameManager );
  void endGraphic2( GameSys::GameManager *pGameManager );

  /**
   *  @brief  ポケモンモデル破棄準備
   */
  void requestDeletePokeModel( void );

  /**
   *  @brief  ポケモンモデル破棄判定
   *  @retval true  安全に削除できる
   *  @retval false まだ待つ必要がある
   */
  bool isSafeDeletePokeModel( void );

  /**
   *  @brief  ポケモンモデル破棄
   */
  void deletePokeModel( void );

  /**
   *  @brief  ポケモンモデルシステム破棄
   */
  void deletePokeSystem( void );

  /**
   *  @brief  メニュー破棄
   */
  void deleteMenu( GameSys::GameManager *pGameManager );

  /**
   *  @brief  メニュー破棄待ち
   */
  bool isDeleteMenu( GameSys::GameManager *pGameManager );

  /**
   *  @brief  カラーバッファ破棄
   */
  void deleteColorBuffer( void );

  /**
   *  @brief  ファイル破棄
   */
  void deleteFile( void );

private:
  //! 状況用Enum
  enum State
  {
    State_Wait,
    State_Initialize,
    State_Execute,
    State_Finalize,
  };
  typedef Field::Camera::Controller::ControllerTypePokeFinderForPlacement FinderCamera;
  typedef PokeTool::KawaigariParamOptionCoreManager KawaigariManager;

  gfl2::heap::HeapBase                  *m_pHeap;
  gfl2::heap::HeapBase                  *m_pHeap_FileRead;
  gfl2::heap::HeapBase                  *m_pPokeHeap[Field::TrialModel::FINDER_POKE_MODEL_MAX];
  gfl2::heap::HeapBase                  *m_pDofHeap;
  System::nijiAllocator                 *m_pAllocator;
  App::Finder::FinderProc               *m_pUIProc;
  Field::Camera::CameraManager          *m_pCameraManager;
  FinderCamera                          *m_pCamera;
  Field::TrialModel::TrialModelFactory  *m_pModelFactory;
  Field::TrialModel::FinderModel        *m_pModel[Field::TrialModel::FINDER_POKE_MODEL_MAX];
  Field::weather::WeatherControl        *m_pWeather;
  KawaigariManager                      *m_pKawaigari;
  App::FinderMenu::APP_PARAM            *m_pFinderMenuParam;
  void                                  *m_pPokeData;
  void                                  *m_pSubjectData;
  void                                  *m_pSituationSingle;
  void                                  *m_pSituationDouble;
  void                                  *m_pSituationOther;
  void                                  *m_pRenderPath;
  void                                  *m_pColorBuffer;

  App::Finder::APP_PARAM                m_rUIParam;
  Field::TrialModel::FinderSpotData     m_rSpotData;
  Field::TrialModel::FinderPathData     m_rPathData[Field::TrialModel::FINDER_PATH_MAX];
  Field::TrialModel::FinderStatueData   m_rStatueData;
  Field::TrialModel::FinderOnewayData   m_rOnewayData;
  Field::TrialModel::FinderModelData    m_rModelData[Field::TrialModel::FINDER_POKE_MODEL_MAX];
  SituationData                         m_rSituation;
  CommentData                           m_rComment[Field::TrialModel::FINDER_VALUE_COMMENT_MAX];

  GameSys::GameManager::SkipDrawMode    m_TmpSkipDrawMode;
  State                                 m_eState;
  EndResult                             m_eResult;
  u32                                   m_nStep;
  u32                                   m_nSpotID;
  s32                                   m_nSubjectID;
  u32                                   m_nSnapCount;
  u32                                   m_nCallReload;
  u32                                   m_nDelayFrame;
  s32                                   m_TrialMonsNo;
  u32                                   m_Focus_Old;
  bool                                  m_bInitialized;
  bool                                  m_bFinalized;
  bool                                  m_HasPathA;
  bool                                  m_HasPathB;
  bool                                  m_HasStatue;
  bool                                  m_HasOneway;
  bool                                  m_IsNoon;
  bool                                  m_IsSnapshot;
  bool                                  m_IsVerLimit;  // バージョンによるコメント制限

#if PM_DEBUG
  Debug::DebugPokeFinder                *m_pDebug;
  Field::DebugInfoWin                   *m_pDebugInfoWin;
  Field::Finder::DebugInfo              *m_pFinderDebugInfo;
  bool                                  m_IsDebugMode;
#endif
};

GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(Field)

#endif // __FINDER_MANAGER_H_INCLUDED__
