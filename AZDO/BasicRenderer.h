/// Author: Ralph Ridley
/// Date: 31/01/19
#pragma once
#include "AbstractRenderer.h"

namespace QZL
{
	namespace AZDO
	{
		class VaoWrapper;

		class BasicRenderer : public AbstractRenderer<MeshInstance> {
			using Base = AbstractRenderer<MeshInstance>;
		public:
			BasicRenderer(ShaderPipeline* pipeline, VaoWrapper* vao);
			void initialise() override;
			void doFrame(const glm::mat4& viewMatrix) override;
		};
	}
}
