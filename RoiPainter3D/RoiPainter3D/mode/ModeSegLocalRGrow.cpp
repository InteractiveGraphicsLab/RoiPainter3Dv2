
#include "ModeSegLocalRGrow.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

#include "FormMain.h"
#include "FormVisParam.h"
#include "FormSegLocalRGrow.h"
#include "climessagebox.h"

#include <deque>
#include <vector>

using namespace std;
using namespace RoiPainter3D;

#pragma unmanaged


ModeSegLocalRGrow::~ModeSegLocalRGrow()
{
}

ModeSegLocalRGrow::ModeSegLocalRGrow() :
	m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_LRG.glsl"   ),
	m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_LRG.glsl")
{
  m_bL = m_bR = m_bM = false;
  m_drag_seedcp_id << -1, -1;
}



//flg useage 
// 0   : locked 
// 1   : not yet segmented (background) 
// 2   : fixed background (negative) region
// 3   : now visiting
// 255 : fixed foreground (pogitive) region

void ModeSegLocalRGrow::StartMode()
{
  //init vol_flg 0:locked, 1:others
  ImageCore::GetInst()->InitializeVolFlgByLockedMask();

  //init field & control point
	m_b_drawstroke = false;
	m_stroke.clear();
  m_seeds .clear();
  LRGSeed::SetCpRadius( ImageCore::GetInst()->GetPitch()[0] * 2.0f );

  //show dialog
  formSegLocalRGrow_Show();

  float  max_radius = ImageCore::GetInst()->GetCuboid()[0] / 2;
  EVec2i minmax     = ImageCore::GetInst()->GetVolMinMax();
  formSegLocalRGrow_setSliderRange( max_radius, minmax[0], minmax[1]);
  formSegLocalRGrow_updateAllItems();

  //Lock/Unlock pitch box
  formVisParam_LockPitchBox();
}


bool ModeSegLocalRGrow::CanLeaveMode()
{
	if ( m_seeds.size() == 0 ) return true;
  if ( !CLI_MessageBox_YESNO_Show("Current Result is not stored. Do you want to leave?", "message") )
	{
		return false;
	}
	return true;
}



void ModeSegLocalRGrow::FinishSegmentation()
{
	if ( !ImageCore::GetInst()->bForeVoxelsExistInVolFlg() ) 
	{
		CLI_MessageBox_OK_Show("No foreground region exist", "message");
		return;
	}

  m_seeds.clear();
	ImageCore::GetInst()->StoreForegroundAsNewMask();
	ModeCore::GetInst()->ModeSwitch( MODE_VIS_MASK );
  RedrawScene();
}


void ModeSegLocalRGrow::CancelSegmentation()
{
  m_seeds.clear();
  formSegLocalRGrow_updateAllItems();
  ModeCore::GetInst()->ModeSwitch(MODE_VIS_NORMAL);
  RedrawScene();
}




///////////////////////////////////////////////////////////////////////
//Mouse Listener///////////////////////////////////////////////////////
// current version(現在のもの)
// Add F/B seed  : Button on Dialog   YET
// Add seed cp   : shift + L click to add new CP (when one seed is activated)  OK
// Move seed     : shift + L click & drag                                      OK
// Remove seed   : shift + R click                                             OK
// Remove seed cp: shift + R click                                             OK
// cut strok     : ctrl  + L drag                                              OK

// old(鶴岡君)バージョン
// Add seed      : L/R double click to add Pos/Neg seed
// Add seed cp   : shift + L double click to add new CP (when one seed is activated) 
// Remove seed   : M double click 
// Remove seed cp: M double click  
// Move seed     : L click & drag


