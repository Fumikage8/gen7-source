#include <gfx/include/gfl2_DrawUtil.h>
#include <gfx/include/gfl2_GFGL.h>
#include <math/include/gfl2_MathCommon.h>
#include <gfx/include/gfl2_GFGLUtil.h>
#include <util/include/gfl2_FixedSizeContainer.h>

namespace gfl2 { namespace gfx {

//=============================================================================
/*
 * @class DrawUtil::Impl
 * @brief 簡易描画関数群の実装部分をまとめたクラス
*/
//=============================================================================
class DrawUtil::Impl : public GLMemory
{
public:
  //!< @brief 通常描画/拡張描画のラベル
  enum DRAWING_MODE
  {
    DRAWING_MODE_NORMAL,      // 通常描画
    DRAWING_MODE_EXTENSION,   // 拡張描画
    NUMBER_OF_DRAWING_MODE    // 総数
  };

  //=============================================================================
  /*
   * @func コンストラクタ
   * @brief 必要なバッファ領域, ステートオブジェクトの初期化
   */
  //=============================================================================
  Impl(IGLAllocator* pAllocator, u32 screenWidth, u32 screenHeight, u32 primitiveNum, s32 drawCommandNum);

  //=============================================================================
  /*
   * @func デストラクタ
   * @brief 必要なバッファ領域, ステートオブジェクトの解放
   */
  //=============================================================================
  virtual ~Impl();

  //=============================================================================
  /*
   * @func 拡張用メモリ領域の確保
   * @brief 必要なバッファ領域の確保, 初期化
   */
  //=============================================================================
  b32 InitializeExtension(IGLAllocator* pAllocator, u32 primitiveNum, s32 drawCommandNum);

  //=============================================================================
  /*
   * @func 拡張用メモリ領域の確保
   * @brief 必要なバッファ領域の解放
   */
  //=============================================================================
  void TerminateExtension();

  //=============================================================================
  /*
   * @func BeginRender
   * @brief 簡易描画開始前の処理. BeginRender()?EndRender()で簡易描画処理を挟む
   * @param DRAWING_MODE drawingMode 描画モード(通常/拡張)
   */
  //=============================================================================
  void BeginRender(DRAWING_MODE drawingMode);

	//=============================================================================
	/*
	 * @func EndRender
	 * @brief 簡易描画開始後の処理. BeginRender()?EndRender()で簡易描画処理を挟む
   * @param DRAWING_MODE drawingMode 描画モード(通常/拡張)
	 */
	//=============================================================================
	void EndRender(DRAWING_MODE drawingMode);

	//=============================================================================
	/*
	 * @func SetCullMode
	 * @brief カリング設定
	 */
	//=============================================================================
	void SetCullMode(const CullMode cullMode);

	//=============================================================================
	/*
	 * @func SetFillMode
	 * @brief ワイヤーフレーム/通常描画切り替え設定
	 */
	//=============================================================================
	void SetFillMode(const FillMode fillMode);

	//=============================================================================
	/*
	 * @func SetMaterialColor
	 * @brief マテリアルカラー設定
	 */
	//=============================================================================
	void SetMaterialColor(const math::Vector4& color);

	//=============================================================================
	/*
	 * @func SetMaterialAlpha
	 * @brief マテリアルカラーのアルファ値設定
	 */
	//=============================================================================
	void SetMaterialAlpha(const f32 alpha);

  //=============================================================================
  /*
   * @func SetBlendTestState
   * @brief ブレンドテスト設定
   */
  //=============================================================================
  void SetBlendTestState(
    b32 blendEnable,
    BlendOp blendOp,
    BlendFunc srcColorFunc,
    BlendFunc destColorFunc,
    BlendFunc srcAlphaFunc,
    BlendFunc destAlphaFunc,
    BlendStateObject::COLOR_WRITE_ENABLE colorWriteEnable);

  //=============================================================================
  /*
   * @func SetBlendTestState
   * @brief ブレンドテスト有効/無効設定
   */
  //=============================================================================
  void SetBlendTestEnable(b32 enable);

	//=============================================================================
	/*
	 * @func SetProjectionMatrix
	 * @brief プロジェクション行列の設定
	 */
	//=============================================================================
	void SetProjectionMatrix(const math::Matrix44& mat);

	//=============================================================================
	/*
	 * @func SetViewMatrix
	 * @brief ビュー行列の設定
	 */
	//=============================================================================
	void SetViewMatrix(const math::Matrix34& mat);

	//=============================================================================
	/*
	 * @func SetModelMatrix
	 * @brief モデル行列(ワールド行列)の設定
	 */
	//=============================================================================
  void SetModelMatrix(const math::Matrix44& mat);

  //=============================================================================
  /*
   * @func SetTexture
   * @brief テクスチャの設定
   */
  //=============================================================================
  void SetTexture(const Texture* texture);

  //=============================================================================
  /*
   * @func SetTexture
   * @brief テクスチャの設定
   */
  //=============================================================================
  const Texture* GetTexture();

  //=============================================================================
  /*
   * @func DrawTriangles
   * @brief 頂点データを三角形リストとして描画
   */
  //=============================================================================
  void DrawTriangles(const VertexFormat* pVertexArray, u32 vertexNum, b32 enableTexture);

#if defined(GF_PLATFORM_WIN32)
  //=============================================================================
  /*
   * @func DrawLines
   * @brief 頂点データをラインリストとして描画
   */
  //=============================================================================
  void DrawLines(const VertexFormat* pVertexArray, u32 vertexNum, b32 enableTexture);
#endif

	//=============================================================================
	/*
	 * @func DrawRect
	 * @brief 矩形描画
	 */
	//=============================================================================
	void DrawRect(f32 x, f32 y, f32 w, f32 h, const math::Vector4& color, f32 u0 = 0.0f, f32 v0 = 0.0f, f32 u1 = 1.0f, f32 v1 = 1.0f);

	//=============================================================================
	/*
	 * @func FillRect
	 * @brief 矩形塗りつぶし
	 */
	//=============================================================================
	void FillRect(f32 x, f32 y, f32 w, f32 h, const math::Vector4& color);

	//=============================================================================
	/*
	 * @func DrawScreen
	 * @brief スクリーン全体に矩形描画
	 */
	//=============================================================================
	void DrawScreen(const math::Vector4& color, f32 u0, f32 v0, f32 u1, f32 v1);

	//=============================================================================
	/*
	 * @func FillScreen
	 * @brief スクリーン全体に矩形塗りつぶし
	 */
	//=============================================================================
	void FillScreen(const math::Vector4& color);

	//=============================================================================
	/*
	 * @func DrawSphere
	 * @brief 球体描画
	 */
	//=============================================================================
	void DrawSphere(const math::Vector4& center, f32 radius, const math::Vector4& color);

	//=============================================================================
	/*
	 * @func DrawAABB
	 * @brief AABB(軸平行境界ボックス)描画
	 */
	//=============================================================================
	void DrawAABB(const math::Vector4& min, const math::Vector4& max, const math::Vector4& color);

  //=============================================================================
	/*
	 * @func DrawCircle
	 * @brief 円描画
	 */
	//=============================================================================
  void DrawCircle(const math::Vector4& position, float radius, const math::Vector4& color);

  //=============================================================================
  /*
   * @func DrawCylinder
   * @brief 筒描画
   */
  //=============================================================================
  void DrawTube(const math::Vector4& position, float radius, float height, const math::Vector4& color);

  //=============================================================================
	/*
	 * @func DrawCylinder
	 * @brief 円柱描画
	 */
	//=============================================================================
  void DrawCylinder(const math::Vector4& position, float radius, float height, const math::Vector4& color);

	//=============================================================================
	/*
	 * @func ClearScissor
	 * @brief シザリング設定解除
	 */
	//=============================================================================
	void ClearScissor(void);

	//=============================================================================
	/*
	 * @func SetScissor
	 * @brief シザリング設定
	 */
	//=============================================================================
	void SetScissor(u32 x, u32 y, u32 width, u32 height);

  //=============================================================================
  /*
   * @func SetDepthTestState
   * @brief 深度テスト設定
   */
  //=============================================================================
  void SetDepthTestState(const gfl2::gfx::DepthStencilStateObject::StateData& stateData);

  //=============================================================================
  /*
   * @func SetDepthTestEnable
   * @brief 深度テスト有効/無効設定
   */
  //=============================================================================
  void SetDepthTestEnable(b32 enable);

  //=============================================================================
  /*
   * @func SetScreenSize
   * @brief スクリーンの横幅/縦幅の設定
   */
  //=============================================================================
  void SetScreenSize(u32 width, u32 height);

  //=============================================================================
  /*
   * @func GetScreenWidth
   * @brief スクリーンの横幅の取得
   */
  //=============================================================================
  u32 GetScreenWidth();

  //=============================================================================
  /*
   * @func GetScreenHeight
   * @brief スクリーンの横幅の取得
   */
  //=============================================================================
  u32 GetScreenHeight();

  //=============================================================================
  /*
   * @func GetPrimitiveMax
   * @brief 通常バッファの描画可能プリミティブ数の取得
   * @param drawingMode 描画モード(通常/拡張)
   */
  //=============================================================================
  u32 GetPrimitiveMax(DRAWING_MODE drawingMode);

  //=============================================================================
  /*
   * @func GetDrawCommandMax
   * @brief 通常バッファの描画可能コマンド数の取得
   * @param drawingMode 描画モード(通常/拡張)
   */
  //=============================================================================
  u32 GetDrawCommandMax(DRAWING_MODE drawingMode);

  //=============================================================================
  /*
   * @func SwapBuffers
   * @brief バッファの切り替え
   */
  //=============================================================================
  void SwapBuffers();

private:
  //=============================================================================
  /*
   * @func コンストラクタ
   * @brief シングルトンなので呼び出し禁止
   */
  //=============================================================================
  Impl(const Impl&);

  //=============================================================================
  /*
   * @func 描画に必要なリソースのメモリ確保
   * @param pAllocator アロケータ
   * @param drawingMode 描画モード(通常/拡張)
   * @param primitiveNum バッファの総数
   * @param drawCommandNum 描画コマンドの総数
   */
  //=============================================================================
  b32 AllocateDrawResource(IGLAllocator* pAllocator, DRAWING_MODE drawingMode, u32 primitiveNum, s32 drawCommandNum);

  //=============================================================================
  /*
   * @func 描画に必要なリソースのメモリ解放
   * @param 描画モード(通常/拡張)
   */
  //=============================================================================
  void DeallocateDrawResource(DRAWING_MODE drawingMode);

	//=============================================================================
	/*
	 * @func デストラクタ
	 * @brief シングルトンなので呼び出し禁止
	 */
	//=============================================================================
	void operator=(const Impl&){}
#if defined(GF_PLATFORM_WIN32)
  static const u32 CIRCLE_DIVISION_NUM = 36;              // 円を描画するときの分割数
#else
  static const u32 CIRCLE_DIVISION_NUM = 16;              // 円を描画するときの分割数(CTR版は頂点バッファで描画しているので短めに確保)
#endif
	static const u32 VERTEX_NUM = 16;                       // 1回で描画可能な頂点数

