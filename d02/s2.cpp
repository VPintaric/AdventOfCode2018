#include <iostream>
#include <string>
#include <map>
#include <memory>

struct TrieNode {
	char c;
	std::map< char, std::unique_ptr< TrieNode > > children;
};

template< class T >
int insertStringAndReturnPrefixMatchCount( TrieNode &node, T iterator, T iteratorEnd ) {
	if( iterator == iteratorEnd ) {
		return 0;
	}

	auto value = node.children.find( *iterator );
	if( value == node.children.end() ) {
		auto newNode = std::make_unique< TrieNode >();
		newNode->c = *iterator;

		insertStringAndReturnPrefixMatchCount( *newNode, iterator + 1, iteratorEnd );
		node.children[ *iterator ] = std::move( newNode );
		
		return 0;
	} else {
		return 1 + insertStringAndReturnPrefixMatchCount( *value->second, iterator + 1, iteratorEnd );
	}
}

int main( void ) {
	TrieNode prefixTrie, suffixTrie;

	std::string id;
	while( std::cin >> id ) {
		auto commonPrefixLength = insertStringAndReturnPrefixMatchCount( prefixTrie, id.begin(), id.end() );
		auto commonSuffixLength = insertStringAndReturnPrefixMatchCount( suffixTrie, id.rbegin(), id.rend() );

		if( commonPrefixLength + commonSuffixLength == id.size() - 1 ) {
			id.erase( commonPrefixLength, 1 );
			std::cout << id << "\n";
			break;
		} 
	}

	return 0;
}