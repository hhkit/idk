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
#include <res/GenericResourceHandle.h>
#include <reflect/reflect.h>

namespace idk {

	class IGE_InspectorWindow :
		public IGE_IWindow
	{
	public:
		IGE_InspectorWindow();

        virtual void Initialize() override;
		virtual void BeginWindow() override;
		virtual void Update() override;
		void Reset();

	private:
        constexpr static float default_item_width_ratio = 0.6f;

        struct DisplayStack
        {
            IGE_InspectorWindow& self;
            float item_width_ratio;
            string curr_prop_path;
            bool has_override = false;

            DisplayStack(IGE_InspectorWindow& self, float item_width_ratio = default_item_width_ratio)
                : self{ self }, item_width_ratio{ item_width_ratio }
            {}
            void GroupBegin();
            void Label(const char* key);
            void ItemBegin(bool align = false);
            void ItemEnd();
            void GroupEnd();
        };


        bool _debug_mode = false;

        Handle<PrefabInstance> _prefab_inst;
        GenericHandle _curr_component;
        int _curr_component_nth;
        bool _curr_component_is_added;
        vector<string> _curr_property_stack;
        // spawn prefab instances in prefab scene so prefab assets can be displayed.
        hash_table<RscHandle<Prefab>, Handle<GameObject>> _prefab_store;

        // for previewing particle systems
        Handle<ParticleSystem> _mocked_ps;

        vector<reflect::dynamic> _original_values;

        reflect::dynamic _copied_component;

        ImGuiTextFilter component_textFilter{};
        ImGuiTextFilter script_textFilter{};

        //If multiple objects are selected, this will only display the first gameObject.
        void DisplayGameObjects(vector<Handle<GameObject>> gos);			
		void DisplayGameObjectHeader(Handle<GameObject> game_object);	    
        void DisplayPrefabInstanceControls(Handle<PrefabInstance> c_prefab);

        //If multiple objects are selected, this will only display the first gameObject.
        void DisplayComponent(GenericHandle component);
        template<typename T> void DisplayComponentInner(Handle<T> component) { DisplayVal(*component); }
        void DisplayComponentInner(Handle<Transform> c_transform);
        void DisplayComponentInner(Handle<RectTransform> c_rt);
        void DisplayComponentInner(Handle<RigidBody> c_rigidbody);
        void DisplayComponentInner(Handle<Animator> c_anim);	
        void DisplayComponentInner(Handle<Bone> c_bone);		
        void DisplayComponentInner(Handle<AudioSource> c_audio);	
		void DisplayComponentInner(Handle<TextMesh> c_font);	
		void DisplayComponentInner(Handle<Text> c_text);	
        void DisplayComponentInner(Handle<ParticleSystem> c_ps);
        void DisplayComponentInner(Handle<ElectronView> c_ev);
        void DisplayComponentInner(Handle<ElectronAnimatorView> c_ev);

		void MenuItem_RemoveComponent(GenericHandle i);
		void MenuItem_CopyComponent(GenericHandle i);
		void MenuItem_PasteComponent();

        void DisplayAsset(GenericResourceHandle handle);

        template<typename Res> void DisplayAsset(RscHandle<Res>) {}
        template<> void DisplayAsset(RscHandle<Prefab> prefab);
        template<> void DisplayAsset(RscHandle<MaterialInstance> material);
        template<> void DisplayAsset(RscHandle<Material> material);
        template<> void DisplayAsset(RscHandle<Texture> texture);
		template<> void DisplayAsset(RscHandle<FontAtlas> fontAtlas);

        // when curr property is key, draws using CustomDrawFn
        using CustomDrawFn = bool(*)(const reflect::dynamic& dyn);
        using InjectDrawTable = hash_table<string_view, CustomDrawFn>;
        bool DisplayVal(reflect::dynamic dyn, InjectDrawTable* inject_draw_table = nullptr);

        template<typename Command,typename ...Args>
        void ExecuteOnSelected(Args&&...);
        void StoreOriginalValues(string_view property_path);
        void ExecuteModify(string_view property_path, reflect::dynamic new_value);
	};





}