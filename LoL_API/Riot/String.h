#pragma once

template<class T> struct extract_type { typedef T type; };
#define WRAP_CALL(_NAME, _RET, _ARGS, _ADDR) \
template<class... T> _RET _NAME(T... args)\
{\
	typedef typename extract_type<_RET(T...)>::type _function_type;\
	static_assert(std::is_same<_function_type, _RET _ARGS>::value, "Invalid arguments in " #_NAME ", expected " #_ARGS);\
	typedef typename extract_type<_RET(__thiscall*)(void*, T...)>::type _pointer_type;\
	static _pointer_type _function_ptr = reinterpret_cast<_pointer_type>(_ADDR);\
	return _function_ptr(static_cast<void*>(this), args...);\
}


namespace lapi {
	namespace Riot
	{
		class String
		{
			static const int INPLACE_BUFFER_SIZE = 16;
		public:
			char m_data[INPLACE_BUFFER_SIZE];
			size_t m_size;
			int m_maxSize;

			String() : m_data{ 0 }, m_size{ 0 }, m_maxSize{ INPLACE_BUFFER_SIZE - 1 } {}

			WRAP_CALL(assign, String&, (char const*, int), _assign)

		public:
			static Address _assign;
		};
	}
};