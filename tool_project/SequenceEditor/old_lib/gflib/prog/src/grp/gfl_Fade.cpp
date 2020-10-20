//=============================================================================
/**
 *
 *	@file		gfl_Fade.cpp
 *	@brief  フェードシステム
 *	@author hosaka genya -> takada keisuke
 *	@data		2011.06.07 -> 2012.08.06
 *
 */
//=============================================================================

#include <grp/gfl_Fade.h>
#include <ro/gfl_RoManager.h>

#include "fade_bg_upper.h"
#include "fade_bg_upper_pane.h"

#include "grp/g2d/gfl_LytSys.h"
#include "grp/util/gfl_UtilTexture.h"

//-----------------------------------------------------------------------------
/**
 *    Singletonインスタンスの実体を定義
 */
//-----------------------------------------------------------------------------
#if GFL_ENABLE_RO
SINGLETON_INSTANCE_DEFINITION(gfl::grp::Fade);
template class gfl::base::SingletonAccessor<gfl::grp::Fade>;
#endif

namespace gfl {
namespace grp {

static void _FillScreen(GraphicsSystem* grpsys, DisplayType type, math::VEC4 color, util::Texture* tex = NULL);
static void _KeepColorBuffer(GraphicsSystem* grpsys, DisplayType type, void* out_buf);
static void _CreateTexture(GraphicsSystem* grpsys, DisplayType type, util::Texture** out_tex, const void* buf);
static void _TransTexture(GraphicsSystem* grpsys, DisplayType type, util::Texture** tex, const void* p_data);
static void _FillAlphaColorBuffer(DisplayType type, void* buf);
static void _DrawColorBuffer(GraphicsSystem* grpsys, DisplayType type, const void* buf);

/**
 * @brief リクエストユニット
 */
class RequestUnit {
  GFL_FORBID_COPY_AND_ASSIGN(RequestUnit);
public:

  /**
   * @brief フェード状態
   */
  enum State {
    ST_FADE_NONE,
    ST_FADE_OUT,
    ST_FADE_WAIT,
    ST_FADE_IN,
  };

  /**
   * @brief フェードタイプ
   */
  enum TYPE {
    TYPE_NORMAL = 0,
    TYPE_BLACK,
    TYPE_WHITE,
    TYPE_COLOR_CAPTURE, // キャプチャした絵を指定色0.5までF.O. -> A抜きでF.I.
    TYPE_CAPTURE,
    TYPE_CIRCLE_SHRINK,
    TYPE_BALL, ///< ボール
    TYPE_UP, ///< 上
    TYPE_DOWN, ///< 下
    TYPE_LEFT, ///< 左
    TYPE_RIGHT, ///< 右
    TYPE_DIAMOND, ///< 菱形
    TYPE_EXTERNAL_LAYOUT_DATA, ///< 外部レイアウトデータ
  };

  void* GetColorBuffer(int id)
  {
    return m_ColorBuffer[id];
  }

  util::Texture* GetTextureWork(int id)
  {
    return m_Texture[id];
  }

  /**
   * @brief タイプを取得する
   * @return タイプ
   */
  TYPE GetType(void)
  {
    return m_Type;
  }

  /**
   * @brief ポーズON/OFF
   * @param is_pause ポーズフラグ
   */
  void SetPause(bool is_pause)
  {
    m_bPause = is_pause;
  }

  /**
   * @brief ポーズ中かどうかを取得する
   * @return ポーズ中かどうか
   */
  bool IsPause(void) const
  {
    return m_bPause;
  }

  /**
   * @brief 外部データ用のレイアウトワークをセットする
   * @param lyt レイアウトワーク
   */
  void SetLayoutWorkOfExternalData(Fade::LAYOUT_WORK* lyt)
  {
    mLayoutWorkOfExternalData = lyt;
  }

  /**
   * @brief 処理カットフラグをセット
   * @note Debug用
   * @param isCutProcess
   */
  void SetIsCutProcess(bool isCutProcess)
  {
    mIsCutProcess = isCutProcess;
  }

  /**
   * @brief 状態を取得する
   * @return 状態
   */
  State GetState(void)
  {
    return m_State;
  }

  RequestUnit(heap::NwAllocator* heap_allocator, heap::NwAllocator* device_allocator, bool is_apper, GraphicsSystem* grpsys, Fade::LAYOUT_WORK* layoutWork);
  ~RequestUnit(void);
  void SetupOut(const math::VEC4* start_col, const math::VEC4* end_col, u32 sync, bool is_keep_buffer, TYPE type, bool is_set_nowcol = false);
  void SetupOutLayoutAnimation(bool is_keep_buffer, TYPE type);
  void SetupOutLayoutAnimationOfExternalData(bool is_keep_buffer, TYPE type);
  void SetupIn(u32 sync);
  void SetupInEx(u32 sync, TYPE type);
  void SetupInNormalAndCapture(void);
  void SetupInColorCapture(void);
  void SetupInLayoutAnimation(void);
  void Update(GraphicsSystem* grpsys);
  bool IsEnd(void);
  void Reset(void);
  void ResetLayoutAnimation(void);
  void StartAnimationOfExternalData(void);

private:

  TYPE m_Type;
  heap::NwAllocator* m_HeapAllocator;
  heap::NwAllocator* m_DeviceAllocator;
  u32 m_FadeSync;
  u32 m_NowSync;
  math::VEC4 m_StartCol;
  math::VEC4 m_EndCol;
  math::VEC4 m_NowCol;
  bool m_bApper;
  bool m_bKeepColorBuffer;
  bool m_bPause;
  bool m_bCaptureReq;
  State m_State;
  void* m_ColorBuffer[2];
  util::Texture* m_Texture[2];
  Fade::LAYOUT_WORK* mLayoutWork;
  Fade::LAYOUT_WORK* mLayoutWorkOfExternalData;
  u8 m_currentAnimID; // 再生中のアニメインデックス
  bool mIsCutProcess; ///< Debug用：処理カットフラグ
  bool mIsStereoVisionAllowedWhenCapture; ///< キャプチャの時に立体視だったかどうか

