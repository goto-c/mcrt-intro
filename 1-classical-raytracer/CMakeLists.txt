# main
add_executable(main "main.cpp" "obj.cpp")
target_link_libraries(main PUBLIC
    spdlog::spdlog
    glm
    stb_image_write
    OpenMP::OpenMP_CXX
    tinyobjloader
)

# # obj
# add_executable(obj "obj.cpp")
# target_link_libraries(obj PUBLIC
#     spdlog::spdlog
#     glm
#     stb_image_write
#     OpenMP::OpenMP_CXX
#     tinyobjloader
# )

# reference
add_subdirectory("reference/")
