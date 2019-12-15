
// Copyright (c) 2017, Takashi Ijiri, Tomofumi Narita, Chika Tomiyama
// All rights reserved.
//
//
// This file is released under GNU GPL v3.
//
//
// This class uses Eigen 3.2.8 released under MPL2 without any modification.
// see http://eigen.tuxfamily.org/index.php?title=Main_Page#License
// 
// This class uses Glew released under the Modified BSD
// see http://glew.sourceforge.net/credits.html
// see http://glew.sourceforge.net/glew.txt
//
// Start development at 2017/7/7




#pragma once

#include <windows.h>
#undef min
#undef max


#include <iostream> 

#include "gl/glew.h"
#include <gl/gl.h> 
#include <gl/glu.h> 
#include "tmath.h"

#pragma unmanaged




class OglCameraParam
{
public:
  EVec3f m_pos; //camera position
  EVec3f m_cnt; //camera focus point
  EVec3f m_up ; //camera Up (y-axis) direction 

  OglCameraParam()
  {
    m_pos = EVec3f(0, 0, 10);
    m_cnt = EVec3f(0, 0, 0 );
    m_up  = EVec3f(0, 1, 0 );
  }

  OglCameraParam( const OglCameraParam &src) {
    Copy(src);
  }

  OglCameraParam &operator=(const OglCameraParam &src)
  {
    Copy(src);
    return *this;
  }

  void Copy (const OglCameraParam &src)
  {
    m_pos = src.m_pos;
    m_cnt = src.m_cnt;
    m_up  = src.m_up ;
  }

  
  //camera rotation/zoom/translate by mouse operation 
  // (mouse_dx, mouse_dy) : mouse offsets in 2D
  //  
  void RotateCamera( int mouse_dx, int mouse_dy)
  {
    float theta = -mouse_dx / 200.0f;
    float phi   = -mouse_dy / 200.0f;

    EVec3f x_dir = ((m_cnt - m_pos).cross(m_up)).normalized();
    Eigen::AngleAxisf rotTheta(theta, m_up);
    Eigen::AngleAxisf rotPhi  (  phi, x_dir      );
    m_up  = rotPhi * rotTheta * m_up;
    m_pos = rotPhi * rotTheta * (m_pos - m_cnt) + m_cnt;
  }

  void ZoomCamera( int mouse_dy)
  {
    EVec3f newEyeP = m_pos + mouse_dy / 80.0f * (m_cnt - m_pos);
    if ((newEyeP - m_cnt).norm() > 0.02f) m_pos = newEyeP;
  }
  
  void TranslateCamera(int mouse_dx, int mouse_dy)
  {
    float c = (m_pos - m_cnt).norm() / 900.0f;
    EVec3f x_dir = ((m_pos - m_cnt).cross(m_up)).normalized();
    EVec3f t = c * mouse_dx * x_dir + c * mouse_dy * m_up;
    m_pos += t;
    m_cnt += t;
  }

  void ZoomCameraByWheel(short z_delta)
  {
    EVec3f rayD = (m_cnt - m_pos);
    float  len = rayD.norm();
    
    float offset = z_delta * 0.1f;

    if (offset > len) return;
    rayD /= len;
    m_pos = m_pos + offset * rayD;
  }

  void Set(const EVec3f &pos, const EVec3f &cnt, const EVec3f up)
  { 
    m_pos = pos;
    m_cnt = cnt;
    m_up  = up ;
  }
  
};






class OglForCLI
{
private:

  //windows handles
  HDC   m_hdc;
  HGLRC m_hglrc;

  // Camera position/center/Up direction
  OglCameraParam m_camera; 
  

  // View Size
  bool   m_is_rendering;
  EVec2i m_mouse_position;
  EVec4f m_background_color;

  enum {
    BTN_NON, BTN_TRANS, BTN_ZOOM, BTN_ROT
  } m_mousebtn_state;

public:

  ~OglForCLI() {}

