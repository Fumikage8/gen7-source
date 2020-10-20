#if PM_DEBUG
//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		FieldDebugInfo.h
 *	@brief  フィールドデバッグインフォ―
 *	@author	tomoya takahashi
 *	@date		2012.09.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __FIELDDEBUGINFO_H__
#define __FIELDDEBUGINFO_H__
#pragma once

#include <macro/include/gfl2_Macros.h>
#include <gfx/include/gfl2_types.h>

#include <gfx/include/gfl2_DrawUtil.h>
#include <RenderingEngine/include/Renderer/gfl2_RenderingPipeLine.h>


namespace gfl2{ namespace util {
  class DrawUtilText;
  class UtilTextRenderingPipeLine;
}}

GFL_NAMESPACE_BEGIN(Field)
GFL_NAMESPACE_BEGIN(Finder)
class DebugInfo;
GFL_NAMESPACE_END(Finder)
GFL_NAMESPACE_END(Field)
GFL_NAMESPACE_BEGIN(Field)
  class Fieldmap;

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

#define F_DEBUG_INFO
//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					クラス宣言
*/
//-----------------------------------------------------------------------------

  //----------------------------------------------------------------------------
  /**
   *	@brief  フィールド　デバッグインフォウィンドウ
   */
  //-----------------------------------------------------------------------------
  class DebugInfoWin
  {
    GFL_FORBID_COPY_AND_ASSIGN(DebugInfoWin); //コピーコンストラクタ＋代入禁止

  public:
    enum Type{
      TYPE_NONE,
      TYPE_INFO_ONLY,
      TYPE_INFO_PERFORMANCE,
    };

    // モーション情報
    static s32 m_DebugInfo_MotionUs;
    static s32 m_DebugInfo_MotionNum;
    
    // コリジョン情報
    static s32 m_DebugInfo_CollisionUs;
    static s32 m_DebugInfo_CollisionTriNum;

    // メッシュレンダリング　情報
    static s32 m_DebugInfo_MeshRenderUs;
    static s32 m_DebugInfo_MeshRenderNum;

    // レイアウト　レンダリング　情報
    static s32 m_DebugInfo_LayoutUpperUs;
    static s32 m_DebugInfo_LayoutUpperNum;
    static s32 m_DebugInfo_LayoutLowerUs;
    static s32 m_DebugInfo_LayoutLowerNum;

    // プレイヤー更新
    static s32 m_DebugInfo_PlayerUpdateUs;
    // 動作モデル更新
    static s32 m_DebugInfo_MmodelUpdateUs;

    // イベントチェック
    static s32 m_DebugInfo_EventCheckUs;

    // ギミック更新
    static s32 m_DebugInfo_GimmickUs;

    // シーン計算
    static s32 m_DebugInfo_SceneCalcUs;
    static s32 m_DebugInfo_SceneSubmitviewUs;

    // SubWindow
    static s32 m_DebugInfo_SubwindowDrawUs;

    
  public:

    // コンストラクタ・デストラクタ
    DebugInfoWin( void );
    ~DebugInfoWin();
  
    //----------------------------------------------------------------------------
    /**
     *	@brief  初期化
     */
    //-----------------------------------------------------------------------------
    void Initialize( Fieldmap* p_fieldmap );

    //----------------------------------------------------------------------------
    /**
     *	@brief  破棄
     */
    //-----------------------------------------------------------------------------
    void Finalize( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  更新
     */
    //-----------------------------------------------------------------------------
    void Update( void );

    //----------------------------------------------------------------------------
    /**
     *	@brief  描画
     */
    //-----------------------------------------------------------------------------
    void Draw( Type type, const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );

    // Fieldのセットアップにかかった時間を表示
    void StartSetupTimeDisp( void );

    // ファイル読み込みのエラー結果設定
    void UpdateFileReadError( bool is_error, s32 readUs );

    // 表示ON/OFF
    static bool IsDebugInfoOn( void );

    // デバッグ表示オーバーフラグ
    void SetDebugDrawOver( b32 flag ){ m_IsDebugDrawOver = flag; }

    // ファインダーデバッグ表示
    void SetFinderInfo( Field::Finder::DebugInfo *pInfo ){ m_pFinderInfo = pInfo; }

  private:
    
    // 読み込み時間表示モジュール
    void initializeSetupTime( void );
    void finalizeSetupTime( void );
    void startSetupTimeDisp( void );
    void updateSetupTime( void );
    void drawSetupTime( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );


    // 処理負荷オーバー表示モジュール
    void initializeProcTime( void );
    void finalizeProcTime( void );
    void updateProcTime( void );
    void drawProcTime( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

    // 読み込み遅延表示モジュール
    void initializeFileRead( void );
    void finalizeFileRead( void );
    void updateFileRead( void );
    void drawFileRead( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
    void setParamFileRead( bool is_error, s32 readUs );

    // 処理負荷
    void drawPerformance( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

    // デバッグ表示オーバー
    void drawDebugDrawOver( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
    
    // DrawUtilTextモジュール
    void initializeDrawUtilText(void);
    void finalizeDrawUtilText(void);
    void drawDrawUtilText(void);

    // ココドコ表示
    void drawWhereIsThis( void );
    // ゾーンデバッグ表示
    void drawZone( void );
    // アトリビュートデバッグ表示
    void drawAttribute( void );
    // 歩数デバッグ表示
    void drawStep( void );
    // プレイヤーデバッグ表示
    void drawPlayer( void );
    // 時間デバッグ表示
    void drawTime( void );
    // Hio描画
    void drawHioOn( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );

  private:
    enum
    {
      SEQ_SETUP_NONE = 0,
      SEQ_SETUP_FADEIN,
      SEQ_SETUP_UPDATE,
      SEQ_SETUP_FADEOUT,
      SEQ_SETUP_END,

      SEQ_PROC_UPDATE = 0,

      SEQ_FILE_NONE = 0,
      SEQ_FILE_FADEIN,
      SEQ_FILE_UPDATE,
      SEQ_FILE_FADEOUT,
    };
    enum{
      P_OV_UPDATE,
      P_OV_DRAW,
      P_OV_GPU,
      P_OV_NUM,
    };
    
    static const u32 FADE_TIME = 4;

    Fieldmap* m_pFieldmap;

    gfl2::util::DrawUtilText* m_pDrawUtilText;

    Field::Finder::DebugInfo *m_pFinderInfo; // ファインダーデバッグ描画

    
    // 読み込み時間表示モジュールメンバ
    u32 m_SetupTimeSeq;
    s32 m_SetupTimeCount;       // 表示カウンタ


    // 処理負荷オーバー表示モジュールメンバ
    u32 m_ProcTimeSeq;    // 処理負荷オーバー表示モジュールメンバ
    s32 m_ProcTimeCount;  // 処理負荷オーバー表示カウンタ
    s32 m_ProcTimeOverTime[P_OV_NUM]; //  処理負荷オーバー時間

    // 読み込み遅延オーバー
    u32 m_FileReadSeq;    // 読み込み遅延オーバー表示
    u32 m_FileReadCount;  // 読み込み遅延オーバー表示

    bool  m_FileReadLastError;
    s32   m_FileReadLastUs;

    // デバッグ表示オーバー
    b32 m_IsDebugDrawOver;

    //ZoneID表示用
    void drawZoneIDInfo( const gfl2::gfx::Surface *pRenderTarget, const gfl2::gfx::Surface *pDepthStencil );
  };


  /**
   *  @brief デバッグのテキスト描画の前に実行されるデバッグ描画処理
   *  @note Fieldmapを保持しているので、他のデバッグ描画でも使用可能です。
   */
  class DebugTextBeforeRenderPath : public gfl2::renderingengine::renderer::RenderingPipeLine::IRenderPath
  {
  public:

    DebugTextBeforeRenderPath(){}
    virtual ~DebugTextBeforeRenderPath(){}
    virtual void Execute( const gfl2::renderingengine::renderer::RenderingPipeLine::DrawContext& rDrawContext );
    inline void SetupFieldmap( Fieldmap * pFieldmap ){ m_pFieldmap = pFieldmap; }
  
  private:
  
    Fieldmap * m_pFieldmap;
  };


GFL_NAMESPACE_END(Field)

#endif	// __FIELDDEBUGINFO_H__


#endif // PM_DEBUG
