add_library(DuckLib core.c memory.c array.c string.c trie.c)

target_include_directories(DuckLib
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
	)
