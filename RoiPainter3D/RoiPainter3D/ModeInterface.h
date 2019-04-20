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
	virtual void MouseWheel	(const EVec2i &p, short z_delta, OglForCLI *ogl) = 0;

	virtual void KeyDown(int nChar) = 0;
	virtual void KeyUp  (int nChar) = 0;

	//this function is called before switch the mode (if return false, the mode will not be switched)
	virtual bool CanLeaveMode() = 0;

	//this function is called just after switch the mode
	virtual void StartMode() = 0;
	virtual void DrawScene(const EVec3f &cuboid, const EVec3f &cam_pos, const EVec3f &cam_center) = 0;
};


inline bool IsCtrKeyOn  () { return GetKeyState(VK_CONTROL) < 0; }
inline bool IsSpaceKeyOn() { return GetKeyState(VK_SPACE  ) < 0; }
inline bool IsShiftKeyOn() { return GetKeyState(VK_SHIFT  ) < 0; }
inline bool IsAltKeyOn  () { return GetKeyState(VK_MENU   ) < 0; }
//inline bool isTabKeyOn  (){ return GetKeyState( VK_TAB     ) < 0 ; }


CRSSEC_ID PickCrssec(const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f *pos);
CRSSEC_ID PickCrsSec(const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f *pos, const bool b_xy, const bool b_yz, const bool b_zx);
CRSSEC_ID PickCrsSec(const CRSSEC_ID trgt_id, const EVec3f &ray_pos, const EVec3f &ray_dir, EVec3f *pos);

bool PickToMoveCrossSecByWheeling(const EVec2i &p, OglForCLI *ogl, short z_delta);

