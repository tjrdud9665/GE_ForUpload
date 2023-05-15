#pragma once

#include "EditorWidget.h"

class PropertyWidget :public EditorWidget
{
	CLASS(PropertyWidget, EditorWidget)

protected:
	PropertyWidget();
	virtual ~PropertyWidget();

	virtual void PropertyGUI() {}

	virtual void Initialize(PropertyHandle* InProperty,bool IsArray);
	
	virtual void Update(float DeltaSeconds) override;	

	bool IsPropertyValueChanged() const;


	virtual void PostEditProperties();

	virtual size_t GetValueSize() const;


	virtual void BeginProperty();
	virtual void EndProperty();


	//Array Àü¿ë
	virtual int32 GetArraySize() const;
	virtual void ArrayElementClear();
	virtual void AddArrayElement();
	virtual void ArrayElementGUI(int32 idx);
	virtual void AdditionalArrayElementGUI(int32 idx, float DeltaSeconds);

	virtual void InsertArrayElement(int32 idx);
	virtual void DuplicateArrayElement(int32 idx);
	virtual void DeleteArrayElement(int32 idx);



	virtual void Construct(const std::string& InName = "") override;

protected:
	template<typename T>
	TPropertyHandle<std::vector<T>>* GetHandle() const
	{
		return dynamic_cast<TPropertyHandle<std::vector<T>>*>(m_Prop);
	}

protected:
	PropertyHandle* m_Prop;

	bool			m_IsArray;


	void* m_preEditedValue;
	void* m_postEditedValue;

	static void*		m_DeleteIcon;
	static void*		m_PlusIcon;

					

public:
	static PropertyWidget* NewPropertyWidget(EditorWidget* owner, PropertyHandle*& prop);



};

