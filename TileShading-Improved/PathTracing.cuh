
#include<cuda_runtime.h>
#include<device_launch_parameters.h>

#ifdef _cplusplus
extern "C" {
#endif



class PathTracer
{
	vector<Triangle<Vertex>*> TrianglePrimitives;

public:

	PathTracer() {}
	virtual ~PathTracer() {}

	void Init();
};

#ifdef _cplusplus
}
#endif