	IGLAllocator* m_pAllocators[NUMBER_OF_DRAWING_MODE];

	//=============================================================================
	/*
	 * @func Draw
	 * @brief 定数バッファに積まれている頂点データの描画
	 */
	//=============================================================================
	bool Draw(const PrimitiveType primitiveType, const u32 vertexNum, b32 enableTexture);

	//=============================================================================
	/*
	 * @func DrawRectangle
	 * @brief 2Dスクリーン座標に合わせた頂点データの描画
	 */
	//=============================================================================
	void DrawRectangle(f32 x, f32 y, f32 w, f32 h, math::Vector4 color, b32 enableTexture, f32 u0 = 0.0f, f32 v0 = 0.0f, f32 u1 = 1.0f, f32 v1 = 1.0f);

  //=============================================================================
  /*
   * @func DrawVertices
   * @brief 頂点データ描画
   */
  //=============================================================================
  void DrawVertices(PrimitiveType primitiveType, const VertexFormat* pVertexArray, u32 vertexNum, b32 enableTexture);

  //=============================================================================
  /*
   * @func DrawPrimitive
   * @brief 頂点データ描画
   */
  //=============================================================================
  void DrawPrimitive(PrimitiveType primitiveType, const VertexFormat* pBuffer, u32 vertexNum, b32 enableTexture);

  //=============================================================================
  /*
   * @func CalculateDelimiter
   * @brief 頂点バッファの区切りインデックスの算出
   */
  //=============================================================================
  u32 CalculateDelimiter(PrimitiveType primitiveType);

  struct CB_TYPES
  {
    enum
    {
      NEVER_CHANGES,
      CHANGES_EVERY_FRAME,
      CHANGES_EVERY_OBJECT,
      MATERIAL_COLOR,
#if defined(GF_PLATFORM_WIN32)
      VERTICES,
#endif
      NumberOf
    };
  };

	// 定数バッファのデータ定義0　変更しないデータ
	struct cbNeverChanges {
		math::Matrix44 Projection;   // 透視変換行列
	};

	// 定数バッファのデータ定義1　変更頻度の低いデータ
	struct cbChangesEveryFrame {
		math::Matrix44  View;  // ビュー変換行列
	};

	// 定数バッファのデータ定義2　変更頻度の低いデータ
	struct cbChangesEveryObject {
      math::Matrix44  World;  // ビュー変換行列
	};

  // 定数バッファのデータ定義3　マテリアルの代わり
  struct cbMaterialColor {
    math::Vector4 Color;				// マテリアルカラー
  };

#if defined(GF_PLATFORM_WIN32)
  // 定数バッファのデータ定義3　頂点バッファの代わり
  struct cbVertices {
    math::Vector4 Position[VERTEX_NUM];		// 頂点座標
    math::Vector4 Color[VERTEX_NUM];				// 頂点カラー
  };
#endif

#if defined(GF_PLATFORM_CTR)
  //!< @brief 描画キックの際のジオメトリの描画情報
  struct DrawCommand
  {
    u16 m_UniqueId;                     //!< @brief 構造体を識別するID(重複しないように設定)
    u16 m_PrimitiveNum;                 //!< @brief 描画する頂点数
    math::Matrix44 m_ProjectionMatrix;  //!< @brief 射影行列
    math::Matrix34 m_ViewMatrix;        //!< @brief ビュー行列
    math::Matrix44 m_WorldMatrix;       //!< @brief ワールド行列
    math::Vector4 m_MaterialColor;      //!< @brief マテリアルカラー
    const Texture* m_pTexture;          //!< @brief マッピングするテクスチャ

    //!<  @brief UniqueIdが一致しているかどうかを確認する
    inline bool operator ==(const DrawCommand& rhs) const
    {
      return (this->m_UniqueId == rhs.m_UniqueId);
    }
  };

  //!< @brief 描画に必要なリソース
  struct DrawResource
  {
    u32           m_PrimitiveMax;               //!< @brief 描画できるプリミティブの最大数
    u32           m_DrawCommandMax;             //!< @brief 描画実行可能コマンドの最大数
    u32           m_CurrentVertexBufferIndex;   //!< @brief 現在使用している頂点バッファ
    VertexBuffer* m_pVertexBuffer;              //!< @brief 頂点バッファ
    VertexFormat* m_VertexDataArray;            //!< @brief 頂点のデータ配列
    u32           m_VertexNum;                  //!< @brief 描画する頂点数
    u32           m_StartVertexIndex;           //!< @brief 各Begin開始時の頂点Index
    GLFixedSizeContainer<DrawCommand> m_DrawCommandContainer; //!< @brief 描画コマンド情報を保存するコンテナ
  };
#endif

  //!< 描画判定フラグ
  b32 m_IsDrawing;

  //!< @brief 現在の描画モード(通常/拡張)
  DRAWING_MODE m_DrawingMode;

	// ライティングは考慮しない(光源座標を扱わない)
  IndexBuffer*        m_pIndexBuffers[NUMBER_OF_DRAWING_MODE];     // インデックスバッファ

  //!< @brief 描画に必要なシェーダ
  struct Shaders
  {
    Shader* m_pVertexShader;   // 頂点シェーダ
    Shader* m_pPixelShader;    // ピクセル・シェーダ
  };

  //!< @brief シェーダ
  Shaders m_Shaders[NUMBER_OF_DRAWING_MODE];

  //!< @brief 描画に必要な定数バッファ
  struct ConstantBuffers
  {
    ConstantBuffer* m_pNeverChanges;        // 透視変換行列
    ConstantBuffer* m_pChangesEveryFrame;   // ビュー変換行列
    ConstantBuffer* m_pChangesEveryObject;  // ワールド変換行列
    ConstantBuffer* m_pMaterialColor;       // マテリアルカラー
#if defined(GF_PLATFORM_WIN32)
    ConstantBuffer* m_pVertices;            // 頂点座標
#endif
  };

  //!< @brief 定数バッファに格納するデータ
  struct ConstantVariables
  {
    // 定数バッファのデータ
    cbNeverChanges        m_cbNeverChanges;       // 透視変換行列
    cbChangesEveryFrame   m_cbChangesEveryFrame;  // ビュー変換行列
    cbChangesEveryObject  m_cbChangesEveryObject; // ワールド変換行列
    cbMaterialColor       m_cbMaterialColor;      // マテリアルカラー
#if defined(GF_PLATFORM_WIN32)
    cbVertices            m_cbVertices;           // 頂点座標
#endif
  };

  //!< 定数バッファ
  ConstantBuffers m_ConstantBuffers[NUMBER_OF_DRAWING_MODE];
  //!< 定数バッファに格納するデータ
  ConstantVariables m_ConstantVariables[NUMBER_OF_DRAWING_MODE];

  // 描画する頂点配列. この配列に書き込まれたデータをバッファに書き込む.
  // Win32版は頂点バッファではなく, 定数バッファで描画するので使用しない.
#if defined(GF_PLATFORM_CTR)
  DrawResource m_DrawResources[NUMBER_OF_DRAWING_MODE];
#endif

  //!< @brief ステートオブジェクトの構造体
  struct StateObjects
  {
    BlendStateObject          *m_pBlendStateObject;           // ブレンド・ステート・オブジェクト
    RasterizerStateObject     *m_RasterizerStateObject;       // ラスタライザ・ステート・オブジェクト
    DepthStencilStateObject   *m_DepthStencilStateObject;     // 深度/ステンシル・ステート・オブジェクト
    SamplerStateObject        *m_SamplerStateObject;          // サンプラー・ステート・オブジェクト
  };

  //!< @brief 使用するステートオブジェクト
  StateObjects m_StateObjects[NUMBER_OF_DRAWING_MODE];
  
  //!< @brief テクスチャ
  struct Textures
  {
    const Texture             *m_pTexture;                    // テクスチャ
    const Texture             *m_pDummyTexture;               // ダミーテクスチャ(8x8. 白)
  };

  //!< @brief 使用するテクスチャ
  Textures m_Textures[NUMBER_OF_DRAWING_MODE];

