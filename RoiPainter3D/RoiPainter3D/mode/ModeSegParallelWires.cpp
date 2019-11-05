#include "stdafx.h"
#include "ModeSegParallelWires.h"


ModeSegParallelWires::ModeSegParallelWires() : 
	m_volume_shader("shader/volVtx.glsl"   , "shader/volFlg_LRG.glsl"   ),
	m_crssec_shader("shader/crssecVtx.glsl", "shader/crssecFlg_LRG.glsl")
{
  m_bL = m_bR = m_bM = false;
  m_draging_cpid << -1, -1;
}


ModeSegParallelWires::~ModeSegParallelWires()
{
}
