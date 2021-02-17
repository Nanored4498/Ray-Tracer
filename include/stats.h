#pragma once

#include <atomic>

// #define STATS

namespace Stats {

extern std::atomic<unsigned long long> sphereRayTest;
extern thread_local unsigned long long localSphereRayTest;
extern std::atomic<unsigned long long> triangleRayTest;
extern thread_local unsigned long long localTriangleRayTest;
extern std::atomic<unsigned long long> nodeRayTest;
extern thread_local unsigned long long localNodeRayTest;
extern std::atomic<unsigned long long> hitBoxTest;
extern thread_local unsigned long long localHitBoxTest;

inline void aggregateLocalStats() {
	sphereRayTest += localSphereRayTest;
	localSphereRayTest = 0;
	triangleRayTest += localTriangleRayTest;
	localTriangleRayTest = 0;
	nodeRayTest += localNodeRayTest;
	localNodeRayTest = 0;
	hitBoxTest += localHitBoxTest;
	localHitBoxTest = 0;
}

#ifdef STATS
#define UPDATE_SPHERE_STATS ++ Stats::localSphereRayTest;
#define UPDATE_TRIANGLE_STATS ++ Stats::localTriangleRayTest;
#define UPDATE_NODE_STATS ++ Stats::localNodeRayTest;
#define UPDATE_BOX_STATS ++ Stats::localHitBoxTest;
#else
#define UPDATE_SPHERE_STATS
#define UPDATE_TRIANGLE_STATS
#define UPDATE_NODE_STATS
#define UPDATE_BOX_STATS
#endif

}