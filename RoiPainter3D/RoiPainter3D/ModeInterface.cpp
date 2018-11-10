#include "ModeInterface.h"
#include "ImageCore.h"


#include "CrsSecCore.h"
#include "FormVisParam.h"

#pragma unmanaged
using namespace RoiPainter3D;




CRSSEC_ID pickCrsSec_onlyTrgt(const CRSSEC_ID trgtID, const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos)
{
  const EVec3f cube  = ImageCore::getInst()->getCuboidF();
  const EVec3i reso  = ImageCore::getInst()->getResolution();
  const EVec3f pitch = ImageCore::getInst()->getPitch();

  if( trgtID == CRSSEC_XY   ) return CrssecCore::getInst()->PickCrssec(true, false, false, false, cube, rayP, rayD, *pos);
  if( trgtID == CRSSEC_YZ   ) return CrssecCore::getInst()->PickCrssec(false, true, false, false, cube, rayP, rayD, *pos);
  if( trgtID == CRSSEC_ZX   ) return CrssecCore::getInst()->PickCrssec(false, false, true, false, cube, rayP, rayD, *pos);
  if( trgtID == CRSSEC_CURVE) return CrssecCore::getInst()->PickCrssec(false, false, false, true, cube, rayP, rayD, *pos);
  return CRSSEC_NON;
}




CRSSEC_ID pickCrsSec(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos)
{
  const EVec3f cube  = ImageCore::getInst()->getCuboidF();
  const EVec3i reso  = ImageCore::getInst()->getResolution();
  const EVec3f pitch = ImageCore::getInst()->getPitch();
  const bool   bXY = formVisParam_bPlaneXY();
  const bool   bYZ = formVisParam_bPlaneYZ();
  const bool   bZX = formVisParam_bPlaneZX();
  return CrssecCore::getInst()->PickCrssec(bXY, bYZ, bZX, true, cube, rayP, rayD, *pos);
}

CRSSEC_ID pickCrsSec(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos, const bool bXY, const bool bYZ, const bool bZX)
{
  const EVec3f cube  = ImageCore::getInst()->getCuboidF();
  const EVec3i reso  = ImageCore::getInst()->getResolution();
  const EVec3f pitch = ImageCore::getInst()->getPitch();
  return CrssecCore::getInst()->PickCrssec(bXY, bYZ, bZX, true, cube, rayP, rayD, *pos);
}



#pragma managed






/*
TODO implement later


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