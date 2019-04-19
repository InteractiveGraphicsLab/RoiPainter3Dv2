
//------------------------------------
//class ImageCore
//
//2018/10/31 
//written by Takashi Ijiri
//
//This class manages following images,
//+ volumetric image
//+ maskId     image
//+ transfer functions (as images)
//-------------------------------------



#pragma once

#include "./COMMON/OglForCLI.h"
#include "./COMMON/OglImage.h"
#include "./COMMON/tmath.h"
#include "./COMMON/tmesh.h"

#include <vector>
#include <string>
#include <iostream>

#pragma unmanaged


class MaskData
{

public:
  std::string m_name     ;
  TMesh  m_surface     ;
  EVec3i m_color    ;
  double m_alpha    ;
  bool   m_b_drawsurface;
  bool   m_b_locked     ;

	MaskData(std::string _name, EVec3i _color, double _alpha, bool _bRendSurf, bool _lock = false) 
	{
		m_name      = _name;
		m_color     = _color;
		m_alpha     = _alpha;
		m_b_drawsurface = _bRendSurf;
		m_b_locked   = _lock;
	}


	void Set(const MaskData& v)
	{
		m_name    = v.m_name;
		m_surface = v.m_surface;
		m_color   = v.m_color;
		m_alpha   = v.m_alpha;
		m_b_drawsurface = v.m_b_drawsurface;
		m_b_locked      = v.m_b_locked;
	}

	MaskData( const MaskData& v)
	{
		Set(v);
	}
	
	MaskData& operator=(const MaskData& v)
	{
		Set(v);
		return *this;
	}

};



class ImageCore
{  

private:
	//volume info 
	EVec3i      m_resolution ;
	EVec3f      m_pitch;
  EVec2i      m_vol_minmax;
	std::string m_filepath;

public:
	//volume images
	short      *m_vol_orig  ; // original volume (1D array representation)
	float      *m_vol_origgm; // gradiente magnitude volume  (1D array representation)
	OglImage3D  m_vol       ; // 8 bit bolume (tone mapping) 
	OglImage3D  m_vol_flag  ; // 8 bit flag volume 
	OglImage3D  m_vol_mask  ; // 8 bit mask volume  
	OglImage3D  m_vol_gm    ; // 8 bit gradient magnitude volume


	OglImage1D<CH_RGBA> m_img_maskcolor ; // func: maskID    --> color

	int m_active_mask_id; // -1:none, 0...:maskID
	std::vector<MaskData> m_mask_data     ;


  //singleton
private:
  ImageCore();
public:
	static ImageCore* GetInst(){ static ImageCore p; return &p;}


  //update opengl volumes by linear tone mapping 
	void UpdateOGLVolume( short windowlv_min,  short windowlv_max);

	//I/O Loaders volume 
	bool  LoadVolume   (std::vector<std::string> fnames, std::string fext);
	bool  LoadVolume   (std::string fname              , std::string fext) ;
	void  LoadMask     (const char *fname);
	void  SaveMask     (const char *fname);
	void  SaveMaskAsFav(const char *fname);
  void  SaveVolumeAsTraw3dss(const char *fname);

	//getter & setter for resolution and pitch
	EVec3f GetCuboid() { 
    return EVec3f( (float)(m_resolution[0] * m_pitch[0]), 
                   (float)(m_resolution[1] * m_pitch[1]), 
                   (float)(m_resolution[2] * m_pitch[2])); 
  }

	EVec3i GetResolution() { return m_resolution; }
  int GetNumVoxels(){ return m_resolution[0] * m_resolution[1] * m_resolution[2];}

  std::string GetFilePath(){ return m_filepath;}

	//getter/setter for pitch 
	EVec3f GetPitch()  { return m_pitch; }
	float  GetPitchW() { return m_pitch[0]; }
	float  GetPitchH() { return m_pitch[1]; }
	float  GetPitchD() { return m_pitch[2]; }

  void   SetPitch (const EVec3f pitch){ m_pitch = pitch;}
  void   SetPitchW(const float &pW) { m_pitch[0] = pW; }
	void   SetPitchH(const float &pH) { m_pitch[1] = pH; }
	void   SetPitchD(const float &pD) { m_pitch[2] = pD; }

	//volume min/mac
	EVec2i GetVolMinMax() { return m_vol_minmax; }

  int GetVoxelIndex(const EVec3f& position)
  {
    const int x = std::min(m_resolution[0] - 1, (int)(position[0] / m_pitch[0]));
    const int y = std::min(m_resolution[1] - 1, (int)(position[1] / m_pitch[1]));
    const int z = std::min(m_resolution[2] - 1, (int)(position[2] / m_pitch[2]));
    return x + y * m_resolution[0] + z * m_resolution[0] * m_resolution[1];
  }

  EVec4i GetVoxelIndex4i(const EVec3f& position)
  {
    EVec4i v;
    v[0] = std::min(m_resolution[0] - 1, (int)(position[0] / m_pitch[0]));
    v[1] = std::min(m_resolution[1] - 1, (int)(position[1] / m_pitch[1]));
    v[2] = std::min(m_resolution[2] - 1, (int)(position[2] / m_pitch[2]));
    v[3] = v[0] + v[1] * m_resolution[0] + v[2] * m_resolution[0] * m_resolution[1];
    return v;
  }


  short GetVoxelValue(const EVec3f& position)
  {
    return m_vol_orig[ GetVoxelIndex(position) ];
  }

  //generate new region by using all voxels with (m_volFlg[i] == 255) 
  void StoreForegroundAsNewMask();
  
  // manipuration for active (user-selected) mask id
  void ActiveMask_SetLocked  (const bool   tf    );
  void ActiveMask_SetRendSurf(const bool   tf    );
  void ActiveMask_SetAlpha   (const double alpha );
  void ActiveMask_SetColor   (const EVec3i &c    );
  void ActiveMask_Delete   ( );
  void ActiveMask_Marge    (const int &trgtMaskID);
  void ActiveMask_Erode    ( );
  void ActiveMask_Dilate   ( );
  void ActiveMask_FillHole ( );
  void ActiveMask_ExportObj(const std::string &fname);


  void InitializeVolFlgByLockedMask();

private:
	void UpdateGradMagnituteVolume();
};

#pragma managed




















