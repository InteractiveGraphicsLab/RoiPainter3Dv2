#pragma once




//------------------------------------------------------------------------------------------
//
// ModeInterfaceは各モードの親クラス 
// 各モードのクラスは・・・
//   + Mode + {Seg,Vis} + MODENAME という名前を持つ
//   + シングルトンで実現される
//   + マウスイベントハンドラ・キーイベントハンドラ・レンダリングイベントハンドラを実装する
//
//------------------------------------------------------------------------------------------



#include "COMMON/OglForCLI.h"
#include "CrsSecCore.h"

#define ONMOVE_SLICE_RATE 0.5


enum MODE_ID
{
	MODE_VIS_NORMAL    , // ModeVizNormal       
	MODE_VIS_MASK      , // ModeVizMask    
  MODE_SEG_REGGROW   , // ModeSegRegGrow     
	MODE_SEG_GCUT      , // ModeSegGGut           
	MODE_SEG_VOXPAINT  , // ModeSegVoxelPaint
	MODE_SEG_LCLRGROW  , // ModeSegLocalRGrow  (Local Region Growing by Takashi Ijiri & Shogo Tsuruoka (Ritumei))
	MODE_SEG_THRESHPNT , // ModeSegThreshfieldPaint (UIST paper by Takeo Igarashi)
	MODE_REF_STRKTRIM  , // ModeRefStrokeTrim
	MODE_REF_VOXPAINT    // ModeSegVoxelPaint
};


class ModeInterface
{
protected:
	bool m_bL, m_bM, m_bR;

	ModeInterface() { m_bL = m_bR = m_bM = false; }

public:
	virtual void LBtnUp		  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnUp		  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnUp		  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void LBtnDown	  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnDown	  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnDown	  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void LBtnDclk	  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void RBtnDclk	  (const EVec2i &p, OglForCLI *ogl) = 0;
	virtual void MBtnDclk	  (const EVec2i &p, OglForCLI *ogl) = 0;
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


CRSSEC_ID pickCrsSec(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos);
CRSSEC_ID pickCrsSec(const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos, const bool bXY, const bool bYZ, const bool bZX);
CRSSEC_ID pickCrsSec_onlyTrgt(const CRSSEC_ID trgtID, const EVec3f &rayP, const EVec3f &rayD, EVec3f *pos);

