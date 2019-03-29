
#include "ModeSegVoxelPaint.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegVoxelPaint.h"
#include "FormMaskIDselection.h"
#include "climessagebox.h"

#include <algorithm>

using namespace RoiPainter3D;
using namespace std;

#pragma unmanaged


//volFlg
//  0 : locked and never modified 
//  1 : background
// 255: forreground
ModeSegVoxelPaint::~ModeSegVoxelPaint()
{

}


ModeSegVoxelPaint::ModeSegVoxelPaint() :
	m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"   ),
	m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_bR = m_bL = m_bM = false;
  m_b_refinementmode = false;
  m_b_lassomode = false;
  m_b_paintmode = false;
}


bool ModeSegVoxelPaint::CanLeaveMode()
{
  return true;
}


void ModeSegVoxelPaint::StartMode()
{
  formSegVoxelPaint_Show();

  m_bR = m_bL = m_bM = false;
  m_b_refinementmode = false;
	m_b_lassomode = false;
  m_b_paintmode = false;

	const vector<MaskData> &mask = ImageCore::GetInst()->m_mask_data;
	const OglImage3D &vol_mask = ImageCore::GetInst()->m_vol_mask;
	OglImage3D       &vol_flg  = ImageCore::GetInst()->m_vol_flag;
	const EVec3i      reso     = ImageCore::GetInst()->GetResolution();
	const int N = reso[0] * reso[1] * reso[2];


  if( ModeCore::GetInst()->GetCurrentModeId() == MODE_SEG_VOXPAINT)
  {
    //SEGMENTATION mode
    for (int i = 0; i < N; ++i) 
    {
      vol_flg[i] = (mask[vol_mask[i]].m_b_locked) ? 0 : 1;
    }
  }
  else if( ModeCore::GetInst()->GetCurrentModeId() == MODE_REF_VOXPAINT )
  {
    //REFINEMENT mode
    m_b_refinementmode = true;
    m_refine_maskid = formMaskIdSelection_showModalDialog();

    if( m_refine_maskid  == -1)
    {
      ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
      return;
    }
    if( m_refine_maskid  == 0)
    {
      CLI_MessageBox_OK_Show("MASK_id = 0 is not editable here ", "message");
      ModeCore::GetInst()->ModeSwitch(MODE_VIS_MASK);
      return;
    }

    for (int i = 0; i < N; ++i) 
    {
      vol_flg[i] = (vol_mask[i] == m_refine_maskid ) ? 255 : 
                   (mask[vol_mask[i]].m_b_locked   ) ?  0  : 1;
    
    }
  }
  else
  {
    //never comes here 
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_NORMAL );
    return;
  }

	vol_flg.SetUpdated();
  CrssecCore::GetInst()->ClearCurvedCrossec();
}






void ModeSegVoxelPaint::FinishSegmentation()
{
  const EVec3i res = ImageCore::GetInst()->GetResolution();
  const int    N   = res[0] * res[1] * res[2];
 
  const OglImage3D &vFlg  = ImageCore::GetInst()->m_vol_flag;
  OglImage3D &vMask = ImageCore::GetInst()->m_vol_mask;


  if ( m_b_refinementmode )
  {
    //refinement mode
    for (int i = 0; i < N; ++i) 
    {
      if ( vFlg[i] == 255 ){
        vMask[i] = m_refine_maskid ;
      }
      else if( vFlg[i] == 1 && vMask[i] == m_refine_maskid ) {
        vMask[i] = 0 ;
      }
    }
    vMask.SetUpdated();
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  }
  else 
  {
    //segmentation mode
    bool bForeExist = false;
    for (int i = 0; i < N; ++i) if ( vFlg[i] == 255)
    {
      bForeExist = true;
      break;
    }

    if (!bForeExist) 
    {
      CLI_MessageBox_OK_Show("No foreground pixel exist", "caution");
      return;
    }
    ImageCore::GetInst()->StoreForegroundAsNewMask();
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  }

}


void ModeSegVoxelPaint::CancelSegmentation()
{
    ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
    FormMain_RedrawMainPanel();
}





///////////////////////////////////////////////////////////////////////////////
//Functions for Lasso /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//íçñ⁄ÇµÇƒÇ¢ÇÈì_Ç™ lasso ÇÃì‡ë§Ç≈Ç†ÇÈÇ©îªíË
static bool t_IsInsideLasso
(
	const EVec3f         &pos, 
	const vector<EVec3f> &lasso_stroke, 
	const CRSSEC_ID      &id
) 
{ 
  const int N = (int)lasso_stroke.size();

	EVec3f axis = (id == CRSSEC_XY) ? EVec3f(0, 0, 1) :
		            (id == CRSSEC_YZ) ? EVec3f(1, 0, 0) : EVec3f(0, 1, 0);

	double sum = 0;
  for( int i=0; i < N; ++i)
  {
		sum += t_CalcAngle(lasso_stroke[ i ]-pos, lasso_stroke[ (i==N-1)?0:i+1 ]-pos, axis);
  }

	return fabs(2 * M_PI - fabs(sum)) < M_PI * 0.5;
}



