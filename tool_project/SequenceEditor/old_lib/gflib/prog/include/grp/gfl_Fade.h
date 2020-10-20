//=============================================================================
/**
 *
 *	@file		gfl_Fade.h
 *	@brief  フェードシステム
 *	@author hosaka genya -> takada keisuke
 *	@data		2011.06.07 -> 2012.08.06
 *
 */
//=============================================================================
#ifndef __GFL_FADE_H__
#define __GFL_FADE_H__
#pragma once


namespace gfl {
namespace grp {

const int FADE_DEFAULT_SYNC = 2; ///< 既定のフェード時間

namespace util {
class Texture;
}

class RequestUnit;

/**
 * @brief フェードクラス
 */
class Fade : public base::Singleton<Fade> {
    GFL_FORBID_COPY_AND_ASSIGN(Fade);
public:

    /**
     * @brief 表示ディスプレイ定義
     */
    typedef enum {
        SETUP_UPPER = 0, ///< 上画面
        SETUP_MAX
    } SETUP_DISPLAY_TYPE;

    /**
     * @brief 上画面アニメID定義
     */
    enum {
        ANMID_UPPER_TEST_IN = 0,
        ANMID_UPPER_TEST_OUT,
        ANMID_UPPER_WC_IN,
        ANMID_UPPER_WC_OUT,
        ANMID_UPPER_WB_IN,
        ANMID_UPPER_WB_OUT,
        ANMID_UPPER_WS_IN_UP,
        ANMID_UPPER_WS_OUT_UP,
        ANMID_UPPER_WS_IN_DOWN,
        ANMID_UPPER_WS_OUT_DOWN,
        ANMID_UPPER_WS_IN_LEFT,
        ANMID_UPPER_WS_OUT_LEFT,
        ANMID_UPPER_WS_IN_RIGHT,
        ANMID_UPPER_WS_OUT_RIGHT,
        ANMID_UPPER_WR_IN,
        ANMID_UPPER_WR_OUT,
        ANMID_UPPER_NUM,
        ANMID_UPPER_NULL,
    };

    /**
     * @brief レイアウトリソースデータ
     */
    struct LYT_RES_DATA {
        gfl::fs::ArcFile::ARCID arcID; ///< レイアウトリソースが含まれているアーカイブ
        gfl::fs::ArcFile::ARCDATID arcDatID; ///< レイアウトリソース（?BCLYT）
        u32 addTexNum; ///< 指定のリソースに追加登録するリソース数の最大数
    };

    /**
     * @brief レイアウトワーク管理
     */
    typedef struct {
        g2d::LytWk* pLytWk; ///< レイアウトワーク
        SETUP_DISPLAY_TYPE setupDisp; ///< 描画画面
        bool enable; ///< 表示設定
    } LAYOUT_WORK;

    /**
     * @brief レイアウトワーク設定データ
     */
    typedef struct {
        u32 resIndex; ///< 設定したリソースのインデックス
        g2d::LytArcIndex layoutDataID; ///< resID内のレイアウトデータID
        const g2d::LytArcIndex * anmDataIDTbl; ///< アニメーションデータIDテーブル アニメのない場合はNULL
        u32 anmTblNum; ///< アニメーションデータIDテーブル数 アニメのない場合は0
        u32 anmMax; ///< 登録アニメーション最大数
        bool calculateMtxFlag; ///< 非表示ペインに行列計算を行うか
        SETUP_DISPLAY_TYPE setupDisp; ///< ディスプレイ
        bool enable; ///< 表示設定
    } LYTWK_DATA;

    /**
     * @brief コールバックデータ
     */
    typedef struct {
        LAYOUT_WORK* ExternalLytWork; ///< レイアウトワーク管理へのポインタ
        void* CallBackWork; ///< ポインタの扱いには注意すること(外部管理)
    } CALLBACK_DATA;

