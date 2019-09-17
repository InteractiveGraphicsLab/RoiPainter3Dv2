#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormIntegerSelection の概要
	/// </summary>
	public ref class FormIntegerSelection : public System::Windows::Forms::Form
	{
	public:
		FormIntegerSelection(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクター コードを追加します
			//
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormIntegerSelection()
		{
			if (components)
			{
				delete components;
			}
		}
  private: System::Windows::Forms::Label^  label1;
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
      this->label1 = (gcnew System::Windows::Forms::Label());
      this->SuspendLayout();
      // 
      // label1
      // 
      this->label1->AutoSize = true;
      this->label1->Location = System::Drawing::Point(90, 85);
      this->label1->Name = L"label1";
      this->label1->Size = System::Drawing::Size(30, 12);
      this->label1->TabIndex = 0;
      this->label1->Text = L"Todo";
      // 
      // FormIntegerSelection
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(232, 223);
      this->Controls->Add(this->label1);
      this->Name = L"FormIntegerSelection";
      this->Text = L"FormIntegerSelection";
      this->ResumeLayout(false);
      this->PerformLayout();

    }
#pragma endregion
	};
}
