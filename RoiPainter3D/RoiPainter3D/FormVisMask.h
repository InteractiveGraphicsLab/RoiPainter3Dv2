#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormVisMask の概要
	/// </summary>
	public ref class FormVisMask : public System::Windows::Forms::Form
	{


  private:
    static FormVisMask^ m_singleton;
  private: System::Windows::Forms::Button^  btnExpObj;

  private: System::Windows::Forms::Button^  btnErode;
  private: System::Windows::Forms::Button^  btnDilate;
  private: System::Windows::Forms::Button^  btnFillHole;



           bool   m_bListUpdating;

    FormVisMask(void)
		{
			InitializeComponent();

      this->ControlBox  = false;
      this->MaximizeBox = false;
      m_bListUpdating   = false;

      trackbar_alpha->SetRange(0, 100);
		}

  public:


    static FormVisMask^ getInst() 
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormVisMask();
      return m_singleton;
    }

    void updateList();





	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormVisMask()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Button^  btnMargeTo;
  protected:
  private: System::Windows::Forms::Button^  btnDelete;
  private: System::Windows::Forms::Button^  btnTestAdd;
  private: System::Windows::Forms::Button^  btnColorPallet;
  private: System::Windows::Forms::CheckBox^  checkbox_lock;
  private: System::Windows::Forms::DataGridView^  maskList;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  regionIDColumn;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  colorColumn;
  private: System::Windows::Forms::TrackBar^  trackbar_alpha;










  private: System::Windows::Forms::TextBox^  alpha;

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
      System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle6 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
      this->btnMargeTo = (gcnew System::Windows::Forms::Button());
      this->btnDelete = (gcnew System::Windows::Forms::Button());
      this->btnTestAdd = (gcnew System::Windows::Forms::Button());
      this->btnColorPallet = (gcnew System::Windows::Forms::Button());
      this->checkbox_lock = (gcnew System::Windows::Forms::CheckBox());
      this->maskList = (gcnew System::Windows::Forms::DataGridView());
      this->regionIDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->colorColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->trackbar_alpha = (gcnew System::Windows::Forms::TrackBar());
      this->alpha = (gcnew System::Windows::Forms::TextBox());
      this->btnExpObj = (gcnew System::Windows::Forms::Button());
      this->btnErode = (gcnew System::Windows::Forms::Button());
      this->btnDilate = (gcnew System::Windows::Forms::Button());
      this->btnFillHole = (gcnew System::Windows::Forms::Button());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->BeginInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_alpha))->BeginInit();
      this->SuspendLayout();
      // 
      // btnMargeTo
      // 
      this->btnMargeTo->Location = System::Drawing::Point(271, 112);
      this->btnMargeTo->Margin = System::Windows::Forms::Padding(4);
      this->btnMargeTo->Name = L"btnMargeTo";
      this->btnMargeTo->Size = System::Drawing::Size(99, 29);
      this->btnMargeTo->TabIndex = 34;
      this->btnMargeTo->Text = L"MARGE TO";
      this->btnMargeTo->UseVisualStyleBackColor = true;
      this->btnMargeTo->Click += gcnew System::EventHandler(this, &FormVisMask::btnMargeTo_Click);
      // 
      // btnDelete
      // 
      this->btnDelete->Location = System::Drawing::Point(179, 112);
      this->btnDelete->Margin = System::Windows::Forms::Padding(4);
      this->btnDelete->Name = L"btnDelete";
      this->btnDelete->Size = System::Drawing::Size(88, 29);
      this->btnDelete->TabIndex = 33;
      this->btnDelete->Text = L"DELETE";
      this->btnDelete->UseVisualStyleBackColor = true;
      this->btnDelete->Click += gcnew System::EventHandler(this, &FormVisMask::btnDelete_Click);
      // 
      // btnTestAdd
      // 
      this->btnTestAdd->Location = System::Drawing::Point(264, 463);
      this->btnTestAdd->Margin = System::Windows::Forms::Padding(4);
      this->btnTestAdd->Name = L"btnTestAdd";
      this->btnTestAdd->Size = System::Drawing::Size(100, 29);
      this->btnTestAdd->TabIndex = 30;
      this->btnTestAdd->Text = L"Add";
      this->btnTestAdd->UseVisualStyleBackColor = true;
      // 
      // btnColorPallet
      // 
      this->btnColorPallet->Location = System::Drawing::Point(255, 6);
      this->btnColorPallet->Margin = System::Windows::Forms::Padding(4);
      this->btnColorPallet->Name = L"btnColorPallet";
      this->btnColorPallet->Size = System::Drawing::Size(100, 29);
      this->btnColorPallet->TabIndex = 29;
      this->btnColorPallet->Text = L"color";
      this->btnColorPallet->UseVisualStyleBackColor = true;
      this->btnColorPallet->Click += gcnew System::EventHandler(this, &FormVisMask::btnColorPallet_Click);
      // 
      // checkbox_lock
      // 
      this->checkbox_lock->AutoSize = true;
      this->checkbox_lock->Location = System::Drawing::Point(184, 11);
      this->checkbox_lock->Margin = System::Windows::Forms::Padding(4);
      this->checkbox_lock->Name = L"checkbox_lock";
      this->checkbox_lock->Size = System::Drawing::Size(60, 19);
      this->checkbox_lock->TabIndex = 26;
      this->checkbox_lock->Text = L"Lock";
      this->checkbox_lock->UseVisualStyleBackColor = true;
      this->checkbox_lock->CheckedChanged += gcnew System::EventHandler(this, &FormVisMask::checkbox_lock_CheckedChanged);
      // 
      // maskList
      // 
      this->maskList->AllowUserToAddRows = false;
      this->maskList->AllowUserToDeleteRows = false;
      this->maskList->AllowUserToResizeColumns = false;
      this->maskList->AutoSizeColumnsMode = System::Windows::Forms::DataGridViewAutoSizeColumnsMode::AllCells;
      this->maskList->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCells;
      this->maskList->BackgroundColor = System::Drawing::SystemColors::ButtonShadow;
      this->maskList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->maskList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
        this->regionIDColumn,
          this->colorColumn
      });
      this->maskList->Location = System::Drawing::Point(6, 7);
      this->maskList->Margin = System::Windows::Forms::Padding(4);
      this->maskList->MultiSelect = false;
      this->maskList->Name = L"maskList";
      this->maskList->ReadOnly = true;
      this->maskList->RowHeadersVisible = false;
      this->maskList->RowTemplate->Height = 21;
      this->maskList->SelectionMode = System::Windows::Forms::DataGridViewSelectionMode::FullRowSelect;
      this->maskList->Size = System::Drawing::Size(171, 490);
      this->maskList->TabIndex = 25;
      this->maskList->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FormVisMask::maskList_CellContentClick);
      this->maskList->SelectionChanged += gcnew System::EventHandler(this, &FormVisMask::maskList_SelectionChanged);
      // 
      // regionIDColumn
      // 
      this->regionIDColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->regionIDColumn->HeaderText = L"region ID";
      this->regionIDColumn->Name = L"regionIDColumn";
      this->regionIDColumn->ReadOnly = true;
      this->regionIDColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->regionIDColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // colorColumn
      // 
      this->colorColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      dataGridViewCellStyle6->BackColor = System::Drawing::Color::White;
      dataGridViewCellStyle6->ForeColor = System::Drawing::Color::Black;
      dataGridViewCellStyle6->SelectionBackColor = System::Drawing::Color::Transparent;
      dataGridViewCellStyle6->SelectionForeColor = System::Drawing::Color::Black;
      this->colorColumn->DefaultCellStyle = dataGridViewCellStyle6;
      this->colorColumn->FillWeight = 80;
      this->colorColumn->HeaderText = L"color";
      this->colorColumn->Name = L"colorColumn";
      this->colorColumn->ReadOnly = true;
      this->colorColumn->Resizable = System::Windows::Forms::DataGridViewTriState::False;
      this->colorColumn->SortMode = System::Windows::Forms::DataGridViewColumnSortMode::NotSortable;
      // 
      // trackbar_alpha
      // 
      this->trackbar_alpha->Location = System::Drawing::Point(216, 41);
      this->trackbar_alpha->Margin = System::Windows::Forms::Padding(4);
      this->trackbar_alpha->Name = L"trackbar_alpha";
      this->trackbar_alpha->Size = System::Drawing::Size(148, 56);
      this->trackbar_alpha->TabIndex = 27;
      this->trackbar_alpha->Scroll += gcnew System::EventHandler(this, &FormVisMask::trackbar_alpha_Scroll);
      // 
      // alpha
      // 
      this->alpha->BackColor = System::Drawing::SystemColors::Menu;
      this->alpha->BorderStyle = System::Windows::Forms::BorderStyle::None;
      this->alpha->Location = System::Drawing::Point(180, 56);
      this->alpha->Margin = System::Windows::Forms::Padding(4);
      this->alpha->Name = L"alpha";
      this->alpha->ReadOnly = true;
      this->alpha->Size = System::Drawing::Size(59, 15);
      this->alpha->TabIndex = 28;
      this->alpha->Text = L"alpha";
      // 
      // btnExpObj
      // 
      this->btnExpObj->Location = System::Drawing::Point(215, 283);
      this->btnExpObj->Margin = System::Windows::Forms::Padding(4);
      this->btnExpObj->Name = L"btnExpObj";
      this->btnExpObj->Size = System::Drawing::Size(79, 29);
      this->btnExpObj->TabIndex = 35;
      this->btnExpObj->Text = L"Exp Obj";
      this->btnExpObj->UseVisualStyleBackColor = true;
      this->btnExpObj->Click += gcnew System::EventHandler(this, &FormVisMask::btnExpObj_Click);
      // 
      // btnErode
      // 
      this->btnErode->Location = System::Drawing::Point(216, 174);
      this->btnErode->Margin = System::Windows::Forms::Padding(4);
      this->btnErode->Name = L"btnErode";
      this->btnErode->Size = System::Drawing::Size(69, 29);
      this->btnErode->TabIndex = 36;
      this->btnErode->Text = L"erode";
      this->btnErode->UseVisualStyleBackColor = true;
      this->btnErode->Click += gcnew System::EventHandler(this, &FormVisMask::btnErode_Click);
      // 
      // btnDilate
      // 
      this->btnDilate->Location = System::Drawing::Point(216, 211);
      this->btnDilate->Margin = System::Windows::Forms::Padding(4);
      this->btnDilate->Name = L"btnDilate";
      this->btnDilate->Size = System::Drawing::Size(69, 29);
      this->btnDilate->TabIndex = 37;
      this->btnDilate->Text = L"dilate";
      this->btnDilate->UseVisualStyleBackColor = true;
      this->btnDilate->Click += gcnew System::EventHandler(this, &FormVisMask::btnDilate_Click);
      // 
      // btnFillHole
      // 
      this->btnFillHole->Location = System::Drawing::Point(216, 247);
      this->btnFillHole->Margin = System::Windows::Forms::Padding(4);
      this->btnFillHole->Name = L"btnFillHole";
      this->btnFillHole->Size = System::Drawing::Size(69, 29);
      this->btnFillHole->TabIndex = 38;
      this->btnFillHole->Text = L"fill hole";
      this->btnFillHole->UseVisualStyleBackColor = true;
      this->btnFillHole->Click += gcnew System::EventHandler(this, &FormVisMask::btnFillHole_Click);
      // 
      // FormVisMask
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(378, 505);
      this->Controls->Add(this->btnExpObj);
      this->Controls->Add(this->btnErode);
      this->Controls->Add(this->btnDilate);
      this->Controls->Add(this->btnFillHole);
      this->Controls->Add(this->btnMargeTo);
      this->Controls->Add(this->btnDelete);
      this->Controls->Add(this->btnTestAdd);
      this->Controls->Add(this->btnColorPallet);
      this->Controls->Add(this->checkbox_lock);
      this->Controls->Add(this->maskList);
      this->Controls->Add(this->trackbar_alpha);
      this->Controls->Add(this->alpha);
      this->Name = L"FormVisMask";
      this->Text = L"FormVisMask";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskList))->EndInit();
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->trackbar_alpha))->EndInit();
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
  private: System::Void maskList_SelectionChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void maskList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
  private: System::Void checkbox_lock_CheckedChanged(System::Object^  sender, System::EventArgs^  e);
  private: System::Void trackbar_alpha_Scroll (System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btnColorPallet_Click  (System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btnDelete_Click       (System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btnMargeTo_Click      (System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btnErode_Click        (System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btnDilate_Click       (System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnFillHole_Click     (System::Object^  sender, System::EventArgs^  e);
  private: System::Void btnExpObj_Click       (System::Object^  sender, System::EventArgs^  e) ;
};

  inline void formVisMask_Show() { FormVisMask::getInst()->Show(); }
  inline void formVisMask_Hide() { FormVisMask::getInst()->Hide(); }
  inline void formVisMask_updateList() { FormVisMask::getInst()->updateList(); }

}