    /**
     * @brief 画面指定
     */
    enum DISP {
        DISP_APPER = 0, ///< 上画面
        DISP_LOWER, ///< 下画面
        DISP_DOUBLE, ///< 両画面
        DISP_MAX = DISP_DOUBLE, ///< 画面数
    };

    /**
     * @brief F.O.タイプ指定
     */
    enum OUTTYPE {
        OUTTYPE_BLACK, ///< 黒
        OUTTYPE_WHITE, ///< 白
        OUTTYPE_BLACK_CAPTURE, ///< 画面キャプチャで黒に途中までF.O.（必ずF.O.が終わってからF.I.すること）
        OUTTYPE_WHITE_CAPTURE, ///< 画面キャプチャで白に途中までF.O.（必ずF.O.が終わってからF.I.すること）
        OUTTYPE_CAPTURE, ///< 画面キャプチャ
        OUTTYPE_CIRCLE_SHRINK, ///< 円形収縮
        OUTTYPE_BALL, ///< ボール
        OUTTYPE_UP, ///< 上
        OUTTYPE_DOWN, ///< 下
        OUTTYPE_LEFT, ///< 左
        OUTTYPE_RIGHT, ///< 右
        OUTTYPE_DIAMOND, ///< 菱形
        OUTTYPE_EXTERNAL_LAYOUT_DATA, ///< 外部レイアウトデータ
    };

    /**
     * @brief F.I.タイプ指定
     */
    enum INTYPE {
        INTYPE_BLACK, ///< 黒
        INTYPE_WHITE, ///< 白
        INTYPE_BLACK_CAPTURE, ///< 画面キャプチャで黒に途中までF.O.（必ずF.O.が終わってからF.I.すること）
        INTYPE_WHITE_CAPTURE, ///< 画面キャプチャで白に途中までF.O.（必ずF.O.が終わってからF.I.すること）
        INTYPE_CAPTURE, ///< 画面キャプチャ
        INTYPE_CIRCLE_SHRINK, ///< 円形収縮
        INTYPE_BALL, ///< ボール
        INTYPE_UP, ///< 上
        INTYPE_DOWN, ///< 下
        INTYPE_LEFT, ///< 左
        INTYPE_RIGHT, ///< 右
        INTYPE_DIAMOND, ///< 菱形
    };

    typedef bool (FadeCallbackFunc) (CALLBACK_DATA* data); ///< FadeCallbackFunc関数の定義

    static const s32 SCREEN_WIDTH = 512; ///< 画面の横幅
    static const s32 SCREAN_HEIGHT = 256; ///< 画面の縦幅
    static const int BLOCK_WIDTH_SIZE = 8;
    static const int BLOCK_HEIGHT_SIZE = 8;

    /**
     * @brief コンストラクタ
     * @param heap_allocator ヒープアロケータ
     * @param device_allocator デバイスヒープアロケータ
     * @param grpsys グラフィックシステム
     * @param lyt_res レイアウトリソース
     */
    Fade(heap::NwAllocator* heap_allocator, heap::NwAllocator* device_allocator, GraphicsSystem* grpsys, const void* lyt_res = NULL);

    /**
     *@brief  デストラクタ
     */
    ~Fade(void);

    /**
     *@brief  主処理
     */
    void Update(void);

