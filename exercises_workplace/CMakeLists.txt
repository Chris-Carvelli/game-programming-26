file(GLOB file_src_list "*.c" "*.cpp")

foreach(file_src ${file_src_list})
	get_filename_component(targetname ${file_src} NAME_WLE)
	add_executable(${targetname} ${file_src})

	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/itu)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/stb)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/imgui)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/glm/include)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/assimp/include)

	target_link_libraries(${targetname} PRIVATE imgui)
	target_link_libraries(${targetname} PRIVATE SDL3::SDL3)

	message("424242: " SDL3_mixer::SDL3_mixer)
	message("424242: " ${SDL3_mixer::SDL3_mixer})
	if(exists )
	target_link_libraries(${targetname} PRIVATE SDL3_mixer::SDL3_mixer)
	endif()
	target_link_libraries(${targetname} PRIVATE SDL3_ttf::SDL3_ttf)
	target_link_libraries(${targetname} PRIVATE box2d::box2d)
	target_link_libraries(${targetname} PRIVATE assimp::assimp)
endforeach()
