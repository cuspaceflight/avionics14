#include "RocketRenderer.h"

static const glm::vec3 colors[6] = {
	glm::vec3(0.5f, 0.5f, 0.5f),
	glm::vec3(0.5f, 0.5f, 0.5f),
	glm::vec3(0.7f, 0.7f, 0.7f),
	glm::vec3(0.4f, 0.4f, 0.4f),
	glm::vec3(0.6f, 0.6f, 0.6f),
	glm::vec3(0.65f, 0.65f, 0.65f)};

RocketRenderer::RocketRenderer() : FTCube(colors) {
	transform_->setPosition(glm::vec3(0, 0, 0));
	//transform_->setRotationDegrees(glm::vec3(0, 0, 0));
	//transform_->setScale(glm::vec3(10, 10, 10));
}

RocketRenderer::~RocketRenderer() {
}
