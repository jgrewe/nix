include(CTest)

find_package(CppUnit)
include_directories(${CPPUNIT_INCLUDE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/test)

file(GLOB Tests_SOURCES "*Test*.[ch]pp")
file(GLOB Real_Test_HEADERS "Test*.hpp")

if(BUILD_TESTING)
  foreach(thebackend ${backends})
    set(thebackend_TEST_HEADERS)
    file(GLOB thebackend_TEST_HEADERS "${thebackend}/Test*.hpp")
    list(APPEND Real_Test_HEADERS ${thebackend_TEST_HEADERS})

    set(thebackend_TEST_SOURCES)
    file(GLOB thebackend_TEST_SOURCES "${thebackend}/*Test*.[ch]pp")
    list(APPEND Tests_SOURCES ${thebackend_TEST_SOURCES})
  endforeach()

  foreach(test ${Real_Test_HEADERS})
    get_filename_component(TestFileName ${test} NAME_WE)
    string(REPLACE "Test" "" TestName ${TestFileName})
    message(STATUS " Test added: ${TestName}")
    add_test(${TestName} TestRunner ${TestName})
  endforeach(test)

  add_executable(TestRunner Runner.cpp ${Tests_SOURCES})
  if(NOT WIN32)
    set_target_properties(TestRunner PROPERTIES COMPILE_FLAGS "-Wno-deprecated-declarations")
  endif()
  target_link_libraries(TestRunner ${CPPUNIT_LIBRARIES} nixio)
endif()
