#pragma once

#include "afxwin.h"
#include "afxdialogex.h"
#include "resource.h"

class TransformDialog : public CDialogEx
{
	DECLARE_DYNAMIC(TransformDialog)

public:

	TransformDialog(CWnd* pParent = NULL);
	virtual ~TransformDialog();

	afx_msg void End();		//kill the dialogue

	void SetObjectData(float PosX, float PosY, float PosZ, float RotX, float RotY, float RotZ, float ScaX, float ScaY, float ScaZ);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRANSFORM };
#endif

	afx_msg void TranslateX();
	afx_msg void TranslateY();
	afx_msg void TranslateZ();
	afx_msg void RotateX();
	afx_msg void RotateY();
	afx_msg void RotateZ();
	afx_msg void ScaleX();
	afx_msg void ScaleY();
	afx_msg void ScaleZ();

	DECLARE_MESSAGE_MAP()

protected:

	virtual void DoDataExchange(CDataExchange* pDX) override;

	CEdit transX, transY, transZ;
	CEdit rotaX, rotaY, rotaZ;
	CEdit scalX, scalY, scalZ;

private:

	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float scaX, scaY, scaZ;
};

INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
