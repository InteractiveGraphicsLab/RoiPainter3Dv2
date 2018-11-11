
#include "ModeSegLocalRGrow.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegLocalRGrow.h"
#include "climessagebox.h"


using namespace RoiPainter3D;

#pragma unmanaged

ModeSegLocalRGrow::~ModeSegLocalRGrow()
{
}

ModeSegLocalRGrow::ModeSegLocalRGrow() :
	m_volumeShader("shader/volVtx.glsl"   , "shader/volFlg_LRG.glsl"   ),
	m_crssecShader("shader/crssecVtx.glsl", "shader/crssecFlg_LRG.glsl")
{
  m_bL = m_bR = m_bM = false;
  m_moveSeedCpId << -1, -1;

}



bool ModeSegLocalRGrow::canEndMode()
{
  return true;
}



//flg useage 
// 0   : locked 
// 1   : not yet segmented (background) 
// 2   : fixed background (negative) region
// 3   : now visiting flg
// 255 : fixed foreground (pogitive) region
void ModeSegLocalRGrow::startMode()
{
	const vector<MaskData> &mask = ImageCore::getInst()->m_maskData  ;
	const OglImage3D      &vMask = ImageCore::getInst()->m_volMsk;
	OglImage3D            &vFlg  = ImageCore::getInst()->m_volFlg ;
	const EVec3i          &reso  = ImageCore::getInst()->getResolution();
	const int N = reso[0] * reso[1] * reso[2];


	//CPï`âÊÇÃÇΩÇﬂ mesh Çèâä˙âª
	m_CpSize = ImageCore::getInst()->getPitchW() * 2.0f;
	m_CPmesh.initializeIcosaHedron(m_CpSize);

  //set vFlg
	for (int i = 0; i < N; ++i) vFlg[i] = ( mask[ vMask[i] ].lock ) ? 0 : 1;
	vFlg.SetUpdated();

  //other flgs
	m_bDrawCutStr = false;
	m_stroke.clear();

  //show dialog
  formSegLocalRGrow_Show();

	//seedÇèâä˙âª
  m_seeds.clear();
  formSegLocalRGrow_updateList();
}


bool ModeSegLocalRGrow::canEndMode()
{
	if ( m_seeds.size() != 0 && !CLI_MessageBox_YESNO_Show("Current Result is not stored. Do you want to leave?", "message") );
	{
		return false;
	}
	return true;
}




void ModeSegLocalRGrow::finishSegmentation()
{
	const EVec3i res = ImageCore::getInst()->getResolution();
	const int    N = res[0] * res[1] * res[2];

	bool bForeExist = false;
	for (int i = 0; i < N; ++i)
	{
		if ( ImageCore::getInst()->m_volFlg[i] == 255)
		{
			bForeExist = true;
			break;
		}
	}

	if (!bForeExist) 
	{
		CLI_MessageBox_OK_Show("No foreground region exist", "message");
		return;
	}

  m_seeds.clear();
	ImageCore::getInst()->mask_storeCurrentForeGround();
	ModeCore::getInst()->ModeSwitch( MODE_VIS_MASK );
	formMain_redrawMainPanel();
}




void ModeSegLocalRGrow::cancelSegmentation()
{
  m_seeds.clear();
  formSegLocalRGrow_updateList();
  ModeCore::getInst()->ModeSwitch(MODE_VIS_NORMAL);
  formMain_redrawMainPanel();
}






///////////////////////////////////////////////////////////////////////
//Mouse Listener///////////////////////////////////////////////////////

// Add seed      : L/R double click to add Pos/Neg seed
// Add seed cp   : shift + L double click to add new CP (when one seed is activated) 
// Remove seed   : L double click 
// Remove seed cp: L double click  
// Move seed     : L click & drag


void ModeSegLocalRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (isCtrKeyOn())
  {
    m_bDrawCutStr = true;
    m_stroke.clear();
    return;
  }

	EVec3f rayP, rayD;
	ogl->GetCursorRay(p, rayP, rayD);

	m_moveSeedCpId = pickSeeds(rayP, rayD);
  m_ActiveSeedIdx = m_moveSeedCpId[0];
	formSegLocalRGrow_updateList();

  if(m_moveSeedCpId[0] == -1)
	{
		ogl->BtnDown_Trans(p);
	}

  formMain_redrawMainPanel();
}


