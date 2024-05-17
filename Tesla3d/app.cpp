#include "app.hpp"

#include "simple_render_system.hpp"
#include "tsl_camera.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>


#include <stdexcept>

namespace tsl {

    FirstApp::FirstApp() {loadObjects();}

    FirstApp::~FirstApp() {}

	void FirstApp::run() {
        SimpleRenderSystem simpleRenderSystem{ tslDevice, tslRenderer.getSwapChainRenderPass() };
        TslCamera camera{};
        camera.setViewDirection(glm::vec3(0.f), glm::vec3(0.5f, 0.f, 1.f));
        //camera.setViewTarget(glm::vec3(-1.f, -2.f, 2.f), glm::vec3(0.f, 0.f, 2.5f));

		while (!tslWindow.shouldClose()) {
			glfwPollEvents();

            float aspect = tslRenderer.getAspectRatio();
            //ортографика
            //camera.setOrthographicProjection(-aspect, aspect, -1, 1, -1, 1);
            //перспектива
            camera.setPerspectiveProjection(glm::radians(50.f),aspect,0.1f,10.f);

            if (auto commandBuffer = tslRenderer.beginFrame()) {
                tslRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderObjects(commandBuffer, sceneObjects, camera);
                tslRenderer.endSwapChainRenderPass(commandBuffer);
                tslRenderer.endFrame();
            }
		}
        vkDeviceWaitIdle(tslDevice.device());
	}
    std::unique_ptr<TslModel> createCubeModel(TslDevice& device, glm::vec3 offset) {
        std::vector<TslModel::Vertex> vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

        };
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<TslModel>(device, vertices);
    }
    void FirstApp::loadObjects() {
        std::shared_ptr<TslModel> TslModel = createCubeModel(tslDevice, { .0f,.0f,.0f });
        auto cube = TslSceneObject::createObject();
        cube.model = TslModel;
        cube.transform.translation = { .0f,.0f,2.5f };
        cube.transform.scale = { .5f,.5f,.5f };
        sceneObjects.push_back(std::move(cube));
       
    }

}