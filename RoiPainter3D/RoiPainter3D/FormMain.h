#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormMain の概要
	/// </summary>
	public ref class FormMain : public System::Windows::Forms::Form
	{

    static FormMain^ m_singleton;
    FormMain(void);

  public:
    static FormMain^ getInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormMain();
      return m_singleton;
    }






	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormMain()
		{
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
      this->FormMainPanel = (gcnew System::Windows::Forms::Panel());
      this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
      this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open2DSlicesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open2DSlicesdcmToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open3DVolumetraw3DToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open3DColumedcmToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->modeToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->saveMaskmskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->loadMaskmskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->saveMaskfavbToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->open3DVolumefavToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exportVolumeAsTraw3dssToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->miscsToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->exportCurrentCameraPosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->importCurrentCameraPosToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->visualizationStandardToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->visualizationMaskToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->toolStripSeparator3 = (gcnew System::Windows::Forms::ToolStripSeparator());
      this->segmentationThresholdToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->segmentationGraphCutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->segmentationToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->segmentationToolStripMenuItem1 = (gcnew System::Windows::Forms::ToolStripMenuItem());
      this->menuStrip1->SuspendLayout();
      this->SuspendLayout();
      // 
      // FormMainPanel
      // 
      this->FormMainPanel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom)
        | System::Windows::Forms::AnchorStyles::Left)
        | System::Windows::Forms::AnchorStyles::Right));
      this->FormMainPanel->Location = System::Drawing::Point(2, 35);
      this->FormMainPanel->Name = L"FormMainPanel";
      this->FormMainPanel->Size = System::Drawing::Size(889, 601);
      this->FormMainPanel->TabIndex = 0;
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
      this->menuStrip1->Size = System::Drawing::Size(891, 28);
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
      this->fileToolStripMenuItem->Size = System::Drawing::Size(44, 24);
      this->fileToolStripMenuItem->Text = L"File";
      // 
      // open2DSlicesToolStripMenuItem
      // 
      this->open2DSlicesToolStripMenuItem->Name = L"open2DSlicesToolStripMenuItem";
      this->open2DSlicesToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->open2DSlicesToolStripMenuItem->Text = L"open 2D slices (bmp, tif)";
      // 
      // open2DSlicesdcmToolStripMenuItem
      // 
      this->open2DSlicesdcmToolStripMenuItem->Name = L"open2DSlicesdcmToolStripMenuItem";
      this->open2DSlicesdcmToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->open2DSlicesdcmToolStripMenuItem->Text = L"open 2D slices (dcm)";
      // 
      // open3DVolumetraw3DToolStripMenuItem
      // 
      this->open3DVolumetraw3DToolStripMenuItem->Name = L"open3DVolumetraw3DToolStripMenuItem";
      this->open3DVolumetraw3DToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->open3DVolumetraw3DToolStripMenuItem->Text = L"open 3D volume (traw3D)";
      // 
      // open3DColumedcmToolStripMenuItem
      // 
      this->open3DColumedcmToolStripMenuItem->Name = L"open3DColumedcmToolStripMenuItem";
      this->open3DColumedcmToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->open3DColumedcmToolStripMenuItem->Text = L"open 3D volume (dcm)";
      // 
      // modeToolStripMenuItem
      // 
      this->modeToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(7) {
        this->visualizationStandardToolStripMenuItem,
          this->visualizationMaskToolStripMenuItem, this->toolStripSeparator3, this->segmentationToolStripMenuItem, this->segmentationThresholdToolStripMenuItem,
          this->segmentationGraphCutToolStripMenuItem, this->segmentationToolStripMenuItem1
      });
      this->modeToolStripMenuItem->Name = L"modeToolStripMenuItem";
      this->modeToolStripMenuItem->Size = System::Drawing::Size(60, 24);
      this->modeToolStripMenuItem->Text = L"Mode";
      // 
      // toolStripSeparator1
      // 
      this->toolStripSeparator1->Name = L"toolStripSeparator1";
      this->toolStripSeparator1->Size = System::Drawing::Size(263, 6);
      // 
      // saveMaskmskToolStripMenuItem
      // 
      this->saveMaskmskToolStripMenuItem->Name = L"saveMaskmskToolStripMenuItem";
      this->saveMaskmskToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->saveMaskmskToolStripMenuItem->Text = L"save mask (msk)";
      // 
      // loadMaskmskToolStripMenuItem
      // 
      this->loadMaskmskToolStripMenuItem->Name = L"loadMaskmskToolStripMenuItem";
      this->loadMaskmskToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->loadMaskmskToolStripMenuItem->Text = L"load mask (msk)";
      // 
      // saveMaskfavbToolStripMenuItem
      // 
      this->saveMaskfavbToolStripMenuItem->Name = L"saveMaskfavbToolStripMenuItem";
      this->saveMaskfavbToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->saveMaskfavbToolStripMenuItem->Text = L"save mask (fav)";
      // 
      // open3DVolumefavToolStripMenuItem
      // 
      this->open3DVolumefavToolStripMenuItem->Name = L"open3DVolumefavToolStripMenuItem";
      this->open3DVolumefavToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->open3DVolumefavToolStripMenuItem->Text = L"open 3D volume (fav)";
      // 
      // exportVolumeAsTraw3dssToolStripMenuItem
      // 
      this->exportVolumeAsTraw3dssToolStripMenuItem->Name = L"exportVolumeAsTraw3dssToolStripMenuItem";
      this->exportVolumeAsTraw3dssToolStripMenuItem->Size = System::Drawing::Size(266, 26);
      this->exportVolumeAsTraw3dssToolStripMenuItem->Text = L"export volume as traw3d_ss";
      // 
      // toolStripSeparator2
      // 
      this->toolStripSeparator2->Name = L"toolStripSeparator2";
      this->toolStripSeparator2->Size = System::Drawing::Size(263, 6);
      // 
      // miscsToolStripMenuItem
      // 
      this->miscsToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {
        this->exportCurrentCameraPosToolStripMenuItem,
          this->importCurrentCameraPosToolStripMenuItem
      });
      this->miscsToolStripMenuItem->Name = L"miscsToolStripMenuItem";
      this->miscsToolStripMenuItem->Size = System::Drawing::Size(57, 24);
      this->miscsToolStripMenuItem->Text = L"miscs";
      // 
      // exportCurrentCameraPosToolStripMenuItem
      // 
      this->exportCurrentCameraPosToolStripMenuItem->Name = L"exportCurrentCameraPosToolStripMenuItem";
      this->exportCurrentCameraPosToolStripMenuItem->Size = System::Drawing::Size(260, 26);
      this->exportCurrentCameraPosToolStripMenuItem->Text = L"export current camera pos";
      // 
      // importCurrentCameraPosToolStripMenuItem
      // 
      this->importCurrentCameraPosToolStripMenuItem->Name = L"importCurrentCameraPosToolStripMenuItem";
      this->importCurrentCameraPosToolStripMenuItem->Size = System::Drawing::Size(260, 26);
      this->importCurrentCameraPosToolStripMenuItem->Text = L"import current camera pos";
      // 
      // visualizationStandardToolStripMenuItem
      // 
      this->visualizationStandardToolStripMenuItem->Name = L"visualizationStandardToolStripMenuItem";
      this->visualizationStandardToolStripMenuItem->Size = System::Drawing::Size(327, 26);
      this->visualizationStandardToolStripMenuItem->Text = L"Visualization Standard";
      // 
      // visualizationMaskToolStripMenuItem
      // 
      this->visualizationMaskToolStripMenuItem->Name = L"visualizationMaskToolStripMenuItem";
      this->visualizationMaskToolStripMenuItem->Size = System::Drawing::Size(327, 26);
      this->visualizationMaskToolStripMenuItem->Text = L"Visualization Mask";
      // 
      // toolStripSeparator3
      // 
      this->toolStripSeparator3->Name = L"toolStripSeparator3";
      this->toolStripSeparator3->Size = System::Drawing::Size(324, 6);
      // 
      // segmentationThresholdToolStripMenuItem
      // 
      this->segmentationThresholdToolStripMenuItem->Name = L"segmentationThresholdToolStripMenuItem";
      this->segmentationThresholdToolStripMenuItem->Size = System::Drawing::Size(327, 26);
      this->segmentationThresholdToolStripMenuItem->Text = L"Segmentation Threshold";
      // 
      // segmentationGraphCutToolStripMenuItem
      // 
      this->segmentationGraphCutToolStripMenuItem->Name = L"segmentationGraphCutToolStripMenuItem";
      this->segmentationGraphCutToolStripMenuItem->Size = System::Drawing::Size(327, 26);
      this->segmentationGraphCutToolStripMenuItem->Text = L"Segmentation GraphCut";
      // 
      // segmentationToolStripMenuItem
      // 
      this->segmentationToolStripMenuItem->Name = L"segmentationToolStripMenuItem";
      this->segmentationToolStripMenuItem->Size = System::Drawing::Size(327, 26);
      this->segmentationToolStripMenuItem->Text = L"Segmentation Pixel Paint";
      // 
      // segmentationToolStripMenuItem1
      // 
      this->segmentationToolStripMenuItem1->Name = L"segmentationToolStripMenuItem1";
      this->segmentationToolStripMenuItem1->Size = System::Drawing::Size(327, 26);
      this->segmentationToolStripMenuItem1->Text = L"Segmentation Local Region Growing";
      // 
      // FormMain
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(891, 635);
      this->Controls->Add(this->FormMainPanel);
      this->Controls->Add(this->menuStrip1);
      this->MainMenuStrip = this->menuStrip1;
      this->Name = L"FormMain";
      this->Text = L"FormMain";
      this->menuStrip1->ResumeLayout(false);
      this->menuStrip1->PerformLayout();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion

};
}
