
//=============================================================================
/**
 * @file   ModelViewerInFrame.h
 * @brief  枠の中におけるモデルビューア制御クラスヘッダ
 * @author 池田 哲平
 * @date   2012.10.12
 * @auther kawada_koji@gamefreak.co.jp
 * @date   2015.11.24
 */
//=============================================================================

#if !defined( __MODEL_VIEWER_IN_FRAME_H__ )
#define __MODEL_VIEWER_IN_FRAME_H__
#pragma once

#include <math/include/gfl2_math.h>
#include <math/include/gfl2_MathAABB.h>
#include <gfx/include/gfl2_DrawUtil.h>

#include "ModelInFrame.h"

namespace PokeTool
{
  class PokeModel;
}

namespace System { namespace Camera
{
  class InFrameCamera;
}
}

namespace System { namespace Camera { namespace test
{
  class PokemonInframeTool;
} } }

namespace System { namespace Camera
{
  // [niji注] このコメントはkujira時代のコメントそのままです。適宜niji環境に変えて読んで下さい。
  //-----------------------------------------------------------------------------
  /**
   * @brief 枠の中におけるモデルビューア制御クラス
   *
   * ・概要
   *   指定した枠の中にモデルを表示し、そのモデルの回転等を行うクラスです。
   *   どの位置に表示してもモデルの絵が変わらないように、常にカメラの中心でモデルを描画してから、
   *   ２次元空間レベルでずらす処理を行っているため、１モデル１カメラが絶対の仕様となります。
   *
   * ・初期化
   *   以下関数でモデルの初期化を行います。
   *   void Init(const EScreen eScreen, const int nLeft, const int nTop, const int nRight, const int nBottom, const ETurnDirection eTurnDir = TURN_DIRECTION_LEFT);
   *
   * ・モデル設定
   *   表示するモデルを設定します。パラメータの初期化等を行いますので、毎フレーム呼んではいけません。
   *   もしポインタが変わることが保証されるなら第２関数でもよいですが、万全を期するならば正しいタイミングで第１関数を呼ぶことがベストです。
   *   また表示するモデルがないときはNULLを設定するか、ResetModel()関数を呼んでください。
   *   void SetAndInitModel(poke_tool::PokeModel* pModel);
   *   void CheckAndSetAndInitModel(poke_tool::PokeModel* pModel);
   *   void ResetModel();
   *
   * ・フレーム処理
   *   枠のアニメーションを行うときは、枠設定関数を呼んでください。
   *   アニメーションを行わないのであれば、初期化時に既に設定されていますので呼ぶ必要はありません。
   *   void SetFrame(const int nLeft, const int nTop, const int nRight, const int nBottom);
   *
   *   フレーム処理関数です。ここでワールド(モデル)・ビュー・プロジェクションマトリクスが更新されます。
   *   void Update(gfl::grp::g3d::StereoCamera* pCamera);
   *
   *   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   *   !! 超重要　超重要　超重要　超重要 !!
   *   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   *   //------------------------------------------------------------------------------------------
   *   gfl::grp::g3d::SceneクラスのSubmitView(), SubmitViewOfEdge()関数は第２引数にデフォルト引数(true)を設定すると、
   *   ビュー・プロジェクションマトリクスが(間違った値で)再度更新されてしまいます。必ず第２引数は明示的にfalseを設定してください。
   *   void SubmitView(Camera* camera, bool update_camera_matrix = true, bool update_fog = true, s32 scene_draw_group_index = 0);
   *   void SubmitViewOfEdge(StereoCamera* camera, bool update_camera_matrix = true, const s32 group_index = SCENE_DRAW_GROUP_INDEX_NONE);
   *
   *   また同様の理由でgfl::grp::g3d::CameraクラスのUpdateCameraMatrix()関数を呼んではいけません。
   *   void UpdateCameraMatrix();
   *
   *   G3DUtilを使用している場合は、Update3D()関数の引数を変更する必要があります。
   *   上画面で当クラスを使用する場合は第２引数をfalseに、下画面で当クラスを使用する場合は第３引数をfalseに設定してください。
   *   void Update3D(bool do_calc = true, const bool bIsUpdateCameraMatUpper = true, const bool bIsUpdateCameraMatLower = true);
   *
   *   //------------------------------------------------------------------------------------------
   *
   *  ・影表示
   *    影を表示するときは当クラスへの設定は必要なく、[PokeModel]クラスを生成する時に引数に設定する
   *    [PokeModel::SetupOption]の[useShadow]データメンバを[true]に設定すれば影が表示されます。
   *
   *  ・デバッグ描画
   *    この関数を呼ぶようにするとデバッグメニューの[ぽけもんいんふれーむ]→[表示情報]を変更することで、枠や境界箱等のデバッグ情報が表示されるようになります。
   *    void DrawDebug(gfl::grp::GraphicsSystem* pGraphicsSystem, const gfl::grp::g3d::Camera* pCamera);
   */
  //-----------------------------------------------------------------------------
  class CModelViewerInFrame : public CModelInFrame
  {
  public:
    /// モデルを向ける方向列挙型
    enum ETurnDirection
    {
      TURN_DIRECTION_LEFT = 0 , ///< 左向き
      TURN_DIRECTION_RIGHT    , ///< 右向き
      TURN_DIRECTION_FREE     , ///< 自由
    };

