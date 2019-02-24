#pragma once


/* ------------------------------------
class GlslShader*

date 2017/07/07
written by Takashi Ijiri, Tomofumi Naritra

This file contains multiple classes managing glsl shaders.
-------------------------------------*/


#include "./COMMON/OglForCLI.h"

#include <string>
using namespace std;



class GlslShaderVolume
{
  const string m_vtxFname  ;
  const string m_frgFname  ;
  GLuint       m_gl2Program;
  bool         m_bInit     ;

public:

  GlslShaderVolume(string vtxFname, string frgFname) :
    m_vtxFname(vtxFname), m_frgFname(frgFname)
  {
    m_bInit = false;
  }
  ~GlslShaderVolume() {}

  void bind
  (
    int UnitID_vol ,//3D 
    int UnitID_gMag,//3D 
    int UnitID_flg ,//3D 
    int UnitID_mask,//3D
    int UnitID_tf  ,//1D
    int UnitID_psu ,//1D
    int UnitID_mskC,//1D
    float  alpha,
    EVec3i reso,
    EVec3f camPos,
    bool   doPsuedo,//do use psuedo color
    bool   doHL     //do high light 
  );

  void unbind()
  {
    glUseProgram(0);
  }
};





class GlslShaderVolumePolar
{
  const string m_vtxFname;
  const string m_frgFname;
  GLuint       m_gl2Program;
  bool         m_bInit;

public:

  GlslShaderVolumePolar(string vtxFname, string frgFname) :
    m_vtxFname(vtxFname), m_frgFname(frgFname)
  {
    m_bInit = false;
  }
  ~GlslShaderVolumePolar() {}

  void bind
  (
    int UnitID_vol ,//3D 
    int UnitID_gMag,//3D 
    int UnitID_flg ,//3D 
    int UnitID_mask,//3D
    int UnitID_tf  ,//1D
    int UnitID_psu ,//1D
    int UnitID_mskC,//1D

    float  alpha, EVec3i reso, EVec3f camPos,
    EVec3f center, float  radi, float  minPhi, float maxPhi,
    bool   doPsuedo
  );

  void unbind()
  {
    glUseProgram(0);
  }
};






class GlslShaderCrsSec
{
  const string m_vtxFname;
  const string m_frgFname;
  GLuint       m_gl2Program;
  bool         m_bInit;

public:

  GlslShaderCrsSec(string vtxFname, string frgFname) :
    m_vtxFname(vtxFname), m_frgFname(frgFname)
  {
    m_bInit = false;
  }
  ~GlslShaderCrsSec() {}

  void bind
  (
    int UnitID_vol ,//3D 
    int UnitID_gMag,//3D 
    int UnitID_flg ,//3D 
    int UnitID_mask,//3D
    int UnitID_mskC,//1D
    EVec3i reso,
    bool   gMag,
    bool   doHL
  );

  void unbind()
  {
    glUseProgram(0);
  }
};


