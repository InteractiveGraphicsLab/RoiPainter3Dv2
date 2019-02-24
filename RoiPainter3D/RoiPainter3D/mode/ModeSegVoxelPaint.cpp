
#include "ModeSegVoxelPaint.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegVoxelPaint.h"
#include "FormMaskIDselection.h"
#include "climessagebox.h"


using namespace RoiPainter3D;

#pragma unmanaged


//volFlg
//  0 : locked and never modified 
//  1 : background
// 255: forreground
ModeSegVoxelPaint::~ModeSegVoxelPaint()
{

}

ModeSegVoxelPaint::ModeSegVoxelPaint() :
	m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"   ),
	m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{
  m_bR = m_bL = m_bM = false;
  m_bDrawLasso = false;
  m_bPaintVoxel = false;
  m_bRefinmentMode = false;
}



bool ModeSegVoxelPaint::canEndMode()
{
  return true;
}



void ModeSegVoxelPaint::startMode()
{
  formSegVoxelPaint_Show();

  m_bR = m_bL = m_bM = false;
	m_bDrawLasso = m_bPaintVoxel = m_bRefinmentMode = false;

	const vector<MaskData> &mask = ImageCore::getInst()->m_maskData;
	const OglImage3D      &vMask = ImageCore::getInst()->m_volMsk;
	const EVec3i           r     = ImageCore::getInst()->getResolution();
	OglImage3D            &vFlg  = ImageCore::getInst()->m_volFlg;
	const int N = r[0] * r[1] * r[2];


  if(ModeCore::getInst()->getCurrentModeId() == MODE_SEG_VOXPAINT)
  {
    //segmentation mode
    for (int i = 0; i < N; ++i) vFlg[i] = (mask[vMask[i]].lock) ? 0 : 1;
  }
  else if(ModeCore::getInst()->getCurrentModeId() == MODE_REF_VOXPAINT )
  {
    //refinement mode
    m_bRefinmentMode = true;
    m_refineMaskId = formMaskIdSelection_showModalDialog();
    if( m_refineMaskId  == -1)
    {
      ModeCore::getInst()->ModeSwitch(MODE_VIS_MASK);
      return;
    }
    
    if( m_refineMaskId  == 0)
    {
      CLI_MessageBox_OK_Show("MASK_id = 0 is not editable here ", "message");
      ModeCore::getInst()->ModeSwitch(MODE_VIS_MASK);
      return;
    }

    for (int i = 0; i < N; ++i) 
      vFlg[i] = (vMask[i] == m_refineMaskId) ? 255 : 
                (mask[vMask[i]].lock       ) ?  0  : 1;
  }
  else
  {
    //never comes here 
    ModeCore::getInst()->ModeSwitch(MODE_VIS_NORMAL);
    return;
  }

	vFlg.SetUpdated();
  CrssecCore::getInst()->ClearCurvedCrossec();
}






void ModeSegVoxelPaint::finishSegmentation()
{
  const EVec3i res = ImageCore::getInst()->getResolution();
  const int    N   = res[0] * res[1] * res[2];
 
  const OglImage3D &vFlg  = ImageCore::getInst()->m_volFlg;
  OglImage3D &vMask = ImageCore::getInst()->m_volMsk;


  if ( m_bRefinmentMode )
  {
    //refinement mode
    for (int i = 0; i < N; ++i) {
      if     ( vFlg[i] == 255                               ) vMask[i] = m_refineMaskId ;
      else if( vFlg[i] == 1   && vMask[i] == m_refineMaskId ) vMask[i] = 0 ;

    }
    vMask.SetUpdated();
    ModeCore::getInst()->ModeSwitch( MODE_VIS_MASK );
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
    ImageCore::getInst()->mask_storeCurrentForeGround();
    ModeCore::getInst()->ModeSwitch( MODE_VIS_MASK );
  }
}

void ModeSegVoxelPaint::cancelSegmentation()
{
    ModeCore::getInst()->ModeSwitch( MODE_VIS_MASK );
    formMain_redrawMainPanel();
}














///////////////////////////////////////////////////////////////////////////////
//Functions for Lasso /////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


//compute angle of p1-p-p2 
static float c_calcAngle
(
	const EVec3f &p , 
	const EVec3f &p1, 
	const EVec3f &p2,
	const EVec3f &axis
) 
{ 
	EVec3f a = p1 - p;
	EVec3f b = p2 - p;
	float an = a.norm();
	float bn = b.norm();
	if (an == 0 || bn == 0) return 0;
	
	float cosTheta = std::min(1.0f, a.dot(b) / an / bn );
	return ( a.cross(b)).dot(axis) > 0 ? acos(cosTheta): -acos(cosTheta);
}


