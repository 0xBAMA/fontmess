#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

// parsing as of commit 0448acfd19fc2c5c0cb6842061234c52797a2828

struct container {
	std::filesystem::directory_entry file;
	std::string GetFullPath () {
		std::stringstream fn;
		fn << file;
		std::string fileName = fn.str();
		if ( !fileName.empty() ) fileName.pop_back();
		fileName.erase( 0, 1 );
		return fileName;
	}

	std::string GetFilename () {
		std::string fileName = GetFullPath();
		fileName = fileName.substr( fileName.find_last_of( "/" ) + 1 );
		return fileName;
	}

	std::string GetExtension () {
		std::string fileName = GetFilename();
		std::string extension = fileName.substr( fileName.find_last_of( "." ) + 1 );
		return extension;
	}

	bool IsYAFF () {
		const std::string myExtension = GetExtension();
		return ( myExtension == "yaff" || myExtension == "YAFF" );
	}

	bool IsDRAW () {
		const std::string myExtension = GetExtension();
		return ( myExtension == "draw" || myExtension == "DRAW" );
	}

};

int main () {
	int count = 0;
	int yaff = 0;
	int draw = 0;

	std::vector<std::string> unrecognizedExtensions;

	for ( const std::filesystem::directory_entry& file : std::filesystem::recursive_directory_iterator( "hoard-of-bitfonts" ) ) {
		if ( file.is_directory() ) {
			std::cout << file << std::endl;
			continue;
		} else {
			// we have a file, categorize
			container c;
			c.file = file;
			
			count++;

			if ( c.IsYAFF() ) {

				yaff++;
				std::string outPath = "cherrypick/yaff/" + std::to_string( count ) + c.GetFilename();
				std::filesystem::copy( c.GetFullPath(), outPath );

			} else if ( c.IsDRAW() ) {

				draw++;
				std::string outPath = "cherrypick/draw/" + std::to_string( count ) + c.GetFilename();
				std::filesystem::copy( c.GetFullPath(), outPath );

			} else {

				unrecognizedExtensions.push_back( c.GetExtension() + " from " + c.GetFullPath() );
			
			}
		}
	}

	std::cout << "encountered " << count << " files - " << yaff << " YAFF and " << draw << " DRAW ( " << draw + yaff << " total )\n";

	/*
	std::cout << std::endl << "unrecognized extensions: \n";
	for ( auto& extension : unrecognizedExtensions ) {
		std::cout << "   " << extension << "\n";
	}
	*/

	return 0;
}
