# CMake generated Testfile for 
# Source directory: /home/alex/cpp/cpp-graduate/01-basics/lfucpp
# Build directory: /home/alex/cpp/cpp-graduate/01-basics/lfucpp/build
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[TestLFU.testEndToEnd]=] "/usr/bin/bash" "/home/alex/cpp/cpp-graduate/01-basics/lfucpp/test.sh" "/home/alex/cpp/cpp-graduate/01-basics/lfucpp/build/lfucpp" "/home/alex/cpp/cpp-graduate/01-basics/lfucpp")
set_tests_properties([=[TestLFU.testEndToEnd]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/alex/cpp/cpp-graduate/01-basics/lfucpp/CMakeLists.txt;21;add_test;/home/alex/cpp/cpp-graduate/01-basics/lfucpp/CMakeLists.txt;0;")
