#include "EnginePCH.h"
#include "Input.h"
#include "../EngineWindow.h"
#include "../Path/PathManager.h"
#include "../FileSystem/FileSystem.h"
#include <CameraManager.h>
#include <SceneManager.h>
#include "Assets/Scene/Scene.h"
#include <Application.h>


Input::Input()
	:m_hWnd(nullptr)
	, m_InputType(EInputType::Direct)
	, m_MouseState{}
	, m_Keyboard(nullptr)
	, m_Mouse(nullptr)
	, m_Input(nullptr)
{
	m_Activate = true;
	::memset(m_OldKeyState, 0, sizeof(m_KeyArray));
	::memset(m_KeyArray, 0, sizeof(m_KeyArray));
}
Input::~Input()
{

	SAFE_RELEASE(m_Keyboard);
	SAFE_RELEASE(m_Mouse);
	SAFE_RELEASE(m_Input);
}


void Input::Initialize()
{
	
	EngineWindow* wnd = g_Engine->Get<EngineWindow>();

	if (!wnd)
		return;

	m_hWnd = wnd->Handle();

	HRESULT Result = DirectInput8Create(::GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8,
										(void**)&m_Input, nullptr);


	if (FAILED(Result))
		m_InputType = EInputType::Window;

	switch (m_InputType)
	{
	case EInputType::Direct:
		InitializeDirectInput();
		break;
	case EInputType::Window:
		InitializeWindowInput();
		break;
	default:
		break;
	}
	m_Ctrl = false;
	m_Alt = false;
	m_Shift = false;


	Load();

}

void Input::SetSceneManager(class SceneManager* sceneMgr)
{
	m_cachedSceneManager = sceneMgr;
}

void Input::InitializeDirectInput()
{


	(m_Input->CreateDevice(GUID_SysKeyboard, &m_Keyboard, nullptr));

	(m_Keyboard->SetDataFormat(&c_dfDIKeyboard));
	(m_Input->CreateDevice(GUID_SysMouse, &m_Mouse, nullptr));
	(m_Mouse->SetDataFormat(&c_dfDIMouse));


}

void Input::InitializeWindowInput()
{

}

void Input::Update(float DeltaSeconds)
{
	if (m_Activate)
	{
		if (m_InputType == EInputType::Direct)
		{
			ReadDirectInputKeyBoard();
			ReadDirectInputMouse();
		}

		UpdateMouse(DeltaSeconds);
		UpdateKey();
	}
}

void Input::PostUpdate(float DeltaSeoncds)
{
	ConsumeMouseWheelDelta();
}

void Input::ReadDirectInputKeyBoard()
{
	::memcpy(m_OldKeyState, m_KeyArray, sizeof(m_KeyArray));

	HRESULT	result = m_Keyboard->GetDeviceState(256, m_KeyArray);
	
	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_Keyboard->Acquire();
	}
}

void Input::ReadDirectInputMouse()
{
	HRESULT	result = m_Mouse->GetDeviceState(sizeof(m_MouseState), &m_MouseState);

	if (FAILED(result))
	{
		if (result == DIERR_INPUTLOST || result == DIERR_NOTACQUIRED)
			m_Mouse->Acquire();
	}
}

void Input::UpdateMouse(float DeltaSeconds)
{
	POINT	MouseWindowPos;

	GetCursorPos(&MouseWindowPos);
	ScreenToClient(m_hWnd, &MouseWindowPos);

	RECT	WindowRC;

	GetClientRect(m_hWnd, &WindowRC);

	m_MouseOnWindowPosition = SVector2((int32)MouseWindowPos.x, (int32)MouseWindowPos.y);

	m_MousePos = m_MouseOnWindowPosition;
	
	
	

}

