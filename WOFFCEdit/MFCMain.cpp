#include "MFCMain.h"
#include "resource.h"


BEGIN_MESSAGE_MAP(MFCMain, CWinApp)
	ON_COMMAND(ID_FILE_QUIT, &MFCMain::MenuFileQuit)
	ON_COMMAND(ID_FILE_SAVETERRAIN, &MFCMain::MenuFileSaveTerrain)
	ON_COMMAND(ID_EDIT_SELECT, &MFCMain::MenuEditSelect)
	ON_COMMAND(ID_BUTTON40001, &MFCMain::ToggleTerrainPainting)
	ON_COMMAND(ID_BUTTON40009, &MFCMain::IncreaseTerrainBrushSize)
	ON_COMMAND(ID_BUTTON40010, &MFCMain::DecreaseTerrainBrushSize)
	ON_COMMAND(ID_BUTTON40017, &MFCMain::ToggleSplineEditing)
	ON_COMMAND(ID_BUTTON40019, &MFCMain::IncreaseSplineQuality)
	ON_COMMAND(ID_BUTTON40020, &MFCMain::DecreaseSplineQuality)
	ON_COMMAND(ID_BUTTON40021, &MFCMain::ToggleAnimateMinecart)
	ON_COMMAND(ID_EDIT_UNDO, &MFCMain::UndoButton)
	ON_COMMAND(ID_EDIT_REDO, &MFCMain::RedoButton)
	ON_COMMAND(ID_EDIT_TOGGLETERRAINPAINTING, &MFCMain::ToggleTerrainPainting)
	ON_COMMAND(ID_EDIT_TRANSFORMTOOL, &MFCMain::TransformButton)
	ON_UPDATE_COMMAND_UI(ID_INDICATOR_TOOL, &CMyFrame::OnUpdatePage)
END_MESSAGE_MAP()

BOOL MFCMain::InitInstance()
{
	//instanciate the mfc frame
	m_frame = new CMyFrame();
	m_pMainWnd = m_frame;

	m_frame->Create(	NULL,
					_T("World Of Flim-Flam Craft Editor"),
					WS_OVERLAPPEDWINDOW,
					CRect(100, 100, 1920, 1080),
					NULL,
					NULL,
					0,
					NULL
				);

	//show and set the window to run and update. 
	m_frame->ShowWindow(SW_SHOW);
	m_frame->UpdateWindow();


	//get the rect from the MFC window so we can get its dimensions
	m_toolHandle = m_frame->m_DirXView.GetSafeHwnd();				//handle of directX child window
	m_frame->m_DirXView.GetClientRect(&WindowRECT);
	m_width		= WindowRECT.Width();
	m_height	= WindowRECT.Height();

	m_ToolSystem.onActionInitialise(m_toolHandle, m_width, m_height);

	return TRUE;
}

int MFCMain::Run()
{
	MSG msg;
	BOOL bGotMsg;

	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (true)
		{
			bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
		}
		else
		{
			bGotMsg = (GetMessage(&msg, NULL, 0U, 0U) != 0);
		}

		if (bGotMsg)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			m_ToolSystem.UpdateInput(&msg);
		}
		else
		{	
			int size = m_ToolSystem.getCurrentSelectionIDs().size();
			std::wstring statusString = L"Selected Objects: " + std::to_wstring(size);
			m_ToolSystem.Tick(&msg);

			//send current object ID to status bar in The main frame
			m_frame->m_wndStatusBar.SetPaneText(1, statusString.c_str(), 1);
		}
	}

	return (int)msg.wParam;
}

void MFCMain::MenuFileQuit()
{
	//will post message to the message thread that will exit the application normally
	PostQuitMessage(0);
}

void MFCMain::MenuFileSaveTerrain()
{
	m_ToolSystem.onActionSaveTerrain();
}

void MFCMain::MenuEditSelect()
{
	//SelectDialogue m_ToolSelectDialogue(NULL, &m_ToolSystem.m_sceneGraph);		//create our dialoguebox //modal constructor
	//m_ToolSelectDialogue.DoModal();	// start it up modal

	//modeless dialogue must be declared in the class.   If we do local it will go out of scope instantly and destroy itself
	m_ToolSelectDialogue.Create(IDD_DIALOG1);	//Start up modeless
	m_ToolSelectDialogue.ShowWindow(SW_SHOW);	//show modeless

	if (m_ToolSystem.getCurrentSelectionIDs().size() > 0)
	{
		int selection = m_ToolSystem.getCurrentSelectionIDs()[0];
		m_ToolSelectDialogue.SetObjectData(&m_ToolSystem.m_sceneGraph, &selection);
	}
}

void MFCMain::ToolBarButton1()
{
	m_ToolSystem.onActionSave();
}

void MFCMain::UndoButton()
{
	m_ToolSystem.onUndoButton();
}

void MFCMain::RedoButton()
{
	m_ToolSystem.onRedoButton();
}

void MFCMain::TransformButton()
{
	if (m_ToolSystem.getCurrentSelectionIDs().size() == 0)
	{
		return;
	}
	
	m_transformDialog.Create(IDD_TRANSFORM);
	m_transformDialog.ShowWindow(SW_SHOW);

	SceneObject* sceneObject = m_ToolSystem.GetSelectedObject();
	m_transformDialog.SetObjectData(&m_ToolSystem, sceneObject->posX, sceneObject->posY, sceneObject->posZ,
									sceneObject->rotX, sceneObject->rotY, sceneObject->rotZ, 
									sceneObject->scaX, sceneObject->scaY, sceneObject->scaZ);
}

void MFCMain::ToggleTerrainPainting()
{
	m_ToolSystem.onToggleTerrainPainting();
}

void MFCMain::IncreaseTerrainBrushSize()
{
	m_ToolSystem.addToTerrainBrushSize(1.f);
}

void MFCMain::DecreaseTerrainBrushSize()
{
	m_ToolSystem.addToTerrainBrushSize(-1.f);
}

void MFCMain::ToggleSplineEditing()
{
	m_ToolSystem.onToggleSplineEditing();
}

void MFCMain::IncreaseSplineQuality()
{
	m_ToolSystem.addToSplineQuality(1.f);
}

void MFCMain::DecreaseSplineQuality()
{
	m_ToolSystem.addToSplineQuality(-1.f);
}

void MFCMain::ToggleAnimateMinecart()
{
	m_ToolSystem.onToggleAnimateMinecart();
}

MFCMain::MFCMain()
{
}


MFCMain::~MFCMain()
{
}
