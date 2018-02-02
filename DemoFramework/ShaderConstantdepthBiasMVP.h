#pragma once
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
		class ShaderConstantdepthBiasMVP : public IShaderConstant
		{
		public:
			ShaderConstantdepthBiasMVP(std::string constantName) :realConstant(constantName)
			{
			}
			~ShaderConstantdepthBiasMVP()
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
				// 12/02
				
				Matrix4 depthMVP_2;
				Matrix4 modelView = MatrixStack::modelViewStack.top();
				Matrix4 projection = MatrixStack::projectionStack.top();
				Matrix4 depthbiasmvp = projection * modelView;
				Matrix4 biasMatrix = Matrix4(
					0.5, 0.0, 0.0, 0.0, 
					0.0, 0.5, 0.0, 0.0,
					0.0, 0.0, 0.5, 0.0,
					0.5, 0.5, 0.5, 1.0);
				biasMatrix.transpose();
				depthMVP_2 = biasMatrix * depthbiasmvp;
				// 12/05
				// for spot light :
				/*
				glm::vec3 lightPos(3, 3, 3);
				glm::vec3 lightInvDir = glm::vec3(10, 10, 0);
				glm::mat4 depthProjectionMatrix = glm::perspective<float>(45.0f, 1.0f, 2.0f, 50.0f);
				glm::mat4 depthViewMatrix = glm::lookAt(lightPos, lightPos-lightInvDir, glm::vec3(0,1,0));
				
				*/
				//
				//// for directional light
				//glm::vec3 lightInvDir = glm::vec3(10, 10, 0);
				//////// Compute the MVP matrix from the light's point of view
				//glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10, 10, -10, 10, -5, 5);
				//glm::mat4 depthViewMatrix = glm::lookAt(lightInvDir, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
				//glm::mat4 depthModelMatrix = glm::mat4(1.0);
				//glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
				//glm::mat4 XbiasMatrix(
				//	0.5, 0.0, 0.0, 0.0,
				//	0.0, 0.5, 0.0, 0.0,
				//	0.0, 0.0, 0.5, 0.0,
				//	0.5, 0.5, 0.5, 1.0
				//);
				//glm::mat4 depthBiasMVP = XbiasMatrix * depthMVP;

				//depthMVP_2[0] = depthBiasMVP[0][0];
				//depthMVP_2[1] = depthBiasMVP[0][1];
				//depthMVP_2[2] = depthBiasMVP[0][2];
				//depthMVP_2[3] = depthBiasMVP[0][3];
				//depthMVP_2[4] = depthBiasMVP[1][0];
				//depthMVP_2[5] = depthBiasMVP[1][1];
				//depthMVP_2[6] = depthBiasMVP[1][2];
				//depthMVP_2[7] = depthBiasMVP[1][3];
				//depthMVP_2[8] = depthBiasMVP[2][0];
				//depthMVP_2[9] = depthBiasMVP[2][1];
				//depthMVP_2[10] = depthBiasMVP[2][2];
				//depthMVP_2[11] = depthBiasMVP[2][3];
				//depthMVP_2[12] = depthBiasMVP[3][0];
				//depthMVP_2[13] = depthBiasMVP[3][1];
				//depthMVP_2[14] = depthBiasMVP[3][2];
				//depthMVP_2[15] = depthBiasMVP[3][3];

				//depthMVP_2.transpose();


				realConstant.setValue(depthMVP_2);
				realConstant.OnShaderEnabled(shaderID);

			}
		private:
			ShaderConstantMatrix4 realConstant;
		};
	}
}
