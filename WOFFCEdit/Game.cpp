//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "DisplayObject.h"
#include <sstream>
#include <iomanip>
#include <string>
#include "vendor/imgui/imgui.h"
#include "vendor/imgui/backends/imgui_impl_win32.h"
#include "vendor/imgui/backends/imgui_impl_dx11.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

using Microsoft::WRL::ComPtr;

Game::Game()

{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
	m_displayList.clear();
    m_camera = std::make_unique<Camera>();
	
	//initial Settings
	//modes
    m_lmbDownLastFrame = false;
    m_rmbDownLastFrame = false;
	m_grid = false;
}

Game::~Game()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
#ifdef DXTK_AUDIO
    if (m_audEngine)
    {
        m_audEngine->Suspend();
    }
#endif
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_gamePad = std::make_unique<GamePad>();

    m_keyboard = std::make_unique<Keyboard>();

    m_mouse = std::make_unique<Mouse>();
    m_mouse->SetWindow(window);

    m_deviceResources->SetWindow(window, width, height);

    m_hwnd = window;
    m_cursor = LoadCursor(NULL, IDC_ARROW);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

#ifdef DXTK_AUDIO
    // Create DirectXTK for Audio objects
    AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
#ifdef _DEBUG
    eflags = eflags | AudioEngine_Debug;
#endif

    m_audEngine = std::make_unique<AudioEngine>(eflags);

    m_audioEvent = 0;
    m_audioTimerAcc = 10.f;
    m_retryDefault = false;

    m_waveBank = std::make_unique<WaveBank>(m_audEngine.get(), L"adpcmdroid.xwb");

    m_soundEffect = std::make_unique<SoundEffect>(m_audEngine.get(), L"MusicMono_adpcm.wav");
    m_effect1 = m_soundEffect->CreateInstance();
    m_effect2 = m_waveBank->CreateInstance(10);

    m_effect1->Play(true);
    m_effect2->Play();
#endif
}

