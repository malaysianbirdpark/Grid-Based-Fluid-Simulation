#include "pch.h"

//_object.push_back(std::move(std::make_shared<Sphere>(Renderer::Context())));
//
//_renderGraph.AddStage(std::move(std::make_shared<CBFluid>()));
//_renderGraph.AddStage(std::move(std::make_shared<Sourcing2DStage>()));
//_renderGraph.AddStage(std::move(std::make_shared<CBTimestep>()));
//_renderGraph.AddStage(std::move(std::make_shared<Advection2DStage>()));
//
//_renderGraph.AddStage(std::move(std::make_shared<CBViscosity>(Renderer::Context())));
//_renderGraph.AddStage(std::move(std::make_shared<Poisson2D2DStage>()));
//_renderGraph.AddStage(std::move(std::make_shared<Poisson2D4DStage>()));
//
//_renderGraph.AddStage(std::move(std::make_shared<Initializer2DStage>()));
//_renderGraph.AddStage(std::move(std::make_shared<Pressure1DStage>()));
//_renderGraph.AddStage(std::move(std::make_shared<CBPoisson>(Renderer::Context())));
//_renderGraph.AddStage(std::move(std::make_shared<Divergence2DStage>()));
//_renderGraph.AddStage(std::move(std::make_shared<Poisson2D1DStage>()));
//_renderGraph.AddStage(std::move(std::make_shared<GradientSubtract2DStage>()));
//
//_renderGraph.AddStage(std::move(std::make_shared<CopyStage>()));
//_renderGraph.AddStage(std::move(std::make_shared<ViewportStage>()));
//
//// full
//{
//	_renderGraph.Link(0, 256, 1, 0);
//	_renderGraph.Link(2, 259, 3, 3);
//	_renderGraph.Link(1, 257, 3, 4);
//	_renderGraph.Link(1, 258, 3, 5);
//
//	_renderGraph.Link(3, 260, 5, 6);
//	_renderGraph.Link(3, 260, 5, 7);
//	_renderGraph.Link(3, 260, 6, 10);
//
//	_renderGraph.Link(4, 262, 5, 8);
//	_renderGraph.Link(4, 262, 6, 11);
//
//	_renderGraph.Link(7, 265, 8, 12);
//	_renderGraph.Link(8, 266, 11, 14);
//
//	_renderGraph.Link(5, 263, 10, 13);
//	_renderGraph.Link(10, 268, 11, 15);
//	_renderGraph.Link(9, 267, 11, 16);
//
//	_renderGraph.Link(11, 269, 12, 17);
//	_renderGraph.Link(5, 263, 12, 18);
//	_renderGraph.Link(12, 270, 1, 1);
//
//	_renderGraph.Link(13, 271, 14, 20);
//	_renderGraph.Link(12, 270, 1, 1);
//
//	_renderGraph.Link(3, 261, 13, 19);
//	_renderGraph.Link(3, 261, 1, 2);
//}
