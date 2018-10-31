#include "ModeInterface.h"
#include "ImageCore.h"
//#include "../CrsSecCore.h"
//#include "../FormVisParam.h"






/*
TODO implement later
using namespace RoiPainter4D;

CRSSEC_ID pickCrsSrc(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos)
{
  const EVec3f cube  = ImageCore::getInst()->GetCuboidF();
  const EVec3i reso  = ImageCore::getInst()->GetReso();
  const EVec3f pitch = ImageCore::getInst()->GetPitch();
  const bool   bXY = formVisParam_bPlaneXY();
  const bool   bYZ = formVisParam_bPlaneYZ();
  const bool   bZX = formVisParam_bPlaneZX();
  return CrssecCore::GetInst()->PickCrssec(bXY, bYZ, bZX, cube, rayP, rayD, *pos);
}

CRSSEC_ID pickCrsSrc(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos, const bool bXY, const bool bYZ, const bool bZX)
{
  const EVec3f cube = ImageCore::getInst()->GetCuboidF();
  const EVec3i reso = ImageCore::getInst()->GetReso();
  const EVec3f pitch = ImageCore::getInst()->GetPitch();
  //const bool   bXY   = formVisParam_bPlaneXY();
  //const bool   bYZ   = formVisParam_bPlaneYZ();
  //const bool   bZX   = formVisParam_bPlaneZX();
  return CrssecCore::GetInst()->PickCrssec(bXY, bYZ, bZX, cube, rayP, rayD, *pos);
}

CRSSEC_ID pickCrsSrc(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos, const CRSSEC_ID id)
{
  const EVec3f cube = ImageCore::getInst()->GetCuboidF();
  const EVec3i reso = ImageCore::getInst()->GetReso();
  const EVec3f pitch = ImageCore::getInst()->GetPitch();

  if (id == CRSSEC_XY)return CrssecCore::GetInst()->PickCrssec(true, false, false, cube, rayP, rayD, *pos);
  else if (id == CRSSEC_YZ)return CrssecCore::GetInst()->PickCrssec(false, true, false, cube, rayP, rayD, *pos);
  else if (id == CRSSEC_ZX)return CrssecCore::GetInst()->PickCrssec(false, false, true, cube, rayP, rayD, *pos);
  else return CRSSEC_NON;
}

*/