    /**
     * @brief F.O.リクエスト
     * @param disp 対象面
     * @param start_col スタート時の画面色(0~1.0f) NULLだと前回の色からスタート
     * @param end_col 終了時の画面色(0~1.0f)
     * @param sync F.O.に費やす時間
     * @param is_keep_buffer 開始時のバッファを保存するかどうか
     */
    void RequestOut(DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

    /**
     * @brief F.O.リクエスト≪タイプ指定版≫
     * @param disp 対象面
     * @param type F.O.タイプ
     * @param sync F.O.に費やす時間
     * @param is_keep_buffer 開始時のバッファを保存するかどうか
     */
    void RequestOut(DISP disp = DISP_DOUBLE, OUTTYPE type = OUTTYPE_CAPTURE, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

    /**
     * @brief F.O.リクエスト≪外部Layoutアニメーション版≫
     * @param lyt_res 外部レイアウトリソース
     * @param anmNum アニメーション数
     * @param is_keep_buffer 開始時のバッファを保存するかどうか
     */
    void RequestOut(const void* lyt_res, u8 anmNum, bool is_keep_buffer = false);

    /**
     * @brief F.I.リクエスト
     * @param disp 対象面
     * @param sync F.I.に費やす時間
     */
    void RequestIn(DISP disp = DISP_DOUBLE, u32 sync = FADE_DEFAULT_SYNC);

    /**
     * @brief F.I.リクエスト≪タイプ指定版≫
     * @param disp 対象面
     * @param type F.I.タイプ
     * @param sync F.I.に費やす時間
     */
    void RequestIn_Ex(DISP disp = DISP_DOUBLE, INTYPE type = INTYPE_BLACK, u32 sync = FADE_DEFAULT_SYNC);

    /**
     * @brief 強制的にF.O.状態にする
     * @param disp 対象面
     * @param col 終了時の画面色 
     */
    void ForceOut(DISP disp, const math::VEC4* col);

    /**
     * @brief フェードが終了したかどうかを取得
     * @return フェードが終了したかどうか 
     */
    bool IsEnd(void);

    /**
     * @brief ポーズON/OFF
     * @param is_pause ポーズフラグ
     */
    void SetPause(bool is_pause);

    /**
     * @brief ポーズ状態取得
     * @retval ポーズ状態
     */
    bool IsPause(void) const;

    /**
     * @brief フェード状態リセット
     * @param disp 対象面
     */
    void Reset(DISP disp);

    /**
     * @brief フェード処理カットフラグをセット 
     * @param disp 対象面
     * @param isCutProcess カットするかどうか
     */
    void SetIsCutProcess(DISP disp, bool isCutProcess);

    /**
     * @brief フェード用のカラーバッファを取得
     * @param type
     * @param id
     * @return カラーバッファ
     */
    void* GetColorBuffer(DisplayType type, int id);

    /**
     * @brief フェード用のテクスチャワークの取得
     * @param type DISPLAY_UPPERかDISPLAY_LOWER
     * @param id 上なら２枚、下なら１枚持っている
     */
    util::Texture* GetTextureWork(DisplayType type, int id);

    /**
     * @brief 2D関連を生成
     * @param lyt_res レイアウトリソース
     */
    void Create2D(const void* lyt_res);

    /**
     * @brief レイアウトシステム作成
     */
    void CreateLayoutSystem(void);

    /**
     * @brief レイアウト環境作成
     */
    void CreateLayoutEnv(void);

    /**
     * @brief レイアウトワーク作成
     * @param lytMax 作成数
     */
    void CreateLayoutWork(u32 lytMax);

    /**
     * @brief 外部データ用のレイアウトワーク作成
     * @param lytMax 作成数
     */
    void CreateLayoutWorkOfExternalData(u32 lytMax = 1);

    /**
     * @brief レイアウトリソース作成
     */
    void CreateLayoutResource(void);

    /**
     * @brief 外部データ用のレイアウトリソース作成
     */
    void CreateLayoutResourceOfExternalData(void);

    /**
     * @brief レイアウトリソース読み込み 
     * @param lyt_res レイアウトリソース
     * @param resTbl レイアウトリソースデータ
     * @param comp 圧縮フラグ
     */
    void LoadLayoutResource(const void* lyt_res, const LYT_RES_DATA* resTbl, bool comp = false);

    /**
     * @brief 外部データ用のレイアウトリソース読み込み
     * @param lyt_res レイアウトリソース
     */
    void LoadLayoutResourceOfExternalData(const void* lyt_res);

    /**
     * @brief テクスチャ転送
     * @param area  転送エリア
     */
    void TransferLayoutTexture(MemoryArea area = MEMORY_AREA_FCRAM);

    /**
     * @brief 外部データ用のテクスチャ転送
     * @param area 転送エリア
     */
    void TransferLayoutTextureOfExternalData(MemoryArea area = MEMORY_AREA_FCRAM);

    /**
     * @brief レイアウトワークをセットする 
     * @param layoutData レイアウトワークデータ
     * @param id セットするレイアウトのID
     * @param num セットするデータ数(デフォルト=1)
     */
    void SetLayoutWork(const LYTWK_DATA* layoutData, u32 id, u32 num);

    /**
     * @brief 外部データ用のレイアウトワークをセット
     * @param anmNum アニメの数
     */
    void SetLayoutWorkOfExternalData(u8 anmNum);

    /**
     * @brief レイアウトリソース削除
     */
    void DeleteLayoutResource(void);

    /**
     * @brief 外部データ用のレイアウトリソース削除
     */
    void DeleteLayoutResourceOfExternalData(void);

    /**
     * @brief レイアウトワーク削除
     */
    void DeleteLayoutWork(void);

    /**
     * @brief 外部データ用のレイアウトワーク削除
     */
    void DeleteLayoutWorkOfExternalData(void);

    /**
     * @brief レイアウト環境削除
     */
    void DeleteLayoutEnv(void);

    /**
     * @brief レイアウトシステム削除
     */
    void DeleteLayoutSystem(void);

    /**
     * @brief コールバックのセット 
     * @memo レイアウトシステムを削除
     * @param pCallbackFunc コールバック関数
     * @param pWork コールバックワーク
     */
    void SetCallback(FadeCallbackFunc* pCallbackFunc, void* pWork);

    /**
     * @brief F.I.待機状態かどうかを取得
     * @param 対象面
     */
    bool IsFadeInStandbyState(DISP disp);

private:
    bool mIsCreate2D; ///< 2Dが生成されたどうか
    static const g2d::LytWkSetUp LytWkDefaultSetup[2]; ///< レイアウトワークの既定設定 
    g2d::LytSys* mLayoutSystem; // レイアウトシステム
    g2d::Env* mUpperLayoutEnv; // 上画面のレイアウト環境
    g2d::Env* mLowerLayoutEnv; // 下画面のレイアウト環境
    LAYOUT_WORK* mLayoutWork; // レイアウトワーク管理データ
    u32 mLwkMax; // レイアウトワーク管理データ数
    g2d::LytResID mLayoutResID; // リソースID

    bool mIsCreate2DOfExternalData; ///< 外部データ用の2Dが生成されたかどうか
    LAYOUT_WORK* mLayoutWorkOfExternalData; // 外部データ用のレイアウトワーク管理データ
    u32 mLwkMaxOfExternalData; // 外部データ用のレイアウトワーク管理データ数
    g2d::LytResID mLayoutResIDOfExternalData; // 外部データ用のリソースID

    heap::NwAllocator* m_HeapAllocator; ///< ヒープアロケーター
    heap::NwAllocator* m_DeviceAllocator; ///< デバイスヒープアロケーター
    GraphicsSystem* m_GraphicsSystem; ///< グラフィックシステム
    RequestUnit* m_RequestUnit[ DISPLAY_NORMAL_MAX ]; ///< ユニット

    FadeCallbackFunc* m_pCallbackFunc; ///< コールバック関数ポインタ
    void* m_pCallbackWork; ///< コールバックワーク

    static const int SYNC_CAPTURE_OUT = 2; ///< キャプチャ アウトにかかる時間

    /**
     * @brief F.O.リクエスト≪キャプチャタイプ≫
     * @param disp 対象面
     */
    void RequestOutCapture(DISP disp);

    /**
     * @brief F.O.リクエスト≪Colorキャプチャタイプ≫ 
     * @param disp 対象面
     * @param start_col スタート時の画面色
     * @param end_col 終了時の画面色
     * @param sync キャプチャF.O.に費やす時間
     */
    void RequestOutColorCapture(DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync);

    /**
     * @brief F.O.リクエスト≪Layoutアニメーションタイプ≫ 
     * @param disp 対象面
     * @param type レイアウトタイプ
     * @param is_keep_buffer 開始時のバッファを保存するかどうか
     */
    void RequestOutLayoutAnimation(DISP disp, OUTTYPE type, bool is_keep_buffer);

    /**
     * @brief F.O.リクエスト≪外部Layoutアニメーションタイプ≫ 
     * @param disp 対象面
     * @param type レイアウトタイプ
     * @param is_keep_buffer 開始時のバッファを保存するかどうか
     */
    void RequestOutLayoutAnimationOfExternalData(DISP disp, OUTTYPE type, bool is_keep_buffer);
};

/**
 * @brief 主処理
 * @note kujiraではシステムでコールするので、アプリ単位でコールする必要はない 
 */
extern void Fade_Update(void);

/**
 * @brief F.O.リクエスト 
 * @param disp 対象面
 * @param start_col スタート時の画面色
 * @param end_col 終了時の画面色
 * @param sync F.O.に費やす時間
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
extern void Fade_RequestOut(Fade::DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

/**
 * @brief F.I.リクエスト≪タイプ指定版≫
 * @param disp 対象面
 * @param type F.I.タイプ
 * @param sync F.I.に費やす時間
 */
extern void Fade_RequestOut(Fade::DISP disp = Fade::DISP_DOUBLE, Fade::OUTTYPE type = Fade::OUTTYPE_CAPTURE, u32 sync = FADE_DEFAULT_SYNC, bool is_keep_buffer = false);

/**
 * @brief F.O.リクエスト≪外部Layoutアニメーション版≫
 * @param lyt_res 外部レイアウトリソース
 * @param anmNum アニメーション数
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
extern void Fade_RequestOut(const void* lyt_res, u8 anmNum, bool is_keep_buffer = false);

/**
 * @brief F.I.リクエスト
 * @param disp 対象面
 * @param sync F.I.に費やす時間
 */
extern void Fade_RequestIn(Fade::DISP disp = Fade::DISP_DOUBLE, u32 sync = FADE_DEFAULT_SYNC);

/**
 * @brief F.I.リクエスト≪タイプ指定版≫
 * @param disp 対象面
 * @param type F.I.タイプ
 * @param sync F.I.に費やす時間
 */
extern void Fade_RequestIn_Ex(Fade::DISP disp = Fade::DISP_DOUBLE, Fade::INTYPE type = Fade::INTYPE_BLACK, u32 sync = FADE_DEFAULT_SYNC);

/**
 * @brief 強制的にF.O.状態にする
 * @param disp 対象面
 * @param col 終了時の画面色 
 */
extern void Fade_ForceOut(Fade::DISP disp, const math::VEC4* col);

/**
 * @brief フェードが終了したかどうかを取得
 * @return フェードが終了したかどうか 
 */
extern bool Fade_IsEnd(void);

/**
 * @brief ポーズON/OFF
 * @param is_pause ポーズフラグ
 */
extern void Fade_SetPause(bool is_pause);

/**
 * @brief ポーズ状態取得
 * @retval ポーズ状態
 */
extern bool Fade_IsPause(void);

/**
 * @brief フェード状態リセット
 * @param disp 対象面
 */
extern void Fade_Reset(Fade::DISP disp);

/**
 * @brief フェード処理カットフラグをセット 
 * @param disp 対象面
 * @param isCutProcess カットするかどうか
 */
extern void Fade_SetIsCutProcess(Fade::DISP disp, bool isCutProcess);

/**
 * @brief コールバックのセット 
 * @memo レイアウトシステムを削除
 * @param pCallbackFunc コールバック関数
 * @param pWork コールバックワーク
 */
extern void Fade_SetCallback(Fade::FadeCallbackFunc* func, void *work = NULL);

/**
 * @brief F.I.待機状態かどうかを取得
 * @param 対象面
 */
extern bool Fade_IsFadeInStandbyState(Fade::DISP disp);

} // namespace grp
} // namespace gfl

#endif // __GFL_FADE_H__
