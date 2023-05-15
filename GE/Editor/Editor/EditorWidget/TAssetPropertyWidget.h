#pragma once
#include "PropertyWidget.h"

#include "Assets/TextureAsset/TextureAsset.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "AssetManager.h"
#include "../ContentEditor.h"
#include "../EditorManager.h"


template<typename AssetType, typename = std::enable_if_t<std::is_base_of<AssetData, AssetType>::value>>
class TAssetPropertyWidget :public PropertyWidget
{
	CLASS(TAssetPropertyWidget, PropertyWidget)

protected:
	TAssetPropertyWidget()
		:m_selectedAssetIndex(INDEX_NONE)		
	{

	}
	virtual ~TAssetPropertyWidget()
	{

	}

protected:
	virtual void PropertyGUI() override
	{
		AssetData** TargetValuePtr = GetTargetAsset();
		TSharedPtr<AssetData>* SharedTarget = nullptr;


		std::string name = "##" + GetName();
		std::vector<AssetData*> Assets;
		std::vector<const char*> AssetNames;
		g_Engine->Get<AssetManager>()->GetAssetsFromInheritedType(OUT Assets, AssetType::StaticClass());
		int32 assetSize = (int32)Assets.size();

		if (m_Prop->IsSharedType_())
		{
			SharedTarget = GetSharedTargetAsset();
		}

		if (!(TargetValuePtr) && !SharedTarget)
		{
			return;
		}



		for (int32 i = 0; i < assetSize; i++)
		{
			AssetNames.push_back(Assets[i]->GetName().c_str());

			if (SharedTarget)
			{
				if ((*SharedTarget))
				{
					if ((*SharedTarget)->GetName() == Assets[i]->GetName())
						m_selectedAssetIndex = i;
				}

			} else if (*TargetValuePtr)
			{
				if ((*TargetValuePtr)->GetName() == Assets[i]->GetName())
					m_selectedAssetIndex = i;
			}
		}

		ID3D11ShaderResourceView* srv = nullptr;
		if (m_selectedAssetIndex != INDEX_NONE)
		{
			if (Assets[m_selectedAssetIndex]->GetClass()->IsA(TextureAsset::StaticClass()))
			{
				srv = ((TextureAsset*)Assets[m_selectedAssetIndex])->GetResource();
				if (srv)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC desc;
					srv->GetDesc(&desc);
					if (desc.ViewDimension != D3D11_SRV_DIMENSION_TEXTURE2D)
					{
						srv = nullptr;
					}
				}
			}
		}
		if(srv)
		{
			if (ImGui::ImageButton(srv, ImVec2(32, 32)))
			{
				//TODO : Show Editor From Asset Type
			}
		}
		else
		{
			if (ImGui::Button("##assetBtn", ImVec2(32, 32)))
			{
				//TODO : Show Editor From Asset Type				
				ContentEditor* contentEdit = g_Engine->Get<EditorManager>()->FindEditor<ContentEditor>("ContentEditor");
				if (m_selectedAssetIndex != INDEX_NONE && m_selectedAssetIndex <= Assets.size())
					contentEdit->OnClickedAsset(Assets[m_selectedAssetIndex]);
				//OnClickedAsset
			}
		}
	

		ImGui::SameLine();


		std::string RelPathWithName;
		if (m_selectedAssetIndex != INDEX_NONE)
			Assets[m_selectedAssetIndex]->GetRelativePathWithName(OUT RelPathWithName);

