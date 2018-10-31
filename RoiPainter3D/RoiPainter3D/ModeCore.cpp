#include "ModeCore.h"
#include "Mode/ModeVizNormal.h"


ModeCore::ModeCore()
{
  printf("ModeCore Constructor\n");
  m_mode = ModeVizNormal::getInst();
  printf("ModeCore Constructor Done \n");
}

ModeCore::~ModeCore()
{

}

void ModeCore::ModeSwitch(MODE_ID m)
{
  printf("TODO TODO TODO");

}