  void Calc(GraphicsSystem* grpsys);
  void CalcLayoutAnimation(GraphicsSystem * grpsys);
  void CalcLayoutAnimationOfExternalData(GraphicsSystem * grpsys);
  void* GetColorBufferForDraw(DisplayType dispType);
  void DrawColorBufferOfDisplayUpper(GraphicsSystem* grpsys);
  void DrawColorBufferOfDisplayLower(GraphicsSystem* grpsys);
  util::Texture* GetTextureForFillScreen(DisplayType dispType);
  void FillScreenOfDisplayUpper(GraphicsSystem* grpsys, bool isUseTexture);
  void FillScreenOfDisplayLower(GraphicsSystem* grpsys, bool isUseTexture);
  void DrawNormal(GraphicsSystem* grpsys);
  void DrawCapture(GraphicsSystem* grpsys);
  void DrawColorCapture(GraphicsSystem* grpsys);
  void DrawLayoutAnimation(GraphicsSystem* grpsys);
  void DrawLayoutAnimationOfExternalData(GraphicsSystem* grpsys);
  void EndProcessNormalAndCapture(void);
  void EndProcessColorCapture(GraphicsSystem * grpsys);
  void EndProcessLayoutAnimation(void);
  void EndProcessLayoutAnimationOfExternalData(void);
  void StartAnimation(u32 animation_index);
  void PauseAnimation(u32 animation_index, u32 setFrame, bool pause);
  void StopAnimation(u32 animation_index);
  bool IsAnimationEnd(u32 animation_index) const;
  void StopAnimationOfExternalData(void);
  bool IsAnimationEndOfExternalData(void) const;
};

/**
 * @brief コンストラクタ
 * @param heap_allocator ヒープアロケータ
 * @param device_allocator デバイスヒープアロケータ
 * @param is_apper上画面かどうか
 * @param grpsys グラフィックシステム
 * @param layoutWork レイアウトワーク
 */
RequestUnit::RequestUnit(heap::NwAllocator* heap_allocator, heap::NwAllocator* device_allocator, bool is_apper, GraphicsSystem* grpsys, Fade::LAYOUT_WORK* layoutWork)
{
  math::VEC4 temp(0, 0, 0, 0);

  m_HeapAllocator = heap_allocator;
  m_DeviceAllocator = device_allocator;
  m_bApper = is_apper;
  m_bPause = false;
  m_StartCol = temp;
  m_EndCol = temp;
  m_NowCol = temp;
  m_State = ST_FADE_NONE;
  mLayoutWork = layoutWork;
  m_currentAnimID = Fade::ANMID_UPPER_NULL;
  mIsCutProcess = false;

  for (int i = 0; i < 2; i++) {
    m_Texture[i] = NULL;
    m_ColorBuffer[i] = NULL;
  }

  if (m_bApper) {
    static const DisplayType sc_DisplayType[2] = {
      DISPLAY_UPPER,
      DISPLAY_UPPER_RIGHT,
    };

    // カラーバッファを白く塗りつぶしておく
    // 後から立体視ONになる可能性もあるため、必ず
    // 両画面のバッファを確保
    for (int i = 0; i < 2; i++) {
      {
        u32 size = 4 * Fade::SCREEN_WIDTH * Fade::SCREAN_HEIGHT;
        m_ColorBuffer[i] = m_DeviceAllocator->Alloc(size, 16);
        gfl::std::MemFill(m_ColorBuffer[i], 0xff, size);
        // テクスチャを生成しておく
        _CreateTexture(grpsys, DISPLAY_UPPER, &m_Texture[i], m_ColorBuffer[i]); // 左目、右目同一設定じゃないことはないはず。
      }
    }
  } else {
    // 白く塗りつぶしておく
    u32 size = 4 * Fade::SCREEN_WIDTH * Fade::SCREAN_HEIGHT;
    m_ColorBuffer[0] = m_DeviceAllocator->Alloc(size, 16);
    gfl::std::MemFill(m_ColorBuffer[0], 0xff, size);

    // テクスチャを生成しておく
    _CreateTexture(grpsys, DISPLAY_LOWER, &m_Texture[0], m_ColorBuffer[0]);
  }

}

/**
 * @brief デストラクタ
 */
RequestUnit::~RequestUnit(void)
{
  for (int i = 0; i < 2; i++) {
    GFL_SAFE_DELETE(m_Texture[i]);
  }

  if (m_bApper) {
    for (int i = 0; i < 2; i++) {
      if (m_ColorBuffer[i]) {
        m_DeviceAllocator->Free(m_ColorBuffer[i]);
        m_ColorBuffer[i] = NULL;
      }
    }
  } else {
    m_DeviceAllocator->Free(m_ColorBuffer[0]);
  }
}

/**
 * @brief F.O.初期化
 * @param start_col スタート時の画面色(0~1.0f) NULLだと前回の色からスタート
 * @param end_col 終了時の画面色
 * @param sync F.I.に費やす時間
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 * @param type F.O.タイプ
 * @param is_set_nowcol 
 */
void RequestUnit::SetupOut(const math::VEC4* start_col, const math::VEC4* end_col, u32 sync, bool is_keep_buffer, TYPE type, bool is_set_nowcol)
{
  if (start_col == NULL) {
    m_StartCol = m_NowCol;
  } else {
    m_StartCol = *start_col;
  }

  m_EndCol = *end_col;

  if (is_set_nowcol) {
    m_NowCol = m_EndCol;
  }

  m_FadeSync = sync;
  m_bKeepColorBuffer = is_keep_buffer;
  m_bPause = false;
  m_Type = type;

  m_NowSync = 0;
  m_State = ST_FADE_OUT;
}

/**
 * @brief F.O.初期化≪Layoutアニメーション≫
 * @param is_keep_buffer 開始時のバッファを保存するかどうか 
 * @param type レイアウトアニメーションタイプ
 */
void RequestUnit::SetupOutLayoutAnimation(bool is_keep_buffer, TYPE type)
{
  m_bKeepColorBuffer = is_keep_buffer;
  m_Type = type;

  m_NowSync = 0;
  m_State = ST_FADE_OUT;

  if (m_bApper == true) {
    switch (type) {
    case TYPE_CIRCLE_SHRINK:
      m_currentAnimID = Fade::ANMID_UPPER_WC_OUT;
      break;
    case TYPE_BALL:
      m_currentAnimID = Fade::ANMID_UPPER_WB_OUT;
      break;
    case TYPE_UP:
      m_currentAnimID = Fade::ANMID_UPPER_WS_OUT_UP;
      break;
    case TYPE_DOWN:
      m_currentAnimID = Fade::ANMID_UPPER_WS_OUT_DOWN;
      break;
    case TYPE_LEFT:
      m_currentAnimID = Fade::ANMID_UPPER_WS_OUT_LEFT;
      break;
    case TYPE_RIGHT:
      m_currentAnimID = Fade::ANMID_UPPER_WS_OUT_RIGHT;
      break;
    case TYPE_DIAMOND:
      m_currentAnimID = Fade::ANMID_UPPER_WR_OUT;
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }

  if (mLayoutWork != NULL) {
    this->StartAnimation(m_currentAnimID);
  }
}

/**
 * @brief F.O.初期化≪外部Layoutアニメーション≫
 * @param is_keep_buffer 開始時のバッファを保存するかどうか 
 * @param type
 */
void RequestUnit::SetupOutLayoutAnimationOfExternalData(bool is_keep_buffer, TYPE type)
{
  m_bKeepColorBuffer = is_keep_buffer;
  m_Type = type;

  m_NowSync = 0;
  m_State = ST_FADE_OUT;
}

/**
 * @brief F.I.初期化
 * @param sync F.I.に費やす時間
 */
void RequestUnit::SetupIn(u32 sync)
{
  // レイアウトアニメをリセット
  this->ResetLayoutAnimation();

  m_FadeSync = sync;
  m_bPause = false;
  m_NowSync = 0;
  m_State = ST_FADE_IN;

  switch (m_Type) {
  case TYPE_NORMAL:
  case TYPE_CAPTURE:
    this->SetupInNormalAndCapture();
    break;
  case TYPE_COLOR_CAPTURE:
    this->SetupInColorCapture();
    break;
  case TYPE_CIRCLE_SHRINK:
  case TYPE_BALL:
  case TYPE_UP:
  case TYPE_DOWN:
  case TYPE_LEFT:
  case TYPE_RIGHT:
  case TYPE_DIAMOND:
    this->SetupInLayoutAnimation();
    break;
  default:
    //GFL_ASSERT(0);
    // 強制的にブラックフェードインをする
    m_Type = TYPE_NORMAL;
    m_StartCol = math::VEC4(0.0f, 0.0f, 0.0f, 0.0f);
    m_NowCol = math::VEC4(0.0f, 0.0f, 0.0f, 1.0f);  ///< NowColがフェードアウト状態でない場合に一瞬で画面が出てしまう不具合解消
    //m_EndCol = math::VEC4(0.0f, 0.0f, 0.0f, 1.0f);
    this->SetupInNormalAndCapture();
    break;
  }
}

/**
 * @brief F.I.初期化≪タイプ指定版≫
 * @param sync F.I.に費やす時間
 * @param type F.I.タイプ
 */
void RequestUnit::SetupInEx(u32 sync, TYPE type)
{
  m_Type = type;
  if (m_Type == TYPE_BLACK) {
    m_Type = TYPE_NORMAL;
    m_StartCol = math::VEC4(0.0f, 0.0f, 0.0f, 0.0f);
    m_NowCol = math::VEC4(0.0f, 0.0f, 0.0f, 1.0f);  ///< NowColがフェードアウト状態でない場合に一瞬で画面が出てしまう不具合解消
    //m_EndCol = math::VEC4(0.0f, 0.0f, 0.0f, 1.0f);
  } else if (m_Type == TYPE_WHITE) {
    m_Type = TYPE_NORMAL;
    m_StartCol = math::VEC4(1.0f, 1.0f, 1.0f, 0.0f);
    m_NowCol = math::VEC4(1.0f, 1.0f, 1.0f, 1.0f);  ///< NowColがフェードアウト状態でない場合に一瞬で画面が出てしまう不具合解消
    //m_EndCol = math::VEC4(1.0f, 1.0f, 1.0f, 1.0f);
  }
  this->SetupIn(sync);
}

/**
 * @brief F.I.初期化≪通常＆キャプチャ≫
 */
void RequestUnit::SetupInNormalAndCapture(void)
{
  m_EndCol = m_StartCol;
  m_StartCol = m_NowCol;
  m_bKeepColorBuffer = false;
}

/**
 * @brief F.I.初期化≪Colorキャプチャ≫
 */
void RequestUnit::SetupInColorCapture(void)
{
  math::VEC4 temp(1.0f, 1.0f, 1.0f, 1.0f);
  math::VEC4 temp2(1.0f, 1.0f, 1.0f, 0);
  m_StartCol = temp;
  m_EndCol = temp2;
  m_bKeepColorBuffer = true;
}

/**
 * @brief F.I.初期化≪Layoutアニメーション≫
 */
void RequestUnit::SetupInLayoutAnimation(void)
{
  if (m_bApper == true) {
    switch (m_Type) {
    case TYPE_CIRCLE_SHRINK:
      m_currentAnimID = Fade::ANMID_UPPER_WC_IN;
      break;
    case TYPE_BALL:
      m_currentAnimID = Fade::ANMID_UPPER_WB_IN;
      break;
    case TYPE_UP:
      m_currentAnimID = Fade::ANMID_UPPER_WS_IN_UP;
      break;
    case TYPE_DOWN:
      m_currentAnimID = Fade::ANMID_UPPER_WS_IN_DOWN;
      break;
    case TYPE_LEFT:
      m_currentAnimID = Fade::ANMID_UPPER_WS_IN_LEFT;
      break;
    case TYPE_RIGHT:
      m_currentAnimID = Fade::ANMID_UPPER_WS_IN_RIGHT;
      break;
    case TYPE_DIAMOND:
      m_currentAnimID = Fade::ANMID_UPPER_WR_IN;
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }

  if (mLayoutWork != NULL) {
    this->StartAnimation(m_currentAnimID);
  }
  m_bKeepColorBuffer = false;
}

/**
 * @brief 主処理
 * @param grpsys グラフィックシステム
 */
void RequestUnit::Update(GraphicsSystem* grpsys)
{
  if (mIsCutProcess) {
    m_State = ST_FADE_NONE;
  }

  if (m_State != ST_FADE_NONE) {
    if (m_bPause == false) {
      switch (m_Type) {
      case TYPE_NORMAL:
      case TYPE_CAPTURE:
      case TYPE_COLOR_CAPTURE:
        Calc(grpsys);
        break;
      case TYPE_CIRCLE_SHRINK:
      case TYPE_BALL:
      case TYPE_UP:
      case TYPE_DOWN:
      case TYPE_LEFT:
      case TYPE_RIGHT:
      case TYPE_DIAMOND:
        if (mLayoutWork != NULL) {
          CalcLayoutAnimation(grpsys);
        }
        break;
      case TYPE_EXTERNAL_LAYOUT_DATA:
        if (mLayoutWorkOfExternalData != NULL) {
          CalcLayoutAnimationOfExternalData(grpsys);
        }
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
    }

    // 描画
    switch (m_Type) {
    case TYPE_NORMAL:
      DrawNormal(grpsys);
      break;
    case TYPE_CAPTURE:
      DrawCapture(grpsys);
      break;
    case TYPE_COLOR_CAPTURE:
      DrawColorCapture(grpsys);
      break;
    case TYPE_CIRCLE_SHRINK:
    case TYPE_BALL:
    case TYPE_UP:
    case TYPE_DOWN:
    case TYPE_LEFT:
    case TYPE_RIGHT:
    case TYPE_DIAMOND:
      if (mLayoutWork != NULL) {
        DrawLayoutAnimation(grpsys);
      }
      break;
    case TYPE_EXTERNAL_LAYOUT_DATA:
      if (mLayoutWorkOfExternalData != NULL) {
        DrawLayoutAnimationOfExternalData(grpsys);
      }
      break;
    default:
      GFL_ASSERT(0);
      break;
    }

    // 終了判定
    if (m_State == ST_FADE_IN || m_State == ST_FADE_OUT) {
      switch (m_Type) {
      case TYPE_NORMAL:
      case TYPE_CAPTURE:
      {
        this->EndProcessNormalAndCapture();
      }
        break;
      case TYPE_COLOR_CAPTURE:
      {
        this->EndProcessColorCapture(grpsys);
      }
        break;
      case TYPE_CIRCLE_SHRINK:
      case TYPE_BALL:
      case TYPE_UP:
      case TYPE_DOWN:
      case TYPE_LEFT:
      case TYPE_RIGHT:
      case TYPE_DIAMOND:
      {
        this->EndProcessLayoutAnimation();
      }
        break;
      case TYPE_EXTERNAL_LAYOUT_DATA:
      {
        this->EndProcessLayoutAnimationOfExternalData();
      }
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
    }
  }
}

/**
 * @brief 演算処理
 * @param grpsys グラフィックシステム
 */
void RequestUnit::Calc(GraphicsSystem * grpsys)
{
  m_bCaptureReq = false;

  // カラーバッファ保存
  if (m_bKeepColorBuffer) {
    // F.O.終了時にキャプチャした絵をテクスチャに転送
    if (m_Type == TYPE_COLOR_CAPTURE && m_State == ST_FADE_IN && m_NowSync == 0) {
      if (m_bApper) {
        _TransTexture(grpsys, DISPLAY_UPPER, &m_Texture[0], m_ColorBuffer[0]);
        _TransTexture(grpsys, DISPLAY_UPPER_RIGHT, &m_Texture[1], m_ColorBuffer[1]);
      } else {
        _TransTexture(grpsys, DISPLAY_LOWER, &m_Texture[0], m_ColorBuffer[0]);
      }
    } else if (m_NowSync == 0) {
      m_bCaptureReq = true;
      if (m_bApper) {
        if (nn::gx::IsStereoVisionAllowed()) {
          grpsys->RequestReadPixels(m_ColorBuffer[0], m_ColorBuffer[1], NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
          mIsStereoVisionAllowedWhenCapture = true;
        } else {
          grpsys->RequestReadPixels(m_ColorBuffer[0], NULL, NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
          mIsStereoVisionAllowedWhenCapture = false;
        }
      } else {
        grpsys->RequestReadPixels(NULL, NULL, m_ColorBuffer[0], RenderTarget::RENDERBUFFER_TYPE_COLOR);
      }
    }// 1SYNC目でキャプチャの描画コマンドが開始、確実に終了するのは2SYNC目
    else if (m_NowSync == 1 && (m_Type == TYPE_CAPTURE)) {
      if (m_bApper) {
        _TransTexture(grpsys, DISPLAY_UPPER, &m_Texture[0], m_ColorBuffer[0]);
        _TransTexture(grpsys, DISPLAY_UPPER_RIGHT, &m_Texture[1], m_ColorBuffer[1]);
      } else {
        _TransTexture(grpsys, DISPLAY_LOWER, &m_Texture[0], m_ColorBuffer[0]);
      }
    }
  }

  // 演算処理
  if (m_State == ST_FADE_IN || m_State == ST_FADE_OUT) {
    math::VEC4 color;

    m_NowSync++;

    // 線形補間
    {
      math::VEC4 temp;

      color = m_StartCol;
      temp = m_EndCol - m_StartCol;
      color += temp * m_NowSync / m_FadeSync;

      m_NowCol = color;
    }
  }
}

/**
 * @brief 演算処理≪LayoutAnimation≫
 * @param grpsys グラフィックシステム
 */
void RequestUnit::CalcLayoutAnimation(GraphicsSystem * grpsys)
{
  if (mLayoutWork->pLytWk != NULL) {
    m_bCaptureReq = false;

    // キャプチャ
    // カラーバッファ保存
    if (m_bKeepColorBuffer) {
      if (m_NowSync == 0) {
        m_bCaptureReq = true;
        if (m_bApper) {
          if (nn::gx::IsStereoVisionAllowed()) {
            grpsys->RequestReadPixels(m_ColorBuffer[0], m_ColorBuffer[1], NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
            mIsStereoVisionAllowedWhenCapture = true;
          } else {
            grpsys->RequestReadPixels(m_ColorBuffer[0], NULL, NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
            mIsStereoVisionAllowedWhenCapture = false;
          }
        } else {
          grpsys->RequestReadPixels(NULL, NULL, m_ColorBuffer[0], RenderTarget::RENDERBUFFER_TYPE_COLOR);
        }
      }
    }

    mLayoutWork->pLytWk->Update();
    m_NowSync++;
  }
}

/**
 * @brief 演算処理≪LayoutAnimationOfExternalData≫
 * @param grpsys グラフィックシステム
 */
void RequestUnit::CalcLayoutAnimationOfExternalData(GraphicsSystem * grpsys)
{
  if (mLayoutWorkOfExternalData->pLytWk != NULL) {
    m_bCaptureReq = false;

    // キャプチャ
    // カラーバッファ保存
    if (m_bKeepColorBuffer) {
      if (m_NowSync == 0) {
        m_bCaptureReq = true;
        if (m_bApper) {
          if (nn::gx::IsStereoVisionAllowed()) {
            grpsys->RequestReadPixels(m_ColorBuffer[0], m_ColorBuffer[1], NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
            mIsStereoVisionAllowedWhenCapture = true;
          } else {
            grpsys->RequestReadPixels(m_ColorBuffer[0], NULL, NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
            mIsStereoVisionAllowedWhenCapture = false;
          }
        } else {
          grpsys->RequestReadPixels(NULL, NULL, m_ColorBuffer[0], RenderTarget::RENDERBUFFER_TYPE_COLOR);
        }
      }
    }

    mLayoutWorkOfExternalData->pLytWk->Update();
    m_NowSync++;
  }
}

/**
 * @brief 描画に使用するカラーバッファを取得する
 * @param dispType
 * @return カラーバッファ
 */
void* RequestUnit::GetColorBufferForDraw(DisplayType dispType)
{
  switch (dispType) {
  case DISPLAY_UPPER:
  case DISPLAY_LOWER:
    return m_ColorBuffer[0];
  case DISPLAY_UPPER_RIGHT:
  {
    if (mIsStereoVisionAllowedWhenCapture) {
      return m_ColorBuffer[1];
    } else {
      return m_ColorBuffer[0];
    }
  }
  default:
    GFL_ASSERT(0);
    return NULL;
  }
}

/**
 * @brief 上画面のブロックイメージを表示する
 * @param grpsys グラフィックシステム
 */
void RequestUnit::DrawColorBufferOfDisplayUpper(GraphicsSystem* grpsys)
{
  _DrawColorBuffer(grpsys, DISPLAY_UPPER, GetColorBufferForDraw(DISPLAY_UPPER));
  _DrawColorBuffer(grpsys, DISPLAY_UPPER_RIGHT, GetColorBufferForDraw(DISPLAY_UPPER_RIGHT));
}

/**
 * @brief 下画面のブロックイメージを表示する
 * @param grpsys グラフィックシステム
 */
void RequestUnit::DrawColorBufferOfDisplayLower(GraphicsSystem* grpsys)
{
  _DrawColorBuffer(grpsys, DISPLAY_LOWER, GetColorBufferForDraw(DISPLAY_LOWER));
}

/**
 * @brief
 * @param dispType 対象面
 * @return 
 */
util::Texture* RequestUnit::GetTextureForFillScreen(DisplayType dispType)
{
  switch (dispType) {
  case DISPLAY_UPPER:
  case DISPLAY_LOWER:
    return m_Texture[0];
  case DISPLAY_UPPER_RIGHT:
  {
    if (mIsStereoVisionAllowedWhenCapture) {
      return m_Texture[1];
    } else {
      return m_Texture[0];
    }
  }
  default:
    GFL_ASSERT(0);
    return NULL;
  }
}

/**
 * @brief 上画面を簡易描画で塗りつぶす
 * @param grpsys グラフィックシステム
 * @param isUseTexture テクスチャを使用するかどうか
 */
void RequestUnit::FillScreenOfDisplayUpper(GraphicsSystem* grpsys, bool isUseTexture)
{
  if (isUseTexture) {
    _FillScreen(grpsys, DISPLAY_UPPER, m_NowCol, GetTextureForFillScreen(DISPLAY_UPPER));
    _FillScreen(grpsys, DISPLAY_UPPER_RIGHT, m_NowCol, GetTextureForFillScreen(DISPLAY_UPPER_RIGHT));
  } else {
    _FillScreen(grpsys, DISPLAY_UPPER, m_NowCol);
    _FillScreen(grpsys, DISPLAY_UPPER_RIGHT, m_NowCol);
  }
}

/**
 * @brief 下画面を簡易描画で塗りつぶす
 * @param grpsys グラフィックシステム
 * @param isUseTexture テクスチャを使用するかどうか
 */
void RequestUnit::FillScreenOfDisplayLower(GraphicsSystem* grpsys, bool isUseTexture)
{
  if (isUseTexture) {
    _FillScreen(grpsys, DISPLAY_LOWER, m_NowCol, GetTextureForFillScreen(DISPLAY_LOWER));
  } else {
    _FillScreen(grpsys, DISPLAY_LOWER, m_NowCol);
  }
}

/**
 * @brief 描画
 * @param grpsys グラフィックシステム
 */
void RequestUnit::DrawNormal(GraphicsSystem * grpsys)
{
  if (m_bApper) {
    if (m_bKeepColorBuffer && m_bCaptureReq == false) {
      GFL_ASSERT(m_State != ST_FADE_IN);
      this->DrawColorBufferOfDisplayUpper(grpsys);
    }
    this->FillScreenOfDisplayUpper(grpsys, false);
  } else {
    if (m_bKeepColorBuffer && m_bCaptureReq == false) {
      GFL_ASSERT(m_State != ST_FADE_IN);
      this->DrawColorBufferOfDisplayLower(grpsys);
    }
    this->FillScreenOfDisplayLower(grpsys, false);
  }
}

/**
 * @brief 描画≪キャプチャ≫
 * @param grpsys グラフィックシステム
 */
void RequestUnit::DrawCapture(GraphicsSystem * grpsys)
{
  if (m_State == ST_FADE_OUT) {
    if (m_bKeepColorBuffer && m_bCaptureReq == false) {
      if (m_bApper) {
        this->DrawColorBufferOfDisplayUpper(grpsys);
      } else {
        this->DrawColorBufferOfDisplayLower(grpsys);
      }
    }
  } else {
    if (m_bApper) {
      this->FillScreenOfDisplayUpper(grpsys, true);
    } else {
      this->FillScreenOfDisplayLower(grpsys, true);
    }
  }
}

/**
 * @brief 描画≪カラーキャプチャ≫
 * @param grpsys グラフィックシステム
 */
void RequestUnit::DrawColorCapture(GraphicsSystem * grpsys)
{
  if (m_State == ST_FADE_IN /* && m_NowSync > 2 */) {
    if (m_bApper) {
      this->FillScreenOfDisplayUpper(grpsys, true);
    } else {
      this->FillScreenOfDisplayLower(grpsys, true);
    }
  } else {
    GFL_ASSERT(m_bKeepColorBuffer);
    //    GFL_ASSERT( m_State != ST_FADE_IN );
    if (m_bApper) {
      if (m_bCaptureReq == false) {
        this->DrawColorBufferOfDisplayUpper(grpsys);
        if (m_State != ST_FADE_WAIT) {
          this->FillScreenOfDisplayUpper(grpsys, false);
        }
      }
    } else {
      if (m_bCaptureReq == false) {
        this->DrawColorBufferOfDisplayLower(grpsys);
        if (m_State != ST_FADE_WAIT) {
          this->FillScreenOfDisplayLower(grpsys, false);
        }
      }
    }
  }

}

/**
 * @brief 描画≪レイアウト≫
 * @param grpsys グラフィックシステム
 */
void RequestUnit::DrawLayoutAnimation(GraphicsSystem * grpsys)
{
  if (mLayoutWork->enable == false) {
    return;
  }

  if (m_State == ST_FADE_OUT) {
    if (m_bKeepColorBuffer && m_bCaptureReq == false) {
      if (m_bApper) {
        this->DrawColorBufferOfDisplayUpper(grpsys);
      }
    }
  }

  if (mLayoutWork->setupDisp == Fade::SETUP_UPPER) {
    RenderTarget * upperFrame = grpsys->GetCurrentFrameBuffer(DISPLAY_UPPER);
    RenderTarget * upperRightFrame = grpsys->GetCurrentFrameBuffer(DISPLAY_UPPER_RIGHT);
    mLayoutWork->pLytWk->Draw(grpsys, DISPLAY_UPPER, upperFrame, upperRightFrame);
  }
}

/**
 * @brief 描画≪外部Layoutアニメーション≫
 * @param grpsys グラフィックシステム
 */
void RequestUnit::DrawLayoutAnimationOfExternalData(GraphicsSystem * grpsys)
{
  if (mLayoutWorkOfExternalData->enable == false) {
    return;
  }

  if (m_State == ST_FADE_OUT) {
    if (m_bKeepColorBuffer && m_bCaptureReq == false) {
      if (m_bApper) {
        this->DrawColorBufferOfDisplayUpper(grpsys);
      }
    }
  }

  if (mLayoutWorkOfExternalData->setupDisp == Fade::SETUP_UPPER) {
    RenderTarget * upperFrame = grpsys->GetCurrentFrameBuffer(DISPLAY_UPPER);
    RenderTarget * upperRightFrame = grpsys->GetCurrentFrameBuffer(DISPLAY_UPPER_RIGHT);
    mLayoutWorkOfExternalData->pLytWk->Draw(grpsys, DISPLAY_UPPER, upperFrame, upperRightFrame);
  }
}

/**
 * @brief 終了処理≪通常＆キャプチャ≫
 */
void RequestUnit::EndProcessNormalAndCapture(void)
{
  if (m_NowSync >= m_FadeSync) {
    switch (m_State) {
    case ST_FADE_OUT:
      m_State = ST_FADE_WAIT;
      break;
    case ST_FADE_IN:
      m_State = ST_FADE_NONE;
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
}

/**
 * @brief 終了処理≪カラーキャプチャ≫ 
 */
void RequestUnit::EndProcessColorCapture(GraphicsSystem * grpsys)
{
  if (m_NowSync >= m_FadeSync) {
    switch (m_State) {
    case ST_FADE_OUT:
      // カラーキャプチャ F.I.用に画面をキャプチャ
      if (m_Type == TYPE_COLOR_CAPTURE) {
        if (m_bApper) {
          if (nn::gx::IsStereoVisionAllowed()) {
            grpsys->RequestReadPixels(m_ColorBuffer[0], m_ColorBuffer[1], NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
            mIsStereoVisionAllowedWhenCapture = true;
          } else {
            grpsys->RequestReadPixels(m_ColorBuffer[0], NULL, NULL, RenderTarget::RENDERBUFFER_TYPE_COLOR);
            mIsStereoVisionAllowedWhenCapture = false;
          }
        } else {
          grpsys->RequestReadPixels(NULL, NULL, m_ColorBuffer[0], RenderTarget::RENDERBUFFER_TYPE_COLOR);
        }
      }
      m_State = ST_FADE_WAIT;
      break;
    case ST_FADE_IN:
      m_State = ST_FADE_NONE;
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
}

/**
 * @brief 終了処理≪Layoutアニメーション≫
 */
void RequestUnit::EndProcessLayoutAnimation(void)
{
  if (mLayoutWork != NULL) {
    if (this->IsAnimationEnd(m_currentAnimID) == true) {
      this->StopAnimation(m_currentAnimID);
      switch (m_State) {
      case ST_FADE_OUT:
        m_State = ST_FADE_WAIT;
        break;
      case ST_FADE_IN:
        m_State = ST_FADE_NONE;
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
    }
  }
}

/**
 * @brief 終了処理≪外部Layoutアニメーション≫
 */
void RequestUnit::EndProcessLayoutAnimationOfExternalData(void)
{
  if (mLayoutWorkOfExternalData != NULL) {
    if (this->IsAnimationEndOfExternalData() == true) {
      this->StopAnimationOfExternalData();
      switch (m_State) {
      case ST_FADE_OUT:
        m_State = ST_FADE_WAIT;
        break;
      case ST_FADE_IN:
        m_State = ST_FADE_NONE;
        break;
      default:
        GFL_ASSERT(0);
        break;
      }
    }
  }
}

/**
 * @brief フェードが終了したかどうかを取得
 * @return フェードが終了したかどうか
 */
bool RequestUnit::IsEnd(void)
{
  return ( m_State != ST_FADE_OUT && m_State != ST_FADE_IN);
}

/**
 * @brief リセット
 */
void RequestUnit::Reset(void)
{
  m_bPause = false;
  m_State = ST_FADE_NONE;
  m_NowSync = 0;
}

/**
 * @brief 全てのレイアウトアニメをリセットする 
 */
void RequestUnit::ResetLayoutAnimation(void)
{
  if (mLayoutWork != NULL) {
    // 全てのアニメをフレーム0で固定する
    for (u8 i = Fade::ANMID_UPPER_WC_IN; i < Fade::ANMID_UPPER_NUM; ++i) {
      this->PauseAnimation(i, 0, false);
    }
  }
}

/**
 * @brief アニメーションを再生する 
 * @param animation_index　再生するアニメーション番号
 */
void RequestUnit::StartAnimation(u32 animation_index)
{
  if (mLayoutWork->pLytWk->IsBindAnime(animation_index) == false) {
    mLayoutWork->pLytWk->BindAnime(animation_index);
  }
  mLayoutWork->pLytWk->SetAutoAnime(animation_index, true);
  mLayoutWork->pLytWk->SetAutoAnimeSpeed(1.5f);
  mLayoutWork->pLytWk->SetAnimeFrame(animation_index, 0);
}

/**
 * @brief アニメーションを一時停止する
 * @param animation_index 一時停止するアニメーション番号
 * @param pause trueなら再開, falseなら停止
 */
void RequestUnit::PauseAnimation(u32 animation_index, u32 setFrame, bool pause)
{
  if (mLayoutWork->pLytWk->IsBindAnime(animation_index) == false) {
    mLayoutWork->pLytWk->BindAnime(animation_index);
  }
  mLayoutWork->pLytWk->SetAutoAnime(animation_index, pause);
  mLayoutWork->pLytWk->SetAnimeFrame(animation_index, 0);
}

/**
 * @brief アニメーションを停止する
 * @param animation_index  停止するアニメーション番号
 */
void RequestUnit::StopAnimation(u32 animation_index)
{
  if (mLayoutWork->pLytWk->IsBindAnime(animation_index)) {
    mLayoutWork->pLytWk->UnBindAnime(animation_index);
  }
}

/**
 * @brief アニメーションが終了したかどうかを調べる
 * @param animation_index 対象のアニメーション番号
 * @return アニメーションが終了したかどうか
 */
bool RequestUnit::IsAnimationEnd(u32 animation_index) const
{
  // return !( 終了していない条件 )
  return !(
    mLayoutWork->pLytWk->IsAnime(animation_index) &&
    mLayoutWork->pLytWk->IsBindAnime(animation_index) &&
    !mLayoutWork->pLytWk->IsAnimeEnd(animation_index));
}

/**
 * @brief 外部データのアニメーションを再生する 
 * @param animation_index 再生するアニメーション番号
 */
void RequestUnit::StartAnimationOfExternalData(void)
{
  if (mLayoutWorkOfExternalData->pLytWk->IsBindAnime(0) == false) {
    mLayoutWorkOfExternalData->pLytWk->BindAnime(0);
  }
  mLayoutWorkOfExternalData->pLytWk->SetAutoAnime(0, true);
  mLayoutWorkOfExternalData->pLytWk->SetAnimeFrame(0, 0);
}

/**
 * @brief 外部データのアニメーションを停止する
 * @param animation_index 停止するアニメーション番号
 */
void RequestUnit::StopAnimationOfExternalData(void)
{
  if (mLayoutWorkOfExternalData->pLytWk->IsBindAnime(0)) {
    mLayoutWorkOfExternalData->pLytWk->UnBindAnime(0);
  }
}

/**
 * @brief 外部データのアニメーションが終了したかどうかを取得
 * @return 外部データのアニメーションが終了したかどうか
 */
bool RequestUnit::IsAnimationEndOfExternalData(void) const
{
  return !(
    mLayoutWorkOfExternalData->pLytWk->IsAnime(0) &&
    mLayoutWorkOfExternalData->pLytWk->IsBindAnime(0) &&
    !mLayoutWorkOfExternalData->pLytWk->IsAnimeEnd(0));
}

/**
 * @brief フェード
 */
const g2d::LytWkSetUp Fade::LytWkDefaultSetup[] = {
  { // 上画面
    gfl::math::VEC3(0, 0, 0), // pos
    gfl::math::VEC2(1, 1), // scale
    gfl::math::VEC3(0, 0, 0), // rotate
    1, // priority
    0, // groupID
  },
  { // 下画面
    gfl::math::VEC3(0, -290, 0), // pos
    gfl::math::VEC2(1, 1), // scale
    gfl::math::VEC3(0, 0, 0), // rotate
    1, // priority
    0, // groupID
  }
};

/**
 * @brief コンストラクタ
 * @param heap_allocator ヒープアロケータ
 * @param device_allocator デバイスヒープアロケータ
 * @param grpsys グラフィックシステム
 * @param lyt_res レイアウトリソース
 */
Fade::Fade(heap::NwAllocator* heap_allocator, heap::NwAllocator* device_allocator, GraphicsSystem* grpsys, const void* lyt_res)
{
  m_HeapAllocator = heap_allocator;
  m_DeviceAllocator = device_allocator;
  m_GraphicsSystem = grpsys;
  mLayoutSystem = NULL;

  this->Create2D(lyt_res);
  mIsCreate2DOfExternalData = false;

  m_RequestUnit[ DISP_APPER ] =
    GFL_NEW(m_DeviceAllocator->GetHeapBase()) RequestUnit(heap_allocator, device_allocator, true, grpsys, &mLayoutWork[SETUP_UPPER]);
  m_RequestUnit[ DISP_LOWER ] =
    GFL_NEW(m_DeviceAllocator->GetHeapBase()) RequestUnit(heap_allocator, device_allocator, false, grpsys, NULL);

  m_pCallbackFunc = NULL;
  m_pCallbackWork = NULL;
}

/**
 *@brief  デストラクタ
 */
Fade::~Fade(void)
{
  for (int i = 0; i < DISP_MAX; i++) {
    GFL_DELETE m_RequestUnit[i];
  }

  this->DeleteLayoutResource();
  this->DeleteLayoutWork();
  //this->DeleteLayoutEnv( );
  this->DeleteLayoutSystem();
}

/**
 * @brief レイアウトリソース削除
 */
void Fade::DeleteLayoutResource(void)
{
  if (mLayoutResID != g2d::LYTSYS_RESID_ERROR) {
    mLayoutSystem->ReleaseLytRes(mLayoutResID);
  }
}

/**
 * @brief 外部データ用のレイアウトリソース削除
 */
void Fade::DeleteLayoutResourceOfExternalData(void)
{
  if (mLayoutResIDOfExternalData != g2d::LYTSYS_RESID_ERROR) {
    mLayoutSystem->ReleaseLytRes(mLayoutResIDOfExternalData);
  }
}

/**
 * @brief レイアウトワーク削除
 */
void Fade::DeleteLayoutWork(void)
{
  for (u32 i = 0; i < mLwkMax; i++) {
    if (mLayoutWork[i].pLytWk != NULL) {
      mLayoutSystem->RemoveLytWk(mLayoutWork[i].pLytWk);
    }
  }
  GFL_SAFE_DELETE_ARRAY(mLayoutWork);
}

/**
 * @brief 外部データ用のレイアウトワーク削除
 */
void Fade::DeleteLayoutWorkOfExternalData(void)
{
  if (mLayoutWorkOfExternalData != NULL) {
    for (u32 i = 0; i < mLwkMaxOfExternalData; i++) {
      if (mLayoutWorkOfExternalData[i].pLytWk != NULL) {
        mLayoutSystem->RemoveLytWk(mLayoutWorkOfExternalData[i].pLytWk);
      }
    }
    GFL_SAFE_DELETE_ARRAY(mLayoutWorkOfExternalData);
  }
}

/**
 * @brief レイアウト環境削除
 */
void Fade::DeleteLayoutEnv(void)
{
  if (mUpperLayoutEnv != NULL) {
    GFL_DELETE mUpperLayoutEnv;
    mUpperLayoutEnv = NULL;
  }
  if (mLowerLayoutEnv != NULL) {
    GFL_DELETE mLowerLayoutEnv;
    mLowerLayoutEnv = NULL;
  }
}

/**
 * @brief レイアウトシステム削除
 */
void Fade::DeleteLayoutSystem(void)
{
  if (mLayoutSystem != NULL) {
    GFL_DELETE mLayoutSystem;
    mLayoutSystem = NULL;
  }
}

/**
 * @brief コールバックのセット 
 * @memo レイアウトシステムを削除
 * @param pCallbackFunc コールバック関数
 * @param pWork コールバックワーク
 */
void Fade::SetCallback(FadeCallbackFunc* pCallbackFunc, void* pWork)
{
  m_pCallbackFunc = pCallbackFunc;
  m_pCallbackWork = pWork;
}

/**
 * @brief F.I.待機状態かどうかを取得
 * @param 対象面
 */
bool Fade::IsFadeInStandbyState(DISP disp)
{
  bool flag = false;

  if (disp == DISP_DOUBLE) {
    if (m_RequestUnit[DISP_APPER]->GetState() == RequestUnit::ST_FADE_WAIT &&
      m_RequestUnit[DISP_LOWER]->GetState() == RequestUnit::ST_FADE_WAIT) {
      flag = true;
    }
  } else {
    flag = (m_RequestUnit[disp]->GetState() == RequestUnit::ST_FADE_WAIT);
  }

  return flag;
}

/**
 *@brief  主処理
 */
void Fade::Update(void)
{
  for (int i = 0; i < DISP_MAX; i++) {
    m_RequestUnit[ i ]->Update(m_GraphicsSystem);
  }

  if (IsEnd()) {
    if (m_pCallbackFunc != NULL) {
      m_pCallbackFunc = NULL;
    }
    if (m_pCallbackWork != NULL) {
      m_pCallbackWork = NULL;
    }

    if (m_RequestUnit[ DISP_APPER ]->GetState() == RequestUnit::ST_FADE_NONE) {
      // 外部レイアウトデータ用のワークとリソースを破棄
      if (mIsCreate2DOfExternalData) {
        this->DeleteLayoutResourceOfExternalData();
        this->DeleteLayoutWorkOfExternalData();
        m_RequestUnit[ DISP_APPER ]->SetLayoutWorkOfExternalData(NULL);
        mIsCreate2DOfExternalData = false;
      }
    }
  } else {
    if (m_pCallbackFunc != NULL) {
      bool end;
      CALLBACK_DATA data;
      data.ExternalLytWork = mLayoutWorkOfExternalData;
      data.CallBackWork = m_pCallbackWork;
      end = m_pCallbackFunc(&data);
      if (end) {
        m_pCallbackFunc = NULL;
        m_pCallbackWork = NULL;
      }
    }
  }
}

/**
 * @brief F.O.リクエスト
 * @param disp 対象面
 * @param start_col スタート時の画面色(0~1.0f) NULLだと前回の色からスタート
 * @param end_col 終了時の画面色(0~1.0f)
 * @param sync F.O.に費やす時間
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade::RequestOut(DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync, bool is_keep_buffer)
{
  GFL_ASSERT(end_col);

  if (start_col) {
    GFL_ASSERT(start_col->x >= 0 && start_col->x <= 1.0f);
    GFL_ASSERT(start_col->y >= 0 && start_col->y <= 1.0f);
    GFL_ASSERT(start_col->z >= 0 && start_col->z <= 1.0f);
    GFL_ASSERT(start_col->w >= 0 && start_col->w <= 1.0f);
  }

  GFL_ASSERT(end_col->x >= 0 && end_col->x <= 1.0f);
  GFL_ASSERT(end_col->y >= 0 && end_col->y <= 1.0f);
  GFL_ASSERT(end_col->z >= 0 && end_col->z <= 1.0f);
  GFL_ASSERT(end_col->w >= 0 && end_col->w <= 1.0f);

  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_APPER ]->SetupOut(start_col, end_col, sync, is_keep_buffer, RequestUnit::TYPE_NORMAL);
  }

  if (disp == DISP_LOWER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_LOWER ]->SetupOut(start_col, end_col, sync, is_keep_buffer, RequestUnit::TYPE_NORMAL);
  }
}

/**
 * @brief キャプチャF.O.リクエスト≪Colorキャプチャタイプ≫ 
 * @param disp 対象面
 * @param start_col スタート時の画面色
 * @param end_col 終了時の画面色
 * @param sync キャプチャF.O.に費やす時間
 */
void Fade::RequestOutColorCapture(DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync)
{
  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_APPER ]->SetupOut(start_col, end_col, sync, true, RequestUnit::TYPE_COLOR_CAPTURE);
  }

  if (disp == DISP_LOWER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_LOWER ]->SetupOut(start_col, end_col, sync, true, RequestUnit::TYPE_COLOR_CAPTURE);
  }
}

/**
 * @brief F.O.リクエスト≪キャプチャタイプ≫
 * @param disp 対象面
 */
void Fade::RequestOutCapture(DISP disp)
{
  const math::VEC4 _start(1.0f, 1.0f, 1.0f, 0.0f);
  const math::VEC4 _end(1.0f, 1.0f, 1.0f, 1.0f);

  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_APPER ]->SetupOut(&_start, &_end, SYNC_CAPTURE_OUT, true, RequestUnit::TYPE_CAPTURE);
  }

  if (disp == DISP_LOWER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_LOWER ]->SetupOut(&_start, &_end, SYNC_CAPTURE_OUT, true, RequestUnit::TYPE_CAPTURE);
  }
}

/**
 * @brief F.O.リクエスト≪Layoutアニメーションタイプ≫ 
 * @param disp 対象面
 * @param type レイアウトタイプ
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade::RequestOutLayoutAnimation(DISP disp, OUTTYPE type, bool is_keep_buffer)
{
  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    switch (type) {
    case OUTTYPE_CIRCLE_SHRINK: ///< 円形収縮
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimation(is_keep_buffer, RequestUnit::TYPE_CIRCLE_SHRINK);
      break;
    case OUTTYPE_BALL: ///< ボール
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimation(is_keep_buffer, RequestUnit::TYPE_BALL);
      break;
    case OUTTYPE_UP: ///< 上
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimation(is_keep_buffer, RequestUnit::TYPE_UP);
      break;
    case OUTTYPE_DOWN: ///< 下
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimation(is_keep_buffer, RequestUnit::TYPE_DOWN);
      break;
    case OUTTYPE_LEFT: ///< 左
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimation(is_keep_buffer, RequestUnit::TYPE_LEFT);
      break;
    case OUTTYPE_RIGHT: ///< 右
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimation(is_keep_buffer, RequestUnit::TYPE_RIGHT);
      break;
    case OUTTYPE_DIAMOND: ///< 菱形
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimation(is_keep_buffer, RequestUnit::TYPE_DIAMOND);
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
}

/**
 * @brief F.O.リクエスト≪外部Layoutアニメーションタイプ≫ 
 * @param disp 対象面
 * @param type レイアウトタイプ
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade::RequestOutLayoutAnimationOfExternalData(DISP disp, OUTTYPE type, bool is_keep_buffer)
{
  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    if (type == OUTTYPE_EXTERNAL_LAYOUT_DATA) {
      m_RequestUnit[ DISP_APPER ]->SetupOutLayoutAnimationOfExternalData(is_keep_buffer, RequestUnit::TYPE_EXTERNAL_LAYOUT_DATA);
      m_RequestUnit[ DISP_APPER ]->SetLayoutWorkOfExternalData(mLayoutWorkOfExternalData);
      m_RequestUnit[ DISP_APPER ]->StartAnimationOfExternalData();
    }
  }
}

/**
 * @brief F.O.リクエスト
 * @param disp 対象面
 * @param type F.O.タイプ
 * @param sync F.O.に費やす時間
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade::RequestOut(DISP disp, OUTTYPE type, u32 sync, bool is_keep_buffer)
{
  // アニメリセット
  if (disp == DISP_APPER) {
    m_RequestUnit[disp]->ResetLayoutAnimation();
  }

  const math::VEC4 _black(0.0f, 0.0f, 0.0f, 1.0f);
  const math::VEC4 _white(1.0f, 1.0f, 1.0f, 1.0f);
  const math::VEC4 _black_half(0.0f, 0.0f, 0.0f, 0.5f);
  const math::VEC4 _white_half(1.0f, 1.0f, 1.0f, 0.5f);
  const math::VEC4 _black_none(0, 0, 0, 0);
  const math::VEC4 _white_none(1.0f, 1.0f, 1.0f, 0);

  switch (type) {
  case OUTTYPE_BLACK:
    this->RequestOut(disp, &_black_none, &_black, sync, is_keep_buffer);
    break;
  case OUTTYPE_WHITE:
    this->RequestOut(disp, &_white_none, &_white, sync, is_keep_buffer);
    break;
  case OUTTYPE_CAPTURE:
    this->RequestOutCapture(disp);
    break;
  case OUTTYPE_BLACK_CAPTURE:
    this->RequestOutColorCapture(disp, &_black_none, &_black_half, sync);
    break;
  case OUTTYPE_WHITE_CAPTURE:
    this->RequestOutColorCapture(disp, &_white_none, &_white_half, sync);
    break;
  case OUTTYPE_CIRCLE_SHRINK:
  case OUTTYPE_BALL:
  case OUTTYPE_UP:
  case OUTTYPE_DOWN:
  case OUTTYPE_LEFT:
  case OUTTYPE_RIGHT:
  case OUTTYPE_DIAMOND:
    if (mIsCreate2D == true) {
      this->RequestOutLayoutAnimation(disp, type, is_keep_buffer);
    }
    break;
  default:
    GFL_ASSERT(0);
    break;
  }
  m_pCallbackFunc = NULL;
  m_pCallbackWork = NULL;
}

/**
 * @brief F.O.リクエスト≪外部Layoutアニメーション版≫
 * @param lyt_res 外部レイアウトリソース
 * @param anmNum アニメーション数
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade::RequestOut(const void* lyt_res, u8 anmNum, bool is_keep_buffer)
{
  this->CreateLayoutWorkOfExternalData(); // 上画面分のみ生成
  this->CreateLayoutResourceOfExternalData();
  this->LoadLayoutResourceOfExternalData(lyt_res);
  this->TransferLayoutTextureOfExternalData();
  this->SetLayoutWorkOfExternalData(anmNum);
  this->RequestOutLayoutAnimationOfExternalData(DISP_APPER, OUTTYPE_EXTERNAL_LAYOUT_DATA, is_keep_buffer);
  mIsCreate2DOfExternalData = true;
  m_pCallbackFunc = NULL;
  m_pCallbackWork = NULL;
}

/**
 * @brief F.I.リクエスト
 * @param disp 対象面
 * @param sync F.I.に費やす時間
 */
void Fade::RequestIn(DISP disp, u32 sync)
{
  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_APPER ]->SetupIn(sync);
  }

  if (disp == DISP_LOWER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_LOWER ]->SetupIn(sync);
  }
  m_pCallbackFunc = NULL;
  m_pCallbackWork = NULL;
}

/**
 * @brief F.I.リクエスト≪タイプ指定版≫
 * @param disp 対象面
 * @param type F.I.タイプ
 * @param sync F.I.に費やす時間
 */
void Fade::RequestIn_Ex(DISP disp, INTYPE type, u32 sync)
{
  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    switch (type) {
    case INTYPE_BLACK: ///< 黒
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_BLACK);
      break;
    case INTYPE_WHITE: ///< 白
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_WHITE);
      break;
    case INTYPE_CIRCLE_SHRINK: ///< 円形収縮
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_CIRCLE_SHRINK);
      break;
    case INTYPE_BALL: ///< ボール
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_BALL);
      break;
    case INTYPE_UP: ///< 上
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_UP);
      break;
    case INTYPE_DOWN: ///< 下
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_DOWN);
      break;
    case INTYPE_LEFT: ///< 左
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_LEFT);
      break;
    case INTYPE_RIGHT: ///< 右
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_RIGHT);
      break;
    case INTYPE_DIAMOND: ///< 菱形
      m_RequestUnit[ DISP_APPER ]->SetupInEx(sync, RequestUnit::TYPE_DIAMOND);
      break;
    default:
      GFL_ASSERT(0);
      break;
    }
  }
  m_pCallbackFunc = NULL;
  m_pCallbackWork = NULL;
}

