#pragma once



/* ------------------------------------
class CrsSecCore

date 2018/10/31
written by Takashi Ijiri

This class manages cross sections to visualize volumes.
-------------------------------------*/

#include "./COMMON/tmesh.h"


enum CRSSEC_ID
{
  CRSSEC_NON,
  CRSSEC_XY,
  CRSSEC_YZ,
  CRSSEC_ZX,
  CRSSEC_CURVE
};


class CrssecCore
{
  //plane
  float  m_plane_xy; //[0,1]
  float  m_plane_yz; //[0,1]
  float  m_plane_zx; //[0,1]

  //curved cross section 
  EVec3f m_curve_crssec_norm;
  TMesh          m_curve_crssec;
  vector<EVec2i> m_curve_crssec_edge;

  CrssecCore();
public:
  ~CrssecCore();

  static CrssecCore *getInst()
  {
    static CrssecCore p;
    return &p;
  }

  CRSSEC_ID PickCrssec(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid, const EVec3f &rayP, const EVec3f &rayD);
  CRSSEC_ID PickCrssec(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid, const EVec3f &rayP, const EVec3f &rayD, EVec3f &pos);

  void DrawCrssec(bool bXY, bool bYZ, bool bZX, const EVec3f &cuboid);
  void MoveCrssec(EVec3i reso, EVec3f pitch, CRSSEC_ID id, short Delta);

  //move three planes to fit to the picked 3D position
  void FitCrssecToPosition(const EVec3i &reso, const EVec3f &pitch, const EVec3f &pos, CRSSEC_ID fixId)
  {
    if (fixId != CRSSEC_XY) m_plane_xy = pos[2] / (pitch[2] * reso[2]);
    if (fixId != CRSSEC_YZ) m_plane_yz = pos[0] / (pitch[0] * reso[0]);
    if (fixId != CRSSEC_ZX) m_plane_zx = pos[1] / (pitch[1] * reso[1]);
  }

  void GenerateCurvedCrssec(const EVec3f &cuboid, const EVec3f &camP, const vector<EVec3f> &stroke);

  float GetPlaneXyPosition() { return m_plane_xy; }
  float GetPlaneYzPosition() { return m_plane_yz; }
  float GetPlaneZxPosition() { return m_plane_zx; }

};





void t_drawAxis();
void t_drawFrame(const EVec3f &cuboid);
void t_drawSlices(const int slineNum, const EVec3f &camP, const EVec3f &camF, const EVec3f &cuboid);


inline void t_drawSphere
(
  const EVec3f &pos, //èÍèä
  const float radius
)
{
  const int M = 20;
  const int N = 20;

  glPushMatrix();
  glTranslated(pos[0], pos[1], pos[2]);


  //ç≈â∫ïî	

  glBegin(GL_TRIANGLE_FAN);

  glNormal3d(0, 0, 1);
  glVertex3d(0, 0, -radius);

  for (int i = 0; i <= M; i++)
  {
    double theta = i * (M_PI * 2 / M);
    double phi = M_PI / N - M_PI * 0.5;
    double x = cos(phi) * cos(theta);
    double y = cos(phi) * sin(theta);
    double z = sin(phi);

    glNormal3d(x, y, z);
    glVertex3d(radius * x, radius * y, radius * z);
  }

  glEnd();

  //ì‡ïî
  for (int j = 1; j < N - 1; j++)
  {
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= M; i++)
    {
      double phi = j * M_PI / N - M_PI * 0.5;
      double theta = i * M_PI * 2 / M;
      double x = cos(phi) * cos(theta);
      double y = cos(phi) * sin(theta);
      double z = sin(phi);
      glNormal3d(x, y, z);
      glVertex3d(radius * x, radius * y, radius * z);

      phi = (j + 1) * (M_PI / (double)N) - M_PI * 0.5;
      x = cos(phi) * cos(theta);
      y = cos(phi) * sin(theta);
      z = sin(phi);

      glNormal3d(x, y, z);
      glVertex3d(radius * x, radius * y, radius * z);
    }
    glEnd();
  }

  //ç≈í∏ïî
  glBegin(GL_TRIANGLE_FAN);

  glNormal3d(0, 0, 1);
  glVertex3d(0, 0, radius);

  for (int i = 0; i <= M; i++)
  {
    double phi = (N - 1) * M_PI / N - M_PI * 0.5;
    double theta = i * M_PI * 2 / M;
    double x = cos(phi) * cos(theta);
    double y = cos(phi) * sin(theta);
    double z = sin(phi);

    glNormal3d(x, y, z);
    glVertex3d(radius * x, radius * y, radius * z);
  }

  glEnd();

  glPopMatrix();
}



inline void t_drawCylinder
(
  const EVec3f &p1,
  const EVec3f &p2,
  const float  R
)
{
  const int N = 20;

  EVec3f dir = (p2 - p1).normalized();
  EVec3f zAxis(0, 0, 1);
  EVec3f axis = zAxis.cross(dir).normalized();

  float theta = acos(zAxis.dot(dir));
  Eigen::AngleAxisf Rot(theta, axis);


  vector <EVec3f> v1(N + 1), v2(N + 1), norm(N + 1);
  for (int i = 0; i <= N; i++)
  {
    float t = i * ((float)M_PI * 2.0f / N);
    float x = cos(t);
    float y = sin(t);
    norm[i] = Rot * EVec3f(x, y, 0);
    v1[i] = Rot * EVec3f(R *x, R *y, 0) + p1;
    v2[i] = Rot * EVec3f(R *x, R *y, 0) + p2;
  }

  glBegin(GL_TRIANGLE_STRIP);
  for (int i = 0; i <= N; ++i)
  {
    glNormal3fv(norm[i].data());
    glVertex3fv(v1[i].data());
    glVertex3fv(v2[i].data());
  }
  glEnd();

}
