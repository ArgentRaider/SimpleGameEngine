#include "Collider.h"

Collider::Collider(double xmin, double ymin, double zmin, double xmax, double ymax, double zmax, Model* model)
	:xmin(xmin), ymin(ymin), zmin(zmin), xmax(xmax), ymax(ymax), zmax(zmax), model(model)
{}
