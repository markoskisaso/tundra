# Define target name and output directory
init_target (EC_HoveringText)

# Define source files
file (GLOB CPP_FILES *.cpp)
file (GLOB H_FILES *.h)
file (GLOB XML_FILES *.xml)
file (GLOB MOC_FILES EC_HoveringText.h)

# Qt4 Moc files to subgroup "CMake Moc"
MocFolder ()

set (SOURCE_FILES ${CPP_FILES} ${H_FILES})
set (FILES_TO_TRANSLATE ${FILES_TO_TRANSLATE} ${H_FILES} ${CPP_FILES} PARENT_SCOPE)

# Qt4 Wrap
QT4_WRAP_CPP(MOC_SRCS ${MOC_FILES})

use_core_modules(Framework Math Scene OgreRenderingModule)

build_library (${TARGET_NAME} STATIC ${SOURCE_FILES} ${MOC_SRCS} )

link_ogre()
link_modules (Framework Scene OgreRenderingModule)

SetupCompileFlags()

final_target ()
