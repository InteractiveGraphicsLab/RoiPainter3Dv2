#include "ModeInterface.h"
#include "ImageCore.h"


#include "CrsSecCore.h"
#include "FormVisParam.h"

#pragma unmanaged
using namespace RoiPainter3D;


CRSSEC_ID PickCrsSec(const CRSSEC_ID trgt_id, const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f *pos)
{
  const EVec3f cuboid= ImageCore::GetInst()->GetCuboid();
  const EVec3i reso  = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch = ImageCore::GetInst()->GetPitch();

  if( trgt_id == CRSSEC_XY   ) return CrssecCore::GetInst()->PickCrssec(true, false, false, false, cuboid, ray_pos, ray_dir, *pos);
  if( trgt_id == CRSSEC_YZ   ) return CrssecCore::GetInst()->PickCrssec(false, true, false, false, cuboid, ray_pos, ray_dir, *pos);
  if( trgt_id == CRSSEC_ZX   ) return CrssecCore::GetInst()->PickCrssec(false, false, true, false, cuboid, ray_pos, ray_dir, *pos);
  if( trgt_id == CRSSEC_CURVE) return CrssecCore::GetInst()->PickCrssec(false, false, false, true, cuboid, ray_pos, ray_dir, *pos);
  return CRSSEC_NON;
}


CRSSEC_ID PickCrssec(const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f *pos)
{
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
  const EVec3i reso   = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch  = ImageCore::GetInst()->GetPitch();
  const bool   b_xy = formVisParam_bPlaneXY();
  const bool   b_yz = formVisParam_bPlaneYZ();
  const bool   b_zx = FormVisParam_bPlaneZX();
  return CrssecCore::GetInst()->PickCrssec(b_xy, b_yz, b_zx, true, cuboid, ray_pos, ray_dir, *pos);
}


CRSSEC_ID PickCrsSec(const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f *pos, const bool b_xy, const bool b_yz, const bool b_zx)
{
  const EVec3f cuboid = ImageCore::GetInst()->GetCuboid();
  const EVec3i reso   = ImageCore::GetInst()->GetResolution();
  const EVec3f pitch  = ImageCore::GetInst()->GetPitch();
  return CrssecCore::GetInst()->PickCrssec(b_xy, b_yz, b_zx, true, cuboid, ray_pos, ray_dir, *pos);
}


//picking‚É¬Œ÷‚µ‚½‚ç ‚»‚Ìcross section‚ð“®‚©‚µ‚Ä true‚ð•Ô‚·
//Ž¸”s‚µ‚½‚çfalse‚ð•Ô‚·
bool PickToMoveCrossSecByWheeling(const EVec2i &p, OglForCLI *ogl, short z_delta)
{
  EVec3f ray_pos, ray_dir, pos;
  ogl->GetCursorRay( p, ray_pos, ray_dir);
  CRSSEC_ID id = PickCrssec( ray_pos, ray_dir, &pos);

  EVec3i reso   = ImageCore::GetInst()->GetResolution();
  EVec3f pitch  = ImageCore::GetInst()->GetPitch();

  if (id != CRSSEC_NON) CrssecCore::GetInst()->MoveCrssec(reso, pitch, id, (IsAltKeyOn()) ? 3 * z_delta : z_delta);
  return id != CRSSEC_NON;
}
#pragma managed

