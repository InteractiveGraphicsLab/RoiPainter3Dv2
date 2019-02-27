#include "ModeVizMask.h"

#include <iostream>

#include "../FormVisMask.h"
#include "../FormVisParam.h"
#include "../FormMain.h"

#include "../ImageCore.h"
#include "../CrsSecCore.h"

#include ",./../COMMON/tmesh.h"
#include ",./../COMMON/tmarchingcubes.h"


#pragma unmanaged

using namespace RoiPainter3D;


ModeVizMask::ModeVizMask() :
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Msk.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Msk.glsl")
{

  std::cout << "ModeVizMask const...----------------------\n";
  m_bL = m_bR = m_bM = false;
  std::cout << "ModeVizMask DONE -------------------------\n";

}

ModeVizMask::~ModeVizMask()
{
}

bool ModeVizMask::canEndMode()
{
  return true;
}



void ModeVizMask::startMode()
{
  m_bL = m_bR = m_bM = false;
  formVisMask_Show();
  formVisMask_updateList();
}



//////////////////////////////////////////////////////////////////////////////////////
//Mouse Listeners////////////////////////////////////////////////////////////////////
void ModeVizMask::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
  
  if (isCtrKeyOn())
  {
    m_stroke.clear();
    m_bDrawStr = true;
  }

  ogl->BtnDown_Trans(p);
}

void ModeVizMask::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{

  if (m_bDrawStr)
  {
    EVec3f cube = ImageCore::getInst()->getCuboidF();
    CrssecCore::getInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }
  m_bL = false;
  m_bDrawStr = false;
  
  ogl->BtnUp();
  formMain_redrawMainPanel();
}

void ModeVizMask::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  ogl->BtnDown_Rot(p);
}

void ModeVizMask::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = false;
  ogl->BtnUp();
  formMain_redrawMainPanel();
}


void ModeVizMask::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeVizMask::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_redrawMainPanel();
}


void ModeVizMask::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeVizMask::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}

void ModeVizMask::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bR && !m_bM) return;

  if (m_bDrawStr)
  {
    EVec3f rayP, rayD, pos;
    ogl->GetCursorRay(p, rayP, rayD);
    m_stroke.push_back(rayP + 0.1f * rayD);
  }
  else{
    ogl->MouseMove(p);
  }
  formMain_redrawMainPanel();
}


void ModeVizMask::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);
  CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);

  EVec3i reso  = ImageCore::getInst()->getResolution();
  EVec3f pitch = ImageCore::getInst()->getPitch();

  if (id != CRSSEC_NON) CrssecCore::getInst()->MoveCrssec(reso, pitch, id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);

  formMain_redrawMainPanel();
}


void ModeVizMask::keyDown(int nChar) {}
void ModeVizMask::keyUp(int nChar) {}






void ModeVizMask::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const bool   bDrawVol  = formVisParam_bRendVol();
  const bool   bGradMag  = formVisParam_bGradMag();
  const bool   bPsuedo   = formVisParam_bDoPsued();
  const bool   bXY       = formVisParam_bPlaneXY();
  const bool   bYZ       = formVisParam_bPlaneYZ();
  const bool   bZX       = formVisParam_bPlaneZX();
  const float  alpha     = formVisParam_getAlpha();
  const EVec3i reso      = ImageCore::getInst()->getResolution();
  const bool   isOnManip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
  const int    sliceN = (int)((isOnManip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());


  //gen mask color id
  OglImage1D<CH_RGBA> &mskCol = ImageCore::getInst()->m_imgMskCol;
  mskCol.SetZero();
  for (int i = 0; i < (int)ImageCore::getInst()->m_maskData.size(); ++i)
  {
    mskCol[4 * i + 0] = ImageCore::getInst()->m_maskData[i].color[0];
    mskCol[4 * i + 1] = ImageCore::getInst()->m_maskData[i].color[1];
    mskCol[4 * i + 2] = ImageCore::getInst()->m_maskData[i].color[2];
    mskCol[4 * i + 3] = (int)(ImageCore::getInst()->m_maskData[i].alpha * 255);

    if (ImageCore::getInst()->m_maskData[i].bRendSurf) ImageCore::getInst()->m_imgMskCol[4 * i + 3] = 0;
  }
  ImageCore::getInst()->m_imgMskCol.SetUpdated();


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

  //Cross Section
  glColor3d(1, 1, 1);
  m_crssecShader.bind(0, 1, 2, 3, 6, reso, false, true);
  CrssecCore::getInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.unbind();

  
  //draw mask surface
  for (int i = 0; i < (int)ImageCore::getInst()->m_maskData.size(); ++i)
  {
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_LIGHT1 );
    glEnable( GL_LIGHT2 );
    glDisable(GL_CULL_FACE);
    glEnable( GL_BLEND );

    if( ImageCore::getInst()->m_maskData[i].bRendSurf )
    {
      float  a  = (float)ImageCore::getInst()->m_maskData[i].alpha;
      EVec3i cd = ImageCore::getInst()->m_maskData[i].color;
      EVec4f c( cd[0]/500.0f, cd[1]/500.0f, cd[2]/500.0f, a);
      static float spec[4] = {0.5f,0.5f,0.5f, 1};
      static float s[1]    = {64.0f};

      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , c.data() );
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , c.data() );
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec     );
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, s        );
      ImageCore::getInst()->m_maskData[i].surf.draw();
    }
    glDisable( GL_BLEND );
  }

  //Volume 
  if (bDrawVol)
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, true);
    t_drawSlices(sliceN, camP, camF, cuboid);
    m_volumeShader.unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}

#pragma managed 

