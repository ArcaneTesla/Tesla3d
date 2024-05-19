#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "simple_render_system.hpp"
#include "tsl_camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>



namespace tsl {

    struct GlobalUbo {
        glm::mat4 projectionView{ 1.f };
        glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f,-3.f, -1.f });

    };

    FirstApp::FirstApp() {loadObjects();}

    FirstApp::~FirstApp() {}

	void FirstApp::run() {
        std::vector<std::unique_ptr<TslBuffer>> uboBuffers(TslSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<TslBuffer>(
                tslDevice,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->map();
        }

        SimpleRenderSystem simpleRenderSystem{ tslDevice, tslRenderer.getSwapChainRenderPass() };
        TslCamera camera{};

        auto viewerObject = TslSceneObject::createObject();

        KeyboardMovementController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

		while (!tslWindow.shouldClose()) {
			glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            //frameTime = glm::min(frameTime, MAX_FRAME_TIME);

            cameraController.moveInPlaneXZ(tslWindow.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = tslRenderer.getAspectRatio();
            //ортографика
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            //перспектива
            camera.setPerspectiveProjection(glm::radians(50.f),aspect,0.1f,10.f);

            if (auto commandBuffer = tslRenderer.beginFrame()) {
                int frameIndex = tslRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera
                };

                //обновление
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //отрисовка
                tslRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderObjects(frameInfo, sceneObjects);
                tslRenderer.endSwapChainRenderPass(commandBuffer);
                tslRenderer.endFrame();
            }
		}
        vkDeviceWaitIdle(tslDevice.device());
	}
   
    void FirstApp::loadObjects() {
        std::shared_ptr<TslModel> TslModel = 
        TslModel::createModelFromFile(tslDevice, "models/blender_monkey_flat.obj");
        auto flatMonkey = TslSceneObject::createObject();
        flatMonkey.model = TslModel;
        flatMonkey.transform.translation = { -1.f,.05f,2.5f };
        flatMonkey.transform.scale = glm::vec3(0.5f);
        sceneObjects.push_back(std::move(flatMonkey));

        TslModel = TslModel::createModelFromFile(tslDevice, "models/blender_monkey.obj");
        auto Monkey = TslSceneObject::createObject();
        Monkey.model = TslModel;
        Monkey.transform.translation = { 1.f,.05f,2.5f };
        Monkey.transform.scale = glm::vec3(0.5f);
        sceneObjects.push_back(std::move(Monkey));
       
    }

}