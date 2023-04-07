#include <engine/components.h>
#include <engine/ecs.h>
#include <memory>

using namespace engine;

// ResourceRegistry loadResourceRegistry() { return ResourceRegistry(); }

Registry::Registry() : components(loadComponentRegistry()) {}

//, resources(loadResourceRegistry()) {}
