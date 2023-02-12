#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

// determining the most frequently appearing characters in a the files - this is going to be useful for mass parsing

struct ocurrence {
	char c;
	int count = 0;
};

void GetDistribution ( std::string root ) {
	std::map< char, int > characterDistribution;

	for ( auto const& file : std::filesystem::directory_iterator{ root } ) {
		// iterate through the file and get character counts
		std::ifstream infile ( root + "/" + file.path().filename().string() );
		char c;
		while ( infile >> c ) {
			// std::cout << c;
			characterDistribution[ c ]++;
			if ( c != '.' && c != '@' && c != '-' && c != '#' ) {
				// std::cout << c << " found in " << file.path().filename().string() << std::endl;
			}
		}
	}

	std::vector< ocurrence > values;
	for ( const auto& [ key, value ] : characterDistribution ) {
		values.push_back( ocurrence{ key, value } );
	}

	std::sort( values.begin(), values.end(), []( ocurrence a, ocurrence b ){ return a.count > b.count; } );
	for ( auto& entry : values ) {
		std::cout << "[ " << entry.c << " ] ocurred " << entry.count << " times\n";
	}
}

int main () {
	std::cout << " ------------------------------------=DRAW=-------------------------------- " << std::endl;
	GetDistribution( "cherrypick/draw" );
	std::cout << " ------------------------------------=YAFF=-------------------------------- " << std::endl;
	GetDistribution( "cherrypick/yaff" );
	return 0;
}
