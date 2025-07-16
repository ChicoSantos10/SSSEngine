target_include_directories(SSSEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# LOW_PRIORITY: Dx12 should not need input but its set there for now for easy of testing
# Remove when restructuring code
target_include_directories(Directx12 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
