#pragma once
#include <Rendering/Mesh/FTMesh.h>
#include <Rendering/Scene/Transform/FTTransformUtil.h>

extern "C" {
#include <state_estimate.h>
}

class RocketPathRenderer : public FTMesh<FTVertex> {
public:
	RocketPathRenderer();
	~RocketPathRenderer();

	FTTransform* getTransform() override {
		return transform_;
	}

	void nextStateEstimate(state_estimate_t& current_state);

protected:
	void addVertex(const FTVertex& vertex);

	FTTransformPosition* transform_;
	FTArray<FTVertex>* vertices_;
};