  u32 m_ScreenWidth;        //!< @brief 描画する画面の横幅
  u32 m_ScreenHeight;       //!< @brief 描画する画面の縦幅
};

DrawUtil::Impl* DrawUtil::s_pImpl = NULL;

DrawUtil::Impl::Impl(IGLAllocator* pAllocator, u32 screenWidth, u32 screenHeight, u32 primitiveNum, s32 drawCommandNum) : 
  m_IsDrawing(false),
  m_DrawingMode(DRAWING_MODE_NORMAL)
{
  if (screenWidth == 0)
  {
    GFL_ASSERT((screenWidth > 0) && "DrawUtil::Impl() doesn't work correctly. 'screenWidth' should be larger than 0.");
    return;
  }

  if (screenHeight == 0)
  {
    GFL_ASSERT((screenHeight > 0) && "DrawUtil::Impl() doesn't work correctly. 'screenHeight' should be larger than 0.");
    return;
  }
  m_ScreenWidth = screenWidth;
  m_ScreenHeight = screenHeight;

  // ポインタの配列をNULLクリア
  for (u32 i = 0; i < NUMBER_OF_DRAWING_MODE; ++i)
  {
#if defined(GF_PLATFORM_CTR)
    m_DrawResources[i].m_PrimitiveMax = 0;
    m_DrawResources[i].m_DrawCommandMax = 0;
    m_DrawResources[i].m_CurrentVertexBufferIndex = 0;
    m_DrawResources[i].m_VertexNum = 0;
    m_DrawResources[i].m_StartVertexIndex = 0;
#endif
    m_pIndexBuffers[i] = NULL;
    m_pAllocators[i] = NULL;

    m_ConstantBuffers[i].m_pNeverChanges = NULL;
    m_ConstantBuffers[i].m_pChangesEveryFrame = NULL;
    m_ConstantBuffers[i].m_pChangesEveryObject = NULL;
    m_ConstantBuffers[i].m_pMaterialColor = NULL;
#if defined(GF_PLATFORM_WIN32)
    m_ConstantBuffers[i].m_pVertices = NULL;
#endif
    m_Textures[i].m_pTexture = NULL;
    m_Textures[i].m_pDummyTexture = NULL;

    m_Shaders[i].m_pVertexShader = NULL;
    m_Shaders[i].m_pPixelShader = NULL;
  }

#if defined(GF_PLATFORM_CTR)
  // 描画可能最大数を設定
  m_DrawResources[DRAWING_MODE_NORMAL].m_PrimitiveMax = primitiveNum;
  m_DrawResources[DRAWING_MODE_NORMAL].m_DrawCommandMax = drawCommandNum;
#endif

  // 通常アロケータの設定
  m_pAllocators[DRAWING_MODE_NORMAL] = pAllocator;

  // 通常の描画に必要なバッファ領域を確保
  AllocateDrawResource(m_pAllocators[DRAWING_MODE_NORMAL], DRAWING_MODE_NORMAL, primitiveNum, drawCommandNum);
}

DrawUtil::Impl::~Impl()
{
  // 通常のメモリ領域を解放
  DeallocateDrawResource(DRAWING_MODE_NORMAL);
}

//=============================================================================
/*
 * @func 拡張用領域での描画に必要なリソースのメモリ確保
 */
//=============================================================================
b32 DrawUtil::Impl::InitializeExtension(IGLAllocator* pAllocator, u32 primitiveNum, s32 drawCommandNum)
{
  b32 isSucceeded = true;
#if defined(GF_PLATFORM_CTR)
  if (m_pAllocators[DRAWING_MODE_EXTENSION])
  {// 既にアロケータが設定されていたら初期化済みなので終了
    GFL_ASSERT(!m_pAllocators[DRAWING_MODE_EXTENSION] && "DrawUtil::Impl::InitializeExtension() is called twice.");
    return false;
  }
  isSucceeded = AllocateDrawResource(pAllocator, DRAWING_MODE_EXTENSION, primitiveNum, drawCommandNum);
  m_DrawResources[DRAWING_MODE_EXTENSION].m_PrimitiveMax = primitiveNum;
  m_DrawResources[DRAWING_MODE_EXTENSION].m_DrawCommandMax = drawCommandNum;

  // アロケータを登録
  m_pAllocators[DRAWING_MODE_EXTENSION] = pAllocator;
#endif

  return isSucceeded;
}

//=============================================================================
/*
 * @func 拡張用領域での描画に必要なリソースのメモリ解放
 */
//=============================================================================
void DrawUtil::Impl::TerminateExtension()
{
  DeallocateDrawResource(DRAWING_MODE_EXTENSION);
}

//=============================================================================
/*
 * @func 描画に必要なリソースのメモリ確保
 * @param pAllocator アロケータ
 * @param drawingMode 描画モード(通常/拡張)
 * @param primitiveNum バッファの総数
 * @param drawCommandNum 描画コマンドの総数
 */
//=============================================================================
b32 DrawUtil::Impl::AllocateDrawResource(IGLAllocator* pAllocator, DRAWING_MODE drawingMode, u32 primitiveNum, s32 drawCommandNum)
{
#if defined(GF_PLATFORM_CTR)
  // 頂点情報の設定
  m_DrawResources[drawingMode].m_VertexDataArray = static_cast<VertexFormat*>(pAllocator->SystemMemoryMallocAlign(primitiveNum * sizeof(VertexFormat) * 2, 128));
  m_DrawResources[drawingMode].m_pVertexBuffer = GFGL::CreateVertexBuffer(pAllocator, sizeof(VertexFormat), primitiveNum * 2,  Usage::WriteOnly, Pool::Default, m_DrawResources[drawingMode].m_VertexDataArray);

  // 描画情報の最大数ぶんだけコンテナを確保
  if (drawCommandNum <= 0)
  {
    GFL_ASSERT((drawCommandNum > 0) && "DrawUtil::Impl::AllocateDrawResource() doesn't work correctly. drawCommandNum should be larger than 0 in CTR.");
    return false;
  }

  m_DrawResources[drawingMode].m_DrawCommandContainer.CreateBuffer(static_cast<u32>(drawCommandNum), pAllocator);
  //for (s32 uniqueId = 0; uniqueId < drawCommandNum; ++uniqueId)
  //{// 重複しないIDを全ての要素に設定.
  //  m_DrawResources[drawingMode].m_DrawCommandContainer[uniqueId].m_UniqueId = uniqueId;
  //}
#endif

  // インデックスバッファを作成
  IndexType indexType = IndexType::Index16;
  u32 indicesNum = primitiveNum * 2;
#if defined(GF_PLATFORM_WIN32)
  indicesNum = DrawUtil::Impl::VERTEX_NUM;    // Win32版はインデックスは最大値を使う
#endif
  u16* idxVertexID = static_cast<u16*>(pAllocator->SystemMemoryMallocAlign(indicesNum * sizeof(u16), 128));
  for (u32 i = 0; i < indicesNum; ++i)
  {
    idxVertexID[i] = i;
  }
  m_pIndexBuffers[drawingMode] = GFGL::CreateIndexBuffer(pAllocator, indicesNum * sizeof(u16), Usage::WriteOnly, Pool::Default, indexType, idxVertexID);
  pAllocator->SystemMemoryFree(idxVertexID);

  // 定数バッファを作成
  ConstantBufferElement elementMatrix[] = {
    { ConstantType::Matrix, 1 },
    ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
  };
      
  m_ConstantBuffers[drawingMode].m_pNeverChanges = GFGL::CreateConstantBuffer( pAllocator, elementMatrix, Usage::WriteOnly, Pool::Default );
  m_ConstantBuffers[drawingMode].m_pChangesEveryFrame = GFGL::CreateConstantBuffer( pAllocator, elementMatrix, Usage::WriteOnly, Pool::Default );
  m_ConstantBuffers[drawingMode].m_pChangesEveryObject = GFGL::CreateConstantBuffer( pAllocator, elementMatrix, Usage::WriteOnly, Pool::Default );
  // マテリアルカラー用の定数バッファを作成
  ConstantBufferElement elementMaterial[] = {
    { ConstantType::Vector, 1 },	// マテリアルカラー
    ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
  };
  m_ConstantBuffers[drawingMode].m_pMaterialColor = GFGL::CreateConstantBuffer( pAllocator, elementMaterial, Usage::WriteOnly, Pool::Default );

#if defined(GF_PLATFORM_WIN32)
  // 頂点データ用の定数バッファを作成
  ConstantBufferElement elementVertex[] = {
    { ConstantType::Vector, DrawUtil::Impl::VERTEX_NUM },	// 頂点座標(wはテクスチャ座標のu座標)
    { ConstantType::Vector, DrawUtil::Impl::VERTEX_NUM },	// 頂点座標(wはテクスチャ座標のv座標)
    ConstantBufferElement::CONSTANT_BUFFER_ELEMENT_END()
  };
  m_ConstantBuffers[drawingMode].m_pVertices = GFGL::CreateConstantBuffer( pAllocator, elementVertex, Usage::WriteOnly, Pool::Default );
#endif

  m_StateObjects[drawingMode].m_pBlendStateObject = GFGL::CreateBlendStateObject(pAllocator);
    
  m_StateObjects[drawingMode].m_pBlendStateObject->SetAlphaTestEnable( false );
  m_StateObjects[drawingMode].m_pBlendStateObject->SetBlendEnable( true );
  m_StateObjects[drawingMode].m_pBlendStateObject->SetBlendOp( BlendOp::Add );
  m_StateObjects[drawingMode].m_pBlendStateObject->SetBlendFunc( BlendFunc::SrcAlpha, BlendFunc::InverseSrcAlpha );
  m_StateObjects[drawingMode].m_pBlendStateObject->UpdateState();

  m_StateObjects[drawingMode].m_RasterizerStateObject = GFGL::CreateRasterizerStateObject(pAllocator);
  m_StateObjects[drawingMode].m_RasterizerStateObject->SetCullMode(CullMode::None);
  m_StateObjects[drawingMode].m_RasterizerStateObject->SetFillMode(FillMode::Fill);
  m_StateObjects[drawingMode].m_RasterizerStateObject->UpdateState();

  m_StateObjects[drawingMode].m_DepthStencilStateObject = GFGL::CreateDepthStencilStateObject(pAllocator);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetDepthTestEnable(false);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetDepthWriteEnable(true);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilOp(PolygonFace::CW, StencilOp::Keep, StencilOp::Keep, StencilOp::Replace);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilFunc(PolygonFace::CW, CompareFunc::Always, 0x00, 0xFF);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilTestEnable(true);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilWriteMask(0xFF);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->UpdateState();

  m_StateObjects[drawingMode].m_SamplerStateObject = GFGL::CreateSamplerStateObject(pAllocator);
  m_StateObjects[drawingMode].m_SamplerStateObject->UpdateState();

  ImageData image(pAllocator);
#if defined(GF_PLATFORM_WIN32)
  GF_ALIGNED(0x80) static const u8 s_dummy_texture[] = {
    #include "../Sample/data/dummy.tga.txt"
    0x00
  };
  image.ReadFromMemory(s_dummy_texture);
#elif defined(GF_PLATFORM_CTR)
  GF_ALIGNED(0x80) static const u8 s_dummy_texture[] = {
    #include "../resource/dummy.ctpk.txt"
    0x00
  };
  u32 texFileSize = sizeof(s_dummy_texture);
  void* fileBuf = pAllocator->SystemMemoryMallocAlign(texFileSize, 128);
  memcpy(fileBuf, s_dummy_texture, sizeof(s_dummy_texture));
  image.ReadFromMemory(static_cast<u8*>(fileBuf));
#endif
  m_Textures[drawingMode].m_pDummyTexture = GFGL::CreateTextureFromImage( pAllocator, image, Pool::Default );

  VertexElement element[] = {
#if defined(GF_PLATFORM_CTR)
    { 0, StreamUsage::Position,	0, VertexType::Float, 4 },
    { 0, StreamUsage::TexCoord,	0, VertexType::Float, 4 },
    { 0, StreamUsage::Color,		0, VertexType::Float, 4 },
#endif
    VertexElement::VERTEX_ELEMENT_END()
  };

  // シェーダの読み込み
#if defined(GF_PLATFORM_WIN32)
  GF_ALIGNED(0x80) static const u8 s_drawutil_gfvsh[] = {
    #include "../resource/DrawUtil.gfvsh.txt"
    0x00
  };
  GF_ALIGNED(0x80) static const u8 s_drawutil_gffsh[] = {
    #include "../resource/DrawUtil.gffsh.txt"
    0x00
  };

  m_Shaders[drawingMode].m_pVertexShader = GFGL::CreateVertexShaderFromSource(pAllocator, reinterpret_cast<const c8*>(pAllocator, s_drawutil_gfvsh), "VS", element, NULL);
  m_Shaders[drawingMode].m_pPixelShader = GFGL::CreatePixelShaderFromSource(pAllocator, reinterpret_cast<const c8*>(pAllocator, s_drawutil_gffsh), "PS", NULL);
#elif defined(GF_PLATFORM_CTR)
  //CTR
  GF_ALIGNED(0x80) static const u8 s_drawutil_shbin[] = {
    #include "../resource/DrawUtil.shbin.txt"
    0x00
  };
  u32 fileSize = sizeof(s_drawutil_shbin);
  void* pShaderBinary = pAllocator->SystemMemoryMallocAlign(fileSize, 128);
  memcpy(pShaderBinary, s_drawutil_shbin, sizeof(s_drawutil_shbin));
  m_Shaders[drawingMode].m_pVertexShader = GFGL::CreateVertexShaderFromMemory(pAllocator, pShaderBinary, fileSize, element);

  u32 pixelShaderCommand[256];
  u32 pixelShaderSize = 0;
  {
    nn::gr::Combiner cm;
    cm.stage[0].rgb.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
    cm.stage[0].rgb.operand[0] = PICA_DATA_OPE_RGB_SRC_COLOR;
    cm.stage[0].rgb.operand[1] = PICA_DATA_OPE_RGB_SRC_COLOR;
    cm.stage[0].rgb.operand[2] = PICA_DATA_OPE_RGB_SRC_COLOR;
    cm.stage[0].rgb.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
    cm.stage[0].rgb.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
    cm.stage[0].rgb.source[2] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
    cm.stage[0].rgb.scale = PICA_DATA_TEX_ENV_SCALE_1;
    cm.stage[0].rgb.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS;
    cm.stage[0].alpha.combine = PICA_DATA_TEX_ENV_COMBINE_MODULATE;
    cm.stage[0].alpha.operand[0] = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
    cm.stage[0].alpha.operand[1] = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
    cm.stage[0].alpha.operand[2] = PICA_DATA_OPE_ALPHA_SRC_ALPHA;
    cm.stage[0].alpha.source[0] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
    cm.stage[0].alpha.source[1] = PICA_DATA_TEX_ENV_SRC_RGBA_PRIMARY_COLOR;
    cm.stage[0].alpha.source[2] = PICA_DATA_TEX_ENV_SRC_RGBA_TEXTURE0;
    cm.stage[0].alpha.scale = PICA_DATA_TEX_ENV_SCALE_1;
    cm.stage[0].alpha.bufferInput = PICA_DATA_TEX_ENV_BUFFER_INPUT_PREVIOUS;

    cm.stage[1].SetupPrevious();
    cm.stage[2].SetupPrevious();
    cm.stage[3].SetupPrevious();
    cm.stage[4].SetupPrevious();
    cm.stage[5].SetupPrevious();
    u32* endAddr = cm.MakeCommand(pixelShaderCommand);

    // サブルーチン用のコマンド追加
    u32 kickCommandSize = sizeof(u32) * 2; // キックコマンドは8バイト
    endAddr = nn::gr::AddDummyDataForCommandBuffer(endAddr, (endAddr - pixelShaderCommand) * sizeof(u32) + kickCommandSize);
    endAddr = nn::gr::MakeChannelKickCommand(endAddr, nn::gr::CMD_BUF_CH1);

    pixelShaderSize = (endAddr - pixelShaderCommand) * sizeof(u32);
  }
  m_Shaders[drawingMode].m_pPixelShader = GFGL::CreatePixelShaderFromMemory(pAllocator, pixelShaderCommand, pixelShaderSize);
#endif

  // 各コンスタントバッファの値を初期化
  DRAWING_MODE prevDrawingMode = m_DrawingMode;
  m_DrawingMode = drawingMode;
  SetProjectionMatrix(math::Matrix44::GetIdentity());
  SetViewMatrix(math::Matrix34::GetIdentity());
  SetModelMatrix(math::Matrix44::GetIdentity());
  SetMaterialColor(math::Vector4(1.0f, 1.0f, 1.0f, 1.0f));
  m_DrawingMode = prevDrawingMode;

  return true;
}

//=============================================================================
/*
 * @func 描画に必要なリソースのメモリ解放
 * @param 描画モード(通常/拡張)
 */
//=============================================================================
void DrawUtil::Impl::DeallocateDrawResource(DRAWING_MODE drawingMode)
{
#if defined(GF_PLATFORM_CTR)
  if (m_DrawResources[drawingMode].m_pVertexBuffer)
  {
    delete m_DrawResources[drawingMode].m_pVertexBuffer;
    m_DrawResources[drawingMode].m_pVertexBuffer = NULL;
  }

  if (m_DrawResources[drawingMode].m_VertexDataArray)
  {
    m_pAllocators[drawingMode]->SystemMemoryFree(m_DrawResources[drawingMode].m_VertexDataArray);
    m_DrawResources[drawingMode].m_VertexDataArray = NULL;
  }
#endif

  if (m_pIndexBuffers[drawingMode])
  {
    delete m_pIndexBuffers[drawingMode];
    m_pIndexBuffers[drawingMode] = NULL;
  }

  if (m_ConstantBuffers[drawingMode].m_pNeverChanges)
  {
    delete m_ConstantBuffers[drawingMode].m_pNeverChanges;
    m_ConstantBuffers[drawingMode].m_pNeverChanges = NULL;
  }

  if (m_ConstantBuffers[drawingMode].m_pChangesEveryFrame)
  {
    delete m_ConstantBuffers[drawingMode].m_pChangesEveryFrame;
    m_ConstantBuffers[drawingMode].m_pChangesEveryFrame = NULL;
  }

  if (m_ConstantBuffers[drawingMode].m_pChangesEveryObject)
  {
    delete m_ConstantBuffers[drawingMode].m_pChangesEveryObject;
    m_ConstantBuffers[drawingMode].m_pChangesEveryObject = NULL;
  }

  if (m_ConstantBuffers[drawingMode].m_pMaterialColor)
  {
    delete m_ConstantBuffers[drawingMode].m_pMaterialColor;
    m_ConstantBuffers[drawingMode].m_pMaterialColor = NULL;
  }

#if defined(GF_PLATFORM_WIN32)
  if (m_ConstantBuffers[drawingMode].m_pVertices)
  {
    delete m_ConstantBuffers[drawingMode].m_pVertices;
    m_ConstantBuffers[drawingMode].m_pVertices = NULL;
  }
#endif

  if (m_StateObjects[drawingMode].m_pBlendStateObject)
  {
    delete m_StateObjects[drawingMode].m_pBlendStateObject;
    m_StateObjects[drawingMode].m_pBlendStateObject = NULL;
  }

  if (m_StateObjects[drawingMode].m_RasterizerStateObject)
  {
    delete m_StateObjects[drawingMode].m_RasterizerStateObject;
    m_StateObjects[drawingMode].m_RasterizerStateObject = NULL;
  }

  if (m_StateObjects[drawingMode].m_DepthStencilStateObject)
  {
    delete m_StateObjects[drawingMode].m_DepthStencilStateObject;
    m_StateObjects[drawingMode].m_DepthStencilStateObject = NULL;
  }

  if (m_StateObjects[drawingMode].m_SamplerStateObject)
  {
    delete m_StateObjects[drawingMode].m_SamplerStateObject;
    m_StateObjects[drawingMode].m_SamplerStateObject = NULL;
  }

  if (m_Textures[drawingMode].m_pDummyTexture)
  {
    delete m_Textures[drawingMode].m_pDummyTexture;
    m_Textures[drawingMode].m_pDummyTexture = NULL;
  }

  if (m_Shaders[drawingMode].m_pVertexShader)
  {
    delete m_Shaders[drawingMode].m_pVertexShader;
    m_Shaders[drawingMode].m_pVertexShader = NULL;
  }

  if (m_Shaders[drawingMode].m_pPixelShader)
  {
    delete m_Shaders[drawingMode].m_pPixelShader;
    m_Shaders[drawingMode].m_pPixelShader = NULL;
  }
}

void DrawUtil::Impl::BeginRender(DRAWING_MODE drawingMode)
{
  if (m_IsDrawing)
  {
    GFL_ASSERT(!m_IsDrawing && "DrawUtil::Impl::BeginRender() is called twice.");
    return;
  }

#if defined(GF_PLATFORM_CTR)
  // バッファが空だとそもそも初期化されていないので止める
  if (!m_DrawResources[drawingMode].m_VertexDataArray)
  {
    GFL_ASSERT(m_DrawResources[drawingMode].m_VertexDataArray && "DrawUtil::Impl::BeginRender() doesn't work correctly. m_VertexDataArray is NULL.");
    return;
  }
  m_DrawResources[drawingMode].m_StartVertexIndex = m_DrawResources[drawingMode].m_VertexNum;
  u32 usedSize = m_DrawResources[drawingMode].m_DrawCommandContainer.size();
  for (u32 i = 0; i < usedSize; ++i)
  {// UniqueIDを比較して、先頭の要素から削除
    m_DrawResources[drawingMode].m_DrawCommandContainer.remove( m_DrawResources[drawingMode].m_DrawCommandContainer[0] );
  }
#endif
  m_DrawingMode = drawingMode;
  m_IsDrawing = true;
}

void DrawUtil::Impl::EndRender(DRAWING_MODE drawingMode)
{
  // 2回呼ばれていたら終了
  if (!m_IsDrawing)
  {
    GFL_ASSERT(m_IsDrawing && "DrawUtil::Impl::EndRender() is called twice.");
    return;
  }

  // 描画開始時と同じモードでなければ終了
  if (m_DrawingMode != drawingMode)
  {
    GFL_ASSERT((m_DrawingMode == drawingMode) && "DrawUtil::Impl::EndRender() doesn't work correctly. drawingMode is not same.");
    return;
  }

#if defined(GF_PLATFORM_CTR)
  // 頂点バッファが生成されていなければ終了(拡張機能を使用せず, drawingModeがExtensionの場合は有り得る)
  if (!m_Shaders[m_DrawingMode].m_pVertexShader)
  {
    GFL_ASSERT(m_Shaders[m_DrawingMode].m_pVertexShader && "DrawUtil::Impl::EndRender() doesn't work correctly. m_pVertexBuffer is NULL.");
    return;
  }

  // バッファが空だとそもそも初期化されていないので止める
  if (!m_DrawResources[drawingMode].m_VertexDataArray)
  {
    GFL_ASSERT(m_DrawResources[drawingMode].m_VertexDataArray && "DrawUtil::Impl::EndRender() doesn't work correctly. m_VertexDataArray is NULL.");
    return;
  }

  // 頂点バッファを更新し, 描画命令を実行.
  // 受け取ったフォーマットの頂点配列から頂点バッファを更新
  void* pRowVertexBuffer = m_DrawResources[drawingMode].m_pVertexBuffer->Lock();
  u32 vertexArrayIndex = m_DrawResources[drawingMode].m_PrimitiveMax * m_DrawResources[drawingMode].m_CurrentVertexBufferIndex;
  u32 VertexBufferIndex = vertexArrayIndex * sizeof(VertexFormat);
  void* pRowVertexBufferPointer = (void*)((u32)pRowVertexBuffer + VertexBufferIndex);
  memcpy(pRowVertexBufferPointer, &m_DrawResources[drawingMode].m_VertexDataArray[vertexArrayIndex], m_DrawResources[drawingMode].m_VertexNum * sizeof(VertexFormat));
  m_DrawResources[drawingMode].m_pVertexBuffer->UnLock();
  GFGL::SetVertexShader( m_Shaders[m_DrawingMode].m_pVertexShader );
  GFGL::SetPixelShader( m_Shaders[m_DrawingMode].m_pPixelShader );
  GFGL::SetIndices( m_pIndexBuffers[drawingMode] );
  GFGL::SetStreamSource(0, m_DrawResources[drawingMode].m_pVertexBuffer);

  u32 startIndex = m_DrawResources[drawingMode].m_StartVertexIndex;
  u32 startOffset = m_DrawResources[drawingMode].m_CurrentVertexBufferIndex * m_DrawResources[drawingMode].m_PrimitiveMax;
  for (u32 i = 0; i < m_DrawResources[drawingMode].m_DrawCommandContainer.size(); ++i)
  {
    m_ConstantVariables[m_DrawingMode].m_cbNeverChanges.Projection = m_DrawResources[drawingMode].m_DrawCommandContainer[i].m_ProjectionMatrix;
    m_ConstantVariables[m_DrawingMode].m_cbChangesEveryFrame.View = m_DrawResources[drawingMode].m_DrawCommandContainer[i].m_ViewMatrix;
    m_ConstantVariables[m_DrawingMode].m_cbChangesEveryObject.World = m_DrawResources[drawingMode].m_DrawCommandContainer[i].m_WorldMatrix;
    m_ConstantVariables[m_DrawingMode].m_cbMaterialColor.Color = m_DrawResources[drawingMode].m_DrawCommandContainer[i].m_MaterialColor;

    m_ConstantBuffers[m_DrawingMode].m_pNeverChanges->Update(&m_ConstantVariables[m_DrawingMode].m_cbNeverChanges, sizeof(cbNeverChanges));
    m_ConstantBuffers[m_DrawingMode].m_pChangesEveryFrame->Update(&m_ConstantVariables[m_DrawingMode].m_cbChangesEveryFrame, sizeof(cbChangesEveryFrame));
    m_ConstantBuffers[m_DrawingMode].m_pChangesEveryObject->Update(&m_ConstantVariables[m_DrawingMode].m_cbChangesEveryObject, sizeof(cbChangesEveryObject));
    m_ConstantBuffers[m_DrawingMode].m_pMaterialColor->Update(&m_ConstantVariables[m_DrawingMode].m_cbMaterialColor.Color, sizeof(cbMaterialColor));

    GFGL::SetVertexShaderConstant(CB_TYPES::NEVER_CHANGES, m_ConstantBuffers[m_DrawingMode].m_pNeverChanges);
    GFGL::SetVertexShaderConstant(CB_TYPES::CHANGES_EVERY_FRAME, m_ConstantBuffers[m_DrawingMode].m_pChangesEveryFrame);
    GFGL::SetVertexShaderConstant(CB_TYPES::CHANGES_EVERY_OBJECT, m_ConstantBuffers[m_DrawingMode].m_pChangesEveryObject);
    GFGL::SetVertexShaderConstant(CB_TYPES::MATERIAL_COLOR, m_ConstantBuffers[m_DrawingMode].m_pMaterialColor);

    GFGL::SetBlendState( m_StateObjects[drawingMode].m_pBlendStateObject );
    GFGL::SetRasterizerState( m_StateObjects[drawingMode].m_RasterizerStateObject );
    GFGL::SetDepthStencilState( m_StateObjects[drawingMode].m_DepthStencilStateObject );

    const gfl2::gfx::Texture* pTexture = m_DrawResources[drawingMode].m_DrawCommandContainer[i].m_pTexture;
    if (pTexture == NULL)
    {
      pTexture = m_Textures[drawingMode].m_pDummyTexture;
    }
    GFGL::SetTexture( 0, pTexture );
    GFGL::SetSamplerState( 0, m_StateObjects[drawingMode].m_SamplerStateObject );//SetTexture後に行う

    GFGL::DrawIndexedPrimitive(gfl2::gfx::PrimitiveType::Triangles, startIndex + startOffset, m_DrawResources[drawingMode].m_DrawCommandContainer[i].m_PrimitiveNum);

    // 描画終了後に, 次の描画開始インデックスを進めておく
    startIndex += m_DrawResources[drawingMode].m_DrawCommandContainer[i].m_PrimitiveNum;
  }
  m_DrawResources[drawingMode].m_StartVertexIndex = 0;
#endif
  m_IsDrawing = false;
}

void DrawUtil::Impl::SetCullMode(const CullMode cullMode)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_StateObjects[drawingMode].m_RasterizerStateObject->SetCullMode(cullMode);
  m_StateObjects[drawingMode].m_RasterizerStateObject->UpdateState();
}

void DrawUtil::Impl::SetFillMode(const FillMode cullMode)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_StateObjects[drawingMode].m_RasterizerStateObject->SetFillMode(cullMode);
  m_StateObjects[drawingMode].m_RasterizerStateObject->UpdateState();
}