//lassoÇÃì‡ë§Ç…Ç†ÇÈvoxelÇ fore/backÇ…ïœçX
//b_fore = true  --> vFlg 1   --> 255Ç…
//b_fore = false --> vFlg 255 --> 1Ç…
static void t_addPixsInsideLasso
(
	const CRSSEC_ID       id   ,
	const EVec3i          reso ,
	const EVec3f          pitch,
	const vector<EVec3f> &lasso_stroke,
	const bool            b_fore,

   		  OglImage3D     &vFlg
)
{
	const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2], WH = W*H;

  vector<EVec3f> lasso;
  const int new_num = std::max( 10, std::min( (int)lasso_stroke.size(), (int)( t_verts_Length(lasso_stroke, true)/pitch[0]) ));

  t_verts_ResampleEqualInterval( new_num, lasso_stroke, lasso);

  //compute bounding box
  EVec3f tmp_min, tmp_max;
  t_CalcBoundingBox( lasso, tmp_min, tmp_max);
  EVec3i bb_min ( (int) (tmp_min[0]/pitch[0] ), (int) (tmp_min[1]/pitch[1] ), (int) (tmp_min[2]/pitch[2] ) );
  EVec3i bb_max ( (int) (tmp_max[0]/pitch[0] ), (int) (tmp_max[1]/pitch[1] ), (int) (tmp_max[2]/pitch[2] ) );
  bb_min[0] = max( 0 ,bb_min[0] - 1);
  bb_min[1] = max( 0 ,bb_min[1] - 1);
  bb_min[2] = max( 0 ,bb_min[2] - 1);
  bb_max[0] = min(W-1,bb_max[0] + 1);
  bb_max[1] = min(H-1,bb_max[1] + 1);
  bb_max[2] = min(D-1,bb_max[2] + 1);
  
	if (id == CRSSEC_XY) 
  {
		const float zpos = lasso.front()[2];
		const int   zi   = (int)(zpos / pitch[2]);

#pragma omp parallel for
		for (int yi = bb_min[1]; yi < bb_max[1]; ++yi)
    {
		  for (int xi = bb_min[0]; xi < bb_max[0]; ++xi)
		  {
        int idx = xi + yi * W + zi * WH;
        if( vFlg[idx] == 0 ) continue;
			  if (  b_fore && vFlg[idx] ==255 ) continue;
			  if ( !b_fore && vFlg[idx] ==1   ) continue;
      
			  EVec3f p((xi + 0.5f) * pitch[0], (yi + 0.5f) * pitch[1], zpos);
        if ( t_IsInsideLasso(p, lasso, id) ) vFlg[idx] = b_fore ? 255 : 1;
		  }
    }
	}
	
	if (id == CRSSEC_YZ) 
  {
		const float xpos = lasso.front()[0];
		const int   xi   = (int)(xpos / pitch[0]);

#pragma omp parallel for
		for (int zi = bb_min[2]; zi < bb_max[2]; ++zi)
    {
		  for (int yi = bb_min[1]; yi < bb_max[1]; ++yi)
		  {
        int idx = xi + yi * W + zi * WH;
        if( vFlg[idx] == 0 ) continue;
        if (  b_fore && vFlg[idx] ==255 ) continue;
			  if ( !b_fore && vFlg[idx] ==1   ) continue;

			  EVec3f p(xpos, (yi + 0.5f) * pitch[1], (zi + 0.5f) * pitch[2]);
        if ( t_IsInsideLasso(p, lasso, id) ) vFlg[idx] = b_fore ? 255 : 1;
	    }
    }
  }

	if (id == CRSSEC_ZX) 
  {
		const float ypos = lasso.front()[1];
		const int   yi   = (int)(ypos / pitch[1]);

#pragma omp parallel for
		for (int zi = bb_min[2]; zi < bb_max[2]; ++zi)
    {
		  for (int xi = bb_min[0]; xi < bb_max[0]; ++xi)
		  {
        int idx = xi + yi * W + zi * WH;
        if( vFlg[idx] == 0 ) continue;
        if (  b_fore && vFlg[idx] ==255 ) continue;
			  if ( !b_fore && vFlg[idx] ==1   ) continue;
      
			  EVec3f p((xi + 0.5f) * pitch[0], ypos, (zi + 0.5f) * pitch[2]);
        if ( t_IsInsideLasso(p, lasso, id) ) vFlg[idx] = b_fore ? 255 : 1;
		  }
    }
	}
}








