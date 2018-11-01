#pragma once

#include "./COMMON/tmath.h"
#include "./COMMON/OglImage.h"


#pragma unmanaged 

class ViewAngleCore
{
  OGLImage2D4 m_texture;

  ViewAngleCore();
public:
  ~ViewAngleCore();

  static ViewAngleCore *getInst()
  {
    static ViewAngleCore p;
    return &p;
  }

  void drawIndicator(int curViewW, int curViewH, EVec3f camP, EVec3f camC, EVec3f camY);

  //return ID of plane 0:non, 1:A, 2:R, 3:P, 4:L, 5:T, 6:B  
  int  pickIndicator(const OglForCLI &ogl, int curViewW, int curViewH, EVec3f camP, EVec3f camC, EVec3f camY, EVec2i p);
};

#pragma managed 
