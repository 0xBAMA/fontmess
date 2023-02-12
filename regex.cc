#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <regex>

// ripping through and fucking up the files into one big thing to parse

std::string fullString;

// I believe this can be done in two passes:

void Condition ( std::string root ) {
	std::map< char, int > characterDistribution;

	for ( auto const& file : std::filesystem::directory_iterator{ root } ) {
		// iterate through the file and get character counts
		std::ifstream infile ( root + "/" + file.path().filename().string() );

		// get entire contents of file into a string
		std::string content( ( std::istreambuf_iterator<char>( infile ) ), ( std::istreambuf_iterator<char>() ) );

		// apply the regex replaces
			//	[a-zA-Z0-9]-[a-zA-Z0-9] will find dashes between alphanumerics ( remove these )
		content = std::regex_replace( content, std::regex( "[a-zA-Z0-9]-[a-zA-Z0-9]" ), "\n" );
			//	[^-#.@\n] will find all characters that are not part of the yaff and draw data - also ignores newlines
		content = std::regex_replace( content, std::regex( "[^-#.@\n]" ), "" );

		// replace characters
		std::replace( content.begin(), content.end(), '-', '0' );
		std::replace( content.begin(), content.end(), '.', '0' );
		std::replace( content.begin(), content.end(), '#', '1' );
		std::replace( content.begin(), content.end(), '@', '1' );

		// append it to the running string
		fullString += "\n\n";
		fullString += content;
	}
}

int main () {
	std::cout << " ------------------------------------=DRAW=-------------------------------- " << std::endl;
	Condition( "cherrypick/draw" );
	std::cout << " ------------------------------------=YAFF=-------------------------------- " << std::endl;
	Condition( "cherrypick/yaff" );
	std::cout << " -----------------------------------=WRITING=------------------------------ " << std::endl;
	std::ofstream out( "model.txt" );
	out << fullString;
	return 0;
}
