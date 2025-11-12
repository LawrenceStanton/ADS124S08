cmake_minimum_required(VERSION 4.0)

project(ADS124S08)

set(LIBRARY ${PROJECT_NAME})

enable_language(CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOURCE_FILES
	Src/*.cpp
)

add_library(${PROJECT_NAME} STATIC
	${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/Inc
)

add_library(${LIBRARY}::${LIBRARY} ALIAS ${LIBRARY})

if(NOT CMAKE_CROSSCOMPILING)
	option(ADS124S08_CODE_COVERAGE "Enable gcovr code coverage for ADS124S08" OFF)

	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest
		GIT_TAG        origin/main
	)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	FetchContent_MakeAvailable(googletest)
	enable_testing()

	set(TEST_EXECUTABLE ${PROJECT_NAME}_Test)

	file(GLOB TEST_SOURCE_FILES
		Test/*.test.cpp
	)

	add_executable(${TEST_EXECUTABLE}
		${TEST_SOURCE_FILES}
	)

	target_compile_options(${TEST_EXECUTABLE} PRIVATE
		$<$<BOOL:${ADS124S08_CODE_COVERAGE}>:--coverage>
	)

	target_link_libraries(${TEST_EXECUTABLE} PRIVATE
		${LIBRARY}::${LIBRARY}
		GTest::gtest_main
		GTest::gmock
	)

	target_link_options(${TEST_EXECUTABLE} PRIVATE
		$<$<BOOL:${ADS124S08_CODE_COVERAGE}>:--coverage>
	)

	include(GoogleTest)
	gtest_discover_tests(${TEST_EXECUTABLE})

	if(ADS124S08_CODE_COVERAGE)
		set(GCOVR_COMMAND gcovr --root ${CMAKE_SOURCE_DIR} --filter '.*/ADS124S08/.*' --exclude '.*\.test\..*' ${CMAKE_CURRENT_BINARY_DIR})
		set(SILENT_GCOVR_COMMAND ./${TEST_EXECUTABLE} > /dev/null)
	
		add_custom_command(TARGET ${TEST_EXECUTABLE} PRE_BUILD
			COMMENT "Cleaning previous gcov data files for ADS124S08 Tests..."
			COMMAND find ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/${TEST_EXECUTABLE}.dir -type f -name "*.gcda" -delete
		)

		add_custom_command(TARGET ${TEST_EXECUTABLE} POST_BUILD
			COMMENT "Running gcovr Code Coverage for ADS124S08 Tests..."
			COMMAND ${SILENT_GCOVR_COMMAND} && ${GCOVR_COMMAND} || echo "Code coverage failed. All tests must pass for code coverage to run."
		)
	endif()
endif()
