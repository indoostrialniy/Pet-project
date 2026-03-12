#include "global.h"


std::shared_ptr<Level> level;	//Level* level = nullptr;


GLFWwindow* engineWindow;


int tick = 0;

#include "Mesh.h"
#include "Shader.h"
#include "Skeleton.h"
#include "Material.h"
#include "SoundAsset.h"
#include "CollisionShell.h"

resManAsset <Mesh> 				Meshes("meshes");
resManAsset <Shader> 			Shaders("shaders");
resManAsset <Skeleton>			Skeletons("skeletons");
resManAsset <Material> 			Materials("materials");
resManAsset <SoundAsset>		SoundAssets("sounds");
resManAsset <CollisionShell> 	CollisionShells;


biCycleSequencer<BICYCLE_LIBRARY_SEQUENCES_ARGUMENT_TYPE> Flow;

glm::mat4 lights[20];

glm::mat3* CameraAxisPtr;
glm::vec3* CameraPositionPtr;
glm::vec3* CameraPrevPosPtr;
glm::quat* CameraQuaternion;