/**
 * @brief 強制的にF.O.状態にする
 * @param disp 対象面
 * @param col 終了時の画面色 
 */
void Fade::ForceOut(DISP disp, const math::VEC4 * col)
{
  math::VEC4 _col_none = *col;
  _col_none.w = 0;

  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_APPER ]->SetupOut(&_col_none, col, 1, false, RequestUnit::TYPE_NORMAL, true);
  }
  if (disp == DISP_LOWER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_LOWER ]->SetupOut(&_col_none, col, 1, false, RequestUnit::TYPE_NORMAL, true);
  }
  m_pCallbackFunc = NULL;
  m_pCallbackWork = NULL;
}

/**
 * @brief フェードが終了したかどうかを取得
 * @return フェードが終了したかどうか 
 */
bool Fade::IsEnd(void)
{
  bool b_end = true;

  for (int i = 0; i < DISP_MAX; i++) {
    if (m_RequestUnit[i]->IsEnd() == false) {
      b_end = false;
      break;
    }
  }

  return b_end;
}

/**
 * @brief ポーズON/OFF
 * @param is_pause ポーズフラグ
 */
void Fade::SetPause(bool is_pause)
{
  for (int i = 0; i < DISP_MAX; i++) {
    m_RequestUnit[ i ]->SetPause(is_pause);
  }
}

