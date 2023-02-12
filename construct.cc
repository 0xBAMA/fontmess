#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

// creating the monolithic model
std::ofstream out( "model.txt" );
void Add ( std::string root ) {
	for ( auto const& file : std::filesystem::directory_iterator{ root } ) {
		// iterate through the file and get character counts
		std::ifstream infile ( root + "/" + file.path().filename().string() );
		std::string line;
		while ( getline( infile, line ) ) {
			std::replace( line.begin(), line.end(), '-', '0' ); 
			std::replace( line.begin(), line.end(), '.', '0' ); 
			std::replace( line.begin(), line.end(), '#', '1' ); 
			std::replace( line.begin(), line.end(), '@', '1' ); 
			std::cout << line << std::endl;
			out << line << std::endl;
		}
	}
}

int main () {
	std::cout << " ------------------------------------=DRAW=-------------------------------- " << std::endl;
	Add( "cherrypick/draw" );
	std::cout << " ------------------------------------=YAFF=-------------------------------- " << std::endl;
	Add( "cherrypick/yaff" );
	return 0;
}
