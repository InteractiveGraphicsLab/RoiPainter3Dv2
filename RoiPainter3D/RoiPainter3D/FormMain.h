#pragma once

#include "COMMON/OglForCLI.h"
#include "COMMON/OglImage.h"


namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormMain �̊T�v
	/// </summary>
	public ref class FormMain : public System::Windows::Forms::Form
	{

    OglForCLI *m_ogl;
    int        m_prevKeyID;

    static FormMain^ m_singleton;
    FormMain(void);

  public:
    static FormMain^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormMain();
      return m_singleton;
    }


  public:
    void redrawMainPanel();
    void initCameraPosition(EVec3f &cuboid);

  private:
    void initializeOtherForms();
    void replaceOtherForms();
    bool pickViewAngleIndicator(const EVec2i p);


	protected:
		/// <summary>
		/// �g�p���̃��\�[�X�����ׂăN���[���A�b�v���܂��B
		/// </summary>
		~FormMain()
		{
      delete m_ogl;

			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Panel^  FormMainPanel;
  private: System::Windows::Forms::MenuStrip^  menuStrip1;
  private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  open2DSlicesToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  modeToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  open2DSlicesdcmToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  open3DVolumetraw3DToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  open3DColumedcmToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  open3DVolumefavToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
  private: System::Windows::Forms::ToolStripMenuItem^  saveMaskmskToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  loadMaskmskToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  saveMaskfavbToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
  private: System::Windows::Forms::ToolStripMenuItem^  exportVolumeAsTraw3dssToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  visualizationStandardToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  visualizationMaskToolStripMenuItem;
  private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator3;
  private: System::Windows::Forms::ToolStripMenuItem^  segmentationToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  segmentationThresholdToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  segmentationGraphCutToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  segmentationToolStripMenuItem1;
  private: System::Windows::Forms::ToolStripMenuItem^  miscsToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  exportCurrentCameraPosToolStripMenuItem;
  private: System::Windows::Forms::ToolStripMenuItem^  importCurrentCameraPosToolStripMenuItem;
  protected:

	private:
		/// <summary>
		/// �K�v�ȃf�U�C�i�[�ϐ��ł��B
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// �f�U�C�i�[ �T�|�[�g�ɕK�v�ȃ��\�b�h�ł��B���̃��\�b�h�̓��e��
		/// �R�[�h �G�f�B�^�[�ŕύX���Ȃ��ł��������B
		/// </summary>
		void InitializeComponent(void)
		{
      this->FormMainPanel = (gcnew System::Windows::Forms::Panel());
      this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
      this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open2DSlicesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open2DSlicesdcmToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open3DVolumetraw3DToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open3DColumedcmToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open3DVolumefavToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->saveMaskmskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->loadMaskmskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->saveMaskfavbToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->exportVolumeAsTraw3dssToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->modeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->visualizationStandardToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->visualizationMaskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->segmentationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->segmentationThresholdToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->segmentationGraphCutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->segmentationToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->miscsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exportCurrentCameraPosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->importCurrentCameraPosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->menuStrip1->SuspendLayout();
      this->SuspendLayout();
      // 
      // FormMainPanel
      // 
      this->FormMainPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
        | System::Windows::Forms::AnchorStyles::Left)
        | System::Windows::Forms::AnchorStyles::Right));
      this->FormMainPanel->Location = System::Drawing::Point(2, 28);
      this->FormMainPanel->Margin = System::Windows::Forms::Padding(2);
      this->FormMainPanel->Name = L"FormMainPanel";
      this->FormMainPanel->Size = System::Drawing::Size(666, 481);
      this->FormMainPanel->TabIndex = 0;
      this->FormMainPanel->Paint += gcnew System::Windows::Forms::PaintEventHandler(this, &FormMain::FormMainPanel_Paint);
      this->FormMainPanel->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseDoubleClick);
      this->FormMainPanel->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseDown);
      this->FormMainPanel->MouseMove += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseMove);
      this->FormMainPanel->MouseUp += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMainPanel_MouseUp);
      this->FormMainPanel->Resize += gcnew System::EventHandler(this, &FormMain::FormMainPanel_Resize);
      // 
      // menuStrip1
      // 
      this->menuStrip1->ImageScalingSize = System::Drawing::Size(20, 20);
      this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {
        this->fileToolStripMenuItem,
          this->modeToolStripMenuItem, this->miscsToolStripMenuItem
      });
      this->menuStrip1->Location = System::Drawing::Point(0, 0);
      this->menuStrip1->Name = L"menuStrip1";
      this->menuStrip1->Padding = System::Windows::Forms::Padding(4, 2, 0, 2);
      this->menuStrip1->Size = System::Drawing::Size(668, 24);
      this->menuStrip1->TabIndex = 1;
      this->menuStrip1->Text = L"menuStrip1";
      // 
      // fileToolStripMenuItem
      // 
      this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(11) {
        this->open2DSlicesToolStripMenuItem,
          this->open2DSlicesdcmToolStripMenuItem, this->open3DVolumetraw3DToolStripMenuItem, this->open3DColumedcmToolStripMenuItem, this->open3DVolumefavToolStripMenuItem,
          this->toolStripSeparator1, this->saveMaskmskToolStripMenuItem, this->loadMaskmskToolStripMenuItem, this->saveMaskfavbToolStripMenuItem,
          this->toolStripSeparator2, this->exportVolumeAsTraw3dssToolStripMenuItem
      });
      this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
      this->fileToolStripMenuItem->Size = System::Drawing::Size(37, 20);
      this->fileToolStripMenuItem->Text = L"File";
      // 
      // open2DSlicesToolStripMenuItem
      // 
      this->open2DSlicesToolStripMenuItem->Name = L"open2DSlicesToolStripMenuItem";
      this->open2DSlicesToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->open2DSlicesToolStripMenuItem->Text = L"open 2D slices (bmp, tif)";
      // 
      // open2DSlicesdcmToolStripMenuItem
      // 
      this->open2DSlicesdcmToolStripMenuItem->Name = L"open2DSlicesdcmToolStripMenuItem";
      this->open2DSlicesdcmToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->open2DSlicesdcmToolStripMenuItem->Text = L"open 2D slices (dcm)";
      // 
      // open3DVolumetraw3DToolStripMenuItem
      // 
      this->open3DVolumetraw3DToolStripMenuItem->Name = L"open3DVolumetraw3DToolStripMenuItem";
      this->open3DVolumetraw3DToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->open3DVolumetraw3DToolStripMenuItem->Text = L"open 3D volume (traw3D)";
      // 
      // open3DColumedcmToolStripMenuItem
      // 
      this->open3DColumedcmToolStripMenuItem->Name = L"open3DColumedcmToolStripMenuItem";
      this->open3DColumedcmToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->open3DColumedcmToolStripMenuItem->Text = L"open 3D volume (dcm)";
      // 
      // open3DVolumefavToolStripMenuItem
      // 
      this->open3DVolumefavToolStripMenuItem->Name = L"open3DVolumefavToolStripMenuItem";
      this->open3DVolumefavToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->open3DVolumefavToolStripMenuItem->Text = L"open 3D volume (fav)";
      // 
      // toolStripSeparator1
      // 
      this->toolStripSeparator1->Name = L"toolStripSeparator1";
      this->toolStripSeparator1->Size = System::Drawing::Size(215, 6);
      // 
      // saveMaskmskToolStripMenuItem
      // 
      this->saveMaskmskToolStripMenuItem->Name = L"saveMaskmskToolStripMenuItem";
      this->saveMaskmskToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->saveMaskmskToolStripMenuItem->Text = L"save mask (msk)";
      // 
      // loadMaskmskToolStripMenuItem
      // 
      this->loadMaskmskToolStripMenuItem->Name = L"loadMaskmskToolStripMenuItem";
      this->loadMaskmskToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->loadMaskmskToolStripMenuItem->Text = L"load mask (msk)";
      // 
      // saveMaskfavbToolStripMenuItem
      // 
      this->saveMaskfavbToolStripMenuItem->Name = L"saveMaskfavbToolStripMenuItem";
      this->saveMaskfavbToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->saveMaskfavbToolStripMenuItem->Text = L"save mask (fav)";
      // 
      // toolStripSeparator2
      // 
      this->toolStripSeparator2->Name = L"toolStripSeparator2";
      this->toolStripSeparator2->Size = System::Drawing::Size(215, 6);
      // 
      // exportVolumeAsTraw3dssToolStripMenuItem
      // 
      this->exportVolumeAsTraw3dssToolStripMenuItem->Name = L"exportVolumeAsTraw3dssToolStripMenuItem";
      this->exportVolumeAsTraw3dssToolStripMenuItem->Size = System::Drawing::Size(218, 22);
      this->exportVolumeAsTraw3dssToolStripMenuItem->Text = L"export volume as traw3d_ss";
      // 
      // modeToolStripMenuItem
      // 
      this->modeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {
        this->visualizationStandardToolStripMenuItem,
          this->visualizationMaskToolStripMenuItem, this->toolStripSeparator3, this->segmentationToolStripMenuItem, this->segmentationThresholdToolStripMenuItem,
          this->segmentationGraphCutToolStripMenuItem, this->segmentationToolStripMenuItem1
      });
      this->modeToolStripMenuItem->Name = L"modeToolStripMenuItem";
      this->modeToolStripMenuItem->Size = System::Drawing::Size(50, 20);
      this->modeToolStripMenuItem->Text = L"Mode";
      // 
      // visualizationStandardToolStripMenuItem
      // 
      this->visualizationStandardToolStripMenuItem->Name = L"visualizationStandardToolStripMenuItem";
      this->visualizationStandardToolStripMenuItem->Size = System::Drawing::Size(267, 22);
      this->visualizationStandardToolStripMenuItem->Text = L"Visualization Standard";
      // 
      // visualizationMaskToolStripMenuItem
      // 
      this->visualizationMaskToolStripMenuItem->Name = L"visualizationMaskToolStripMenuItem";
      this->visualizationMaskToolStripMenuItem->Size = System::Drawing::Size(267, 22);
      this->visualizationMaskToolStripMenuItem->Text = L"Visualization Mask";
      // 
      // toolStripSeparator3
      // 
      this->toolStripSeparator3->Name = L"toolStripSeparator3";
      this->toolStripSeparator3->Size = System::Drawing::Size(264, 6);
      // 
      // segmentationToolStripMenuItem
      // 
      this->segmentationToolStripMenuItem->Name = L"segmentationToolStripMenuItem";
      this->segmentationToolStripMenuItem->Size = System::Drawing::Size(267, 22);
      this->segmentationToolStripMenuItem->Text = L"Segmentation Pixel Paint";
      // 
      // segmentationThresholdToolStripMenuItem
      // 
      this->segmentationThresholdToolStripMenuItem->Name = L"segmentationThresholdToolStripMenuItem";
      this->segmentationThresholdToolStripMenuItem->Size = System::Drawing::Size(267, 22);
      this->segmentationThresholdToolStripMenuItem->Text = L"Segmentation Threshold";
      // 
      // segmentationGraphCutToolStripMenuItem
      // 
      this->segmentationGraphCutToolStripMenuItem->Name = L"segmentationGraphCutToolStripMenuItem";
      this->segmentationGraphCutToolStripMenuItem->Size = System::Drawing::Size(267, 22);
      this->segmentationGraphCutToolStripMenuItem->Text = L"Segmentation GraphCut";
      // 
      // segmentationToolStripMenuItem1
      // 
      this->segmentationToolStripMenuItem1->Name = L"segmentationToolStripMenuItem1";
      this->segmentationToolStripMenuItem1->Size = System::Drawing::Size(267, 22);
      this->segmentationToolStripMenuItem1->Text = L"Segmentation Local Region Growing";
      // 
      // miscsToolStripMenuItem
      // 
      this->miscsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
        this->exportCurrentCameraPosToolStripMenuItem,
          this->importCurrentCameraPosToolStripMenuItem
      });
      this->miscsToolStripMenuItem->Name = L"miscsToolStripMenuItem";
      this->miscsToolStripMenuItem->Size = System::Drawing::Size(49, 20);
      this->miscsToolStripMenuItem->Text = L"miscs";
      // 
      // exportCurrentCameraPosToolStripMenuItem
      // 
      this->exportCurrentCameraPosToolStripMenuItem->Name = L"exportCurrentCameraPosToolStripMenuItem";
      this->exportCurrentCameraPosToolStripMenuItem->Size = System::Drawing::Size(215, 22);
      this->exportCurrentCameraPosToolStripMenuItem->Text = L"export current camera pos";
      // 
      // importCurrentCameraPosToolStripMenuItem
      // 
      this->importCurrentCameraPosToolStripMenuItem->Name = L"importCurrentCameraPosToolStripMenuItem";
      this->importCurrentCameraPosToolStripMenuItem->Size = System::Drawing::Size(215, 22);
      this->importCurrentCameraPosToolStripMenuItem->Text = L"import current camera pos";
      // 
      // FormMain
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(668, 508);
      this->Controls->Add(this->FormMainPanel);
      this->Controls->Add(this->menuStrip1);
      this->MainMenuStrip = this->menuStrip1;
      this->Margin = System::Windows::Forms::Padding(2);
      this->Name = L"FormMain";
      this->Text = L"FormMain";
      this->Move       += gcnew System::EventHandler(this, &FormMain::FormMain_Move);
      this->Resize     += gcnew System::EventHandler(this, &FormMain::FormMain_Resize);
      this->MouseWheel += gcnew System::Windows::Forms::MouseEventHandler(this, &FormMain::FormMain_MouseWheel);
      this->menuStrip1->ResumeLayout(false);
      this->menuStrip1->PerformLayout();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
    private: 
      System::Void FormMainPanel_MouseDown(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
      System::Void FormMainPanel_MouseMove(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
      System::Void FormMainPanel_MouseUp  (System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
      System::Void FormMainPanel_Resize   (System::Object^  sender, System::EventArgs^  e);
      System::Void FormMainPanel_Paint    (System::Object^  sender, System::Windows::Forms::PaintEventArgs^  e);
      System::Void FormMainPanel_MouseDoubleClick(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);

      System::Void FormMain_MouseWheel(System::Object^  sender, System::Windows::Forms::MouseEventArgs^  e);
      System::Void FormMain_Resize(System::Object^  sender, System::EventArgs^  e);
      System::Void FormMain_Move  (System::Object^  sender, System::EventArgs^  e);
  };



  inline void formMain_redrawMainPanel(){
    FormMain::getInst()->redrawMainPanel();
  }
}