void ModeSegLocalRGrow::AddNewSeed(bool bForeSeed)
{
  //fix position (表示されている planeの中心に配置)
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();

  EVec3f cp_position;
  if ( formVisParam_bPlaneXY() )
  {
    const float plane_xy = CrssecCore::GetInst()->GetPlaneXyPosition();
    cp_position << cuboid[0]/2, cuboid[1]/2, plane_xy * cuboid[2];
  }
  else if ( formVisParam_bPlaneYZ() )
  {
    const float plane_yz = CrssecCore::GetInst()->GetPlaneYzPosition();
    cp_position << plane_yz * cuboid[0], cuboid[1]/2, cuboid[2]/2;
  }
  else if ( FormVisParam_bPlaneZX() )
  {
    const float plane_zx = CrssecCore::GetInst()->GetPlaneZxPosition();
    cp_position << cuboid[0]/2, plane_zx * cuboid[1], cuboid[2]/2;
  }
  else
  {
    cp_position = cuboid * 0.5;
  }

  //get initial threshold
  short vol_v  = ImageCore::GetInst()->GetVoxelValue(cp_position);
	short min_v  = bForeSeed ? vol_v - 100 : 0         ;
	short max_v  = bForeSeed ? 4095       : vol_v + 100;
	float radius = bForeSeed ? cuboid[0] * 0.05f : cuboid[0] * 0.02f;

  EVec2i minmax = ImageCore::GetInst()->GetVolMinMax();
  min_v = max( min_v, (short)minmax[0]);
  max_v = min( max_v, (short)minmax[1]);

  m_seeds.push_back( LRGSeed( cp_position, min_v, max_v, bForeSeed, radius) );
	m_activeseed_idx = (int)m_seeds.size() - 1;

  //sort seed (Original code was by Hiryu Kamoshita)
  //negativeは前半に積む
  if( m_seeds.back().m_flg_fore == false)
  {
    for(int i = (int)m_seeds.size()-1; i > 0 ; --i) 
	  {
      //手前がnegativeなら終了
		  if( !m_seeds[i-1].m_flg_fore ) break;
      //そうでなければ入れ替え
      swap(m_seeds[i-1], m_seeds[i] );
      m_activeseed_idx = i-1;
	  }
  }

  formSegLocalRGrow_updateAllItems();
  RedrawScene();
}




void ModeSegLocalRGrow::LBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bL = true;

  if (IsCtrKeyOn() )
  {
    m_b_drawstroke = true;
    m_stroke.clear();
  }
  else if ( IsShiftKeyOn() )
  {
    EVec3f ray_pos, ray_dir;
    ogl->GetCursorRay(p, ray_pos, ray_dir);
    m_drag_seedcp_id = PickSeeds(ray_pos, ray_dir);

    if(m_drag_seedcp_id[0] != -1)
    {
      //hit --> activate and move
      m_activeseed_idx = m_drag_seedcp_id[0];
	    formSegLocalRGrow_updateAllItems();
    }
    else if( 0 <= m_activeseed_idx && m_activeseed_idx < (int)m_seeds.size() )
    {
      //not hit --> pick crossection and add cp
      EVec3f pos;
	    if ( PickCrssec(ray_pos, ray_dir, &pos) != CRSSEC_NON)
	    {
        m_seeds[ m_activeseed_idx ].m_cps.push_back(pos);
	    }
      formSegLocalRGrow_updateAllItems();
    }
  }
  else
  {
    ogl->BtnDown_Trans(p);
  }

  RedrawScene();
}


void ModeSegLocalRGrow::LBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  if (m_b_drawstroke)
	{
    EVec3f cuboid  = ImageCore::GetInst()->GetCuboid();
    EVec3f cam_pos = ogl->GetCamPos();
		CrssecCore::GetInst()->GenerateCurvedCrssec( cuboid, cam_pos, m_stroke );
	}

	m_b_drawstroke = false;
  m_bL = false;
	m_drag_seedcp_id << -1, -1;
	ogl->BtnUp();
  RedrawScene();
}