void Game::SetGridState(bool state)
{
	m_grid = state;
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick(InputCommands *Input)
{
    // (Your code process and dispatch Win32 messages)
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

	//copy over the input commands so we have a local version to use elsewhere.
	m_InputCommands = *Input;
    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

#ifdef DXTK_AUDIO
    // Only update audio engine once per frame
    if (!m_audEngine->IsCriticalError() && m_audEngine->Update())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
#endif

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
	//TODO  any more complex than this, and the camera should be abstracted out to somewhere else
	//camera motion is on a plane, so kill the 7 component of the look direction
    Mouse::State mouseState = m_mouse->GetState();

    if (mouseState.rightButton)
    {
        RECT rect;
        GetWindowRect(m_hwnd, &rect);
        float middleX = (rect.right - rect.left) / 2.f + rect.left;
        float middleY = (rect.bottom - rect.top) / 2.f + rect.top;

        if (!m_rmbDownLastFrame)
        {
            SetCursor(NULL);
            SetCursorPos(middleX, middleY);
        }

        POINT point;
        GetCursorPos(&point);
        Vector3 mouseDelta = Vector3::Zero;
        if (m_rmbDownLastFrame)
        {
            float diffX = point.x - middleX;
            float diffY = point.y - middleY;
            mouseDelta = Vector3(diffX, diffY, 0.f);
        }
        SetCursorPos(middleX, middleY);
        m_camera->AddMouseInput(mouseDelta);
    }
    else
    {
        if (m_rmbDownLastFrame)
        {
            SetCursor(m_cursor);
        }

        if (m_InputCommands.rotRight)
        {
            m_camera->AddPitchInput(1.f);
        }
        if (m_InputCommands.rotLeft)
        {
            m_camera->AddPitchInput(-1.f);
        }
        if (m_InputCommands.lookUp)
        {
            m_camera->AddYawInput(1.f);
        }
        if (m_InputCommands.lookDown)
        {
            m_camera->AddYawInput(-1.f);
        }
    }
    if (m_InputCommands.forward)
    {
        m_camera->AddMovementInput(Vector3(1.f, 0.f, 0.f));
    }
    if (m_InputCommands.back)
    {
        m_camera->AddMovementInput(Vector3(-1.f, 0.f, 0.f));
    }
    if (m_InputCommands.right)
    {
        m_camera->AddMovementInput(Vector3(0.f, 0.f, 1.f));
    }
    if (m_InputCommands.left)
    {
        m_camera->AddMovementInput(Vector3(0.f, 0.f, -1.f));
    }
    if (m_InputCommands.up)
    {
        m_camera->AddMovementInput(Vector3(0.f, 1.f, 0.f));
    }
    if (m_InputCommands.down)
    {
        m_camera->AddMovementInput(Vector3(0.f, -1.f, 0.f));
    }

    bool wasLMBReleased = m_lmbDownLastFrame == true && mouseState.leftButton == false;
    if (wasLMBReleased && !ImGui::IsAnyItemHovered())
    {
        int selected = PickObjectUnderMouse();
        HandleObjectPicking(selected);
    }

    m_camera->Update();
    m_batchEffect->SetView(m_camera->GetViewMatrix());
    m_batchEffect->SetWorld(Matrix::Identity);
	m_displayChunk.m_terrainEffect->SetView(m_camera->GetViewMatrix());
	m_displayChunk.m_terrainEffect->SetWorld(Matrix::Identity);

    m_rmbDownLastFrame = mouseState.rightButton;
    m_lmbDownLastFrame = mouseState.leftButton;

    m_lastMouse = Vector3(mouseState.x, mouseState.y, 0.f);

#ifdef DXTK_AUDIO
    m_audioTimerAcc -= (float)timer.GetElapsedSeconds();
    if (m_audioTimerAcc < 0)
    {
        if (m_retryDefault)
        {
            m_retryDefault = false;
            if (m_audEngine->Reset())
            {
                // Restart looping audio
                m_effect1->Play(true);
            }
        }
        else
        {
            m_audioTimerAcc = 4.f;

            m_waveBank->Play(m_audioEvent++);

            if (m_audioEvent >= 11)
                m_audioEvent = 0;
        }
    }
#endif

   
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();

	if (m_grid)
	{
		// Draw procedurally generated dynamic grid
		const XMVECTORF32 xaxis = { 512.f, 0.f, 0.f };
		const XMVECTORF32 yaxis = { 0.f, 0.f, 512.f };
		DrawGrid(xaxis, yaxis, g_XMZero, 512, 512, Colors::Gray);
	}

	//RENDER OBJECTS FROM SCENEGRAPH
	int numRenderObjects = m_displayList.size();
	for (int i = 0; i < numRenderObjects; i++)
	{
		m_deviceResources->PIXBeginEvent(L"Draw model");
		const XMVECTORF32 scale = { m_displayList[i].m_scale.x, m_displayList[i].m_scale.y, m_displayList[i].m_scale.z };
		const XMVECTORF32 translate = { m_displayList[i].m_position.x, m_displayList[i].m_position.y, m_displayList[i].m_position.z };

		//convert degrees into radians for rotation matrix
		XMVECTOR rotate = Quaternion::CreateFromYawPitchRoll(m_displayList[i].m_orientation.y *3.1415 / 180,
															m_displayList[i].m_orientation.x *3.1415 / 180,
															m_displayList[i].m_orientation.z *3.1415 / 180);

		XMMATRIX local = m_world * XMMatrixTransformation(g_XMZero, Quaternion::Identity, scale, g_XMZero, rotate, translate);

		m_displayList[i].m_model->Draw(context, *m_states, local, m_camera->GetViewMatrix(), m_projection, false);	//last variable in draw,  make TRUE for wireframe

		m_deviceResources->PIXEndEvent();
	}
    m_deviceResources->PIXEndEvent();

	//RENDER TERRAIN
	context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
	context->OMSetDepthStencilState(m_states->DepthDefault(),0);
	context->RSSetState(m_states->CullNone());
//	context->RSSetState(m_states->Wireframe());		//uncomment for wireframe

	//Render the batch,  This is handled in the Display chunk becuase it has the potential to get complex
	m_displayChunk.RenderBatch(m_deviceResources);

    DirectX::Mouse::State mouseState = m_mouse->GetState();
    //CAMERA POSITION ON HUD
    m_sprites->Begin();
    WCHAR   Buffer[256];
    
    std::stringstream stream;
    stream << "Cam X: " << std::fixed << std::setprecision(2) << m_camera->GetCameraPosition().x;
    stream << " Cam Z: " << std::fixed << std::setprecision(2) << m_camera->GetCameraPosition().z;
    
    std::string streamed = stream.str();
    std::wstring var(streamed.begin(), streamed.end());
    m_font->DrawString(m_sprites.get(), var.c_str(), XMFLOAT2(100, 10), Colors::Yellow);
    std::wstring mouse = L"Mouse X: " + std::to_wstring(mouseState.x) + L" Mouse Y: " + std::to_wstring(mouseState.y);
    m_font->DrawString(m_sprites.get(), mouse.c_str(), XMFLOAT2(100, 35), Colors::Yellow);

    m_sprites->End();
    DrawImGui();
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetBackBufferRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}

std::shared_ptr<DX::DeviceResources> Game::GetDeviceResources()
{
    return m_deviceResources;
}

void XM_CALLCONV Game::DrawGrid(FXMVECTOR xAxis, FXMVECTOR yAxis, FXMVECTOR origin, size_t xdivs, size_t ydivs, GXMVECTOR color)
{
    m_deviceResources->PIXBeginEvent(L"Draw grid");

    auto context = m_deviceResources->GetD3DDeviceContext();
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthNone(), 0);
    context->RSSetState(m_states->CullCounterClockwise());

    m_batchEffect->Apply(context);

    context->IASetInputLayout(m_batchInputLayout.Get());

    m_batch->Begin();

    xdivs = std::max<size_t>(1, xdivs);
    ydivs = std::max<size_t>(1, ydivs);

    for (size_t i = 0; i <= xdivs; ++i)
    {
        float fPercent = float(i) / float(xdivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(xAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, yAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, yAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    for (size_t i = 0; i <= ydivs; i++)
    {
        float fPercent = float(i) / float(ydivs);
        fPercent = (fPercent * 2.0f) - 1.0f;
        XMVECTOR vScale = XMVectorScale(yAxis, fPercent);
        vScale = XMVectorAdd(vScale, origin);

        VertexPositionColor v1(XMVectorSubtract(vScale, xAxis), color);
        VertexPositionColor v2(XMVectorAdd(vScale, xAxis), color);
        m_batch->DrawLine(v1, v2);
    }

    m_batch->End();

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
}

void Game::OnDeactivated()
{
}

void Game::OnSuspending()
{
#ifdef DXTK_AUDIO
    m_audEngine->Suspend();
#endif
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

#ifdef DXTK_AUDIO
    m_audEngine->Resume();
#endif
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();
}

void Game::BuildDisplayList(std::vector<SceneObject> * SceneGraph)
{
	auto device = m_deviceResources->GetD3DDevice();
	auto devicecontext = m_deviceResources->GetD3DDeviceContext();

	if (!m_displayList.empty())		//is the vector empty
	{
		m_displayList.clear();		//if not, empty it
	}

	//for every item in the scenegraph
	int numObjects = SceneGraph->size();
	for (int i = 0; i < numObjects; i++)
	{
		
		//create a temp display object that we will populate then append to the display list.
		DisplayObject newDisplayObject;
		
		//load model
		std::wstring modelwstr = StringToWCHART(SceneGraph->at(i).model_path);							//convect string to Wchar
		newDisplayObject.m_model = Model::CreateFromCMO(device, modelwstr.c_str(), *m_fxFactory, true);	//get DXSDK to load model "False" for LH coordinate system (maya)

		//Load Texture
		std::wstring texturewstr = StringToWCHART(SceneGraph->at(i).tex_diffuse_path);								//convect string to Wchar
		HRESULT rs;
		rs = CreateDDSTextureFromFile(device, texturewstr.c_str(), nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource

		//if texture fails.  load error default
		if (rs)
		{
			CreateDDSTextureFromFile(device, L"database/data/Error.dds", nullptr, &newDisplayObject.m_texture_diffuse);	//load tex into Shader resource
		}

		//apply new texture to models effect
		newDisplayObject.m_model->UpdateEffects([&](IEffect* effect) //This uses a Lambda function,  if you dont understand it: Look it up.
		{	
			auto lights = dynamic_cast<BasicEffect*>(effect);
			if (lights)
			{
				lights->SetTexture(newDisplayObject.m_texture_diffuse);			
			}
		});

		//set position
		newDisplayObject.m_position.x = SceneGraph->at(i).posX;
		newDisplayObject.m_position.y = SceneGraph->at(i).posY;
		newDisplayObject.m_position.z = SceneGraph->at(i).posZ;
		
		//setorientation
		newDisplayObject.m_orientation.x = SceneGraph->at(i).rotX;
		newDisplayObject.m_orientation.y = SceneGraph->at(i).rotY;
		newDisplayObject.m_orientation.z = SceneGraph->at(i).rotZ;

		//set scale
		newDisplayObject.m_scale.x = SceneGraph->at(i).scaX;
		newDisplayObject.m_scale.y = SceneGraph->at(i).scaY;
		newDisplayObject.m_scale.z = SceneGraph->at(i).scaZ;

		//set wireframe / render flags
		newDisplayObject.m_render		= SceneGraph->at(i).editor_render;
		newDisplayObject.m_wireframe	= SceneGraph->at(i).editor_wireframe;

		newDisplayObject.m_light_type		= SceneGraph->at(i).light_type;
		newDisplayObject.m_light_diffuse_r	= SceneGraph->at(i).light_diffuse_r;
		newDisplayObject.m_light_diffuse_g	= SceneGraph->at(i).light_diffuse_g;
		newDisplayObject.m_light_diffuse_b	= SceneGraph->at(i).light_diffuse_b;
		newDisplayObject.m_light_specular_r = SceneGraph->at(i).light_specular_r;
		newDisplayObject.m_light_specular_g = SceneGraph->at(i).light_specular_g;
		newDisplayObject.m_light_specular_b = SceneGraph->at(i).light_specular_b;
		newDisplayObject.m_light_spot_cutoff = SceneGraph->at(i).light_spot_cutoff;
		newDisplayObject.m_light_constant	= SceneGraph->at(i).light_constant;
		newDisplayObject.m_light_linear		= SceneGraph->at(i).light_linear;
		newDisplayObject.m_light_quadratic	= SceneGraph->at(i).light_quadratic;
		
		m_displayList.push_back(newDisplayObject);
		
	}
		
		
		
}

void Game::BuildDisplayChunk(ChunkObject * SceneChunk)
{
	//populate our local DISPLAYCHUNK with all the chunk info we need from the object stored in toolmain
	//which, to be honest, is almost all of it. Its mostly rendering related info so...
	m_displayChunk.PopulateChunkData(SceneChunk);		//migrate chunk data
	m_displayChunk.LoadHeightMap(m_deviceResources);
	m_displayChunk.m_terrainEffect->SetProjection(m_projection);
	m_displayChunk.InitialiseBatch();
}

void Game::SaveDisplayChunk(ChunkObject * SceneChunk)
{
	m_displayChunk.SaveHeightMap();			//save heightmap to file.
}

const std::vector<int>& Game::GetPickedObjects()
{
    return m_pickedObjects;
}

#ifdef DXTK_AUDIO
void Game::NewAudioDevice()
{
    if (m_audEngine && !m_audEngine->IsAudioDevicePresent())
    {
        // Setup a retry in 1 second
        m_audioTimerAcc = 1.f;
        m_retryDefault = true;
    }
}
#endif


#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();

    m_states = std::make_unique<CommonStates>(device);

    m_fxFactory = std::make_unique<EffectFactory>(device);
	m_fxFactory->SetDirectory(L"database/data/"); //fx Factory will look in the database directory
	m_fxFactory->SetSharing(false);	//we must set this to false otherwise it will share effects based on the initial tex loaded (When the model loads) rather than what we will change them to.

    m_sprites = std::make_unique<SpriteBatch>(context);

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);

    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_font = std::make_unique<SpriteFont>(device, L"SegoeUI_18.spritefont");

//    m_shape = GeometricPrimitive::CreateTeapot(context, 4.f, 8);

    // SDKMESH has to use clockwise winding with right-handed coordinates, so textures are flipped in U
    m_model = Model::CreateFromSDKMESH(device, L"tiny.sdkmesh", *m_fxFactory);
	

    // Load textures
    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"seafloor.dds", nullptr, m_texture1.ReleaseAndGetAddressOf())
    );

    DX::ThrowIfFailed(
        CreateDDSTextureFromFile(device, L"windowslogo.dds", nullptr, m_texture2.ReleaseAndGetAddressOf())
    );

}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_projection = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        1000.0f
    );

    m_batchEffect->SetProjection(m_projection);
	
}

