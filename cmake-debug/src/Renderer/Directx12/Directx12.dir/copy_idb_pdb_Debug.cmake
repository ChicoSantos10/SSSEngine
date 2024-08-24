# CMake generated file
# The compiler generated pdb file needs to be written to disk
# by mspdbsrv. The foreach retry loop is needed to make sure
# the pdb file is ready to be copied.

foreach(retry RANGE 1 30)
  if (EXISTS "N:/C++Projects/SSSEngine/cmake-debug/src/SSSEngine.dir/${PDB_PREFIX}SSSEngine.pdb" AND (NOT EXISTS "N:/C++Projects/SSSEngine/cmake-debug/src/Renderer/Directx12/Directx12.dir/${PDB_PREFIX}SSSEngine.pdb" OR NOT "N:/C++Projects/SSSEngine/cmake-debug/src/Renderer/Directx12/Directx12.dir/${PDB_PREFIX}SSSEngine.pdb  " IS_NEWER_THAN "N:/C++Projects/SSSEngine/cmake-debug/src/SSSEngine.dir/${PDB_PREFIX}SSSEngine.pdb"))
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "N:/C++Projects/SSSEngine/cmake-debug/src/SSSEngine.dir/${PDB_PREFIX}SSSEngine.pdb" "N:/C++Projects/SSSEngine/cmake-debug/src/Renderer/Directx12/Directx12.dir/${PDB_PREFIX}" RESULT_VARIABLE result  ERROR_QUIET)
    if (NOT result EQUAL 0)
      execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 1)
    else()
      break()
    endif()
  elseif(NOT EXISTS "N:/C++Projects/SSSEngine/cmake-debug/src/SSSEngine.dir/${PDB_PREFIX}SSSEngine.pdb")
    execute_process(COMMAND ${CMAKE_COMMAND} -E sleep 1)
  endif()
endforeach()
