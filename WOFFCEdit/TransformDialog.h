#pragma once

#include "afxwin.h"
#include "afxdialogex.h"
#include "resource.h"

class ToolMain;

class TransformDialog : public CDialogEx
{
	DECLARE_DYNAMIC(TransformDialog)

public:

	TransformDialog(CWnd* pParent = NULL);
	virtual ~TransformDialog();

	afx_msg void End();		//kill the dialogue

	void SetObjectData(ToolMain* main, float PosX, float PosY, float PosZ, float RotX, float RotY, float RotZ, float ScaX, float ScaY, float ScaZ);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TRANSFORM };
#endif

	afx_msg void Translate();
	afx_msg void Rotate();
	afx_msg void Scale();

	DECLARE_MESSAGE_MAP()

protected:

	virtual void DoDataExchange(CDataExchange* pDX) override;

	CEdit transX, transY, transZ;
	CEdit rotaX, rotaY, rotaZ;
	CEdit scalX, scalY, scalZ;

private:

	ToolMain* toolMain;
	float posX, posY, posZ;
	float rotX, rotY, rotZ;
	float scaX, scaY, scaZ;
};

INT_PTR CALLBACK SelectProc(HWND   hwndDlg, UINT   uMsg, WPARAM wParam, LPARAM lParam);
