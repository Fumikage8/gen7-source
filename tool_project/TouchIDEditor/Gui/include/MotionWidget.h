//==============================================================================
/**
 * @file MotionWidget.h
 * @brief
 * @author kimura_shin
 * @data 2012/03/26, 12:13
 */
// =============================================================================

#if !defined( __MOTIONWIDGET_H__ )
#define __MOTIONWIDGET_H__

#include <gflib.h>
#include <rapidxml/include/gfl_CsklaXml.h>

#include <Animation/include/gfl2_AnimationController.h>

class GLAllocator;

class MotionWidget : public QGroupBox {
  Q_OBJECT

public:

  MotionWidget();
  ~MotionWidget();
  void Clear(){
    delete [] m_pId;
    delete [] m_pMatrix;
  }

  void SetMotionList();
  void CalcMatrix( const s32 frame );

  gfl::core::Matrix* GetMatrix(){
    return m_pMatrix;
  }

private:
  QComboBox* m_pMotionList;
  QSlider* m_pFrame;
  gfl2::animation::AnimationController m_AnimationController;
  gfl2::heap::HeapBase* m_pHeap;
  bool m_IsInitialized;
  gfl2::renderingengine::scenegraph::resource::ResourceNode* m_pCurrentResourceNode;
  s32* m_pId;
  gfl::core::Matrix *m_pMatrix;

  GLAllocator* allocator;

  public slots:
    void MotionChanged(const QString& fname);
    void FrameChanged(int);
};

#endif /* __MOTIONWIDGET_H__ */
