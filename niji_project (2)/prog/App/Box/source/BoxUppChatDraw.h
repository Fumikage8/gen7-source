//======================================================================
/**
 * @file BoxUppChatDraw.h
 * @date 2015/07/22 19:21:37
 * @author uchida_yuto
 * @brief Boxの上画面のチャット表示
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __BOX_UPP_CHAT_DRAW_H_INCLUDED__
#define __BOX_UPP_CHAT_DRAW_H_INCLUDED__
#pragma once

//  gflib2のインクルード
#include <macro/include/gfl2_Macros.h>
#include <types/include/gfl2_Typedef.h>
#include <heap/include/gfl2_Heap.h>
#include <RenderingEngine/include/SceneGraph/Instance/gfl2_DrawEnvNode.h>

//  nijiのインクルード
#include "AppLib/include/ui/UIView.h"
#include "AppLib/include/Tool/AppToolTrainerIconRendering.h"

//  前方宣言
GFL_NAMESPACE_BEGIN(PokeTool)

  class PokeModelSystem;

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

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN( App )
GFL_NAMESPACE_BEGIN( Box )

  /**
   * @class BoxUppChatDraw
   * @brief P2Pでのポケモン交換で表示するチャットView
   * @note
   *     使い方
   *        使用初回準備
   *          1: app::util::AppRenderingManagerのインスタンスを生成(上画面にポケモンモデルを描画できるようにする)
   *          2: app::util::AppRenderingManagerのインスタンスに上画面のカメラ設定をする
   *          3: 数字フォントを生成
   *          4: Box用のMsgDataのインスタンスを生成
   *          5: Boxのレイアウトバイナリデータをロードする
   *          6: BoxUppChatDrawのインスタンスを生成してコンストラクタに必要なパラメータを指定
   *          7: BoxUppChatDraw::InitFunc()関数を呼ぶ。
   *             戻り値がtrueになるまでBoxUppChatDraw::InitFunc()を延々と呼ぶ
   *
   *        使用開始
   *          1: 親Viewがあるのであれば親View側でAddSubView関数で設定する
   *             そうでないのであれば
   *             更新箇所でBoxUppChatDraw::UpdateTree()関数を
   *             描画箇所でBoxUppChatDraw::DrawTree()関数を呼び続ける
   *
   *          2: BoxUppChatDraw::Open関数を呼んで表示させる
   *
   *          3: ポケモン設定にはBoxUppChatDraw::SetPokeParam()関数を
   *             チャットメッセージテキスト更新にはBoxUppChatDraw::SetChatWindowMessageText()関数を呼ぶ
   *
   *        使用停止
   *          1: 使用停止して表示しないように BoxUppChatDrwa::Close関数を呼ぶ
   *          2: 親Viewにくっつけている場合はBoxUppChatDraw::RemoveFromSuperView()関数を呼んで親からはずす
   *
   *        使用破棄
   *          1: 親Viewにくっつけている場合はBoxUppChatDraw::RemoveFromSuperView()関数を呼んで親からはずす
   *
   *          2: BoxUppChatDraw::EndFunc()を呼ぶ
   *             BoxUppChatDraw::EndFunc()を呼んだらBoxUppChatDraw::UpdateTree()/BoxUppChatDraw::DrwaTree()の関数を呼んではいけない
   *             BoxUppChatDraw::EndFunc()の戻り値がtrueになるまでBoxUppChatDraw::EndFunc()を延々と呼ぶ
   *
   *          3: インスタンスを破棄する
   *
   *     注意点
   *        1: 内部で数字テキスト表示しているので数字フォントを事前にロードしないとハングするので注意
   *        2: 2体のポケモン表示するためにローカルヒープを 10MB 内部で確保している
   */
  class BoxUppChatDraw : public app::ui::UIView
  {
      GFL_FORBID_COPY_AND_ASSIGN(BoxUppChatDraw);
  public:
    enum PlayerType
    {
      PLYAER_TYPE_SELF  = 0,
      PLYAER_TYPE_OTHER,
      PLYAER_TYPE_MAX
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      初期設定
    * @param[in]  pLytBinData       : レイアウトデータ
    * @param[in]  pRenderingManager : 描画管理
    * @param[in]  pUtilHeap         : ヒープ
    * @param[in]  pBoxMsgData       : Box用のメッセージデータ
    * @note
    *     1:  各引数のポインタはクラスの変数に設定して使われるのでポインタ先のメモリ開放するとハングするので注意
    *     2:  pBoxMsgDataの引数に渡すメッセージファイルは「box.mstxt」でないといけない
    *     3:  上画面にモデルを描画するので引数 pRenderinManager には上画面のポケモンモデル描画設定したのを渡さないといけない
    */
    //--------------------------------------------------------------------------------------------
    BoxUppChatDraw( void* pLytBinData, app::util::AppRenderingManager* pRenderinManager, app::util::Heap* pHeap, gfl2::str::MsgData* pBoxMsgData ) : app::ui::UIView( pHeap )
    {
      GFL_ASSERT( pLytBinData );
      GFL_ASSERT( pRenderinManager );
      GFL_ASSERT( pHeap );
      GFL_ASSERT( pBoxMsgData );

      _Clear();

      m_pLytBinData       = pLytBinData;
      m_pRenderinManager  = pRenderinManager;
      m_pUtilHeap         = pHeap;
      m_pBoxMsgData       = pBoxMsgData;
    }

    virtual ~BoxUppChatDraw();

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
    */
    //--------------------------------------------------------------------------------------------
    void Close();

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      指定プレイヤーが参照するポケモンパラメータを指定
    * @param[in]  type:       プレイヤーを指定(自身か相手か)
    * @param[in]  pPokeParam: 参照するポケモンパラメータを指定
    * @note
    *   この関数が呼ばれた段階で表示するポケモンモデルのロードをする
    */
    //--------------------------------------------------------------------------------------------
    void SetPokeParam( const PlayerType type, const pml::pokepara::PokemonParam* pPokeParam );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      表示するトレーナーパラメータを指定
    * @param[in]  type:         プレイヤーを指定(自身か相手か)
    * @param[in]  rIconData     プレイヤーのアイコンパラメータ
    * @note
    *   この関数が呼ばれた段階で表示するデータのロードをする
    */
    //--------------------------------------------------------------------------------------------
    void SetTrainerParam( const PlayerType type, const app::tool::AppToolTrainerIconRendering::ICON_OBJECT_DATA& rIconData );

    //--------------------------------------------------------------------------------------------
    /**
    * @brief      チャットウィンドウに表示するメッセージを指定
    * @param[in]  type:     プレイヤーを指定(自身か相手か)
    * @param[in]  pStrBuf:  チャットウィンドウに表示するテキスト
    */
    //--------------------------------------------------------------------------------------------
    void SetChatWindowMessageText( const PlayerType type, gfl2::str::StrBuf* pStrBuf );
    //--------------------------------------------------------------------------------------------
    /**
    * @brief      チャットウィンドウに表示するポケモンの名前を指定
    * @param[in]  wordsetno:  ワードセット番号
    * @param[in]  monsno:  モンスター番号
    */
    //--------------------------------------------------------------------------------------------
    void ExpandPokemon(int wordsetno, int monsno );
    //--------------------------------------------------------------------------------------------
    /**
     * @brief      チャットウィンドウに表示するポケモンの名前を指定
     * @param[in]  wordsetno:  ワードセット番号
     * @param[in]  pp:  モンスター構造体
     */
    //--------------------------------------------------------------------------------------------
    void ExpandPokemon(int wordsetno,  const pml::pokepara::CoreParam * pp  );
    //--------------------------------------------------------------------------------------------
    /**
     * @brief      チャットウィンドウに表示するメッセージを指定
     * @param[in]  type:     プレイヤーを指定(自身か相手か)
     * @param[in]  strID:  チャットウィンドウに表示するテキスト
     */
    //--------------------------------------------------------------------------------------------
    void SetChatWindowMessageID( const PlayerType type, u32 strID );
    //--------------------------------------------------------------------------------------------
    /**
     * @brief      ポケモンステータスのONOFF
     * @param[in]  type:   プレイヤーを指定(自身か相手か)
     * @param[in]  bvisible  onoff
     */
    //--------------------------------------------------------------------------------------------
    void SetPokemonStateOnOff( const PlayerType type,bool bvisible );

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
      LYT_WORK_NORMAL = 0,
      LYT_WORK_MAX,
    };

    //  プレイヤーのオブジェクト
    class PlayerObj
    {
    public:
      //--------------------------------------------------------------------------------------------
      /**
      * @brief  コンストラクタ
      * @param[in]  pUtilHeap:          基本となるHeap
      * @param[in]  pPokeModelSys:      モデル描画に必要な管理
      * @param[in]  pRenderingManager:  描画レンダリング
      * @param[in]  layerNo:            ポケモンモデルの描画レイヤー(0 or 1)
      * @note
      *   内部でポケモンモデルのロードに必要なローカルメモリを確保している。
      *   最低でも5MByteのサイズがヒープに必要なので注意
      */
      //--------------------------------------------------------------------------------------------
      PlayerObj( app::util::Heap* pUtilHeap, PokeTool::PokeModelSystem* pPokeModelSys, app::util::AppRenderingManager* pRenderingManager, u32 layerNo );
      ~PlayerObj();

      //  表示するポケモン
      app::tool::PokeSimpleModelInFrame* m_pPokeSimpleModelInFrame;

      //  ポケモンインフレームの描画設定
      gfl2::renderingengine::scenegraph::instance::DrawEnvNode* m_pPokeInFrameDrawEnvNode;

      //  表示するポケモン名のテキストペイン
      gfl2::lyt::LytTextBox*      m_pPokeNameTextPane;

      //  表示するポケモンのレベルタイトルテキストペイン
      gfl2::lyt::LytTextBox*      m_pPokeLvTitleTextPane;

      //  表示するポケモンのレベル値テキストペイン
      gfl2::lyt::LytTextBox*      m_pPokeLvNumTextPane;

      //  表示するチャットのテキストペイン
      gfl2::lyt::LytTextBox*      m_pChatMesssageTextPane;

      //  表示するポケモンが所持しているアイテムアイコンペイン
      gfl2::lyt::LytPane*         m_pHaveItemIconPane;

      //  表示するポケモンの所持しているアイテムのテキストペイン
      gfl2::lyt::LytTextBox*      m_pHaveItemTextPane;

      //  表示するトレーナーアイコン画像ペイン
      gfl2::lyt::LytPicture*      m_pTrainerIconPane;

      //  表示するトレーナーアイコンのフレームペイン
      gfl2::lyt::LytPane*         m_pTrainerIconFramePane;
      
      //  性別アイコンペイン(性別が男女でもない場合は非表示にする)
      gfl2::lyt::LytPane*         m_pSexIconPane;

      //  性別アイコン男女アニメid
      u32 m_sexMaleIconAnimeId;
      u32 m_sexFemaleIconAnimeId;

      bool  m_bVisible;

    private:
      void _Clear()
      {
        m_pPokeSimpleModelInFrame = NULL;
        m_pPokeInFrameDrawEnvNode = NULL;
        m_pPokeLvNumTextPane      = NULL;
        m_pPokeNameTextPane       = NULL;
        m_pPokeLvTitleTextPane    = NULL;
        m_pChatMesssageTextPane   = NULL;
        m_pHaveItemIconPane       = NULL;
        m_pHaveItemTextPane       = NULL;
        m_pTrainerIconPane        = NULL;
        m_pTrainerIconFramePane   = NULL;
        m_pSexIconPane            = NULL;
        m_bVisible                = true;


        m_sexMaleIconAnimeId    = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
        m_sexFemaleIconAnimeId  = gfl2::lyt::LYTSYS_ANIMEINDEX_ERROR;
      }
    };

    //--------------------------------------------------------------------------------------------
    /**
    * @brief  2D初期化処理
    * @note
    *     レイアウトデータから描画に必要な生成をしている
    */
    //--------------------------------------------------------------------------------------------
    void _Init2D();

    //  コメントアニメ再生
    void _PlayAnimeInComment( const PlayerType type );

    void _Clear()
    {
      m_pLytBinData                   = NULL;
      m_pRenderinManager              = NULL;
      m_pUtilHeap                     = NULL;
      m_pPokeModelSys                 = NULL;
      m_pBoxMsgData                   = NULL;
      m_pWordSet                      = NULL;
      m_pTrainerIconRendering         = NULL;

      gfl2::std::MemClear( m_pPlayerObjList, sizeof( m_pPlayerObjList ) );

      m_initSeq   = 0;
      m_endSeq    = 0;
      m_bInit     = false;
    }

    void* m_pLytBinData;

    app::util::AppRenderingManager*                           m_pRenderinManager;
    app::util::Heap*                                          m_pUtilHeap;

    PokeTool::PokeModelSystem*                                m_pPokeModelSys;

    gfl2::str::MsgData*                                       m_pBoxMsgData;
    print::WordSet*                                           m_pWordSet;

    app::tool::AppToolTrainerIconRendering*                   m_pTrainerIconRendering;

    //  制御するプレイヤーたち
    PlayerObj*                                                m_pPlayerObjList[ PLYAER_TYPE_MAX ];

    u32   m_initSeq;
    u32   m_endSeq;

    bool  m_bInit;
  };

GFL_NAMESPACE_END( Box )
GFL_NAMESPACE_END( App )

#endif // __BOX_UPP_CHAT_DRAW_H_INCLUDED__