///////////////////////////////////////////////////////////////////////////////
//Mouse Listener //////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

void ModeSegVoxelPaint::LBtnDown( const EVec2i &p, OglForCLI *ogl)
{

  m_bL = true;
  m_paint_voxels.clear();
  m_lasso.clear();
		
	if ( IsShiftKeyOn() )
	{
		if ( formSegVoxelPaint_bLassoMode() ) 
    {
      m_b_lassomode  = true;
      EVec3f rayP, rayD, pos;
      ogl->GetCursorRay(p, rayP, rayD);
      m_trgt_crssecid = PickCrssec(rayP, rayD, &pos);

      if(m_trgt_crssecid != CRSSEC_NON) m_b_lassomode = true;
    }
		else 
    {
      m_b_paintmode = true;
    }
	}
	else
	{
		ogl->BtnDown_Trans( p );
	}
}


void ModeSegVoxelPaint::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bR = true;
  m_paint_voxels.clear();
  m_lasso.clear();
		
	if ( IsShiftKeyOn() )
	{
		if ( formSegVoxelPaint_bLassoMode() ) 
    {
      m_b_lassomode  = true;
      EVec3f rayP, rayD, pos;
      ogl->GetCursorRay(p, rayP, rayD);
      m_trgt_crssecid = PickCrssec(rayP, rayD, &pos);

      if(m_trgt_crssecid != CRSSEC_NON) m_b_lassomode = true;
    }
		else 
    {
      m_b_paintmode = true;
    }
	}
	else
	{
		ogl->BtnDown_Rot( p );
	}
}

void ModeSegVoxelPaint::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnDown_Zoom(p);
	m_bM = true;
}



void ModeSegVoxelPaint::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  OglImage3D  &vol_flg = ImageCore::GetInst()->m_vol_flag;

	if (m_b_paintmode) 
	{
		for (auto itr : m_paint_voxels) if( vol_flg[itr[3]] == 1 ) vol_flg[itr[3]] = 255;
		vol_flg.SetUpdated();
	}
	if (m_b_lassomode)
	{
    const EVec3i reso  = ImageCore::GetInst()->GetResolution();
    const EVec3f pitch = ImageCore::GetInst()->GetPitch();
    const EVec3f cube  = ImageCore::GetInst()->GetCuboid();

		t_addPixsInsideLasso( m_trgt_crssecid, reso, pitch, m_lasso, m_bL, vol_flg);
		vol_flg.SetUpdated();
	}

	m_lasso.clear();
	m_paint_voxels.clear();
	m_bL = m_b_paintmode = m_b_lassomode = false;
	ogl->BtnUp();
	FormMain_RedrawMainPanel();
}


void ModeSegVoxelPaint::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  OglImage3D  &vol_flg = ImageCore::GetInst()->m_vol_flag;

	if (m_b_paintmode) 
	{
		for (auto itr : m_paint_voxels) if( vol_flg[itr[3]] == 255) vol_flg[itr[3]] = 1;
		vol_flg.SetUpdated();
	}
	if (m_b_lassomode)
	{
    const EVec3i reso  = ImageCore::GetInst()->GetResolution();
    const EVec3f pitch = ImageCore::GetInst()->GetPitch();
    const EVec3f cube  = ImageCore::GetInst()->GetCuboid();

		t_addPixsInsideLasso(m_trgt_crssecid, reso, pitch, m_lasso, m_bL, vol_flg);
		vol_flg.SetUpdated();
	}

	m_lasso.clear();
	m_paint_voxels.clear();
	m_bL = m_b_paintmode = m_b_lassomode = false;
	ogl->BtnUp();
	FormMain_RedrawMainPanel();
}

void ModeSegVoxelPaint::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  FormMain_RedrawMainPanel();
}




void ModeSegVoxelPaint::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bM && !m_bR) return;

	EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay(p, ray_pos, ray_dir);

	const OglImage3D &vFlg = ImageCore::GetInst()->m_vol_flag    ;

	if ( m_b_paintmode )
	{
		if ( PickCrssec(ray_pos, ray_dir, &pos) == CRSSEC_NON) return;
    EVec4i vi = ImageCore::GetInst()->GetVoxelIndex4i(pos);

		if( m_bL && vFlg[vi[3]] == 1 ) m_paint_voxels.push_back( vi );
		if(!m_bL && vFlg[vi[3]] ==255) m_paint_voxels.push_back( vi );
	}
	else if (m_b_lassomode) 
	{
		if ( PickCrsSec( m_trgt_crssecid, ray_pos, ray_dir, &pos) != CRSSEC_NON){
			m_lasso.push_back(pos);
		}
	}
	else
	{
		ogl->MouseMove(p);
	}	
  FormMain_RedrawMainPanel();
}