    /// モデルを描画する位置列挙型
    enum EDrawPosition
    {
      DRAW_POSITION_BOTTOM = 0 , ///< 下
      DRAW_POSITION_CENTER     , ///< 真ん中
    };

  private:
    /// 中心オフセット取得段階列挙型
    enum ECenterOffsetGetPhase
    {
      CENTER_OFFSET_GET_PHASE_1 = 0 ,
      CENTER_OFFSET_GET_PHASE_2     ,
      CENTER_OFFSET_GET_PHASE_END   ,
    };

  private:
    static const float VIEW_ANGLE; ///< 画角
    static const float SCREEN_DISTANCE_DEFAULT; ///< カメラからスクリーンまでの距離デフォルト
    static const float CAMERA_PITCH_MAX;
    static const float CAMERA_PITCH_MIN;

  public:
    /// フレーム処理で取得するデータ構造体
    struct SUpdateData
    {
    public:
      gfl2::math::Matrix34 m_inViewMat;
      float m_fViewAngle;
      float m_fAspectRatio;
      gfl2::math::Vector2 m_inCenterPosi; ///< 枠の中心位置(プロジェクション空間)

    public:
      SUpdateData() : m_inViewMat(gfl2::math::Matrix34::GetIdentity()) , m_fViewAngle(VIEW_ANGLE) , m_fAspectRatio(1.0f) , m_inCenterPosi(gfl2::math::Vector2::GetZero()) { return ; }
    };

  private:
    EScreen m_eScreen; ///< 描画する画面
    ETurnDirection m_eTurnDir;  ///< 向ける方向
    EDrawPosition  m_eDrawPosi; ///< 描画する位置

    gfl2::math::VEC2 m_inBaseFramePosiLT; ///< 基準枠左上位置
    gfl2::math::VEC2 m_inBaseFramePosiRB; ///< 基準枠右下位置
    float m_fFrameScale; ///< 枠スケール
    float m_fFramePosiOfstRatioY; ///< 枠位置オフセット率Y

    PokeTool::PokeModel* m_pModel; ///< ぽけもでる
    float m_fModelYaw;      ///< ぽけもでるよー
    float m_fModelYawOfstR; ///< ぽけもでるよーおふせっと(右向き時限定)

    float m_fCameraYaw;   ///< カメラよー
    float m_fCameraPitch; ///< カメラぴっち

    b8    m_bIsULCD;         ///< 立体視を行うフラグ
    float m_fDepthRange;     ///< 立体具合の調整係数  // 立体視パラメータ(CCameraULCD::Setのパラメータ参照)
    float m_fDepthLevel;     ///< 液晶面に配置したい場所までのカメラからの仮想空間距離。カメラからスクリーンまでの距離。
    float m_fLimitParallax;  ///< 奥行き方向の限界視差(mm)
    bool m_bIsEnableDepthOffset; ///< 深度オフセット許可フラグ
    bool m_bIsTouchGround;       ///< 接地フラグ

    /// モデルの初期化時のSRT
    gfl2::math::VEC3 m_inModelInitScale;
    gfl2::math::VEC3 m_inModelInitRot;
    gfl2::math::VEC3 m_inModelInitPosi;
    gfl2::math::VEC3 m_inModelInitPosiOfst;

    float m_fCenterOfstY; ///< 中心オフセットY
    ECenterOffsetGetPhase m_eCenterOfstGetPhase; ///< 中心オフセット取得段階

    gfl2::math::VEC3 m_userPosOffset;   ///< ユーザが外部から指定する位置オフセット。(インフレームで出した値に更にこの値を加える。モデルに掛かっているスケールは一切考慮しない。)
    gfl2::math::VEC3 m_userScale;       ///< ユーザが外部から指定するスケール(インフレームで出した値に更にこの値を掛ける。)

    // 内部でモデルのVisibleを変更するので、モデルを表示するのかしないのかはこの変数で決める。
    bool m_modelVisible;

  public:
    CModelViewerInFrame(); ///< コンストラクタ

