file(GLOB file_src_list "*.c" "*.cpp")

foreach(file_src ${file_src_list})
	get_filename_component(targetname ${file_src} NAME_WLE)
	add_executable(${targetname} ${file_src})

	# link all libraries that we will need during the course
	# NOTE: this libraries expose the needed API publicly, so our game can see the include
	#       paths these libraries want
	target_link_libraries(${targetname} PRIVATE $<TARGET_NAME_IF_EXISTS:SDL3::SDL3>)
	target_link_libraries(${targetname} PRIVATE $<TARGET_NAME_IF_EXISTS:imgui>)
	target_link_libraries(${targetname} PRIVATE $<TARGET_NAME_IF_EXISTS:SDL3_mixer::SDL3_mixer>)
	target_link_libraries(${targetname} PRIVATE $<TARGET_NAME_IF_EXISTS:SDL3_ttf::SDL3_ttf>)
	target_link_libraries(${targetname} PRIVATE $<TARGET_NAME_IF_EXISTS:box2d::box2d>)
	target_link_libraries(${targetname} PRIVATE $<TARGET_NAME_IF_EXISTS:assimp::assimp>)

	# add extra include paths that we will need along the course
	# (not all dependencies are compiled, so those we need to look for explicitely)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/itu)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/stb)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/imgui)
	target_include_directories(${targetname} PRIVATE ${CMAKE_SOURCE_DIR}/lib/glm/include)
endforeach()