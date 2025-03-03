#include "stdafx.h"
#include <string>
#include "pch.h"
#include "TransformDialog.h"

IMPLEMENT_DYNAMIC(TransformDialog, CDialogEx)

BEGIN_MESSAGE_MAP(TransformDialog, CDialogEx)
	ON_COMMAND(IDOK, &TransformDialog::End)
	ON_EN_CHANGE(IDC_TRANSLATION_X, &TransformDialog::TranslateX)
	ON_EN_CHANGE(IDC_TRANSLATION_Y, &TransformDialog::TranslateY)
	ON_EN_CHANGE(IDC_TRANSLATION_Z, &TransformDialog::TranslateZ)
	ON_EN_CHANGE(IDC_ROTATION_X, &TransformDialog::RotateX)
	ON_EN_CHANGE(IDC_ROTATION_y, &TransformDialog::RotateY)
	ON_EN_CHANGE(IDC_ROTATION_Z, &TransformDialog::RotateZ)
	ON_EN_CHANGE(IDC_SCALE_X, &TransformDialog::ScaleX)
	ON_EN_CHANGE(IDC_SCALE_Y, &TransformDialog::ScaleY)
	ON_EN_CHANGE(IDC_SCALE_Z, &TransformDialog::ScaleZ)
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

}

void TransformDialog::TranslateX()
{

}

void TransformDialog::TranslateY()
{

}

void TransformDialog::TranslateZ()
{

}

void TransformDialog::RotateX()
{

}

void TransformDialog::RotateY()
{

}

void TransformDialog::RotateZ()
{

}

void TransformDialog::ScaleX()
{

}

void TransformDialog::ScaleY()
{

}

void TransformDialog::ScaleZ()
{

}

void TransformDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TRANSLATION_X, transX);
	DDX_Control(pDX, IDC_TRANSLATION_Y, transY);
	DDX_Control(pDX, IDC_TRANSLATION_Z, transZ);
	DDX_Control(pDX, IDC_ROTATION_X, rotaX);
	DDX_Control(pDX, IDC_ROTATION_X, rotaY);
	DDX_Control(pDX, IDC_ROTATION_X, rotaZ);
	DDX_Control(pDX, IDC_SCALE_X, scalX);
	DDX_Control(pDX, IDC_SCALE_X, scalY);
	DDX_Control(pDX, IDC_SCALE_X, scalZ);
}

void TransformDialog::SetObjectData(float PosX, float PosY, float PosZ, float RotX, float RotY, float RotZ, float ScaX, float ScaY, float ScaZ)
{
	posX = PosX;
	posY = PosY;
	posZ = PosZ;
	rotX = RotX;
	rotY = RotY;
	rotZ = RotZ;
	scaZ = ScaX;
	scaZ = ScaY;
	scaZ = ScaZ;

	transX.SetWindowTextW(std::to_wstring(posX).c_str());
	transY.SetWindowTextW(std::to_wstring(posX).c_str());
	transZ.SetWindowTextW(std::to_wstring(posX).c_str());
	rotaX.SetWindowTextW(std::to_wstring(posX).c_str());
	rotaY.SetWindowTextW(std::to_wstring(posX).c_str());
	rotaZ.SetWindowTextW(std::to_wstring(posX).c_str());
	scalX.SetWindowTextW(std::to_wstring(posX).c_str());
	scalY.SetWindowTextW(std::to_wstring(posX).c_str());
	scalZ.SetWindowTextW(std::to_wstring(posX).c_str());
}