  OglForCLI(HDC dc)
  {
    if (dc == 0) return;

    m_camera.m_pos = EVec3f(0, 0, 10);
    m_camera.m_cnt = EVec3f(0, 0, 0 );
    m_camera.m_up  = EVec3f(0, 1, 0 );
    m_background_color = EVec4f(0, 0, 0, 0.5);

    m_is_rendering = false;

    SetDefaultProperties();


    m_hdc = dc;
    static PIXELFORMATDESCRIPTOR pfd =
    {
      sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
      1,                              // version number
      PFD_DRAW_TO_WINDOW |            // support window
      PFD_SUPPORT_OPENGL |            // support OpenGL
      PFD_DOUBLEBUFFER,               // double buffered
      PFD_TYPE_RGBA,                  // RGBA type
      32,                             // 24-bit color depth
      0, 0, 0, 0, 0, 0,               // color bits ignored
      0,                              // no alpha buffer
      0,                              // shift bit ignored
      0,                              // no accumulation buffer
      0, 0, 0, 0,                     // accum bits ignored
      32,                             // 32-bit z-buffer (or 16, 24 bit)
      0,                              // stencil buffer (no)
      0,                              // auxiliary buffer (no)
      PFD_MAIN_PLANE,                 // main layer
      0,                              // reserved
      0, 0, 0                         // layer masks ignored
    };

    int pfmt = ChoosePixelFormat(m_hdc, &pfd);
    if (pfmt == 0) return;
    if (!SetPixelFormat(m_hdc, pfmt, &pfd)) return;

    // pure Managed だとランタイムでエラーに 
    if ( (m_hglrc = wglCreateContext(m_hdc) ) == 0) return;
    if ( (wglMakeCurrent(m_hdc, m_hglrc)    ) == 0) return;

    GLenum err = glewInit();
    if (err != GLEW_OK) 
    {
      std::cout << "Error: \n" << glewGetErrorString(err) << "\n";
    }

    wglMakeCurrent(0, 0);

    return;
  }


  void oglMakeCurrent() const
  {
    wglMakeCurrent(m_hdc, m_hglrc);
  }