void Input::UpdateKey()
{
	switch (m_InputType)
	{
	case EInputType::Direct:
		if (m_KeyArray[DIK_LCONTROL] & 0x80)
			m_Ctrl = true;
		else
			m_Ctrl = false;
		if (m_KeyArray[DIK_LALT] & 0x80)
			m_Alt = true;
		else
			m_Alt = false;
		if (m_KeyArray[DIK_LSHIFT] & 0x80)
			m_Shift = true;
		else
			m_Shift = false;
		break;
	case EInputType::Window:
		if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			m_Ctrl = true;

		else
			m_Ctrl = false;

		if (GetAsyncKeyState(VK_MENU) & 0x8000)
			m_Alt = true;

		else
			m_Alt = false;

		if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			m_Shift = true;

		else
			m_Shift = false;

		break;
	default:
		break;
	}


	size_t size = m_mappedKey.size();
	bool ConsumeIMGUIMouseInput = false;
	bool ConsumeIMGUIKeyInput = false;	

	if(g_Engine->IsEditMode())
	{
		Application* app = g_Engine->GetApplication();
		if (app)
		{
			ConsumeIMGUIMouseInput = app->IsConsumeIMGUIMouseInput();
			ConsumeIMGUIKeyInput = app->IsConsumeIMGUIKeyInput();
		}

		Scene* curScene = m_cachedSceneManager->GetCurrentScene();
		if (curScene && curScene->IsBegan())
		{
			ConsumeIMGUIMouseInput = false;
			ConsumeIMGUIKeyInput = false;
		}
	}
	
	for (size_t i = 0; i < size; i++)
	{
		uchar8 Key = m_mappedKey[i];
		//float axis = 0.0f;
		std::vector<float>	Axisis;
		

		std::vector<SKeyAxisValue>	vecAxisInfos;
		std::vector<std::string>	vecAxisNames;
		size_t axisSize = GetAxisInfoFromKey(Key, OUT vecAxisInfos, OUT vecAxisNames);
		Axisis.resize(axisSize);


		switch (Key)
		{
		case DIK_MOUSELBUTTON:
			if (m_MouseState.rgbButtons[0] & 0x80)
			{
				if (!ConsumeIMGUIMouseInput)
					m_KeyArray[Key] = m_MouseState.rgbButtons[0];
			}
			break;
		case DIK_MOUSERBUTTON:
			if (m_MouseState.rgbButtons[1] & 0x80)
			{
				if (!ConsumeIMGUIMouseInput)
					m_KeyArray[Key] = m_MouseState.rgbButtons[1];
			}
			break;
		}


		if (ConsumeIMGUIKeyInput)
			break;
		//등록된 키를  눌렀을 떄
		if (m_KeyArray[Key] & 0x80)
		{	
			for (size_t a = 0; a < axisSize; a++)
			{				
				if (vecAxisInfos[a].Key == Key && 
					vecAxisInfos[a].Shift == m_Shift &&
					vecAxisInfos[a].Ctrl == m_Ctrl && 
					vecAxisInfos[a].Alt == m_Alt)
				{
					Axisis[a] = vecAxisInfos[a].scale;
					
				}
			}
			
	
			//이전프래임에 입력이없었다면
			if (!(m_OldKeyState[Key] & 0x80))
			{
				size_t actionNameSize = m_KeyToActionNames[Key].size();
				for(size_t a = 0;a<actionNameSize;a++)
				{
					SKeyActionEvent actionEvent = m_ActionNameToEvent[m_KeyToActionNames[Key][a].ActionName];
					if (m_KeyToActionNames[Key][a].Ctrl == m_Ctrl &&
						m_KeyToActionNames[Key][a].Alt == m_Alt &&
						m_KeyToActionNames[Key][a].Shift == m_Shift)
					{
						uint64 pressSize = (uint64)actionEvent.onPressed.size();
						for(size_t p = 0; p<pressSize; p++)
						{
							if (actionEvent.onPressed[p])
								actionEvent.onPressed[p](m_KeyToActionNames[Key][a].ActionName,p+1);
						}

					}
				}		



			}

		}
		//등록된 키를떗을떄
		else
		{
			
			for (int32 i = 0; i < axisSize; i++)
			{
				vecAxisInfos[i].Called = false;
			}
			//axis = 0;		
			//이전 프레임에서 키를 눌렀다면
			if ((m_OldKeyState[Key] & 0x80))
			{
				size_t actionNameSize = m_KeyToActionNames[Key].size();
				for (size_t a = 0; a < actionNameSize; a++)
				{
					SKeyActionEvent* actionEvent = &(m_ActionNameToEvent[m_KeyToActionNames[Key][a].ActionName]);
					if (m_KeyToActionNames[Key][a].Ctrl == m_Ctrl &&
						m_KeyToActionNames[Key][a].Alt == m_Alt &&
						m_KeyToActionNames[Key][a].Shift == m_Shift)
					{
						uint64 releaseSize = (uint64)actionEvent->onReleased.size();
						for (uint64 r = 0; r < releaseSize; r++)
						{
							if (actionEvent->onReleased[r])
								actionEvent->onReleased[r](m_KeyToActionNames[Key][a].ActionName,r+1);
						}
					}
				}

			}

		}

		//left를 누를떄 right를 이미 처리했다면 들어오지말것
		for(size_t a = 0; a<axisSize;a++)
		{
			//일치하는 키를 누른경우에만..			
			AxisEventMap::iterator iter = m_AxisEvents.find(vecAxisNames[a]);
			
			if (iter != m_AxisEvents.end() && !vecAxisInfos[a].Called)
			{
				size_t bindSize = iter->second.size();
				for(size_t b= 0;b<bindSize;b++)
				{
					if (iter->second[b])
					{
						iter->second[b](Axisis[a]);
					}

				}
		

				if (axisSize > 1)
				{					
					vecAxisInfos[a].Called = true;
				}

			}
		}
		
		//if (findValueByKey != Axisvalue.end() && !findValueByKey->Called)
		//{
		//	AxisEventMap::iterator iter = m_AxisEvents.find(axisName);
		//	if (iter != m_AxisEvents.end())
		//	{
		//		if(iter->second)
		//			iter->second(axis);
		//		//단일 키인경우 예외처리
		//		//r이 한번이라도온경우 l을 부르면안됨..
		//		if (axisSize > 1)
		//		{
		//			if (FoundValues)
		//			{
		//				for (int32 i = 0; i < axisSize; i++)
		//				{
		//					valueIter->second[i].Called = true;

		//				}
		//			}
		//		}

		//	}
		//}
	}
}

void Input::CreateActionKey(const std::string& InName, const uchar8 InKey)
{
	uchar8 convertedKey = ConvertKey(InKey);

	if (!IsMappedKey(convertedKey))
		m_mappedKey.push_back(convertedKey);

	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator nameFindIter = m_KeyToActionNames.find(convertedKey);

	if (nameFindIter != m_KeyToActionNames.end())
	{
		size_t actionNameSize = nameFindIter->second.size();
		for (size_t i = 0; i < actionNameSize; i++)
		{
			if (nameFindIter->second[i].ActionName == InName)
				return;
		}
	}

	
	m_ActionNameToEvent.insert(std::make_pair(InName, SKeyActionEvent()));

	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator findIter = m_KeyToActionNames.find(convertedKey);
	if (findIter == m_KeyToActionNames.end())
	{
		std::vector<SActionKeyIfno>	vecInfos;
	
		vecInfos.push_back(SActionKeyIfno(InName));
		m_KeyToActionNames.insert(make_pair(convertedKey, vecInfos));
	}
	else
	{
		findIter->second.push_back(InName);
	}
	

	Save();
	
}

void Input::DeleteActionKey(const std::string& InName)
{
	//m_ActionNameToEvent 에서 InName에 해당하는 부분 제거
	std::unordered_map <std::string, SKeyActionEvent>::iterator findIter = m_ActionNameToEvent.find(InName);
	
	if (findIter != m_ActionNameToEvent.end())
	{
		m_ActionNameToEvent.erase(findIter);		
	}



	//m_KeyToActionNames 에서 이름이 같은 부분을 제거
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator keyIter = m_KeyToActionNames.begin();
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator keyIterEnd = m_KeyToActionNames.end();
	for(;keyIter != keyIterEnd;keyIter++)
	{
		std::vector<SActionKeyIfno>::iterator infoIter = keyIter->second.begin();
		std::vector<SActionKeyIfno>::iterator infoIterEnd= keyIter->second.end();

		for(; infoIter != infoIterEnd;)
		{
			if ((*infoIter).ActionName == InName)
			{
				infoIter = keyIter->second.erase(infoIter);
				infoIterEnd = keyIter->second.end();
			} else
				infoIter++;
		}

	}	
	
	Save();
}

