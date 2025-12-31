cmake_minimum_required(VERSION 3.15)
project(OptionsMarketMaking VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

# Find required packages
find_package(Eigen3 REQUIRED NO_MODULE)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/core/config.cpp
    src/core/utils.cpp
    src/core/models/regime.cpp
    src/core/models/regimeparams.cpp
    src/core/models/asset.cpp
    src/core/models/security.cpp
    src/core/models/option.cpp
    src/core/models/future.cpp
    src/core/models/volsurface.cpp
    src/core/models/market.cpp
    src/core/models/position.cpp
    src/core/models/risk.cpp
    src/core/workers/simulator.cpp
    src/core/workers/calculator.cpp
    src/analytics/table.cpp
)

# Create executable
add_executable(options-market-making ${SOURCES})

# Link Eigen
target_link_libraries(options-market-making PUBLIC Eigen3::Eigen)

# Optional: Add optimization flags
if(MSVC)
    target_compile_options(options-market-making PRIVATE /O2)
else()
    target_compile_options(options-market-making PRIVATE -O3)
endif()
