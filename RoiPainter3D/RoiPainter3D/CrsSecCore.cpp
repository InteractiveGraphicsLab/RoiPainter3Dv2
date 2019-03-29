#include "CrsSecCore.h"

#include "./COMMON/OglForCLI.h"
#include <map>
#include <vector>

using namespace std;


#pragma unmanaged


CrssecCore::CrssecCore()
{
  m_plane_xy = m_plane_yz = m_plane_zx = 0.6f;
}


CrssecCore::~CrssecCore()
{
}


//!!NOTE!! shader will be unbinded in this function
void CrssecCore::DrawCrssec
(
  bool b_draw_xy, 
  bool b_draw_yz, 
  bool b_draw_zx, 
  const EVec3f &cuboid
)
{
  glDisable(GL_CULL_FACE);

  //draw curved cross section
  for (int i = 0; i < m_curve_crssec.m_vSize; ++i)
  {
    m_curve_crssec.m_vTexCd[i][0] = m_curve_crssec.m_vVerts[i][0] / cuboid[0];
    m_curve_crssec.m_vTexCd[i][1] = m_curve_crssec.m_vVerts[i][1] / cuboid[1];
    m_curve_crssec.m_vTexCd[i][2] = m_curve_crssec.m_vVerts[i][2] / cuboid[2];
  }
  m_curve_crssec.draw();

  const int idxQ[1][4] = { { 0,1,2,3 } };
  const int idxF[4][2] = { {0,1}, {1,2},{2,3},{3,0} };

  float cx = cuboid[0], x = cx * m_plane_yz;
  float cy = cuboid[1], y = cy * m_plane_zx;
  float cz = cuboid[2], z = cz * m_plane_xy;
  float vertex_xy[4][3] = { { 0, 0, z }, {cx, 0, z},{cx,cy, z}, { 0,cy, z} };
  float vertex_yz[4][3] = { { x, 0, 0 }, { x,cy, 0},{ x,cy,cz}, { x, 0,cz} };
  float vertex_zx[4][3] = { { 0, y, 0 }, { 0, y,cz},{cx, y,cz}, {cx, y, 0} };
  float texturecd_xy[4][3] = { { 0, 0, m_plane_xy },{ 1, 0, m_plane_xy },{ 1, 1, m_plane_xy },{ 0, 1, m_plane_xy } };
  float texturecd_yz[4][3] = { { m_plane_yz, 0, 0 },{ m_plane_yz, 1, 0 },{ m_plane_yz, 1, 1 },{ m_plane_yz, 0, 1 } };
  float texturecd_zx[4][3] = { { 0, m_plane_zx, 0 },{ 0, m_plane_zx, 1 },{ 1, m_plane_zx, 1 },{ 1, m_plane_zx, 0 } };

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glLineWidth(2);
  glColor3d(1, 1, 1);

  if (b_draw_xy) {
    glVertexPointer  (3, GL_FLOAT, 0, vertex_xy);
    glTexCoordPointer(3, GL_FLOAT, 0, texturecd_xy);
    glDrawElements(GL_QUADS, 4 * 1, GL_UNSIGNED_INT, idxQ);
  }
  if (b_draw_yz) {
    glVertexPointer  (3, GL_FLOAT, 0, vertex_yz);
    glTexCoordPointer(3, GL_FLOAT, 0, texturecd_yz);
    glDrawElements(GL_QUADS, 4 * 1, GL_UNSIGNED_INT, idxQ);
  }

  if (b_draw_zx) {
    glVertexPointer  (3, GL_FLOAT, 0, vertex_zx);
    glTexCoordPointer(3, GL_FLOAT, 0, texturecd_zx);
    glDrawElements(GL_QUADS, 4 * 1, GL_UNSIGNED_INT, idxQ);
  }

  glUseProgram(0);

  glLineWidth(3);
  glColor3d(1, 1, 1);

  if (b_draw_xy) {
    glVertexPointer(3, GL_FLOAT, 0, vertex_xy);
    glDrawElements(GL_LINES, 4 * 2, GL_UNSIGNED_INT, idxF);
  }

  if (b_draw_yz) {
    glVertexPointer(3, GL_FLOAT, 0, vertex_yz);
    glDrawElements(GL_LINES, 4 * 2, GL_UNSIGNED_INT, idxF);
  }
  if (b_draw_zx) {
    glVertexPointer(3, GL_FLOAT, 0, vertex_zx);
    glDrawElements(GL_LINES, 4 * 2, GL_UNSIGNED_INT, idxF);
  }
  if (m_curve_crssec.m_vSize != 0) {
    glVertexPointer(3, GL_FLOAT, 0, m_curve_crssec.m_vVerts);
    glDrawElements(GL_LINES, (int)m_curve_crssec_edge.size() * 2, GL_UNSIGNED_INT, m_curve_crssec_edge.data());
  }

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}



