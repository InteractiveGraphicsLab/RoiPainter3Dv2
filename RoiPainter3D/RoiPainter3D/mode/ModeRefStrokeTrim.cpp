
#include "ModeRefStrokeTrim.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"


using namespace RoiPainter3D;

#pragma unmanaged


ModeRefStrokeTrim::~ModeRefStrokeTrim()
{

}

ModeRefStrokeTrim::ModeRefStrokeTrim() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
}




bool ModeRefStrokeTrim::canEndMode()
{
  return true;
}

void ModeRefStrokeTrim::startMode()
{
}


void ModeRefStrokeTrim::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeRefStrokeTrim::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeRefStrokeTrim::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeRefStrokeTrim::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeRefStrokeTrim::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeRefStrokeTrim::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeRefStrokeTrim::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{


}


void ModeRefStrokeTrim::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeRefStrokeTrim::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeRefStrokeTrim::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeRefStrokeTrim::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
}



void ModeRefStrokeTrim::keyDown(int nChar) {}
void ModeRefStrokeTrim::keyUp(int nChar) {}


void ModeRefStrokeTrim::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
}



void ModeRefStrokeTrim::cancelSegmentation(){}
void ModeRefStrokeTrim::finishSegmentation(){}

#pragma managed