  void OnDrawBegin(
    int    screem_width, 
    int    screen_height, 
    double fovY = 45.0, 
    double view_near = 0.02, 
    double view_far = 700.0)
  {
    if (m_is_rendering) return;

    m_is_rendering = true;
    oglMakeCurrent();

    glViewport(0, 0, screem_width, screen_height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fovY, screem_width / (double)screen_height, view_near, view_far);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(m_camera.m_pos[0], m_camera.m_pos[1], m_camera.m_pos[2],
              m_camera.m_cnt[0], m_camera.m_cnt[1], m_camera.m_cnt[2],
              m_camera.m_up [0], m_camera.m_up [1], m_camera.m_up [2]);
    glClearColor( m_background_color[0], 
                  m_background_color[1], 
                  m_background_color[2], 
                  m_background_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
  }

  void OnDrawEnd()
  {
    glFinish();
    SwapBuffers(m_hdc);
    wglMakeCurrent(NULL, NULL);
    m_is_rendering = false;
  }


  //for rendering for other panel 
  int m_tmp_viewport[4];

  

  void OnDrawBeginByOtherForm(
    HDC hdc,
    int    screem_width, 
    int    screen_height, 
    OglCameraParam cam,
    double fovY = 45.0, 
    double view_near = 0.02, 
    double view_far = 700.0)
  {
    if (m_is_rendering) return;

    m_is_rendering = true;
    wglMakeCurrent(hdc, m_hglrc);

    glGetIntegerv(GL_VIEWPORT, m_tmp_viewport);

    glViewport(0, 0, screem_width, screen_height);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluPerspective(fovY, screem_width / (double)screen_height, view_near, view_far);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    gluLookAt(cam.m_pos[0], cam.m_pos[1], cam.m_pos[2],
              cam.m_cnt[0], cam.m_cnt[1], cam.m_cnt[2],
              cam.m_up [0], cam.m_up [1], cam.m_up [2]);
    glClearColor( m_background_color[0], 
                  m_background_color[1], 
                  m_background_color[2], 
                  m_background_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
  }


  void OnDrawEndByOtherForm(HDC hdc)
  {
    glFinish();
    SwapBuffers(hdc);

    //元の情報を戻す
    glViewport(m_tmp_viewport[0], m_tmp_viewport[1], m_tmp_viewport[2], m_tmp_viewport[3]);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    wglMakeCurrent(NULL, NULL);
    m_is_rendering = false;
  }


  inline bool   IsDrawing() const { return m_is_rendering; }
  inline EVec3f GetCamPos() const { return m_camera.m_pos; }
  inline EVec3f GetCamCnt() const { return m_camera.m_cnt; }
  inline EVec3f GetCamUp()  const { return m_camera.m_up ; }

  inline void   SetCam(const EVec3f &pos, const EVec3f &cnt, const EVec3f &up) 
  { 
    m_camera.Set(pos, cnt, up);
  }

  inline void  SetBgColor(EVec4f bg) 
  { 
    m_background_color = bg; 
  }

  inline void  SetBgColor(float r, float g, float b, float a) 
  { 
    m_background_color << r, g, b, a; 
  }


  //Mouse Listener for Camera manipuration
  void BtnDown_Trans(const EVec2i &p)
  {
    m_mouse_position = p;
    m_mousebtn_state = BTN_TRANS;
  }

  void BtnDown_Zoom(const EVec2i &p)
  {
    m_mouse_position = p;
    m_mousebtn_state = BTN_ZOOM;
  }

  void BtnDown_Rot(const EVec2i &p)
  {
    m_mouse_position = p;
    m_mousebtn_state = BTN_ROT;
  }

  void BtnUp()
  {
    m_mousebtn_state = BTN_NON;
    ReleaseCapture();
  }

  void MouseMove(const EVec2i &p)
  {
    if (m_mousebtn_state == BTN_NON) return;

    int dX = p[0] - m_mouse_position[0];
    int dY = p[1] - m_mouse_position[1];

    if ( m_mousebtn_state == BTN_ROT)
      m_camera.RotateCamera(dX, dY);
    if ( m_mousebtn_state == BTN_ZOOM)
      m_camera.ZoomCamera(dY);
    if ( m_mousebtn_state == BTN_TRANS)
      m_camera.TranslateCamera(dX,dY);

    m_mouse_position = p;
  }


  void ZoomCamByWheel(short z_delta)
  {
    m_camera.ZoomCameraByWheel(z_delta);
  }


  void unProject_correctY(
    const double cx, 
    const double cy, 
    const double depth, 
    double &x, 
    double &y, 
    double &z) const
  {
    if (!m_is_rendering) oglMakeCurrent();
    double modelMat[16], projMat[16];
    int vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
    glGetDoublev(GL_PROJECTION_MATRIX, projMat);
    glGetIntegerv(GL_VIEWPORT, vp);

    gluUnProject(cx, vp[3] - cy, depth, modelMat, projMat, vp, &x, &y, &z);

    if (!m_is_rendering) wglMakeCurrent(NULL, NULL);
  }



  void Project(
    const double inX, 
    const double inY, 
    const double inZ,
          double &outX, 
          double &outY,
          double &outZ) const
  {
    if (!m_is_rendering) oglMakeCurrent();
    double model[16], proj[16];
    int vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, vp);

    gluProject(inX, inY, inZ, model, proj, vp, &outX, &outY, &outZ);
    if (!m_is_rendering) wglMakeCurrent(NULL, NULL);
  }



  inline void GetCursorRay(
    int cx, 
    int cy, 
    EVec3f &rayPos, 
    EVec3f &rayDir) const
  {
    double x1, y1, z1, x2, y2, z2;
    unProject_correctY(cx, cy, 0.01, x1, y1, z1);
    unProject_correctY(cx, cy, 0.2, x2, y2, z2);

    rayPos << (float)x1, (float)y1, (float)z1;
    rayDir << (float)(x2 - x1), (float)(y2 - y1), (float)(z2 - z1);
    rayDir.normalize();
  }



  inline void GetCursorRay(const EVec2i &pt, EVec3f &rayPos, EVec3f &rayDir) const
  {
    GetCursorRay(pt[0], pt[1], rayPos, rayDir);
  }

private:

  void SetDefaultProperties()
  {
    glClearDepth(1.0f);

    //Material 
    float   shin[1] = { 64 };
    EVec4f  spec(1, 1, 1, 0.5), diff(0.5f, 0.5f, 0.5f, 0.5f), ambi(0.5f, 0.5f, 0.5f, 0.5f);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec.data());
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diff.data());
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambi.data());
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shin);

    //lights
    glEnable(GL_LIGHTING);
    EVec4f lPosi[3] = { EVec4f(1000,1000,-1000,1), 
                        EVec4f(-1000,1000,-1000,1), 
                        EVec4f(1000,-1000,-1000,1) };
    EVec4f lambi[3] = { EVec4f(1.0f,1.0f,1.0f,1), 
                        EVec4f(0,0,0,0)         , 
                        EVec4f(0,0,0,0) };
    EVec4f ldiff[3] = { EVec4f(1.0f,1.0f,1.0f,1), 
                        EVec4f(0.5f,0.5f,0.5f,1), 
                        EVec4f(0.5f,0.5f,0.5f,1) };
    EVec4f lspec[3] = { EVec4f(0.3f,0.3f,0.3f,1), 
                        EVec4f(0.3f,0.3f,0.3f,1), 
                        EVec4f(0.3f,0.3f,0.3f,1) };

    for (int i = 0; i < 3; ++i)
    {
      GLenum light = (i == 0) ? GL_LIGHT0 : (i == 1) ? GL_LIGHT1 : GL_LIGHT2;
      glLightfv(light, GL_POSITION, lPosi[i].data());
      glLightfv(light, GL_AMBIENT , lambi[i].data());
      glLightfv(light, GL_DIFFUSE , ldiff[i].data());
      glLightfv(light, GL_SPECULAR, lspec[i].data());
      glEnable(light);
    }

    // other general states
    glEnable(GL_DEPTH_TEST);

    glDisable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    glShadeModel(GL_SMOOTH);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glPolygonMode(GL_FRONT, GL_FILL);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
  }

};







