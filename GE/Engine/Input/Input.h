#pragma once



#define DIK_MOUSELBUTTON    0xfc
#define DIK_MOUSERBUTTON    0xfd
#define DIK_MOUSEWHEEL      0xfe

enum class EInputType
{
	Direct,
	Window
};


enum class EInputEventType
{
	IE_Pressed = 0,
	IE_Released,
	Max,
};

struct SKeyActionEvent
{	
	std::vector<std::function<void(const std::string&,uint64)>> onPressed;
	std::vector<std::function<void(const std::string&,uint64)>> onReleased;

	//std::function<void(void)>	onPressed;
	//std::function<void(void)>	onReleased;
	


	SKeyActionEvent()
		//:onReleased(nullptr)
		//, onPressed(nullptr)
	{}
	~SKeyActionEvent()
	{
		//onReleased = nullptr;
		//onPressed = nullptr;
	}
};


class Input : public XObject
{

	friend class Globals;
	friend class Engine;
	friend class InputEditorWidget;


	using ActionMap = unordered_map <std::string, SKeyActionEvent>;
	using AxisValueMap = unordered_map<std::string, std::vector<SKeyAxisValue>>;
	using AxisEventMap = std::unordered_map<std::string, std::vector<std::function<void(float)>>>;
	//unordered_map<std::string, std::function<void(float)>>;


private:
	Input();
	virtual~Input();


private:
	void Initialize();

	void SetSceneManager(class SceneManager* sceneMgr);


	void InitializeDirectInput();
	void InitializeWindowInput();


	virtual void Update(float DeltaSeconds);

	virtual void PostUpdate(float DeltaSeoncds) override;


	void ReadDirectInputKeyBoard();
	void ReadDirectInputMouse();


	void UpdateMouse(float DeltaSeconds);
	void UpdateKey();

public:
	/* 이함수만 InKey 윈도우 가상키 값 을 사용 */
	void CreateActionKey(const std::string& InName, const uchar8 InKey);
	void DeleteActionKey(const std::string& InName);
	void DeleteActionKey(const std::string& InName,const uchar8 InKey);


	/* Return Changed Converted Key */
	uchar8 ChangeActionKey(const std::string& InName, const uchar8 InOldKey , const char8 InNewKey);

	void ReNameActionKey(const std::string& InOriginName, const std::string& InNewName);



	/* 이함수만 InKey 윈도우 가상키 값 을 사용 */
	void CreateAxisKey(const std::string& InName, const uchar8 InKey, const float InScale);

	
	uchar8 ChangeAxisKey(const std::string& InName, const uchar8 InOldKey, const uchar8 InNewKey);

	void ReNameAxisKey(const std::string& InOriginName, const std::string& InNewName);

	void DeleteAxisBind(const std::string& InName);

	void DeleteAxisKey(const std::string& InName,const uchar8 InKey);




	


	virtual bool Serialize(Archive& ar) override;

public:
	uint64 AddActionBind(const std::string& InName, EInputEventType EventType, std::function<void(const std::string&, uint64)> callback);
	void ClearActionBind(const std::string& InName, EInputEventType EventType , uint64 ID = 0);
	void ClearActionBinds();

	uint64 AddAxisBind(const std::string& InName, std::function<void(float)> callbck);

	void ClearAxisBinds();
	void ClearAxisBind(const std::string& InName, int64 ID = INDEX_NONE);

public:
	void SetKeyCtrl(const std::string& InName, uchar8 InKey, bool Ctrl = true);
	void SetKeyShift(const std::string& InName, uchar8 InKey, bool Shift = true);
	void SetKeyAlt(const std::string& InName, uchar8 InKey, bool Alt = true);


	bool GetKeyCtrl(const std::string& InName, uchar8 InKey) const;
	bool GetKeyShift(const std::string& InName, uchar8 InKey) const;
	bool GetKeyAlt(const std::string& InName, uchar8 InKey) const;


	void SetAxisKeyCtrl(const std::string& InName, uchar8 InKey, bool Ctrl = true);
	void SetAxisKeyShift(const std::string& InName, uchar8 InKey, bool Shift= true);
	void SetAxisKeyAlt(const std::string& InName, uchar8 InKey, bool Alt = true);


	bool GetAxisKeyCtrl(const std::string& InName, uchar8 InKey) const;
	bool GetAxisKeyShift(const std::string& InName, uchar8 InKey) const;
	bool GetAxisKeyAlt(const std::string& InName, uchar8 InKey) const;



	SVector2	GetMousePosition() const;
	SVector2	GetMouseWorld2D() const;
	SVector2	GetMouseMove() const;

	void		SetMouseWheelDelta(int32 WheelDelta);

	int32		GetMouseWheelDelta() const;

	int32		ConsumeMouseWheelDelta();


	bool		GetMouseState(uchar8 MouseButton);
	

	size_t GetActionBindingNames(std::vector<std::string>& ActionBindingNames);	
	
	size_t GetKeyFromActionName(const std::string& InActionName , std::list<uchar8>& BoundKeys);

	std::string ConvertKeyToString(uchar8 Key);

	size_t GetAxisBindingNames(std::vector<std::string>& AxisBindingNames);

	float GetAxisScale(const std::string& InName, uchar8 InKey) const;

	void SetAxisScale(const std::string& InName, uchar8 InKey, const float InScale);

	size_t GetAxisKeys(const std::string& InName, std::vector<uchar8>& keys);	

	size_t GetAxisInfoFromKey(uchar8 InKey, std::vector<SKeyAxisValue>& AxisInfos, std::vector<std::string>& AxisBindNames);

	size_t GetAxisKeyNameFromKey(uchar8 InKey, std::vector<std::string>& AxisBindNames) const;

	bool IsMappedKey(uchar8 InKey);


public:
	uchar8 ConvertKey(uchar8 Key);
	

private:
	uchar8	m_KeyArray[256];
	uchar8	m_OldKeyState[256];


	std::vector<uchar8>										m_mappedKey;		


	std::unordered_map <std::string, SKeyActionEvent>					m_ActionNameToEvent;	
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>			m_KeyToActionNames;	




	std::unordered_map<std::string, std::vector<SKeyAxisValue>>						m_AxisValues;
	//std::unordered_map<std::string, std::function<void(float)>>						m_AxisEvents;

	std::unordered_map<std::string, std::vector<std::function<void(float)>>>		m_AxisEvents;

	std::unordered_map<uchar8, std::string>											m_KeyToAxisName;
	

	bool	m_Ctrl;
	bool	m_Alt;
	bool	m_Shift;

	SVector2		m_MousePos;
	SVector2		m_MouseWorldPos;
	SVector2		m_MouseMove;

	//DInput
private:
	IDirectInput8* m_Input;

	IDirectInputDevice8* m_Keyboard;
	IDirectInputDevice8* m_Mouse;

	DIMOUSESTATE	m_MouseState;

	EInputType		 m_InputType;

	HWND			m_hWnd;

	bool			m_Activate;


	SVector2			m_MouseOnWindowPosition;

	//Check if GameBegin ?
	class SceneManager* m_cachedSceneManager;
	

	int32			m_WheelDelta;

public:
	bool	Save();
	bool	Load();



};

