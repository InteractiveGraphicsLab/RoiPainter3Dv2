// RoiPainter3D.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "FormMain.h"
#include <stdio.h>
#include "ImageCore.h"
#include "ModeCore.h"
#include "CrsSecCore.h"

using namespace System;
using namespace RoiPainter3D;


[STAThreadAttribute]
int main()
{
  printf("start main function\n");

  //Initialize non-managed Singletons
  //��managed�ȃV���O���g���͂����ŏ�����
  //���������̃^�C�~���O�ł́AForm�𐶐���show���Ă͂��߁D
    
  ModeCore  ::getInst();
  ImageCore ::getInst();
  CrssecCore::getInst();

  /*
  ModeVizMask::getInst();
  ModeVizNormal::getInst();
  ModeSegRGrow::getInst();
  ModeSegPixPaint::getInst();
  ModeSegRigidICP::GetInstance();
  ModeSegClosestPix::getInst();
  ModeSegLocalRGrow::getInst();
  ModeRefStrokeTrim::getInst();
  */

  printf("FormMain::getInst()->ShowDialog() \n");
  FormMain::getInst()->ShowDialog();
  
  printf("aaaa \ FormMain::getInst()->ShowDialog() \n");

  return 0;
}



// done 2018/11/1
// mode_vis_norm
// ImageCore
// ModeCore
// event handler : Wheeling hander
//
//
//TODO  
// event handler
// mode_vis_mask
// mode_seg_rgrow
//
// IO --> 2D slices bmp
// IO --> 2D slices tif
// IO --> 2D slices dcm
// IO --> 3D slices traw3D
// IO --> 3D slices dcm
// etc...


