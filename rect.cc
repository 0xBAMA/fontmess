#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h"

struct letter {
	std::vector< std::string > charData;
	friend bool operator== ( const letter& l1, const letter& l2 ) {
		// early outs
		if ( l1.charData.size() != l2.charData.size() )
			return false; // y dim doesn't match

		if ( l1.charData[ 0 ].length() != l2.charData[ 0 ].length() )
			return false; // x dim doesn't match

		// check for all bits in the pattern
		for ( unsigned int i = 0; i < l1.charData.size(); i++ )
		for ( unsigned int j = 0; j < l1.charData[ 0 ].length(); j++ )
			if ( l1.charData[ i ][ j ] != l2.charData[ i ][ j ] )
				return false;
		return true;
	}
};

std::vector< letter > letterSet;
void LoadAll ( std::string filename ) {
	std::ifstream in( filename );
	std::string line;
	bool insideLetter = false;
	letter currentLetter;
	while ( getline( in, line ) ) {
		const int lineLength = line.length();
		if ( lineLength > 0 ) { // there is glyph data on this line
			if ( insideLetter == false ) { // first line encountered of a new glyph
				currentLetter.charData.clear();
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

void RemoveDuplicates () {
	auto tStart = std::chrono::steady_clock::now();
	auto tCurrent = std::chrono::steady_clock::now();

	// duplicate glyph check
	int numDuplicates = 0;
	std::cout << std::endl << "Begin Duplicate Check" << std::endl;
	for ( int idx1 = 0; idx1 < letterSet.size(); idx1++ ) {
		tCurrent = std::chrono::steady_clock::now();
		const float percent = ( float( idx1 ) / float( letterSet.size() - 1 ) ) * 100.0f;
		const float duration = std::chrono::duration<double>( tCurrent - tStart ).count();
		std::cout << idx1 << "/" << letterSet.size() << " ( " << percent << " % ) " << numDuplicates << " duplicates found in " << duration << " s\n";
		for ( int idx2 = idx1 + 1; idx2 < letterSet.size(); idx2++ ) {
			if ( letterSet[ idx1 ] == letterSet[ idx2 ] ) {
				numDuplicates++;
				// remove invalid glyph from the list
				letterSet.erase( letterSet.begin() + idx2 );
				idx2--; // decrement idx2 to compensate
			}
		}
	}
}

void RectPack ( std::string filename ) {
	// shuffle around the entries
	// auto rd = std::random_device {};
	// auto rng = std::default_random_engine { rd() };
	// std::shuffle( std::begin( letterSet ), std::end( letterSet ), rng );

	size_t maxVerticalSize = 0;
	size_t maxHorizontalSize = 0;
	const int padding = 4;

	for ( auto& l : letterSet ) {
		maxVerticalSize = std::max( l.charData.size() + padding, maxVerticalSize );
	}

	for ( auto& l : letterSet ) {
		maxHorizontalSize = std::max( l.charData[ 0 ].length() + padding, maxHorizontalSize );
	}

	constexpr int numWide = 150; // how many side to side
	const int numRows = int( std::ceil( static_cast< float >( letterSet.size() ) / static_cast< float >( numWide ) ) );

	int widthPixels = ( numWide * maxHorizontalSize ) / 2;
	int heightPixels = ( numRows * maxVerticalSize ) / 2;

	std::cout << widthPixels << " from " << numWide << " by " << maxHorizontalSize << std::endl;
	std::cout << heightPixels << " from " << numRows << " by " << maxVerticalSize << std::endl;

	// this is scratch space memory, from what I understand
	std::vector< stbrp_node > nodes;
	nodes.resize( letterSet.size() * 2 );
	stbrp_context c;
	stbrp_init_target( &c, widthPixels, heightPixels, &nodes[ 0 ], letterSet.size() * 2 );

	int k = 0;
	std::vector< stbrp_rect > rects;
	for ( auto& letter : letterSet ) {
		stbrp_rect r;
		r.w = letter.charData[ 0 ].size() + padding;
		r.h = letter.charData.size() + padding;
		r.index = k;
		k++;
		rects.push_back( r );
	}

	if ( stbrp_pack_rects( &c, &rects[ 0 ], letterSet.size() ) ) {
		std::cout << "success" << std::endl;
	}

	widthPixels += 14;
	int ymax = 0;
	size_t pixelcnt = heightPixels * widthPixels * 4;
	std::vector< unsigned char > image;
	std::cout << "resizing to " << pixelcnt << std::endl;
	image.resize( pixelcnt, 255 );

	for ( auto& rect : rects ) {
		for ( int x = 0; x < rect.w; x++ ) { // compensate for padding
			for ( int y = 0; y < rect.h; y++ ) {

				int base = ( ( x + rect.x + 7 ) + widthPixels * ( y + rect.y + 7 ) ) * 4;
				if (
					( x - 2 ) < letterSet[ rect.index ].charData[ 0 ].length() &&
					( x - 2 ) >= 0 &&
					( y - 2 ) < letterSet[ rect.index ].charData.size() &&
					( y - 2 ) >= 0
				) { // bounds checking
					if ( letterSet[ rect.index ].charData[ y - 2 ][ x - 2 ] == '1' ) {
						image[ base + 0 ] = 69;
						image[ base + 1 ] = 69;
						image[ base + 2 ] = 69;
						image[ base + 3 ] = 255;
					} else {
						image[ base + 0 ] = 205;
						image[ base + 1 ] = 205;
						image[ base + 2 ] = 205;
						image[ base + 3 ] = 255;
					}
				} else if (
					( x - 2 ) < letterSet[ rect.index ].charData[ 0 ].length() + 1 &&
					( x - 2 ) >= 1 &&
					( y - 2 ) < letterSet[ rect.index ].charData.size() + 1 &&
					( y - 2 ) >= 1 )
				{
					// colored border, indicating index
						image[ base + 0 ] = rect.index % 255;
						image[ base + 1 ] = ( rect.index / 255 ) % 255;
						image[ base + 2 ] = ( rect.index / ( 255 * 255 ) ) % 255;
						image[ base + 3 ] = 255;
				}
			}
		}
		ymax = std::max( rect.y + rect.h + 14, ymax );
	}
	stbi_write_png( filename.c_str(), widthPixels, ymax, 4, &image[ 0 ], widthPixels * 4 );
}

void WriteModel ( std::string filename ) {
	std::ofstream out( filename );
	out << std::endl;
	for ( auto& l : letterSet ) {
		for ( unsigned int i = 0; i < l.charData.size(); i++ ) {
			out << l.charData[ i ] << std::endl;
		}
		out << std::endl << std::endl;
	}
}

int main () {
	LoadAll( "model3_filtered.txt" );
	// RemoveDuplicates();
	RectPack( "out.png" );
	// WriteModel( "model3_filtered.txt" );
	return 0;
}
