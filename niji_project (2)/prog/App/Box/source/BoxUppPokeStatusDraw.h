//======================================================================
/**
 * @file BoxUppPokeStatusDraw.h
 * @date 2015/07/28 12:47:37
 * @author uchida_yuto
 * @brief Box上画面のポケモンステータス表示View
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BOX_UPP_POKE_STATUS_DRAW_H_INCLUDED__
#define __BOX_UPP_POKE_STATUS_DRAW_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)

  class PokeModelSystem;
  class ExtendData;

GFL_NAMESPACE_END(PokeTool)

GFL_NAMESPACE_BEGIN(poke_3d)
GFL_NAMESPACE_BEGIN(model)

    class BaseCamera;

GFL_NAMESPACE_END(model)
GFL_NAMESPACE_END(poke_3d)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppRenderingManager;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  class PokeSimpleModelInFrame;
  class AppCommonGrpIconData;
  class ItemIconToolEx;
  class BoxRaderChart;

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(Savedata)

  class ZukanData;

GFL_NAMESPACE_END(Savedata)

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

  /**
   * @class BoxUppPokeStatusDraw
   * @brief Boxアプリの上画面に表示するポケモンステータスView
   * @note
   *     使い方
   *        使用初回準備
   *          1: app::util::AppRenderingManagerのインスタンスを生成(上画面にレイアウト+ポケモンモデル+レーダーチャートを描画できるようにする)
   *             レンダー設定に app::util::AppRenderingManager::RenderOrderType::LAYOUT_RADERCHART_POKEMODEL をしてください。
   *          2: app::util::AppRenderingManagerのインスタンスに上画面のカメラ設定をする
   *          3: 数字フォントを生成
   *          4: 状態異常などアイコンを差し替える app::tool::AppCommonGrpIconDataのインスタンスを生成
   *          4: Box用のMsgDataのインスタンスを生成
   *          5: Boxのレイアウトバイナリデータをロードする
   *          6: BoxUppPokeStatusDrawのインスタンスを生成してコンストラクタに必要なパラメータを指定
   *          7: BoxUppPokeStatusDraw::InitFunc()関数を呼ぶ。
   *             戻り値がtrueになるまでBoxUppPokeStatusDraw::InitFunc()を延々と呼ぶ
   *
   *        使用開始
   *          1: 親Viewがあるのであれば親View側でAddSubView関数で設定する
   *             そうでないのであれば
   *             更新箇所でBoxUppPokeStatusDraw::UpdateTree()関数を
   *             描画箇所でBoxUppPokeStatusDraw::DrawTree()関数を呼び続ける
   *
   *          2: BoxUppPokeStatusDraw::Open関数を呼んで表示させる
   *
   *        表示パラメータ設定
   *          1: BoxUppPokeStatusDraw::SetParam関数を呼ぶ
   *
   *        使用停止
   *          1: 使用停止して表示しないように m_pTrainerIconRendering::Close関数を呼ぶ
   *          2: 親Viewにくっつけている場合はm_pTrainerIconRendering::RemoveFromSuperView()関数を呼んで親からはずす
   *
   *        使用破棄
   *          1: 親Viewにくっつけている場合はBoxUppChatDraw::RemoveFromSuperView()関数を呼んで親からはずす
   *
   *          2: BoxUppPokeStatusDraw::EndFunc()を呼ぶ
   *             BoxUppPokeStatusDraw::EndFunc()を呼んだらBoxUppPokeStatusDraw::UpdateTree()/BoxUppPokeStatusDraw::DrwaTree()の関数を呼んではいけない
   *             BoxUppPokeStatusDraw::EndFunc()の戻り値がtrueになるまでBoxUppPokeStatusDraw::EndFunc()を延々と呼ぶ
   *
   *          3: インスタンスを破棄する
   *
   *     注意点
   *        ・内部で数字テキスト表示しているので数字フォントを事前にロードしないとハングするので注意
   *        ・1体のポケモン表示するためにローカルヒープを 5MB 内部で確保している
   *        ・レーダーチャートを表示しているためレーダーチャートアプリのdllファイルをロードしたprocで使わないといけない
   *        ・図鑑No表示に図鑑のセーブデータが必要
   */
  class BoxUppPokeStatusDraw : public app::ui::UIView
  {
    GFL_FORBID_COPY_AND_ASSIGN(BoxUppPokeStatusDraw);
  public:
    enum DrawMode
    {
      DRAW_MODE_STATUS  = 0,
      DRAW_MODE_JUDDGE
    };

    //  コンストラクタに引き渡す初期化パラメータ(引数の数が増えたので構造体にまとめる)
    //  ポインタ先のメモリ開放するとハングするので注意
    struct INIT_PARAM
    {
      //  Boxレイアウトバイナリデータ
      void*                             pLytBinData;

      //  UIView描画で使うレンダリング
      //  上画面にモデルを描画するので引数 pRenderinManager には上画面のポケモンモデル描画設定したのを渡さないといけない
      app::util::AppRenderingManager*   pRenderinManager;

      //  レーダーチャートのロードで必要
      gfl2::fs::AsyncFileManager*       pAsyncFileManager;

      //  クラス内で使うヒープ
      app::util::Heap*                  pHeap;

      //  Box用のメッセージデータ
      //  メッセージファイルは「box.mstxt」でないといけない
      gfl2::str::MsgData*               pBoxMsgData;

      //  図鑑No表示に使う図鑑セーブデータ
      Savedata::ZukanData*              pZukanSaveData;

      //  グラフィックフォントのアイコン差し替えデータ
      app::tool::AppCommonGrpIconData*  pAppCommonGrapIconData;

      //  ポケモンのインフレーム表示する時に必要なレイアウトno
      u32 pokeInFrameLayerNo;

      INIT_PARAM()
      {
        pLytBinData             = NULL;
        pRenderinManager        = NULL;
        pHeap                   = NULL;
        pBoxMsgData             = NULL;
        pZukanSaveData          = NULL;
        pAsyncFileManager       = NULL;
        pAppCommonGrapIconData  = NULL;
        pokeInFrameLayerNo      = 2;
      }
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期設定
    * @param[in]  rInitParam       : 初期化パラメータ
    */
    //--------------------------------------------------------------------------------------------
    BoxUppPokeStatusDraw( const INIT_PARAM& rInitParam )
    : app::ui::UIView( rInitParam.pHeap ),
      m_normalDraw( this ),
      m_judgeDraw( this )
    {
      GFL_ASSERT( rInitParam.pLytBinData );
      GFL_ASSERT( rInitParam.pRenderinManager );
      GFL_ASSERT( rInitParam.pHeap );
      GFL_ASSERT( rInitParam.pBoxMsgData );
      GFL_ASSERT( rInitParam.pZukanSaveData );
      GFL_ASSERT( rInitParam.pAppCommonGrapIconData );

      _Clear();

      m_initParam = rInitParam;
    }

    virtual ~BoxUppPokeStatusDraw();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期化処理
    * @param[in]  pLytBinData       : レイアウトデータ
    * @param[in]  pRenderingManager : 描画管理
    * @param[in]  pUtilHeap         : ヒープ
    * @return     true 処理終了 / false 処理が終了していないので再度呼び出す
    * @note
    *         返り値が true になるまで呼び出す
    */
    //--------------------------------------------------------------------------------------------
    bool InitFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  終了処理
    * @return true 処理終了 / false 処理が終了していないので再度呼び出す
    * @note
    *         返り値が true になるまで
    *         この関数を呼び出すとInitFunc()で初期化しないと使えないので注意
    *         使うケースとしては画面をもう使わないときの後処理に使う
    */
    //--------------------------------------------------------------------------------------------
    bool EndFunc();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  画面を開く
    */
    //--------------------------------------------------------------------------------------------
    void Open();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  画面を閉じる
    * @note
    *         画面を非表示にする時にはこれを使う
    *         SetVisible関数で非表示にしても3Dモデル表示は非表示にできないから
    */
    //--------------------------------------------------------------------------------------------
    void Close();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      描画モード変更
    * @param[in]  mode: 変更した描画モードを設定
    */
    //--------------------------------------------------------------------------------------------
    void SetDrawMode( const DrawMode mode );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      ステータスパラメータを指定
    * @param[in]  pPokeParam: 参照するポケモンパラメータを指定
    * @parma[in]  bLock:        ロック中
    * @param[in]  bRegistTeam:  チーム登録中
    * @note
    *   この関数が呼ばれた段階で表示するポケモンモデルのロードをする
    */
    //--------------------------------------------------------------------------------------------
    void SetParam( const pml::pokepara::PokemonParam& rPokeParam, const bool bLock, const bool bRegistTeam );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  初期化パラメータを取得
    * @return 初期化パラメータ参照を返す
    */
    //--------------------------------------------------------------------------------------------
    const INIT_PARAM& GetInitParam() const { return m_initParam; }

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  更新処理
    *         子ウインドウについても自動的にコールする(子→親の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Update();

  protected:
    //--------------------------------------------------------------------------------------------
    /**
    * @brief  描画処理
    *         子ウインドウについても自動的にコールする(親→子の順）
    */
    //--------------------------------------------------------------------------------------------
    virtual void Draw( gfl2::gfx::CtrDisplayNo displayNo );

  private:
    enum
    {
      LYT_WORK_NORMAL,
      LYT_WORK_MAX,
    };

    //  通常ステータス表示
    class NormalDraw
    {
    public:
      NormalDraw( BoxUppPokeStatusDraw* pRootView )
      {
        _Clear();

        GFL_ASSERT( pRootView );
        m_pRootView = pRootView;
      }

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      初期化
      * @note
      *   必ずViewの2D作成後に実行する
      */
      //--------------------------------------------------------------------------------------------
      void Initialize();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      ステータスパラメータを指定
      * @param[in]  pPokeParam:           参照するポケモンパラメータを指定
      */
      //--------------------------------------------------------------------------------------------
      void SetParam( const pml::pokepara::PokemonParam& rPokeParam );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      表示指定
      * @param[in]  bFlg:           表示フラグ
      */
      //--------------------------------------------------------------------------------------------
      void SetVisible( const bool bFlg );

    private:
      struct SKILL_ITEM_DATA
      {
        //  わざ項目ペイン
        gfl2::lyt::LytParts*   pItemPartsPane;
        //  わざ名テキストペイン
        gfl2::lyt::LytTextBox* pTextPane;
        //  ポケモンわざタイプ色アニメid
        u32                     aWazaColorAnimeId[ POKETYPE_MAX ];

        SKILL_ITEM_DATA()
        {
          pItemPartsPane  = NULL;
          pTextPane = NULL;

          {
            u32 loopNum = GFL_NELEMS( aWazaColorAnimeId );
            for( u32 i = 0; i < loopNum; ++i )
            {
              aWazaColorAnimeId[ i ]  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            }
          }
        }
      };

      void _Clear()
      {
        m_pRootView                 = NULL;
        m_pZukanNoNumTextPane       = NULL;
        m_pPokeKindNameTextPane     = NULL;
        m_pHavePlayerNameTextPane   = NULL;
        m_pPokeHPNowNumTextPane     = NULL;
        m_pPokeHPMaxNumTextPane     = NULL;
        m_pPokeAtkNumTextPane       = NULL;
        m_pPokeDefNumTextPane       = NULL;
        m_pPokeTokukouNumTextPane   = NULL;
        m_pPokeTokubouNumTextPane   = NULL;
        m_pPokeSpeedNumTextPane     = NULL;
        m_pPokeSeikakuNameTextPane  = NULL;
        m_pPokeTokuseiNameTextPane  = NULL;
        m_pPokeItemNameTextPane     = NULL;
        m_pPokeTypeIcon01Pane       = NULL;
        m_pPokeTypeIcon02Pane       = NULL;


        m_bZukanNo  = true;
      }

      bool  m_bZukanNo;

      BoxUppPokeStatusDraw* m_pRootView;

      //  図鑑Noテキストペイン
      gfl2::lyt::LytTextBox*  m_pZukanNoNumTextPane;

      //  ポケモン種類名テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeKindNameTextPane;
      
      //  所持プレイヤー名テキストペイン
      gfl2::lyt::LytTextBox*  m_pHavePlayerNameTextPane;

      //  ポケモンHPの現在と最大値テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeHPNowNumTextPane;
      gfl2::lyt::LytTextBox*  m_pPokeHPMaxNumTextPane;

      //  ポケモン攻撃値テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeAtkNumTextPane;

      //  ポケモン防御値テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeDefNumTextPane;

      //  ポケモンとくこう値テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeTokukouNumTextPane;

      //  ポケモンとくぼう値テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeTokubouNumTextPane;

      //  ポケモンすばやさ値テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeSpeedNumTextPane;

      //  ポケモン性格名テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeSeikakuNameTextPane;

      //  ポケモン特性名テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeTokuseiNameTextPane;

      //  ポケモンもちもの名テキストペイン
      gfl2::lyt::LytTextBox*  m_pPokeItemNameTextPane;

      //  ポケモンタイプのアイコン画像ペイン
      gfl2::lyt::LytPicture*  m_pPokeTypeIcon01Pane;
      gfl2::lyt::LytPicture*  m_pPokeTypeIcon02Pane;

      //  わざ項目のペイン一覧
      SKILL_ITEM_DATA m_aSkillItemData[ pml::MAX_WAZA_NUM ];
    };

    //  ジャッジステータス表示
    class JudgeDraw
    {
    public:
      JudgeDraw( BoxUppPokeStatusDraw* pRootView )
      {
        _Clear();

        GFL_ASSERT( pRootView );
        m_pRootView = pRootView;
      }

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      デストラクタ
      * @note
      *             レーダーチャートのメモリ開放をここでしている
      */
      //--------------------------------------------------------------------------------------------
      ~JudgeDraw();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      初期化
      * @param[in]  pHeap:              メモリ確保するアプリ用ヒープ
      * @param[in]  pAsyncFileManager:  レーダーチャート作成にファイル管理が必要
      * @note
      *   必ずViewの2D作成後に実行する
      */
      //--------------------------------------------------------------------------------------------
      void Initialize( app::util::Heap* pUtilHeap, gfl2::fs::AsyncFileManager* pAsyncFileManager );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      終了処理
      */
      //--------------------------------------------------------------------------------------------
      void Terminate();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      破棄可能か
      * @return     可能 true / 不可能 false
      */
      //--------------------------------------------------------------------------------------------
      bool IsDelete();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      使用準備できているか
      * @return     できている true / できていない false
      */
      //--------------------------------------------------------------------------------------------
      bool IsReady();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      更新処理
      */
      //--------------------------------------------------------------------------------------------
      void Update();

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      ステータスパラメータを指定
      * @param[in]  pPokeParam:           参照するポケモンパラメータを指定
      */
      //--------------------------------------------------------------------------------------------
      void SetParam( const pml::pokepara::PokemonParam& rPokeParam );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      表示指定
      * @param[in]  bFlg:           表示フラグ
      */
      //--------------------------------------------------------------------------------------------
      void SetVisible( const bool bFlg );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      レーダーチャート表示指定
      * @param[in]  bFlg:           表示フラグ
      */
      //--------------------------------------------------------------------------------------------
      // @fix GFNMCat[1117]　レーダーチャートは個別で表示設定できるようにしないといけない 
      void SetVisibleReaderChart( const bool bFlg );

    private:
      //  各項目の色変えアニメフレーム値
      enum
      {
        ANIME_FRAME_PARAM_ITEM_COLOR_NONE = 0,
        ANIME_FRAME_PARAM_ITEM_COLOR_RAD,
        ANIME_FRAME_PARAM_ITEM_COLOR_BLUE
      };

      //  パラメータ種類
      struct JUDGE_SETUP_PARAM_ITEM_DATA
      {
        //  種族種類
        pml::personal::ParamID  personalId;

        //  固体種類
        pml::pokepara::PowerID  talentId;

        //  項目名のグラフィックフォントの画像ペイン
        gfl2::lyt::LytPicture*  pItenNameIconPane;

        //  項目カラーアニメid
        u32                     colorAnimeId;

        //  星マークのエフェクトアニメ(なくなりました)
//        u32                     aStarMarkAnimeIdList[ 3 ];

        //  項目最大値の星マークのエフェクトペイン
        gfl2::lyt::LytPane*     pStarMarkEffectPane;

        //  評価内容テキストペイン
        gfl2::lyt::LytTextBox*  pEvaluationTextPane;

        //  レーダーチャートの頂点位置( 一番上から時計周りに 0 - 5 )
        u32 readerChartPointIdx;
      };

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      パラメータ値から評価テキストidを変換
      * @param[in]  paramNum: テキストidに変換したいパラメータ値
      * @return     paramNumの値から評価テキストidを返す
      */
      //--------------------------------------------------------------------------------------------
      u32 _ConverParamNumToEvaluationTextId( const u32 paramNum );

      //--------------------------------------------------------------------------------------------
      /**
      * @brief      トータルパラメータ値から評価テキストidを変換
      * @param[in]  paramTotalNum: テキストidに変換したいパラメータ値
      * @return     paramTotalNumの値から評価テキストidを返す
      */
      //--------------------------------------------------------------------------------------------
      u32 _ConverParamTotalNumToEvaluationTextId( const u32 paramTotalNum );

      void _Clear()
      {
        m_pRootView                     = NULL;
        m_pRaderChart                   = NULL;
        m_pStatusCommentMessageTextPane = NULL;
        m_raderChartRadiusVal           = 0.0f;
        m_bReady                        = false;
        m_bVisible                      = false;

        m_raderChartPos.Set( 0.0f, 0.0f );

        {
          u32 loopNum = GFL_NELEMS( m_aSetupParamItemDataList );
          for( u32 i = 0; i < loopNum; ++i )
          {
            m_aSetupParamItemDataList[ i ].personalId = pml::personal::PARAM_ID_egg_group1;
            m_aSetupParamItemDataList[ i ].talentId   = pml::pokepara::POWER_NUM;
            m_aSetupParamItemDataList[ i ].pItenNameIconPane = NULL;
            m_aSetupParamItemDataList[ i ].colorAnimeId = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
            m_aSetupParamItemDataList[ i ].pStarMarkEffectPane  = NULL;
            m_aSetupParamItemDataList[ i ].pEvaluationTextPane  = NULL;
            m_aSetupParamItemDataList[ i ].readerChartPointIdx  = 0;
          }
        }
      }

      app::tool::BoxRaderChart*     m_pRaderChart;
      BoxUppPokeStatusDraw*         m_pRootView;

      gfl2::math::Vector2           m_raderChartPos;
      f32                           m_raderChartRadiusVal;

      //  パラメータからのコメントメッセージテキストペイン表示
      gfl2::lyt::LytTextBox*        m_pStatusCommentMessageTextPane;

      //  パラメータ項目の設定データリスト
      JUDGE_SETUP_PARAM_ITEM_DATA   m_aSetupParamItemDataList[ pml::pokepara::POWER_NUM ];

      bool                          m_bReady;

      gfl2::math::Random            m_rand;

      bool  m_bVisible;
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  2D初期化処理
    * @note
    *     レイアウトデータから描画に必要な生成をしている
    */
    //--------------------------------------------------------------------------------------------
    void _Init2D();

    void _Clear()
    {
      m_pPokeModelSys                 = NULL;
      m_pPokeSimpleModelInFrame              = NULL;
      m_pWordSet                      = NULL;
      m_pSickPartsPane                = NULL;
      m_pSickIconPicturePane          = NULL;
      m_pRareIconPane                 = NULL;
      m_pPokerusAntibodyPane          = NULL;
      m_pLangIconPartsPane            = NULL;
      m_pLangIconPicturePane          = NULL;
      m_pNickNameTextPane             = NULL;
      m_pMonsBallPicturePane          = NULL;
      m_pSexPane                      = NULL;
      m_pLvTextPane                   = NULL;
      m_pItemIconTool                 = NULL;
      m_pLockIconPane                 = NULL;
      m_pRegistTeamIconPane           = NULL;
      m_pPokeInFrameDrawEnvNode       = NULL;
      m_changeBallId                  = item::BALLID_NULL;
      m_nowBallId                     = item::BALLID_NULL;
      m_nowDrawMode                   = DRAW_MODE_STATUS;

      m_initSeq       = 0;
      m_endSeq        = 0;
      m_changeBallSeq = 0;
      m_bInit         = false;
      m_bEggDraw      = false;
    }

    INIT_PARAM                                                m_initParam;

    //  ポケモンモデルシステム
    PokeTool::PokeModelSystem*                                m_pPokeModelSys;

    //  テキストのワード設定
    print::WordSet*                                           m_pWordSet;

    //  表示するポケモン
    app::tool::PokeSimpleModelInFrame*                        m_pPokeSimpleModelInFrame;

    //  ポケモンインフレームの描画設定
    gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pPokeInFrameDrawEnvNode;

    //  ステータス表示
    NormalDraw                                                m_normalDraw;

    //  ジャッジ表示
    JudgeDraw                                                 m_judgeDraw;

    //  共通パラメータのペイン一覧

    //  状態異常のアイコンルートペイン
    gfl2::lyt::LytParts*                                      m_pSickPartsPane;
    gfl2::lyt::LytPicture*                                    m_pSickIconPicturePane;

    //  色違いレアアイコンペイン
    gfl2::lyt::LytPane*                                       m_pRareIconPane;

    //  ポケルス抗体アイコンペイン
    gfl2::lyt::LytPane*                                       m_pPokerusAntibodyPane;

    //  言語アイコンペイン
    gfl2::lyt::LytParts*                                      m_pLangIconPartsPane;
    gfl2::lyt::LytPicture*                                    m_pLangIconPicturePane;

    //  ニックネームテキストペイン
    gfl2::lyt::LytTextBox*                                    m_pNickNameTextPane;

    //  モンスターボールアイコン画像ペイン
    gfl2::lyt::LytPicture*                                    m_pMonsBallPicturePane;

    //  性別アイコンペイン
    gfl2::lyt::LytPane*                                       m_pSexPane;

    //  レベル値のテキストペイン
    gfl2::lyt::LytTextBox*                                    m_pLvTextPane;

    //  ロックアイコン
    gfl2::lyt::LytPane*                                       m_pLockIconPane;

    //  チーム登録アイコン
    gfl2::lyt::LytPane*                                       m_pRegistTeamIconPane;

    //  モンスターボールアイコン画像管理
    app::tool::ItemIconToolEx*                                m_pItemIconTool;

    //  ボールアイコン張替えする時のid
    item::BALL_ID                                             m_changeBallId;
    item::BALL_ID                                             m_nowBallId;

    //  現在の表示モード
    DrawMode                                                  m_nowDrawMode;

    u32   m_initSeq;
    u32   m_endSeq;

    u32   m_changeBallSeq;
    bool  m_bInit;
    bool  m_bEggDraw;
  };

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif // __BOX_UPP_POKE_STATUS_DRAW_H_INCLUDED__