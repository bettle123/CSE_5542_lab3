#pragma once

#include "ITransformNode.h"
#include "IKeyboardHandler.h"
#include "IMouseHandler.h"
#include "OpenGL.h"
#include "ITextureDataObject.h"
#include "TextureBindManager.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include "IDrawable.h"

namespace Crawfis
{
	namespace Graphics
	{
		//
		// A composite ITransformNode used for moving a paint brush
		// Also supports the IKeyboardHandler and IMouseHandler interfaces.
		//
		class DrawableBrush : public IDrawable
		{
		public:
			//
			// Constructor. 
			//
			DrawableBrush(std::vector<glm::vec3> brushPath)
			{
				windowWidth = glutGet(GLUT_WINDOW_WIDTH);
				windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
				//windowWidth = 600;
				//windowHeight = 600;
				isPainting = false;
				this->brushPath = brushPath;
			}
			virtual void Draw() {
				if (brushPath.size() != 0) {
					if (!isPainting)
						CreateBrush();
					int error = glGetError();
					glBindVertexArray(vaoBrush);
					glDrawArrays(GL_POINTS, 0, brushPath.size());
				}
			}
		private:
			void CreateBrush()
			{
				//if (brushPath.size() <= 0)
				//	return;
				// Create a Vertex Array Object to organize all of the bindings.
				glGenVertexArrays(1, &vaoBrush);
				glBindVertexArray(vaoBrush);
				// Steps:
				//  1) Create buffer for vertex positions
				//  2) Set vertex positions to be sent to the shaders at slot 0.
				// Vertex positions, layout(location = 0)
				// Allocate Vertex Buffer Object (get a handle or ID)
				glGenBuffers(1, &vboBrush);
				// VBO for vertex data
				glBindBuffer(GL_ARRAY_BUFFER, vboBrush);
				// Set the model data into the VBO.
				glBufferData(GL_ARRAY_BUFFER, brushPath.size() * sizeof(glm::vec3), &brushPath[0], GL_STATIC_DRAW);
				// Define the layout of the vertex data.
				// This also set's the vertex array's location for slot 0.
				glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
				glEnableVertexAttribArray(0);
				//brushPath.clear();
				// Reset 
				glBindVertexArray(0);
				isPainting = true;
			}
		private:
			bool isPainting = false;
			int windowWidth, windowHeight;
			std::vector<glm::vec3> brushPath;
			unsigned int vaoBrush, vboBrush;
		};
	}
}
