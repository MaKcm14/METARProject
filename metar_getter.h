#pragma once

#ifndef METAR_GETTER

#    include "D:\PostgreSql\include\libpq-fe.h"
#    include <iostream>
#    include "packages/nlohmann.json.3.11.2/build/native/include/nlohmann/json.hpp"
#    include <sstream>
#    include <memory>
#    include <vector>
#    include <Windows.h>
#    include <wininet.h>

#    pragma comment(lib, "Ws2_32.lib")
#    pragma comment(lib,"wininet")


namespace NWeather {

	class THttpException : public std::exception {
	public:
		THttpException(std::string description)
			: exception(description.c_str())
		{
		}

	};


	class TApiKeys {
	private:
		std::string CoordinatesApiKey;
		std::string WeatherApiKey;

	public:
		TApiKeys() = default;


		bool InitializeApiKeys(std::string apiKeysFile) {
			std::ifstream finApiKeys(apiKeysFile);

			if (!finApiKeys.is_open()) {
				throw std::runtime_error("file ApiKeys wasn't open");
			}

			std::getline(finApiKeys, WeatherApiKey);
			std::getline(finApiKeys, CoordinatesApiKey);

			return !CoordinatesApiKey.empty() && !WeatherApiKey.empty();
		}

	};


	class TMetar {
	private:
		std::string Icao;
		std::string Metar;
		static TApiKeys ApiKeys;

	public:
		TMetar() = default;
		
		TMetar(std::string icao)
			: Icao(std::move(icao))
		{

		}

		TMetar(const TMetar& metar) 
			: Icao()
			, Metar() 
		{
		}


		void FormMetar();

		std::string GetMetar() const noexcept {
			return Metar;
		}

		static void InitializeApi(std::string apiKeysFile) {
			bool initFlag = TMetar::ApiKeys.InitializeApiKeys(apiKeysFile);
			if (!initFlag) {
				throw std::runtime_error("api variables weren't initialized correctly");
			}
		}

	private:
		std::string GetWeather(double lat, double lon) const;

		std::string GetCoordinates() const;

		void ParseJsonWeather(std::string jsonWeather);

	};

	TApiKeys TMetar::ApiKeys = TApiKeys();

}

#endif