    //-----------------------------------------------------------------------------
    /**
     * @brief 初期化
     *
     * @param eScreen  描画する画面
     * @param nLeft    枠左上位置左
     * @param nTop     枠左上位置上
     * @param nRight   枠右下位置右
     * @param nBottom  枠右下位置下
     * @param eTurnDir モデルを向ける方向
     */
    //-----------------------------------------------------------------------------
    void Init(const EScreen eScreen, const int nLeft, const int nTop, const int nRight, const int nBottom, const ETurnDirection eTurnDir = TURN_DIRECTION_LEFT);

    //-----------------------------------------------------------------------------
    /**
     * @brief フレーム処理
     *
     * @param pCamera カメラ
     * @param pData   取得するデータ
     */
    //-----------------------------------------------------------------------------
    void Update(InFrameCamera* pCamera, SUpdateData* pData = NULL);

    //-----------------------------------------------------------------------------
    /**
     * @brief デバッグ描画
     *
     * @param pGraphicsSystem グラフィックスシステム
     * @param pCamera         カメラ
     */
    //-----------------------------------------------------------------------------
    void DrawDebug(gfl2::gfx::CtrDisplayNo displayNo, const InFrameCamera* pCamera);

    //-----------------------------------------------------------------------------
    /**
     * @brief モデルを設定して初期化します。
     *        描画するモデルがない時はNULLを設定してください。
     *        内部でパラメータ初期化等を行っているので、毎フレーム同じモデルを設定し続けてはいけません。
     *
     * @param pModel ぽけもでる
     */
    //-----------------------------------------------------------------------------
    void SetAndInitModel(PokeTool::PokeModel* pModel);

    //-----------------------------------------------------------------------------
    /**
     * @brief モデルが変わったかチェックしてから、設定して初期化します。
     *        ポインタチェックなので、モデルが変わってもポインタが変わらないときは正しい挙動を行いません。
     *
     * @param pModel ぽけもでる
     */
    //-----------------------------------------------------------------------------
    void CheckAndSetAndInitModel(PokeTool::PokeModel* pModel) { if( pModel != m_pModel ) SetAndInitModel(pModel); }

    //-----------------------------------------------------------------------------
    /**
     * @brief モデルの初期化時のSRTに戻します。
     */
    //-----------------------------------------------------------------------------
    void ReturnModelInitSRT();

    //-----------------------------------------------------------------------------
    /**
     * @brief モデルをリセットします。SetAndInitModel(NULL)と同義です。
     */
    //-----------------------------------------------------------------------------
    inline void ResetModel() { SetAndInitModel(NULL); }

    //-----------------------------------------------------------------------------
    /**
     * @brief 枠を(再)設定します。
     *        枠のアニメーションを行う時に使用します。
     *        特に枠がアニメーションしないのであれば、初期化時に設定するだけでよいです。
     *
     * @param nLeft   枠左上位置左
     * @param nTop    枠左上位置上
     * @param nRight  枠右下位置右
     * @param nBottom 枠右下位置下
     */
    //-----------------------------------------------------------------------------
    void SetFrame(const int nLeft, const int nTop, const int nRight, const int nBottom);

    void SetDrawPosition(const EDrawPosition eDrawPosi) { m_eDrawPosi = eDrawPosi; } ///< 描画する位置を設定します
    void SetULCDFlag(const bool bIsULCD) { m_bIsULCD = bIsULCD; } ///< 立体視を行うフラグを設定します(デフォルトはtrue)
    void SetULCDParam(const float fDepthRange, const float fDepthLevel, const float fLimitParallax);  ///< 立体視パラメータを設定する(CCameraULCD::Setのパラメータ参照)
    void GetULCDParam(float* fDepthRange, float* fDepthLevel, float* fLimitParallax) const;           ///< 立体視パラメータを取得する
    void SetEnableDepthOffsetFlag(const bool bIsEnable) { m_bIsEnableDepthOffset = bIsEnable; } ///< 深度オフセット許可フラグを設定します
    void SetTouchGroundFlag(const bool bIsTouchGround)  { m_bIsTouchGround = bIsTouchGround;  } ///< 接地フラグを設定します

    float GetCameraYaw()   const { return m_fCameraYaw;   } ///< カメラよー　を取得します(単位はラジアン)
    float GetCameraPitch() const { return m_fCameraPitch; } ///< カメラぴっちを取得します(単位はラジアン)

    void ResetCameraYawPitch(); ///< カメラよー・ぴっちをリセットします
    void SetCameraYaw  (const float fSetRadi); ///< カメラよー　を設定　します(単位はラジアン)
    void AddCameraYaw  (const float fAddRadi); ///< カメラよー　を加減算します(単位はラジアン)
    void SetCameraPitch(const float fSetRadi); ///< カメラぴっちを設定　します(単位はラジアン)
    void AddCameraPitch(const float fAddRadi); ///< カメラぴっちを加減算します(単位はラジアン)