/**
 * @brief ポーズON/OFF
 * @param is_pause ポーズフラグ
 */
bool Fade::IsPause(void) const
{
  // ポーズフラグは全て同時にかかるはず
  GFL_ASSERT(m_RequestUnit[0]->IsPause() == m_RequestUnit[1]->IsPause());

  return m_RequestUnit[0]->IsPause();
}

/**
 * @brief フェード状態リセット
 * @param disp 対象面
 */
void Fade::Reset(DISP disp)
{
  if (disp == DISP_APPER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_APPER ]->Reset();
  }

  if (disp == DISP_LOWER || disp == DISP_DOUBLE) {
    m_RequestUnit[ DISP_LOWER ]->Reset();
  }
}

/**
 * @brief フェード処理カットフラグをセット 
 * @param disp 対象面
 * @param isCutProcess カットするかどうか
 */
void Fade::SetIsCutProcess(DISP disp, bool isCutProcess)
{
  m_RequestUnit[disp]->SetIsCutProcess(isCutProcess);
}

/**
 * @brief フェード用のカラーバッファを取得
 * @param type
 * @param id
 * @return カラーバッファ
 */
void* Fade::GetColorBuffer(DisplayType type, int id)
{
  if (type == DISPLAY_UPPER) {
    return m_RequestUnit[ DISP_APPER ]->GetColorBuffer(id);
  } else {
    return m_RequestUnit[ DISP_LOWER ]->GetColorBuffer(id);
  }
}

