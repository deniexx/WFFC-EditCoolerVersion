#pragma once

#include <afxext.h>
#include "pch.h"
#include "Game.h"
#include "sqlite3.h"
#include "SceneObject.h"
#include "InputCommands.h"
#include <vector>

using namespace DirectX::SimpleMath;
static class ToolMain* g_ToolMain;

class ToolMain
{
public: //methods
	ToolMain();
	~ToolMain();

	//onAction - These are the interface to MFC
	const std::vector<int>& getCurrentSelectionIDs();										//returns the selection number of currently selected object so that It can be displayed.
	void	onActionInitialise(HWND handle, int width, int height);			//Passes through handle and hieght and width and initialises DirectX renderer and SQL LITE
	void	onActionFocusCamera();
	void	onActionLoad();													//load the current chunk
	afx_msg	void	onActionSave();											//save the current chunk
	afx_msg void	onActionSaveTerrain();									//save chunk geometry

	afx_msg void onUndoButton();
	afx_msg void onRedoButton();
	afx_msg void onToggleTerrainPainting();
	afx_msg void onToggleSplineEditing();
	afx_msg void addToTerrainBrushSize(float delta);
	afx_msg void addToSplineQuality(float delta);

	void TranslateSelected(float x, float y, float z);
	void RotateSelected(float x, float y, float z);
	void ScaleSelected(float x, float y, float z);

	void UpdateTransformDialog();
	void OnDialogHovered();
	void OnDialogMouseLeave();
	void SetTransformOnSelected(Vector3 pos, Vector3 rot, Vector3 sca);

	void	Tick(MSG *msg);
	void	UpdateInput(MSG *msg);

	SceneObject* GetSelectedObject();

public:	//variables
	std::vector<SceneObject>    m_sceneGraph;	//our scenegraph storing all the objects in the current chunk
	ChunkObject					m_chunk;		//our landscape chunk

private:	//methods
	void	onContentAdded();


		
private:	//variables
	HWND	m_toolHandle;		//Handle to the  window
	Game	m_d3dRenderer;		//Instance of D3D rendering system for our tool
	InputCommands m_toolInputCommands;		//input commands that we want to use and possibly pass over to the renderer
	CRect	WindowRECT;		//Window area rectangle. 
	char	m_keyArray[256];
	sqlite3 *m_databaseConnection;	//sqldatabase handle
	
	int m_width;		//dimensions passed to directX
	int m_height;
	int m_currentChunk;			//the current chunk of thedatabase that we are operating on.  Dictates loading and saving. 
	

	
};