int Game::PickObjectUnderMouse()
{
    int selectedID = -1;
    float pickedDistance = 9999999.f;

    const RECT sreenDimensions = m_deviceResources->GetOutputSize();
    const DirectX::Mouse::State state = m_mouse->GetState();
    const XMVECTOR nearSource = XMVectorSet(state.x, state.y, 0.f, 1.f);
    const XMVECTOR farSource = XMVectorSet(state.x, state.y, 1.f, 1.f);

    for (int i = 0; i < m_displayList.size(); ++i)
    {
        const XMVECTORF32 scale = 
        { 
            m_displayList[i].m_scale.x,
            m_displayList[i].m_scale.y, 
            m_displayList[i].m_scale.z 
        };
        const XMVECTORF32 translation =
        {
            m_displayList[i].m_position.x,
            m_displayList[i].m_position.y,
            m_displayList[i].m_position.z
        };

        const XMVECTOR rotation = Quaternion::CreateFromYawPitchRoll(
            m_displayList[i].m_orientation.y * 3.1415 / 180,
            m_displayList[i].m_orientation.x * 3.1415 / 180,
            m_displayList[i].m_orientation.z * 3.1415 / 180
        );

        const XMMATRIX worldSpace = m_world * XMMatrixTransformation(
            g_XMZero, Quaternion::Identity, scale, g_XMZero,
            rotation, translation
        );
 
        const XMVECTOR nearPoint = XMVector3Unproject(
            nearSource, 0.f, 0.f, sreenDimensions.right, sreenDimensions.bottom,
            m_deviceResources->GetScreenViewport().MinDepth,
            m_deviceResources->GetScreenViewport().MaxDepth,
            m_projection, m_camera->GetViewMatrix(), worldSpace
        );

        const XMVECTOR farPoint = XMVector3Unproject(
            farSource, 0.f, 0.f, sreenDimensions.right, sreenDimensions.bottom,
            m_deviceResources->GetScreenViewport().MinDepth,
            m_deviceResources->GetScreenViewport().MaxDepth,
            m_projection, m_camera->GetViewMatrix(), worldSpace
        );

        const XMVECTOR pickingVector = XMVector3Normalize(farPoint - nearPoint);
        
        for (int y = 0; y < m_displayList[i].m_model.get()->meshes.size(); ++y)
        {
            float currentPickedDistance = 999999.f;
            if (m_displayList[i].m_model.get()->meshes[y]->boundingBox.Intersects(
                nearPoint, pickingVector, currentPickedDistance))
            {
                if (currentPickedDistance < pickedDistance)
                {
                    selectedID = i;
                }
            }
        }
    }

    return selectedID;
}

