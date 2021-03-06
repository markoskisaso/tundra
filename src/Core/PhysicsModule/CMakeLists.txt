# Define target name and output directory
init_target (PhysicsModule OUTPUT plugins)

MocFolder ()

# Define source files
file (GLOB CPP_FILES *.cpp)
file (GLOB H_FILES *.h)
set (XML_FILES PhysicsModule.xml)
set (MOC_FILES PhysicsModule.h PhysicsWorld.h EC_RigidBody.h EC_VolumeTrigger.h)
set (SOURCE_FILES ${CPP_FILES} ${H_FILES})

set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${H_FILES} ${CPP_FILES} PARENT_SCOPE)

# Qt4 Wrap
QT4_WRAP_CPP(MOC_SRCS ${MOC_FILES})

add_definitions (-DPHYSICS_MODULE_EXPORTS)

use_package_bullet()
use_core_modules(Framework Math Scene OgreRenderingModule Asset Console EnvironmentModule)

build_library (${TARGET_NAME} SHARED ${SOURCE_FILES} ${MOC_SRCS} ${UI_SRCS})

link_ogre()
link_package_bullet()
link_modules (Framework Scene OgreRenderingModule EnvironmentModule Asset Console)

# MSVC -specific settings for preprocessor and PCH use
if (MSVC)
    # Label StableHeaders.cpp to create the PCH file and mark all other .cpp files to use that PCH file.
    # Add a #define DEBUG_CPP_NAME "this compilation unit name" to each compilation unit to aid in memory leak checking.
    foreach(src_file ${CPP_FILES})
            if (${src_file} MATCHES "StableHeaders.cpp$")
                set_source_files_properties(${src_file} PROPERTIES COMPILE_FLAGS "/YcStableHeaders.h")
            else()
                get_filename_component(basename ${src_file} NAME)
                set_source_files_properties(${src_file} PROPERTIES COMPILE_FLAGS "/YuStableHeaders.h -DDEBUG_CPP_NAME=\"\\\"${basename}\"\\\"")
            endif()
    endforeach(src_file ${CPP_FILES})
endif (MSVC)

if (WIN32)
target_link_libraries (${TARGET_NAME} ws2_32.lib)
endif()

# Xcode hack: For a very unknown reason, link_package_bullet() does not cause Xcode to add Bullet libs to PhysicsModule linked libs list.
# So, add it manually. TODO: Figure this out and remove the following lines. link_package_bullet() should do the job for Xcode as well.
if (XCODE_VERSION)
target_link_libraries(${TARGET_NAME} LinearMath BulletDynamics BulletCollision)
endif()

final_target ()