void ModeSegLocalRGrow::RBtnDown(const EVec2i &p, OglForCLI *ogl)
{	
  m_bR = true;
  
  if( IsShiftKeyOn() )
  {
    EVec3f ray_pos, ray_dir;
	  ogl->GetCursorRay(p, ray_pos, ray_dir);
	  EVec2i seedid_cpid = PickSeeds(ray_pos, ray_dir);
    int seedId = seedid_cpid[0];
    int cpId   = seedid_cpid[1];
	  
    if(0 <= seedId && seedId < (int)m_seeds.size() ) 
    {
		  m_seeds[ seedId ].m_cps.erase( m_seeds[seedId].m_cps.begin() + cpId );

      if( m_seeds[ seedId ].m_cps.size() == 0 )
      {
		    m_seeds.erase( m_seeds.begin() + seedId );
      }
      formSegLocalRGrow_updateAllItems();
    }
  }
  else
  {
    ogl->BtnDown_Rot(p);
  }

  RedrawScene();
}



void ModeSegLocalRGrow::RBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
  m_bR = false;
  m_drag_seedcp_id << -1, -1;
  RedrawScene();
}



void ModeSegLocalRGrow::MBtnDown(const EVec2i &p, OglForCLI *ogl)
{
  m_bM = true;
  ogl->BtnDown_Zoom(p);
}

void ModeSegLocalRGrow::MBtnUp(const EVec2i &p, OglForCLI *ogl)
{
  ogl->BtnUp();
	m_bM = false;
	m_drag_seedcp_id << -1, -1;
  RedrawScene();
}



void ModeSegLocalRGrow::LBtnDclk(const EVec2i &p, OglForCLI *ogl)
{
}


void ModeSegLocalRGrow::RBtnDclk(const EVec2i &p, OglForCLI *ogl) 
{
}


void ModeSegLocalRGrow::MBtnDclk(const EVec2i &p, OglForCLI *ogl) 
{
}