    void SetUserPosOffset(const gfl2::math::Vector3& a_userPosOffset) { m_userPosOffset = gfl2::math::ConvertGfVec3ToNwVec3(a_userPosOffset); }
    void ResetUserPosOffset(void) { m_userPosOffset = gfl2::math::VEC3(0.0f,0.0f,0.0f); }
    gfl2::math::Vector3 GetUserPosOffset(void) const { return gfl2::math::ConvertNwVec3ToGfVec3(m_userPosOffset); }
    void SetUserScale(const gfl2::math::Vector3& a_userScale) { m_userScale = gfl2::math::ConvertGfVec3ToNwVec3(a_userScale); }
    void ResetUserScale(void) { m_userScale = gfl2::math::VEC3(1.0f,1.0f,1.0f); }
    gfl2::math::Vector3 GetUserScale(void) const { return gfl2::math::ConvertNwVec3ToGfVec3(m_userScale); }

    // 内部でモデルのVisibleを変更するので、モデルを表示するのかしないのかはこの関数で設定して下さい。
    void SetModelVisible(const bool visible);
    bool GetModelVisible(void) const { return m_modelVisible; }  // この関数を使うか、IsModelOnDisplayを使うか、きちんと使い分けて下さい。

    // モデルを画面上に表示中か否か(ユーザがm_modelVisibleにfalseを設定しているときfalse、モデルがないときfalse、モデルを非表示にしているときfalse)
    bool IsModelOnDisplay(void) const;

  private:
    inline float GetScale() const; ///< スケールを取得します
    void ResetModelInitSRT();
    void GetFrameData(float* pfAspectRatio, gfl2::math::VEC2* pFramePosiMax, gfl2::math::VEC2* pFramePosiMin, gfl2::math::VEC2* pFrameLenHalf, float* pfGroundHeight = NULL); ///< 枠に関するデータを取得します
    void GetFrameScreenPosition(gfl2::math::VEC2* pFramePosiLT, gfl2::math::VEC2* pFramePosiRB); ///< 枠のスクリーン位置を取得します
    bool GetBoundingAABBWorld(gfl2::math::AABB* pAABB); ///< ワールド空間基準の境界AABBを取得します
    void UpdateShadow(const float fGroundHeight, const gfl2::math::VEC3& inCameraPosi); ///< 影を更新します
    void GetCameraAndLookAtPosition(gfl2::math::VEC3* pCameraPosi, gfl2::math::VEC3* pLookAtPosi); ///< カメラ・注視点位置を取得します

#if PM_DEBUG
  /* PokemonInframeToolクラスが使用する関数 */
  private:
    void DrawFrame(gfl2::gfx::CtrDisplayNo displayNo, const bool bIsDrawAdjustFrame = true); ///< 枠を描画します
    void DrawAABB(gfl2::gfx::CtrDisplayNo displayNo, const InFrameCamera& inCamera); ///< AABBを描画します

    float GetFrameScale()                const { return m_fFrameScale; }
    float GetFramePositionOffsetRatioY() const { return m_fFramePosiOfstRatioY; }
    float GetModelYaw()                  const { return m_fModelYaw; }
    float GetModelYawOffsetR()           const { return m_fModelYawOfstR; }

    void SetFrameScale(const float fScale)                 { m_fFrameScale = fScale; }
    void SetFramePositionOffsetRatioY(const float fRatioY) { m_fFramePosiOfstRatioY = fRatioY; }
    void SetModelYaw(const float fYaw)                     { m_fModelYaw = fYaw; }

  private:
    void GetDisplayTypeArray(gfl2::gfx::CtrDisplayNo aDisplayNo[], int* pnNum); ///< ディスプレイタイプ配列を取得します
    bool IsDisplayNoTheSame(gfl2::gfx::CtrDisplayNo displayNo);   ///< 画面番号が同じか

  public:
    static int m_nDispInfo; ///< 表示情報(デバッグメニュー[ぽけもんいんふれーむ]で切り替えます)

  public:
    friend class System::Camera::test::PokemonInframeTool;
#endif

  public:
    //-----------------------------------------------------------------------------
    /**
     * @brief ワールド位置を取得します
     *
     * @param pWorldPosi 出力ワールド位置
     * @param fProjPosiX プロジェクション空間位置X(-1～1)
     * @param fProjPosiY プロジェクション空間位置Y(-1～1)
     * @param fViewPosiZ ビュー(カメラ)空間位置Z
     * @param inData     データ
     */
    //-----------------------------------------------------------------------------
    static void GetWorldPosition(gfl2::math::Vector3* pWorldPosi, const float fProjPosiX, const float fProjPosiY, const float fViewPosiZ, const SUpdateData& inData);
  };

} // namespace Camera
} // namespace System

#endif // __MODEL_VIEWER_IN_FRAME_H__

/*  EOF  */





