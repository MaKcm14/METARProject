#pragma once

#ifndef WEATHER_GETTER

#    include <iostream>
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


	std::string GetWeather(double lat, double lon) {
		std::string result;
		std::string pathInterm = "data/2.5/weather?lat=";
		{
			std::ostringstream soutLat, soutLon;
			soutLat << lat;
			soutLon << lon;

			pathInterm += soutLat.str();
			pathInterm += "&lon=";
			pathInterm += soutLon.str();
			pathInterm += "&appid=55fd63b9fc0df56901b0ae195b65c16b";
		}



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

	std::string GetCoordinates(std::string icao) {
		std::string result;
		std::string pathInterm = "1.x/?apikey=8a5e90fe-b6bf-4e41-9c40-2aed4d7cfdec&geocode=";
		pathInterm += icao;
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

}

#endif

#define WEATHER_GETTER
