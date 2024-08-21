# MetarProject
METAR is a format for reporting weather information. A METAR weather report is predominantly used by aircraft pilots, and by meteorologists, who use aggregated METAR information to assist in weather forecasting.
This application helps get and store info about the weather (METAR) in airports. It can potentially helps some pilots with PPL to make sure that the current weather is the same as they are expecting.
<h3>P.S.</h3>
<h4>Example of the METAR for ICAO UUEE:</h4>

UUEE 161600Z 07006MPS 9999 DRSN SKC M18/M25 Q1028 NOSIG RMK 07420157 57420157
<br>
<hr>
<hr>
<h2>How to install the library?</h2>
Library uses the STL, Winsock, PostgreSQL libraries.
Current project consists only of the sources and headers.
For building you need to use CMake or another system.
<br>
<hr>
<h2>How to use the library?</h2>
The library has several classes that helps to make coverting the weather into the METAR. 

But here we consider *only the class that lets to interact with the clients.*
<h3>NMetarService::TMetarService</h3>
This is the main class that provides the interaction with the client.

<h4>You can use it as in the example:</h4>

```
NMetarService::TMetarService metarService(textFileWithDesiredIcao, textFileWithApiKeys, textOutputMetarFile); // automatically makes the metar
auto metar = metarService.GetMetar(); // returns the metar in std::vector<std::string> for every ICAO in the inputIcaoFile
```
<h4>Peculiarities of using</h4>
1. Parameter textFileWithDesiredIcao must be the directory to the text file where the ICAO are in the order one under the other.
<h5>For example:</h5>

UUEE

UULL

UWKE

UWKD

...

2. Parameter textFileWithApiKeys must be the directory to the text file where specified the API keys for the next services.

The order of the API keys in the file must correspond with the order of the services below. The order must be decorate as the METAR file.
<h5>Services:</h5>

- https://openweathermap.org/api
- https://yandex.ru/maps-api/products/geocoder-api

3. Parameter textOutputMetarFile must be the directory to the text file where METARs will be written after the constructor call.


<br>
<hr>
<h2>P.S.</h2>
<h3>Be carefully</h3>
METAR summary of this application is very approximately and defective and you CAN'T use it as
the right source of information.

The info about the weather was taken from different sources that are not the aviation centres of weather,
due to this METAR is defective.
<br>
<hr>
