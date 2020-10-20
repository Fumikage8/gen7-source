#ifndef GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_JOINT_ANIMATION_RESOURCE_NODE_FACTORY_H_INCLUDED
#define GFLIB2_RENDERINGENGINE_CLR_SCENEGRAPH_RESOURCE_JOINT_ANIMATION_RESOURCE_NODE_FACTORY_H_INCLUDED


namespace gfl2 { namespace clr { namespace renderingengine { namespace scenegraph { namespace resource {

public ref class JointAnimationResourceNodeFactory
{
public:

  JointAnimationResourceNodeFactory()
  {
  }

  /**
   * @brief Originノードの移動量を０にする
   */
  void SetOriginTranslationZero(bool flag)
  {
    //m_pNative->SetOriginTranslationZero(flag);
  }

  /**
   * @brief Originノードの移動量を０にする
   */
  bool GetOriginTranslationZero()
  {
    //return m_pNative->GetOriginTranslationZero() != 0;
    return false;
  }

private:

  //gfl2::renderingengine::scenegraph::resource::JointAnimationResourceNodeFactory* m_pNative;
};

}}}}}

#endif