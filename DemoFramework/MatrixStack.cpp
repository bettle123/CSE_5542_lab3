#include "MatrixStack.h"


		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::modelViewStack;
		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::depthmodelView;
		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::projectionStack;
		std::stack<Matrix4> Crawfis::Graphics::MatrixStack::conditionmatrix;

