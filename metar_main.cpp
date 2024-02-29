#include <fstream>
#include <iostream>
#include "metar_getter.cpp"
#include <vector>


namespace NMetarService {

	class TService {
	private:
		const std::string OutputMetarFile;
		std::vector<NWeather::TMetar> Metars;
		NWeather::TApiKeys ApiKeys;

	public:
		TService(std::string icaoCodesFile, std::string apiKeysFile, std::string outputMetar) 
			: OutputMetarFile(std::move(outputMetar))
		{
			GetIcaoCodes(icaoCodesFile, apiKeysFile);
		}


		std::vector<std::string> GetMetar() {
			std::vector<std::string> metars;
			for (auto& metar : Metars) {
				metar.FormMetar();
				metars.push_back(metar.GetMetar());
			}

			std::ofstream foutMetar(OutputMetarFile);

			if (!foutMetar.is_open()) {
				throw std::runtime_error("file MetarFile wasn't open");
			}

			for (const auto& metar : metars) {
				foutMetar << metar << std::endl;
			}

			return metars;
		}

	private:
		void GetIcaoCodes(std::string icaoCodesFile, std::string apiKeysFile) {
			std::ifstream finIcaoCodes(icaoCodesFile);

			if (!finIcaoCodes.is_open()) {
				throw std::runtime_error("file IcaoCodes wasn't open");
			}

			NWeather::TMetar::InitializeApi(apiKeysFile);
			for (std::string icao; std::getline(finIcaoCodes, icao); ) {
				Metars.push_back(NWeather::TMetar(icao));
			}
		}

	};

}


int main() {
	NMetarService::TService metarService("D:\\MetarProject\\metar_main\\ICAO_Codes.txt", 
		"D:\\MetarProject\\metar_main\\api_keys.txt", "D:\\MetarProject\\metar_main\\metars.txt");

	auto metars = metarService.GetMetar();

	return 0;
}