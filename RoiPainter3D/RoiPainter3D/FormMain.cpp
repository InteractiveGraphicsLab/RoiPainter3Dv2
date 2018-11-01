#include "FormMain.h"

#include "ImageCore.h"
#include "ModeCore.h"
#include "ViewAngleCore.h"


#include "FormVisParam.h"
#include "FormVisNorm.h"


using namespace RoiPainter3D;



#pragma comment( lib, "opengl32.lib" )
#pragma comment( lib, "glu32.lib"    )
#pragma comment( lib, "gdi32.lib"    )
#pragma comment( lib, "User32.lib"   )



FormMain::FormMain(void)
{
  printf("FormMain constructor\n");

  m_prevKeyID = -1;
  m_ogl = 0;
  
  InitializeComponent();
  
  m_ogl = new OglForCLI(GetDC((HWND)FormMainPanel->Handle.ToPointer()));
  initCameraPosition(ImageCore::getInst()->getCuboidF());
  m_ogl->SetBgColor(0.3f, 0.3f, 0.3f, 0.5f);

  printf("FormMain constructor .. DONE\n");
}



System::Void FormMain::FormMainPanel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left  ) ModeCore::getInst()->LBtnDown(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) ModeCore::getInst()->MBtnDown(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right ) ModeCore::getInst()->RBtnDown(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  ModeCore::getInst()->MouseMove(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseUp  (System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left  ) ModeCore::getInst()->LBtnUp(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) ModeCore::getInst()->MBtnUp(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right ) ModeCore::getInst()->RBtnUp(EVec2i(e->X, e->Y), m_ogl);
}

System::Void FormMain::FormMainPanel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  if (e->Button == System::Windows::Forms::MouseButtons::Left)
  {
    EVec2i cursorP(e->X, e->Y);
    if (!pickViewAngleIndicator(cursorP) ) ModeCore::getInst()->LBtnDblClk( cursorP, m_ogl);
  }
  if (e->Button == System::Windows::Forms::MouseButtons::Middle) ModeCore::getInst()->MBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
  if (e->Button == System::Windows::Forms::MouseButtons::Right) ModeCore::getInst()->RBtnDblClk(EVec2i(e->X, e->Y), m_ogl);
}





System::Void FormMain::FormMainPanel_Resize   (System::Object^  sender, System::EventArgs^  e)
{
  if (m_ogl == 0) return;
  redrawMainPanel();
}


System::Void FormMain::FormMainPanel_Paint    (System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e)
{
  if (m_ogl == 0) return;
  redrawMainPanel();
}


System::Void FormMain::FormMain_Resize(System::Object^  sender, System::EventArgs^  e)
{
}

System::Void FormMain::FormMain_Move  (System::Object^  sender, System::EventArgs^  e)
{
  if (m_ogl == 0) return;
  replaceOtherForms();
}



System::Void FormMain::FormMain_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e)
{
  int x = e->X - this->FormMainPanel->Location.X;
  int y = e->Y - this->FormMainPanel->Location.Y;

  ModeCore::getInst()->MouseWheel(EVec2i(x, y), e->Delta, m_ogl);
}





bool FormMain::pickViewAngleIndicator(const EVec2i p)
{
  if (!FormVisParam::getInst()->bRendIndi()) return false;

  int curViewW = FormMainPanel->Width;
  int curViewH = FormMainPanel->Height;
  EVec3f  camP = m_ogl->GetCamPos();
  EVec3f  camC = m_ogl->GetCamCnt();
  EVec3f  camY = m_ogl->GetCamUp();

  int flg = ViewAngleCore::getInst()->pickIndicator(*m_ogl, curViewW, curViewH, camP, camC, camY, p);

  bool tf = false;
  if      (flg == 1) { camP << 0,-1, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 2) { camP << 1, 0, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 3) { camP << 0, 1, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 4) { camP <<-1, 0, 0;  camY << 0, 0, 1; tf = true; }
  else if (flg == 5) { camP << 0, 0, 1;  camY << 0,-1, 0; tf = true; }
  else if (flg == 6) { camP << 0, 0,-1;  camY << 0,-1, 0; tf = true; }

  if (!tf) return false;

  EVec3f cuboid = ImageCore::getInst()->getCuboidF();
  double  D = cuboid.norm() * 1.0;
  camC = 0.5 * cuboid;
  camP = camC + ((float)D) * camP;
  m_ogl->SetCam(camP, camC, camY);
  formMain_redrawMainPanel();
  return true;
}




/*
    System::Void FormMainPanel_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e) {}

    System::Void FormMain_Move(System::Object^  sender, System::EventArgs^  e);
    System::Void FormMain_Resize(System::Object^  sender, System::EventArgs^  e);
    System::Void file_dcm3d_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_dcm2D_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_traw3d_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_saveMask_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void file_loadMask_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_visNorm_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_visMask_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_visRegionGrow_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segPaint_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segRigidICP_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segClosestPix_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segParaConts_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_segLocalRrow_Click(System::Object^  sender, System::EventArgs^  e);
    System::Void switch_refStrokeTrim_Click(System::Object^  sender, System::EventArgs^  e);

    System::Void FormMain_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
    System::Void FormMain_KeyUp(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
    System::Void FormMain_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e);
*/





