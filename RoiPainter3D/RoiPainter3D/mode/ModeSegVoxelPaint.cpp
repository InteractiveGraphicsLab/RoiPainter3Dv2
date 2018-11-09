
#include "ModeSegVoxelPaint.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormVisNorm.h"


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
  m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg.glsl"),
  m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg.glsl")
{
}




bool ModeSegVoxelPaint::canEndMode()
{
  return true;
}

void ModeSegVoxelPaint::startMode()
{
}


void ModeSegVoxelPaint::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegVoxelPaint::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegVoxelPaint::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegVoxelPaint::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegVoxelPaint::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
}

void ModeSegVoxelPaint::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegVoxelPaint::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{


}


void ModeSegVoxelPaint::RBtnDclk(const EVec2i &p, OglForCLI *ogl) {}
void ModeSegVoxelPaint::MBtnDclk(const EVec2i &p, OglForCLI *ogl) {}


void ModeSegVoxelPaint::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegVoxelPaint::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
}



void ModeSegVoxelPaint::keyDown(int nChar) {}
void ModeSegVoxelPaint::keyUp(int nChar) {}


void ModeSegVoxelPaint::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
{
}




todo 以下の実装を上にコピー
refinement modeへの対応も行う（start とfinish以外は同じでok）


/*


ModeSegPixPaint::ModeSegPixPaint() : 
	m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_Seg.glsl"   ),
	m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_Seg.glsl")
{

	m_bR = m_bL = m_bM = false;
	m_dlg.Create(IDD_DIALOG_SEG_PIXPAINT);

}


ModeSegPixPaint::~ModeSegPixPaint()
{

}


void ModeSegPixPaint::FinishSegmentation()
{

 const EVec3i res = TexCore::getInst()->getResolution();
	const int    N = res[0] * res[1] * res[2];

	bool bForeExist = false;
	for (int i = 0; i < N; ++i)
	{
		if ( TexCore::getInst()->m_volFlg[i] == 255)
		{
			bForeExist = true;
			break;
		}
	}

	if (!bForeExist) 
	{
		AfxMessageBox("No foreground pixel exist\n");
		return;
	}


	TexCore::getInst()->mask_storeCurrentForeGround();
	ModeCore::getInst()->SwitchMode( MODE_VIS_MASK );

}

void ModeSegPixPaint::startMode() 
{
	m_dlg.ShowWindow( SW_SHOW );

	m_bR = m_bL = m_bM = false;
	m_bDrawLasso = m_bPaintCP = false;


	vector<MaskData> &mask = TexCore::getInst()->m_maskData;
	OglImage3D      &vMask = TexCore::getInst()->m_volMsk;
	OglImage3D      &vFlg  = TexCore::getInst()->m_volFlg;


	EVec3i r = TexCore::getInst()->getResolution();
	const int N = r[0] * r[1] * r[2];
	
	for (int i = 0; i < N; ++i)
	{
		vFlg[i] = (mask[vMask[i]].lock) ? 0 : 1;
	}

	//call TOGLImage.setUpdated after modification (the data will send to GPU if this flg is ture)
	vFlg.setUpdated();

}

bool ModeSegPixPaint::canEndMode() 
{
	return true;//IDYES == AfxMessageBox("Current Result is not stored. \nDo you want to leave?", MB_YESNO);
}










// position & vCube --> voxel index
inline EVec4i t_calcVoxIdx(const EVec3f &p, const EVec3f &pitch, const EVec3i &reso)
{
	int x = (int)(p[0] / pitch[0]);
	int y = (int)(p[1] / pitch[1]);
	int z = (int)(p[2] / pitch[2]);
	t_crop(0, reso[0] - 1, x);
	t_crop(0, reso[1] - 1, y);
	t_crop(0, reso[2] - 1, z);
	return EVec4i(x, y, z, x + y * reso[0] + z * reso[0] * reso[1]);
}



static void drawCubes
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
		glPopMatrix();
	}
}






void ModeSegPixPaint::drawScene(
	const EVec3f &cuboid, 
	const EVec3f &camP  , 
	const EVec3f &camF)	
{
	const bool   bRendVol = TexCore::getInst()->m_dlg.bRendVol();
	const bool   bGradMag = TexCore::getInst()->m_dlg.bGradMag();
	const bool   bPsuedo  = TexCore::getInst()->m_dlg.bDoPsued();
	const float  alpha    = TexCore::getInst()->m_dlg.getTransAlpha();
	const EVec3i reso     = TexCore::getInst()->getResolution();
	const EVec3f pitch    = TexCore::getInst()->getPitch();

	bool isOnManip = TexCore::getInst()->m_dlg.bOnManip() || m_bL || m_bR || m_bM;
	const int  sliceN = (int)((isOnManip ? ONMOVE_SLICE_RATE : 1.0) * TexCore::getInst()->m_dlg.getSliceNum());


	//bind volumes ---------------------------------------
	glActiveTextureARB(GL_TEXTURE0);
	TexCore::getInst()->m_vol.bindOgl();
	glActiveTextureARB(GL_TEXTURE1);
	TexCore::getInst()->m_volGmag.bindOgl();
	glActiveTextureARB(GL_TEXTURE2);
	TexCore::getInst()->m_volFlg.bindOgl(false);
	glActiveTextureARB(GL_TEXTURE3);
	TexCore::getInst()->m_volMsk.bindOgl(false);
	glActiveTextureARB(GL_TEXTURE4);
	TexCore::getInst()->m_imgTf.bindOgl(false);
	glActiveTextureARB(GL_TEXTURE5);
	TexCore::getInst()->m_imgPsu.bindOgl();
	glActiveTextureARB(GL_TEXTURE6);
	TexCore::getInst()->m_imgMskCol.bindOgl(false);

	
	//render cross sections ----------------------------------
	m_crssecShader.bind(0, 1, 2, 3, 6, reso, bGradMag, !isSpaceKeyOn());
	CrsSecCore::getInst()->drawCrosSec(cuboid);
	m_crssecShader.unbind();



	//volume rendering ---------------------------------------
	if (bRendVol && !isSpaceKeyOn())
	{
		glDisable( GL_DEPTH_TEST );
		glEnable(GL_BLEND);
		m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, camP, bPsuedo, true);
		t_drawSlices(sliceN, camP, camF, cuboid);
		m_volumeShader.unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}	

	if (m_bPaintCP) drawCubes(m_paintPixels, pitch, m_bL);
	
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
	
	float cosTheta = min(1,a.dot(b) / an / bn );

	return ( a.cross(b)).dot(axis) > 0 ? acos(cosTheta): -acos(cosTheta);
}


//注目している点が内側であるか判定
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





//lassoの内側にあるpixel flgを 255に

static void c_addPixsInsideLasso
(
	const EVec3i         &WHD,
	const EVec3f         &pitch,
	const vector<EVec3f> &lassoPs,
	const CRSSEC_ID      &id,
	const bool &isFill,
   		  OglImage3D     &vFlg

)
{
	const int W = WHD[0];
	const int H = WHD[1];
	const int D = WHD[2], WH = W*H;

  fprintf(stderr, "---- %d  %d %d %d\n", id, WHD[0],WHD[1],WHD[2]);
  Trace(pitch);

	if (id == CRSSEC_XY) 
  {
		const float zPos = lassoPs.begin()->z();
		const int   zi   = (int)(zPos / pitch[2]);

	#pragma omp parallel for
		for (int yi = 0; yi < H; ++yi)
		for (int xi = 0; xi < W; ++xi)
		{
			EVec3f p((xi + 0.5f) * pitch[0], (yi + 0.5f) * pitch[1], zPos);
			if ( c_isInsideLasso(p, lassoPs, id)) vFlg[xi + yi * W + zi * WH] = (isFill)? 255 : 1;
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
			EVec3f p(xPos, (yi + 0.5f) * pitch[1], (zi + 0.5f) * pitch[2]);
			if (c_isInsideLasso(p, lassoPs, id)) vFlg[xi + yi * W + zi * WH] = (isFill) ? 255 : 1;
		}
	}

		
	if (id == CRSSEC_ZX) 
  {
		const float yPos = lassoPs.begin()->y();
		const int   yi   = (int)(yPos / pitch[1]);

	#pragma omp parallel for
		for (int zi = 0; zi < WHD[2]; ++zi)
		for (int xi = 0; xi < WHD[0]; ++xi)
		{
			EVec3f p((xi + 0.5f) * pitch[0], yPos, (zi + 0.5f) * pitch[2]);
			if (c_isInsideLasso(p, lassoPs, id)) vFlg[xi + yi * W + zi * WH] = (isFill) ? 255 : 1;
		}
	}
}





// mouse listener 
void ModeSegPixPaint::LBtnUp  (const CPoint &p, OglForMFC &ogl) 
{
	OglImage3D  &vFlg = TexCore::getInst()->m_volFlg;
	EVec3f rayP, rayD, pos;
	ogl.GetCursorRay(p, rayP, rayD);
	EVec3i reso  = TexCore::getInst()->getResolution();
	EVec3f pitch = TexCore::getInst()->getPitch();
	EVec3f cube  = TexCore::getInst()->getCuboidF();

	CRSSEC_ID id = CrsSecCore::getInst()->pickCrosSec(cube, rayP, rayD, pos);

	if (m_bPaintCP) 
	{
		for (auto itr : m_paintPixels)  if( vFlg[itr[3]] == 1 ) vFlg[itr[3]] = 255;
		vFlg.setUpdated();
	}
	if (m_bDrawLasso)
	{
		c_addPixsInsideLasso(reso, pitch, m_lasso, id, m_bL, vFlg);
		vFlg.setUpdated();
	}

	m_lasso.clear();
	m_paintPixels.clear();
	m_bL = m_bPaintCP = m_bDrawLasso = false;
	ogl.BtnUp();
	ogl.Redraw();
}


void ModeSegPixPaint::RBtnUp(const CPoint &p, OglForMFC &ogl)
{
	OglImage3D  &vFlg = TexCore::getInst()->m_volFlg;
	EVec3f rayP, rayD, pos;
	ogl.GetCursorRay(p, rayP, rayD);
	EVec3i reso  = TexCore::getInst()->getResolution();
	EVec3f pitch = TexCore::getInst()->getPitch();
	EVec3f cube  = TexCore::getInst()->getCuboidF();

	CRSSEC_ID id = CrsSecCore::getInst()->pickCrosSec(cube, rayP, rayD, pos);

	if (m_bPaintCP) 
	{
		for (auto itr : m_paintPixels)  if( vFlg[itr[3]] == 255 ) vFlg[itr[3]] = 1;
		vFlg.setUpdated();
	}
	if (m_bDrawLasso)
	{
		c_addPixsInsideLasso(reso, pitch, m_lasso, id, m_bL, vFlg);
		vFlg.setUpdated();
	}

	m_lasso.clear();
	m_paintPixels.clear();

	m_bR = m_bPaintCP = false;
	ogl.BtnUp();
	ogl.Redraw();
}


void ModeSegPixPaint::MBtnUp  (const CPoint &p, OglForMFC &ogl) 
{
	m_bM = false;
	ogl.BtnUp();
	ogl.Redraw();
}



void ModeSegPixPaint::LBtnDown(const CPoint &p, OglForMFC &ogl)
{
	m_bL = true;
		
	if (isShiftKeyOn() )
	{
		if( m_dlg.m_check_lasso.GetCheck()) m_bDrawLasso = true;
		else  m_bPaintCP = true;
	}
	else
	{
		ogl.BtnDown_Trans(p);
	}
}


void ModeSegPixPaint::RBtnDown(const CPoint &p, OglForMFC &ogl) 
{
	m_bR = true;
	if (isShiftKeyOn() )
	{
		if( m_dlg.m_check_lasso.GetCheck()) m_bDrawLasso = true;
		else  m_bPaintCP = true;
	}
	else
	{
		ogl.BtnDown_Rot(p);
	}
	m_bR = true;
}


void ModeSegPixPaint::MBtnDown(const CPoint &p, OglForMFC &ogl) 
{
	ogl.BtnDown_Zoom(p);
	m_bM = true;
}





void ModeSegPixPaint::MouseMove(const CPoint &p, OglForMFC &ogl) 
{
	if (!m_bL && !m_bM && !m_bR) return;


	EVec3f rayP, rayD;
	ogl.GetCursorRay(p, rayP, rayD);
	EVec3i reso  = TexCore::getInst()->getResolution();
	EVec3f pitch = TexCore::getInst()->getPitch();
	EVec3f cube  = TexCore::getInst()->getCuboidF();
	OglImage3D &vFlg = TexCore::getInst()->m_volFlg;

	EVec3f pos;
	
	if (m_bPaintCP)
	{
		if (CrsSecCore::getInst()->pickCrosSec(cube, rayP, rayD, pos) == CRSSEC_NON) return;
		EVec4i pixIdx = t_calcVoxIdx(pos, pitch, reso);
		if( m_bL && vFlg[pixIdx[3]] == 1 ) m_paintPixels.push_back( pixIdx );
		if(!m_bL && vFlg[pixIdx[3]] ==255) m_paintPixels.push_back( pixIdx );
	}
	else if (m_bDrawLasso) 
	{
		if (CrsSecCore::getInst()->pickCrosSec(cube, rayP, rayD, pos) != CRSSEC_NON)
		{
			m_lasso.push_back(pos);
		}
	}
	else
	{
		ogl.MouseMove(p);
	}	

	ogl.Redraw();
}



void ModeSegPixPaint::LBtnDclk(const CPoint &p, OglForMFC &ogl) {}
void ModeSegPixPaint::RBtnDclk(const CPoint &p, OglForMFC &ogl) {}
void ModeSegPixPaint::MBtnDclk(const CPoint &p, OglForMFC &ogl) {}

void ModeSegPixPaint::MouseWheel(const CPoint &p, int nFlags, short zDelta, OglForMFC &ogl) 
{
	EVec3f eyeRay, eyePos, cuboid = TexCore::getInst()->getCuboidF();
	ogl.GetCursorRay(p, eyePos, eyeRay);

	CRSSEC_ID id = CrsSecCore::getInst()->pickCrosSec(cuboid, eyePos, eyeRay);
	if (id != CRSSEC_NON) CrsSecCore::getInst()->moveCrossSec(id, zDelta);
	else ogl.ZoomCam(zDelta * 0.1f);

	ogl.Redraw();
}

void ModeSegPixPaint::keyDown(int nChar, int nRepCnt, int nFlags) {
	CRoiPaintView::MyRedrawWindow();
}
void ModeSegPixPaint::keyUp(int nChar, int nRepCnt, int nFlags) {
	CRoiPaintView::MyRedrawWindow();
}


*/




#pragma managed