void Input::DeleteActionKey(const std::string& InName, const uchar8 InKey)
{
	

	std::unordered_map <std::string, SKeyActionEvent>::iterator findIter = m_ActionNameToEvent.find(InName);
	if (findIter != m_ActionNameToEvent.end())
	{

		uchar8 key = InKey;
		std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator keyFinder = m_KeyToActionNames.find(key);
		if (keyFinder != m_KeyToActionNames.end())
		{
			size_t keySize = keyFinder->second.size();
			for (size_t i = 0; i < keySize; i++)
			{
				if (keyFinder->second[i].ActionName == InName)
				{
					keyFinder->second.erase(keyFinder->second.begin() + i);
					break;
				}
			}
		}

		//m_ActionNameToEvent.erase(findIter);
	}

	Save();
}

uchar8 Input::ChangeActionKey(const std::string& InName, const uchar8 InOldKey, const char8 InNewKey)
{
	uchar8 convertedKey = ConvertKey(InNewKey);
	uchar8 oldKey = InOldKey;	

	//이전 키에서 ActionName을 제거.
	if (oldKey != convertedKey)
	{
		std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator keyIter = m_KeyToActionNames.find(oldKey);		
		size_t infoSize = keyIter->second.size();

		for(size_t i =0;i<infoSize;i++)
		{
			if (keyIter->second[i].ActionName == InName)
				keyIter->second.erase(keyIter->second.begin() + i);
		}


		std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator findIter = m_KeyToActionNames.find(convertedKey);
		if (findIter == m_KeyToActionNames.end())
		{
			std::vector<SActionKeyIfno>	vecInfos;
			vecInfos.push_back(SActionKeyIfno(InName));
			m_KeyToActionNames.insert(make_pair(convertedKey, vecInfos));
		}
		else
		{
			std::vector<SActionKeyIfno>::iterator actionInfoIter = std::find_if(findIter->second.begin(), findIter->second.end(), [=](const SActionKeyIfno& e)
			{
				return e.ActionName == InName;
			});
			
			if(actionInfoIter == findIter->second.end())
			{
				findIter->second.push_back(InName);
			}

		}

	}


	

	
	if(!IsMappedKey(convertedKey))
		m_mappedKey.push_back(convertedKey);
	

	Save();
	return convertedKey;
}

void Input::CreateAxisKey(const std::string& InName, const uchar8 InKey, const float InScale)
{
	uchar8 convertedKey = ConvertKey(InKey);

	if (!IsMappedKey(convertedKey))
		m_mappedKey.push_back(convertedKey);


	AxisValueMap::iterator finditer = m_AxisValues.find(InName);
	//이미있다면 
	if (finditer != m_AxisValues.end())
	{
		vector<SKeyAxisValue> axisVals = finditer->second;
		vector<SKeyAxisValue>::iterator eventfinditer =
			find_if(axisVals.begin(), axisVals.end(), [=](SKeyAxisValue e)
		{
			return e.Key == convertedKey;
		});
		if (eventfinditer == axisVals.end())
		{
			finditer->second.push_back(SKeyAxisValue{ convertedKey,InScale });
		}

	} 
	else
	{
		vector<SKeyAxisValue>	AxisEvents;
		AxisEvents.push_back(SKeyAxisValue{ convertedKey,InScale });
		m_AxisValues.insert(make_pair(InName, AxisEvents));

	}


	m_AxisEvents.insert(make_pair(InName, std::vector<std::function<void(float)>>()));
	
	Save();

}

uchar8 Input::ChangeAxisKey(const std::string& InName, const uchar8 InOldKey, const uchar8 InNewKey)
{
	uchar8 convertedKey = ConvertKey(InNewKey);
	uchar8 oldKey = InOldKey;

	
	if (oldKey != convertedKey)
	{
		AxisValueMap::iterator findIter = m_AxisValues.find(InName);
		if(findIter != m_AxisValues.end())
		{
			size_t axisValSize = findIter->second.size();
			size_t changeIdx = -1;
			for(size_t i =0; i < axisValSize;i++)
			{
				if(findIter->second[i].Key == InOldKey)
				{					
					changeIdx = i;
				}
				else if(findIter->second[i].Key == convertedKey)
				{
					changeIdx = -1;
					break;
				}
			}
			if (changeIdx != INDEX_NONE)
				findIter->second[changeIdx].Key = convertedKey;
			
		}
	}
	
	Save();
	return convertedKey;
}

void Input::ReNameAxisKey(const std::string& InOriginName, const std::string& InNewName)
{	
	AxisValueMap::iterator findNew= m_AxisValues.find(InNewName);	

	if (findNew == m_AxisValues.end())
	{
		if (InOriginName != InNewName)
		{
			AxisValueMap::iterator findOrigin = m_AxisValues.find(InOriginName);
			AxisEventMap::iterator findOriginEvent = m_AxisEvents.find(InOriginName);
			if(findOrigin != m_AxisValues.end())
			{
				m_AxisValues.insert(std::make_pair(InNewName, findOrigin->second));
				m_AxisValues.erase(findOrigin);
				m_AxisEvents.insert(std::make_pair(InNewName, findOriginEvent->second));
				m_AxisEvents.erase(findOriginEvent);
				
			}

		}
	}
	
	

	Save();
}

void Input::DeleteAxisBind(const std::string& InName)
{
	AxisValueMap::iterator iter =  m_AxisValues.find(InName);
	AxisEventMap::iterator iter2 = m_AxisEvents.find(InName);

	if (iter != m_AxisValues.end())
		m_AxisValues.erase(iter);

	if (iter2 != m_AxisEvents.end())
		m_AxisEvents.erase(iter2);

	Save();
}

void Input::DeleteAxisKey(const std::string& InName, const uchar8 InKey)
{
	AxisValueMap::iterator iter = m_AxisValues.find(InName);
	

	if (iter != m_AxisValues.end())
	{
		size_t axisvalSize = iter->second.size();
		for(size_t i =0;i<axisvalSize;i++)
		{
			//같은 이름의 바인드에 같은 키는 1개..
			if (iter->second[i].Key == InKey)
			{
				iter->second.erase(iter->second.begin() + i);
				break;
			}

		}
	}

	Save();
}