/*
inline void t_DrawCuboidFrame(const EVec3f &c)
{
  glDisable(GL_LIGHTING);
  glLineWidth(2);
  glColor3d(1, 1, 0);
  glBegin(GL_LINES);
    glVertex3d(   0,    0,    0); glVertex3d(c[0],    0,    0);
    glVertex3d(c[0],    0,    0); glVertex3d(c[0], c[1],    0);
    glVertex3d(c[0], c[1],    0); glVertex3d(   0, c[1],    0);
    glVertex3d(   0, c[1],    0); glVertex3d(   0,    0,    0);
    glVertex3d(   0,    0, c[2]); glVertex3d(c[0],    0, c[2]);
    glVertex3d(c[0],    0, c[2]); glVertex3d(c[0], c[1], c[2]);
    glVertex3d(c[0], c[1], c[2]); glVertex3d(   0, c[1], c[2]);
    glVertex3d(   0, c[1], c[2]); glVertex3d(   0,    0, c[2]);
    glVertex3d(   0,    0,    0); glVertex3d(   0,    0, c[2]);
    glVertex3d(c[0],    0,    0); glVertex3d(c[0],    0, c[2]);
    glVertex3d(c[0], c[1],    0); glVertex3d(c[0], c[1], c[2]);
    glVertex3d(   0, c[1],    0); glVertex3d(   0, c[1], c[2]);
  glEnd();

  glTranslated(-0.1, -0.1, -0.1);
  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
    glColor3d(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(10, 0, 0);
    glColor3d(0, 1, 0); glVertex3d(0, 0, 0); glVertex3d(0, 10, 0);
    glColor3d(0, 0, 1); glVertex3d(0, 0, 0); glVertex3d(0, 0, 10);
  glEnd();
  glTranslated( 0.1,  0.1,  0.1);
}


*/












#pragma managed
