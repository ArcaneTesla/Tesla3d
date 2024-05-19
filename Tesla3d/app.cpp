#include "app.hpp"

#include "keyboard_movement_controller.hpp"
#include "point_light_system.hpp"
#include "simple_render_system.hpp"
#include "tsl_camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>



namespace tsl {

    struct GlobalUbo {
        glm::mat4 projection{ 1.f };
        glm::mat4 view{ 1.f };
        glm::vec4 ambientLightColor{ 1.f,1.f,1.f, .02f };
        glm::vec3 lightPosition{ -2.f,-1.f,-1.f };
        alignas(16) glm::vec4 lightColor{ 1.f };

    };

    FirstApp::FirstApp() {
        globalPool = TslDescriptorPool::Builder(tslDevice)
            .setMaxSets(TslSwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, TslSwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        loadObjects();
    }

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
        auto globalSetLayout = TslDescriptorSetLayout::Builder(tslDevice)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(TslSwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            TslDescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{ tslDevice, tslRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout()};
        PointLightSystem pointLightLSystem{ tslDevice, tslRenderer.getSwapChainRenderPass(), globalSetLayout->getDescriptorSetLayout() };

        TslCamera camera{};

        auto viewerObject = TslSceneObject::createObject();
        viewerObject.transform.translation.z = -3.0f;

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
            camera.setPerspectiveProjection(glm::radians(50.f),aspect,0.1f,100.f);

            if (auto commandBuffer = tslRenderer.beginFrame()) {
                int frameIndex = tslRenderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    sceneObjects,
                };

                //обновление
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                //отрисовка
                tslRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderSceneObjects(frameInfo);
                pointLightLSystem.render(frameInfo);
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
        flatMonkey.transform.translation = { 1.f,.05f,0.f };
        flatMonkey.transform.scale = glm::vec3(0.5f);
        sceneObjects.emplace(flatMonkey.getId(), std::move(flatMonkey));

        TslModel = TslModel::createModelFromFile(tslDevice, "models/blender_monkey.obj");
        auto Monkey = TslSceneObject::createObject();
        Monkey.model = TslModel;
        Monkey.transform.translation = { -1.f,.05f,0.f };
        Monkey.transform.scale = glm::vec3(0.5f);
        sceneObjects.emplace(Monkey.getId(), std::move(Monkey));
        
        TslModel = TslModel::createModelFromFile(tslDevice, "models/quad.obj");
        auto floor = TslSceneObject::createObject();
        floor.model = TslModel;
        floor.transform.translation = { 0.f,0.54f,0.f };
        floor.transform.scale = glm::vec3(2.5f);
        sceneObjects.emplace(floor.getId(), std::move(floor));

    }

}