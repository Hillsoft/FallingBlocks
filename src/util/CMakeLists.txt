add_subdirectory(test)

add_library(util.arena STATIC "Arena.hpp" "Arena.cpp")
target_link_libraries(util.arena
	util.raii_helpers)

add_library(util.atomiccircularbufferqueue INTERFACE "AtomicCircularBufferQueue.hpp")
target_link_libraries(util.atomiccircularbufferqueue INTERFACE
	util.raii_helpers
	util.storage)

add_library(util.blockforwardallocatedarena STATIC "BlockForwardAllocatedArena.hpp" "BlockForwardAllocatedArena.cpp")

add_library(util.basic_match INTERFACE "basic_math.hpp")

add_library(util.debug INTERFACE "debug.hpp")

add_library(util.file STATIC "file.cpp" "file.hpp")

add_library(util.generator INTERFACE "Generator.hpp")
target_link_libraries(util.generator INTERFACE
	util.debug
	util.raii_helpers)

add_library(util.indexedresourcestorage INTERFACE "IndexedResourceStorage.hpp")
target_link_libraries(util.indexedresourcestorage INTERFACE
	util.atomiccircularbufferqueue)

add_library(util.notnull INTERFACE "NotNull.hpp")
target_link_libraries(util.notnull INTERFACE
	util.debug)

add_library(util.portability INTERFACE "portability.hpp")

add_library(util.raii_helpers INTERFACE "raii_helpers.hpp")

add_library(util.registry INTERFACE "Registry.hpp")
target_link_libraries(util.registry INTERFACE
	util.atomiccircularbufferqueue
	util.debug
	util.raii_helpers
	util.synchronized)

add_library(util.resettable INTERFACE "resettable.hpp")
target_link_libraries(util.resettable INTERFACE
	util.storage)

add_library(util.storage INTERFACE "storage.hpp")
target_link_libraries(util.storage INTERFACE
	util.raii_helpers)

add_library(util.string INTERFACE "string.hpp")

add_library(util.synchronized INTERFACE "Synchronized.hpp")

add_library(util.unicode STATIC "unicode.hpp" "unicode.cpp")
target_link_libraries(util.unicode
	util.generator)

add_library(util.vec_generators INTERFACE "vec_generators.hpp")
target_link_libraries(util.vec_generators INTERFACE
	util.generator)

add_library(util.zlib STATIC "zlib.hpp" "zlib.cpp")
target_link_libraries(util.zlib
	util.debug)
