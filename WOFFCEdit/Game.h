//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "SceneObject.h"
#include "DisplayObject.h"
#include "DisplayChunk.h"
#include "ChunkObject.h"
#include "InputCommands.h"
#include <vector>
#include <memory>
#include "Camera.h"

class Command;

// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game : public DX::IDeviceNotify
{
public:

	Game();
	~Game();

	// Initialization and management
	void Initialize(HWND window, int width, int height);
	void SetGridState(bool state);

	// Basic game loop
	void Tick(InputCommands * Input);
	void Render();

	// Rendering helpers
	void Clear();

	std::shared_ptr<DX::DeviceResources> GetDeviceResources();

	// IDeviceNotify
	virtual void OnDeviceLost() override;
	virtual void OnDeviceRestored() override;

	void OnDialogOpened();
	void OnDialogClosed();

	// Messages
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();
	void OnWindowSizeChanged(int width, int height);

	//tool specific
	void BuildDisplayList(std::vector<SceneObject> * SceneGraph); //note vector passed by reference 
	void BuildDisplayChunk(ChunkObject *SceneChunk);
	void SaveDisplayChunk(ChunkObject *SceneChunk);	//saves geometry et al
	void ClearDisplayList();

	void SetPickedObjectsVector(std::vector<int> newPickedObjects);
	void SetPickedObject(int id);
	void AddPickedObject(int id);
	void RemovePickedObject(int id);
	void SetTerrainHeightMap(const std::vector<BYTE>& newHeightMap);
	void ToggleTerrainPainting();
	
	//template <typename T = Command, typename ...Args>
	void ExecuteCommand(std::shared_ptr<Command> command);
	
	void UndoCommand();
	void RedoCommand();

	const std::vector<int>& GetPickedObjects();
	std::vector<int> GetPickedObjectsCopy() const;

#ifdef DXTK_AUDIO
	void NewAudioDevice();
#endif

private:

	void Update(DX::StepTimer const& timer);
	void UpdateHotkeys();
	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	int PickObjectUnderMouse();
	void HandleObjectPicking(int selected);
	void PickTerrainAndModify(bool modify);
	void UpdateTerrainDebugCircle();

	void DrawImGui();
	void DrawHierarchy();

	void XM_CALLCONV DrawGrid(DirectX::FXMVECTOR xAxis, DirectX::FXMVECTOR yAxis, DirectX::FXMVECTOR origin, size_t xdivs, size_t ydivs, DirectX::GXMVECTOR color);

	//tool specific
	std::vector<DisplayObject>			m_displayList;
	DisplayChunk						m_displayChunk;
	InputCommands*						m_InputCommands;

	//control variables
	bool m_grid;							//grid rendering on / off
	// Device resources.
    std::shared_ptr<DX::DeviceResources>    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                           m_timer;

	std::vector<std::shared_ptr<Command>>	m_undoStack;
	std::vector<std::shared_ptr<Command>>	m_redoStack;

    // Input devices.
    std::unique_ptr<DirectX::GamePad>       m_gamePad;
    std::unique_ptr<DirectX::Keyboard>      m_keyboard;
    std::unique_ptr<DirectX::Mouse>         m_mouse;
	std::unique_ptr<Camera>					m_camera;

	std::vector<int>						m_pickedObjects;
	Vector3									m_lastMouse;
	HWND									m_hwnd;
	HCURSOR									m_cursor;

	float m_transformDragStep = 1.f;

    // DirectXTK objects.
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
    std::unique_ptr<DirectX::EffectFactory>                                 m_fxFactory;
    std::unique_ptr<DirectX::GeometricPrimitive>                            m_shape;
    std::unique_ptr<DirectX::Model>                                         m_model;
    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;
    std::unique_ptr<DirectX::SpriteFont>                                    m_font;

#ifdef DXTK_AUDIO
    std::unique_ptr<DirectX::AudioEngine>                                   m_audEngine;
    std::unique_ptr<DirectX::WaveBank>                                      m_waveBank;
    std::unique_ptr<DirectX::SoundEffect>                                   m_soundEffect;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect1;
    std::unique_ptr<DirectX::SoundEffectInstance>                           m_effect2;
#endif

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_texture2;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
	
	bool m_lmbDownLastFrame = false;
	bool m_rmbDownLastFrame = false;
	bool m_syncScale = false;
	bool m_wasZDown = false;
	bool m_wasYDown = false;
	bool m_zReleased = false;
	bool m_yReleased = false;
	bool m_tDowLastFrame = false;
	bool m_dialogHovered = false;
	bool m_editingTerrain = true;

	/** Terrain Debug Circle */
	Vector3 m_currentTerrainHit;
	float m_terrainEditRadius = 15.f;
	bool m_isHittingTerrain;
	std::vector<DirectX::VertexPositionColor> m_debugCircleVertices;

	std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>> m_debugBatch;
	std::unique_ptr<DirectX::BasicEffect> m_debugEffect;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_debugInputLayout;
	std::vector<BYTE> m_oldTerrainData;
	/** End Terrain Debug Circle */

#ifdef DXTK_AUDIO
    uint32_t                                                                m_audioEvent;
    float                                                                   m_audioTimerAcc;

    bool                                                                    m_retryDefault;
#endif

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_projection;


};

std::wstring StringToWCHART(std::string s);