#pragma once


/* ------------------------------------
class GlslShader*

date 2017/07/07
written by Takashi Ijiri, Tomofumi Naritra

This file contains multiple classes managing glsl shaders.
-------------------------------------*/


#include "./COMMON/OglForCLI.h"

#include <string>

class GlslShaderVolume
{
  const std::string m_vtxshader_fname;
  const std::string m_frgshader_fname;
  GLuint m_gl_program;
  bool m_b_initialized;

public:

  GlslShaderVolume( std::string vtxFname, std::string frgFname ) :
    m_vtxshader_fname( vtxFname ), m_frgshader_fname( frgFname )
  {
    m_b_initialized = false;
  }
  ~GlslShaderVolume() {}

  void Bind
  (
    int UnitID_vol,//3D 
    int UnitID_gMag,//3D 
    int UnitID_flg,//3D 
    int UnitID_mask,//3D
    int UnitID_tf,//1D
    int UnitID_psu,//1D
    int UnitID_mskC,//1D
    float  alpha,
    EVec3i reso,
    EVec3f camPos,
    bool   doPsuedo,//do use psuedo color
    bool   doHL     //do high light 
  );

  void Unbind()
  {
    glUseProgram(0);
  }
};





class GlslShaderVolumePolar
{
  const std::string m_vtxshader_fname;
  const std::string m_frgshader_fname;
  GLuint m_gl_program;
  bool   m_b_initialized;

public:

  GlslShaderVolumePolar( std::string vtxFname, std::string frgFname ) :
    m_vtxshader_fname(vtxFname), m_frgshader_fname(frgFname)
  {
    m_b_initialized = false;
  }
  ~GlslShaderVolumePolar() {}

  void Bind
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

  void Unbind()
  {
    glUseProgram(0);
  }
};






class GlslShaderCrsSec
{
  const std::string m_vtxshader_fname;
  const std::string m_frgshader_fname;
  GLuint  m_gl_program;
  bool    m_b_initialized;

public:

  GlslShaderCrsSec( std::string vtxFname, std::string frgFname ) :
    m_vtxshader_fname(vtxFname), m_frgshader_fname(frgFname)
  {
    m_b_initialized = false;
  }
  ~GlslShaderCrsSec() {}

  void Bind
  (
    int UnitID_vol,//3D 
    int UnitID_gMag,//3D 
    int UnitID_flg,//3D 
    int UnitID_mask,//3D
    int UnitID_mskC,//1D
    EVec3i reso,
    bool   gMag,
    bool   doHL
  );

  void Unbind()
  {
    glUseProgram(0);
  }
};


