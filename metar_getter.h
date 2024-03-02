#pragma once

#ifndef METAR_GETTER

#    include <chrono>
#    include <ctime>
#    include "D:\PostgreSql\include\libpq-fe.h"
#    include <iostream>
#    include <memory>
#    include <sstream>
#    include <thread>
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

		std::string GetCoordinatesApiKey() const noexcept {
			return CoordinatesApiKey;
		}

		std::string GetWeatherApiKey() const noexcept {
			return WeatherApiKey;
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
		std::string GetWeather(std::string lat, std::string lon) const;

		std::string GetCoordinates() const;

		std::string GetData() const;

		void AddZuluTime();

		void AddWindInfo(const std::string& jsonWeather);

		void AddVisibility(const std::string& jsonWeather);

		void AddWeatherScene(const std::string& jsonWeather);

		void AddTemperature(const std::string& jsonWeather);

		void AddPressure(const std::string& jsonWeather);

		void ParseJsonWeather(std::string jsonWeather);

	};

	TApiKeys TMetar::ApiKeys = TApiKeys();

}


#endif