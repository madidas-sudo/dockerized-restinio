#include <iostream>
#include <string>

#include <restinio/all.hpp>
#include <json_dto/pub.hpp>

struct Place {
	Place() = default;

	Place(
		std::string place_name,
		double lat,
		double lon )
		: m_place_name( place_name ),
		m_lat( lat ),
		m_lon( lon )
	{}

	std::string m_place_name;
	double m_lat;
	double m_lon;
};

struct WeatherReg_t {

	WeatherReg_t() = default;

	WeatherReg_t(
		uint16_t id,
		std::string date,
		std::string time,
		Place place,
		float temp,
		uint8_t hum )
		: m_id( id ),
		m_date( date ),
		m_time( time ),
		m_place( place ),
		m_temp( temp ),
		m_hum( hum )
	{}

	uint16_t m_id;
	std::string m_date;
	std::string m_time;
	Place m_place;
	float m_temp;
	uint8_t m_hum;
};

using WeatherReg_collection_t = std::vector< WeatherReg_t >;

namespace rr = restinio::router;
using router_t = rr::express_router_t<>;

class WeatherReg_handler_t
{
public:
	explicit WeatherReg_handler_t( WeatherReg_collection_t& weather_regs )
		: m_WeatherRegs( weather_regs )
	{}

	WeatherReg_handler_t( const WeatherReg_handler_t& ) = delete;
	WeatherReg_handler_t( WeatherReg_handler_t&& ) = delete;

	auto on_WeatherRegs_list(
		const restinio::request_handle_t& req, rr::route_params_t ) const
	{
		auto resp = init_resp( req->create_response() );

		resp.set_body(
			"WeatherRegs collection (WeatherReg count: " +
			std::to_string( m_WeatherRegs.size() ) + ")\n" );

		for ( std::size_t i = 0; i < m_WeatherRegs.size(); ++i )
		{
			resp.append_body( std::to_string( i + 1 ) + ". " );
			const auto& w = m_WeatherRegs[ i ];
			resp.append_body( w.m_time + " " + w.m_date + "\n" );
		}

		return resp.done();
	}

	auto on_WeatherReg_get(
		const restinio::request_handle_t& req, rr::route_params_t params )
	{
		const auto WeatherRegnum = restinio::cast_to< std::uint32_t >( params[ "WeatherRegnum" ] );

		auto resp = init_resp( req->create_response() );

		if ( 0 != WeatherRegnum && WeatherRegnum <= m_WeatherRegs.size() )
		{
			const auto& w = m_WeatherRegs[ WeatherRegnum - 1 ];
			resp.set_body(
				"WeatherReg #" + std::to_string( WeatherRegnum ) + " is: " +
				w.m_time + " " + w.m_date + "\n" );
		}
		else
		{
			resp.set_body(
				"No WeatherReg with #" + std::to_string( WeatherRegnum ) + "\n" );
		}

		return resp.done();
	}
	/*
		auto on_author_get(
			const restinio::request_handle_t& req, rr::route_params_t params )
		{
			auto resp = init_resp( req->create_response() );
			try
			{
				auto author = restinio::utils::unescape_percent_encoding( params[ "author" ] );

				resp.set_body( "Books of " + author + ":\n" );

				for( std::size_t i = 0; i < m_WeatherRegs.size(); ++i )
				{
					const auto & w = m_WeatherRegs[ i ];
					if( author == w.m_author )
					{
						resp.append_body( std::to_string( i + 1 ) + ". " );
						resp.append_body( w.m_title + "[" + w.m_author + "]\n" );
					}
				}
			}
			catch( const std::exception & )
			{
				mark_as_bad_request( resp );
			}

			return resp.done();
		}
	*/

	auto on_new_WeatherReg(
		const restinio::request_handle_t& req, rr::route_params_t )
	{
		auto resp = init_resp( req->create_response() );

		try
		{
			m_WeatherRegs.emplace_back(
				json_dto::from_json< WeatherReg_t >( req->body() ) );
		}
		catch ( const std::exception& /*ex*/ )
		{
			mark_as_bad_request( resp );
		}

		return resp.done();
	}

	auto on_WeatherReg_update(
		const restinio::request_handle_t& req, rr::route_params_t params )
	{
		const auto WeatherRegnum = restinio::cast_to< std::uint32_t >( params[ "WeatherRegnum" ] );

		auto resp = init_resp( req->create_response() );

		try
		{
			auto w = json_dto::from_json< WeatherReg_t >( req->body() );

			if ( 0 != WeatherRegnum && WeatherRegnum <= m_WeatherRegs.size() )
			{
				m_WeatherRegs[ WeatherRegnum - 1 ] = w;
			}
			else
			{
				mark_as_bad_request( resp );
				resp.set_body( "No book with #" + std::to_string( WeatherRegnum ) + "\n" );
			}
		}
		catch ( const std::exception& /*ex*/ )
		{
			mark_as_bad_request( resp );
		}

		return resp.done();
	}