CRSSEC_ID CrssecCore::PickCrssec
(
  bool  b_pick_xy, 
  bool  b_pick_yz, 
  bool  b_pick_zx,
  bool  b_pick_curvecrssec, //curved surfaceもピックするか？
  const EVec3f &cuboid, 
  const EVec3f &ray_pos, 
  const EVec3f &ray_dir, 
  EVec3f &picked_position
)
{
  double minDist = DBL_MAX;
  CRSSEC_ID id = CRSSEC_NON;
  const float plnYZ = cuboid[0] * m_plane_yz;
  const float plnZX = cuboid[1] * m_plane_zx;
  const float plnXY = cuboid[2] * m_plane_xy;

  bool flg[3] = { b_pick_xy, b_pick_yz, b_pick_zx };

  for (int i = 0; i < 3; ++i) if (flg[i])
  {
    EVec3f p[4];
    if (i == 0) { p[0] << 0, 0, plnXY;  p[1] << cuboid[0], 0, plnXY;  p[2] << cuboid[0], cuboid[1], plnXY;  p[3] << 0, cuboid[1], plnXY; }
    if (i == 1) { p[0] << plnYZ, 0, 0;  p[1] << plnYZ, cuboid[1], 0;  p[2] << plnYZ, cuboid[1], cuboid[2];  p[3] << plnYZ, 0, cuboid[2]; }
    if (i == 2) { p[0] << 0, plnZX, 0;  p[1] << 0, plnZX, cuboid[2];  p[2] << cuboid[0], plnZX, cuboid[2];  p[3] << cuboid[0], plnZX, 0; }

    EVec3f pos;
    if (!t_intersectRayToQuad(ray_pos, ray_dir, p[0], p[1], p[2], p[3], pos))
      continue;

    double d = (pos - ray_pos).norm();
    if (d < minDist) {
      minDist = d;
      id = (i == 0) ? CRSSEC_XY : (i == 1) ? CRSSEC_YZ : CRSSEC_ZX;
      picked_position = pos;
    }
  }

  //pick curved cross section

  EVec3f pos;

  if (b_pick_curvecrssec && m_curve_crssec.pickByRay(ray_pos, ray_dir, pos) && (pos - ray_pos).norm() < minDist) {
    id = CRSSEC_CURVE;
    picked_position = pos;
  }

  return id;
}




CRSSEC_ID CrssecCore::PickCrssec(
    bool  b_pick_xy, 
    bool  b_pick_yz, 
    bool  b_pick_zx,
    bool  b_pick_curvecrssec, //curved surfaceもピックするか？
    const EVec3f &cuboid, 
    const EVec3f &ray_pos, 
    const EVec3f &ray_dir
)
{
  EVec3f p;
  return PickCrssec(b_pick_xy, b_pick_yz, b_pick_zx, b_pick_curvecrssec, cuboid, ray_pos, ray_dir, p);
}




void CrssecCore::MoveCrssec(EVec3i reso, EVec3f pitch, CRSSEC_ID id, short delta)
{
  if      (id == CRSSEC_XY) m_plane_xy = (float)min(1.0f, max(0.f, m_plane_xy + delta / 120.0f / (float)reso[0]));
  else if (id == CRSSEC_YZ) m_plane_yz = (float)min(1.0f, max(0.f, m_plane_yz + delta / 120.0f / (float)reso[1]));
  else if (id == CRSSEC_ZX) m_plane_zx = (float)min(1.0f, max(0.f, m_plane_zx + delta / 120.0f / (float)reso[2]));
  else if (id == CRSSEC_CURVE) m_curve_crssec.Translate(m_curve_crssec_norm * delta / 120.0f * pitch[0]);
}






