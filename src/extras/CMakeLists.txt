# Targets

## Objects libraries

### dynamicelements
add_library(dynamicelements OBJECT "dynamic_elements.cc")
target_include_directories(dynamicelements PUBLIC ${HEADER} ${PROJECT_BINARY_DIR})
set_target_properties(dynamicelements PROPERTIES POSITION_INDEPENDENT_CODE 1)

### securityverification
add_library(securityverification OBJECT "security_verification.cc")
target_include_directories(securityverification PUBLIC ${HEADER} ${PROJECT_BINARY_DIR})
set_target_properties(securityverification PROPERTIES POSITION_INDEPENDENT_CODE 1)

### valuesproperties
add_library(valuesproperties OBJECT "values_properties.cc")
target_include_directories(valuesproperties PUBLIC ${HEADER} ${PROJECT_BINARY_DIR})
set_target_properties(valuesproperties PROPERTIES POSITION_INDEPENDENT_CODE 1)
