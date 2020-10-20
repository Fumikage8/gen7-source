//======================================================================
/**
 * @file AppToolBaseGrpFont.h
 * @date 2015/08/28 12:34:48
 * @author uchida_yuto
 * @brief グラフィックフォントを扱う基本クラス
 * @par Copyright
 * (C)1995-2015 GAME FREAK inc. All Rights Reserved.
 */
//======================================================================

#if !defined __APP_TOOL_BASE_GRP_FONT_H_INCLUDED__
#define __APP_TOOL_BASE_GRP_FONT_H_INCLUDED__
#pragma once

#pragma once

#include <types/include/gfl2_Typedef.h>
#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>

#include <Layout/include/gfl2_Layout.h>

#include <pml/include/poketype_def.h>
#include <pml/include/pokepara/pml_PokemonParamType.h>

#include <math/include/gfl2_Rect.h>

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(util)

  class AppUtilFileControlState;
  class Heap;

GFL_NAMESPACE_END(util)
GFL_NAMESPACE_END(app)

GFL_NAMESPACE_BEGIN(app)
GFL_NAMESPACE_BEGIN(tool)

  //  UVDataのインクルード
  #include "niji_conv_header/app/common/CommonGrpIconUVDataTable.cdat"
  #include "niji_conv_header/app/common/BtlVideoPlayerGrpFontUVDataTale.cdat"

  /**
   * @class AppToolBaseGrpFont
   * @brief グラフィックフォント基本クラス
   *     フォント新規追加方法
   *          1: UVDataの*.cdatファイルをインクルード
   *          2: enum FontType にフォント定義を追加
   *          3: AppToolBaseGrpFont::ms_aFontDataTable[] 配列にテクスチャレイアウトのarcidと*.cdatの配列、配列の最大数を設定
   *
   *     使い方
   *        使用初回準備(インスタンス生成からインスタンスを使えるまでの流れ)
   *          1: AppToolBaseGrpFontのインスタンスを生成
   *          2: AppToolBaseGrpFont::Initialize()関数に必要なパラメータを渡して呼ぶ
   *               この関数は一度しか呼んではいけません。
   *               連続で呼ぶと動作保障できません。
   *               
   *          3: AppToolBaseGrpFont::IsReady()関数を呼んで true が引けるまで呼び続ける
   *
   *        使用破棄(後始末からインスタンス破棄までの流れ)
   *          1: AppToolBaseGrpFont::Termiate()関数を一度呼ぶ
   *              この関数を呼んだ段階で破棄と見なします。
   *              そしてこれは一度しか呼んではいけません。
   *              連続で呼ぶと動作保障できません。
   *          2: インスタンスを破棄する
   */
  class AppToolBaseGrpFont
  {
    GFL_FORBID_COPY_AND_ASSIGN( AppToolBaseGrpFont );
  public:
    //  フォントデータ
    struct FONT_DATA
    {
      gfl2::fs::ArcFile::ARCID        arcId;
      const gfl2::math::RectU16*      pUIRectData;
      u32                             uiRectDataMax;
    };

    //  フォントデータタイプ一覧
    enum FontType
    {
      FONT_TYPE_COMMON  = 0,
      FONT_TYPE_BTL_VIDEO_PLAYER,
      FONT_TYPE_MAX
    };

    //-----------------------------------------------------------------------------
    /**
     * @brief コンストラクタ
     */
    //-----------------------------------------------------------------------------
    AppToolBaseGrpFont();

    //-----------------------------------------------------------------------------
    /**
     * @brief デストラクタ
     */
    //-----------------------------------------------------------------------------
    ~AppToolBaseGrpFont();

    //-----------------------------------------------------------------------------
    /**
     * @brief 初期化
     * @brief[in] pUtilHeap:  クラス内で使用するヒープ設定
     * @brief[in] pFontData:  フォントデータ(このクラスインスタンス生存中は、呼び出し元はfontDataを破棄せず保持しておいて下さい)
     * @brief[in] lang:       対象言語
     */
    //-----------------------------------------------------------------------------
    bool Initialize( app::util::Heap* pUtilHeap, const FONT_DATA* pFontData, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

    //-----------------------------------------------------------------------------
    /**
     * @brief 初期化
     * @brief[in] pUtilHeap:  クラス内で使用するヒープ設定
     * @brief[in] fontType:   フォントのタイプ
     * @brief[in] lang:       対象言語
     */
    //-----------------------------------------------------------------------------
    bool Initialize( app::util::Heap* pUtilHeap, const FontType fontType, gfl2::fs::ToolDefine::ArcLangType::Tag lang = gfl2::fs::ToolDefine::ArcLangType::DEFAULT_LANG );

    //-----------------------------------------------------------------------------
    /**
     * @brief 終了
     */
    //-----------------------------------------------------------------------------
    virtual void Terminate();

    //-----------------------------------------------------------------------------
    /**
     * @func    IsReady
     * @brief   使用可能か
     * @date    2015.06.16
     *
     * @retval  false = 使用不可能
     * @retval  true  = 使用可能
     */
    //-----------------------------------------------------------------------------
    bool IsReady();

    //-----------------------------------------------------------------------------
    /**
     * @brief   ペインのテクスチャを変更
     *
     * @param   uvID      UVデータのID
     * @param   pane      ペイン
     * @param   material  マテリアルのインデックス
     * @param   texture   テクスチャのインデックス
     * @note
     *          テクスチャを貼り付けるlytDataのidは０が前提
     */
    //-----------------------------------------------------------------------------
    bool ReplacePaneTexture( u32 uvID, gfl2::lyt::LytPicture * pane, u32 materialDataIdx, u32 textureDataIdx );

  private:
    void _Clear()
    {
      m_pFileLoadState    = NULL;
      m_pLytSys           = NULL;
      m_pCurrentFontData  = NULL;
      m_pUtilHeap         = NULL;
    }

    app::util::AppUtilFileControlState*       m_pFileLoadState;
    app::util::Heap*                          m_pUtilHeap;

    gfl2::lyt::LytSys*                        m_pLytSys;
    gfl2::lyt::LytMultiResID                  m_loadTexMuiltResID;

    const FONT_DATA*                          m_pCurrentFontData;

    static const FONT_DATA                    ms_aFontDataTable[ FONT_TYPE_MAX ];
  };

GFL_NAMESPACE_END(tool)
GFL_NAMESPACE_END(app)

#endif // __APP_TOOL_BASE_GRP_FONT_H_INCLUDED__