#pragma once



#define TRANS_FUNC_SIZE 256
#include "COMMON/OglImage.h"

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormVisParam の概要
	/// </summary>
	public ref class FormVisParam : public System::Windows::Forms::Form
	{

    OglImage1D<CH_RGBA> *m_imgTf ; // 1st ch(intensity-->trans), 2nd (grad mag-->trans) 
    OglImage1D<CH_RGBA> *m_imgPsu; // func: intensity --> psude color 

    static FormVisParam^ m_singleton;

    FormVisParam(void)
    {
      InitializeComponent();

      m_imgTf  = new OglImage1D<CH_RGBA>();
      m_imgPsu = new OglImage1D<CH_RGBA>();
      m_imgTf->Allocate(TRANS_FUNC_SIZE);
	    m_imgPsu->AllocateHeuImg(256);
	    for (int i = 0; i < TRANS_FUNC_SIZE; ++i) (*m_imgTf)[4 * i] = (*m_imgTf)[4 * i + 1] = i;
    }


  public:
    static FormVisParam^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormVisParam();
      return m_singleton;
    }

    //TODO TODO TODO
    void initAllItemsForNewImg(){
      printf("TODO TODO TODO");
    }

    bool  bRendFrame(){ return true;}
    bool  bRendIndi (){ return true;}   
    bool  bPlaneXY(){return true;}
    bool  bPlaneYZ(){return true;}
    bool  bPlaneZX(){return true;}

    bool  bRendVol(){return true;}
    bool  bGradMag(){return true;}
    bool  bDoPsued(){return true;}

    bool  bOnManip(){return true;}

    float getAlpha(){return 0.8f;}
    int   getSliceNum(){return 128;}

    void bindTfImg (){ m_imgTf ->BindOgl(false); }
    void bindPsuImg(){ m_imgPsu->BindOgl(true ); }



      

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormVisParam()
		{
			if (components)
			{
				delete components;
			}
		}

	private:
		/// <summary>
		/// 必要なデザイナー変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナー サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディターで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			this->components = gcnew System::ComponentModel::Container();
			this->Size = System::Drawing::Size(300,300);
			this->Text = L"FormVisParam";
			this->Padding = System::Windows::Forms::Padding(0);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
		}
#pragma endregion
	};


  inline bool  formVisParam_bPlaneXY   (){return FormVisParam::getInst()->bPlaneXY();}
  inline bool  formVisParam_bPlaneYZ   (){return FormVisParam::getInst()->bPlaneYZ();}
  inline bool  formVisParam_bPlaneZX   (){return FormVisParam::getInst()->bPlaneZX();}
  inline bool  formVisParam_bRendVol   (){return FormVisParam::getInst()->bRendVol();}
  inline bool  formVisParam_bGradMag   (){return FormVisParam::getInst()->bGradMag();}
  inline bool  formVisParam_bDoPsued   (){return FormVisParam::getInst()->bDoPsued();}
  inline bool  formVisParam_bOnManip   (){return FormVisParam::getInst()->bOnManip();}
  inline float formVisParam_getAlpha   (){return FormVisParam::getInst()->getAlpha();}
  inline int   formVisParam_getSliceNum(){return FormVisParam::getInst()->getSliceNum();}
  inline void  formVisParam_bindTfImg (){ FormVisParam::getInst()->bindTfImg ();}
  inline void  formVisParam_bindPsuImg(){ FormVisParam::getInst()->bindPsuImg();}

}
