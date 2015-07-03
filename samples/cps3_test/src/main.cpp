#include "cps/CPS_API.hpp"

#include "TestSuite.hpp"
#include "Utils.hpp"

#include <boost/program_options.hpp>

#include <iostream>
#include <cstdlib>
#include <memory>
#include <vector>

int main(int argc, char* argv[]) {
	int result = EXIT_FAILURE;
	try {
		namespace po = boost::program_options;
		po::options_description desc("Options");
		desc.add_options()
				("help,h", "Print this help message")
				("host,c", po::value<std::string>(), "Clusterpoint host URL")
				("db,d", po::value<std::string>(), "Clusterpoint database name")
				("account,a", po::value<std::string>(), "Clusterpoint account id")
				("user,u", po::value<std::string>(), "User name")
				("password,p", po::value<std::string>(), "User password");
		po::variables_map vm;
		std::vector<string> additionalParameters;
		po::parsed_options parsed = po::command_line_parser(argc, argv).
				options(desc).allow_unregistered().run();
		po::store(parsed, vm);

		// Handle --help option
		if (vm.count("help")) {
			std::cout << "Clusterpoint test app" << std::endl
					<< desc << std::endl;
			return EXIT_SUCCESS;
		}

		additionalParameters = po::collect_unrecognized(parsed.options,
				po::include_positional);
		po::notify(vm);

		if (!additionalParameters.empty()) {
			throw std::runtime_error(std::string("Unrecognized option: ") + additionalParameters[0]);
		}
		if (!vm.count("host")) {
			throw std::runtime_error("Clusterpoint host URL must be provided");
		}
		if (!vm.count("db")) {
			throw std::runtime_error("Clusterpoint database name must be provided");
		}
		if (!vm.count("user")) {
			throw std::runtime_error("User name must be provided");
		}
		if (!vm.count("password")) {
			throw std::runtime_error("User password must be provided");
		}

		std::map<std::string, std::string> account_info;
		if (vm.count("account")) {
			account_info["account"] = vm["account"].as<std::string>();
		}

		std::unique_ptr<CPS::Connection> conn(new CPS::Connection(
				vm["host"].as<std::string>(),
				vm["db"].as<std::string>(),
				vm["user"].as<std::string>(),
				vm["password"].as<std::string>(),
				"document",
				"document/id",
				account_info));
		conn->setDebug(true);
		conn->setSocketTimeouts(10, 60, 180);
		TestSuite(*conn).run();

		result = EXIT_SUCCESS;
	} catch (CPS::Exception& e) {
		if (e.getResponse()) {
			// Print out errors and related document ids
			print_errors(std::cout, e.getResponse()->getErrors());
		}
		std::cerr << e.what() << std::endl;
		std::cerr << boost::diagnostic_information(e) << std::endl;
	} catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
		std::cerr << boost::diagnostic_information(e) << std::endl;
	} catch (...) {
		std::cerr << "Unhandled exception" << std::endl;
	}
	return result;
}
