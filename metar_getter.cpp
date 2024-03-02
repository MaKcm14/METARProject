#pragma once

#include "metar_getter.h"

#ifndef METAR_GETTER

// Form the METAR:
void NWeather::TMetar::FormMetar() {
	PGconn* connection = PQsetdbLogin("localhost", "5432", 
		nullptr, nullptr, "postgres", "postgres", "WinServerPost");
	auto condition = PQstatus(connection);

	if (condition == CONNECTION_BAD) {
		PQfinish(connection);
		throw std::runtime_error("can't connect to DB: check connection data");
	}


	std::string queryIcaoStr = "SELECT * FROM metar_info\nWHERE icao = '" + Icao + "'";
	auto resultOfQuery = PQexec(connection, queryIcaoStr.c_str());

	if (resultOfQuery == nullptr) {
		PQfinish(connection);
		throw std::logic_error("SQL-query isn't correct");
	}
	std::string metarTime = PQgetvalue(resultOfQuery, 0, 2);


	bool checkMetarTime = true;
	std::time_t curTime = time(0);
	std::tm* locTime = localtime(&curTime);
	
	std::vector<std::string> partsData = { metarTime.substr(0, 4), metarTime.substr(5, 2),
	metarTime.substr(8, 2), metarTime.substr(11, 2), metarTime.substr(14, 2) };

	for (size_t i = 0; i != partsData.size(); ++i) {
		int32_t part;
		std::istringstream sin(partsData[i]);
		
		sin >> part;
		if (!i) {
			if (part < locTime->tm_year) {
				checkMetarTime = false;
				break;
			}
		} else if (i == 1) {
			if (part < locTime->tm_mon) {
				checkMetarTime = false;
				break;
			}
		} else if (i == 2) {
			if (part < locTime->tm_mday) {
				checkMetarTime = false;
				break;
			}
		} else if (i == 3) {
			if (part < locTime->tm_hour) {
				checkMetarTime = false;
				break;
			}
		} else if (i == 4) {
			if ((locTime->tm_min - part) > 30) {
				checkMetarTime = false;
				break;
			}
		}
	}
	
	if (checkMetarTime) {
		Metar = PQgetvalue(resultOfQuery, 0, 1);
		PQfinish(connection);
		return;
	} else {
		ParseJsonWeather(GetData());
	}
	

	curTime = time(0);
	locTime = localtime(&curTime);
	std::ostringstream sout;

	sout << locTime->tm_year + 1900 << "-";
	if (locTime->tm_mon < 10) {
		sout << "0";
	}
	sout << locTime->tm_mon << "-";

	if (locTime->tm_mday < 10) {
		sout << "0";
	}
	sout << locTime->tm_mday << " ";

	if (locTime->tm_hour < 10) {
		sout << "0";
	}
	sout << locTime->tm_hour << ":";
	
	if (locTime->tm_min < 10) {
		sout << "0";
	}
	sout << locTime->tm_min << ":00";
	
	queryIcaoStr = "UPDATE metar_info\nSET metar = '" + Metar + "', metar_time = '" + sout.str() + "'\nWHERE icao = '" + Icao + "'";
	resultOfQuery = PQexec(connection, queryIcaoStr.c_str());

	if (resultOfQuery == nullptr) {
		PQfinish(connection);
		throw std::logic_error("SQL-query isn't correct");
	}


	PQfinish(connection);

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
	std::string jsonCoordsData;
	
	while (true) {
		try {
			jsonCoordsData = GetCoordinates();
			break;
		} catch (NWeather::THttpException& httpExcp) {
			continue;
		}
	}

	auto coordsBegin = jsonCoordsData.find(":", jsonCoordsData.find("\"pos\"")) + 2;
	std::string coordsOfIcao = jsonCoordsData.substr(coordsBegin, 
		jsonCoordsData.find("\"", coordsBegin) - coordsBegin);

	while (true) {
		try {
			return GetWeather(coordsOfIcao.substr(0, coordsOfIcao.find(" ")), coordsOfIcao.substr(coordsOfIcao.find(" ") + 1));
		} catch (NWeather::THttpException& httpExcp) {
			continue;
		}
	}

}

// Define ZULU-time
void NWeather::TMetar::AddZuluTime() {
	std::time_t curTime = time(0);
	std::tm* locTime = gmtime(&curTime);
	std::string dateTimeUTC = asctime(locTime);
	std::vector<std::string> partsTime;

	for (size_t posSpaceCur = dateTimeUTC.find(" "), posSpaceAfterLast = 0; posSpaceCur < dateTimeUTC.size(); ) {
		partsTime.push_back(dateTimeUTC.substr(posSpaceAfterLast, posSpaceCur - posSpaceAfterLast));

		posSpaceAfterLast = posSpaceCur + 1;
		posSpaceCur = dateTimeUTC.find(" ", posSpaceAfterLast);
	}

	Metar += Icao + " ";

	if (partsTime[3].size() == 1) {
		Metar += "0";
	}

	Metar += partsTime[3];

	Metar.push_back(partsTime.back()[0]);
	Metar.push_back(partsTime.back()[1]);
	Metar.push_back(partsTime.back()[3]);
	Metar.push_back(partsTime.back()[4]);

	Metar += "Z ";

}