void t_DrawCoordinateAxis()
{
  static const float col[4][3] = { {0,0,0},{ 1,0,0},{ 0, 1,0},{0,0, 1} };
  static const float vtx[4][3] = { {0,0,0},{10,0,0},{ 0,10,0},{0,0,10} };
  static const int   idx[3][2] = { {0,1  },{ 0,2  },{ 0,3   } };

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glColorPointer(3, GL_FLOAT, 0, col);
  glVertexPointer(3, GL_FLOAT, 0, vtx);
  glDrawElements(GL_LINES, 3 * 2, GL_UNSIGNED_INT, idx);

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
}



void t_DrawCuboidFrame(const EVec3f &cuboid)
{
  float x = cuboid[0];
  float y = cuboid[1];
  float z = cuboid[2];

  float vtx[8][3] = {
    {0,0,0},{x,0,0}, {x,y,0}, {0,y,0},
    {0,0,z},{x,0,z}, {x,y,z}, {0,y,z}
  };

  static const int idx[12][2] = {
    { 0,1 },{ 1,2 },{ 2,3 },{ 3,0 },
    { 0,4 },{ 1,5 },{ 2,6 },{ 3,7 },
    { 4,5 },{ 5,6 },{ 6,7 },{ 7,4 },
  };

  glDisable(GL_LIGHTING);
  glColor3d(1, 1, 0.2);
  glLineWidth(4);

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vtx);
  glDrawElements(GL_LINES, 12 * 2, GL_UNSIGNED_INT, idx);
  glDisableClientState(GL_VERTEX_ARRAY);
}



class SortLessEq
{
public:
  bool operator()(const pair<float, int>& rsLeft, const pair<float, int>& rsRight) const
  {
    return rsLeft.first >= rsRight.first;
  }
};