/**
 * @brief フェード用のテクスチャワークを取得
 * @param type
 * @param id
 * @return テクスチャワーク
 */
util::Texture * Fade::GetTextureWork(DisplayType type, int id)
{
  if (type == DISPLAY_UPPER) {
    return m_RequestUnit[ DISP_APPER ]->GetTextureWork(id);
  } else {
    return m_RequestUnit[ DISP_LOWER ]->GetTextureWork(id);
  }
}

/**
 * @brief 2D関連を生成
 * @param lyt_res レイアウトリソース
 */
void Fade::Create2D(const void* lyt_res)
{
  // アーカイブIDテーブル
  static const LYT_RES_DATA resTbl = {
    0, 0, 0, // リソース
  };
  // アニメデータ
  static const g2d::LytArcIndex UpperAnmIndex[] = {
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_FADE_01_OUT_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_FADE_01_IN_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WC_IN_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WC_OUT_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WB_IN_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WB_OUT_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_IN_UE_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_OUT_UE_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_IN_SHITA_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_OUT_SHITA_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_IN_HIDARI_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_OUT_HIDARI_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_IN_MIGI_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WS_OUT_MIGI_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WR_IN_BCLAN,
    LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_WR_OUT_BCLAN,
  };
  // レイアウトデータ
  static const LYTWK_DATA layoutData[] = {
    { // 上画面
      0,
      LYTRES_FADE_BG_UPPER_FADE_BG_UPPER_BCLYT, // resID内のレイアウトデータID
      UpperAnmIndex, // アニメーションデータIDテーブル アニメのない場合はNULL
      GFL_NELEMS(UpperAnmIndex), // アニメーションデータIDテーブル数 アニメのない場合は0
      GFL_NELEMS(UpperAnmIndex), // 登録アニメーション最大数
      false, // 非表示ペインに行列計算を行うか
      SETUP_UPPER, // ディスプレイ
      true, // 表示設定
    },
  };

  this->CreateLayoutSystem();
  //this->CreateLayoutEnv( );
  this->CreateLayoutWork(GFL_NELEMS(layoutData));
  this->CreateLayoutResource();
  this->LoadLayoutResource(lyt_res, &resTbl, true);
  this->TransferLayoutTexture();
  this->SetLayoutWork(layoutData, 0, GFL_NELEMS(layoutData));
}

