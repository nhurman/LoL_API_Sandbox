#pragma once
#include "stdafx.h"

namespace lapi
{
	class Signature
	{
		std::string m_pattern;
		std::string m_mask;
	public:
		Signature(std::string const& pattern, std::string const& mask);
		Signature(char const* pattern, char const* mask);

		// Formats:
		// - AA BB CC ?? DD EE
		// - AA BB CC ? DD EE
		// - AABBCCDD??EE
		Signature(char const* pattern);

		std::string const& pattern() const;
		std::string const& mask() const;
		std::string::size_type size() const;
	};

	typedef BYTE* Address;
	
	template<class T>
	Address GetAddress(T v)
	{
		return reinterpret_cast<Address>(v);
	}

	class Memory
	{
	public:
		Memory(std::wstring const& moduleName);
		~Memory();

		Address findSignature(Signature const& sig);
		template<typename T> T findSignature(Signature const& sig);

		void beginTransaction();
		Address detourAddress(Address const& source, Address const& dest);
		bool detourAddress(Address *source, Address const& dest);
		void commit();

		void resume();
	
	private:
		bool m_inTransaction;
		static bool getModule(std::wstring const& moduleName, MODULEENTRY32W& module);

	private:
		MODULEENTRY32W m_module;
	};

	template <typename T>
	T Memory::findSignature(Signature const& sig)
	{
		return reinterpret_cast<T>(findSignature(sig));
	}
}