void t_DrawCuboidSlices(const int num_slice, const EVec3f &cam_pos, const EVec3f &cam_center, const EVec3f &cuboid)
{
  EVec3f camRay = (cam_center - cam_pos).normalized();
  const float  cx = (float)cuboid[0];
  const float  cy = (float)cuboid[1];
  const float  cz = (float)cuboid[2];


  static int one_ring[8][3] = {
    { 3,4,1 },{ 0,5,2 },{ 1,6,3 },{ 2,7,0 },
    { 5,0,7 },{ 6,1,4 },{ 7,2,5 },{ 4,3,6 }
  };

  const EVec3f p[8] = {
    EVec3f(0,0,0), EVec3f(cx,0,0), EVec3f(cx,cy, 0), EVec3f(0,cy,0),
    EVec3f(0,0,cz), EVec3f(cx,0,cz), EVec3f(cx,cy,cz), EVec3f(0,cy,cz)
  };

  const float p_depth[8] = {
    camRay.dot(p[0] - cam_pos), camRay.dot(p[1] - cam_pos), camRay.dot(p[2] - cam_pos), camRay.dot(p[3] - cam_pos),
    camRay.dot(p[4] - cam_pos), camRay.dot(p[5] - cam_pos), camRay.dot(p[6] - cam_pos), camRay.dot(p[7] - cam_pos)
  };

  //sort 8 points by distance from camP --> verts
  multimap<float, int> tmp_verts;
  for (auto i = 0; i < 8; ++i) 
  {
    tmp_verts.insert( make_pair(p_depth[i], i) );
  }

  vector<pair<float, int>> verts;
  for (auto it = tmp_verts.rbegin(); it != tmp_verts.rend(); ++it)
  {
    verts.push_back(*it);
  }

  //gen edges for rendering
  vector < vector< EVec2i > > TargetEdges(7);
  vector < byte > vtxUsed(8, false);

  for (int i = 0; i < 7; ++i) 
  {
    int  pivIdx = verts[i].second;
    int  *ringI = one_ring[pivIdx];

    vtxUsed[pivIdx] = true;

    vector<EVec2i> &edgesI = TargetEdges[i];

    if (i == 0) {
      edgesI.push_back(EVec2i(pivIdx, ringI[0]));
      edgesI.push_back(EVec2i(pivIdx, ringI[1]));
      edgesI.push_back(EVec2i(pivIdx, ringI[2]));
      continue;
    }

    bool vertexAlreadyComputed = false;
    for (const auto it : TargetEdges[i - 1]) {

      if (it[0] != pivIdx && it[1] != pivIdx) {
        edgesI.push_back(it);
      }
      else if (!vertexAlreadyComputed) {
        vertexAlreadyComputed = true;
        if (ringI[0] == it[0] || ringI[0] == it[1]) {
          if (!vtxUsed[ringI[1]]) edgesI.push_back(EVec2i(pivIdx, ringI[1]));
          if (!vtxUsed[ringI[2]]) edgesI.push_back(EVec2i(pivIdx, ringI[2]));
        }
        else if (ringI[1] == it[0] || ringI[1] == it[1]) {
          if (!vtxUsed[ringI[2]]) edgesI.push_back(EVec2i(pivIdx, ringI[2]));
          if (!vtxUsed[ringI[0]]) edgesI.push_back(EVec2i(pivIdx, ringI[0]));
        }
        else {
          if (!vtxUsed[ringI[0]]) edgesI.push_back(EVec2i(pivIdx, ringI[0]));
          if (!vtxUsed[ringI[1]]) edgesI.push_back(EVec2i(pivIdx, ringI[1]));
        }
      }
    }
  }


  struct Poly {
    unsigned int p[3];
    Poly(unsigned int i0, unsigned int i1, unsigned int i2)
    {
      p[0] = i0;
      p[1] = i1;
      p[2] = i2;
    }
  };

  vector<EVec3f> vtx;
  vector<EVec3f> tex;
  vector<Poly>   ids;
  vtx.reserve(num_slice * 6);
  tex.reserve(num_slice * 6);
  ids.reserve(num_slice * 4);

  const float  depthStart = verts.front().first;
  const float  depthEnd = max(.0f, verts.back().first);
  const float  SliceInterval = (depthStart - depthEnd) / num_slice;

  for (float depth = depthStart; depth > depthEnd; depth -= SliceInterval) {
    int pivIdx = 0;
    for (int kk = 0; kk < verts.size() - 1; ++kk) if (verts[kk].first >= depth && depth > verts[kk + 1].first)
    {
      pivIdx = kk;
      break;
    }

    if (pivIdx < 0 || pivIdx >= 7)
    {
      std::cout << "strange error";
      break;
    }

    const int I = (int)vtx.size();

    for (int kk = 0; kk < (int)TargetEdges[pivIdx].size(); ++kk)
    {
      const int idx_0 = TargetEdges[pivIdx][kk][0];
      const int idx_1 = TargetEdges[pivIdx][kk][1];
      const float  d0 = p_depth[idx_0];
      const float  d1 = p_depth[idx_1];

      if (fabs(d0 - d1) < 0.00001) break;

      const EVec3f &pos = p[idx_0] + (p[idx_1] - p[idx_0]) * (depth - d0) / (d1 - d0);
      vtx.push_back( pos );
      tex.push_back( EVec3f(pos[0] / cx, pos[1] / cy, pos[2] / cz));
      if (kk > 1) ids.push_back( Poly(I, I + kk - 1, I + kk) );
    }
  }


  glDisable(GL_CULL_FACE);

  glDisable(GL_LIGHTING);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_DEPTH_TEST);
  glDepthMask(false);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glVertexPointer(3, GL_FLOAT, 0, vtx.data());
  glTexCoordPointer(3, GL_FLOAT, 0, tex.data());
  glDrawElements(GL_TRIANGLES, 3 * (int)ids.size(), GL_UNSIGNED_INT, ids.data());
  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  glDisable(GL_DEPTH_TEST);
  glDepthMask(true);

}





































