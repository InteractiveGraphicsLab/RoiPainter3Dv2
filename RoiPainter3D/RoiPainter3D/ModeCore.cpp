#include "ModeCore.h"

#include "Mode/ModeVizNormal.h"
#include "Mode/ModeVizMask.h"
#include "Mode/ModeSegRGrow.h"
#include "Mode/ModeSegGCut.h"
#include "Mode/ModeSegVoxelPaint.h"
#include "Mode/ModeRefStrokeTrim.h"
#include "Mode/ModeSegLocalRGrow.h"

#include "FormVisNorm.h"
#include "FormVisMask.h"
#include "FormSegRGrow.h"
#include "FormSegGCut.h"
#include "FormSegVoxelPaint.h"
#include "FormSegLocalRGrow.h"
#include "FormRefStrokeTrim.h"

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
  else if (m == MODE_REF_STRKTRIM  ) { m_mode = ModeRefStrokeTrim::getInst(); m_mode_id = m; }
  else if (m == MODE_SEG_LCLRGROW  ) { m_mode = ModeSegLocalRGrow::getInst(); m_mode_id = m; }
  else	                             { m_mode = ModeVizNormal::getInst(); m_mode_id = MODE_VIS_NORMAL; }

  //// Hide all Forms
  formVisNorm_Hide();
  formVisMask_Hide();
  formSegRGrow_Hide();
  formSegGCut_Hide();
  formSegVoxelPaint_Hide();
  formRefStrokeTrim_Hide();
  formSegLocalRGrow_Hide();

  m_mode->startMode();

}
