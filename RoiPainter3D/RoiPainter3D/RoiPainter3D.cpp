// RoiPainter3D.cpp : ���C�� �v���W�F�N�g �t�@�C���ł��B

#include "stdafx.h"

#include "FormMain.h"
#include <stdio.h>

using namespace System;
using namespace RoiPainter3D;


[STAThreadAttribute]
int main()
{
  printf("start main function\n");


  //Initialize non-managed Singletons
  //��managed�ȃV���O���g���͂����ŏ�����
  //���������̃^�C�~���O�ł́AForm�𐶐���show���Ă͂��߁D
    
  /*
  ImageCore::getInst();
  CrssecCore::GetInst();
  ModeCore::getInst();
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

  return 0;
}

// todo 2018/10/31
// mode manager
// mode_vis_mask
// mode_vis_norm
// mode_seg_rgrow
//
// IO --> 2D slices bmp
// IO --> 2D slices tif
// IO --> 2D slices dcm
// IO --> 3D slices traw3D
// IO --> 3D slices dcm
// etc...