void DrawUtil::Impl::SetProjectionMatrix(const math::Matrix44& mat)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_ConstantVariables[drawingMode].m_cbNeverChanges.Projection = mat;
#if defined(GF_PLATFORM_WIN32)
  m_ConstantBuffers[drawingMode].m_pNeverChanges->Update(&m_ConstantVariables[drawingMode].m_cbNeverChanges, sizeof(cbNeverChanges));
#endif
}

void DrawUtil::Impl::SetViewMatrix(const math::Matrix34& mat)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_ConstantVariables[drawingMode].m_cbChangesEveryFrame.View = mat;
#if defined(GF_PLATFORM_WIN32)
  m_ConstantBuffers[drawingMode].m_pChangesEveryFrame->Update(&m_ConstantVariables[drawingMode].m_cbChangesEveryFrame, sizeof(cbChangesEveryFrame));
#endif
}

void DrawUtil::Impl::SetModelMatrix(const math::Matrix44& mat)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_ConstantVariables[drawingMode].m_cbChangesEveryObject.World = mat;
#if defined(GF_PLATFORM_WIN32)
  m_ConstantBuffers[drawingMode].m_pChangesEveryObject->Update(&m_ConstantVariables[drawingMode].m_cbChangesEveryObject, sizeof(cbChangesEveryObject));
