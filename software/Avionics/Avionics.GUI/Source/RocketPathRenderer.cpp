#include "RocketPathRenderer.h"
#include <Rendering/Shader/FTShaderCache.h>

RocketPathRenderer::RocketPathRenderer() : FTMesh(FTShaderCache::getSharedInstance()->getShaderProgram<FTVertexShaderProgram>()), transform_(new FTTransformPosition()), vertices_(new FTArray<FTVertex>()) {
	loadEmptyMesh(100);
	setPrimitiveType(GL_LINE_STRIP);

	addVertex(FTVertex());
}

RocketPathRenderer::~RocketPathRenderer() {
	transform_->release();
	vertices_->release();
}

void RocketPathRenderer::nextStateEstimate(state_estimate_t& current_state) {
	FTVertex vertex;
	vertex.position_.x = current_state.pos[0];
	vertex.position_.y = current_state.pos[1];
	vertex.position_.z = current_state.pos[2];
	addVertex(vertex);
}

void RocketPathRenderer::addVertex(const FTVertex& vertex) {
	vertices_->add(vertex);
	if (num_vertices_ < max_num_vertices_)
		modifyVertices(num_vertices_, 1, &vertex);
	else {
		resizeVertexBuffer(max_num_vertices_ * 2, nullptr);
		modifyVertices(0, vertices_->size(), vertices_->getData());
	}
}