# src/


# set(VIS_HEADERS_AUDIO
	# "audio/AudioReader.h"
	# "audio/Buffer.h"
	# "audio/Recorder.h"
# )
# set(VIS_SOURCE_AUDIO
	# "audio/Buffer.cpp"
	# "audio/Recorder.cpp"
# )
# set(VIS_HEADERS_CAMERA
	# "camera/Camera.h"
	# "camera/CameraSystem.h"
# )
# set(VIS_SOURCE_CAMERA
	# "camera/Camera.cpp"
	# "camera/CameraSystem.cpp"
# )


set(HEADER_LIST
	"camera/Camera.h"
	"camera/CameraSystem.h"
	"general/About.h"
	"general/Constants.h"
	"general/Serialisation.h"
	"game-of-life/GameOfLife_Element.h"
	"game-of-life/GameOfLife_ElementList.h"
	"game-of-life/GameOfLife_Game.h"
	"game-of-life/GameOfLife_Grid.h"
	"graphics/GeometryTools.h"
	"graphics/Mesh.h"
	"graphics/MeshGenerator.h"
	"graphics/PostProcessing.h"
	"graphics/Scene.h"
	"graphics/Shader.h"
	"graphics/Texture.h"
	"graphics/Vertex.h"
	"maths/AbstractRandomObject.h"
	"maths/Interpolation.h"
	"maths/FunctionSource.h"
	"maths/MetaDistribution.h"
	"maths/ODE.h"
	"maths/Tree.h"
	"maths/VectorField.h"
	"models/Model_AudioInterface.h"
	"models/Model_Scene.h"
	"models/Model_Session.h"
	"models/Model_Transport.h"
	"models/Model_VideoRendering.h"
	"models/Model_ViewportSystem.h"
	"models/Model_Visualisation.h"
	"particles/ParticleSet.h"
	"particles/ParticleSystem.h"
	"particles/Trajectory.h"
	"program/Program.h"
	"scripting/Event.h"
	"scripting/EventsSystem.h"
	"scripting/MappableProperty.h"
	"time/Timekeeping.h"
	"time/Timestring.h"
	"video/FFMPEG_Encoder.h"
	"visualisations/Visualisation.h"
	"visualisations/Visualisation_Cubes.h"
	"visualisations/Visualisation_Fractal.h"
	"visualisations/Visualisation_GameOfLife.h"
	"visualisations/Visualisation_Oscilloscope.h"
	"visualisations/Visualisation_Particles.h"
	"visualisations/Visualisation_PointClouds.h"
	"visualisations/Visualisation_Sandbox.h"
	"visualisations/Visualisation_Splinters.h"
	"visualisations/Visualisation_TreeField.h"
	"visualisations/Visualisation_Surfaces.h"
	"windows/UI_Recorder.h"
	"windows/UserInterface.h"
	"windows/Window_Abstract.h"
	"windows/Window_AudioInterface.h"
	"windows/Window_PostProcessing.h"
	"windows/Window_Session.h"
	"windows/Window_Transport.h"
	"windows/Window_VideoRendering.h"
	"windows/Window_ViewportSystem.h"
	"windows/Window_Visualisation.h"
)
	
set(SOURCE_LIST
	"audio/Buffer.cpp"
	"audio/Recorder.cpp"
	"camera/Camera.cpp"
	"camera/CameraSystem.cpp"
	"general/About.cpp"
	"game-of-life/GameOfLife_Game.cpp"
	"game-of-life/GameOfLife_Grid.cpp"
	"graphics/GeometryTools.cpp"
	"graphics/Mesh.cpp"
	"graphics/MeshGenerator.cpp"
	"graphics/PostProcessing.cpp"
	"graphics/Shader.cpp"
	"graphics/Texture.cpp"
	"maths/ODE.cpp"
	"maths/Interpolation.cpp"
	"maths/Tree.cpp"
	"models/Model_AudioInterface.cpp"
	"models/Model_Scene.cpp"
	"models/Model_Session.cpp"
	"models/Model_Transport.cpp"
	"models/Model_VideoRendering.cpp"
	"models/Model_ViewportSystem.cpp"
	"models/Model_Visualisation.cpp"
	"particles/ParticleSet.cpp"
	"particles/ParticleSystem.cpp"
	"particles/Trajectory.cpp"
	"program/Program.cpp"
	"scripting/Event.cpp"
	"video/FFMPEG_Encoder.cpp"
	"visualisations/Visualisation_Cubes.cpp"
	"visualisations/Visualisation_Fractal.cpp"
	"visualisations/Visualisation_GameOfLife.cpp"
	"visualisations/Visualisation_Oscilloscope.cpp"
	"visualisations/Visualisation_Particles.cpp"
	"visualisations/Visualisation_PointClouds.cpp"
	"visualisations/Visualisation_Sandbox.cpp"
	"visualisations/Visualisation_Splinters.cpp"
	"visualisations/Visualisation_Surfaces.cpp"
	"visualisations/Visualisation_TreeField.cpp"
	"windows/UI_Recorder.cpp"
	"windows/UserInterface.cpp"
	"windows/Window_Abstract.cpp"
	"windows/Window_AudioInterface.cpp"
	"windows/Window_PostProcessing.cpp"
	"windows/Window_Session.cpp"
	"windows/Window_Transport.cpp"
	"windows/Window_VideoRendering.cpp"
	"windows/Window_ViewportSystem.cpp"
	"windows/Window_Visualisation.cpp"
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${HEADER_LIST} ${SOURCE_LIST})

add_library(VISUALISATIONS_LIBRARY
			STATIC ${HEADER_LIST} ${SOURCE_LIST} ${SOURCE_LIST_IMGUI} ${SOURCE_LIST_KISSFFT})
target_include_directories(VISUALISATIONS_LIBRARY PUBLIC ${glm_SOURCE_DIR} ${imgui_SOURCE_DIR} ${kissfft_SOURCE_DIR} ${FFMPEG_INCLUDE} ${stb_SOURCE_DIR})
target_link_libraries(VISUALISATIONS_LIBRARY glfw glad portaudio ${Boost_LIBRARIES} ${FFMPEG_LIB_AVCODEC} ${FFMPEG_LIB_AVDEVICE} ${FFMPEG_LIB_AVFILTER} ${FFMPEG_LIB_AVFORMAT} ${FFMPEG_LIB_AVUTIL} ${FFMPEG_LIB_POSTPROC} ${FFMPEG_LIB_SWRESAMPLE} ${FFMPEG_LIB_SWSCALE})

add_executable(VISUALISATIONS
			main.cpp ${HEADER_LIST} ${SOURCE_LIST_IMGUI})
#target_include_directories(VISUALISATIONS PUBLIC ${glm_SOURCE_DIR} ${imgui_SOURCE_DIR} ${kissfft_SOURCE_DIR} ${FFMPEG_INCLUDE} ${stb_SOURCE_DIR})
target_link_libraries(VISUALISATIONS PUBLIC VISUALISATIONS_LIBRARY)