#endif
}

void DrawUtil::Impl::SetMaterialColor(const math::Vector4& color)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
	m_ConstantVariables[drawingMode].m_cbMaterialColor.Color = color;
#if defined(GF_PLATFORM_WIN32)
  m_ConstantBuffers[drawingMode].m_pMaterialColor->Update(&m_ConstantVariables[drawingMode].m_cbMaterialColor, sizeof(cbMaterialColor));
#endif
}

void DrawUtil::Impl::SetMaterialAlpha(const f32 alpha)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
	m_ConstantVariables[drawingMode].m_cbMaterialColor.Color.w = alpha;
#if defined(GF_PLATFORM_WIN32)
  m_ConstantBuffers[drawingMode].m_pMaterialColor->Update(&m_ConstantVariables[drawingMode].m_cbMaterialColor, sizeof(cbMaterialColor));
#endif
}

void DrawUtil::Impl::SetBlendTestState(
  b32 blendEnable,
  BlendOp blendOp,
  BlendFunc srcColorFunc,
  BlendFunc destColorFunc,
  BlendFunc srcAlphaFunc,
  BlendFunc destAlphaFunc,
  BlendStateObject::COLOR_WRITE_ENABLE colorWriteEnable)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_StateObjects[drawingMode].m_pBlendStateObject->SetBlendEnable(blendEnable);
  m_StateObjects[drawingMode].m_pBlendStateObject->SetBlendOp(blendOp);
  m_StateObjects[drawingMode].m_pBlendStateObject->SetBlendFunc(srcColorFunc, destColorFunc, srcAlphaFunc, destAlphaFunc);
  for (u32 i = 0; i < RenderTargetMax; ++i)
  {
    m_StateObjects[drawingMode].m_pBlendStateObject->SetRenderTargetWriteMask(i, colorWriteEnable);
  }
  m_StateObjects[drawingMode].m_pBlendStateObject->UpdateState();
}

void DrawUtil::Impl::SetBlendTestEnable(b32 enable)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_StateObjects[drawingMode].m_pBlendStateObject->SetBlendEnable(enable);
  m_StateObjects[drawingMode].m_pBlendStateObject->UpdateState();
}

void DrawUtil::Impl::SetTexture(const Texture* texture)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_Textures[drawingMode].m_pTexture = texture;
}

const Texture* DrawUtil::Impl::GetTexture()
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
	return m_Textures[drawingMode].m_pTexture;
}


void DrawUtil::Impl::DrawVertices(PrimitiveType primitiveType, const VertexFormat* pBuffer, u32 vertexNum, b32 enableTexture)
{
  if (pBuffer == NULL)
  {
    GFL_ASSERT((pBuffer != NULL) && "DrawUtil::Impl::DrawVertices() doesn't work correctly. pBuffer is not allowed NULL.");
    return;
  }

  if (!m_IsDrawing)
  {
    GFL_ASSERT(m_IsDrawing && "DrawUtil::Draw function is called before DrawUtil::BeginRender().");
    return;
  }

  if (vertexNum == 0)
  {
    return;
  }

  DRAWING_MODE drawingMode = m_DrawingMode;

#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;

  // プリミティブの種類に合わせてバッファのポインタを操作
  s32 writeVertexNum = 0;
  
  s32 index = 0;
  b32 isFinished = false;
  while (!isFinished)
  {
    writeVertexNum = CalculateDelimiter(primitiveType);
    if ((index + writeVertexNum) > static_cast<s32>(vertexNum))
    {// 総頂点数を超えていたら書き込む量をセーブ
      writeVertexNum = vertexNum - index;
      isFinished = true;
    }
    DrawPrimitive(primitiveType, &pBuffer[index], writeVertexNum, enableTexture);
    index += writeVertexNum;
    if (primitiveType == PrimitiveType::TriangleStrip)
    {
      index -= 2;
    }
    else if(primitiveType == PrimitiveType::LineStrip)
    {
      index -= 1;
    }
  }

#elif defined(GF_PLATFORM_CTR)
  // バッファが空では描画できないので止める
  if (!m_DrawResources[drawingMode].m_VertexDataArray)
  {
    GFL_ASSERT(m_DrawResources[drawingMode].m_VertexDataArray && "DrawUtil::Impl::DrawVertices() doesn't work correctly. m_VertexDataArray is NULL.");
    return;
  }

  // 描画可能頂点数をオーバーしている
  if (m_DrawResources[drawingMode].m_VertexNum + vertexNum > m_DrawResources[drawingMode].m_PrimitiveMax)
  {
    return;
  }

  memcpy(&m_DrawResources[drawingMode].m_VertexDataArray[m_DrawResources[drawingMode].m_CurrentVertexBufferIndex * m_DrawResources[drawingMode].m_PrimitiveMax + m_DrawResources[drawingMode].m_VertexNum], pBuffer, sizeof(VertexFormat) * vertexNum);
  // CTR用の描画コマンドを生成
  DrawCommand drawCommand;
  drawCommand.m_UniqueId = m_DrawResources[drawingMode].m_DrawCommandContainer.size(); // 重複しないIDを設定
  drawCommand.m_PrimitiveNum = vertexNum;
  drawCommand.m_ProjectionMatrix = m_ConstantVariables[drawingMode].m_cbNeverChanges.Projection;
  drawCommand.m_ViewMatrix[0] = m_ConstantVariables[drawingMode].m_cbChangesEveryFrame.View[0];
  drawCommand.m_ViewMatrix[1] = m_ConstantVariables[drawingMode].m_cbChangesEveryFrame.View[1];
  drawCommand.m_ViewMatrix[2] = m_ConstantVariables[drawingMode].m_cbChangesEveryFrame.View[2];
  drawCommand.m_WorldMatrix = m_ConstantVariables[drawingMode].m_cbChangesEveryObject.World;
  drawCommand.m_MaterialColor = m_ConstantVariables[drawingMode].m_cbMaterialColor.Color;
  drawCommand.m_pTexture = enableTexture ? m_Textures[drawingMode].m_pTexture : NULL;
  m_DrawResources[drawingMode].m_DrawCommandContainer.push_back(drawCommand);
  m_DrawResources[drawingMode].m_VertexNum += vertexNum;
#endif
}

