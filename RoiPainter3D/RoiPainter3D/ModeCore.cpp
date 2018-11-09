#include "ModeCore.h"

#include "Mode/ModeVizNormal.h"
#include "Mode/ModeVizMask.h"
#include "Mode/ModeSegRGrow.h"
#include "Mode/ModeSegGCut.h"
#include "Mode/ModeSegVoxelPaint.h"

#include "FormVisNorm.h"
#include "FormVisMask.h"
#include "FormSegRGrow.h"
#include "FormSegGCut.h"
#include "FormSegVoxelPaint.h"

using namespace RoiPainter3D;


ModeCore::ModeCore()
{
  printf("ModeCore Constructor\n");
  m_mode    = ModeVizNormal::getInst();
  m_mode_id = MODE_VIS_NORMAL;
  printf("ModeCore Constructor Done \n");
}

ModeCore::~ModeCore()
{

}

void ModeCore::ModeSwitch(MODE_ID m)
{
  printf("\n ModeSwitch %d \n\n", m);

  if (!m_mode->canEndMode()) return;

  if      (m == MODE_VIS_MASK      ) { m_mode = ModeVizMask      ::getInst(); m_mode_id = m; }
  else if (m == MODE_SEG_REGGROW   ) { m_mode = ModeSegRGrow     ::getInst(); m_mode_id = m; }
  else if (m == MODE_SEG_GCUT      ) { m_mode = ModeSegGCut      ::getInst(); m_mode_id = m; }
  else if (m == MODE_SEG_VOXPAINT  ) { m_mode = ModeSegVoxelPaint::getInst(); m_mode_id = m; }
  else if (m == MODE_REF_VOXPAINT  ) { m_mode = ModeSegVoxelPaint::getInst(); m_mode_id = m; }
  /*else if (m == MODE_SEG_RIGIDICP  ) m_mode = ModeSegRigidICP::GetInstance();
  else if (m == MODE_SEG_CLOSESTPIX) m_mode = ModeSegClosestPix::getInst();
  else if (m == MODE_SEG_PARACONT  ) m_mode = ModeSegParaConts::getInst();
  else if (m == MODE_SEG_LCLRGROW  ) m_mode = ModeSegLocalRGrow::getInst();
  else if (m == MODE_REF_STRKTRIM  ) m_mode = ModeRefStrokeTrim::getInst();*/
  else	                             { m_mode = ModeVizNormal::getInst(); m_mode_id = MODE_VIS_NORMAL; }

  //// Hide all Forms
  formVisNorm_Hide();
  formVisMask_Hide();
  formSegRGrow_Hide();
  formSegGCut_Hide();
  formSegVoxelPaint_Hide();

  //formSegPixPaint_Hide();
  //FormSegRigidICPHide();
  //formSegClosestPix_Hide();
  //formSegParaConts_Hide();
  //FormSegLocalRGrow_Hide();
  //formRefStrokeTrim_Hide();

  m_mode->startMode();

}