void ModeSegVoxelPaint::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{
  EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay(p, ray_pos, ray_dir);
  
  CRSSEC_ID id = PickCrssec(ray_pos, ray_dir, &pos);
  if( id != CRSSEC_NON ) 
  {
    CrssecCore::GetInst()->MoveCrssec(ImageCore::GetInst()->GetResolution(), 
                                      ImageCore::GetInst()->GetPitch(), id, z_delta);
  }
  else
  {
    ogl->ZoomCam(z_delta * 0.1f);
  }

  FormMain_RedrawMainPanel();
}



void ModeSegVoxelPaint::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegVoxelPaint::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegVoxelPaint::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}



void ModeSegVoxelPaint::KeyDown(int nChar) {
  FormMain_RedrawMainPanel();
}

void ModeSegVoxelPaint::KeyUp(int nChar) {
  FormMain_RedrawMainPanel();
}







static void t_drawCubes
(
	const vector<EVec4i> &pixIds,
	const EVec3f &pitch,
	const bool   &tf
) 
{
	if( tf ) glColor3d(0.2, 0.8, 1.0);
	else     glColor3d(0.9, 0.9, 0.9);

	const float x = pitch[0];
	const float y = pitch[1];
	const float z = pitch[2];

	for (const auto itr : pixIds) 
	{
		glPushMatrix();
      glEnable(GL_LIGHTING);
			glTranslated(itr.x()*x,  itr.y()*y, itr.z()*z);
			glDisable(GL_CULL_FACE);
			glBegin(GL_QUADS);
				glNormal3f(-1,0, 0); glVertex3f(0, 0, 0); glVertex3f(0, 0, z); glVertex3f(0, y, z); glVertex3f(0, y, 0);
				glNormal3f(0,-1, 0); glVertex3f(0, 0, 0); glVertex3f(x, 0, 0); glVertex3f(x, 0, z); glVertex3f(0, 0, z);
				glNormal3f(0, 0,-1); glVertex3f(0, 0, 0); glVertex3f(0, y, 0); glVertex3f(x, y, 0); glVertex3f(y, 0, 0);
				glNormal3f(1, 0, 0); glVertex3f(x, 0, 0); glVertex3f(x, 0, z); glVertex3f(x, y, z); glVertex3f(x, y, 0);
				glNormal3f(0, 1, 0); glVertex3f(0, y, 0); glVertex3f(x, y, 0); glVertex3f(x, y, z); glVertex3f(0, y, z);
				glNormal3f(0, 0, 1); glVertex3f(0, 0, z); glVertex3f(0, y, z); glVertex3f(x, y, z); glVertex3f(y, 0, z);
			glEnd();
			glEnable(GL_CULL_FACE);
      glDisable(GL_LIGHTING);
		glPopMatrix();
	}
}



void ModeSegVoxelPaint::DrawScene
(
    const EVec3f &cuboid, 
    const EVec3f &camP  ,
    const EVec3f &camF
)
{
  const bool   bXY = formVisParam_bPlaneXY();
  const bool   bYZ = formVisParam_bPlaneYZ();
  const bool   bZX = formVisParam_bPlaneZX();
  const bool   bDrawVol = formVisParam_bRendVol();
  const bool   bGradMag = formVisParam_bGradMag();
  const bool   bPsuedo  = formVisParam_bDoPsued();
  const float  alpha    = formVisParam_getAlpha();
  const EVec3i reso     = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch    = ImageCore::GetInst()->GetPitch();

  const bool isOnManip  = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
  const int  sliceN     = (int)((isOnManip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

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
  

  //draw planes
  glColor3d(1, 1, 1);
  m_crssec_shader.Bind(0, 1, 2, 3, 6, reso, bGradMag, !IsSpaceKeyOn());
  CrssecCore::GetInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssec_shader.Unbind();

  
  if (bDrawVol && !IsSpaceKeyOn())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, !IsSpaceKeyOn());
    t_DrawCuboidSlices(sliceN, camP, camF, cuboid);
    m_volume_shader.Unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }


	if ( m_b_paintmode ) t_drawCubes( m_paint_voxels, pitch, m_bL);
	
	if (m_b_lassomode) {
		EVec3f ofset = (camP - camF).normalized() * 0.5;
		glLineWidth(2);
		glColor3d(0.2, 0.4, 1.0);
		
		glPushMatrix();
		glTranslated(ofset[0], ofset[1], ofset[2]);	
		glBegin(GL_LINE_LOOP);
		for (auto itr : m_lasso) glVertex3f(itr.x(), itr.y(), itr.z());
		glEnd();
		glPopMatrix();
	}
}




#pragma managed