#if defined(GF_PLATFORM_WIN32)
void DrawUtil::Impl::DrawPrimitive(PrimitiveType primitiveType, const VertexFormat* pBuffer, u32 vertexNum, b32 enableTexture)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
  drawingMode = DRAWING_MODE_NORMAL;

  for (u32 i = 0; i < vertexNum; ++i)
  {
    m_ConstantVariables[drawingMode].m_cbVertices.Position[i] = pBuffer[i].pos;
    m_ConstantVariables[drawingMode].m_cbVertices.Color[i] = pBuffer[i].color;
    m_ConstantVariables[drawingMode].m_cbVertices.Position[i].w = pBuffer[i].uv.x;
    m_ConstantVariables[drawingMode].m_cbVertices.Color[i].w = pBuffer[i].uv.y;
  }

  m_ConstantBuffers[drawingMode].m_pVertices->Update(&m_ConstantVariables[drawingMode].m_cbVertices, sizeof(cbVertices));

	Draw(primitiveType, vertexNum, enableTexture);
}
#endif

void DrawUtil::Impl::DrawRect(f32 x, f32 y, f32 w, f32 h, const math::Vector4& color, f32 u0, f32 v0, f32 u1, f32 v1)
{
	DrawRectangle(x, y, w, h, color, true, u0, v0, u1, v1);
}

void DrawUtil::Impl::FillRect(f32 x, f32 y, f32 w, f32 h, const math::Vector4& color)
{
	DrawRectangle(x, y, w, h, color, false);
}

void DrawUtil::Impl::DrawTriangles(const VertexFormat* pVertexArray, u32 vertexNum, b32 enableTexture)
{
  DrawVertices(gfl2::gfx::PrimitiveType::Triangles, pVertexArray, vertexNum, enableTexture);
}

#if defined(GF_PLATFORM_WIN32)
void DrawUtil::Impl::DrawLines(const VertexFormat* pVertexArray, u32 vertexNum, b32 enableTexture)
{
  DrawVertices(gfl2::gfx::PrimitiveType::Lines, pVertexArray, vertexNum, enableTexture);
}
#endif

void DrawUtil::Impl::DrawScreen(const math::Vector4& color, f32 u0, f32 v0, f32 u1, f32 v1)
{
  DrawRect(0.0f, 0.0f, m_ScreenWidth, m_ScreenHeight, color, u0, v0, u1, v1);
}

void DrawUtil::Impl::FillScreen(const math::Vector4& color)
{
	FillRect(0.0f, 0.0f, m_ScreenWidth, m_ScreenHeight, color);
}

void DrawUtil::Impl::DrawAABB(const math::Vector4& min, const math::Vector4& max, const math::Vector4& color)
{
	math::Vector4 uv = math::Vector4::GetZero();
	VertexFormat vertexFormat[] =
	{
		// 底面
		VertexFormat(math::Vector4(min.x, min.y, min.z), uv, color),	// [0]
		VertexFormat(math::Vector4(max.x, min.y, min.z), uv, color),	// [1]
		VertexFormat(math::Vector4(min.x, min.y, max.z), uv, color),	// [2]
		VertexFormat(math::Vector4(min.x, min.y, max.z), uv, color),	// [2]
		VertexFormat(math::Vector4(max.x, min.y, min.z), uv, color),	// [1]
		VertexFormat(math::Vector4(max.x, min.y, max.z), uv, color),	// [3]

		// 上面
		VertexFormat(math::Vector4(min.x, max.y, min.z), uv, color),	// [4]
		VertexFormat(math::Vector4(min.x, max.y, max.z), uv, color),	// [6]
		VertexFormat(math::Vector4(max.x, max.y, min.z), uv, color),	// [5]
		VertexFormat(math::Vector4(min.x, max.y, max.z), uv, color),	// [6]
		VertexFormat(math::Vector4(max.x, max.y, max.z), uv, color),	// [7]
		VertexFormat(math::Vector4(max.x, max.y, min.z), uv, color),	// [5]

		// 右面
		VertexFormat(math::Vector4(max.x, min.y, min.z), uv, color),	// [1]
		VertexFormat(math::Vector4(max.x, max.y, min.z), uv, color),	// [5]
		VertexFormat(math::Vector4(max.x, min.y, max.z), uv, color),	// [3]
		VertexFormat(math::Vector4(max.x, max.y, min.z), uv, color),	// [5]
		VertexFormat(math::Vector4(max.x, max.y, max.z), uv, color),	// [7]
		VertexFormat(math::Vector4(max.x, min.y, max.z), uv, color),	// [3]

		// 左面
		VertexFormat(math::Vector4(min.x, min.y, min.z), uv, color),	// [0]
		VertexFormat(math::Vector4(min.x, min.y, max.z), uv, color),	// [2]
		VertexFormat(math::Vector4(min.x, max.y, min.z), uv, color),	// [4]
		VertexFormat(math::Vector4(min.x, max.y, min.z), uv, color),	// [4]
		VertexFormat(math::Vector4(min.x, min.y, max.z), uv, color),	// [2]
		VertexFormat(math::Vector4(min.x, max.y, max.z), uv, color),	// [6]

		// 前面
		VertexFormat(math::Vector4(min.x, min.y, min.z), uv, color),	// [0]
		VertexFormat(math::Vector4(min.x, max.y, min.z), uv, color),	// [4]
		VertexFormat(math::Vector4(max.x, min.y, min.z), uv, color),	// [1]
		VertexFormat(math::Vector4(min.x, max.y, min.z), uv, color),	// [4]
		VertexFormat(math::Vector4(max.x, max.y, min.z), uv, color),	// [5]
		VertexFormat(math::Vector4(max.x, min.y, min.z), uv, color),	// [1]
		
		// 背面
		VertexFormat(math::Vector4(min.x, min.y, max.z), uv, color),	// [2]
		VertexFormat(math::Vector4(max.x, min.y, max.z), uv, color),	// [3]
		VertexFormat(math::Vector4(min.x, max.y, max.z), uv, color),	// [6]
		VertexFormat(math::Vector4(min.x, max.y, max.z), uv, color),	// [6]
		VertexFormat(math::Vector4(max.x, min.y, max.z), uv, color),	// [3]
		VertexFormat(math::Vector4(max.x, max.y, max.z), uv, color),	// [7]
	};

	DrawVertices(PrimitiveType::Triangles, vertexFormat, sizeof(vertexFormat) / sizeof(vertexFormat[0]), false);
}

void DrawUtil::Impl::DrawCircle(const math::Vector4& position, float radius, const math::Vector4& color)
{
  VertexFormat CircleVertices[CIRCLE_DIVISION_NUM * 3];

  // 円を描画する際の頂点配列の設定
  math::Vector4 uv = math::Vector4::GetZero();
  for (u32 i = 0; i < CIRCLE_DIVISION_NUM; ++i)
  {
    CircleVertices[3 * i + 0].pos =  position;
    CircleVertices[3 * i + 0].uv = uv;
    CircleVertices[3 * i + 0].color = color;

    // @fix CTR/Win32版で表示内容が違う問題を対処、Vector4の足し算でw値が加算されており、なおかつw成分の扱いがWin32とCtrで違っていた。w値が加算されないような計算に修正。
    f32 theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(CIRCLE_DIVISION_NUM);
    CircleVertices[3 * i + 1].pos.x = position.x + math::FCos(theta) * radius;
    CircleVertices[3 * i + 1].pos.z = position.z + math::FSin(theta) * radius;
    CircleVertices[3 * i + 1].pos.y = position.y;
    CircleVertices[3 * i + 1].pos.w = position.w;
      
    CircleVertices[3 * i + 1].uv = uv;
    CircleVertices[3 * i + 1].color = color;

    theta = 2.0f * PI * static_cast<float>(i + 1) / static_cast<float>(CIRCLE_DIVISION_NUM);
    CircleVertices[3 * i + 2].pos.x = position.x + math::FCos(theta) * radius;
    CircleVertices[3 * i + 2].pos.z = position.z + math::FSin(theta) * radius;
    CircleVertices[3 * i + 2].pos.y = position.y;
    CircleVertices[3 * i + 2].pos.w = position.w;

    CircleVertices[3 * i + 2].uv = uv;
    CircleVertices[3 * i + 2].color = color;
  }
  DrawVertices(PrimitiveType::Triangles, CircleVertices, CIRCLE_DIVISION_NUM * 3, false);
}

void DrawUtil::Impl::DrawTube(const math::Vector4& position, float radius, float height, const math::Vector4& color)
{
  math::Vector4 uv = math::Vector4::GetZero();
  const u32 length = CIRCLE_DIVISION_NUM * 6;
  VertexFormat vertexFormat[length];
  for (u32 i = 0; i < length; ++i)
  {
    vertexFormat[i] = VertexFormat(position, uv, color);
  }

  for (u32 i = 0; i < CIRCLE_DIVISION_NUM; ++i)
  {
    // 底面の座標を設定
    vertexFormat[6 * i + 4].pos.x = position.x + radius * math::FCos(2.0f * PI * i / (float)CIRCLE_DIVISION_NUM);
    vertexFormat[6 * i + 4].pos.y = position.y;
    vertexFormat[6 * i + 4].pos.z = position.z + radius * math::FSin(2.0f * PI * i / (float)CIRCLE_DIVISION_NUM);
    vertexFormat[6 * i + 5].pos.x = position.x + radius * math::FCos(2.0f * PI * (i + 1) / (float)CIRCLE_DIVISION_NUM);
    vertexFormat[6 * i + 5].pos.y = position.y + 0.0f;
    vertexFormat[6 * i + 5].pos.z = position.z + radius * math::FSin(2.0f * PI * (i + 1) / (float)CIRCLE_DIVISION_NUM);

    // 上面の座標を設定
    vertexFormat[6 * i + 0].pos.x = vertexFormat[6 * i + 4].pos.x;
    vertexFormat[6 * i + 0].pos.y = position.y + height;
    vertexFormat[6 * i + 0].pos.z = vertexFormat[6 * i + 4].pos.z;
    vertexFormat[6 * i + 1].pos.x = vertexFormat[6 * i + 5].pos.x;
    vertexFormat[6 * i + 1].pos.y = position.y + height;
    vertexFormat[6 * i + 1].pos.z = vertexFormat[6 * i + 5].pos.z;

    // 縮退を設定
    vertexFormat[6 * i + 2].pos = vertexFormat[6 * i + 4].pos;
    vertexFormat[6 * i + 3].pos = vertexFormat[6 * i + 1].pos;
  }

  DrawVertices(PrimitiveType::Triangles, vertexFormat, length, false);
}