void Input::ReNameActionKey(const std::string& InOriginName, const std::string& InNewName)
{	
	std::unordered_map <std::string, SKeyActionEvent>::iterator findNew = m_ActionNameToEvent.find(InNewName);

	if (findNew == m_ActionNameToEvent.end())
	{
		if (InOriginName != InNewName)
		{

			std::unordered_map <std::string, SKeyActionEvent>::iterator iterFinder = m_ActionNameToEvent.find(InOriginName);
			uchar8 Key = 0;
			if (iterFinder != m_ActionNameToEvent.end())
			{
				m_ActionNameToEvent.insert(std::make_pair(InNewName, (*iterFinder).second));				

				std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator ktnIter= m_KeyToActionNames.begin();
				std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator ktnEnd= m_KeyToActionNames.end();
				for(; ktnIter !=ktnEnd; ktnIter++)
				{

					size_t nameSize = ktnIter->second.size();
					for(size_t i =0;i<nameSize;i++)
					{
						if (ktnIter->second[i].ActionName == InOriginName)
							ktnIter->second[i].ActionName = InNewName;
					}
			

				}

				m_ActionNameToEvent.erase(iterFinder);
			}
		}
	}

	Save();
}

bool Input::Serialize(Archive& ar)
{	

	if(ar.IsLoading())
	{
		std::vector<std::string>	vecActionNames;
		ar.Serialize("actionNames", vecActionNames);
		size_t nameSize = vecActionNames.size();
		for(size_t i = 0; i <nameSize;i++)
		{
			if(m_ActionNameToEvent.find(vecActionNames[i]) == m_ActionNameToEvent.end())
			{
				m_ActionNameToEvent.insert(std::make_pair(vecActionNames[i], SKeyActionEvent()));
			}			
		}	
		std::vector<uchar8>	vecKeys;
		std::vector<std::vector<SActionKeyIfno>>	vecValuse;		
		ar.Serialize("actionbindKeys", vecKeys);		

		uint32 valueSize = 0;

		ar.BeginObject("actionmap_valuse");

		ar.SerializeArraySize(valueSize);
		vecValuse.resize(valueSize);
		for (uint32 i = 0; i < valueSize; i++)
		{
			ar.BeginObject(i);
			ar.Serialize("value", vecValuse[i]);
			ar.EndObject();
		}
		ar.EndObject();
		//ar.Serialize("actionbindInfos", vecValuse);
		size_t keySize = vecKeys.size();
		for(size_t i =0;i< keySize;i++)
		{
			m_KeyToActionNames.insert(std::make_pair(vecKeys[i],vecValuse[i]));
			if (std::find(m_mappedKey.begin(), m_mappedKey.end(), vecKeys[i]) == m_mappedKey.end())
				m_mappedKey.push_back(vecKeys[i]);
		}



		//////////////////////////////////////Axis
		
		std::vector<std::string>	vecAxisNames;		
		ar.Serialize("axisNames", vecAxisNames);		
		size_t axisNameSize = vecAxisNames.size();


		for (size_t i = 0; i < axisNameSize; i++)
		{			

			ar.BeginObject(vecAxisNames[i]);
			std::vector<SKeyAxisValue> vals;
			ar.Serialize("axisValues", vals);
			size_t axisValSize = vals.size();

			for(size_t j=0;j<axisValSize;j++)
			{

				if (!IsMappedKey(vals[j].Key))
					m_mappedKey.push_back(vals[j].Key);

				

				AxisValueMap::iterator finditer = m_AxisValues.find(vecAxisNames[i]);
				//이미있다면 
				if (finditer != m_AxisValues.end())
				{
					vector<SKeyAxisValue> axisVals = finditer->second;
					vector<SKeyAxisValue>::iterator eventfinditer =
						find_if(axisVals.begin(), axisVals.end(), [=](SKeyAxisValue e)
					{
						return e.Key == vals[j].Key;
					});
					if (eventfinditer == axisVals.end())
					{
						finditer->second.push_back(SKeyAxisValue{ vals[j].Key,vals[j].scale });
					}

				} else
				{
					vector<SKeyAxisValue>	AxisEvents;
					AxisEvents.push_back(SKeyAxisValue{ vals[j].Key,vals[j].scale });
					m_AxisValues.insert(make_pair(vecAxisNames[i], AxisEvents));
				}				

				m_AxisEvents.insert(make_pair(vecAxisNames[i], std::vector<std::function<void(float)>>()));
				
			}

			ar.EndObject();
		}

		

	}
	else
	{		
		
		std::vector<std::string>	vecActionNames;
		GetActionBindingNames(OUT vecActionNames);
		ar.Serialize("actionNames", vecActionNames);

		//int32 actionmapSize = (int32)m_KeyToActionNames.size();
		//ar.Serialize("actionmapSize", actionmapSize);

		std::vector<uchar8>	vecKeys;
		std::vector<std::vector<SActionKeyIfno>>	vecValuse;

		
		std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator keyIter = m_KeyToActionNames.begin();
		std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator keyIterEnd = m_KeyToActionNames.end();
		for(;keyIter!= keyIterEnd;keyIter++)
		{
			if(keyIter->second.size()>0)
			{
				vecKeys.push_back((*keyIter).first);
				vecValuse.push_back(keyIter->second);
			}

		}				
		
		ar.Serialize("actionbindKeys", vecKeys);


		uint32 valueSize = (uint32)vecValuse.size();
		ar.BeginObject("actionmap_valuse");
		ar.SerializeArraySize(valueSize);		
		for(uint32 i=0;i<valueSize;i++)
		{
			ar.BeginObject(i);
			ar.Serialize("value", vecValuse[i]);
			ar.EndObject();
		}
		ar.EndObject();
	


		std::vector<std::string>	vecAxisNames;
		size_t axisNameSize = GetAxisBindingNames(OUT vecAxisNames);
		ar.Serialize("axisNames", vecAxisNames);
		for(size_t i =0;i<axisNameSize;i++)
		{
			ar.BeginObject(vecAxisNames[i]);
			AxisValueMap::iterator axisValIter = m_AxisValues.find(vecAxisNames[i]);
			if(axisValIter != m_AxisValues.end())
			{
				ar.Serialize("axisValues", axisValIter->second);
			}
			ar.EndObject();

		}

		



	}



	return true;
}

