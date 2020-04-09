
struct Light {
	int type;			// 0 for directional, 1 for point
	float3 ambientColor;
	float3 diffuseColor;
	float radius;
	float3 direction;	// this will double as position for spot and point
};