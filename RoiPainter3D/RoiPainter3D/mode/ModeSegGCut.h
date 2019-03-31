#pragma once

//--------------------------------------------------------------
// class ModeSegGCut  
// GraphCutÇ…ÇÊÇÈsegmentationÇçsÇ»Ç§ÉÇÅ[Éh
// 
//--------------------------------------------------------------

#include "ModeInterface.h"
#include "GlslShader.h"
#include <vector>

#pragma unmanaged


class GCutCp
{
public:
	EVec3f m_pos ;
	EVec4i m_vidx;

	GCutCp(EVec3f p, EVec4i vidx)
	{
		m_pos  = p   ;
		m_vidx = vidx;
	}	
};




//Watershed super pixel node (WsdLevel)
class WsdNode
{
public:
	
  //pixel indices
  std::vector<int> m_voxel_ids ;	
	
  //total/average intensity
  float m_total_val;
  float m_mean_val ;
	
  //target for segmentation?
  bool   m_b_enable ;
	
  ~WsdNode(){}
  
  WsdNode ()
  {
    m_total_val  = m_mean_val = 0;
    m_b_enable =  false;
  }

  WsdNode(const WsdNode &src)
  {
    Set(src);
  }
  
  WsdNode &operator=(const WsdNode &src)
  {
    Set(src);
    return *this;
  }
  void Set(const WsdNode &src)
  {
    m_total_val = src.m_total_val;   
    m_mean_val  = src.m_mean_val;  
    m_b_enable  = src.m_b_enable;
    m_voxel_ids = src.m_voxel_ids;
  }

  inline void addVoxel(int voxelIdx, float intensity)
  {
    m_voxel_ids.push_back( voxelIdx );
    m_total_val += intensity;
    m_mean_val  = m_total_val / (float) m_voxel_ids.size();
  }
};



//vocel node (vocel Level)
class GCVoxNode
{
public:
  int   m_idx;
  float m_val;


  GCVoxNode(const GCVoxNode &src){
    Set( src );
  }
  
  GCVoxNode( const int vox_idx = 0, const float intensity = 0){
    Set( vox_idx,intensity );
  }

  GCVoxNode &operator=(const GCVoxNode &src)
  {
    Set(src);
    return *this;
  }

  void Set( const int &vox_idx, const float &intensity){
    m_idx   = vox_idx;
    m_val = intensity;
  }

  void Set( const GCVoxNode &src ){
    m_idx   = src.m_idx  ;
    m_val = src.m_val;
  }

};








class ModeSegGCut : public ModeInterface
{
  GlslShaderVolume m_volumeShader;
  GlslShaderCrsSec m_crssecShader;
  
	//mouse manipuration
	bool m_bPaintCP;
	bool m_bDrawCutStr;
	std::vector<EVec3f> m_stroke;

	//control points
	std::vector<GCutCp> m_fCPs, m_bCPs;
	TMesh          m_CpMesh;
	float          m_CpSize;

	//watershad super pixel 
	bool             m_bWsdInitialized;
	bool             m_bWsdComputing  ;
	std::vector<int     > m_map_vox2wsd; // map voxel idx --> wsdNode idx --> 
	std::vector<WsdNode > m_wsdNodes   ; // wsdNodes 
	std::vector<std::set<int>> m_wsdNodeNei ; // neighbors of wsdNode[i] (small idx --> local idx)


  ModeSegGCut();
public:
  ~ModeSegGCut();

  static ModeSegGCut* getInst() { 
    static ModeSegGCut p; 
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

  void KeyDown(int nChar);
  void KeyUp  (int nChar);

  bool CanLeaveMode();
  void StartMode ();
  void DrawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF);

public:
	void runGraphCutWsdLv(float lambda);
	void runGraphCutVoxLv(float lambda, int bandWidth, bool genBundOnlyBack );
	void finishSegm ();
	void cancelSegm ();
	void clearAllCPs();
  void newVolLoaded(){ m_bWsdInitialized = false; }

private:
	static void initWsdNodes_thread( void *pParam );
};

#pragma managed