uint64 Input::AddActionBind(const std::string& InName, EInputEventType EventType, std::function<void(const std::string&, uint64)> callback)
{
	ActionMap::iterator finditer = m_ActionNameToEvent.find(InName);

	if (finditer == m_ActionNameToEvent.end())
		return 0;
	
	switch (EventType)
	{
	case EInputEventType::IE_Pressed:
	{		
		finditer->second.onPressed.push_back(callback);
		return finditer->second.onPressed.size();		
	}
	case EInputEventType::IE_Released:
	{
		finditer->second.onReleased.push_back(callback);
		return finditer->second.onReleased.size();
	}
	default:
		break;
	}


	return 0;
}

void Input::ClearActionBind(const std::string& InName, EInputEventType EventType, uint64 ID )
{
	ActionMap::iterator finditer = m_ActionNameToEvent.find(InName);


	if (finditer != m_ActionNameToEvent.end())
	{
		switch (EventType)
		{
		case EInputEventType::IE_Pressed:
		{
			size_t idToIdx = (size_t)(ID - 1);

			if ((finditer)->second.onPressed.size() > idToIdx)
				finditer->second.onPressed[idToIdx] = nullptr;

			if (ID == 0)
				finditer->second.onPressed.clear();			
		}
		return;
		case EInputEventType::IE_Released:
		{
			size_t idToIdx = (size_t)(ID - 1);

			if ((finditer)->second.onReleased.size() > idToIdx)
				finditer->second.onReleased[idToIdx] = nullptr;

			if (ID == 0)
				finditer->second.onReleased.clear();
			
		}
		return;
		default:
			break;
		}

	}
}

void Input::ClearActionBinds()
{
	ActionMap::iterator iter = m_ActionNameToEvent.begin();
	ActionMap::iterator End = m_ActionNameToEvent.end();

	for (; iter != End; iter++)
	{
		(iter)->second.onReleased.clear();
		(iter)->second.onPressed.clear();
	}
}

uint64 Input::AddAxisBind(const std::string& InName, std::function<void(float)> callbck)
{
	if (m_AxisEvents.find(InName) != m_AxisEvents.end())
	{
		m_AxisEvents[InName].push_back(callbck);
		return (int64)m_AxisEvents[InName].size();
	} else
	{
		//CreateAxisKey로 입력을 만든후에 추가.

	}
	
	return 0;
}

void Input::ClearAxisBinds()
{
	AxisEventMap::iterator iter = m_AxisEvents.begin();
	AxisEventMap::iterator End = m_AxisEvents.end();

	for (; iter != End; iter++)
	{
		(iter)->second.clear();

	}

}

void Input::ClearAxisBind(const std::string& InName, int64 ID )
{
	AxisEventMap::iterator iter = m_AxisEvents.find(InName);

	if (iter != m_AxisEvents.end())
	{
		size_t idToIdx = (size_t)(ID - 1);

		if ((iter)->second.size() > idToIdx)
			(iter)->second[idToIdx] = nullptr;

		if(ID == INDEX_NONE)
			(iter)->second.clear();		

	}
}

void Input::SetKeyCtrl(const std::string& InName, uchar8 InKey, bool Ctrl /*= true*/)
{
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator findIter = m_KeyToActionNames.find(InKey);	

	if (findIter == m_KeyToActionNames.end())
		return;
	
	size_t actionInfoSize = findIter->second.size();

	for(size_t i =0;i< actionInfoSize;i++)
	{
		if (findIter->second[i].ActionName == InName)
			findIter->second[i].Ctrl = Ctrl;
	}
	Save();
	
}

void Input::SetKeyShift(const std::string& InName, uchar8 InKey, bool Shift /*= true*/)
{
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator findIter = m_KeyToActionNames.find(InKey);

	if (findIter == m_KeyToActionNames.end())
		return;


	size_t actionInfoSize = findIter->second.size();

	for (size_t i = 0; i < actionInfoSize; i++)
	{
		if (findIter->second[i].ActionName == InName)
			findIter->second[i].Shift = Shift;
	}
	Save();
}


void Input::SetKeyAlt(const std::string& InName, uchar8 InKey, bool Alt /*= true*/)
{
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator findIter = m_KeyToActionNames.find(InKey);

	if (findIter == m_KeyToActionNames.end())
		return;


	size_t actionInfoSize = findIter->second.size();

	for (size_t i = 0; i < actionInfoSize; i++)
	{
		if (findIter->second[i].ActionName == InName)
			findIter->second[i].Alt = Alt;
	}

	Save();
}

bool Input::GetKeyCtrl(const std::string& InName, uchar8 InKey) const
{
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::const_iterator findIter = m_KeyToActionNames.find(InKey);

	if (findIter == m_KeyToActionNames.end())
		return false;

	size_t actionInfoSize = findIter->second.size();

	for (size_t i = 0; i < actionInfoSize; i++)
	{
		if (findIter->second[i].ActionName == InName)
			return findIter->second[i].Ctrl;
	}

	return false;

}


bool Input::GetKeyShift(const std::string& InName, uchar8 InKey) const
{
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::const_iterator findIter = m_KeyToActionNames.find(InKey);

	if (findIter == m_KeyToActionNames.end())
		return false;

	size_t actionInfoSize = findIter->second.size();

	for (size_t i = 0; i < actionInfoSize; i++)
	{
		if (findIter->second[i].ActionName == InName)
			return findIter->second[i].Shift;
	}

	return false;
}

bool Input::GetKeyAlt(const std::string& InName, uchar8 InKey) const
{
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::const_iterator findIter = m_KeyToActionNames.find(InKey);

	if (findIter == m_KeyToActionNames.end())
		return false;

	size_t actionInfoSize = findIter->second.size();

	for (size_t i = 0; i < actionInfoSize; i++)
	{
		if (findIter->second[i].ActionName == InName)
			return findIter->second[i].Alt;
	}

	return false;
}

void Input::SetAxisKeyCtrl(const std::string& InName, uchar8 InKey, bool Ctrl /*= true*/)
{
	AxisValueMap::iterator findIter = m_AxisValues.find(InName);

	if(findIter != m_AxisValues.end())
	{
		size_t valSize = findIter->second.size();
		for(size_t i =0 ; i<valSize;i++)
		{
			if (findIter->second[i].Key == InKey)
				findIter->second[i].Ctrl = Ctrl;
		}
	}
}

void Input::SetAxisKeyShift(const std::string& InName, uchar8 InKey, bool Shift/*= true*/)
{
	AxisValueMap::iterator findIter = m_AxisValues.find(InName);

	if (findIter != m_AxisValues.end())
	{
		size_t valSize = findIter->second.size();
		for (size_t i = 0; i < valSize; i++)
		{
			if (findIter->second[i].Key == InKey)
				findIter->second[i].Shift = Shift;
		}
	}
}