void DrawUtil::Impl::DrawCylinder(const math::Vector4& position, float radius, float height, const math::Vector4& color)
{
  math::Vector4 topPosition = position;
  topPosition.y += height;
  // 底を描画
  DrawCircle(position, radius, color);
  DrawCircle(topPosition, radius, color);

  // 側面を描画
  DrawTube(position, radius, height, color);
}

/* 制作途中
void DrawUtil::Impl::DrawSphere(const math::Vector4& center, f32 radius, const math::Vector4& color)
{
	const u32 length = 16;
	VertexFormat vertexFormat[length * length];
	for (u32 i = 0; i < length; ++i)
	{
		for (u32 j = 0; j < length; ++j)
		{
			f32 theta = ((static_cast<f32>(i) - static_cast<f32>(length)) / static_cast<f32>(length - 1) * 360.0f);
			f32 phi = ((static_cast<f32>(j) - static_cast<f32>(length)) / static_cast<f32>(length - 1) * 360.0f);
			theta = math::ConvDegToRad(theta);
			phi = math::ConvDegToRad(phi);
			vertexFormat[i * length + j].pos.x = radius * math::FCos(phi) * math::FCos(theta);
			vertexFormat[i * length + j].pos.y = radius * math::FCos(phi) * math::FSin(theta);
			vertexFormat[i * length + j].pos.z = radius * math::FSin(phi);
			vertexFormat[i * length + j].color = color;
			vertexFormat[i * length + j].uv = math::Vector4::GetZero();
		}
	}

	DrawVertices(PrimitiveType::LineStrip, vertexFormat, sizeof(vertexFormat) / sizeof(vertexFormat[0]), false);
}
*/

void DrawUtil::Impl::ClearScissor(void)
{
	// シザー領域を画面サイズに戻しておく
	float displayWidth = static_cast<float>(GFGLUtil::GetDisplayWidthByDisplayMode( DisplayMode::HD720P ) );
	float displayHeight = static_cast<float>(GFGLUtil::GetDisplayHeightByDisplayMode( DisplayMode::HD720P ) );
  GFGL::SetScissor(0, 0, static_cast<u32>(displayWidth), static_cast<u32>(displayHeight));
}

void DrawUtil::Impl::SetScissor(u32 x, u32 y, u32 width, u32 height)
{
  GFGL::SetScissor(x, y, width, height);
}

void DrawUtil::Impl::SetDepthTestState(const gfl2::gfx::DepthStencilStateObject::StateData& stateData)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetDepthTestEnable(stateData.m_DepthTestEnable);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetDepthWriteEnable(stateData.m_DepthWriteEnable);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetDepthFunc(stateData.m_DepthFunc);
  // CTRでは表/裏の設定はできない. 表面に使われている設定を適用
  PolygonFace face = PolygonFace::CCW;
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilOp(face, stateData.m_StencilFailOp[face], stateData.m_StencilDepthFailOp[face], stateData.m_StencilPassOp[face]);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilFunc(face, stateData.m_StencilFunc[face], stateData.m_StencilRef, stateData.m_StencilMask);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilTestEnable(stateData.m_StencilTestEnable);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetStencilWriteMask(stateData.m_SetStencilWriteMask);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->UpdateState();
}

void DrawUtil::Impl::SetDepthTestEnable(b32 enable)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  m_StateObjects[drawingMode].m_DepthStencilStateObject->SetDepthTestEnable(enable);
  m_StateObjects[drawingMode].m_DepthStencilStateObject->UpdateState();
}

void DrawUtil::Impl::SetScreenSize(u32 width, u32 height)
{
  if (width == 0)
  {
    GFL_ASSERT((width > 0) && "DrawUtil::Impl::SetScreenSize() doesn't work correctly. 'width' should be larger than 0.");
    return;
  }

  if (height == 0)
  {
    GFL_ASSERT((height > 0) && "DrawUtil::Impl::SetScreenSize() doesn't work correctly. 'height' should be larger than 0.");
    return;
  }

  m_ScreenWidth = width;
  m_ScreenHeight = height;
}

u32 DrawUtil::Impl::GetScreenWidth()
{
  return m_ScreenWidth;
}

u32 DrawUtil::Impl::GetScreenHeight()
{
  return m_ScreenHeight;
}

u32 DrawUtil::Impl::GetPrimitiveMax(DRAWING_MODE drawingMode)
{
  u32 primitiveMax = 0;
#if defined(GF_PLATFORM_CTR)
  primitiveMax = m_DrawResources[drawingMode].m_PrimitiveMax;
#endif
  return primitiveMax;
}

u32 DrawUtil::Impl::GetDrawCommandMax(DRAWING_MODE drawingMode)
{
  u32 commandMax = 0;
#if defined(GF_PLATFORM_CTR)
  commandMax = m_DrawResources[drawingMode].m_DrawCommandMax;
#endif
  return commandMax;
}

void DrawUtil::Impl::SwapBuffers()
{
#if defined(GF_PLATFORM_CTR)
  for (u32 i = 0; i < NUMBER_OF_DRAWING_MODE; ++i)
  {
    m_DrawResources[i].m_CurrentVertexBufferIndex ^= 1;
    m_DrawResources[i].m_VertexNum = 0;
  }
#endif
}

void DrawUtil::Impl::DrawRectangle(f32 x, f32 y, f32 w, f32 h, math::Vector4 color, b32 enableTexture, f32 u0, f32 v0, f32 u1, f32 v1)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
#if defined(GF_PLATFORM_WIN32)
  drawingMode = DRAWING_MODE_NORMAL;
#endif
  b32 isRenderTargetTexture = false;
  if (m_Textures[drawingMode].m_pTexture)
  {
    const gfl2::gfx::Texture::TextureDesc& textureDesc = m_Textures[drawingMode].m_pTexture->GetTextureDesc();
    isRenderTargetTexture = (textureDesc.usage & gfl2::gfx::Usage::RenderTarget);
  }

  // WIN32版は定数バッファを更新して描画
  math::Matrix44 mtxProjection = m_ConstantVariables[drawingMode].m_cbNeverChanges.Projection;
  math::Matrix44 mtxView = m_ConstantVariables[drawingMode].m_cbChangesEveryFrame.View;
  math::Matrix44 mtxWorld = m_ConstantVariables[drawingMode].m_cbChangesEveryObject.World;
  math::Matrix44 matInvViewport = math::Matrix44::GetIdentity();
  math::Matrix34 view = math::Matrix34::GetIdentity();
  math::Matrix44 rotate = math::Matrix44::GetIdentity();
  matInvViewport.SetElem(0, 0, 2.0f / m_ScreenWidth);
  matInvViewport.SetElem(1, 1, -2.0f / m_ScreenHeight);
  matInvViewport.SetElem(0, 3, -1.0f);
  matInvViewport.SetElem(1, 3, 1.0f);

#if defined(GF_PLATFORM_WIN32)
  SetProjectionMatrix(matInvViewport);
  SetViewMatrix(view);
  SetModelMatrix(rotate);
#elif defined(GF_PLATFORM_CTR)
  if (!isRenderTargetTexture)
  {// レンターターゲットのテクスチャでなければY軸が左に向いているので回す
    rotate.SetElem(0, 0, 0.0f);
    rotate.SetElem(1, 0, -1.0f);
    rotate.SetElem(0, 1, 1.0f);
    rotate.SetElem(1, 1, 0.0f);
  }
  SetProjectionMatrix(rotate);
  SetViewMatrix(view);
  SetModelMatrix(matInvViewport);
#endif

  math::Vector4 uv[4]; 
#if defined(GF_PLATFORM_WIN32)
  uv[0] = math::Vector4(u0, v0, 0.0f, 0.0f);    // [0]
  uv[1] = math::Vector4(u0, v1, 0.0f, 0.0f);    // [1]
  uv[2] = math::Vector4(u1, v0, 0.0f, 0.0f);    // [2]
  uv[3] = math::Vector4(u1, v1, 0.0f, 0.0f);    // [3]

  if (isRenderTargetTexture)
  {// Win版のRenderTargetのテクスチャは上下が反転するので, 縦方向の符号を反転
    uv[0].y = -uv[0].y;
    uv[1].y = -uv[1].y;
    uv[2].y = -uv[2].y;
    uv[3].y = -uv[3].y;
  }
#elif defined(GF_PLATFORM_CTR)
  uv[0] = math::Vector4(u0, v1, 0.0f, 0.0f);    // [1]
  uv[1] = math::Vector4(u0, v0, 0.0f, 0.0f);    // [0]
  uv[2] = math::Vector4(u1, v1, 0.0f, 0.0f);    // [3]
  uv[3] = math::Vector4(u1, v0, 0.0f, 0.0f);    // [2]
#endif

  VertexFormat vertexFormat[] =
  {
    VertexFormat(	math::Vector4( x, y, 0.0f), uv[0], color),          // [0]
    VertexFormat(	math::Vector4( x, y + h, 0.0f), uv[1], color),      // [1]
    VertexFormat(	math::Vector4( x + w, y, 0.0f), uv[2], color),      // [2]
    VertexFormat(	math::Vector4( x + w, y, 0.0f), uv[2], color),      // [2]
    VertexFormat(	math::Vector4( x, y + h, 0.0f), uv[1], color),      // [1]
    VertexFormat(	math::Vector4( x + w, y + h, 0.0f), uv[3], color),  // [3]
	};

	DrawVertices(PrimitiveType::Triangles, vertexFormat, 6, enableTexture);

  // 2D描画が終わったら定数バッファを元に戻す
  SetProjectionMatrix(mtxProjection);
  gfl2::math::Matrix34 mtxView34;
  mtxView34[0] = mtxView[0];
  mtxView34[1] = mtxView[1];
  mtxView34[2] = mtxView[2];
  SetViewMatrix(mtxView34);
  SetModelMatrix(mtxWorld);
}

