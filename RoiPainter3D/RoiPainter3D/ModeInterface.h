#pragma once

#include "COMMON/OglForCLI.h"

#define ONMOVE_SLICE_RATE 0.2


enum MODE_ID
{
	MODE_VIS_NORMAL    , // ModeVizNormal       
	MODE_VIS_MASK      , // ModeVizMask         

	MODE_SEG_PIXPAINT  , // ModeSegPixPaint
	MODE_SEG_REGGROW   , // ModeSegRegGrow     
	MODE_SEG_RIGIDICP  , // ModeSegRigidICP   (SJTracker���)
	MODE_SEG_CLOSESTPIX, // ModeSegClosestPix (SproutViewer��� TODO Tomofumi Narita)

	MODE_SEG_PARACONT,   //ModeSegParaConts   (Parallel Contours by Chika Tomiyama)
	MODE_SEG_LCLRGROW,   //ModeSegLocalRGrow  (Local Region Growing by Hikaru Shionozaki)

	MODE_REF_STRKTRIM     // ModeRefStrkTrim     

	//MODE_SEG_GCUT   , // ModeSegGGut           
	//MODE_SEG_THRESHPNT, // ModeSegTreshPnt      
	//MODE_SEG_LOCALRGROW, // ModeSegLocalRGrow    
};


class ModeInterface
{
protected:
	bool m_bL, m_bM, m_bR;

	ModeInterface() { m_bL = m_bR = m_bM = false; }

public:
	virtual MODE_ID getModeID() = 0;
	virtual void LBtnUp		(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnUp		(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnUp		(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void LBtnDown	(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnDown	(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnDown	(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void LBtnDclk	(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnDclk	(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnDclk	(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MouseMove	(const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MouseWheel	(const EVec2i &p, short zDelta, OglForCLI *ogl) = 0;

	virtual void keyDown(int nChar) = 0;
	virtual void keyUp  (int nChar) = 0;

	//this function is called before switch the mode (if return false, the mode will not be switched)
	virtual bool canEndMode() = 0;

	//this function is called just after switch the mode
	virtual void startMode() = 0;

	virtual void drawScene(const EVec3f &cuboid, const EVec3f &camP, const EVec3f &camF) = 0;
};


inline bool isCtrKeyOn  () { return GetKeyState(VK_CONTROL) < 0; }
inline bool isSpaceKeyOn() { return GetKeyState(VK_SPACE  ) < 0; }
inline bool isShiftKeyOn() { return GetKeyState(VK_SHIFT  ) < 0; }
inline bool isAltKeyOn  () { return GetKeyState(VK_MENU   ) < 0; }
//inline bool isTabKeyOn  (){ return GetKeyState( VK_TAB     ) < 0 ; }



/*

CRSSEC_ID pickCrsSrc(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos);
CRSSEC_ID pickCrsSrc(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos, const bool bXY, const bool bYZ, const bool bZX);
CRSSEC_ID pickCrsSrc(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos, const CRSSEC_ID id);
{
	const EVec3f cube  = ImageCore::getInst()->getCuboidF();
	const EVec3i reso  = ImageCore::getInst()->getReso   ();
	const EVec3f pitch = ImageCore::getInst()->getPitch  ();
	const bool   bXY   = formVisParam_bPlaneXY();
	const bool   bYZ   = formVisParam_bPlaneYZ();
	const bool   bZX   = formVisParam_bPlaneZX();
	return CrsSecCore::getInst()->pickCrosSec(bXY, bYZ, bZX, cube, rayP, rayD, *pos);
}
*/


