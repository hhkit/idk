#include <memory>
#include <utility>


namespace idk::monadic
{
	struct success_t{};
	using error_t = std::nullopt_t;

	// a struct stolen from rust
	// lets us define a result, or return an error otherwise
	template<typename Result, typename Error>
	class result final
	{
		static_assert(std::is_default_constructible_v<Error>, "Error must be default constructible");
	public:
		using value_type = Result;
	
		result(); // constructs an empty error object
		
		// construct from types
		// todo: sfinae if Result == Error
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result(const Result&rhs);
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result(Result&& rhs);
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result& operator=(const Result&rhs);
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result& operator=(Result&& rhs);		
		
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result(const Error&rhs);
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result(Error&& rhs);
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result& operator=(const Error&rhs);
		template<class = std::enable_if_t<!std::is_same_v<Result, Error>>> result& operator=(Error&& rhs);
		
		result(const result& rhs);
		result(result&& rhs);
		result& operator=(const result& rhs);
		result& operator=(result&& rhs);
		
		~result();
		
		template<typename ... Args>
		explicit result(success_t, Args&&...);
		template<typename ... Args>
		result(error_t, Args&&...);

		// accessors
		const Result& value() const;
		const Error&  error() const;
		
		// monadic operators
		template<class Fn>              auto map(Fn&& visitor) const;
		template<class Fn>              auto and_then(Fn&& visitor) const;
		template<class Fn, class ErrFn> auto and_then(Fn&& visitor, ErrFn&& err_visitor) const;
		template<class ErrFunc>         auto or_else(ErrFunc&& err_visitor) const;
		
		// modifiers
		template<typename ... Args> Result& emplace(success_t, Args&& ...);
		template<typename ... Args> Result& emplace(error_t, Args&& ...);
		
		// operator overloads 
		explicit operator bool() const;
		Result&  operator*() const;
		Result*  operator->() const;
		
	private:
		using storage_t = std::aligned_storage_t<
			std::max(sizeof(Result), sizeof(Error)),
			std::max(alignof(Result),alignof(Error))
			>;
		storage_t _data{};
		bool      _success = false;
	};
}