EVec2i ModeSegLocalRGrow::PickSeeds(const EVec3f &ray_pos, const EVec3f &ray_dir)
{
  float CP_RADIUS = LRGSeed::GetCpRadius();
  for(int i = 0; i < (int) m_seeds.size(); ++i)
	{
		for(int j = 0; j < (int) m_seeds[i].m_cps.size(); ++j)
		{
			if ( t_distRayToPoint(ray_pos, ray_dir, m_seeds[i].m_cps[j] ) < CP_RADIUS )
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

	EVec3f ray_pos, ray_dir, pos;
	ogl->GetCursorRay( p, ray_pos, ray_dir);

	if ( m_b_drawstroke )
	{
		m_stroke.push_back( ray_pos + 0.1f * ray_dir );
	}
	else if ( m_drag_seedcp_id[0] != -1 )
	{
    if ( PickCrssec( ray_pos, ray_dir, &pos) != CRSSEC_NON )
    {
      m_seeds[ m_drag_seedcp_id[0] ].m_cps[ m_drag_seedcp_id[1] ] = pos;
    }
	}
  else 
	{
		ogl->MouseMove( p );
	}

  RedrawScene(false);
}


void ModeSegLocalRGrow::MouseWheel(const EVec2i &p, short z_delta, OglForCLI *ogl)
{

	if ( IsShiftKeyOn() && 0 <= m_activeseed_idx && m_activeseed_idx < m_seeds.size() )
	{
    //active seed があれば、その半径を変更する (algキーが押されているときは高速) 
    float d = IsAltKeyOn() ? (float)(z_delta * 0.1) : (float)(z_delta * 0.001);
    m_seeds[ m_activeseed_idx ].ModifyRadius( d );
    formSegLocalRGrow_updateSliders();
	}
	else
	{
    PickToMoveCrossSecByWheeling(p, ogl, z_delta );
	}
  RedrawScene();
}



void ModeSegLocalRGrow::KeyDown(int nChar) 
{ 
  RedrawScene(); 
}


void ModeSegLocalRGrow::KeyUp  (int nChar) 
{
  RedrawScene(); 
}



void ModeSegLocalRGrow::DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center)
{
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

	//render cross sections ----------------------------------
  const bool   b_xy = formVisParam_bPlaneXY();
  const bool   b_yz = formVisParam_bPlaneYZ();
  const bool   b_zx = FormVisParam_bPlaneZX();
  const bool   b_gm = formVisParam_bGradMag();
  const EVec3i reso = ImageCore::GetInst()->GetResolution();

  glColor3d(1, 1, 1);
  m_crssec_shader.Bind( 0, 1, 2, 3, 6, reso, b_gm, !IsSpaceKeyOn());
  CrssecCore::GetInst()->DrawCrssec(b_xy, b_yz, b_zx, cuboid);
  m_crssec_shader.Unbind();


	//volume rendering ---------------------------------------
  const bool   b_draw_vol = formVisParam_bRendVol();
	if ( b_draw_vol && !IsSpaceKeyOn() )
	{
    const bool  b_psuedo  = formVisParam_bDoPsued();
    const float alpha     = formVisParam_getAlpha();
    const bool  b_onmanip = formVisParam_bOnManip() || m_bL || m_bR || m_bM;
    const int   num_slice = (int)((b_onmanip ? ONMOVE_SLICE_RATE : 1.0) * formVisParam_getSliceNum());

		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		m_volume_shader.Bind(0, 1, 2, 3, 4, 5, 6, alpha * 0.1f, reso, cam_pos, b_psuedo, true );
		t_DrawCuboidSlices(num_slice, cam_pos, cam_center, cuboid);
		m_volume_shader.Unbind();
		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
	}

  //draw seeds
  if( !IsSpaceKeyOn() ) 
  {
    for( const auto& s : m_seeds) s.Draw( );
  }

	if(0 <= m_activeseed_idx && m_activeseed_idx < m_seeds.size())
  {
    m_seeds[m_activeseed_idx].DrawAsActive( );
  }
	
	if (m_b_drawstroke)
  {
    t_DrawPolyLine(EVec3f(1,1,0), 3, m_stroke);
  }
}



















static void s_LocalRegionGrow
(
	const int W,
	const int H,
	const int D,
	const LRGSeed &seed ,
  const short   *vol  ,
	const EVec3f  &pitch,

	byte* vol_f
);


// Local Region Grow
// flg useage 
// 0   : lock 
// 1   : yet
// 2   : negative region
// 3   : now visiting
// 255 : pogitive region
void ModeSegLocalRGrow::RunLocalRegionGrow()
{
  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
  const short *vol   = ImageCore::GetInst()->m_vol_orig     ;
	const EVec3f pitch = ImageCore::GetInst()->GetPitch()     ;

  byte *flg3d = ImageCore::GetInst()->m_vol_flag.GetVolumePtr();

	for (int i = 0; i < WHD; ++i ) flg3d[i] = (flg3d[i] == 0) ? 0 : 1;

	byte *flg = new byte[ WHD ];

  //seed毎にRegion growingを計算
	for( int j = 0; j < (int) m_seeds.size(); ++j)
	{
    //flg初期化(0:locked, 1:yet fixed, 2:negative, 255:positive)
    //前景seed : locked(0)とnegative(2)以外をyet(1)に
    //背景seed : 領域計算は全体に対して行う
		if ( m_seeds[j].m_flg_fore)
    {
      for ( int i = 0; i < WHD; ++i) 
        flg[i] = (flg3d[i] == 255) ? 1 : flg3d[i];
    }
		else
    {
      for ( int i = 0; i < WHD; ++i) 
        flg[i] = 1;
    }
	
    //region grow
		s_LocalRegionGrow( W,H,D, m_seeds[j], vol, pitch, flg);
		
		for (int i = 0; i < WHD; ++i ) 
      if(flg[i] != 1) 
        flg3d[i] = flg[i];
	}

	delete[] flg;

  ImageCore::GetInst()->m_vol_flag.SetUpdated();
	std::cout << "Local Region Grow done\n";
}





// grow distans transform volume (vol_dist)
// return minimum value of grown voxel
static float s_DistTransGrowFronteer
(
    const int W, const int H, const int D,
    const EVec3f &pitch, 

    deque<EVec4i> &Q1, //input : current fronteer 
    deque<EVec4i> &Q2, //output: new fronteer

    float *vol_dist,
    byte  *vol_flg  
)
{
	const int WH = W*H, WHD = W*H*D;
	float pX   = pitch[0];
  float pY   = pitch[1];
  float pZ   = pitch[2];
	float pXY  = sqrt(pX*pX + pY*pY        );
	float pYZ  = sqrt(        pY*pY + pZ*pZ);
	float pXZ  = sqrt(pX*pX         + pZ*pZ);
	float pXYZ = sqrt(pX*pX + pY*pY + pZ*pZ);
	
	float min_dist = FLT_MAX; 

	while(!Q1.empty())
	{
		const int x = Q1.front()[0];
		const int y = Q1.front()[1];
		const int z = Q1.front()[2];
		const int I = Q1.front()[3];
		Q1.pop_front();

		int K ;
	  
    //1. vol[I]の26近傍を見て，距離が最小となるものを入れる
    //2. one ring分growさせる
		//calc Dist for voxel (x,y,z) 
    //unisotropic volumeで、seedが曲線状なのでこの実装のほうが多少正確
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  -W-WH; if(       y>0  &&z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I-1  -WH; if(x>0         &&z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I    -WH; if(              z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pZ   );
		K = I+1  -WH; if(x<W-1       &&z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  +W-WH; if(       y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );
		
		K = I-1-W   ; if(x>0  &&y>0         && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXY );
		K = I  -W   ; if(       y>0         && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pY  );
		K = I+1-W   ; if(x<W-1&&y>0         && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXY );
		K = I-1     ; if(x>0                && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pX  );
		K = I+1     ; if(x<W-1              && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pX  );
		K = I-1+W   ; if(x>0  &&y<H-1       && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXY );
		K = I  +W   ; if(       y<H-1       && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pY  );
		K = I+1+W   ; if(x<W-1&&y<H-1       && vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXY );

		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  -W+WH; if(       y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I-1  +WH; if(x>0         &&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I    +WH; if(              z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pZ   );
		K = I+1  +WH; if(x<W-1       &&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXZ  );
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );
		K = I  +W+WH; if(       y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pYZ  );
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&& vol_flg[K]) vol_dist[I] = min( vol_dist[I], vol_dist[K] + pXYZ );

		//grow fronteer to deque
		K = I-1-W-WH; if(x>0  &&y>0  &&z>0	&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I  -W-WH; if(       y>0  &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I+1-W-WH; if(x<W-1&&y>0  &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z-1, K)); vol_flg[K] = 1;}	
		K = I-1  -WH; if(x>0         &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z-1, K)); vol_flg[K] = 1;}	
		K = I    -WH; if(              z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y  , z-1, K)); vol_flg[K] = 1;}
		K = I+1  -WH; if(x<W-1       &&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z-1, K)); vol_flg[K] = 1;}
		K = I-1+W-WH; if(x>0  &&y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z-1, K)); vol_flg[K] = 1;}
		K = I  +W-WH; if(       y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z-1, K)); vol_flg[K] = 1;}
		K = I+1+W-WH; if(x<W-1&&y<H-1&&z>0  &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z-1, K)); vol_flg[K] = 1;}
		
		K = I-1-W   ; if(x>0  &&y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z  , K)); vol_flg[K] = 1;}
		K = I  -W   ; if(       y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z  , K)); vol_flg[K] = 1;}
		K = I+1-W   ; if(x<W-1&&y>0         &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z  , K)); vol_flg[K] = 1;}
		K = I-1     ; if(x>0                &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z  , K)); vol_flg[K] = 1;}
		K = I+1     ; if(x<W-1              &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z  , K)); vol_flg[K] = 1;}
		K = I-1+W   ; if(x>0  &&y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z  , K)); vol_flg[K] = 1;}
		K = I  +W   ; if(       y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z  , K)); vol_flg[K] = 1;}
		K = I+1+W   ; if(x<W-1&&y<H-1       &&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z  , K)); vol_flg[K] = 1;}

		K = I-1-W+WH; if(x>0  &&y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y-1, z+1, K)); vol_flg[K] = 1;}
		K = I  -W+WH; if(       y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y-1, z+1, K)); vol_flg[K] = 1;}
		K = I+1-W+WH; if(x<W-1&&y>0  &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y-1, z+1, K)); vol_flg[K] = 1;}
		K = I-1  +WH; if(x>0         &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y  , z+1, K)); vol_flg[K] = 1;}
		K = I    +WH; if(              z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y  , z+1, K)); vol_flg[K] = 1;}
		K = I+1  +WH; if(x<W-1       &&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y  , z+1, K)); vol_flg[K] = 1;}
		K = I-1+W+WH; if(x>0  &&y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x-1, y+1, z+1, K)); vol_flg[K] = 1;}
		K = I  +W+WH; if(       y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x  , y+1, z+1, K)); vol_flg[K] = 1;}
		K = I+1+W+WH; if(x<W-1&&y<H-1&&z<D-1&&!vol_flg[K]) { Q2.push_back(EVec4i(x+1, y+1, z+1, K)); vol_flg[K] = 1;}

		min_dist = min( min_dist, vol_dist[I] );
	}
	return min_dist;
}



