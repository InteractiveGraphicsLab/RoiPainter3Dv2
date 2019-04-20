#pragma once

//--------------------------------------------------------------
// class ModeSegGCut  
// GraphCutによるsegmentationを行なうモード
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

  GCutCp(const GCutCp& src)
	{
		m_pos  = src.m_pos ;
		m_vidx = src.m_vidx;
	}	

  GCutCp &operator=(const GCutCp &src)
  {
		m_pos  = src.m_pos ;
		m_vidx = src.m_vidx;
    return *this;
  }
};



//Watershed super pixel node (WsdLevel)
class GCWsdNode
{
public:
	
  //pixel indices
  std::vector<int> m_voxel_ids ;	
	
  //total/average intensity
  float m_total_val;
  float m_mean_val ;
	
  //target for segmentation?
  bool   m_b_enable ;
	
  ~GCWsdNode(){}
  
  GCWsdNode ()
  {
    m_total_val = m_mean_val = 0;
    m_b_enable  =  false;
  }

  GCWsdNode(const GCWsdNode &src)
  {
    Set(src);
  }
  
  GCWsdNode &operator=(const GCWsdNode &src)
  {
    Set(src);
    return *this;
  }
  void Set(const GCWsdNode &src)
  {
    m_total_val = src.m_total_val;   
    m_mean_val  = src.m_mean_val;  
    m_b_enable  = src.m_b_enable;
    m_voxel_ids = src.m_voxel_ids;
  }

  inline void addVoxel(int voxel_idx, float intensity)
  {
    m_voxel_ids.push_back( voxel_idx );
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
    m_idx = vox_idx;
    m_val = intensity;
  }

  void Set( const GCVoxNode &src ){
    m_idx = src.m_idx  ;
    m_val = src.m_val;
  }

};








class ModeSegGCut : public ModeInterface
{
  GlslShaderVolume m_volume_shader;
  GlslShaderCrsSec m_crssec_shader;
  
	//mouse manipuration
	bool m_b_paint_cps     ;
	bool m_b_draw_cutsrtoke;
	std::vector<EVec3f> m_stroke;

	//control points
	TMesh  m_cp_mesh;
	float  m_cp_radius;
	std::vector<GCutCp> m_cps_fore;
  std::vector<GCutCp> m_cps_back;

	//watershad super pixel 
	bool   m_b_wsdnode_initialized;
	bool   m_b_wsdnode_computing  ;
	std::vector<int       >    m_map_vox2wsd   ; // map voxel idx --> wsdNode idx 
	std::vector<GCWsdNode >    m_wsdnodes      ; // wsd nodes
	std::vector<std::set<int>> m_wsdnode_neibor; // 1 ring neighbors of wsdNode[i] (片方向, 小さいラベル値Nodeに大きいラベル値Nodeを追加 --)

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
	void RunGraphCutWsdLv(float lambda);
	void RunGraphCutVoxLv(float lambda, int bandWidth, bool genBundOnlyBack );

	void FinishSegmemntation ();
	void CancelSegmentation ();
	void ClearAllCPs();
  void NewVolLoaded();

private:
	static void InitializeWsdNodes_thread( void *pParam );
};

#pragma managed