	auto on_WeatherReg_delete(
		const restinio::request_handle_t& req, rr::route_params_t params )
	{
		const auto WeatherRegnum = restinio::cast_to< std::uint32_t >( params[ "WeatherRegnum" ] );

		auto resp = init_resp( req->create_response() );

		if ( 0 != WeatherRegnum && WeatherRegnum <= m_WeatherRegs.size() )
		{
			const auto& w = m_WeatherRegs[ WeatherRegnum - 1 ];
			resp.set_body(
				"Delete WheaterReg #" + std::to_string( WeatherRegnum ) + ": " +
				w.m_title + "[" + w.m_author + "]\n" );

			m_WeatherRegs.erase( m_WeatherRegs.begin() + ( WeatherRegnum - 1 ) );
		}
		else
		{
			resp.set_body(
				"No book with #" + std::to_string( WeatherRegnum ) + "\n" );
		}

		return resp.done();
	}

private:
	WeatherReg_collection_t& m_WeatherRegs;

	template < typename RESP >
	static RESP
		init_resp( RESP resp )
	{
		resp
			.append_header( "Server", "RESTinio sample server /v.0.6" )
			.append_header_date_field()
			.append_header( "Content-Type", "text/plain; charset=utf-8" );

		return resp;
	}

	template < typename RESP >
	static void
		mark_as_bad_request( RESP& resp )
	{
		resp.header().status_line( restinio::status_bad_request() );
	}
};

auto server_handler( WeatherReg_collection_t& WeatherReg_collection )
{
	auto router = std::make_unique< router_t >();
	auto handler = std::make_shared< WeatherReg_handler_t >( std::ref( WeatherReg_collection ) );

	auto by = [ & ]( auto method ) {
		using namespace std::placeholders;
		return std::bind( method, handler, _1, _2 );
	};

	auto method_not_allowed = []( const auto& req, auto ) {
		return req->create_response( restinio::status_method_not_allowed() )
			.connection_close()
			.done();
	};

	// Handlers for '/' path.
	router->http_get( "/", by( &WeatherReg_handler_t::on_WeatherRegs_list ) );
	router->http_post( "/", by( &WeatherReg_handler_t::on_new_WeatherReg ) );

	// Disable all other methods for '/'.
	router->add_handler(
		restinio::router::none_of_methods(
			restinio::http_method_get(), restinio::http_method_post() ),
		"/", method_not_allowed );

	// Handler for '/author/:author' path.
	// router->http_get( "/author/:author", by( &WeatherReg_handler_t::on_author_get ) );

	// Disable all other methods for '/author/:author'.
	//router->add_handler(
	//		restinio::router::none_of_methods( restinio::http_method_get() ),
	//		"/author/:author", method_not_allowed );

	// Handlers for '/:WeatherRegnum' path.
	router->http_get(
		R"(/:WeatherRegnum(\d+))",
		by( &WeatherReg_handler_t::on_WeatherReg_get ) );
	router->http_put(
		R"(/:WeatherRegnum(\d+))",
		by( &WeatherReg_handler_t::on_WeatherReg_update ) );
	router->http_delete(
		R"(/:WeatherRegnum(\d+))",
		by( &WeatherReg_handler_t::on_WeatherReg_delete ) );

	// Disable all other methods for '/:WeatherRegnum'.
	router->add_handler(
		restinio::router::none_of_methods(
			restinio::http_method_get(),
			restinio::http_method_post(),
			restinio::http_method_delete() ),
		R"(/:WeatherRegnum(\d+))", method_not_allowed );

	return router;
}

int main()
{
	using namespace std::chrono;

	try
	{
		using traits_t =
			restinio::traits_t<
			restinio::asio_timer_manager_t,
			restinio::single_threaded_ostream_logger_t,
			router_t >;

		// WeatherReg_collection_t WeatherReg_collection{
		// 	{ "Agatha Christie", "Murder on the Orient Express" },
		// 	{ "Agatha Christie", "Sleeping Murder" },
		// 	{ "B. Stroustrup", "The C++ Programming Language" }
		// };

		WeatherReg_collection_t WeatherReg_collection{
			{0, "20211105", "12:15", {"Aarhus N", 13.692, 19.438, }, 13.1, 70}
		};

		restinio::run(
			restinio::on_this_thread< traits_t >()
			.address( "localhost" )
			.request_handler( server_handler( WeatherReg_collection ) )
			.read_next_http_message_timelimit( 10s )
			.write_http_response_timelimit( 1s )
			.handle_request_timeout( 1s ) );
	}
	catch ( const std::exception& ex )
	{
		std::cerr << "Error: " << ex.what() << std::endl;
		return 1;
	}

	return 0;
}