//Written by Shogo Tsuruoka 11/15
//refactored by Takashi Ijiri 11/29
static void s_calcDistTrans
(
	const deque<EVec4i> &seedPixels,
	const float max_dist, 
	float *vol_dist //allocated
)	
{
	std::cout << "start Distance Transform...\n";
	const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
	const EVec3f pitch  = ImageCore::GetInst()->GetPitch ();

  int W,H,D,WH,WHD;
  std::tie(W,H,D,WH,WHD) = ImageCore::GetInst()->GetResolution5();
	
  //vol_flg -- 0:yet, 1:visited. 
	byte *vol_flg = new byte[WHD];

	for(int i = 0; i < WHD; ++i)
	{
		vol_dist[i] = FLT_MAX;
		vol_flg [i] = 0;
	}
	
	deque<EVec4i> Q1, Q2;

	for (const auto &p : seedPixels)
	{
		Q1.push_back(p);
		vol_dist[ p[3] ] = 0.0;
		vol_flg [ p[3] ] = 1  ;
	}


	if( max_dist * 2 > cuboid[0] ) 
	{
    //R大きい場合は，distfieldの計算は無駄なので全て1にしてしまう
		for(int i = 0; i < WHD; ++i) vol_dist[i] = 1;
	}
	else
	{
		while( !Q1.empty() || !Q2.empty() )
		{
			float min_dist;
			min_dist = s_DistTransGrowFronteer(W,H,D,pitch, Q1,Q2, vol_dist, vol_flg);
			if( min_dist > max_dist) break;
			min_dist = s_DistTransGrowFronteer(W,H,D,pitch, Q2,Q1, vol_dist, vol_flg);			
			if( min_dist > max_dist) break;
		}
	}

	delete[] vol_flg;
  std::cout << "...Distance Transform Done\n";
}



