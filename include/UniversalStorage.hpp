/*

██╗░░░██╗███╗░░██╗██╗██╗░░░██╗███████╗██████╗░░██████╗░█████╗░██╗░░░░░
██║░░░██║████╗░██║██║██║░░░██║██╔════╝██╔══██╗██╔════╝██╔══██╗██║░░░░░
██║░░░██║██╔██╗██║██║╚██╗░██╔╝█████╗░░██████╔╝╚█████╗░███████║██║░░░░░
██║░░░██║██║╚████║██║░╚████╔╝░██╔══╝░░██╔══██╗░╚═══██╗██╔══██║██║░░░░░
╚██████╔╝██║░╚███║██║░░╚██╔╝░░███████╗██║░░██║██████╔╝██║░░██║███████╗
░╚═════╝░╚═╝░░╚══╝╚═╝░░░╚═╝░░░╚══════╝╚═╝░░╚═╝╚═════╝░╚═╝░░╚═╝╚══════╝
		░██████╗████████╗░█████╗░██████╗░░█████╗░░██████╗░███████╗
		██╔════╝╚══██╔══╝██╔══██╗██╔══██╗██╔══██╗██╔════╝░██╔════╝
		╚█████╗░░░░██║░░░██║░░██║██████╔╝███████║██║░░██╗░█████╗░░
		░╚═══██╗░░░██║░░░██║░░██║██╔══██╗██╔══██║██║░░╚██╗██╔══╝░░
		██████╔╝░░░██║░░░╚█████╔╝██║░░██║██║░░██║╚██████╔╝███████╗
		╚═════╝░░░░╚═╝░░░░╚════╝░╚═╝░░╚═╝╚═╝░░╚═╝░╚═════╝░╚══════╝

Header-only C++20 simple and fast universal container https://github.com/3lyrion/UniversalStorage

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2024 3lyrion

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files( the "Software" ), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

namespace internal
{
	class BaseValueHolder
	{
	public:
		virtual ~BaseValueHolder() = default;

	protected:
		BaseValueHolder() = default;
	};

	template <typename T>
	class ValueHolder : public BaseValueHolder
	{
	public:
		ValueHolder() = default;
		ValueHolder(T const& theValue) : m_value(theValue) { }
		ValueHolder(auto&&... args)    : m_value(std::forward<decltype(args)>(args)...) { }

		T* getValue()
		{
			return static_cast<T*>(std::addressof(m_value));
		}

	private:
		T m_value;
	};

} // namespace internal

class UniversalStorage
{
public:
	UniversalStorage() = default;

    template <typename T>
	constexpr void add(std::string const& name, T const& element)
	{
		m_elements.emplace(name, std::make_unique<internal::ValueHolder<T> >(element));
	}
	
	template <typename T>
	constexpr T& emplace(std::string const& name, auto&&... args)
	{
		return *static_cast<T*>(
			static_cast<internal::ValueHolder<T>*>(
				m_elements.emplace(name, std::make_unique<internal::ValueHolder<T> >
				(std::forward<decltype(args)>(args)...)).first->second.get()
			)->getValue()
		);
	}

	inline void remove(std::string const& name)
	{
		auto entry = m_elements.find(name);

		if (entry != m_elements.end())
			m_elements.erase(entry);
	}

	inline void clear()
	{
		m_elements.clear();
	}

	template <typename T>
	constexpr T* get(std::string const& name) const
	{
		auto entry = m_elements.find(name);

		if (entry != m_elements.end())
			return static_cast<internal::ValueHolder<T>*>(entry->second.get())->getValue();

		return nullptr;
	}

private:
	using Element = std::unique_ptr<internal::BaseValueHolder>;

	std::unordered_map<std::string, Element> m_elements;
};