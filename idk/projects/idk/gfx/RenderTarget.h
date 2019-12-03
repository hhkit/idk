#pragma once
#include <res/Resource.h>
#include <res/ResourceMeta.h>
#include <gfx/Texture.h>

namespace idk
{
	struct RenderTargetMeta
	{
		ivec2 size {1024, 1024};
		std::array<RscHandle<Texture>,2> textures;
		bool is_world_renderer = true;
		bool render_debug = true;
		//RscHandle<Texture> depth_buffer;
	};

	class RenderTarget
		: public Resource<RenderTarget>
		, public Saveable<RenderTarget, false_type>
	{
	public:
		using Saveable_t = Saveable<RenderTarget, false_type>;
		RenderTarget()noexcept = default;
		RenderTarget(RenderTarget&&) noexcept = default;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;
		RenderTarget (const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;

		static constexpr uint32_t kColorIndex = 0;
		static constexpr uint32_t kDepthIndex = 1;
		float AspectRatio() const noexcept;
		ivec2 Size()const noexcept { return size; }
		void Size(const ivec2& new_size) { size = new_size; Dirty(); }

		bool IsWorldRenderer()const noexcept { return is_world_renderer; }
		bool RenderDebug()const noexcept { return render_debug; }
		void IsWorldRenderer(bool new_val)noexcept { is_world_renderer = new_val; }
		void RenderDebug(bool new_val)noexcept { render_debug = new_val; }

		RscHandle<Texture> GetColorBuffer();
		RscHandle<Texture> GetDepthBuffer();
		void SetColorBuffer(RscHandle<Texture>);
		void SetDepthBuffer(RscHandle<Texture>);

		void Finalize(); //Finalizes regardless of _need_finalizing. Sets to false after.
		bool NeedsFinalizing()const noexcept { return _need_finalizing; }
		
		static constexpr char ext[] = ".rtis";

		virtual ~RenderTarget();

		ivec2 size{ 1024, 1024 };
		RscHandle<Texture> color_tex;
		RscHandle<Texture> depth_tex;
		inline const static string names[] = {"Color","Depth"};
		bool is_world_renderer = true;
		bool render_debug = true;
		void Dirty(); // hide Saveable Dirty
	protected:
		template<typename Obj>
		struct HandleWrapper
		{
			Obj& ref;
			HandleWrapper(Obj* r=nullptr) :ref{ *r} {}
			HandleWrapper(Obj& r) :ref{ r } {}
			void Set(Obj& new_ref)
			{
				new (this) HandleWrapper{new_ref};
			}
			Obj& operator=(const Obj& r) { return ref = r; }
			operator Obj& ()const { return ref; }
			Obj& operator*() const { return ref; }
			Obj& Get() const { return ref; }
		};

		template<typename T, size_t N>
		struct ArrayRef :std::array<T*,N>
		{
			using base =std::array<T*, N>;
			T& operator[](size_t i)const { return *base::operator[](i); }
			void Set(size_t i, T& t) { 
				base& b= *this;
				b[i] = &t;
			}
			template<typename itr_t>
			struct iterator
			{
				itr_t itr;

				iterator(itr_t b):itr{b}{}
				T* operator->() { return *itr; }
				T& operator*() { return **itr; }
				iterator& operator++()
				{
					++itr;
					return *this;
				}
				iterator operator++(int)
				{
					return itr++;
				}

				iterator& operator--()
				{
					--itr;
					return *this;
				}
				iterator operator--(int)
				{
					return itr--;
				}
				iterator& operator=(const iterator& rhs)
				{
					itr = rhs.itr;
					return *this;
				}
				bool operator==(const iterator& rhs)const
				{
					return itr == rhs.itr;
				}
				bool operator!=(const iterator& rhs)const
				{
					return itr != rhs.itr;
				}

			};
			iterator<typename base::iterator> begin() { return iterator<typename base::iterator>{ base::begin() }; }
			iterator<typename base::iterator> end  () { return iterator<typename base::iterator>{ base::end  () }; }
			iterator<typename base::const_iterator> begin() const { return iterator<typename base::const_iterator>{ base::begin() }; }
			iterator<typename base::const_iterator> end()   const { return iterator<typename base::const_iterator>{ base::end() }; }
		};

		ArrayRef<RscHandle<Texture>, 2>  Textures();
		ArrayRef<const RscHandle<Texture>, 2>  Textures()const;
		virtual void OnFinalize() {}///Stuff to do to finalize the framebuffer
		//virtual void  AddAttachmentImpl([[maybe_unused]]AttachmentType type, [[maybe_unused]] RscHandle<Texture> texture) {};
		//virtual size_t  AddAttachmentImpl(AttachmentType type, uint32_t size_x, uint32_t size_y) = 0;
		//vector<size_t> attachments[AttachmentType::eSizeAT];
	private:
		bool _need_finalizing = true;
	};

}