		if (ImGui::BeginCombo("##custom combo", m_selectedAssetIndex == INDEX_NONE ? "NONE" : RelPathWithName.c_str(), ImGuiComboFlags_None))
		{
			for (int32 i = 0; i < assetSize; i++)
			{
				bool is_selected = (m_selectedAssetIndex == i);
				std::string SelectedRelPathWithName;
				Assets[i]->GetRelativePathWithName(OUT SelectedRelPathWithName);
				if (ImGui::Selectable(SelectedRelPathWithName.c_str(), is_selected))
				{
					m_selectedAssetIndex = i;

					if (SharedTarget)
					{
						(*SharedTarget) = Assets[m_selectedAssetIndex];

					}
					else if (*TargetValuePtr) // Not Shared But Grab this 
					{
						//(*TargetValuePtr)->Release();
						(*TargetValuePtr) = Assets[m_selectedAssetIndex];
						//(*TargetValuePtr)->AddRef();
					}
					else if((*TargetValuePtr) == nullptr )
					{
						(*TargetValuePtr) = Assets[m_selectedAssetIndex];
					}
				}

				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					AssetData* toolTipTargetAsset = Assets[i];
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::Text(toolTipTargetAsset->GetClass()->GetName().c_str());
					ImGui::Text(SelectedRelPathWithName.c_str());

					if (toolTipTargetAsset->GetClass()->IsA(TextureAsset::StaticClass()))
					{
						ID3D11ShaderResourceView* srv = ((TextureAsset*)toolTipTargetAsset)->GetResource();
						if (srv)
						{
							D3D11_SHADER_RESOURCE_VIEW_DESC desc;
							srv->GetDesc(&desc);
							if (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
							{
								ImGui::Image(srv, ImVec2(64, 64));
							}
						}
					} 
					
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}

		if (ImGui::IsItemHovered() && m_selectedAssetIndex != INDEX_NONE)
		{
			ImGui::BeginTooltip();
			AssetData* toolTipTargetAsset = Assets[m_selectedAssetIndex];
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::Text(toolTipTargetAsset->GetClass()->GetName().c_str());
			std::string tooltipRelPathWithName;
			toolTipTargetAsset->GetRelativePathWithName(OUT tooltipRelPathWithName);
			ImGui::Text(tooltipRelPathWithName.c_str());

			if (toolTipTargetAsset->GetClass()->IsA(TextureAsset::StaticClass()))
			{
				ID3D11ShaderResourceView* srv = ((TextureAsset*)toolTipTargetAsset)->GetResource();
				if(srv)
				{
					D3D11_SHADER_RESOURCE_VIEW_DESC desc;
					srv->GetDesc(&desc);
					if (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
					{
						ImGui::Image(srv, ImVec2(64, 64));
					}
				}
				
			} 

			ImGui::PopTextWrapPos();

			ImGui::EndTooltip();
		}


		ImGui::SameLine();
		if (ImGui::Button("null", ImVec2(12, 12)))
		{
			m_selectedAssetIndex = INDEX_NONE;

			if (SharedTarget)
			{
				(*SharedTarget) = nullptr;

			} else if (*TargetValuePtr)
			{
				//(*TargetValuePtr)->Release();
				(*TargetValuePtr) = nullptr;
				//(*TargetValuePtr)->AddRef();
			}
		}

	}

	virtual int32 GetArraySize() const override
	{
		if (m_Prop->IsSharedType_())
		{
			std::vector<TSharedPtr<AssetData>>* val = GetSharedArrayPropHandle()->GetValuePtr();

			if (val)
			{
				return (int32)val->size();
			}
		}
		else
		{
			std::vector<AssetData*>* val = GetArrayHandle()->GetValuePtr();

			if (val)
			{
				return (int32)val->size();
			}
		}
	

		return INDEX_NONE;		
	}

	virtual void ArrayElementClear() override
	{
		if (m_Prop->IsSharedType_())
		{
			std::vector<TSharedPtr<AssetData>>* val = GetSharedArrayPropHandle()->GetValuePtr();

			if (val)
			{
				val->clear();
			}
		} else
		{
			std::vector<AssetData*>* val = GetArrayHandle()->GetValuePtr();

			if (val)
			{
				val->clear();
			}
		}
	}

	virtual void AddArrayElement() override
	{
		if (m_Prop->IsSharedType_())
		{
			std::vector<TSharedPtr<AssetData>>* val = GetSharedArrayPropHandle()->GetValuePtr();

			if (val)
			{
				val->push_back(nullptr);
			}
		} else
		{
			std::vector<AssetData*>* val = GetArrayHandle()->GetValuePtr();

			if (val)
			{
				val->push_back(nullptr);
			}
		}
	}

	virtual void ArrayElementGUI(int32 idx) override
	{

		std::string name = "##" + GetName();
		std::vector<AssetData*> Assets;
		std::vector<const char*> AssetNames;
		g_Engine->Get<AssetManager>()->GetAssetsFromInheritedType(OUT Assets, AssetType::StaticClass());
		int32 assetSize = (int32)Assets.size();


		if (m_Prop->IsSharedType_())
		{
			std::vector<TSharedPtr<AssetData>>* val = GetSharedArrayPropHandle()->GetValuePtr();

			if(val)
			{
				m_SelectedAssetIndexMap[idx] = INDEX_NONE;
				for (int32 i = 0; i < assetSize; i++)
				{
					AssetNames.push_back(Assets[i]->GetName().c_str());
					if (((*val)[idx]))
					{
						if (((*val)[idx])->GetName() == Assets[i]->GetName())
							m_SelectedAssetIndexMap[idx] = i;
					}

				}
				ID3D11ShaderResourceView* srv = nullptr;
				if (m_SelectedAssetIndexMap[idx] != INDEX_NONE)
				{
					if (Assets[m_SelectedAssetIndexMap[idx]]->GetClass()->IsA(TextureAsset::StaticClass()))
					{
						srv = ((TextureAsset*)Assets[m_SelectedAssetIndexMap[idx]])->GetResource();
						if (srv)
						{
							D3D11_SHADER_RESOURCE_VIEW_DESC desc;
							srv->GetDesc(&desc);
							if (desc.ViewDimension != D3D11_SRV_DIMENSION_TEXTURE2D)
							{
								srv = nullptr;
							}
						}
					}
				}

				if (srv)
				{
					if (ImGui::ImageButton(srv, ImVec2(32, 32)))
					{
						//TODO : Show Editor From Asset Type
					}
				} else
				{
					std::string openAssetbtnLabel = "##assetBtn" + std::to_string(idx);
					if (ImGui::Button(openAssetbtnLabel.c_str(), ImVec2(32, 32)))
					{
						//TODO : Show Editor From Asset Type
					}
				}



				ImGui::SameLine();

				std::string RelPathWithName;
				if (m_SelectedAssetIndexMap[idx] != INDEX_NONE)
					Assets[m_SelectedAssetIndexMap[idx]]->GetRelativePathWithName(OUT RelPathWithName);

				std::string comboLabel = "##custom combo" + std::to_string(idx);

				if (ImGui::BeginCombo(comboLabel.c_str(), m_SelectedAssetIndexMap[idx] == INDEX_NONE ? "NONE" : RelPathWithName.c_str(), ImGuiComboFlags_None))
				{
					for (int32 i = 0; i < assetSize; i++)
					{
						bool is_selected = (m_selectedAssetIndex == i);
						std::string SelectedRelPathWithName;
						Assets[i]->GetRelativePathWithName(OUT SelectedRelPathWithName);
						if (ImGui::Selectable(SelectedRelPathWithName.c_str(), is_selected))
						{
							m_SelectedAssetIndexMap[idx] = i;
							(*val)[idx] = Assets[i];
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							AssetData* toolTipTargetAsset = Assets[i];
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::Text(toolTipTargetAsset->GetClass()->GetName().c_str());
							ImGui::Text(SelectedRelPathWithName.c_str());

							if (toolTipTargetAsset->GetClass()->IsA(TextureAsset::StaticClass()))
							{
								ID3D11ShaderResourceView* srv = ((TextureAsset*)toolTipTargetAsset)->GetResource();
								if (srv)
								{
									D3D11_SHADER_RESOURCE_VIEW_DESC desc;
									srv->GetDesc(&desc);
									if (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
									{
										ImGui::Image(srv, ImVec2(64, 64));
									}
								}
							}

							ImGui::PopTextWrapPos();
							ImGui::EndTooltip();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				if (ImGui::IsItemHovered() && m_SelectedAssetIndexMap[idx] != INDEX_NONE)
				{
					ImGui::BeginTooltip();
					AssetData* toolTipTargetAsset = Assets[m_SelectedAssetIndexMap[idx]];
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::Text(toolTipTargetAsset->GetClass()->GetName().c_str());
					std::string tooltipRelPathWithName;
					toolTipTargetAsset->GetRelativePathWithName(OUT tooltipRelPathWithName);
					ImGui::Text(tooltipRelPathWithName.c_str());

					if (toolTipTargetAsset->GetClass()->IsA(TextureAsset::StaticClass()))
					{
						ID3D11ShaderResourceView* srv = ((TextureAsset*)toolTipTargetAsset)->GetResource();
						if (srv)
						{
							D3D11_SHADER_RESOURCE_VIEW_DESC desc;
							srv->GetDesc(&desc);
							if (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
							{
								ImGui::Image(srv, ImVec2(64, 64));
							}
						}

					}

					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}

				ImGui::SameLine();
				std::string btnLabel = "null" + std::to_string(idx);
				if (ImGui::Button(btnLabel.c_str(), ImVec2(12, 12)))
				{
					m_SelectedAssetIndexMap[idx] = INDEX_NONE;
					(*val)[idx] = nullptr;
				}
			}
		
		}
		//Version of Raw Ptr
		else
		{
			std::vector<AssetData*>* val = GetArrayHandle()->GetValuePtr();

			if(val)
			{
				m_SelectedAssetIndexMap[idx] = INDEX_NONE;
				for (int32 i = 0; i < assetSize; i++)
				{
					AssetNames.push_back(Assets[i]->GetName().c_str());
					if(((*val)[idx]))
					{
						if (((*val)[idx])->GetName() == Assets[i]->GetName())
							m_SelectedAssetIndexMap[idx] = i;
					}

				}
				ID3D11ShaderResourceView* srv = nullptr;
				if (m_SelectedAssetIndexMap[idx] != INDEX_NONE)
				{
					if (Assets[m_SelectedAssetIndexMap[idx]]->GetClass()->IsA(TextureAsset::StaticClass()))
					{
						srv = ((TextureAsset*)Assets[m_SelectedAssetIndexMap[idx]])->GetResource();
						if (srv)
						{
							D3D11_SHADER_RESOURCE_VIEW_DESC desc;
							srv->GetDesc(&desc);
							if (desc.ViewDimension != D3D11_SRV_DIMENSION_TEXTURE2D)
							{
								srv = nullptr;
							}
						}
					}
				}			

				if(srv)
				{
					if (ImGui::ImageButton(srv, ImVec2(32, 32)))
					{
						//TODO : Show Editor From Asset Type
					}
				}
				else
				{
					std::string openAssetbtnLabel = "##assetBtn" + std::to_string(idx);
					if (ImGui::Button(openAssetbtnLabel.c_str(), ImVec2(32, 32)))
					{
						//TODO : Show Editor From Asset Type
					}
				}
			


				ImGui::SameLine();

				std::string RelPathWithName;
				if (m_SelectedAssetIndexMap[idx] != INDEX_NONE)
					Assets[m_SelectedAssetIndexMap[idx]]->GetRelativePathWithName(OUT RelPathWithName);

				std::string comboLabel = "##custom combo" + std::to_string(idx);
				
				if (ImGui::BeginCombo(comboLabel.c_str(), m_SelectedAssetIndexMap[idx] == INDEX_NONE ? "NONE" : RelPathWithName.c_str(), ImGuiComboFlags_None))
				{
					for (int32 i = 0; i < assetSize; i++)
					{
						bool is_selected = (m_selectedAssetIndex == i);
						std::string SelectedRelPathWithName;
						Assets[i]->GetRelativePathWithName(OUT SelectedRelPathWithName);
						if (ImGui::Selectable(SelectedRelPathWithName.c_str(), is_selected))
						{
							m_SelectedAssetIndexMap[idx]  = i;
							(*val)[idx] = Assets[i];
						}

						if (ImGui::IsItemHovered())
						{
							ImGui::BeginTooltip();
							AssetData* toolTipTargetAsset = Assets[i];
							ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
							ImGui::Text(toolTipTargetAsset->GetClass()->GetName().c_str());
							ImGui::Text(SelectedRelPathWithName.c_str());

							if (toolTipTargetAsset->GetClass()->IsA(TextureAsset::StaticClass()))
							{
								ID3D11ShaderResourceView* srv = ((TextureAsset*)toolTipTargetAsset)->GetResource();
								if (srv)
								{
									D3D11_SHADER_RESOURCE_VIEW_DESC desc;
									srv->GetDesc(&desc);
									if (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
									{
										ImGui::Image(srv, ImVec2(64, 64));
									}
								}
							}

							ImGui::PopTextWrapPos();
							ImGui::EndTooltip();
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}

					ImGui::EndCombo();
				}
				if (ImGui::IsItemHovered() && m_SelectedAssetIndexMap[idx] != INDEX_NONE)
				{
					ImGui::BeginTooltip();
					AssetData* toolTipTargetAsset = Assets[m_SelectedAssetIndexMap[idx]];
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::Text(toolTipTargetAsset->GetClass()->GetName().c_str());
					std::string tooltipRelPathWithName;
					toolTipTargetAsset->GetRelativePathWithName(OUT tooltipRelPathWithName);
					ImGui::Text(tooltipRelPathWithName.c_str());

					if (toolTipTargetAsset->GetClass()->IsA(TextureAsset::StaticClass()))
					{
						ID3D11ShaderResourceView* srv = ((TextureAsset*)toolTipTargetAsset)->GetResource();
						if (srv)
						{
							D3D11_SHADER_RESOURCE_VIEW_DESC desc;
							srv->GetDesc(&desc);
							if (desc.ViewDimension == D3D11_SRV_DIMENSION_TEXTURE2D)
							{
								ImGui::Image(srv, ImVec2(64, 64));
							}
						}

					}

					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}

				ImGui::SameLine();
				std::string btnLabel = "null" + std::to_string(idx);
				if (ImGui::Button(btnLabel.c_str(), ImVec2(12, 12)))
				{
					m_SelectedAssetIndexMap[idx] = INDEX_NONE;
					(*val)[idx] = nullptr;
				}
			}			
			
		}	
	
	}

	//virtual void InsertArrayElement(int32 idx) override;

	//virtual void DuplicateArrayElement(int32 idx) override;

	//virtual void DeleteArrayElement(int32 idx) override;

	virtual void PostEditProperties() override
	{
		int32 t = 0;
	}

protected:
	TPropertyHandle<std::vector<AssetData*>>* GetArrayHandle() const
	{
		TPropertyHandle<std::vector<AssetData*>>* ret = (TPropertyHandle<std::vector<AssetData*>>*)m_Prop;
		return ret;
	}

	TPropertyHandle<std::vector<TSharedPtr<AssetData>>>* GetSharedArrayPropHandle() const
	{
		TPropertyHandle<std::vector<TSharedPtr<AssetData>>>* ret = (TPropertyHandle<std::vector<TSharedPtr<AssetData>>>*)m_Prop;
		return ret;
	}

protected:
	TPropertyHandle<AssetData*>* GetHandle() const
	{
		if (m_Prop->IsSharedType_())
			return nullptr;

		TPropertyHandle<AssetData*>* ret = (TPropertyHandle<AssetData*>*)m_Prop;

		return ret;		
	}

	TPropertyHandle<TSharedPtr<AssetData>>* GetSharedPtrPropHandle() const
	{
		TPropertyHandle<TSharedPtr<AssetData>>* ret = (TPropertyHandle<TSharedPtr<AssetData>>*)m_Prop;

		return ret;		
	}

	AssetData** GetTargetAsset() const
	{
		AssetData** targetAsset = nullptr;

		bool PropAcc = GetHandle();
		if (PropAcc)
			targetAsset = GetHandle()->GetValuePtr();

		return targetAsset;
	}

	TSharedPtr<AssetData>* GetSharedTargetAsset()
	{
		TSharedPtr<AssetData>* sharedAsset = nullptr;

		auto propHandle = GetSharedPtrPropHandle();
		if (propHandle)
			sharedAsset = propHandle->GetValuePtr();

		return sharedAsset;
	}
protected:
	int32			m_selectedAssetIndex;

	std::unordered_map<int32, int32>	m_SelectedAssetIndexMap;
	//std::vector<int32>	
};