void Input::SetAxisKeyAlt(const std::string& InName, uchar8 InKey, bool Alt /*= true*/)
{
	AxisValueMap::iterator findIter = m_AxisValues.find(InName);

	if (findIter != m_AxisValues.end())
	{
		size_t valSize = findIter->second.size();
		for (size_t i = 0; i < valSize; i++)
		{
			if (findIter->second[i].Key == InKey)
				findIter->second[i].Alt = Alt;
		}
	}
}

bool Input::GetAxisKeyCtrl(const std::string& InName, uchar8 InKey) const
{
	AxisValueMap::const_iterator findIter = m_AxisValues.find(InName);
	if (findIter != m_AxisValues.end())
	{
		size_t valSize = findIter->second.size();
		for (size_t i = 0; i < valSize; i++)
		{
			if (findIter->second[i].Key == InKey)
				return findIter->second[i].Ctrl;
		}
	}
	return false;
}

bool Input::GetAxisKeyShift(const std::string& InName, uchar8 InKey) const
{
	AxisValueMap::const_iterator findIter = m_AxisValues.find(InName);
	if (findIter != m_AxisValues.end())
	{
		size_t valSize = findIter->second.size();
		for (size_t i = 0; i < valSize; i++)
		{
			if (findIter->second[i].Key == InKey)
				return findIter->second[i].Shift;
		}
	}

	return false;
}

bool Input::GetAxisKeyAlt(const std::string& InName, uchar8 InKey) const
{
	AxisValueMap::const_iterator findIter = m_AxisValues.find(InName);
	if (findIter != m_AxisValues.end())
	{
		size_t valSize = findIter->second.size();
		for (size_t i = 0; i < valSize; i++)
		{
			if (findIter->second[i].Key == InKey)
				return findIter->second[i].Alt;
		}
	}
	return false;
}

SVector2 Input::GetMousePosition() const
{
	return m_MousePos;
}

SVector2 Input::GetMouseWorld2D() const
{
	return m_MouseWorldPos;
}

SVector2 Input::GetMouseMove() const
{
	return m_MouseMove;
}

void Input::SetMouseWheelDelta(int32 WheelDelta)
{
	m_WheelDelta = WheelDelta;
}

int32 Input::GetMouseWheelDelta() const
{
	return m_WheelDelta;
}

int32 Input::ConsumeMouseWheelDelta()
{
	int32 ret = m_WheelDelta;
	m_WheelDelta = 0;
	return ret;
}

bool Input::GetMouseState(uchar8 MouseButton)
{
	switch (MouseButton)
	{
	case VK_LBUTTON:
		return (m_MouseState.rgbButtons[0] & 0x80);		
	case VK_RBUTTON:
		return(m_MouseState.rgbButtons[1] & 0x80);		
	}

	return false;
}

size_t Input::GetActionBindingNames(std::vector<std::string>& ActionBindingNames)
{
	ActionBindingNames.resize(m_ActionNameToEvent.size());
	int32 i = 0;
	std::unordered_map <std::string, SKeyActionEvent>::iterator iter = m_ActionNameToEvent.begin();
	std::unordered_map <std::string, SKeyActionEvent>::iterator iterEnd = m_ActionNameToEvent.end();

	for(;iter != iterEnd;iter++)
	{
		ActionBindingNames[i++] = ((iter)->first);
	}

	return ActionBindingNames.size();
}



size_t Input::GetKeyFromActionName(const std::string& InActionName, std::list<uchar8>& BoundKeys)
{
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator iter = m_KeyToActionNames.begin();
	std::unordered_map <uchar8, std::vector<SActionKeyIfno>>::iterator iterEnd = m_KeyToActionNames.end();

	for (; iter != iterEnd; iter++)
	{
		size_t nameSize = (iter)->second.size();

		for(size_t i =0;i<nameSize;i++)
		{
			if ((iter)->second[i].ActionName == InActionName)
				BoundKeys.push_back(iter->first);
		}


	}

	return BoundKeys.size();
}

std::string Input::ConvertKeyToString(uchar8 Key)
{
		switch (Key)
		{
		case DIK_MOUSELBUTTON:
			return "Mouse LeftButton";
		case DIK_MOUSERBUTTON:
			return "Mouse RightButton";
		case DIK_MOUSEWHEEL:
			return "Mouse Wheel";
		case DIK_BACK:
			return "Back";
		case DIK_SPACE:
			return "SpaceBar";
		//case VK_TAB:
		//	return DIK_TAB;
		//case VK_RETURN:
		//	return DIK_RETURN;
		//case VK_LCONTROL:
		//	return DIK_LCONTROL;
		//case VK_RCONTROL:
		//	return DIK_RCONTROL;
		//case VK_LMENU:
		//	return DIK_LALT;
		//case VK_RMENU:
		//	return DIK_RALT;
		//case VK_LSHIFT:
		//	return DIK_LSHIFT;
		//case VK_RSHIFT:
		//	return DIK_RSHIFT;
		//case VK_PAUSE:
		//	return DIK_PAUSE;
		//case VK_CAPITAL:
		//	return DIK_CAPSLOCK;
		//case VK_ESCAPE:
		//	return DIK_ESCAPE;
		//case VK_SPACE:
		//	return DIK_SPACE;
		//case VK_NEXT:
		//	return DIK_PGDN;
		//case VK_PRIOR:
		//	return DIK_PGUP;
		//case VK_END:
		//	return DIK_END;
		//case VK_HOME:
		//	return DIK_HOME;
		case DIK_LEFT:
			return "Arrow Left";
		case DIK_RIGHT:
			return "Arrow Right";
		case DIK_UP:
			return "Arrow Up";
		case DIK_DOWN:
			return "Arrow Down";
		//case VK_PRINT:
		//	return DIK_SYSRQ;
		//case VK_INSERT:
		//	return DIK_INSERT;
		//case VK_DELETE:
		//	return DIK_DELETE;
		//case VK_HELP:
		//	return 0;
		case DIK_0:
			return "0";
		case DIK_1:
			return "1";
		case DIK_2:
			return "2";
		case DIK_3:
			return "3";
		case DIK_4:
			return "4";
		case DIK_5:
			return "5";
		case DIK_6:
			return "6";
		case DIK_7:
			return "7";
		case DIK_8:
			return "8";
		case DIK_9:
			return "9";
		case DIK_A:
			return "A";
		case DIK_B:
			return "B";
		case DIK_C:
			return "C";
		case DIK_D:
			return "D";
		case DIK_E:
			return "E";
		case DIK_F:
			return "F";
		case DIK_G:
			return "G";
		case DIK_H:
			return "H";
		case DIK_I:
			return "I";
		case DIK_J:
			return "J";
		case DIK_K:
			return "K";
		case DIK_L:
			return "L";
		case DIK_M:
			return "M";
		case DIK_N:
			return "N";
		case DIK_O:
			return "O";
		case DIK_P:
			return "P";
		case DIK_Q:
			return "Q";
		case DIK_R:
			return "R";
		case DIK_S:
			return "S";
		case DIK_T:
			return "T";
		case DIK_U:
			return "U";
		case DIK_V:
			return "V";
		case DIK_W:
			return "W";
		case DIK_X:
			return "X";
		case DIK_Y:
			return "Y";
		case DIK_Z:
			return "Z";
	}	
	
	return "Invalid";
}

