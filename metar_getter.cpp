#pragma once

#include "metar_getter.h"

#ifndef METAR_GETTER

void NWeather::TMetar::FormMetar() {
	// TODO: 1. set connection with DB;
	// TODO: 2. get data;
	// TODO: 3. check data;
	// TODO: 4. write METAR or request weather data; (use GetWeather and GetCoordinates);
	// TODO: 5*. parse (ParseJsonWeather) and form METAR;
	// TODO: 6*. write in DB result if data was updated;
	ParseJsonWeather(GetData());

}

std::string NWeather::TMetar::GetWeather(std::string lat, std::string lon) const {
	std::string result;
	std::string pathInterm = "data/2.5/weather?lat=";
	pathInterm += lat;
	pathInterm += "&lon=";
	pathInterm += lon;
	pathInterm += "&appid=";
	pathInterm += ApiKeys.GetWeatherApiKey();



	std::unique_ptr<wchar_t> path(new wchar_t[pathInterm.size() + 1]);

	for (size_t i = 0; i != pathInterm.size(); ++i) {
		path.get()[i] = pathInterm[i];
	}
	path.get()[pathInterm.size()] = '\0';



	HINTERNET hintDesriptor = ::InternetOpen(TEXT("LocalGetter"), INTERNET_OPEN_TYPE_PRECONFIG, \
		NULL, NULL, 0);

	if (hintDesriptor) {
		HINTERNET hintConnection = ::InternetConnect(hintDesriptor, TEXT("api.openweathermap.org"), \
			INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);

		if (hintConnection) {
			LPCWSTR lpczAcceptTypes[] = { TEXT("text/*"), NULL };
			HINTERNET hintOpenHttp = ::HttpOpenRequest(hintConnection, NULL, path.get(),
				NULL, NULL, lpczAcceptTypes, INTERNET_FLAG_SECURE, 1);

			if (hintOpenHttp) {
				BOOL httpRequest = ::HttpSendRequest(hintOpenHttp, NULL, NULL, NULL, 0);

				if (httpRequest) {
					for (char symbol; true; ) {
						DWORD dRead;
						BOOL read = ::InternetReadFile(hintOpenHttp, &symbol, 1, &dRead);

						if (read == FALSE || !dRead) {
							break;
						} else {
							result.push_back(symbol);
						}
					}
				} else {
					throw THttpException("::HttpSendRequest: problem");
				}

				::InternetCloseHandle(hintOpenHttp);
			} else {
				throw THttpException("::HttpOpenRequest: problem");
			}

			::InternetCloseHandle(hintConnection);
		} else {
			throw THttpException("::InternetConnect: problem");
		}

		::InternetCloseHandle(hintDesriptor);
	} else {
		throw THttpException("::InternetOpen: problem");
	}
	return result;
}

std::string NWeather::TMetar::GetCoordinates() const {
	std::string result;
	std::string pathInterm = "1.x/?apikey=";
	pathInterm += ApiKeys.GetCoordinatesApiKey();
	pathInterm += "&geocode=";
	pathInterm += Icao;
	pathInterm += "&format=json";



	std::unique_ptr<wchar_t> path(new wchar_t[pathInterm.size() + 1]);

	for (size_t i = 0; i != pathInterm.size(); ++i) {
		path.get()[i] = pathInterm[i];
	}
	path.get()[pathInterm.size()] = '\0';



	HINTERNET hintDesriptor = ::InternetOpen(TEXT("LocalGetter"), INTERNET_OPEN_TYPE_PRECONFIG, \
		NULL, NULL, 0);

	if (hintDesriptor) {
		HINTERNET hintConnection = ::InternetConnect(hintDesriptor, TEXT("geocode-maps.yandex.ru"), \
			INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, NULL, NULL);

		if (hintConnection) {
			LPCWSTR lpczAcceptTypes[] = { TEXT("text/*"), NULL };
			HINTERNET hintOpenHttp = ::HttpOpenRequest(hintConnection, NULL, path.get(),
				NULL, NULL, lpczAcceptTypes, INTERNET_FLAG_SECURE, 1);

			if (hintOpenHttp) {
				BOOL httpRequest = ::HttpSendRequest(hintOpenHttp, NULL, NULL, NULL, 0);

				if (httpRequest) {
					for (char symbol; true; ) {
						DWORD dRead;
						BOOL read = ::InternetReadFile(hintOpenHttp, &symbol, 1, &dRead);

						if (read == FALSE || !dRead) {
							break;
						} else {
							result.push_back(symbol);
						}
					}
				} else {
					throw THttpException("::HttpSendRequest: problem");
				}

				::InternetCloseHandle(hintOpenHttp);
			} else {
				throw THttpException("::HttpOpenRequest: problem");
			}

			::InternetCloseHandle(hintConnection);
		} else {
			throw THttpException("::InternetConnect: problem");
		}

		::InternetCloseHandle(hintDesriptor);
	} else {
		throw THttpException("::InternetOpen: problem");
	}
	return result;
}

std::string NWeather::TMetar::GetData() const {
	std::string jsonCoordsData = GetCoordinates();

	auto coordsBegin = jsonCoordsData.find(":", jsonCoordsData.find("\"pos\"")) + 2;
	std::string coordsOfIcao = jsonCoordsData.substr(coordsBegin, 
		jsonCoordsData.find("\"", coordsBegin) - coordsBegin);

	return GetWeather(coordsOfIcao.substr(0, coordsOfIcao.find(" ")), coordsOfIcao.substr(coordsOfIcao.find(" ") + 1));
}

void NWeather::TMetar::ParseJsonWeather(std::string jsonWeather) {

}

#endif


#define METAR_GETTER