// Define wind's variables
void NWeather::TMetar::AddWindInfo(const std::string& jsonWeather) {
	size_t windPos = jsonWeather.find("wind") + 7;
	std::string windInfo = jsonWeather.substr(windPos, jsonWeather.find("}", windPos) - windPos);
	std::vector<std::string> windDesc;

	for (size_t posInfoBeg = windInfo.find(":"), posInfoEnd = windInfo.find(","); posInfoBeg < windInfo.size(); ) {
		std::string windInfoInterm = windInfo.substr(posInfoBeg + 1, posInfoEnd - posInfoBeg - 1);

		windDesc.push_back(windInfoInterm.substr(0, windInfoInterm.find(".")));

		posInfoBeg = windInfo.find(":", posInfoBeg + 1);
		posInfoEnd = windInfo.find(",", posInfoEnd + 1);
	}

	Metar += windDesc[1];
	if (windDesc[0].size() == 1) {
		Metar += "0";
	}
	Metar += windDesc[0];

	if (windDesc.back().size() >= 2) {
		Metar += "G" + windDesc.back();
	}

	Metar += "MPS ";

}

// Define visibility
void NWeather::TMetar::AddVisibility(const std::string& jsonWeather) {
	size_t visibPos = jsonWeather.find("visibility") + 12;
	std::string visibInfo = jsonWeather.substr(visibPos, jsonWeather.find(",", visibPos) - visibPos);

	if (visibInfo.size() == 5) {
		visibInfo = "9999";
	}

	Metar += visibInfo + " ";

}

// Define weather scene and clouds
void NWeather::TMetar::AddWeatherScene(const std::string& jsonWeather) {
	size_t weatherIdPos = jsonWeather.find("id") + 4;
	std::string weatherCode = jsonWeather.substr(weatherIdPos, 3);
	const std::vector<std::vector<std::string>> weatherCodesInIcaoCodes = {
		{"SQ", "771"}, {"PO", "731"}, {"FC", "781"}, {"BR", "721"},
		{"HZ", "701"}, {"FU", "711"}, {"DU", "761"}, {"SS", "751"},
		{"BLSN", "620", "622"}, {"FG", "741"}, {"RA", "500", "501", "502", "503", "504"},
		{"SN", "600", "601", "602", "611", "612", "613"}, {"RASN", "615", "616"},
		{"SHSN", "620", "621", "622"}, {"SHRA", "520", "521", "522", "531"},
		{"DZ", "3XX"}, {"PE", "511"}, {"TS", "221"}, {"TSRA", "2XX"}, {"CAVOK", "800"}
	};

	bool flagCodeFind = false;
	bool flagCavok = false;
	for (const auto& codes : weatherCodesInIcaoCodes) {
		for (const auto& code : codes) {
			if (weatherCode == code || weatherCode.front() == '3' && code == "3XX" ||
				weatherCode.front() == '2' && code == "2XX")
			{
				if (weatherCode == "800") {
					flagCavok = true;
				}

				Metar += codes.front() + " ";
				flagCodeFind = true;
				break;

			}
		}

		if (flagCodeFind) {
			break;
		}
	}


	/// Define clouds:
	if (!flagCavok) {
		if (weatherCode == "801") {
			Metar += "SKS";
		} else if (weatherCode == "802") {
			Metar += "FEW";
		} else if (weatherCode == "803") {
			Metar += "SKT";
		} else if (weatherCode == "804") {
			Metar += "OVC";
		} else {
			Metar += "BKN";
		}
	}

	Metar += " ";

}

// Define temperature (without dew point)
void NWeather::TMetar::AddTemperature(const std::string& jsonWeather) {
	size_t tempInfoPos = jsonWeather.find("temp") + 6;
	int32_t temp;
	std::istringstream sinTemp(jsonWeather.substr(tempInfoPos, jsonWeather.find(".", tempInfoPos) - tempInfoPos));

	sinTemp >> temp;
	temp = 273 - temp;

	std::ostringstream soutTemp;

	soutTemp << std::abs(temp);
	if (temp < 0) {
		Metar += "M";
	}

	Metar += soutTemp.str() + " ";

}

// Define pressure
void NWeather::TMetar::AddPressure(const std::string& jsonWeather) {
	size_t pressPos = jsonWeather.find("pressure") + 10;
	std::string pressInfo = jsonWeather.substr(pressPos, jsonWeather.find(",", pressPos) - pressPos);

	Metar += "Q" + pressInfo;

}

void NWeather::TMetar::ParseJsonWeather(std::string jsonWeather) {
	AddZuluTime();

	AddWindInfo(jsonWeather);

	AddVisibility(jsonWeather);

	AddWeatherScene(jsonWeather);
	
	AddTemperature(jsonWeather);

	AddPressure(jsonWeather);

}

#endif


#define METAR_GETTER