void FormMain::replaceOtherForms()
{
  const int thisX = this->Location.X;
  const int thisY = this->Location.Y;
  const int thisW = this->Width;
  const int dlgH = FormVisParam::getInst()->Height;
  FormVisParam::getInst()->Location = Point(thisX + thisW, thisY);
  FormVisNorm ::getInst()->Location = Point(thisX + thisW, thisY + dlgH);

  /*
  FormVisMask::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegRGrow::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegPixPaint::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegRigidICP::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegClosestPix::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegParaConts::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormSegLocalRGrow::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  FormRefStrokeTrim::getInst()->Location = Point(thisX + thisW, thisY + dlgH);
  */
}



//�������̃^�C�~���O�͏d�v
//managed�N���X�͂����ŏ���������
void FormMain::initializeOtherForms()
{
  printf("--------initialize form(dialogs)...\n");
  FormVisParam::getInst()->initAllItemsForNewImg();
  FormVisParam::getInst()->Show();
  FormVisParam::getInst()->Location = Point(this->Location.X + this->Width, this->Location.Y);


  //��xShow�� Hide����(�������Ȃ��ƈړ��������Ȃ�)
  FormVisNorm::getInst()->Show();
  
  replaceOtherForms();
  
  FormVisNorm::getInst()->Hide();
  printf("--------initialize form(dialogs)...DONE\n");

  /*
  FormVisMask::getInst()->Show();
  FormSegRGrow::getInst()->Show();
  FormSegPixPaint::getInst()->Show();
  FormSegRigidICP::getInst()->Show();
  FormSegClosestPix::getInst()->Show();
  FormSegLocalRGrow::getInst()->Show();
  FormRefStrokeTrim::getInst()->Show();
  FormVisNorm::getInst()->Hide();
  FormVisMask::getInst()->Hide();
  FormSegRGrow::getInst()->Hide();
  FormSegPixPaint::getInst()->Hide();
  FormSegRigidICP::getInst()->Hide();
  FormSegClosestPix::getInst()->Hide();
  //FormSegLocalRGrow::getInst()->Hide();
  FormRefStrokeTrim::getInst()->Hide();
  */
}





static void t_drawFrame(const EVec3f &c)
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

  glDisable(GL_LIGHTING);
  glBegin(GL_LINES);
    glColor3d(1, 0, 0); glVertex3d(0, 0, 0); glVertex3d(10, 0, 0);
    glColor3d(0, 1, 0); glVertex3d(0, 0, 0); glVertex3d(0, 10, 0);
    glColor3d(0, 0, 1); glVertex3d(0, 0, 0); glVertex3d(0, 0, 10);
  glEnd();
}



static void initializeLights()
{
  //initialize lights	
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
  glEnable(GL_LIGHT2);
  float p0[4] = {  1000,  1000, -1000, 1 };
  float p1[4] = { -1000,  1000, -1000, 1 };
  float p2[4] = {  1000, -1000, -1000, 1 };
  glLightfv(GL_LIGHT0, GL_POSITION, p0);
  glLightfv(GL_LIGHT1, GL_POSITION, p1);
  glLightfv(GL_LIGHT2, GL_POSITION, p2);

}









void FormMain::redrawMainPanel()
{
  static bool isFirst = true;
  if (isFirst)
  {
    //���̃^�C�~���O�� ����form�𐶐���, Show()���ǂ��OK
    isFirst = false;
    initializeOtherForms();
    ModeCore::getInst()->ModeSwitch(MODE_VIS_NORMAL);
  }

  const EVec3f cuboid   = ImageCore::getInst()->getCuboidF();
  const float  nearDist = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 0.01f;
  const float  farDist  = (cuboid[0] + cuboid[1] + cuboid[2]) / 3.0f * 8;
  const int    viewW    = FormMainPanel->Width;
  const int    viewH    = FormMainPanel->Height;

  m_ogl->OnDrawBegin(viewW, viewH, 45.0, nearDist, farDist);
  initializeLights();

  if (FormVisParam::getInst()->bRendFrame()) t_drawFrame(cuboid);

  ModeCore::getInst()->drawScene(cuboid, m_ogl->GetCamPos(), m_ogl->GetCamCnt());

  if (FormVisParam::getInst()->bRendIndi())
  {
    ViewAngleCore::getInst()->drawIndicator(viewW, viewH, m_ogl->GetCamPos(), m_ogl->GetCamCnt(), m_ogl->GetCamUp());
  }
  m_ogl->OnDrawEnd();
}



void FormMain::initCameraPosition(EVec3f &cuboid)
{
  EVec3f camC = cuboid * 0.5;
  EVec3f camY(0, -1, 0);
  EVec3f camP = camC - cuboid[2] * 1.5f * EVec3f(0, 0, 1);
  m_ogl->SetCam(camP, camC, camY);
}


