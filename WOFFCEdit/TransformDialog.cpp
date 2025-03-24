#include "stdafx.h"
#include <string>
#include "ToolMain.h"
#include "TransformDialog.h"

IMPLEMENT_DYNAMIC(TransformDialog, CDialogEx)

BEGIN_MESSAGE_MAP(TransformDialog, CDialogEx)
	ON_COMMAND(IDOK, &TransformDialog::End)
	ON_COMMAND(IDCANCEL, &TransformDialog::End)
	ON_EN_CHANGE(IDC_TRANSLATION_X, &TransformDialog::Translate)
	ON_EN_CHANGE(IDC_TRANSLATION_Y, &TransformDialog::Translate)
	ON_EN_CHANGE(IDC_TRANSLATION_Z, &TransformDialog::Translate)
	ON_EN_CHANGE(IDC_ROTATION_X, &TransformDialog::Rotate)
	ON_EN_CHANGE(IDC_ROTATION_y, &TransformDialog::Rotate)
	ON_EN_CHANGE(IDC_ROTATION_Z, &TransformDialog::Rotate)
	ON_EN_CHANGE(IDC_SCALE_X, &TransformDialog::Scale)
	ON_EN_CHANGE(IDC_SCALE_Y, &TransformDialog::Scale)
	ON_EN_CHANGE(IDC_SCALE_Z, &TransformDialog::Scale)
END_MESSAGE_MAP()

TransformDialog::TransformDialog(CWnd* pParent)
	: CDialogEx(IDD_TRANSFORM, pParent)
{
}

TransformDialog::~TransformDialog()
{
}

void TransformDialog::End()
{
	m_toolMain->OnDialogMouseLeave();
	DestroyWindow();
}

void TransformDialog::Translate()
{
	if (numberOfChanges < 9)
	{
		numberOfChanges++;
		return;
	}
	
	CString tranString;
	transX.GetWindowText(tranString);
	float x = _ttof(tranString);

	transY.GetWindowText(tranString);
	float y = _ttof(tranString);

	transZ.GetWindowText(tranString);
	float z = _ttof(tranString);

	m_toolMain->TranslateSelected(x, y, z);
}

void TransformDialog::Rotate()
{
	if (numberOfChanges < 9)
	{
		numberOfChanges++;
		return;
	}
	
	CString tranString;
	rotaX.GetWindowText(tranString);
	float x = _ttof(tranString);

	rotaY.GetWindowText(tranString);
	float y = _ttof(tranString);

	rotaZ.GetWindowText(tranString);
	float z = _ttof(tranString);

	m_toolMain->RotateSelected(x, y, z);
}

void TransformDialog::Scale()
{
	if (numberOfChanges < 9)
	{
		numberOfChanges++;
		return;
	}
	
	CString tranString;
	scalX.GetWindowText(tranString);
	float x = _ttof(tranString);

	scalY.GetWindowText(tranString);
	float y = _ttof(tranString);

	scalZ.GetWindowText(tranString);
	float z = _ttof(tranString);

	m_toolMain->ScaleSelected(x, y, z);
}

void TransformDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TRANSLATION_X, transX);
	DDX_Control(pDX, IDC_TRANSLATION_Y, transY);
	DDX_Control(pDX, IDC_TRANSLATION_Z, transZ);
	DDX_Control(pDX, IDC_ROTATION_X, rotaX);
	DDX_Control(pDX, IDC_ROTATION_y, rotaY);
	DDX_Control(pDX, IDC_ROTATION_Z, rotaZ);
	DDX_Control(pDX, IDC_SCALE_X, scalX);
	DDX_Control(pDX, IDC_SCALE_Y, scalY);
	DDX_Control(pDX, IDC_SCALE_Z, scalZ);
}

void TransformDialog::SetObjectData(ToolMain* toolMain, float PosX, float PosY, float PosZ, float RotX, float RotY, float RotZ, float ScaX, float ScaY, float ScaZ)
{
	m_toolMain = toolMain;
	m_toolMain->OnDialogHovered();

	posX = PosX;
	posY = PosY;
	posZ = PosZ;
	rotX = RotX;
	rotY = RotY;
	rotZ = RotZ;
	scaX = ScaX;
	scaY = ScaY;
	scaZ = ScaZ;

	transX.SetWindowText(std::to_wstring((int)posX).c_str());
	transY.SetWindowText(std::to_wstring((int)posY).c_str());
	transZ.SetWindowText(std::to_wstring((int)posZ).c_str());
	rotaX.SetWindowText(std::to_wstring((int)rotX).c_str());
	rotaY.SetWindowText(std::to_wstring((int)rotY).c_str());
	rotaZ.SetWindowText(std::to_wstring((int)rotZ).c_str());
	scalX.SetWindowText(std::to_wstring((int)scaX).c_str());
	scalY.SetWindowText(std::to_wstring((int)scaY).c_str());
	scalZ.SetWindowText(std::to_wstring((int)scaZ).c_str());
}
