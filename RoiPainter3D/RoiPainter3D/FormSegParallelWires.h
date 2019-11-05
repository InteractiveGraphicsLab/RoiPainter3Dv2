#pragma once

namespace RoiPainter3D {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// FormSegParallelWires の概要
	/// </summary>
	public ref class FormSegParallelWires : public System::Windows::Forms::Form
	{

  private:
    static FormSegParallelWires^ m_singleton;

	private:
		FormSegParallelWires(void)
		{
			InitializeComponent();
			//
		}

  public:
    static FormSegParallelWires^ GetInst()
    {
      if (m_singleton == nullptr) m_singleton = gcnew FormSegParallelWires();
      return m_singleton;
    }


	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~FormSegParallelWires()
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
      this->SuspendLayout();
      // 
      // FormSegParallelWires
      // 
      this->AutoScaleDimensions = System::Drawing::SizeF(8, 15);
      this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
      this->ClientSize = System::Drawing::Size(535, 412);
      this->Name = L"FormSegParallelWires";
      this->Text = L"FormSegParallelWires";
      this->ResumeLayout(false);

    }
#pragma endregion
	};

  inline void FormSegParallelWires_Hide(){
    FormSegParallelWires::GetInst()->Hide();
  }
  inline void FormSegParallelWires_Show(){
    FormSegParallelWires::GetInst()->Show();
  }

}
