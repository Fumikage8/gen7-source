/* 
 * File:   ResourceConv.h
 * Author: makoto takebe
 *
 * Created on 2014/08/06
 */

#ifndef RESOURCE_CONV_H
#define	RESOURCE_CONV_H

#include "GLWidget.h"

#include <RenderingEngine/include/SceneGraph/Instance/gfl2_ModelInstanceNode.h>

class ResourceConv
{
public:

  static void ToModel(gfl::cmdl::Model* pModel, gfl2::renderingengine::scenegraph::instance::ModelInstanceNode* pModelInstance);
};

#endif