void ModeSegLocalRGrow::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_bDrawCutStr)
	{
		CrssecCore::getInst()->GenerateCurvedCrssec( ImageCore::getInst()->getCuboidF(), ogl->GetCamPos(), m_stroke );
	}

	m_bDrawCutStr = m_bL = false;
	m_moveSeedCpId << -1, -1;
	ogl->BtnUp();
  formMain_redrawMainPanel();
}


void ModeSegLocalRGrow::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{	
  m_bR = true;
  
  EVec3f rayP, rayD;
	ogl->GetCursorRay(p, rayP, rayD);
	m_moveSeedCpId = pickSeeds(rayP, rayD);

	if(m_moveSeedCpId [0] == -1) ogl->BtnDown_Rot(p);
	
  formMain_redrawMainPanel();
}


void ModeSegLocalRGrow::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
  m_bR = false;
  m_moveSeedCpId << -1, -1;
  formMain_redrawMainPanel();
}



void ModeSegLocalRGrow::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
  formMain_redrawMainPanel();
}

void ModeSegLocalRGrow::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
	m_bM = false;
	m_moveSeedCpId << -1, -1;
  formMain_redrawMainPanel();
}




void ModeSegLocalRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
  EVec3f rayP, rayD;
	ogl->GetCursorRay(p, rayP, rayD);

  // highlightçœÇ› + Shift Ldouble click --> add cp to a seed
	if( isShiftKeyOn() && m_ActiveSeedIdx  != -1) 
	{
		dblclkToAddNewCp(rayP, rayD, m_ActiveSeedIdx);
	}
	else
	{
		dblclkToAddNewSeed(rayP, rayD, true);
	}

  formSegLocalRGrow_updateList();
  formMain_redrawMainPanel();
}




void ModeSegLocalRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl) 
{
  EVec3f rayP, rayD;
  ogl->GetCursorRay(p, rayP, rayD);
	
  // highlightçœÇ› + Shift Ldouble click --> add cp to a seed
	if( isShiftKeyOn() && m_ActiveSeedIdx  != -1) 
	{
		dblclkToAddNewCp(rayP, rayD, m_ActiveSeedIdx);
	}
	else
	{
		 dblclkToAddNewSeed(rayP, rayD, false); 
	}

  formSegLocalRGrow_updateList();
  formMain_redrawMainPanel();
}


void ModeSegLocalRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl) 
{
  EVec3f rayP, rayD;
  ogl->GetCursorRay(p, rayP, rayD);
	
	EVec2i seedId = pickSeeds(rayP, rayD);
	if(seedId[0] != -1 && seedId[1] != -1) dblclkToRemoveCP( seedId );
		
  formSegLocalRGrow_updateList();
  formMain_redrawMainPanel();
}



EVec2i ModeSegLocalRGrow::pickSeeds(const EVec3f &rayP, const EVec3f &rayD)
{
  for(int i = 0; i < (int)m_seeds.size(); ++i)
	{
		for(int j = 0; j < m_seeds[i].m_pos.size(); ++j)
		{
			if ( t_distRayToPoint(rayP,rayD,m_seeds[i].m_pos[j]) < m_CpSize )
			{				
				return EVec2i(i,j);
			}
		}
	}	

	return EVec2i( -1, -1);
}





void ModeSegLocalRGrow::MouseMove(const EVec2i &p, OglForCLI *ogl)
{
	if ( !m_bL && !m_bR && !m_bM ) return;

	EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);

	if ( m_bDrawCutStr )
	{
		m_stroke.push_back( rayP + 0.1f * rayD );
	}
	else if( m_moveSeedCpId[0] != -1 )
	{
    if ( pickCrsSec( rayP, rayD, &pos) != CRSSEC_NON )
      m_seeds[ m_moveSeedCpId[0] ].m_pos[m_moveSeedCpId[1] ] = pos;
	}
  else 
	{
		ogl->MouseMove( p );
	}

  formMain_redrawMainPanel();
}