size_t Input::GetAxisBindingNames(std::vector<std::string>& AxisBindingNames)
{
	AxisBindingNames.resize(m_AxisEvents.size());
	int32 i = 0;
	AxisEventMap::iterator iter = m_AxisEvents.begin();
	AxisEventMap::iterator iterEnd = m_AxisEvents.end();

	for (; iter != iterEnd; iter++)
	{
		AxisBindingNames[i++] = ((iter)->first);
	}

	
	return AxisBindingNames.size();
}

float Input::GetAxisScale(const std::string& InName, uchar8 InKey) const
{
	AxisValueMap::const_iterator Iter = m_AxisValues.find(InName);
	std::vector<SKeyAxisValue>::const_iterator axisIter = Iter->second.begin();
	std::vector<SKeyAxisValue>::const_iterator axisIterEnd = Iter->second.end();

	for(;axisIter!= axisIterEnd;axisIter++)
	{
		if(axisIter->Key == InKey)
		{
			return axisIter->scale;
		}
	}
	
	return 0;
}



void Input::SetAxisScale(const std::string& InName, uchar8 InKey, const float InScale)
{
	AxisValueMap::iterator Iter = m_AxisValues.find(InName);
	std::vector<SKeyAxisValue>::iterator axisIter = Iter->second.begin();
	std::vector<SKeyAxisValue>::iterator axisIterEnd = Iter->second.end();
	

	
	for (; axisIter != axisIterEnd; axisIter++)
	{
		if(axisIter->Key == InKey)
		{
			axisIter->scale = InScale;
		}
	}

	Save();	
}

size_t Input::GetAxisKeys(const std::string& InName, std::vector<uchar8>& keys)
{	
	AxisValueMap::const_iterator Iter = m_AxisValues.find(InName);
	if (Iter == m_AxisValues.end())
		return 0;

	std::vector<SKeyAxisValue>::const_iterator axisIter = Iter->second.begin();
	std::vector<SKeyAxisValue>::const_iterator axisIterEnd = Iter->second.end();
	keys.resize(Iter->second.size());

	int32 i = 0;
	for (; axisIter != axisIterEnd; axisIter++)
	{
		keys[i++] = axisIter->Key;
	}
	


	return keys.size();
}

size_t Input::GetAxisInfoFromKey(uchar8 InKey, std::vector<SKeyAxisValue>& AxisInfos, std::vector<std::string>& AxisBindNames)
{
	AxisValueMap::iterator iter = m_AxisValues.begin();
	AxisValueMap::iterator iterEnd = m_AxisValues.end();

	for (; iter != iterEnd; iter++)
	{
		std::vector<SKeyAxisValue>::iterator axisValIter = iter->second.begin();
		std::vector<SKeyAxisValue>::iterator axisValIterEnd = iter->second.end();

		for (; axisValIter != axisValIterEnd; axisValIter++)
		{
			if ((*axisValIter).Key == InKey)
			{
				AxisInfos.push_back((*axisValIter));
				AxisBindNames.push_back(iter->first);
				
			}
		}

	}

	return AxisInfos.size();
}

size_t Input::GetAxisKeyNameFromKey(uchar8 InKey, std::vector<std::string>& AxisBindNames) const
{
	AxisValueMap::const_iterator iter = m_AxisValues.begin();
	AxisValueMap::const_iterator iterEnd = m_AxisValues.end();

	for (; iter != iterEnd; iter++)
	{
		std::vector<SKeyAxisValue>::const_iterator axisValIter = iter->second.begin();
		std::vector<SKeyAxisValue>::const_iterator axisValIterEnd = iter->second.end();

		for (; axisValIter != axisValIterEnd; axisValIter++)
		{
			if ((*axisValIter).Key == InKey)
			{
				AxisBindNames.push_back(iter->first);
				//(*axisValIter).scale
			}
		}

	}

	return AxisBindNames.size();
}

bool Input::IsMappedKey(uchar8 InKey)
{
	std::vector<uchar8>::iterator iter = m_mappedKey.begin();
	std::vector<uchar8>::iterator iterEnd = m_mappedKey.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter) == InKey)
			return true;
	}

	return false;
}