/**
 * @brief レイアウトシステム作成
 */
void Fade::CreateLayoutSystem(void)
{
  g2d::LytSysSetUp setup = {
    128, // 管理リソース最大数
    2 // 管理レイアウトワーク最大数
  };
  mLayoutSystem = GFL_NEW(m_HeapAllocator->GetHeapBase()) g2d::LytSys(&setup, m_HeapAllocator, m_DeviceAllocator);
}

/**
 * @brief レイアウト環境作成
 */
void Fade::CreateLayoutEnv(void)
{
  // 射影設定（正射影）
  const ProjectionParam projection[] = {
    { // 上画面
      PROJECTION_ORTHO,
      { -200.0f, 200.0f, -120.0f, 120.0f},
      0.1f,
      1000.0f,
      math::PIVOT_UPSIDE_TO_TOP,
    },
    { // 下画面
      PROJECTION_ORTHO,
      { -160.0f, 160.0f, -120.0f, 120.0f},
      0.1f,
      1000.0f,
      math::PIVOT_UPSIDE_TO_TOP,
    }
  };

  // ビュー設定
  const ViewParam view[] = {
    { // 上画面
      math::VEC3(0, 0, 1), // カメラ座標
      math::VEC3(0, 1, 0), // 上方向
      math::VEC3(0, 0, 0), // ターゲット座標
    },
    { // 下画面
      math::VEC3(0, -290, 1), // カメラ座標
      math::VEC3(0, 1, 0), // 上方向
      math::VEC3(0, -290, 0), // ターゲット座標
    },
  };

  // 上画面
  mUpperLayoutEnv = GFL_NEW(m_HeapAllocator->GetHeapBase()) g2d::Env();
  // 下画面
  mLowerLayoutEnv = GFL_NEW(m_HeapAllocator->GetHeapBase()) g2d::Env();
  mLowerLayoutEnv->SetCameraParam(g2d::Env::CAMERA_NORMAL, projection[1], view[1]);
}

