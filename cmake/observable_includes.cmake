# Add the main observable include directory to the specified target.
#
# - target_name Name of the target who's include directories to configure.
function(add_observable_includes target_name)
    target_include_directories(${target_name} PUBLIC ${PROJECT_SOURCE_DIR}/include)
endfunction(add_observable_includes)
