#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormMaskIDselection の概要
	/// </summary>
	public ref class FormMaskIDselection : public System::Windows::Forms::Form
	{
  private:
    int  m_bListInit;
    int  m_activeId;
    void initList();

	public:
		FormMaskIDselection(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
      m_bListInit = false;
      initList();
		}

    int  getTrgtID() { return m_activeId; } //should be called after the OK btn presssed

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormMaskIDselection()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Button^  btnCancel;
  protected:
  private: System::Windows::Forms::Button^  btnOk;
  private: System::Windows::Forms::DataGridView^  maskIdList;

  private: System::Windows::Forms::DataGridViewTextBoxColumn^  maskIDColumn;
  private: System::Windows::Forms::DataGridViewTextBoxColumn^  colorColumn;

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
      this->btnCancel = (gcnew System::Windows::Forms::Button());
      this->btnOk = (gcnew System::Windows::Forms::Button());
      this->maskIdList = (gcnew System::Windows::Forms::DataGridView());
      this->maskIDColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      this->colorColumn = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskIdList))->BeginInit();
      this->SuspendLayout();
      // 
      // btnCancel
      // 
      this->btnCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
      this->btnCancel->Location = System::Drawing::Point(139, 126);
      this->btnCancel->Name = L"btnCancel";
      this->btnCancel->Size = System::Drawing::Size(63, 32);
      this->btnCancel->TabIndex = 5;
      this->btnCancel->Text = L"CANCEL";
      this->btnCancel->UseVisualStyleBackColor = true;
      this->btnCancel->Click += gcnew System::EventHandler(this, &FormMaskIDselection::btnCancel_Click);
      // 
      // btnOk
      // 
      this->btnOk->DialogResult = System::Windows::Forms::DialogResult::OK;
      this->btnOk->Location = System::Drawing::Point(139, 88);
      this->btnOk->Name = L"btnOk";
      this->btnOk->Size = System::Drawing::Size(63, 32);
      this->btnOk->TabIndex = 4;
      this->btnOk->Text = L"OK";
      this->btnOk->UseVisualStyleBackColor = true;
      this->btnOk->Click += gcnew System::EventHandler(this, &FormMaskIDselection::btnOk_Click);
      // 
      // maskIdList
      // 
      this->maskIdList->AllowUserToAddRows = false;
      this->maskIdList->AllowUserToDeleteRows = false;
      this->maskIdList->AllowUserToResizeColumns = false;
      this->maskIdList->AllowUserToResizeRows = false;
      this->maskIdList->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
      this->maskIdList->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {
        this->maskIDColumn,
          this->colorColumn
      });
      this->maskIdList->Location = System::Drawing::Point(12, 12);
      this->maskIdList->Name = L"maskIdList";
      this->maskIdList->ReadOnly = true;
      this->maskIdList->RowHeadersVisible = false;
      this->maskIdList->RowTemplate->Height = 21;
      this->maskIdList->Size = System::Drawing::Size(121, 289);
      this->maskIdList->TabIndex = 3;
      this->maskIdList->CellContentClick += gcnew System::Windows::Forms::DataGridViewCellEventHandler(this, &FormMaskIDselection::maskIdList_CellContentClick);
      this->maskIdList->SelectionChanged += gcnew System::EventHandler(this, &FormMaskIDselection::maskIdList_SelectionChanged);
      // 
      // maskIDColumn
      // 
      this->maskIDColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->maskIDColumn->HeaderText = L"maskId";
      this->maskIDColumn->Name = L"maskIDColumn";
      this->maskIDColumn->ReadOnly = true;
      // 
      // colorColumn
      // 
      this->colorColumn->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
      this->colorColumn->FillWeight = 70;
      this->colorColumn->HeaderText = L"color";
      this->colorColumn->Name = L"colorColumn";
      this->colorColumn->ReadOnly = true;
      // 
      // FormMaskIDselection
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(207, 312);
      this->Controls->Add(this->btnCancel);
      this->Controls->Add(this->btnOk);
      this->Controls->Add(this->maskIdList);
      this->Name = L"FormMaskIDselection";
      this->Text = L"FormMaskIDselection";
      (cli::safe_cast<System::ComponentModel::ISupportInitialize^>(this->maskIdList))->EndInit();
      this->ResumeLayout(false);

    }
#pragma endregion
  private: System::Void btnOk_Click(System::Object^  sender, System::EventArgs^  e) ;
  private: System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e);
  private: System::Void maskIdList_CellContentClick(System::Object^  sender, System::Windows::Forms::DataGridViewCellEventArgs^  e);
  private: System::Void maskIdList_SelectionChanged(System::Object^  sender, System::EventArgs^  e);
};

  //選択されなかった場合は-1が返る
  inline int formMaskIdSelection_showModalDialog()
  {
    FormMaskIDselection ^modal = gcnew FormMaskIDselection();
    if (modal->ShowDialog() == System::Windows::Forms::DialogResult::Cancel) return -1;

    int trgtId = modal->getTrgtID();
    modal->Close();
    return trgtId;
  }

}