/**
 * @brief レイアウトワーク作成
 * @param lytMax 作成数
 */
void Fade::CreateLayoutWork(u32 lytMax)
{
  mLayoutWork = GFL_NEW_ARRAY(m_HeapAllocator->GetHeapBase()) LAYOUT_WORK[lytMax];
  mLwkMax = lytMax;

  for (int i = 0; i < mLwkMax; i++) {
    mLayoutWork[i].pLytWk = NULL;
  }
}

/**
 * @brief 外部データ用のレイアウトワーク作成
 * @param lytMax 作成数
 */
void Fade::CreateLayoutWorkOfExternalData(u32 lytMax)
{
  mLayoutWorkOfExternalData = GFL_NEW_ARRAY(m_HeapAllocator->GetHeapBase()) LAYOUT_WORK[lytMax];
  mLwkMaxOfExternalData = lytMax;

  for (int i = 0; i < mLwkMaxOfExternalData; i++) {
    mLayoutWorkOfExternalData[i].pLytWk = NULL;
  }
}

/**
 * @brief レイアウトリソース作成
 */
void Fade::CreateLayoutResource(void)
{
  // エラーコードで初期化
  mLayoutResID = g2d::LYTSYS_RESID_ERROR;
}

/**
 * @brief 外部データ用のレイアウトリソース作成
 */
void Fade::CreateLayoutResourceOfExternalData(void)
{
  // エラーコードで初期化
  mLayoutResIDOfExternalData = g2d::LYTSYS_RESID_ERROR;
}

/**
 * @brief レイアウトリソース読み込み 
 * @param lyt_res レイアウトリソース
 * @param resTbl レイアウトリソースデータ
 * @param comp 圧縮フラグ
 */
void Fade::LoadLayoutResource(const void* lyt_res, const LYT_RES_DATA* resTbl, bool comp)
{
  mIsCreate2D = true;
  if (lyt_res == NULL) {
    gfl::fs::ArcFile* af = GFL_NEW_LOW(m_HeapAllocator->GetHeapBase()) gfl::fs::ArcFile(m_HeapAllocator->GetHeapBase()->GetLowerHandle(), L"rom:/gfl_grp/fade.garc", gfl::fs::ArcFile::OPEN);
    mIsCreate2D = af->IsOpen();
    if (mIsCreate2D == false) {
      GFL_PRINT("fade.garcのオープンに失敗しました\n");
    } else {
      if (comp == true) {
        mLayoutResID = mLayoutSystem->LoadLytResComp(m_DeviceAllocator, af, resTbl->arcDatID, true, resTbl->addTexNum);
      } else {
        mLayoutResID = mLayoutSystem->LoadLytRes(m_DeviceAllocator, af, resTbl->arcDatID, true, resTbl->addTexNum);
      }
    }
    GFL_DELETE af;
  } else {
    mLayoutResID = mLayoutSystem->LoadLytRes(m_DeviceAllocator, lyt_res, true, 0);
  }
}

/**
 * @brief 外部データ用のレイアウトリソース読み込み
 * @param lyt_res レイアウトリソース
 */
void Fade::LoadLayoutResourceOfExternalData(const void* lyt_res)
{
  GFL_ASSERT(lyt_res != NULL);
  mLayoutResIDOfExternalData = mLayoutSystem->LoadLytRes(m_DeviceAllocator, lyt_res, true, 0);
}

/**
 * @brief テクスチャ転送
 * @param area  転送エリア
 */
void Fade::TransferLayoutTexture(MemoryArea area)
{
  mLayoutSystem->TransferAllTexture(mLayoutResID, area);
}

/**
 * @brief 外部データのテクスチャ転送
 * @param area  転送エリア
 */
void Fade::TransferLayoutTextureOfExternalData(MemoryArea area)
{
  mLayoutSystem->TransferAllTexture(mLayoutResIDOfExternalData, area);
}

/**
 * @brief レイアウトワークをセットする 
 * @param layoutData レイアウトワークデータ
 * @param id セットするレイアウトのID
 * @param num セットするデータ数(デフォルト=1)
 */
void Fade::SetLayoutWork(const LYTWK_DATA* layoutData, u32 id, u32 num)
{
  for (int i = id; i < id + num; i++) {
    mLayoutWork[i].pLytWk = mLayoutSystem->CreateLytWk(
      m_HeapAllocator,
      m_DeviceAllocator,
      mLayoutResID,
      layoutData[i - id].layoutDataID,
      layoutData[i - id].anmDataIDTbl,
      layoutData[i - id].anmTblNum,
      layoutData[i - id].anmMax,
      &LytWkDefaultSetup[ layoutData[i - id].setupDisp ],
      false);

    //mLayoutWork[i].pLytWk->SetUserEnv( mUpperLayoutEnv, mLowerLayoutEnv ); // ワークに関連付け
    mLayoutWork[i].pLytWk->SetAutoDrawFlag(false); // 個別表示へ

    mLayoutWork[i].pLytWk->SetInvisiblePaneCalculateMtx(layoutData[i - id].calculateMtxFlag);

    mLayoutWork[i].setupDisp = layoutData[i - id].setupDisp;
    mLayoutWork[i].enable = layoutData[i - id].enable;
  }
}

/**
 * @brief 外部データ用のレイアウトワークをセット
 * @param anmNum アニメの数
 */
void Fade::SetLayoutWorkOfExternalData(u8 anmNum)
{
  // アニメデータ
  static const g2d::LytArcIndex UpperAnmIndex[1] = {0};
  // レイアウトデータ
  static const LYTWK_DATA layoutData[] = {
    { // 上画面
      0,
      anmNum, // resID内のレイアウトデータID
      UpperAnmIndex, // アニメーションデータIDテーブル アニメのない場合はNULL
      anmNum, // アニメーションデータIDテーブル数 アニメのない場合は0
      anmNum, // 登録アニメーション最大数
      false, // 非表示ペインに行列計算を行うか
      SETUP_UPPER, // ディスプレイ
      true, // 表示設定
    },
  };

  for (int i = 0; i < GFL_NELEMS(UpperAnmIndex); i++) {
    mLayoutWorkOfExternalData[i].pLytWk = mLayoutSystem->CreateLytWk(
      m_HeapAllocator,
      m_DeviceAllocator,
      mLayoutResIDOfExternalData,
      layoutData[i].layoutDataID,
      layoutData[i].anmDataIDTbl,
      layoutData[i].anmTblNum,
      layoutData[i].anmMax,
      &LytWkDefaultSetup[ layoutData[i].setupDisp ],
      false);

    //mLayoutWorkOfExternalData[i].pLytWk->SetUserEnv( mUpperLayoutEnv, mLowerLayoutEnv ); // ワークに関連付け
    mLayoutWorkOfExternalData[i].pLytWk->SetAutoDrawFlag(false); // 個別表示へ

    mLayoutWorkOfExternalData[i].pLytWk->SetInvisiblePaneCalculateMtx(layoutData[i].calculateMtxFlag);

    mLayoutWorkOfExternalData[i].setupDisp = layoutData[i].setupDisp;
    mLayoutWorkOfExternalData[i].enable = layoutData[i].enable;
  }
}

/**
 * @brief 主処理
 * @note kujiraではシステムでコールするので、アプリ単位でコールする必要はない 
 */
void Fade_Update(void)
{
  GFL_SINGLETON_INSTANCE(Fade)->Update();
}

/**
 * @brief F.O.リクエスト 
 * @param disp 対象面
 * @param start_col スタート時の画面色
 * @param end_col 終了時の画面色
 * @param sync F.O.に費やす時間
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade_RequestOut(Fade::DISP disp, const math::VEC4* start_col, const math::VEC4* end_col, u32 sync, bool is_keep_buffer)
{
  GFL_SINGLETON_INSTANCE(Fade)->RequestOut(disp, start_col, end_col, sync, is_keep_buffer);
}

/**
 * @brief F.O.リクエスト≪タイプ指定版≫
 * @param disp 対象面
 * @param type F.O.タイプ
 * @param sync F.O.に費やす時間
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade_RequestOut(Fade::DISP disp, Fade::OUTTYPE type, u32 sync, bool is_keep_buffer)
{
  GFL_SINGLETON_INSTANCE(Fade)->RequestOut(disp, type, sync, is_keep_buffer);
}

/**
 * @brief F.O.リクエスト≪外部Layoutアニメーション版≫
 * @param lyt_res 外部レイアウトリソース
 * @param anmNum アニメーション数
 * @param is_keep_buffer 開始時のバッファを保存するかどうか
 */
