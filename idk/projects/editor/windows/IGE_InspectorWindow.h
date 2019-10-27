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

        Handle<PrefabInstance> _prefab_inst;
        GenericHandle _prefab_curr_component;
        int _prefab_curr_component_nth;
        vector<string> _curr_property_stack;

        //If multiple objects are selected, this will only display the first gameObject.
        void DisplayGameObjects(vector<Handle<GameObject>> gos);			
		void DisplayGameObjectHeader(Handle<GameObject> game_object);	    
        void DisplayPrefabInstanceControls(Handle<PrefabInstance> c_prefab);

        //If multiple objects are selected, this will only display the first gameObject.
        void DisplayComponent(GenericHandle& component);
        template<typename T> void DisplayComponentInner(T component) { displayVal(component); }
        template<> void DisplayComponentInner(Handle<Transform> c_transform);
        template<> void DisplayComponentInner(Handle<Animator> c_anim);	
        template<> void DisplayComponentInner(Handle<Bone> c_anim);		
        template<> void DisplayComponentInner(Handle<AudioSource> c_anim);		

		void MenuItem_RemoveComponent(GenericHandle i);
		void MenuItem_CopyComponent(GenericHandle i);
		void MenuItem_PasteComponent();

        void DisplayAsset(GenericResourceHandle handle);

        template<typename Res> void DisplayAsset(RscHandle<Res>) {}
        template<> void DisplayAsset(RscHandle<Prefab> prefab);
        template<> void DisplayAsset(RscHandle<MaterialInstance> material);
        template<> void DisplayAsset(RscHandle<Material> material);
        template<> void DisplayAsset(RscHandle<Texture> texture);

        bool displayVal(reflect::dynamic dyn);

        constexpr static float item_width_ratio = 0.6f;

		//For when transforms are edited
		bool			hasChanged = false; 
		void			TransformModifiedCheck();

	};





}