void ModeSegLocalRGrow::MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl)
{
  EVec3f rayP, rayD, pos;
	ogl->GetCursorRay(p, rayP, rayD);	
	

	if ( 0 <= m_ActiveSeedIdx && m_ActiveSeedIdx < m_seeds.size() )
	{
    //active seed Ç™Ç†ÇÍÇŒÅAÇªÇÃîºåaÇïœçXÇ∑ÇÈ (algÉLÅ[Ç™âüÇ≥ÇÍÇƒÇ¢ÇÈÇ∆Ç´ÇÕçÇë¨) 
    double d = isAltKeyOn() ? (float)(zDelta * 0.1) : (float)(zDelta * 0.001);
    m_seeds[m_ActiveSeedIdx].modifyRadius(d);
	}
	else
	{
    CRSSEC_ID id = pickCrsSec(rayP, rayD, &pos);
    if( id != CRSSEC_NON ) {
      CrssecCore::getInst()->MoveCrssec( ImageCore::getInst()->getResolution(), 
                                         ImageCore::getInst()->getPitch(), id, 
                                         (isAltKeyOn) ? 3 * zDelta : zDelta);
    }
	}
  formMain_redrawMainPanel();
}



void ModeSegLocalRGrow::keyDown(int nChar) {formMain_redrawMainPanel();}
void ModeSegLocalRGrow::keyUp(int nChar) {formMain_redrawMainPanel();}









void ModeSegLocalRGrow::drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF)
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

	//render cross sections ----------------------------------
  glColor3d(1, 1, 1);
  m_crssecShader.bind(0, 1, 2, 3, 6, reso, false, !isSpaceKeyOn());
  CrssecCore::getInst()->DrawCrssec(bXY, bYZ, bZX, cuboid);
  m_crssecShader.unbind();


	//volume rendering ---------------------------------------
	if ( bDrawVol && !isSpaceKeyOn() )
	{
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volumeShader.bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, camP, bPsuedo, true );
		t_drawSlices(sliceN, camP, camF, cuboid);
		m_volumeShader.unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

  //draw seeds
  if(!isShiftKeyOn()) 
  {
    for( const auto& s : m_seeds) s.draw( m_CPmesh );
  }
	if(0 <= m_ActiveSeedIdx && m_ActiveSeedIdx < m_seeds.size())
  {
    m_seeds[m_ActiveSeedIdx].drawAsActive( m_CPmesh );
  }

	
	if (m_bDrawCutStr)
    t_drawLineStrip(EVec3f(1,1,0), 3, m_stroke);
}

#pragma managed






// êVÇµÇ¢SeedÇí«â¡
// bNegPos = true : foreground seed
// bNegPos = false: background seed
void ModeSegLocalRGrow::dblclkToAddNewSeed(const EVec3f &rayP, const EVec3f &rayD, bool bNegPos)
{
  EVec3f cuboid = ImageCore::getInst()->getCuboidF();
  EVec3f pos;
	CRSSEC_ID id = pickCrsSec( rayP, rayD, &pos);
	if (id == CRSSEC_NON) return;

	float imgV = ImageCore::getInst()->getVoxelValue(pos);
	float minV = bNegPos ? imgV - 100 : 0         ;
	float maxV = bNegPos ? 4095       : imgV + 100;
	float size = bNegPos ? cuboid[0] * 0.05f : cuboid[0] * 0.02f;

  m_seeds.push_back( LRGseed( pos, minV, maxV, bNegPos, size) );
	m_ActiveSeedIdx = (int)m_seeds.size() - 1;

  //TODO SORT SEED
  formSegLocalRGrow_updateList();
  formMain_redrawMainPanel();
}



//SeedÇ…Control pointÇí«â¡
void ModeSegLocalRGrow::dblclkToAddNewCp(const EVec3f &rayP, const EVec3f &rayD, const int seedId)
{
	EVec3f pos;

	if ( pickCrsSec(rayP, rayD, &pos) != CRSSEC_NON)
	{
    m_seeds[seedId].m_pos.push_back(pos);
	}

  formSegLocalRGrow_updateList();
  formMain_redrawMainPanel();
}



