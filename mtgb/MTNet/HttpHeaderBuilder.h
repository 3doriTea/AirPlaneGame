#pragma once
#include <sstream>

namespace mtnet
{
	class HttpClient;

	class HttpHeaderBuilder
	{
		friend HttpClient;

	public:
		HttpHeaderBuilder() : 
			oss_{}
		{};

		inline HttpHeaderBuilder& ContentType(const std::string& _content)
		{
			oss_ << "Content-Type: " << _content << std::endl;
			return *this;
		};

		inline HttpHeaderBuilder& Accept(const std::string& _content)
		{
			oss_ << "accept: " << _content << std::endl;
			return *this;
		}

		inline HttpHeaderBuilder& ResponseType(const std::string& _content)
		{
			oss_ << "responseType: " << _content << std::endl;
			return *this;
		}

	private:
		std::ostringstream oss_;
	};
}
