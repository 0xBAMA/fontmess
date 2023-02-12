#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>


struct letter {
	std::vector< std::string > charData;
	int lineFirstEncountered = 0;

	bool isDamaged () {
		const int baseSize = charData[ 0 ].size();
		for ( unsigned int i = 0; i < charData.size(); i++ ) {
			if ( charData[ i ].length() != baseSize ) {
				return true;
			}
		}
		return false;
	}

	bool isSimpleDamage () {
		const int baseSize = charData[ 0 ].size();
		bool pass = true;
		for ( unsigned int i = 1; i < charData.size(); i++ ) {
			if ( charData[ i ].length() != ( baseSize + 1 ) ) {
				pass = false;
			}
		}
		return pass;
	}

	void fixSimpleDamage () {
		charData[ 0 ] = "0" + charData[ 0 ];
	}
};

std::vector< letter > letterSet;

void LoadAll ( std::string filename ) {
	std::ifstream in( filename );
	std::string line;
	bool insideLetter = false;

	letter currentLetter;
	int currentLine = 0;
	while ( getline( in, line ) ) {
		currentLine++;
		const int lineLength = line.length();
		if ( lineLength > 0 ) { // there is glyph data on this line
			if ( insideLetter == false ) { // first line encountered of a new glyph
				currentLetter.charData.clear();
				currentLetter.lineFirstEncountered = currentLine;
				insideLetter = true;
			}
			// in any event, we have a new line for the current glyph
			currentLetter.charData.push_back( line );
		} else {
			if ( insideLetter == true ) { // we are leaving a letter
				letterSet.push_back( currentLetter );
				insideLetter = false;
			}
		}
	}
}

void WalkModel ( bool fix ) {
	int damaged = 0;
	for ( auto& l : letterSet ) {
		if ( l.isDamaged() == true ) {
			damaged++;
			if ( fix == true ) {
				if ( l.isSimpleDamage() == true ) {
					l.fixSimpleDamage();
				}
			}
		}
	}
	std::cout << std::endl << "total damaged glyphs: " << damaged << std::endl;
}

void WriteModel ( std::string filename ) {
	std::ofstream out( filename );
	out << std::endl;
	for ( auto& l : letterSet ) {
		for ( unsigned int i = 0; i < l.charData.size(); i++ ) {
			out << l.charData[ i ] << std::endl;
		}
	}
}

int main () {
	LoadAll( "model.txt" );
	WalkModel( true );
	WalkModel( false );
	WriteModel( "model2.txt" );
	return 0;
}