uchar8 Input::ConvertKey(uchar8 Key)
{
	if (m_InputType == EInputType::Direct)
	{
		switch (Key)
		{
		case VK_LBUTTON:
			return DIK_MOUSELBUTTON;
		case VK_RBUTTON:
			return DIK_MOUSERBUTTON;
		case VK_MBUTTON:
			return DIK_MOUSEWHEEL;
		case VK_BACK:
			return DIK_BACK;
		case VK_TAB:
			return DIK_TAB;
		case VK_RETURN:
			return DIK_RETURN;
		case VK_LCONTROL:
			return DIK_LCONTROL;
		case VK_RCONTROL:
			return DIK_RCONTROL;
		case VK_LMENU:
			return DIK_LALT;
		case VK_RMENU:
			return DIK_RALT;
		case VK_LSHIFT:
			return DIK_LSHIFT;
		case VK_RSHIFT:
			return DIK_RSHIFT;
		case VK_PAUSE:
			return DIK_PAUSE;
		case VK_CAPITAL:
			return DIK_CAPSLOCK;
		case VK_ESCAPE:
			return DIK_ESCAPE;
		case VK_SPACE:
			return DIK_SPACE;
		case VK_NEXT:
			return DIK_PGDN;
		case VK_PRIOR:
			return DIK_PGUP;
		case VK_END:
			return DIK_END;
		case VK_HOME:
			return DIK_HOME;
		case VK_LEFT:
			return DIK_LEFT;
		case VK_UP:
			return DIK_UP;
		case VK_RIGHT:
			return DIK_RIGHT;
		case VK_DOWN:
			return DIK_DOWN;
		case VK_PRINT:
			return DIK_SYSRQ;
		case VK_INSERT:
			return DIK_INSERT;
		case VK_DELETE:
			return DIK_DELETE;
		case VK_HELP:
			return 0;
		case '0':
			return DIK_0;
		case '1':
			return DIK_1;
		case '2':
			return DIK_2;
		case '3':
			return DIK_3;
		case '4':
			return DIK_4;
		case '5':
			return DIK_5;
		case '6':
			return DIK_6;
		case '7':
			return DIK_7;
		case '8':
			return DIK_8;
		case '9':
			return DIK_9;
		case 'A':
			return DIK_A;
		case 'B':
			return DIK_B;
		case 'C':
			return DIK_C;
		case 'D':
			return DIK_D;
		case 'E':
			return DIK_E;
		case 'F':
			return DIK_F;
		case 'G':
			return DIK_G;
		case 'H':
			return DIK_H;
		case 'I':
			return DIK_I;
		case 'J':
			return DIK_J;
		case 'K':
			return DIK_K;
		case 'L':
			return DIK_L;
		case 'M':
			return DIK_M;
		case 'N':
			return DIK_N;
		case 'O':
			return DIK_O;
		case 'P':
			return DIK_P;
		case 'Q':
			return DIK_Q;
		case 'R':
			return DIK_R;
		case 'S':
			return DIK_S;
		case 'T':
			return DIK_T;
		case 'U':
			return DIK_U;
		case 'V':
			return DIK_V;
		case 'W':
			return DIK_W;
		case 'X':
			return DIK_X;
		case 'Y':
			return DIK_Y;
		case 'Z':
			return DIK_Z;
		case VK_OEM_3:
			return DIK_GRAVE;
		case VK_OEM_MINUS:
			return DIK_MINUS;
		case VK_OEM_NEC_EQUAL:
			return DIK_EQUALS;
		case VK_OEM_4:
			return DIK_LBRACKET;
		case VK_OEM_6:
			return DIK_RBRACKET;
		case VK_OEM_5:
			return DIK_BACKSLASH;
		case VK_OEM_1:
			return DIK_SEMICOLON;
		case VK_OEM_7:
			return DIK_APOSTROPHE;
		case VK_OEM_COMMA:
			return DIK_COMMA;
		case VK_OEM_PERIOD:
			return DIK_PERIOD;
		case VK_OEM_2:
			return DIK_SLASH;
		case VK_NUMPAD0:
			return DIK_NUMPAD0;
		case VK_NUMPAD1:
			return DIK_NUMPAD1;
		case VK_NUMPAD2:
			return DIK_NUMPAD2;
		case VK_NUMPAD3:
			return DIK_NUMPAD3;
		case VK_NUMPAD4:
			return DIK_NUMPAD4;
		case VK_NUMPAD5:
			return DIK_NUMPAD5;
		case VK_NUMPAD6:
			return DIK_NUMPAD6;
		case VK_NUMPAD7:
			return DIK_NUMPAD7;
		case VK_NUMPAD8:
			return DIK_NUMPAD8;
		case VK_NUMPAD9:
			return DIK_NUMPAD9;
		case VK_MULTIPLY:
			return DIK_MULTIPLY;
		case VK_ADD:
			return DIK_ADD;
		case VK_SEPARATOR:
			return DIK_NUMPADCOMMA;
		case VK_SUBTRACT:
			return DIK_SUBTRACT;
		case VK_DECIMAL:
			return DIK_DECIMAL;
		case VK_DIVIDE:
			return DIK_DIVIDE;
			//case VK_RETURN:		
			return DIK_NUMPADENTER;
		case VK_F1:
			return DIK_F1;
		case VK_F2:
			return DIK_F2;
		case VK_F3:
			return DIK_F3;
		case VK_F4:
			return DIK_F4;
		case VK_F5:
			return DIK_F5;
		case VK_F6:
			return DIK_F6;
		case VK_F7:
			return DIK_F7;
		case VK_F8:
			return DIK_F8;
		case VK_F9:
			return DIK_F9;
		case VK_F10:
			return DIK_F10;
		case VK_F11:
			return DIK_F11;
		case VK_F12:
			return DIK_F12;
		case VK_F13:
			return DIK_F13;
		case VK_F14:
			return DIK_F14;
		case VK_F15:
			return DIK_F15;
		case VK_F16:
		case VK_F17:
		case VK_F18:
		case VK_F19:
		case VK_F20:
		case VK_F21:
		case VK_F22:
		case VK_F23:
		case VK_F24:
			return 0;
		case VK_NUMLOCK:
			return DIK_NUMLOCK;
		case VK_SCROLL:
			return DIK_SCROLL;
		case VK_LWIN:
			return DIK_LWIN;
		case VK_RWIN:
			return DIK_RWIN;
		case VK_APPS:
			return DIK_APPS;
		case VK_OEM_102:
			return DIK_OEM_102;
		}

		return 0xff;
	}

	return Key;
}


bool Input::Save()
{

	JsonArchive ar{};

	const SPathInfo* settingPath = g_Engine->Get<PathManager>()->FindPath(SETTING_PATH);

	std::string InputSettingPath = settingPath->aPath + "Input.meta";


	Serialize(ar);

	return FileSystem::SaveJsonFile(InputSettingPath, ar.GetData(), ar.GetIndent());

	
}

bool Input::Load()
{
	Json data;
	const SPathInfo* settingPath = g_Engine->Get<PathManager>()->FindPath(SETTING_PATH);

	std::string InputSettingPath = settingPath->aPath + "Input.meta";

	bool jsonLoad = FileSystem::LoadJsonFile(InputSettingPath, OUT data);

	JsonArchive ar(data);

	Serialize(ar);

	return jsonLoad;
}

