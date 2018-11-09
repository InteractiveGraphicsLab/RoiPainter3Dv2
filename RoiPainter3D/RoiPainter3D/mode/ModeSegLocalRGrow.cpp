
#include "ModeSegLocalRGrow.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"


using namespace RoiPainter3D;

#pragma unmanaged


ModeSegLocalRGrow::~ModeSegLocalRGrow()
{

}

ModeSegLocalRGrow::ModeSegLocalRGrow() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
}




bool ModeSegLocalRGrow::canEndMode()
{
  return true;
}

void ModeSegLocalRGrow::startMode()
{
}


void ModeSegLocalRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegLocalRGrow::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegLocalRGrow::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegLocalRGrow::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegLocalRGrow::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegLocalRGrow::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegLocalRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{


}


void ModeSegLocalRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegLocalRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegLocalRGrow::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegLocalRGrow::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
}



void ModeSegLocalRGrow::keyDown(int nChar) {}
void ModeSegLocalRGrow::keyUp(int nChar) {}


void ModeSegLocalRGrow::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
}

#pragma managed