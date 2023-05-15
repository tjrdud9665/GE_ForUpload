#pragma once

struct SEditionalRender
{
	std::function<void(void)>	PreRenderFunc;
	std::function<void(void)>	RenderFunc;
};


class Engine : public XObject
{
	using EngineObjectMap = std::unordered_map<type_index, XObject*>;
	using ExternalEngineObjectRenderMap = std::unordered_map<type_index, SEditionalRender>;

public:
	Engine();
	~Engine();

public:

	virtual XObject* GetObject(type_index TypeIdx) const;

	/* Return Old Object */
	virtual XObject* SetObject(type_index TypeIdx, XObject* New);

	/* generic version of GetObject. */
	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	inline Type* Get() const
	{
		return (Type*)(GetObject(type_index(typeid(Type))));
	}

	/* generic version of SetObject. */
	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	inline Type* Set(Type* Object)
	{
		return (Type*)SetObject(type_index(typeid(Type)), Object);
	}

	void AddRenderCall(type_index TypeIdx, std::function<void(void)>&& PreFunc , std::function<void(void)>&& Func);

	template<typename Type, typename = std::enable_if_t<std::is_base_of<XObject, Type>::value>>
	inline void AddRenderCall(Type* Object, std::function<void(void)>&& PreFunc, std::function<void(void)>&& Func)
	{
		AddRenderCall(type_index(typeid(Type)), std::move(PreFunc), std::move(Func));
	}


public:
	void Initialize(const wstring& InTitle, const int32 WndWidth, const int32 WndHeight, const int32 dxWidth, const int32 dxHeight, bool FullScreen = false);

	void Update();

	void ShutDown();

	bool Run(class Application* app);

	static bool IsEditMode() { return m_EditMode; }

	static void SetEditMode(bool editMode)
	{
		m_EditMode = editMode;
	}

	D3D11_VIEWPORT GetEditViewport() const { return m_EditViewport; }

	void SetEditViewport(const D3D11_VIEWPORT& viewport)
	{
		m_EditViewport = viewport;
	}

	//For call IMGUI Editor Only
	bool OnMessageHandle(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	class Application* GetApplication() const;



private:
	EngineObjectMap	m_EngineObjects;

	void PreRender();

	void Render();


private:
	bool LoadShaders();

	void LoadBuiltinAssets();


private:
	bool				m_Loop;

	static bool			m_EditMode;

	bool				m_FirstStep;


private:
	class Application* m_App;
	//Win
	class EngineWindow*		m_EngineWindow;

	//Rendering
	class D3DManager*		m_D3DManager;

	class Renderer*			m_Renderer;

	//Utils
	class PathManager*		m_PathManger;

	class TimerManager*		m_TimerManager;

	class AssetManager*		m_AssetManager;

	class SoundManager*		m_SoundManager;


	//GameFrm
	class SceneManager*		m_SceneManager;

	class RenderManager*	m_RenderManager;

	class UIManager*		m_UIManager;

	//PhySys

	class PhysicsSystem* m_PhySys;
	class CollisionManager* m_CollisionManager;

	class NavigationSystem* m_NavSystem;

	class Input* m_Input;

	class GameInstance* m_GameInst;

private:
	std::vector<FShaderID>			m_BuiltinShaders;
	FShaderID						m_SelectedShader;	

	ExternalEngineObjectRenderMap	m_EditionalRender;

	D3D11_VIEWPORT					m_EditViewport;
	




	

};

