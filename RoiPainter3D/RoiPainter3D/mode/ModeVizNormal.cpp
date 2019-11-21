#include "ModeVizNormal.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"

#include <iostream>

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




bool ModeVizNormal::CanLeaveMode()
{
  return true;
}





void ModeVizNormal::StartMode()
{
  std::cout << "ModeVizNormal...startMode----------\n";

  m_bL = m_bR = m_bM = false;
  formVisNorm_Show();

  //Lock/Unlock pitch box 
  formVisParam_UnlockPitchBox();

  std::cout << "ModeVizNormal...startMode DONE-----\n";
}



void ModeVizNormal::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (IsCtrKeyOn())
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
    EVec3f cube = ImageCore::GetInst()->GetCuboid();
    CrssecCore::GetInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }

  m_bDrawStr = false;
  m_bL = false;
  ogl->BtnUp();
  FormMain_RedrawMainPanel();
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
  FormMain_RedrawMainPanel();
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
  FormMain_RedrawMainPanel();
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

  CRSSEC_ID id = PickCrssec( rayP, rayD, &pos);
  if(id != CRSSEC_NON){
    short v = ImageCore::GetInst()->GetVoxelValue(pos);
    formVisNorm_setVoxelVis(v);
  }

  if (m_bDrawStr)
  {
    m_stroke.push_back(rayP + 0.1f * rayD);
    FormMain_RedrawMainPanel();
  }
  else if (m_bL || m_bR || m_bM)
  {
    ogl->MouseMove(p);
    FormMain_RedrawMainPanel();
  }
}


void ModeVizNormal::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  if( !PickToMoveCrossSecByWheeling(p, ogl, zDelta ) )
  {
    ogl->ZoomCam(zDelta * 0.1f);
  }
  FormMain_RedrawMainPanel();
}


void ModeVizNormal::KeyDown(int nChar) {}
void ModeVizNormal::KeyUp(int nChar) {}

void ModeVizNormal::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  //bind volumes ---------------------------------------
  const bool image_interpolation = formVisParam_doInterpolation();

  glActiveTextureARB(GL_TEXTURE0);
  ImageCore::GetInst()->m_vol.BindOgl(image_interpolation);
  glActiveTextureARB(GL_TEXTURE1);
  ImageCore::GetInst()->m_vol_gm.BindOgl(image_interpolation);
  glActiveTextureARB(GL_TEXTURE2);
  ImageCore::GetInst()->m_vol_flag.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE3);
  ImageCore::GetInst()->m_vol_mask.BindOgl(false);
  glActiveTextureARB(GL_TEXTURE4);
  formVisParam_bindTfImg();
  glActiveTextureARB(GL_TEXTURE5);
  formVisParam_bindPsuImg();
  glActiveTextureARB(GL_TEXTURE6);
  ImageCore::GetInst()->m_img_maskcolor.BindOgl(false);


  if (m_bDrawStr) t_DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);

  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  const bool b_gm = formVisParam_bGradMag();
  const bool b_xy = formVisParam_bPlaneXY();
  const bool b_yz = formVisParam_bPlaneYZ();
  const bool b_zx = FormVisParam_bPlaneZX();

  glColor3d(1, 1, 1);
  m_crssecShader.Bind(0, 1, 2, 3, 6, reso, b_gm, false);
  CrssecCore::GetInst()->DrawCrssec(b_xy, b_yz, b_zx, cuboid);
  m_crssecShader.Unbind();


  const bool   b_rend_vol = formVisParam_bRendVol();

  if (b_rend_vol)
  {
    const bool   b_psuedo = formVisParam_bDoPsued();
    const float alpha     = formVisParam_getAlpha();
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   num_slice = (int)((b_onmanip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

    glDisable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    m_volumeShader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, b_psuedo, false);
    t_DrawCuboidSlices(num_slice, camP, camF, cuboid);
    m_volumeShader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }

}
    
#pragma managed