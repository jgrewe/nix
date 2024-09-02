include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/ff233bdd4cac0a0bf6e5cd45bda3406814cb2796.zip
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(BUILD_TESTING)
  file(GLOB GTEST_SOURCES "*test_*.cpp")

  foreach(test_src ${GTEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} GTest::gtest_main nixio)
    add_test(NAME ${test_name} COMMAND ${test_name})
  endforeach()
endif()

include(GoogleTest)