//íçñ⁄ÇµÇƒÇ¢ÇÈì_Ç™ lasso ÇÃì‡ë§Ç≈Ç†ÇÈÇ©îªíË
static bool c_isInsideLasso
(
	const EVec3f         &pos, 
	const vector<EVec3f> &lassoPosLst, 
	const CRSSEC_ID &id
) 
{ 

	EVec3f axis = 
		(id == CRSSEC_XY) ? EVec3f(0, 0, 1) :
		(id == CRSSEC_YZ) ? EVec3f(1, 0, 0) : EVec3f(0, 1, 0);

	float sumTheta = 0;

	for (auto itr = lassoPosLst.begin(); itr != lassoPosLst.end(); ++itr) 
	{
		auto nextItr = itr;
		++nextItr;
		if(nextItr == lassoPosLst.end()) nextItr = lassoPosLst.begin();
		sumTheta += c_calcAngle(pos, *itr, *nextItr, axis);
	}

	return fabs(2 * M_PI - fabs(sumTheta)) < M_PI * 0.5;
}



//lassoÇÃì‡ë§Ç…Ç†ÇÈvoxelÇ fore/backÇ…ïœçX
//bFore = true  --> vFlg 1 --> 255Ç…
//bFore = false --> vFlg 255 --> 1Ç…
static void t_addPixsInsideLasso
(
	const CRSSEC_ID      id      ,
	const EVec3i         reso    ,
	const EVec3f         pitch   ,
	const vector<EVec3f> &lassoPs,
	const bool           bFore   ,

   		  OglImage3D     &vFlg
)
{
	const int W = reso[0];
	const int H = reso[1];
	const int D = reso[2], WH = W*H;
  
  std::cout << "---- " << id << " " << W << " " << H << " " << D << std::endl;
  Trace(pitch);

	if (id == CRSSEC_XY) 
  {
		const float zPos = lassoPs.begin()->z();
		const int   zi   = (int)(zPos / pitch[2]);

#pragma omp parallel for
		for (int yi = 0; yi < H; ++yi)
		for (int xi = 0; xi < W; ++xi)
		{
      int idx = xi + yi * W + zi * WH;
      std::cout << "a";
      if( vFlg[idx] == 0 ) continue;
      std::cout << "b";
      
			EVec3f p((xi + 0.5f) * pitch[0], (yi + 0.5f) * pitch[1], zPos);

			if ( bFore ){
        if ( vFlg[idx] == 1  &&  c_isInsideLasso(p, lassoPs, id) ) vFlg[idx] = 255;
      }else{
        if ( vFlg[idx] ==255 &&  c_isInsideLasso(p, lassoPs, id) ) vFlg[idx] = 1  ;
      }
		}
	}
	
	if (id == CRSSEC_YZ) 
  {
		const float xPos = lassoPs.begin()->x();
		const int   xi   = (int)(xPos / pitch[0]);

#pragma omp parallel for
		for (int zi = 0; zi < D; ++zi)
		for (int yi = 0; yi < H; ++yi)
		{
      int idx = xi + yi * W + zi * WH;
      if( vFlg[idx] == 0 ) continue;
      
			EVec3f p(xPos, (yi + 0.5f) * pitch[1], (zi + 0.5f) * pitch[2]);

			if ( bFore ){
        if ( vFlg[idx] == 1  &&  c_isInsideLasso(p, lassoPs, id) ) vFlg[idx] = 255;
      }else{
        if ( vFlg[idx] ==255 &&  c_isInsideLasso(p, lassoPs, id) ) vFlg[idx] = 1  ;
		  }
	  }
  }

		
	if (id == CRSSEC_ZX) 
  {
		const float yPos = lassoPs.begin()->y();
		const int   yi   = (int)(yPos / pitch[1]);

#pragma omp parallel for
		for (int zi = 0; zi < D; ++zi)
		for (int xi = 0; xi < W; ++xi)
		{
      int idx = xi + yi * W + zi * WH;
      if( vFlg[idx] == 0 ) continue;
      
			EVec3f p((xi + 0.5f) * pitch[0], yPos, (zi + 0.5f) * pitch[2]);

			if ( bFore ){
        if ( vFlg[idx] == 1  &&  c_isInsideLasso(p, lassoPs, id) ) vFlg[idx] = 255;
      }else{
        if ( vFlg[idx] ==255 &&  c_isInsideLasso(p, lassoPs, id) ) vFlg[idx] = 1  ;
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
  m_paintVoxels.clear();
  m_lasso.clear();
		
	if ( isShiftKeyOn() )
	{
		if ( formSegVoxelPaint_bLassoMode() ) 
    {
      m_bDrawLasso  = true;
      EVec3f rayP, rayD, pos;
      ogl->GetCursorRay(p, rayP, rayD);
      m_lassoTrgtId = pickCrsSec(rayP, rayD, &pos);

      if(m_lassoTrgtId != CRSSEC_NON) m_bDrawLasso = true;
    }
		else 
    {
      m_bPaintVoxel = true;
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
  m_paintVoxels.clear();
  m_lasso.clear();
		
	if ( isShiftKeyOn() )
	{
		if ( formSegVoxelPaint_bLassoMode() ) 
    {
      m_bDrawLasso  = true;
      EVec3f rayP, rayD, pos;
      ogl->GetCursorRay(p, rayP, rayD);
      m_lassoTrgtId = pickCrsSec(rayP, rayD, &pos);

      if(m_lassoTrgtId != CRSSEC_NON) m_bDrawLasso = true;
    }
		else 
    {
      m_bPaintVoxel = true;
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
  OglImage3D  &vFlg = ImageCore::getInst()->m_volFlg;

	if (m_bPaintVoxel) 
	{
		for (auto itr : m_paintVoxels) if( vFlg[itr[3]] == 1 ) vFlg[itr[3]] = 255;
		vFlg.SetUpdated();
	}
	if (m_bDrawLasso)
	{
    const EVec3i reso  = ImageCore::getInst()->getResolution();
    const EVec3f pitch = ImageCore::getInst()->getPitch();
    const EVec3f cube  = ImageCore::getInst()->getCuboidF();

		t_addPixsInsideLasso( m_lassoTrgtId, reso, pitch, m_lasso, m_bL, vFlg);
		vFlg.SetUpdated();
	}

	m_lasso.clear();
	m_paintVoxels.clear();
	m_bL = m_bPaintVoxel = m_bDrawLasso = false;
	ogl->BtnUp();
	formMain_redrawMainPanel();
}


void ModeSegVoxelPaint::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  OglImage3D  &vFlg = ImageCore::getInst()->m_volFlg;

	if (m_bPaintVoxel) 
	{
		for (auto itr : m_paintVoxels) if( vFlg[itr[3]] == 255) vFlg[itr[3]] = 1;
		vFlg.SetUpdated();
	}
	if (m_bDrawLasso)
	{
    const EVec3i reso  = ImageCore::getInst()->getResolution();
    const EVec3f pitch = ImageCore::getInst()->getPitch();
    const EVec3f cube  = ImageCore::getInst()->getCuboidF();

		t_addPixsInsideLasso(m_lassoTrgtId, reso, pitch, m_lasso, m_bL, vFlg);
		vFlg.SetUpdated();
	}

	m_lasso.clear();
	m_paintVoxels.clear();
	m_bL = m_bPaintVoxel = m_bDrawLasso = false;
	ogl->BtnUp();
	formMain_redrawMainPanel();
}

void ModeSegVoxelPaint::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = false;
  ogl->BtnUp();
  formMain_redrawMainPanel();
}




void ModeSegVoxelPaint::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
  if (!m_bL && !m_bM && !m_bR) return;

	EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);

	const OglImage3D &vFlg = ImageCore::getInst()->m_volFlg    ;

	if ( m_bPaintVoxel )
	{
		if ( pickCrsSec(rayP, rayD, &pos) == CRSSEC_NON) return;
    EVec4i vi = ImageCore::getInst()->getVoxelIndex4i(pos);

		if( m_bL && vFlg[vi[3]] == 1 ) m_paintVoxels.push_back( vi );
		if(!m_bL && vFlg[vi[3]] ==255) m_paintVoxels.push_back( vi );
	}
	else if (m_bDrawLasso) 
	{
		if ( pickCrsSec_onlyTrgt( m_lassoTrgtId, rayP, rayD, &pos) != CRSSEC_NON){
			m_lasso.push_back(pos);
		}
	}
	else
	{
		ogl->MouseMove(p);
	}	
  formMain_redrawMainPanel();
}



void ModeSegVoxelPaint::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);
  
  CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);
  if( id != CRSSEC_NON ) CrssecCore::getInst()->MoveCrssec(ImageCore::getInst()->getResolution(), 
                                                           ImageCore::getInst()->getPitch(), id, zDelta);
  else ogl->ZoomCam(zDelta * 0.1f);

  formMain_redrawMainPanel();
}



void ModeSegVoxelPaint::LBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegVoxelPaint::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegVoxelPaint::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}



void ModeSegVoxelPaint::keyDown(int nChar) {
  formMain_redrawMainPanel();
}

void ModeSegVoxelPaint::keyUp(int nChar) {
  formMain_redrawMainPanel();
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



void ModeSegVoxelPaint::drawScene
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
  const EVec3i reso     = ImageCore::getInst()->getResolution();
  const EVec3f pitch    = ImageCore::getInst()->getPitch();

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
  

  //draw planes
  glColor3d(1, 1, 1);
  m_crssecShader.bind(0, 1, 2, 3, 6, reso, bGradMag, !isSpaceKeyOn());
  CrssecCore::getInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.unbind();

  
  if (bDrawVol && !isSpaceKeyOn())
  {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha, reso, camP, bPsuedo, !isSpaceKeyOn());
    t_drawSlices(sliceN, camP, camF, cuboid);
    m_volumeShader.unbind();
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
  }


	if ( m_bPaintVoxel ) t_drawCubes( m_paintVoxels, pitch, m_bL);
	
	if (m_bDrawLasso) {
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