#if defined(GF_PLATFORM_WIN32)
bool DrawUtil::Impl::Draw(const PrimitiveType primitiveType, const u32 vertexNum, b32 enableTexture)
{
  DRAWING_MODE drawingMode = m_DrawingMode;
  drawingMode = DRAWING_MODE_NORMAL;

  GFGL::SetVertexShader( m_Shaders[drawingMode].m_pVertexShader );
  GFGL::SetVertexShaderConstant( 1, m_ConstantBuffers[drawingMode].m_pNeverChanges );
  GFGL::SetVertexShaderConstant( 2, m_ConstantBuffers[drawingMode].m_pChangesEveryFrame );
  GFGL::SetVertexShaderConstant( 3, m_ConstantBuffers[drawingMode].m_pChangesEveryObject );
  GFGL::SetVertexShaderConstant( 4, m_ConstantBuffers[drawingMode].m_pMaterialColor );
  GFGL::SetVertexShaderConstant( 5, m_ConstantBuffers[drawingMode].m_pVertices );

  GFGL::SetPixelShader( m_Shaders[drawingMode].m_pPixelShader );
  GFGL::SetPixelShaderConstant( 1, m_ConstantBuffers[drawingMode].m_pNeverChanges );
  GFGL::SetPixelShaderConstant( 2, m_ConstantBuffers[drawingMode].m_pChangesEveryFrame );
  GFGL::SetPixelShaderConstant( 3, m_ConstantBuffers[drawingMode].m_pChangesEveryObject );
  GFGL::SetPixelShaderConstant( 4, m_ConstantBuffers[drawingMode].m_pMaterialColor );
  GFGL::SetPixelShaderConstant( 5, m_ConstantBuffers[drawingMode].m_pVertices );

  GFGL::SetIndices( m_pIndexBuffers[drawingMode] );
  GFGL::SetBlendState( m_StateObjects[drawingMode].m_pBlendStateObject );
  GFGL::SetRasterizerState( m_StateObjects[drawingMode].m_RasterizerStateObject );
  GFGL::SetDepthStencilState( m_StateObjects[drawingMode].m_DepthStencilStateObject );
  GFGL::SetSamplerState( 0, m_StateObjects[drawingMode].m_SamplerStateObject );

  // テクスチャが設定されていなかったら, ダミーのテクスチャ(8x8, 白)を設定
  const Texture* pTexture = (enableTexture) ? m_Textures[drawingMode].m_pTexture : m_Textures[drawingMode].m_pDummyTexture;
  if (!pTexture)
  {
    pTexture = m_Textures[drawingMode].m_pDummyTexture;
  }
  GFGL::SetTexture( 0, pTexture );

  // ***************************************
  GFGL::DrawIndexedPrimitive( primitiveType, 0, vertexNum	);

  return true;
}
#endif

u32 DrawUtil::Impl::CalculateDelimiter(PrimitiveType primitiveType)
{
	switch(primitiveType)
	{
	case PrimitiveType::Points:
		return DrawUtil::Impl::VERTEX_NUM;
	case PrimitiveType::Lines:
		return (DrawUtil::Impl::VERTEX_NUM / 2) * 2;
	case PrimitiveType::LineStrip:
		return (DrawUtil::Impl::VERTEX_NUM / 2) * 2;
	case PrimitiveType::Triangles:
		return (DrawUtil::Impl::VERTEX_NUM / 3) * 3;
	case PrimitiveType::TriangleStrip:
		return DrawUtil::Impl::VERTEX_NUM;
	default:
		GFL_ASSERT(0 && "DrawUtil::Impl::CalculateDelimiter() doesn't work correctly. primitiveType is wrong.");
		return 0xFFFFFFFF;        // 異常終了
	}
}

void DrawUtil::Initialize(IGLAllocator* pAllocator, u32 screenWidth, u32 screenHeight, u32 primitiveNum, s32 drawCommandNum)
{
  if (s_pImpl)
  {
    GFL_ASSERT(!s_pImpl && "DrawUtil::Initialize() is called twice.");
    return;
  }

  GLMemory::SetInstanceAllocator( pAllocator );
	s_pImpl = new Impl(pAllocator, screenWidth, screenHeight, primitiveNum, drawCommandNum);
  GLMemory::ClearInstanceAllocator();
}

void DrawUtil::Terminate()
{
  if (!s_pImpl)
  {
    GFL_ASSERT(s_pImpl && "DrawUtil::Terminate() is called twice.");
    return;
  }

	delete s_pImpl;
	s_pImpl = NULL;
}

void DrawUtil::BeginRender(void)
{
  s_pImpl->BeginRender(DrawUtil::Impl::DRAWING_MODE_NORMAL);
}

void DrawUtil::EndRender(void)
{
	s_pImpl->EndRender(DrawUtil::Impl::DRAWING_MODE_NORMAL);
}

void DrawUtil::InitializeExtension(IGLAllocator* pAllocator, u32 primitiveNum, s32 drawCommandNum)
{
  s_pImpl->InitializeExtension(pAllocator, primitiveNum, drawCommandNum);
}

void DrawUtil::TerminateExtension(void)
{
  s_pImpl->TerminateExtension();
}

void DrawUtil::BeginRenderExtension(void)
{
  s_pImpl->BeginRender(DrawUtil::Impl::DRAWING_MODE_EXTENSION);
}

void DrawUtil::EndRenderExtension(void)
{
  s_pImpl->EndRender(DrawUtil::Impl::DRAWING_MODE_EXTENSION);
}

void DrawUtil::SetCullMode(const CullMode cullMode)
{
	s_pImpl->SetCullMode(cullMode);
}

void DrawUtil::SetFillMode(const FillMode fillMode)
{
	s_pImpl->SetFillMode(fillMode);
}

void DrawUtil::SetMaterialColor(const math::Vector4& color)
{
	s_pImpl->SetMaterialColor(color);
}

void DrawUtil::SetMaterialAlpha(const f32 alpha)
{
	s_pImpl->SetMaterialAlpha(alpha);
}

void DrawUtil::SetBlendTestState(
  b32 blendEnable,
  BlendOp blendOp,
  BlendFunc srcColorFunc,
  BlendFunc destColorFunc,
  BlendFunc srcAlphaFunc,
  BlendFunc destAlphaFunc,
  BlendStateObject::COLOR_WRITE_ENABLE colorWriteEnable)
{
  s_pImpl->SetBlendTestState(blendEnable, blendOp, srcColorFunc, destColorFunc, srcAlphaFunc, destAlphaFunc, colorWriteEnable);
}

void DrawUtil::SetBlendTestEnable(b32 enable)
{
  s_pImpl->SetBlendTestEnable(enable);
}

void DrawUtil::SetProjectionMatrix(const math::Matrix44& mat)
{
	s_pImpl->SetProjectionMatrix(mat);
}

void DrawUtil::SetViewMatrix(const math::Matrix34& mat)
{
	s_pImpl->SetViewMatrix(mat);
}

void DrawUtil::SetModelMatrix(const math::Matrix44& mat)
{
	s_pImpl->SetModelMatrix(mat);
}

void DrawUtil::SetTexture(const Texture* texture)
{
  s_pImpl->SetTexture(texture);
}

const Texture* DrawUtil::GetTexture()
{
  return s_pImpl->GetTexture();
}

void DrawUtil::DrawTriangles(const VertexFormat* pVertexArray, u32 vertexNum, b32 enableTexture)
{
	s_pImpl->DrawTriangles(pVertexArray, vertexNum, enableTexture);
}

void DrawUtil::DrawLines(const VertexFormat* pVertexArray, u32 vertexNum, b32 enableTexture)
{
#if defined(GF_PLATFORM_WIN32)
	s_pImpl->DrawLines(pVertexArray, vertexNum, enableTexture);
#endif
}

void DrawUtil::DrawRect(f32 x, f32 y, f32 w, f32 h, f32 u0, f32 v0, f32 u1, f32 v1)
{
	s_pImpl->DrawRect(x, y, w, h, math::Vector4(1.0f, 1.0f, 1.0f), u0, v0, u1, v1);
}

void DrawUtil::DrawRect(f32 x, f32 y, f32 w, f32 h, const math::Vector4& color, f32 u0, f32 v0, f32 u1, f32 v1)
{
	s_pImpl->DrawRect(x, y, w, h, color, u0, v0, u1, v1);
}

void DrawUtil::FillRect(f32 x, f32 y, f32 w, f32 h, const math::Vector4& color)
{
	s_pImpl->FillRect(x, y, w, h, color);
}

void DrawUtil::DrawScreen(f32 u0, f32 v0, f32 u1, f32 v1)
{
	s_pImpl->DrawScreen(math::Vector4(1.0f, 1.0f, 1.0f), u0, v0, u1, v1);
}

void DrawUtil::DrawScreen(const math::Vector4& color, f32 u0, f32 v0, f32 u1, f32 v1)
{
	s_pImpl->DrawScreen(color, u0, v0, u1, v1);
}

void DrawUtil::FillScreen(const math::Vector4& color)
{
	s_pImpl->FillScreen(color);
}

void DrawUtil::DrawAABB(const math::Vector4& min, const math::Vector4& max, const math::Vector4& color)
{
	s_pImpl->DrawAABB(min, max, color);
}

void DrawUtil::DrawAABB(const math::AABB& aabb, const math::Vector4& color)
{
	s_pImpl->DrawAABB(aabb.GetMin(), aabb.GetMax(), color);
}

void DrawUtil::DrawAABB(const math::Vector4& min, const math::Vector4& max)
{
	s_pImpl->DrawAABB(min, max, math::Vector4(0.0f, 0.0f, 0.0f));
}

void DrawUtil::DrawAABB(const math::AABB& aabb)
{
	s_pImpl->DrawAABB(aabb.GetMin(), aabb.GetMax(), math::Vector4(0.0f, 0.0f, 0.0f));
}

void DrawUtil::DrawCircle(const math::Vector4& position, float radius, const math::Vector4& color)
{
  s_pImpl->DrawCircle(position, radius, color);
}

void DrawUtil::DrawTube(const math::Vector4& position, float radius, float height, const math::Vector4& color)
{
  s_pImpl->DrawTube(position, radius, height, color);
}

void DrawUtil::DrawCylinder(const math::Vector4& position, float radius, float height, const math::Vector4& color)
{
  s_pImpl->DrawCylinder(position, radius, height, color);
}

//void DrawUtil::DrawSphere(const math::Vector4& center, f32 radius, const math::Vector4& color)
//{
//	s_pImpl->DrawSphere(center, radius, color);
//}

void DrawUtil::ClearScissor(void)
{
	s_pImpl->ClearScissor();
}

void DrawUtil::SetScissor(u32 x, u32 y, u32 width, u32 height)
{
	s_pImpl->SetScissor(x, y, width, height);
}

void DrawUtil::SetDepthTestState(const gfl2::gfx::DepthStencilStateObject::StateData& stateData)
{
  s_pImpl->SetDepthTestState(stateData);
}

void DrawUtil::SetDepthTestEnable(b32 enable)
{
  s_pImpl->SetDepthTestEnable(enable);
}

void DrawUtil::SetScreenSize(u32 width, u32 height)
{
  s_pImpl->SetScreenSize(width, height);
}

u32 DrawUtil::GetScreenWidth()
{
  return s_pImpl->GetScreenWidth();
}

u32 DrawUtil::GetScreenHeight()
{
  return s_pImpl->GetScreenHeight();
}

void DrawUtil::SwapBuffers()
{
  s_pImpl->SwapBuffers();
}

u32 DrawUtil::GetPrimitiveMax()
{
  return s_pImpl->GetPrimitiveMax(Impl::DRAWING_MODE_NORMAL);
}

u32 DrawUtil::GetPrimitiveMaxExtension()
{
  return s_pImpl->GetPrimitiveMax(Impl::DRAWING_MODE_EXTENSION);
}

u32 DrawUtil::GetDrawCommandMax()
{
  return s_pImpl->GetDrawCommandMax(Impl::DRAWING_MODE_NORMAL);
}

u32 DrawUtil::GetDrawCommandMaxExtension()
{
  return s_pImpl->GetDrawCommandMax(Impl::DRAWING_MODE_EXTENSION);
}

}}