void Game::HandleObjectPicking(int selected)
{
    if (m_InputCommands.shiftDown)
    {
        if (selected == -1)
        {
            return;
        }

        // Unselect object if it is already in the array
        if (std::find(m_pickedObjects.begin(), m_pickedObjects.end(), selected) != m_pickedObjects.end())
        {
            m_pickedObjects.erase(std::remove(m_pickedObjects.begin(), m_pickedObjects.end(), selected), m_pickedObjects.end());
        }
        else
        {
            m_pickedObjects.push_back(selected);
        }
    }
    else
    {
        if (selected == -1)
        {
            m_pickedObjects.clear();
            return;
        }

        if (m_pickedObjects.size() > 1)
        {
            m_pickedObjects.clear();
        }

        if (std::find(m_pickedObjects.begin(), m_pickedObjects.end(), selected) != m_pickedObjects.end())
        {
            m_pickedObjects.clear();
        }
        else
        {
            m_pickedObjects.clear();
            m_pickedObjects.push_back(selected);
        }

    }
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_sprites.reset();
    m_batch.reset();
    m_batchEffect.reset();
    m_font.reset();
    m_shape.reset();
    m_model.reset();
    m_texture1.Reset();
    m_texture2.Reset();
    m_batchInputLayout.Reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}
#pragma endregion

