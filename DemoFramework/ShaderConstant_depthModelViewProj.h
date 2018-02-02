#pragma once
#include "IShaderConstant.h"
#include "ShaderConstantMatrix4.h"
#include "MatrixStack.h"
#include <string>
#include <glm/gtc/matrix_transform.hpp>
using namespace Crawfis::Math;


namespace Crawfis
{
	namespace Graphics
	{
		class ShaderConstantdepthModelViewProj : public IShaderConstant
		{
		public:
			ShaderConstantdepthModelViewProj(std::string constantName) :realConstant(constantName)
			{
			}
			~ShaderConstantdepthModelViewProj()
			{
			}
			//
			// Note that there is no shader bahavior that is publically calleble. This 
			// object is not called directly in the rendering. As an IShaderConstant, 
			// it will be called by the ShaderProgram it is associated with
			//
		protected:
			//
			// Set-up any logic after a shader program has linked.
			// These should not be called by the user.
			//
			virtual void OnLink(unsigned int shaderID)
			{
				realConstant.OnLink(shaderID);
			}
			//
			// Called once a Shader Program has been enabled (glUseProgram)
			// These should not be called by the user.
			
			virtual void OnShaderEnabled(unsigned int shaderID)
			{
				Matrix4 depthMVP_2;
				

				////// Compute the MVP matrix from the light's point of view
				//glm::vec3 lightInvDir = glm::vec3(10, 1, 1);
				//glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);
				//////glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -10, 20);

				//glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				//glm::mat4 depthModelMatrix = glm::mat4(1.0);
				//glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;

				//depthMVP_2[0] = depthMVP[0][0];
				//depthMVP_2[1] = depthMVP[0][1];
				//depthMVP_2[2] = depthMVP[0][2];
				//depthMVP_2[3] = depthMVP[0][3];
				//depthMVP_2[4] = depthMVP[1][0];
				//depthMVP_2[5] = depthMVP[1][1];
				//depthMVP_2[6] = depthMVP[1][2];
				//depthMVP_2[7] = depthMVP[1][3];
				//depthMVP_2[8] = depthMVP[2][0];
				//depthMVP_2[9] = depthMVP[2][1];
				//depthMVP_2[10] = depthMVP[2][2];
				//depthMVP_2[11] = depthMVP[2][3];
				//depthMVP_2[12] = depthMVP[3][0];
				//depthMVP_2[13] = depthMVP[3][1];
				//depthMVP_2[14] = depthMVP[3][2];
				//depthMVP_2[15] = depthMVP[3][3];
				////depthMVP_2.transpose();
				//
				// depthMatrix another way to write
				Matrix4 modelView = MatrixStack::depthmodelView.top();
				Matrix4 projection = MatrixStack::projectionStack.top();
				Matrix4 mvp = projection * modelView;
				depthMVP_2 = mvp;

				//std::cout << "depthMVP" << std::endl;
				//for (int i = 0; i < 16; i++) {
				//	std::cout << mvp[i] << " ";
				//}
				//std::cout << std::endl;
				realConstant.setValue(depthMVP_2);
				realConstant.OnShaderEnabled(shaderID);
				
			}
		private:
			ShaderConstantMatrix4 realConstant;
		};
	}
}