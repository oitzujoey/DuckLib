
add_library(DuckLib core.c memory.c array.c string.c trie.c)

target_include_directories(
	DuckLib
	INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

# target_compile_definitions(DuckLib PRIVATE "USE_DUCKLIB")

set(installable_libs DuckLib)
install(TARGETS ${installable_libs} DESTINATION lib)
install(FILES core.h memory.h DESTINATION include)
