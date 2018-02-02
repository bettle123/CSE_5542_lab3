#pragma once

#include "OpenGL.h"
#include "IRenderTarget.h"
#include "NullCommand.h"
#include "ITextureDataObject.h"
#include "IDepthBuffer.h"
#include <vector>
#include <string>

namespace Crawfis
{
	namespace Graphics 
	{
		//
		// Concrete implementation of IRenderTarget that sets
		// the current render target. Can also be used to set
		// the render target to the frame-buffer to allow for 
		// consistency.
		//
		class RenderTarget : public IRenderTarget
		{
		public:
			//
			// Constructor. All sub-classes require a name.
			//
			RenderTarget(std::vector<ITextureDataObject*>& renderTextures, IDepthBuffer* depthBuffer = 0)
			{
				created = false;
				this->depthBuffer = depthBuffer;
				std::vector<ITextureDataObject*>::iterator textureIterator;
				for( textureIterator = renderTextures.begin(); textureIterator != renderTextures.end(); textureIterator++ )
				{
					textures.push_back(*textureIterator);
				}

				guid = 0;
				enableCommand = &Crawfis::Collections::NullCommand::Instance;
				disableCommand = &Crawfis::Collections::NullCommand::Instance;
			}
			//
			// Constructor for the default frame-buffer (the screen or window).
			//
			RenderTarget()
			{
				created = true;
				guid = 0;
				enableCommand = &Crawfis::Collections::NullCommand::Instance;
				disableCommand = &Crawfis::Collections::NullCommand::Instance;
			}
			// 
			// Apply the Draw mode.
			//
			virtual void Enable()
			{
				if (!created)
					CreateFBO();
				glBindFramebuffer(GL_FRAMEBUFFER, guid);
				enableCommand->Execute();
			}
			// 
			// Remove or undo the application of the draw mode.
			//
			virtual void Disable()
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0 );
				disableCommand->Execute();
			}
			//
			// Set a command to be executed after the render target is enabled.
			//
			void setEnableCommand(Crawfis::Collections::IGenericCommand* enableCommand)
			{
				this->enableCommand = enableCommand;
			}
			//
			// Set a command to be executed after the render target is enabled.
			//
			void setDisableCommand(Crawfis::Collections::IGenericCommand* disableCommand)
			{
				this->disableCommand = disableCommand;
			}

		private:
			void CreateFBO()
			{
				glGenFramebuffers(1, &guid);
				glBindFramebuffer(GL_FRAMEBUFFER, guid);
				// 12/02
				// size of the viewport should be equal to size of the shadow map
				glViewport(0, 0, 1024, 1024);
				// clear the shadow map with default value
				//glClearDepth(1.0);
				//glClear(GL_DEPTH_BUFFER_BIT);

				int attachment = 0;
				std::vector<ITextureDataObject*>::iterator textureIterator;
				for( textureIterator = textures.begin(); textureIterator != textures.end(); textureIterator++ )
				{
					unsigned int textureGuid = (*textureIterator)->GUID();
					// 12/03
					//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
					//	GL_TEXTURE_2D, textureGuid, 0);
					//glDrawBuffer(GL_NONE);
					//glReadBuffer(GL_NONE);
					// 12/02 original
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + attachment,
						GL_TEXTURE_2D, textureGuid, 0);
				
					attachment++;
				}

				if (depthBuffer != 0)
				{
					unsigned int depthGuid = depthBuffer->GUID();
					glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthGuid);
				}
				int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
				if (status != GL_FRAMEBUFFER_COMPLETE){
					throw "Internal Error: The Frame Buffer Object (FBO) was not complete.";
				}
				// 12/03
				//glClearDepth(1.0f); glEnable(GL_DEPTH_TEST);
				//glBindFramebuffer(GL_FRAMEBUFFER, 0);
				//glViewport(0, 0, 800, 600);
			}
		private:
			bool created;
			unsigned int guid;
			Crawfis::Collections::IGenericCommand* enableCommand;
			Crawfis::Collections::IGenericCommand* disableCommand;
			std::vector<ITextureDataObject*> textures;
			IDepthBuffer* depthBuffer;
		};
	}
}