
#include "ModeVizNormal.h"

#include <iostream>
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"


using namespace RoiPainter3D;

#pragma unmanaged


ModeVizNormal::~ModeVizNormal()
{

}

ModeVizNormal::ModeVizNormal() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
  std::cout << "ModeVizNormal...\n";

  m_bL = m_bR = m_bM = false;
  m_bDrawStr = false;

  std::cout << "ModeVizNormal done\n";
}




bool ModeVizNormal::canEndMode()
{
  return true;
}

void ModeVizNormal::startMode()
{
  std::cout << "ModeVizNormal...startMode----------\n";

  m_bL = m_bR = m_bM = false;
  formVisNorm_Show();

  std::cout << "ModeVizNormal...startMode DONE-----\n";
}



void ModeVizNormal::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (isCtrKeyOn())
  {
    m_stroke.clear();
    m_bDrawStr = true;
  }
  ogl->BtnDown_Trans(p);
}


void ModeVizNormal::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_bDrawStr)
  {
    EVec3f cube = ImageCore::getInst()->getCuboidF();
    CrssecCore::getInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }

  m_bDrawStr = false;
  m_bL = false;
  ogl->BtnUp();
  formMain_redrawMainPanel();
}

void ModeVizNormal::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeVizNormal::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_redrawMainPanel();
}

void ModeVizNormal::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeVizNormal::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_redrawMainPanel();
}


void ModeVizNormal::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{


}


void ModeVizNormal::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizNormal::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeVizNormal::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);

  CRSSEC_ID id = pickCrsSec( rayP, rayD, &pos);
  if(id != CRSSEC_NON){
    short v = ImageCore::getInst()->getVoxelValue(pos);
    formVisNorm_setVoxelVis(v);
  }

  if (m_bDrawStr)
  {
    m_stroke.push_back(rayP + 0.1f * rayD);
    formMain_redrawMainPanel();
  }
  else if (m_bL || m_bR || m_bM)
  {
    ogl->MouseMove(p);
    formMain_redrawMainPanel();
  }
}


void ModeVizNormal::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);
  CRSSEC_ID id = pickCrsSec( rayP, rayD, &pos);

  EVec3i reso   = ImageCore::getInst()->getResolution();
  EVec3f pitch  = ImageCore::getInst()->getPitch();


  if (id != CRSSEC_NON) CrssecCore::getInst()->MoveCrssec(reso, pitch, id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);

  formMain_redrawMainPanel();
}



void ModeVizNormal::keyDown(int nChar) {}
void ModeVizNormal::keyUp(int nChar) {}









void ModeVizNormal::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const bool   bXY      = formVisParam_bPlaneXY();
  const bool   bYZ      = formVisParam_bPlaneYZ();
  const bool   bZX      = formVisParam_bPlaneZX();
  const bool   bDrawVol = formVisParam_bRendVol();
  const bool   bGradMag = formVisParam_bGradMag();
  const bool   bPsuedo  = formVisParam_bDoPsued();
  const float  alpha    = formVisParam_getAlpha();
  const EVec3i reso     = ImageCore::getInst()->getResolution();
  const bool isOnManip  = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
  const int  sliceN     = (int)((isOnManip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

  //bind volumes ---------------------------------------
  glActiveTextureARB(GL_TEXTURE0);
  ImageCore::getInst()->m_vol.BindOgl();
  glActiveTextureARB(GL_TEXTURE1);
  ImageCore::getInst()->m_volGmag.BindOgl();
  glActiveTextureARB(GL_TEXTURE2);
  ImageCore::getInst()->m_volFlg.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE3);
  ImageCore::getInst()->m_volMsk.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE4);
  formVisParam_bindTfImg();
  glActiveTextureARB(GL_TEXTURE5);
  formVisParam_bindPsuImg();
  glActiveTextureARB(GL_TEXTURE6);
  ImageCore::getInst()->m_imgMskCol.BindOgl(false);


  if (m_bDrawStr) t_drawLineStrip(EVec3f(1,1,0), 3, m_stroke);

  glColor3d(1, 1, 1);
  m_crssecShader.bind(0, 1, 2, 3, 6, reso, bGradMag, false);
  CrssecCore::getInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.unbind();

  if (bDrawVol)
  {
    glDisable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, false);
    t_drawSlices(sliceN, camP, camF, cuboid);
    m_volumeShader.unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }


}
    
#pragma managed