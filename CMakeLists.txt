cmake_minimum_required(VERSION 3.25)
project(FILP-MO)
set(CMAKE_CXX_STANDARD 23)
enable_testing()

add_executable(allocators
    Allocators/main.cc
    Allocators/resources/TextContainer.cc
    Allocators/PoolAllocator.cc
    Allocators/LinkedListAllocator.cc
    Allocators/DescriptorAllocator.cc
    Allocators/BuddyAllocator.cc
    )

add_executable(text_container_test
    Allocators/resources/TextContainer_test.cc
    Allocators/resources/TextContainer.cc
)
add_test(NAME text_container_test COMMAND text_container_test)

add_executable(binary_tree
    Binary\ tree/main_BinarySearchTree.cc
    Binary\ tree/BinarySearchTree.cc
)

add_executable(smart_pointer
    Pointers/main_SmartPointer.cc
)