//SeedÇ©ÇÁControl PointÇçÌèú
void ModeSegLocalRGrow::dblclkToRemoveCP(EVec2i seedIdCpId)
{
  int seedId = seedIdCpId[0];
  int cpId   = seedIdCpId[1];

  if( seedId < 0 || m_seeds.size() <= seedId) return;

	if( cpId == 0 ) 
	{
    //seedé©ëÃÇçÌèú
		m_seeds.erase(m_seeds.begin() + seedId);
		m_ActiveSeedIdx = -1;
	}
	else
	{
    //control point ÇæÇØÇçÌèú
		m_seeds[seedId].m_pos.erase( m_seeds[seedId].m_pos.begin() + cpId );
	}
  formSegLocalRGrow_updateList();
  formMain_redrawMainPanel();
}

















// Local Region Grow
// flg useage 
// 0   : lock 
// 1   : yet
// 2   : negative region
// 3   : now visiting
// 255 : pogitive region
void ModeSegLocalRGrow::runLocalRegionGrow()
{
	const short *vol   = ImageCore::getInst()->m_volOrig      ;
	const EVec3f pitch = ImageCore::getInst()->getPitch()     ;
	const EVec3i res   = ImageCore::getInst()->getResolution();
	const int W  = res[0];
	const int H  = res[1];
	const int D  = res[2], WH = W*H, WHD = W*H*D;

	OglImage3D &volFlg = ImageCore::getInst()->m_volFlg;
	for (int i = 0; i < WHD; ++i ) volFlg[i] = (volFlg[i] == 0) ? 0 : 1;

	byte *flg = new byte[W*H*D];

  //seedñàÇ…Ç–Ç∆Ç¬Ç√Ç¬Region growingÇåvéZ
	for( int j = 0; j < m_seeds.size(); ++j)
	{
		//flgèâä˙âª
		if(seeds[j].m_flg) for(int i=0;i<WHD;++i) flg[i] = (LRGflg[i] == 255) ? 1 : LRGflg[i];
		else               for(int i=0;i<WHD;++i) flg[i] = 1;
	
		//marge flg & LRGflg
		s_LocalRegionGrow(seeds[j], W, H, D, img, pitch, flg, j);
		for (int i = 0; i < WHD; ++i ) if(flg[i] != 1) LRGflg[i] = flg[i];
	}

	delete[] flg;

	TexCore::getInst()->m_volFlg.setUpdated();
	fprintf(stderr,"Local Region Grow done\n");
}


