std::wstring StringToWCHART(std::string s)
{

	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

void Game::DrawImGui()
{
    if (!ImGui::Begin("World Outliner"))
    {
        ImGui::End();
    }
    else
    {
        DrawHierarchy();
        ImGui::End();
    }


    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Game::DrawHierarchy()
{
    ImVec2 contentRegion = ImGui::GetContentRegionAvail();
    contentRegion.y /= 1.5f;
    ImGui::BeginChild("Hierarchy", contentRegion);
    if (ImGui::CollapsingHeader("Hierarchy")) 
    {
        ImGui::SetWindowFontScale(1.2f);
        ImVec2 buttonSize = ImGui::GetContentRegionAvail();
        buttonSize.x -= 12.f;
        buttonSize.y = 18.f;
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.f, 0.f, 0.f, 1.f));
        for (int i = 0; i < m_displayList.size(); ++i)
        {
            std::string text = std::to_string(i);
            ImVec4 color = ImVec4(1.f ,1.f, 1.f, 1.f);
            if (std::find(m_pickedObjects.begin(), m_pickedObjects.end(), i) != m_pickedObjects.end())
            {
                color = ImVec4(0.47f, 0.67f, 0.97f, 1.f);
            }
            ImGui::PushStyleColor(ImGuiCol_Button, color);
            if (ImGui::Button(text.c_str(), buttonSize))
            {
                HandleObjectPicking(i);
            }
            ImGui::PopStyleColor();
        }
        ImGui::SetWindowFontScale(1.f);
        ImGui::PopStyleColor();
    }
    ImGui::EndChild();


    ImGui::BeginChild("Transform");
    if (ImGui::CollapsingHeader("Transform"))
    {
        if (m_pickedObjects.size() == 0)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 0.f, 0.f, 1.f));
            ImGui::SetWindowFontScale(1.5f);
            ImGui::Text("Please select an object");
            ImGui::SetWindowFontScale(1.f);
            ImGui::PopStyleColor();
        }
        else
        {
            int index = m_pickedObjects[m_pickedObjects.size() - 1];

            ImGui::SeparatorText("Translation:");
            ImGui::PushID("Translation");
            ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.1);
            ImGui::DragFloat("##X", &m_displayList[index].m_position.x, m_transformDragStep, 0.f, 0.f, "X: %.2f");
            ImGui::SameLine(); ImGui::DragFloat("##Y", &m_displayList[index].m_position.y, 1.f, 0.f, 0.f, "Y: %.2f");
            ImGui::SameLine(); ImGui::DragFloat("##Z", &m_displayList[index].m_position.z, 1.f, 0.f, 0.f, "Z: %.2f");
            ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::SeparatorText("Rotation:");
            ImGui::PushID("Rotation");
            ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.1);
            ImGui::DragFloat("##X", &m_displayList[index].m_orientation.x, m_transformDragStep, 0.f, 0.f, "X: %.2f");
            ImGui::SameLine(); ImGui::DragFloat("##Y", &m_displayList[index].m_orientation.y, 1.f, 0.f, 0.f, "Y: %.2f");
            ImGui::SameLine(); ImGui::DragFloat("##Z", &m_displayList[index].m_orientation.z, 1.f, 0.f, 0.f, "Z: %.2f");
            ImGui::PopItemWidth();
            ImGui::PopID();

            ImGui::SeparatorText("Scale:");
            ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.1);
            ImGui::DragFloat("##X", &m_displayList[index].m_scale.x, m_transformDragStep, 0.f, 0.f, "X: %.2f");
            ImGui::SameLine(); ImGui::DragFloat("##Y", &m_displayList[index].m_scale.y, 1.f, 0.f, 0.f, "Y: %.2f");
            ImGui::SameLine(); ImGui::DragFloat("##Z", &m_displayList[index].m_scale.z, 1.f, 0.f, 0.f, "Z: %.2f");
            ImGui::PopItemWidth();

            ImGui::Text("Step: ");
            ImGui::SameLine(); ImGui::DragInt("##S", &m_transformDragStep, 1.0, 1, 10);
        }
    }

    ImGui::EndChild();
}
