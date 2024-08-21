# MetarProject
Application that helps get and store info about the weather (METAR) in airports (only for OS Windows).
<br>
<hr>
<hr>
<h2>How to install the library?</h2>
Library uses the STL and Winsock lib from WinApi.
Current project consists only of the sources and headers.
For building you need to use CMake or another system.
<br>
<hr>
<h2>How to use the library?</h2>
Firstly, you need to create the NMetarService::TMetarService object.
Constructor of this object gets text file with ICAO codes of the airports which weather we want to know,
text file with API keys for the yandex geocoder and api.openweathermap.org services respectively and output text file where the METAR will be written.
<br>
Method NMetarService::TMetarService::GetMetar() lets get the METAR for every airport that were in the input list (note in the output file makes automatically after call the constructor and getting weather). 

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
