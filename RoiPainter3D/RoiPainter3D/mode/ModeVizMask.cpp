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



//////////////////////////////////////////////////////////////////////////////////////
//Mouse Listeners////////////////////////////////////////////////////////////////////
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


void ModeVizMask::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
  ogl->GetCursorRay(p, rayP, rayD);
  CRSSEC_ID id = PickCrssec(rayP, rayD, &pos);

  EVec3i reso  = ImageCore::GetInst()->GetResolution();
  EVec3f pitch = ImageCore::GetInst()->GetPitch();

  if (id != CRSSEC_NON) CrssecCore::GetInst()->MoveCrssec(reso, pitch, id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);

  FormMain_RedrawMainPanel();
}


void ModeVizMask::KeyDown(int nChar) {}
void ModeVizMask::KeyUp(int nChar) {}






void ModeVizMask::DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
  const bool   bDrawVol  = formVisParam_bRendVol();
  const bool   bGradMag  = formVisParam_bGradMag();
  const bool   bPsuedo   = formVisParam_bDoPsued();
  const bool   bXY       = formVisParam_bPlaneXY();
  const bool   bYZ       = formVisParam_bPlaneYZ();
  const bool   bZX       = formVisParam_bPlaneZX();
  const float  alpha     = formVisParam_getAlpha();
  const EVec3i reso      = ImageCore::GetInst()->GetResolution();
  const bool   isOnManip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
  const int    sliceN = (int)((isOnManip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());


  //gen mask color id
  OglImage1D<CH_RGBA> &mskCol = ImageCore::GetInst()->m_img_maskcolor;
  mskCol.SetZero();
  for (int i = 0; i < (int)ImageCore::GetInst()->m_mask_data.size(); ++i)
  {
    mskCol[4 * i + 0] = ImageCore::GetInst()->m_mask_data[i].m_color[0];
    mskCol[4 * i + 1] = ImageCore::GetInst()->m_mask_data[i].m_color[1];
    mskCol[4 * i + 2] = ImageCore::GetInst()->m_mask_data[i].m_color[2];
    mskCol[4 * i + 3] = (int)(ImageCore::GetInst()->m_mask_data[i].m_alpha * 255);

    if (ImageCore::GetInst()->m_mask_data[i].m_b_drawsurface) ImageCore::GetInst()->m_img_maskcolor[4 * i + 3] = 0;
  }
  ImageCore::GetInst()->m_img_maskcolor.SetUpdated();


  //bind volumes ---------------------------------------
  glActiveTextureARB(GL_TEXTURE0);
  ImageCore::GetInst()->m_vol.BindOgl();
  glActiveTextureARB(GL_TEXTURE1);
  ImageCore::GetInst()->m_vol_gm.BindOgl();
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
  glColor3d(1, 1, 1);
  m_crssecShader.Bind(0, 1, 2, 3, 6, reso, false, true);
  CrssecCore::GetInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.Unbind();

  
  //draw mask surface
  for (int i = 0; i < (int)ImageCore::GetInst()->m_mask_data.size(); ++i)
  {
    glEnable( GL_LIGHTING );
    glEnable( GL_LIGHT0 );
    glEnable( GL_LIGHT1 );
    glEnable( GL_LIGHT2 );
    glDisable(GL_CULL_FACE);
    glEnable( GL_BLEND );

    if( ImageCore::GetInst()->m_mask_data[i].m_b_drawsurface )
    {
      float  a  = (float)ImageCore::GetInst()->m_mask_data[i].m_alpha;
      EVec3i cd = ImageCore::GetInst()->m_mask_data[i].m_color;
      EVec4f c( cd[0]/500.0f, cd[1]/500.0f, cd[2]/500.0f, a);
      static float spec[4] = {0.5f,0.5f,0.5f, 1};
      static float s[1]    = {64.0f};

      glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , c.data() );
      glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , c.data() );
      glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec     );
      glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, s        );
      ImageCore::GetInst()->m_mask_data[i].m_surface.draw();
    }
    glDisable( GL_BLEND );
  }

  //Volume 
  if (bDrawVol)
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volumeShader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, true);
    t_DrawCuboidSlices(sliceN, camP, camF, cuboid);
    m_volumeShader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }
}

#pragma managed 