//vol_f
//入力時 : 0:locked, 1:yet fixed
//出力時 : 0:locked, 1:yet(ないはず), 2:negative, 255:positive

static void s_LocalRegionGrow
(
	const int W,
	const int H,
	const int D,
	const LRGSeed &seed ,
  const short   *vol  ,
	const EVec3f  &pitch,

	byte* vol_f
)
{
	if( seed.m_cps.size() == 0 )
  {
    std::cout << "never comes here, s_LocalRegionGrow\n";
    return;
  }
  
  const int WH = W*H;

	//1 seed --> seed pixel (最初と最後の二点 + ポリラインをsampling
	deque<EVec4i> Q;

	Q.push_back( ImageCore::GetInst()->GetVoxelIndex4i( seed.m_cps.front() ) );
	Q.push_back( ImageCore::GetInst()->GetVoxelIndex4i( seed.m_cps.back () ) );

  float len = pitch.norm() * 2;
	for (int i = 1; i < seed.m_cps.size(); ++i)
	{
		const EVec3f &p1    = seed.m_cps[i - 1];
		const EVec3f &p2    = seed.m_cps[  i  ];
		const EVec3f dir    = (p2 - p1).normalized();
		const float  length = (p2 - p1).norm();

		for (float step_len = 0; step_len < length; step_len += pitch[0] * 0.5f) 
    {
      Q.push_back( ImageCore::GetInst()->GetVoxelIndex4i(p1 + step_len * dir) );
    }
	}


	//2 distance transform
	const byte    VALUE    = seed.m_flg_fore ? 255 : 2;
	const float   max_dist = seed.m_radius ;
	const EVec3f &seedP    = seed.m_cps[0] ;
	const float   minv     = seed.m_min_v  ;
	const float   maxv     = seed.m_max_v  ;
  
	float *dist = new float[ W*H*D ];
  s_calcDistTrans( Q, max_dist, dist);

	//3  region growing for pix dist[i] < max_dist
	for (const auto &p : Q)
	{
		if (vol_f[p[3]] == 1) vol_f[p[3]] = VALUE;
	}

	while ( !Q.empty() )
	{
		const int x = Q.front()[0];
		const int y = Q.front()[1];
		const int z = Q.front()[2];
		const int I = Q.front()[3];
		Q.pop_front();
		
		if (vol_f[I] == 0 ) continue;
		
		int K;
    K = I - 1 ; if (x > 0   && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x - 1, y, z, K)); }
		K = I - W ; if (y > 0   && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y - 1, z, K)); }
		K = I - WH; if (z > 0   && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y, z - 1, K)); }
		K = I + 1 ; if (x < W-1 && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x + 1, y, z, K)); }
		K = I + W ; if (y < H-1 && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y + 1, z, K)); }
		K = I + WH; if (z < D-1 && vol_f[K] == 1 && dist[K] < max_dist && minv <= vol[K] && vol[K] <= maxv){ vol_f[K] = VALUE; Q.push_back(EVec4i(x, y, z + 1, K)); }
	}

	delete[] dist;
}


