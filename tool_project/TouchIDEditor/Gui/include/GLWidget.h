/* 
* File:   GLWidget.h
* Author: s-kimura
*
* Created on 2010/06/09, 18:04
* ウィジェット
*/

#ifndef GLWIDGET_H
#define	GLWIDGET_H

#include <gflib.h>
#include <rapidxml/include/gfl_CmdlXml.h>
#include <qt/Shader/ShaderObject.h>
#include "PaintUtil.h"

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

#define DECLARE_ENUM_BEGIN( CLASSNAME )\
class CLASSNAME {\
public:\
  enum Enum {
#define DECLARE_ENUM_END( CLASSNAME )\
        };\
};



class GLWidget : public gfl::qt::GLWidget {

  //-----------------------------------------------------------------------------
  //! レンダーターゲットタイプ
  DECLARE_ENUM_BEGIN( RT64Type )
  Main = 0,
    Depth,
    NumberOf
    DECLARE_ENUM_END( RT64Type )

  //-----------------------------------------------------------------------------
  //! レンダーターゲットタイプ
  DECLARE_ENUM_BEGIN( RTType )
  Painted = 0,
    Paint,
    MainCopy,
    Debug,
    NumberOf
    DECLARE_ENUM_END( RTType )

protected:
  virtual void initializeGL();
  virtual void resizeGL( int width, int height );
  virtual void Render( const GLenum mode );
  virtual void mouseMoveEvent( QMouseEvent *event );
  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);
  virtual void keyPressEvent( QKeyEvent * event );

public:
  GLWidget( QWidget *parent );
  virtual ~GLWidget( );

  enum {
    MODEL_ORG,
    MODEL_ID,
    MODEL_MAX
  };

  gfl::cmdl::Model* GetModel( s32 id = MODEL_ORG ) {
    return m_pModel[id];
  }
  gfl::motion::MotionBase* GetMotion( void ) {
    return m_pMotion;
  }

  void SetMotion( gfl::motion::MotionBase* Motion ) {
    this->m_pMotion = Motion;
  }

  void SetModel( gfl::cmdl::Model* Model, s32 id=MODEL_ORG ) {
    this->m_pModel[id] = Model;
  }

  void SetModelInstanceNode(gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstanceNode)
  {
    this->m_pModelInstanceNode = pModelInstanceNode;
  }

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* GetModelInstanceNode() const {
    return m_pModelInstanceNode;
  }

  void Clear();
  void RenderBoundingBox();

#define WAIT_LOAD
#ifdef WAIT_LOAD
  enum {
    RENDER_NONE,
    RENDER_NOW,
    RENDER_WAIT,
  };
  gfl::core::AtomicInteger GetRenderState( ) const {
    return m_RenderState;
  }

  void SetRenderState( s32 RenderState ) {
    this->m_RenderState = RenderState;
  }
#endif

  s32 m_ShowAllStep;

private:

  void setProjectionMatrix( gfl::core::Vector2 size );
  void mainRender( const GLenum mode );
  void uvRender();
  void drawModelUv( gfl::cmdl::Model *pModel, gfl::core::Matrix* mat );
  void recordMouseMove(QMouseEvent* event);
  void refreshWindows();

  gfl::cmdl::Model *m_pModel[MODEL_MAX];
  gfl::motion::MotionBase* m_pMotion;
  s32 m_Width;
  s32 m_Height;
#ifdef WAIT_LOAD
  gfl::core::AtomicInteger m_RenderState;
#endif

  gfl::core::Vector2 m_RTSize;
  GLuint m_FrameBuffer64;
  GLuint m_RenderBuffer64;
  GLuint m_RenderTexture64[RT64Type::NumberOf];
  GLuint m_FrameBuffer;
  GLuint m_RenderBuffer;
  GLuint m_RenderTexture[RTType::NumberOf];

  int m_RenderRTNo;

  std::vector<Gui::PaintUtil::FPoint> m_MousePoints;

  bool m_fIsPainted;
  bool m_fFlush;
  bool m_fPainedRTClear;
  bool m_fMouseReaveFlush;

  bool m_fOutPut;

  gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* m_pModelInstanceNode;
};

#undef DECLARE_ENUM_BEGIN
#undef DECLARE_ENUM_END

#endif	/* GLWIDGET_H */
