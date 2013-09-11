//
// response.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2009, GuangZhu Wu  <guangzhuwu@gmail.com>
//
//This program is free software; you can redistribute it and/or modify it 
//under the terms of the GNU General Public License or any later version.
//
//This program is distributed in the hope that it will be useful, but 
//WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
//or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
//for more details.
//
//You should have received a copy of the GNU General Public License along 
//with this program; if not, contact <guangzhuwu@gmail.com>.
//


#ifndef HttpResponse_h__
#define HttpResponse_h__

#include "p2engine/http/header.hpp"
#include "p2engine/uri.hpp"

namespace p2engine { namespace http{

	class  response
		:public header
	{
	public:
		response();
		virtual ~response();

		virtual void clear();

		//����������ȡ״̬
		/// 100 Continue ������.
		virtual int parse(const char* begin, std::size_t len);
		virtual int serialize(std::ostream& ostr) const;
		virtual int serialize(std::string& str) const;

		status_type status() const;
		void status(status_type s);

		bool range_supported();

		void content_range(int64_t from,int64_t to, int64_t total);
		std::pair<int64_t,int64_t> content_range()const;

		static const std::string& reason_for_status(status_type s);

	private:
		enum Limits
		{
			MAX_VERSION_LENGTH = 8,//HTTP/1.1
			MAX_STATUS_LENGTH  = 3,
			MAX_REASON_LENGTH  = 512
		};

		int m_statusForParser;
		status_type m_status;
		std::string m_reason;
	};


	//
	// inlines
	//
	inline bool response::range_supported()
	{
		return version()==HTTP_VERSION_1_1
			&&has(HTTP_ATOM_Content_Length)
			&&(has(HTTP_ATOM_ETag) ||has(HTTP_ATOM_Last_Modified)) 
			&&get(HTTP_ATOM_Accept_Ranges).find("bytes")!=std::string::npos;
	}

	inline response::status_type response::status() const
	{
		return m_status;
	}

	inline void response::status(status_type s ) 
	{
		m_status=s;
	}

}

template<>
inline safe_buffer_io& operator << (safe_buffer_io&io, const http::response& obj) 
{ 
	((const http::header*)&obj)->serialize(io);
	return io;
}

template<>
inline safe_buffer_io& operator >> (safe_buffer_io&io,  http::response& obj) 
{ 
	((http::header*)&obj)->parse(io);
	return io;
}

}
#endif // HttpResponse_h__

