#pragma once

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

using namespace::std;





class LRGseed
{
  

public:
	LRGseed(EVec3f pos = EVec3f(0,0,0), short minV = 0, short maxV = 0, int flg = 0, float rad = 0)
	{
		m_pos.push_back(pos);
		m_minV = minV;
		m_maxV = maxV;
		m_flg  = flg;
		m_rad  = rad;
	}

	//m_pos.size() == 1: sphere, > 1: cylinder 
	vector < EVec3f > m_pos;

	short  m_minV, m_maxV;
  float  m_rad;
	int    m_flg;

  void modifyRadius(float delta){
    m_rad += delta;
    if( m_rad < 0.1 ) m_rad  = 0.1f; 
  }

	void draw        (const TMesh &cp) const;
	void drawAsActive(const TMesh &cp) const;
};





#pragma unmanaged

class ModeSegLocalRGrow : public ModeInterface
{
  // shader 
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;

	// CPサイズとCP描画用のメッシュ
	float  m_CpSize;
	TMesh  m_CPmesh;

  //seed id for moving (seed_id, cp_id)
	EVec2i m_moveSeedCpId;

	//cut stroke
	bool m_bDrawCutStr;
	vector<EVec3f> m_stroke;

public:
  // seeds and index of activated Seed (when -1 all seeds are non-activated)
  int m_ActiveSeedIdx; 
	vector<LRGseed> m_seeds;


private:
  ModeSegLocalRGrow();
public:
  ~ModeSegLocalRGrow();

  static ModeSegLocalRGrow* getInst() { 
    static ModeSegLocalRGrow p; 
    return &p; 
  }

  // overload functions ---------------------------------------------

  void LBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void RBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void MBtnUp    (const EVec2i &p, OglForCLI *ogl);
  void LBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDown  (const EVec2i &p, OglForCLI *ogl);
  void LBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void RBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MBtnDclk  (const EVec2i &p, OglForCLI *ogl);
  void MouseMove (const EVec2i &p, OglForCLI *ogl);
  void MouseWheel(const EVec2i &p, short zDelta, OglForCLI *ogl);

  void keyDown(int nChar);
  void keyUp  (int nChar);

  bool canEndMode();
  void startMode ();
  void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

  void finishSegmentation();
  void cancelSegmentation();
  void runLocalRegionGrow();

private:

  void dblclkToAddNewSeed( const EVec3f &rayP, const EVec3f &rayD, bool flgPosNeg   );
	void dblclkToAddNewCp  ( const EVec3f &rayP, const EVec3f &rayD, const int seedId );
	void dblclkToRemoveCP  ( EVec2i seedIdCpId );
	void s_LocalRegionGrow(const LRGseed &seed,const int W,const int H,const int D,const short *img,const EVec3f &pitch,byte* flg,int a);

  EVec2i pickSeeds(const EVec3f &rayP, const EVec3f &rayD);
};

#pragma managed



