//=============================================================================
/**
 * @file AppToolGaugePartsLayout.h
 * @brief パーツレイアウトゲージの制御
 * @author yuto_uchida
 * @date 2015.03.27
 */
//=============================================================================
#if !defined( __APP_TOOL_HP_GAUGE_PARTS_LAYOUT_H__ )
#define __APP_TOOL_HP_GAUGE_PARTS_LAYOUT_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <heap/include/gfl2_Heap.h>
#include <layout/include/gfl2_LytSys.h>

GFL_NAMESPACE_BEGIN( app )

//  前方宣言
GFL_NAMESPACE_BEGIN( util )
  class G2DUtil;
GFL_NAMESPACE_END( util )

GFL_NAMESPACE_BEGIN( tool )

  /**
    * @brief HPゲージパーツレイアウトの制御クラス
    * @note
    *    レイアウトファイル「Common_GAG_000.flyt」のパーツの制御をしている
   */
  class AppToolHPGaugePartsLayout
  {
    GFL_FORBID_COPY_AND_ASSIGN( AppToolHPGaugePartsLayout );  // コピーを禁止

    public:
      //  クラス初期化時に必要なパラメータ
      typedef struct
      {
        //  パーツの細かい設定を行うのに必要
        util::G2DUtil*          p_g2d_util;

        //  ゲージパーツが配置されているLayoutId
        s32                     layout_id;

        //  数字を表示するメッセージID
        s32 number_message_id;

        //  数字を表示する桁数
        s32 number_keta;

        //  ゲージ左側の数字テキストボックスペインのポインタ（ペイン名「TextBox_00」）
        gfl2::lyt::LytTextBox*  p_left_number_text_box_pane;

        //  ゲージ真ん中の「/」テキストボックスペインのポインタ（ペイン名「TextBox_01」）
        gfl2::lyt::LytTextBox*  p_center_number_text_box_pane;

        //  ゲージ右側の数字テキストボックスペインのポインタ（ペイン名「TextBox_02」）
        gfl2::lyt::LytTextBox*  p_right_number_text_box_pane;

        //  ゲージアニメidx(アニメ名「gauge_hp」)
        s32 gauge_hp_anime_idx;

        //  ゲージ通常色切り替えアニメidx(アニメ名「color_normal」)
        s32 color_normal_anime_idx;

        //  ゲージ半分以下の色切り替えアニメidx(アニメ名「color_half」)
        s32 color_half_anime_idx;

        //  ゲージ現象の色切り替えアニメidx(アニメ名「color_few」)
        s32 color_few_anime_idx;

      } INIT_PARAM;

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  コンストラクタ
        */
      //-----------------------------------------------------------------------------------------
      AppToolHPGaugePartsLayout( const INIT_PARAM& param, gfl2::heap::HeapBase* pHeap );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  デストラクタ
        */
      //-----------------------------------------------------------------------------------------
      virtual ~AppToolHPGaugePartsLayout();

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージアニメを開始する
        * @param  nextNumber      : 数の最終値（この数になるまでゲージアニメが進行する）
        * @param  nowNumber       : 数基準となる現在値を設定
        * @param  maxNumber       : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
        * @param  playAnimeFrame  : アニメを再生してから終了するまでのフレーム数を指定
        */
      //-----------------------------------------------------------------------------------------
      void StartAnime( const s32 nextNumber, const s32 nowNumber, const s32 maxNumber, const s32 playAnimeFrame = 30 );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージアニメスキップ
        */
      //-----------------------------------------------------------------------------------------
      void SkipAnime();

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージアニメ実行中か
        */
      //-----------------------------------------------------------------------------------------
      bool IsAnime() const;

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージの値を設定（アニメせずにいきなり反映）
        * @param  nowNumber   : 数基準となる現在値を設定
        * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
        * @note
        *   ゲージアニメ中であればアニメは終了する
        */
      //-----------------------------------------------------------------------------------------
      void SetNumber( const s32 nowNumber, const s32 maxNumber );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  更新処理
        */
      //-----------------------------------------------------------------------------------------
      void Update( void );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  テキスト表示設定
        */
      //-----------------------------------------------------------------------------------------
      void SetTextVisible( const bool bFlg );

    private:
      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージの値を設定（アニメせずにいきなり反映）
        * @param  nowNumber   : 数基準となる現在値を設定
        * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
        */
      //-----------------------------------------------------------------------------------------
      void _SetNumber( const f32 nowNumber, const s32 maxNumber );

      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      s32         m_gaugeFrameMax;

      class AppToolGaugePartsLayoutProc*  m_pCoreProc;
  };

  /**
    * @brief ゲージパーツレイアウトの制御クラス（経験値ゲージ）
    * @note
    *    レイアウトファイル「Common_GAG_001.flyt」のパーツの制御をしている
    *    経験値が最大の場合は０にする（レベル１００の場合は経験値が０なので）
   */
  class AppToolGaugePartsLayout
  {
    GFL_FORBID_COPY_AND_ASSIGN( AppToolGaugePartsLayout );  // コピーを禁止

    public:
      //  クラス初期化時に必要なパラメータ
      typedef struct
      {
        //  パーツの細かい設定を行うのに必要
        util::G2DUtil*          p_g2d_util;

        //  ゲージパーツが配置されているLayoutId
        s32                     layout_id;

        //  ゲージパーツペイン
        gfl2::lyt::LytParts*    p_parts_pane;

        //  ゲージアニメidx(アニメ名「gauge_00」)
        s32 gauge_hp_anime_idx;

      } INIT_PARAM;

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  コンストラクタ
        */
      //-----------------------------------------------------------------------------------------
      AppToolGaugePartsLayout( const INIT_PARAM& param, gfl2::heap::HeapBase* pHeap );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  デストラクタ
        */
      //-----------------------------------------------------------------------------------------
      virtual ~AppToolGaugePartsLayout();

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージアニメを開始する
        * @param  nextNumber      : 数の最終値（この数になるまでゲージアニメが進行する）
        * @param  nowNumber       : 数基準となる現在値を設定
        * @param  maxNumber       : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
        * @param  playAnimeFrame  : アニメを再生してから終了するまでのフレーム数を指定
        */
      //-----------------------------------------------------------------------------------------
      void StartAnime( const s32 nextNumber, const s32 nowNumber, const s32 maxNumber, const s32 playAnimeFrame = 30 );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージアニメスキップ
        */
      //-----------------------------------------------------------------------------------------
      void SkipAnime();

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージアニメ実行中か
        */
      //-----------------------------------------------------------------------------------------
      bool IsAnime() const;

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージの値を設定（アニメせずにいきなり反映）
        * @param  nowNumber   : 数基準となる現在値を設定
        * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
        * @note
        *   ゲージアニメ中であればアニメは終了する
        */
      //-----------------------------------------------------------------------------------------
      void SetNumber( const s32 nowNumber, const s32 maxNumber );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  更新処理
        */
      //-----------------------------------------------------------------------------------------
      void Update( void );

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  表示設定
        */
      //-----------------------------------------------------------------------------------------
      void SetVisible( const bool bFlg );

    protected:
      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージの値を設定（アニメせずにいきなり反映）
        * @param  nowNumber   : 数基準となる現在値を設定
        * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
        */
      //-----------------------------------------------------------------------------------------
      virtual void _SetNumber( const f32 nowNumber, const s32 maxNumber );

    private:
      //  変数宣言一覧
      INIT_PARAM  m_initParam;
      s32         m_gaugeFrameMax;

      class AppToolGaugePartsLayoutProc*  m_pCoreProc;
  };

  /**
    * @brief ゲージパーツレイアウトの制御クラス
    * @note
    *    レイアウトファイル「Common_GAG_002.flyt」のパーツの制御をしている
   */
   class AppToolHPGaugePartsLayoutGAG_002 : public AppToolGaugePartsLayout
   {
      GFL_FORBID_COPY_AND_ASSIGN( AppToolHPGaugePartsLayoutGAG_002 );  // コピーを禁止

    public:
      //  クラス初期化時に必要なパラメータ
      typedef struct
      {
        AppToolGaugePartsLayout::INIT_PARAM baseInitParam;

        //  ゲージ通常色切り替えアニメidx(アニメ名「color_normal」)
        s32 color_normal_anime_idx;

        //  ゲージ半分以下の色切り替えアニメidx(アニメ名「color_half」)
        s32 color_half_anime_idx;

        //  ゲージ現象の色切り替えアニメidx(アニメ名「color_few」)
        s32 color_few_anime_idx;

      } INIT_PARAM;

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  コンストラクタ
        */
      //-----------------------------------------------------------------------------------------
      AppToolHPGaugePartsLayoutGAG_002( const INIT_PARAM& param, gfl2::heap::HeapBase* pHeap );

    private:

      //-----------------------------------------------------------------------------------------
      /**
        * @brief  ゲージの値を設定（アニメせずにいきなり反映）
        * @param  nowNumber   : 数基準となる現在値を設定
        * @param  maxNumber   : 数基準の最大値を設定（最小値は０と決めているので最小値はなしにしている）
        */
      //-----------------------------------------------------------------------------------------
      virtual void _SetNumber( const f32 nowNumber, const s32 maxNumber );

      INIT_PARAM  m_initParam;
   };

GFL_NAMESPACE_END( tool )
GFL_NAMESPACE_END( app )

#endif // __APP_TOOL_HP_GAUGE_PARTS_LAYOUT_H__ 
