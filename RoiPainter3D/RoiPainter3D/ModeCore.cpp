#include "ModeCore.h"

#include "Mode/ModeVizNormal.h"
#include "Mode/ModeVizMask.h"
#include "Mode/ModeSegRGrow.h"
#include "Mode/ModeSegGCut.h"
#include "Mode/ModeSegVoxelPaint.h"
#include "Mode/ModeRefStrokeTrim.h"
#include "Mode/ModeSegLocalRGrow.h"
#include "Mode/ModeSegParallelWires.h"

#include "FormVisNorm.h"
#include "FormVisMask.h"
#include "FormSegRGrow.h"
#include "FormSegGCut.h"
#include "FormSegVoxelPaint.h"
#include "FormSegLocalRGrow.h"
#include "FormSegParallelWires.h"
#include "FormRefStrokeTrim.h"
#include <iostream>

using namespace RoiPainter3D;

#pragma unmanaged

ModeCore::ModeCore()
{
  std::cout << "ModeCore Constructor\n";
  m_mode    = ModeVizNormal::getInst();
  m_mode_id = MODE_VIS_NORMAL;
  std::cout << "ModeCore Constructor Done \n";
}

ModeCore::~ModeCore()
{

}

void ModeCore::ModeSwitch(MODE_ID m)
{
  std::cout << "\n\nModeSwitch id = " <<  m << "\n\n";

  if (!m_mode->CanLeaveMode()) return;

  if      (m == MODE_VIS_MASK      ) { m_mode = ModeVizMask         ::getInst(); m_mode_id = m; }
  else if (m == MODE_SEG_REGGROW   ) { m_mode = ModeSegRGrow        ::GetInst(); m_mode_id = m; }
  else if (m == MODE_SEG_GCUT      ) { m_mode = ModeSegGCut         ::getInst(); m_mode_id = m; }
  else if (m == MODE_SEG_VOXPAINT  ) { m_mode = ModeSegVoxelPaint   ::GetInst(); m_mode_id = m; }
  else if (m == MODE_SEG_PARAWIRE  ) { m_mode = ModeSegParallelWires::GetInst(); m_mode_id = m; }
  else if (m == MODE_REF_STRKTRIM  ) { m_mode = ModeRefStrokeTrim   ::GetInst(); m_mode_id = m; }
  else if (m == MODE_REF_VOXPAINT  ) { m_mode = ModeSegVoxelPaint   ::GetInst(); m_mode_id = m; }
  else if (m == MODE_SEG_LCLRGROW  ) { m_mode = ModeSegLocalRGrow   ::GetInst(); m_mode_id = m; }
  else	                             { m_mode = ModeVizNormal::getInst(); m_mode_id = MODE_VIS_NORMAL; }

  //// Hide all Forms
  formVisNorm_Hide();
  formVisMask_Hide();
  formSegRGrow_Hide();
  formSegGCut_Hide();
  formSegVoxelPaint_Hide();
  formRefStrokeTrim_Hide();
  FormSegParallelWires_Hide();
  formSegLocalRGrow_Hide();

  // start new mode
  m_mode->StartMode();
}




#pragma managed