/*

inline bool isInRad(
	const EVec3f &pitch, 
	const int x,
	const int y, 
	const int z, 
	const EVec3f &P,
	const float &R2 )
{
	float fx = x * pitch[0];
	float fy = y * pitch[1];
	float fz = z * pitch[2];
	return (P[0] - fx) * (P[0] - fx)  + 
		   (P[1] - fy) * (P[1] - fy)  + 
		   (P[2] - fz) * (P[2] - fz)  < R2;
}



static EVec4i getPixel
(
	const int W,
	const int H,
	const int D,
	const EVec3f &pitch,
	const EVec3f &p)
{
	int x = min(W - 1, (int)(p[0] / pitch[0]));
	int y = min(H - 1, (int)(p[1] / pitch[1]));
	int z = min(D - 1, (int)(p[2] / pitch[2]));
	return EVec4i(x, y, z, x + y*W + z*W*H);
}





	//int k[26] = {
	//		(x>0  &&y>0  &&z>0	) ? I-1-W-WH : -1, 
	//		(x>0  &&y>0  &&z>0	) ? I  -W-WH : -1,
	//		(x<W-1&&y>0  &&z>0  ) ? I+1-W-WH : -1, 
	//		(x>0         &&z>0  ) ? I-1  -WH : -1, 
	//		(              z>0  ) ? I    -WH : -1,
	//		(x<W-1       &&z>0  ) ? I+1  -WH : -1,
	//		(x>0  &&y<H-1&&z>0  ) ? I-1-W-WH : -1,
	//		(       y<H-1&&z>0  ) ? I  -W-WH : -1,
	//		(x<W-1&&y<H-1&&z>0  ) ? I+1-W-WH : -1, 

	//		(x>0  &&y>0         ) ? I-1-W    : -1,
	//		(       y>0         ) ? I  -W    : -1,
	//		(x<W-1&&y>0         ) ? I+1-W    : -1,
	//	    (x>0                ) ? I-1      : -1,
	//		(x<W-1              ) ? I+1      : -1,
	//		(x>0  &&y<H-1       ) ? I-1-W    : -1,
	//		(       y<H-1       ) ? I  -W    : -1,
	//		(x<W-1&&y<H-1       ) ? I+1-W    : -1,

	//		(x>0  &&y>0  &&z<D-1) ? I-1-W+WH : -1,
	//		(       y>0  &&z<D-1) ? I  -W+WH : -1,
	//		(x<W-1&&y>0  &&z<D-1) ? I+1-W+WH : -1,
	//		(x>0         &&z<D-1) ? I-1  +WH : -1,
	//		(              z<D-1) ? I    +WH : -1,
	//		(x<W-1       &&z<D-1) ? I+1  +WH : -1,
	//		(x>0  &&y<H-1&&z<D-1) ? I-1-W+WH : -1,
	//		(       y<H-1&&z<D-1) ? I  -W+WH : -1,
	//		(x<W-1&&y<H-1&&z<D-1) ? I+1-W+WH : -1
	//	};








static float distTrans_growFronteer
(
	 deque<EVec4i> &Q1,
	 deque<EVec4i> &Q2,
	 float *dist,
	 byte  *flg  
)
{
	const EVec3f pitch = TexCore::getInst()->getPitch();
	const EVec3i res   = TexCore::getInst()->getResolution();
	const int W  = res[0];
	const int H  = res[1];
	const int D  = res[2], WH = W*H, WHD = W*H*D;
	vector <float> distbox;

	float minD = FLT_MAX; 

	float pX   = pitch[0], pY = pitch[1], pZ = pitch[2];
	float pXY  = sqrt(pX*pX + pY*pY        );
	float pYZ  = sqrt(        pY*pY + pZ*pZ);
	float pXZ  = sqrt(pX*pX         + pZ*pZ);
	float pXYZ = sqrt(pX*pX + pY*pY + pZ*pZ);
	
	while(!Q1.empty())
	{
		const int x = Q1.front()[0];
		const int y = Q1.front()[1];
		const int z = Q1.front()[2];
		const int I = Q1.front()[3];
		Q1.pop_front();
	

		int K ;
	
		//calcMinDist
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&& flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );
		K = I  -W-WH; if(       y>0  &&z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pYZ  );
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );
		K = I-1  -WH; if(x>0         &&z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pXZ  );
		K = I    -WH; if(              z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pZ   );
		K = I+1  -WH; if(x<W-1       &&z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pXZ  );
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );
		K = I  +W-WH; if(       y<H-1&&z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pYZ  );
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  && flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );
		
		K = I-1-W   ; if(x>0  &&y>0         && flg[K]) dist[I] = min( dist[I], dist[K] + pXY );
		K = I  -W   ; if(       y>0         && flg[K]) dist[I] = min( dist[I], dist[K] + pY  );
		K = I+1-W   ; if(x<W-1&&y>0         && flg[K]) dist[I] = min( dist[I], dist[K] + pXY );
		K = I-1     ; if(x>0                && flg[K]) dist[I] = min( dist[I], dist[K] + pX  );
		K = I+1     ; if(x<W-1              && flg[K]) dist[I] = min( dist[I], dist[K] + pX  );
		K = I-1+W   ; if(x>0  &&y<H-1       && flg[K]) dist[I] = min( dist[I], dist[K] + pXY );
		K = I  +W   ; if(       y<H-1       && flg[K]) dist[I] = min( dist[I], dist[K] + pY  );
		K = I+1+W   ; if(x<W-1&&y<H-1       && flg[K]) dist[I] = min( dist[I], dist[K] + pXY );

		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );
		K = I  -W+WH; if(       y>0  &&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pYZ  );
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );
		K = I-1  +WH; if(x>0         &&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pXZ  );
		K = I    +WH; if(              z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pZ   );
		K = I+1  +WH; if(x<W-1       &&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pXZ  );
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );
		K = I  +W+WH; if(       y<H-1&&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pYZ  );
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&& flg[K]) dist[I] = min( dist[I], dist[K] + pXYZ );

		//add to deque
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&&!flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z-1, K)); flg[K] = 1;}	
		K = I  -W-WH; if(       y>0  &&z>0  &&!flg[K]) { Q2.push_back(EVec4i(x  , y-1, z-1, K)); flg[K] = 1;}	
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  &&!flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z-1, K)); flg[K] = 1;}	
		K = I-1  -WH; if(x>0         &&z>0  &&!flg[K]) { Q2.push_back(EVec4i(x-1, y  , z-1, K)); flg[K] = 1;}	
		K = I    -WH; if(              z>0  &&!flg[K]) { Q2.push_back(EVec4i(x  , y  , z-1, K)); flg[K] = 1;}
		K = I+1  -WH; if(x<W-1       &&z>0  &&!flg[K]) { Q2.push_back(EVec4i(x+1, y  , z-1, K)); flg[K] = 1;}
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  &&!flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z-1, K)); flg[K] = 1;}
		K = I  +W-WH; if(       y<H-1&&z>0  &&!flg[K]) { Q2.push_back(EVec4i(x  , y+1, z-1, K)); flg[K] = 1;}
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  &&!flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z-1, K)); flg[K] = 1;}
		
		K = I-1-W   ; if(x>0  &&y>0         &&!flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z  , K)); flg[K] = 1;}
		K = I  -W   ; if(       y>0         &&!flg[K]) { Q2.push_back(EVec4i(x  , y-1, z  , K)); flg[K] = 1;}
		K = I+1-W   ; if(x<W-1&&y>0         &&!flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z  , K)); flg[K] = 1;}
		K = I-1     ; if(x>0                &&!flg[K]) { Q2.push_back(EVec4i(x-1, y  , z  , K)); flg[K] = 1;}
		K = I+1     ; if(x<W-1              &&!flg[K]) { Q2.push_back(EVec4i(x+1, y  , z  , K)); flg[K] = 1;}
		K = I-1+W   ; if(x>0  &&y<H-1       &&!flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z  , K)); flg[K] = 1;}
		K = I  +W   ; if(       y<H-1       &&!flg[K]) { Q2.push_back(EVec4i(x  , y+1, z  , K)); flg[K] = 1;}
		K = I+1+W   ; if(x<W-1&&y<H-1       &&!flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z  , K)); flg[K] = 1;}

		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z+1, K)); flg[K] = 1;}
		K = I  -W+WH; if(       y>0  &&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x  , y-1, z+1, K)); flg[K] = 1;}
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z+1, K)); flg[K] = 1;}
		K = I-1  +WH; if(x>0         &&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x-1, y  , z+1, K)); flg[K] = 1;}
		K = I    +WH; if(              z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x  , y  , z+1, K)); flg[K] = 1;}
		K = I+1  +WH; if(x<W-1       &&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x+1, y  , z+1, K)); flg[K] = 1;}
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z+1, K)); flg[K] = 1;}
		K = I  +W+WH; if(       y<H-1&&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x  , y+1, z+1, K)); flg[K] = 1;}
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&&!flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z+1, K)); flg[K] = 1;}



		minD = min( minD, dist[I]);
	}
	return minD;
}




//Written by Shogo Tsuruoka 11/15
//refactored by Takashi Ijiri 11/29
static void calcDistTrans
(
	const deque<EVec4i> &seedPixels,
	float *dist,
	float R
)	
{
	fprintf(stderr,"start Distance Transform...\n");
	
	const EVec3f cuboid = TexCore::getInst()->getCuboidF();
	const EVec3i res    = TexCore::getInst()->getResolution();
	const int WHD = res[0] * res[1] * res[2];
	
	byte *flg = new byte[WHD];

	for(int i = 0; i < WHD; ++i)
	{
		dist[i] = FLT_MAX;
		flg[i] = 0;
	}
	
	deque<EVec4i> Q1, Q2;

	for (const auto &p : seedPixels)
	{
		Q1.push_back(p);
		dist[p[3]] = 0.0;
		flg [p[3]] = 1  ;
	}


	if(R * 2 > cuboid[0])
	{
		for(int i = 0; i < WHD; ++i) dist[i] = 1;
	}
	else
	{
		while( !Q1.empty() || !Q2.empty() )
		{
			float minD;
			minD = distTrans_growFronteer(Q1,Q2, dist, flg);
			if(minD > R) break;
			minD = distTrans_growFronteer(Q2,Q1, dist, flg);			
			if(minD > R) break;
		}
	}

	delete[] flg;
	fprintf(stderr,"...Distance Transform Done\n");
}



void ModeSegLocalRGrow::s_LocalRegionGrow
(
	const LRGseed &seed,
	const int   W,
	const int   H,
	const int   D,
	const float *img,
	const EVec3f &pitch,
	byte* flg, //0:lock, 1:yet, 2:negative, 255:positive
	int seedId
)
{
	const int WH = W*H;

	//1 seed --> seed pixel
	deque<EVec4i> Q;
	if (seed.m_pos.size() == 1)
	{
		Q.push_back(getPixel(W, H, D, pitch, seed.m_pos[0]));
	}
	else
	{
		float len = pitch.norm() * 2;
		for (int i = 1; i < seed.m_pos.size(); ++i)
		{
			const EVec3f &p1 = seed.m_pos[i - 1];
			const EVec3f &p2 = seed.m_pos[  i  ];
			const EVec3f d   = (p2 - p1).normalized();
			const float  L   = (p2 - p1).norm();

			for (float a = 0; a < L; a += pitch[0] * 0.5f) Q.push_back(getPixel(W, H, D, pitch, p1 + a * d));
			Q.push_back(getPixel(W, H, D, pitch, p2) );
		}
	}


	fprintf(stderr,"result thresh = %f\n",seed.m_maxV);


	//3 region growing
	const byte    SET_FLG = seed.m_flg ? 255 : 2;
	const float   R       = seed.m_rad  ;
	const EVec3f &seedP   = seed.m_pos[0];
	const float   minV    = seed.m_minV;
	const float   maxV    = seed.m_maxV;

	//2 distance transform
	float *dist = new float[W*H*D ];
	calcDistTrans(Q, dist, R);

	for (const auto &p : Q)
	{
		if (flg[p[3]] == 1) flg[p[3]] = SET_FLG;
	}

	while ( !Q.empty() )
	{
		const int x = Q.front()[0];
		const int y = Q.front()[1];
		const int z = Q.front()[2];
		const int I = Q.front()[3];
		Q.pop_front();
		
		if (flg[I] == 0 ) continue;
		
		int K = I - 1;
		if (x > 0     && flg[K] == 1 && dist[K] < R && minV <= img[K] && img[K] <= maxV){ flg[K] = SET_FLG; Q.push_back(EVec4i(x - 1, y, z, K)); }
		K = I - W;
		if (y > 0     && flg[K] == 1 && dist[K] < R && minV <= img[K] && img[K] <= maxV){ flg[K] = SET_FLG; Q.push_back(EVec4i(x, y - 1, z, K)); }
		K = I - WH;
		if (z > 0     && flg[K] == 1 && dist[K] < R && minV <= img[K] && img[K] <= maxV){ flg[K] = SET_FLG; Q.push_back(EVec4i(x, y, z - 1, K)); }
		K = I + 1;
		if (x < W - 1 && flg[K] == 1 && dist[K] < R && minV <= img[K] && img[K] <= maxV){ flg[K] = SET_FLG; Q.push_back(EVec4i(x + 1, y, z, K)); }
		K = I + W;
		if (y < H - 1 && flg[K] == 1 && dist[K] < R && minV <= img[K] && img[K] <= maxV){ flg[K] = SET_FLG; Q.push_back(EVec4i(x, y + 1, z, K)); }
		K = I + WH;
		if (z < D - 1 && flg[K] == 1 && dist[K] < R && minV <= img[K] && img[K] <= maxV){ flg[K] = SET_FLG; Q.push_back(EVec4i(x, y, z + 1, K)); }
	}
	delete[] dist;
}







*/