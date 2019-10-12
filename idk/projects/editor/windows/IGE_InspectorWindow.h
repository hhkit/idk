//////////////////////////////////////////////////////////////////////////////////
//@file		IGE_InspectorWindow.h
//@author	Muhammad Izha B Rahim
//@param	Email : izha95\@hotmail.com
//@date		16 SEPT 2019
//@brief	

/*
This window displays the editor window where you can select and modify gameobjects.
*/
//////////////////////////////////////////////////////////////////////////////////



#pragma once
#include <editor/windows/IGE_IWindow.h>
#include <IncludeComponents.h>

namespace idk {

	class IGE_InspectorWindow :
		public IGE_IWindow
	{
	public:
		IGE_InspectorWindow();

        virtual void Initialize() override;
		virtual void BeginWindow() override;
		virtual void Update() override;

	
	protected:


	private:
        GenericResourceHandle _displayed_asset{ RscHandle<Texture>() }; // if false, show gameobject(s)

		bool isComponentMarkedForDeletion = false;
		string componentNameMarkedForDeletion{}; //Is empty by default

        Handle<PrefabInstance> _prefab_inst;
        GenericHandle _prefab_curr_component;
        vector<string> _curr_property_stack;

        void DisplayGameObjects(vector<Handle<GameObject>> gos);			   //If multiple objects are selected, this will only display the first gameObject.
		void DisplayGameObjectHeader(Handle<GameObject> game_object);	       //If multiple objects are selected, this will only display the first gameObject.
        void DisplayPrefabInstanceControls(Handle<PrefabInstance> c_prefab);   //If multiple objects are selected, this will only display the first gameObject.
		void DisplayTransformComponent(Handle<Transform>& c_transform);		   //If multiple objects are selected, this will only display the first gameObject.
		void DisplayAnimatorComponent(Handle<Animator>& c_anim);			   //If multiple objects are selected, this will only display the first gameObject.
		void DisplayOtherComponent(GenericHandle& component);				   //If multiple objects are selected, this will only display the first gameObject.

		void MenuItem_RemoveComponent(GenericHandle i);
		void MenuItem_CopyComponent(GenericHandle i);
		void MenuItem_PasteComponent();


        void DisplayAsset(GenericResourceHandle handle);
        void DisplayAsset(RscHandle<Prefab> prefab);
        void DisplayAsset(RscHandle<MaterialInstance> material);
        void DisplayAsset(RscHandle<Material> material);

        bool displayVal(reflect::dynamic dyn);

		//Variables for vec3/vec4
		const float heightOffset = 2;
		const float widthOffset = 80;
		const float float3Size = 0.33f;
		const float float4Size = 0.25f;
		const float itemSpacing = 50;
		const float XYZSliderWidth = 10;
        constexpr static float item_width_ratio = 0.6f;

		//For when transforms are edited
		bool			hasChanged = false; 
		void			TransformModifiedCheck();

	};





}