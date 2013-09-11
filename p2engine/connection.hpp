//
// connection.hpp
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

#ifndef p2engine_connection_hpp__
#define p2engine_connection_hpp__

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include "p2engine/basic_engine_object.hpp"
#include "p2engine/variant_endpoint.hpp"
#include "p2engine/basic_dispatcher.hpp"
#include "p2engine/shared_access.hpp"
#include "p2engine/contrib.hpp"
#include "p2engine/time.hpp"

namespace p2engine {

	namespace urdp{
		template<typename BaseConnectionType>
		class basic_urdp_connection;
		template<typename ConnectionType,typename ConnectionBaseType>
		class basic_urdp_acceptor;
	}
	namespace trdp{
		template<typename BaseConnectionType>
		class basic_trdp_connection;
		template<typename ConnectionType,typename ConnectionBaseType>
		class basic_trdp_acceptor; 
	}

	class basic_connection_base
		:public basic_engine_object
		,public fssignal::trackable
	{
		typedef basic_connection_base this_type;
		SHARED_ACCESS_DECLARE;

	public:
		typedef variant_endpoint endpoint_type;
		typedef variant_endpoint endpoint;
		typedef this_type connection_base_type;
		typedef urdp::basic_urdp_connection<connection_base_type> urdp_connection_type;
		typedef trdp::basic_trdp_connection<connection_base_type> trdp_connection_type;
		typedef urdp::basic_urdp_acceptor<urdp_connection_type,connection_base_type> urdp_acceptor_type;
		typedef trdp::basic_trdp_acceptor<trdp_connection_type,connection_base_type> trdp_acceptor_type;

	protected:
		basic_connection_base(io_service& ios,bool realTimeUsage,bool isPassive)
			:basic_engine_object(ios),b_real_time_usage_(realTimeUsage)
			,is_passive_(isPassive){}

	public:
		enum connection_t{
			TCP,//trdp message connection 
			UDP,//urdp message connection
			MIX//mix_rdp message connection
		};
	public:
		virtual error_code open(const endpoint& local_edp, error_code& ec,
			const proxy_settings& ps=proxy_settings()
			)=0;
		virtual void async_connect(const std::string& remote_host, int port, 
			const std::string& domainName, 
			const time_duration& time_out=boost::date_time::pos_infin
			)=0;
		virtual void  async_connect(const endpoint& peer_endpoint,
			const std::string& domainName,
			const time_duration& time_out=boost::date_time::pos_infin
			)=0;

		void async_connect(const std::string& remote_host,
			const std::string& domainName, 
			const time_duration& time_out=boost::date_time::pos_infin
			)
		{
			std::size_t pos=remote_host.rfind(':');
			if (pos==std::string::npos)
				async_connect(remote_host,0,domainName,time_out);
			else
			{
				async_connect(remote_host.substr(0,pos),
					atoi(remote_host.c_str()+pos+1),domainName,time_out
					);
			}
		}

		//reliable send
		virtual void async_send_reliable(const safe_buffer& buf, message_type msgType)=0;
		//unreliable send
		virtual void async_send_unreliable(const safe_buffer& buf, message_type msgType)=0;
		//partial reliable send. message will be send for twice within about 100ms.
		//reliable is not confirmed.
		virtual void async_send_semireliable(const safe_buffer& buf, message_type msgType)=0;

		virtual void keep_async_receiving()=0;
		virtual void block_async_receiving()=0;

		virtual void close(bool greaceful=true)=0;

		virtual bool is_open() const=0;
		virtual bool is_connected() const=0;

		virtual void ping_interval(const time_duration& t)=0;
		virtual time_duration ping_interval()const=0;
		virtual void ping(error_code& ec)=0;

		virtual endpoint local_endpoint(error_code& ec)const=0;
		virtual endpoint remote_endpoint(error_code& ec)const=0;

		virtual connection_t connection_category()const=0;
		virtual const std::string& domain()const=0;

		virtual time_duration rtt() const=0;
		virtual double alive_probability()const=0;
		virtual double local_to_remote_speed()const=0;
		virtual double remote_to_local_speed()const=0;
		virtual double local_to_remote_lost_rate()  const=0;
		virtual double remote_to_local_lost_rate() const=0;

		virtual safe_buffer make_punch_packet(error_code& ec,const endpoint& externalEdp)=0;
		virtual void on_received_punch_request(safe_buffer& buf)=0;

		virtual uint32_t session_id()const=0;

	public:
		bool is_real_time_usage()const
		{
			return b_real_time_usage_;
		}
		bool is_passive()const
		{
			return is_passive_;
		}
	protected:
		//std::string domain_;
		bool b_real_time_usage_;
		bool is_passive_;
	};

	class basic_connection
		:public basic_connection_base
		,public basic_connection_dispatcher<messsage_extractor<message_type> >
	{
		typedef basic_connection this_type;
		SHARED_ACCESS_DECLARE;
	public:
		basic_connection(io_service& ios,bool realTimeUsage,bool isPassive)
			:basic_connection_base(ios,realTimeUsage,isPassive)
		{}
	};

} // namespace p2engine

#endif//basic_urdp_acceptor_h__