/*-------------------------------------------------------------------------------------------------
bool t_intersectRayToCuboid
const EVec3f &rayP　: input ray point
const EVec3f &rayD  : input ray direction
EVec3f &nearP : output intersection point (close to rayP)
EVec3f &farP  : output intersection point (far from rayP)
--------------------------------------------------------------------------------------------------*/
bool t_intersectRayToCuboid(
  const EVec3f &cube,
  const EVec3f &rayP,
  const EVec3f &rayD,
  EVec3f &nearP,
  EVec3f &farP)
{
  bool hasNear = false;
  bool hasFar = false;

  for (int xyz = 0; xyz < 3; ++xyz)
  {
    if (rayD[xyz] == 0) continue;

    float t1 = (cube[xyz] - rayP[xyz]) / rayD[xyz];
    float t2 = (0 - rayP[xyz]) / rayD[xyz];
    EVec3f tmpP1 = rayP + t1 * rayD;
    EVec3f tmpP2 = rayP + t2 * rayD;

    if (t_bInWindow3D(EVec3f(0, 0, 0), cube, tmpP1, 0.001f)) {
      if (rayD[xyz] < 0) {
        nearP = tmpP1;
        hasNear = true;
      }
      else {
        farP = tmpP1;
        hasFar = true;
      }
    }
    if (t_bInWindow3D(EVec3f(0, 0, 0), cube, tmpP2, 0.001f)) {
      if (rayD[xyz] > 0) {
        nearP = tmpP2;
        hasNear = true;
      }
      else {
        farP = tmpP2;
        hasFar = true;
      }
    }
  }

  return hasNear && hasFar;
}








void CrssecCore::GenerateCurvedCrssec(const EVec3f &cube, const EVec3f &camP, const vector<EVec3f> &_stroke)
{
  m_curve_crssec.clear();
  m_curve_crssec_edge.clear();

  if (_stroke.size() <= 5) return;

  // smoothing and resampling stroke
  vector<EVec3f> tmp = _stroke, stroke3D;
  t_verts_Smoothing(2, tmp);

  int strN = min(100, max(30, (int)tmp.size()));
  t_verts_ResampleEqualInterval(strN, tmp, stroke3D);

  //gen surface
  vector< EVec3f > Vs;
  vector< TPoly  > Ps;

  if (t_bInWindow3D(EVec3f(0, 0, 0), cube, camP))
  {
    Vs.push_back(camP);

    bool isTherePreVerts = false;

    for (const auto &p : stroke3D)
    {
      EVec3f p0, p1, dir = (p - camP).normalized();

      if (t_intersectRayToCuboid(cube, camP, dir, p0, p1)) {
        Vs.push_back(p1);
        if (isTherePreVerts) {
          const int piv = (int)Vs.size() - 1;
          Ps.push_back(TPoly(piv, 0, piv - 1));
          m_curve_crssec_edge.push_back(EVec2i(piv, piv - 1));
        }
        isTherePreVerts = true;
      }
      else {
        isTherePreVerts = false;
      }
    }

    if (!Ps.empty()) {
      m_curve_crssec_edge.push_back(EVec2i(0, 1));
      m_curve_crssec_edge.push_back(EVec2i(0, (int)Vs.size() - 1));
    }
  }
  else
  {
    bool isTherePreVerts = false;
    for (const auto &p : stroke3D) {

      EVec3f p0, p1, dir = (p - camP).normalized();

      if (t_intersectRayToCuboid(cube, camP, dir, p0, p1)) {
        Vs.push_back(p0);
        Vs.push_back(p1);

        if (isTherePreVerts) {
          int piv = (int)Vs.size() - 1;
          Ps.push_back(TPoly(piv, piv - 1, piv - 2));
          Ps.push_back(TPoly(piv - 1, piv - 3, piv - 2));
          m_curve_crssec_edge.push_back(EVec2i(piv, piv - 2));
          m_curve_crssec_edge.push_back(EVec2i(piv - 1, piv - 3));
        }
        isTherePreVerts = true;
      }
      else {
        isTherePreVerts = false;
      }
    }
  }

  m_curve_crssec.initialize(Vs, Ps);


  m_curve_crssec_norm << 0, 0, 0;
  for (int i = 0; i < m_curve_crssec.m_vSize; ++i) m_curve_crssec_norm += m_curve_crssec.m_vNorms[i];
  m_curve_crssec_norm.normalize();

}


#pragma managed