void Fade_RequestOut(const void* lyt_res, u8 anmNum, bool is_keep_buffer)
{
  GFL_SINGLETON_INSTANCE(Fade)->RequestOut(lyt_res, anmNum, is_keep_buffer);
}

/**
 * @brief F.I.リクエスト
 * @param disp 対象面
 * @param sync F.I.に費やす時間
 */
void Fade_RequestIn(Fade::DISP disp, u32 sync)
{
  GFL_SINGLETON_INSTANCE(Fade)->RequestIn(disp, sync);
}

/**
 * @brief F.I.リクエスト≪タイプ指定版≫
 * @param disp 対象面
 * @param type F.I.タイプ
 * @param sync F.I.に費やす時間
 */
void Fade_RequestIn_Ex(Fade::DISP disp, Fade::INTYPE type, u32 sync)
{
  GFL_SINGLETON_INSTANCE(Fade)->RequestIn_Ex(disp, type, sync);
}

/**
 * @brief 強制的にF.O.状態にする
 * @param disp 対象面
 * @param col 終了時の画面色 
 */
void Fade_ForceOut(Fade::DISP disp, const math::VEC4 * col)
{
  GFL_SINGLETON_INSTANCE(Fade)->ForceOut(disp, col);
}

/**
 * @brief フェードが終了したかどうかを取得
 * @return フェードが終了したかどうか 
 */
bool Fade_IsEnd(void)
{
  return GFL_SINGLETON_INSTANCE(Fade)->IsEnd();
}

/**
 * @brief ポーズON/OFF
 * @param is_pause ポーズフラグ
 */
void Fade_SetPause(bool is_pause)
{
  GFL_SINGLETON_INSTANCE(Fade)->SetPause(is_pause);
}

/**
 * @brief ポーズ状態取得
 * @retval ポーズ状態
 */
bool Fade_IsPause(void)
{
  return GFL_SINGLETON_INSTANCE(Fade)->IsPause();
}

/**
 * @brief フェード状態リセット
 * @param disp 対象面
 */
void Fade_Reset(Fade::DISP disp)
{
  GFL_SINGLETON_INSTANCE(Fade)->Reset(disp);
}

/**
 * @brief フェード処理カットフラグをセット 
 * @param disp 対象面
 * @param isCutProcess カットするかどうか
 */
void Fade_SetIsCutProcess(Fade::DISP disp, bool isCutProcess)
{
  GFL_SINGLETON_INSTANCE(Fade)->SetIsCutProcess(disp, isCutProcess);
}

/**
 * @brief コールバックのセット 
 * @memo レイアウトシステムを削除
 * @param pCallbackFunc コールバック関数
 * @param pWork コールバックワーク
 */
void Fade_SetCallback(Fade::FadeCallbackFunc* func, void* work)
{
  GFL_SINGLETON_INSTANCE(Fade)->SetCallback(func, work);
}

/**
 * @brief F.I.待機状態かどうかを取得
 * @param 対象面
 */
bool Fade_IsFadeInStandbyState(Fade::DISP disp)
{
  return GFL_SINGLETON_INSTANCE(Fade)->IsFadeInStandbyState(disp);
}

//=====================================================================================
// static
//=====================================================================================

/**
 * @brief 簡易描画で画面塗りつぶし
 * @param grpsys グラフィックシステム
 * @param	type 対象面
 * @param	color 塗りつぶし色
 */
static void _FillScreen(GraphicsSystem* grpsys, DisplayType type, math::VEC4 color, util::Texture * tex)
{
  RenderTarget* renderTarget;
  util::DrawUtil::MaterialInfo info;

  renderTarget = grpsys->GetCurrentFrameBuffer(type);

  if (renderTarget == NULL) {
    return;
  }

  if (grpsys->BindCurrentCommandList(renderTarget)) {
    nngxFlush3DCommand();

    util::DrawUtil::BeginRender(renderTarget);

    if (tex) {
      info.m_pTexture = tex;
      info.m_ShadingType = util::DrawUtil::MaterialInfo::RGBTEXTURE_SHADER;

      f32 l, t, r, b;
      renderTarget->GetViewport(&l, &t, &r, &b);
      f32 width = r - l; //renderTarget->GetWidth();
      f32 height = b - t;

      math::VEC2 uv(width / 512.0f, height / 256.0f);

      const gfl::math::VEC2 uvSet[4] = {
        gfl::math::VEC2(uv.y, uv.x), //左上ＧＬ座標
        gfl::math::VEC2(uv.y, 0.0f), //右上ＧＬ座標
        gfl::math::VEC2(0.0f, uv.x), //左下ＧＬ座標
        gfl::math::VEC2(0.0f, 0.0f)//右下ＧＬ座標
      };

      util::DrawUtil::SetMaterial(info);
      util::DrawUtil::DrawFillScreen(uvSet, color);
    } else {
      util::DrawUtil::SetMaterial(info);
      util::DrawUtil::DrawFillScreen(color);
    }

    util::DrawUtil::EndRender();
  }
}

/**
 * @brief 対象面のカラーバッファを保持する描画コマンドを積む
 * @param grpsys グラフィックシステム
 * @param type  対象面
 * @param out_buf バッファ
 */
static void _KeepColorBuffer(GraphicsSystem* grpsys, DisplayType type, void* out_buf)
{
  RenderTarget* renderTarget;

  renderTarget = grpsys->GetCurrentFrameBuffer(type);

  if (renderTarget) {
    if (grpsys->BindCurrentCommandList(renderTarget)) {
      nngxFlush3DCommand();
      GFL_GL_ASSERT();

      renderTarget->OutputImage(RenderTarget::RENDERBUFFER_TYPE_COLOR, out_buf);
    }
  }
}

/**
 * @brief テクスチャを生成
 * @param grpsys グラフィックシステム
 * @param type 対象面
 * @param buf ダミーデータ
 */
static void _CreateTexture(GraphicsSystem* grpsys, DisplayType type, util::Texture** out_tex, const void* buf)
{
  RenderTarget* renderTarget;
  renderTarget = grpsys->GetCurrentFrameBuffer(type);

  RenderColorFormat format;

  if (renderTarget) {
    format = renderTarget->GetRenderColorFormat();

    u32 w = 256;
    u32 h = 512;
    *out_tex = util::DrawUtil::CreateTexture(grpsys, format, w, h, buf);
  }
}

/**
 * @brief テクスチャ転送
 * @param	grpsys グラフィックシステム
 * @param type 対称面のカラーフォーマット、サイズ設定を取得して内部で使用
 * @param tex テクスチャ(ここで指定したテクスチャに転送する)
 * @param	p_data 転送データ（転送元）
 */
static void _TransTexture(GraphicsSystem* grpsys, DisplayType type, util::Texture** tex, const void* p_data)
{
  u32 format_size;

  RenderTarget* renderTarget;
  renderTarget = grpsys->GetCurrentFrameBuffer(type);

  RenderColorFormat format;
  s32 width;
  s32 height;

  if (p_data == NULL) {
    return;
  }

  if (renderTarget) {
    format = renderTarget->GetRenderColorFormat();
    width = renderTarget->GetWidth();
    height = renderTarget->GetHeight();
  } else {
    return;
  }

  grpsys->BindCurrentCommandList(DISPLAY_NONE);
  nngxFlush3DCommand();

  switch (format) {
  case RENDER_COLOR_FORMAT_RGBA8:
    format_size = 4;
    break;
  default:
    GFL_ASSERT(0);
    format_size = 4;
  }

  u32 size = width * height * format_size;

  s32 src_block_height = height / Fade::BLOCK_HEIGHT_SIZE;
  s32 dest_block_height = (*tex)->GetWidth() / Fade::BLOCK_HEIGHT_SIZE;

  GLvoid* dstadr = reinterpret_cast<GLvoid*> ((*tex)->GetTextureVirtualAddr());
  const GLvoid* p_src = reinterpret_cast<const GLvoid*> (p_data);

  static const int BLOCK_SIZE = 8 * 8;
  static const int DOT_BYTE_SIZE = 4;
  // 8 block line > 240(30) + 16(2)
  nngxAddBlockImageCopyCommand(
    p_src,
    0, 0,
    dstadr,
    BLOCK_SIZE * DOT_BYTE_SIZE * src_block_height,
    BLOCK_SIZE * DOT_BYTE_SIZE * (dest_block_height - src_block_height), //オフセット
    size);
}

/**
 * @brief 指定したカラーバッファのアルファ値を0xffでfill
 * @param	type 対象面
 * @param	buf カラーバッファ
 */
static void _FillAlphaColorBuffer(DisplayType type, void* buf)
{
  u8* adr = static_cast<u8*> (buf);

  int width = Fade::SCREEN_WIDTH;
  int height = Fade::SCREAN_HEIGHT;

  if (type == DISPLAY_LOWER) {
    width = 320;
    height = 240;
  }

  for (int i = 0; i < width * height; i++) {
    adr[0] = 0xff;
    //    adr[1] = 0;
    //    adr[2] = 0;
    //    adr[3] = 0xff;

    adr += 4;
  }

}

/**
 * @brief 保持しているブロックイメージを表示する。
 * @param grpsys グラフィックシステム
 * @param type 対象面
 * @param buf 保持バッファ
 */
static void _DrawColorBuffer(GraphicsSystem* grpsys, DisplayType type, const void* buf)
{
  RenderTarget* renderTarget;
  GLvoid* dstadr;
  u32 format_size = 0;
  u32 size;

  renderTarget = grpsys->GetCurrentFrameBuffer(type);

  if (renderTarget == NULL) {
    return;
  }

  if (buf == NULL) {
    return;
  }

  dstadr = reinterpret_cast<GLvoid*> (renderTarget->GetColorAddress());

  switch (renderTarget->GetRenderColorFormat()) {
  case RENDER_COLOR_FORMAT_RGBA8:
  case RENDER_COLOR_FORMAT_RGB8:
    format_size = 4;
    break;
  case RENDER_COLOR_FORMAT_RGBA4:
  case RENDER_COLOR_FORMAT_RGB5_A1:
  case RENDER_COLOR_FORMAT_RGB565:
    format_size = 2;
    break;
  case RENDER_COLOR_FORMAT_NONE:
  default:
    GFL_ASSERT(0);
    break;
  }

  if (grpsys->BindCurrentCommandList(renderTarget)) {
    size = renderTarget->GetWidth() * renderTarget->GetHeight() * format_size;
    nngxFlush3DCommand();
    nngxAddVramDmaCommand(buf, dstadr, size);
  }
}


} // namespace grp
} // namespace gfl




