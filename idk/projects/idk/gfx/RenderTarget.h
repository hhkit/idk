#pragma once
#include <res/Resource.h>
#include <res/ResourceExtension.h>
#include <res/ResourceMeta.h>
#include <res/SaveableResource.h>
#include <gfx/Texture.h>
#include <gfx/RenderTargetMeta.h>

namespace idk
{
	class RenderTarget
		: public Resource<RenderTarget>
		, public Saveable<RenderTarget, false_type>
	{
	public:
		using Saveable_t = Saveable<RenderTarget, false_type>;
		static constexpr string_view names[] = { "Color","Depth" };
		static constexpr uint32_t    kColorIndex = 0;
		static constexpr uint32_t    kDepthIndex = 1;
		EXTENSION(".rtis");

		// actually renderer-related stuff
		ivec2              size              { 1024, 1024 };
		AntiAliasing       anti_aliasing     { AntiAliasing::None };
		RscHandle<Texture> color_tex         { };
		RscHandle<Texture> depth_tex         { };
		bool               enable_mipmapping { false };

		// debug information
		bool is_world_renderer { false };
		bool render_debug      { false };

		RenderTarget()noexcept = default;
		RenderTarget(RenderTarget&&) noexcept = default;
		RenderTarget& operator=(RenderTarget&&) noexcept = default;
		// non-copiable
		RenderTarget (const RenderTarget&) = delete;
		RenderTarget& operator=(const RenderTarget&) = delete;
		virtual ~RenderTarget();

		float AspectRatio() const noexcept;

		ivec2 Size() const noexcept                  { return size; }
		void  Size(const ivec2& new_size)            { size = new_size; Dirty(); }

		bool  IsWorldRenderer() const noexcept       { return is_world_renderer; }
		void  IsWorldRenderer(bool new_val) noexcept { is_world_renderer = new_val; }

		bool  RenderDebug() const noexcept           { return render_debug; }
		void  RenderDebug(bool new_val) noexcept     { render_debug = new_val; }

		RscHandle<Texture> GetColorBuffer();
		RscHandle<Texture> GetDepthBuffer();
		void SetColorBuffer(RscHandle<Texture>);
		void SetDepthBuffer(RscHandle<Texture>);

		void Finalize(); // Finalizes regardless of _need_finalizing. Sets to false after.
		bool NeedsFinalizing() const noexcept        { return _need_finalizing; }
		void Dirty(); // hide Saveable Dirty

	protected:
		template<typename T, size_t N> struct ArrayRef;
		ArrayRef<RscHandle<Texture>, 2>        Textures();
		ArrayRef<const RscHandle<Texture>, 2>  Textures() const;
		virtual void OnFinalize() {} /// Stuff to do to finalize the framebuffer

	private:
		bool _need_finalizing = true;
	};

	template<typename T, size_t N>
	struct RenderTarget::ArrayRef 
		: std::array<T*, N>
	{
		using base = std::array<T*, N>;
		template<typename itr_t> struct iterator;

		T& operator[](size_t i) const { return *base::operator[](i); }
		void Set(size_t i, T& t)      { base& b = *this; b[i] = &t; }

		auto begin()       { return iterator<typename base::iterator>{ base::begin() }; }
		auto end()         { return iterator<typename base::iterator>{ base::end() }; }
		auto begin() const { return iterator<typename base::const_iterator>{ base::begin() }; }
		auto end()   const { return iterator<typename base::const_iterator>{ base::end() }; }
	};

	template<typename T, size_t N>
	template<typename itr_t>
	struct RenderTarget::ArrayRef<T, N>::iterator
	{
		itr_t itr;

		iterator(itr_t b) : itr{ b } {}
		auto* operator->() const                       { return *itr; }
		auto& operator*()  const                       { return **itr; }
		iterator& operator++()                      { ++itr; return *this; }
		iterator operator++(int)                    { return iterator{itr++}; }
		iterator& operator--()                      { --itr; return *this; }
		iterator operator--(int)                    { return iterator{itr--}; }
		bool operator==(const iterator & rhs) const { return itr == rhs.itr; }
		bool operator!=(const iterator & rhs) const { return itr != rhs.itr; }

	};
}