//
// class LRGSeed 
//

const static float diffR[4] = {1   ,0,0,0}, diffB[4] = {0.3f,0.3f,1   ,0}, diffG[4] = {0,  1 ,0,0};
const static float ambiR[4] = {0.5f,0,0,0}, ambiB[4] = {0.3f,0.3f,0.8f,0}, ambiG[4] = {0,0.8f,0,0};
const static float spec [4] = {1   ,1,1,0};
const static float shin [1] = {56.0f};


TMesh LRGSeed::m_cp_mesh = TMesh();
float LRGSeed::m_cp_radius = 1.0;
void LRGSeed::SetCpRadius(float radius)
{
  m_cp_radius = radius;
	m_cp_mesh.initializeIcosaHedron( m_cp_radius );
}

float LRGSeed::GetCpRadius()
{
  return m_cp_radius;
}



void LRGSeed::Draw() const
{
	//draw seed 
	glPointSize(15);
	glEnable(GL_LIGHTING);

	for( const auto& p : m_cps)
	{
		glTranslated( p[0], p[1], p[2] );
		if(m_flg_fore) m_cp_mesh.draw( diffR, ambiR, spec, shin);
		else           m_cp_mesh.draw( diffB, ambiB, spec, shin);
		glTranslated( -p[0], -p[1],-p[2] );
	}
	glDisable(GL_LIGHTING);

}



void LRGSeed::DrawAsActive() const
{
	float r = (m_flg_fore == 1) ? 1.0f : 0;
	float b = (m_flg_fore == 1) ? 0 : 1.0f;

	float spec[4] = { 1,1,1,0.3f };
	float diff[4] = { r,0,b,0.3f };
	float ambi[4] = { 0.3f*r,0,0.3f*b,0.3f };
	float shin[1] = { 64.0f };
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR , spec);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE  , diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT  , ambi);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glDepthMask(false);
	t_drawSphere(m_cps[0], m_radius);

	for (int i = 1; i < (int)m_cps.size(); ++i)
	{
		t_drawCylinder(m_cps[i-1], m_cps[i], m_radius);
		t_drawSphere  (m_cps[ i ], m_radius);
	}

	glDepthMask(true);
	glDisable(GL_BLEND);
	glDisable(GL_LIGHTING);
}


#pragma managed
