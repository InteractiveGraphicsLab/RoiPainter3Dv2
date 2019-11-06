#include "ModeVizMask.h"
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

bool ModeVizMask::CanLeaveMode()
{
  return true;
}



void ModeVizMask::StartMode()
{
  m_bL = m_bR = m_bM = false;
  formVisMask_Show();
  formVisMask_updateList();
}



///////////////////////////////////////////////////////////////////////////////
//Mouse Listeners//////////////////////////////////////////////////////////////
void ModeVizMask::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;
  
  if (IsCtrKeyOn())
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
    EVec3f cube = ImageCore::GetInst()->GetCuboid();
    CrssecCore::GetInst()->GenerateCurvedCrssec(cube, ogl->GetCamPos(), m_stroke);
  }
  m_bL = false;
  m_bDrawStr = false;
  
  ogl->BtnUp();
  FormMain_RedrawMainPanel();
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
  FormMain_RedrawMainPanel();
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
  FormMain_RedrawMainPanel();
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
  FormMain_RedrawMainPanel();
}


void ModeVizMask::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  if( !PickToMoveCrossSecByWheeling(p, ogl, z_delta ) )
  {
    ogl->ZoomCam(z_delta * 0.1f);
  }

  FormMain_RedrawMainPanel();
}


void ModeVizMask::KeyDown(int nChar) {}
void ModeVizMask::KeyUp(int nChar) {}






void ModeVizMask::DrawScene(
    const EVec3f &cuboid, 
    const EVec3f &cam_pos, 
    const EVec3f &cam_cnt)
{
  ImageCore::GetInst()->UpdateOGLMaskColorImg();
  const bool image_interpolation = formVisParam_doInterpolation();

  //bind volumes ---------------------------------------
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

  //Cross Section
  const EVec3i reso = ImageCore::GetInst()->GetResolution();
  const bool   b_xy = formVisParam_bPlaneXY();
  const bool   b_yz = formVisParam_bPlaneYZ();
  const bool   b_zx = FormVisParam_bPlaneZX();

  glColor3d(1, 1, 1);
  m_crssecShader.Bind(0, 1, 2, 3, 6, reso, false, true);
  CrssecCore::GetInst()->DrawCrssec( b_xy, b_yz, b_zx, cuboid);
  m_crssecShader.Unbind();
  
  //draw mask surface
  ImageCore::GetInst()->DrawMaskSurfaces();

  //Volume 
  if ( formVisParam_bRendVol() )
  {
    const bool   bPsuedo   = formVisParam_bDoPsued();
    const float  alpha     = formVisParam_getAlpha();
    const bool   isOnManip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int    sliceN = (int)((isOnManip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volumeShader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, cam_pos, bPsuedo, true);
    t_DrawCuboidSlices(sliceN, cam_pos, cam_cnt, cuboid);
    m_volumeShader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}

#pragma managed 

