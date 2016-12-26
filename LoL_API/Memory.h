#pragma once
#include "stdafx.h"
#include <deque>

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
	
	class Memory
	{
	public:
		typedef BYTE* Address;
		Memory(std::wstring const& moduleName);
		~Memory();

		Address findSignature(Signature const& sig) const;
		template<typename T> T findSignature(Signature const& sig) const;

		void beginTransaction();
		bool detourAddress(Address* originalFunction, Address const& newFunction);
		template<typename T> bool detourAddress(T* originalFunction, T newFunction);
		void commit();

		void resume();

		template<typename T> static Address GetAddress(T v);
		static Address BaseAddress;
	
	private:
		bool m_inTransaction;
		static bool getModule(std::wstring const& moduleName, MODULEENTRY32W& module);
		MODULEENTRY32W m_module;

		struct Detour {
			Address* originalFunction;
			Address newFunction;
		};
		std::deque<Detour> m_detours;
		std::deque<Detour> m_uncommitedDetours;
	};

	template <typename T>
	T Memory::findSignature(Signature const& sig) const
	{
		return reinterpret_cast<T>(findSignature(sig));
	}

	template<class T>
	Memory::Address Memory::GetAddress(T v)
	{
		return reinterpret_cast<Address>(v);
	}

	template<typename T>
	bool Memory::detourAddress(T* originalFunction, T newFunction)
	{
		Memory::Address* originalA = reinterpret_cast<Memory::Address*>(originalFunction);
		Memory::Address newA = reinterpret_cast<Memory::Address>(newFunction);
		return detourAddress(originalA, newA);
	}
}
