#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Camera {
	vec3 camPos;
	vec3 front;
	vec3 up;
	vec3 right;
	float halfH;
	float halfW;
	vec3 leftbottom;
	int LoopNum;
};
uniform Camera camera;

struct Ray {
	vec3 origin;
	vec3 direction;
};

uniform float randOrigin;
uint wseed;

struct Sphere {
	vec3 center;
	float radius;
};
uniform Sphere sphere[4];

struct Triangle {
	vec3 v0, v1, v2;
};
Triangle tri;

struct hitRecord {
	vec3 Normal;
	vec3 Pos;
};
hitRecord rec;

// 返回值：ray到球交点的距离
float hitSphere(Sphere s, Ray r);
bool hitWorld(Ray r);
vec3 pathTracing(Ray r);

// 采样历史帧的纹理采样器
uniform sampler2D historyTexture;

/*
uniform sampler2D objMainTexture; //vertex
uniform sampler2D objVertIdxTexture; //indicies
uniform int vertex_count;
uniform int index_count;
*/

uniform samplerBuffer triangles;
#define SIZE_TRIANGLE 3;
uniform int triangles_count;

void main() {
	wseed = uint(randOrigin * float(6.95857) * (TexCoords.x * TexCoords.y));

	// 获取历史帧信息
	vec3 hist = texture(historyTexture, TexCoords).rgb;

	Ray cameraRay;
	cameraRay.origin = camera.camPos;
	cameraRay.direction = normalize(camera.leftbottom + (TexCoords.x * 2.0 * camera.halfW) * camera.right + (TexCoords.y * 2.0 * camera.halfH) * camera.up);

	vec3 curColor = pathTracing(cameraRay);
	curColor = (1.0 / float(camera.LoopNum))*curColor + (float(camera.LoopNum - 1) / float(camera.LoopNum))*hist;
	FragColor = vec4(curColor, 1.0);
}


// ************ 随机数功能 ************** //
float randcore(uint seed) {
	seed = (seed ^ uint(61)) ^ (seed >> uint(16));
	seed *= uint(9);
	seed = seed ^ (seed >> uint(4));
	seed *= uint(0x27d4eb2d);
	wseed = seed ^ (seed >> uint(15));
	return float(wseed) * (1.0 / 4294967296.0);
}
float rand() {
	return randcore(wseed);
}


// ********* 读取纹理相关 ********* //
//写一个函数，按照index读取纹理的值
float getTexValueByIdx(sampler2D tex, float index) {
	float texSize = float(textureSize(tex, 0).x);
	float x = mod(index, texSize) / texSize;
	float y = floor(index / texSize) / texSize;
	return texture(tex, vec2(x, y)).r;
}

Triangle updateTri(int index){
	Triangle tri_t;
	
	/*
	float vertexIndex0 = getTexValueByIdx(objVertIdxTexture, float(index * 3));
	float vertexIndex1 = getTexValueByIdx(objVertIdxTexture, float(index * 3 + 1));
	float vertexIndex2 = getTexValueByIdx(objVertIdxTexture, float(index * 3 + 2));

	// 在ModelTexture的processData中为每个顶点保留了8个float，其中前三个为顶点
	// v0, v1, v2的x, y, z坐标
	tri_t.v0.x = getTexValueByIdx(objMainTexture, vertexIndex0 * 3.0);
	tri_t.v0.y = getTexValueByIdx(objMainTexture, vertexIndex0 * 3.0 + 1.0);
	tri_t.v0.z = getTexValueByIdx(objMainTexture, vertexIndex0 * 3.0 + 2.0);

	tri_t.v1.x = getTexValueByIdx(objMainTexture, vertexIndex1 * 3.0);
	tri_t.v1.y = getTexValueByIdx(objMainTexture, vertexIndex1 * 3.0 + 1.0);
	tri_t.v1.z = getTexValueByIdx(objMainTexture, vertexIndex1 * 3.0 + 2.0);

	tri_t.v2.x = getTexValueByIdx(objMainTexture, vertexIndex2 * 3.0);
	tri_t.v2.y = getTexValueByIdx(objMainTexture, vertexIndex2 * 3.0 + 1.0);
	tri_t.v2.z = getTexValueByIdx(objMainTexture, vertexIndex2 * 3.0 + 2.0);
	*/

	if(index == 0){
		tri_t.v0 = vec3(0.0, 0.0, -1.0);
		tri_t.v1 = vec3(1.0, 0.0, -1.0);
		tri_t.v2 = vec3(-1.0, 1.0, -1.0);
	}else{
		tri_t.v0 = vec3(0.0, 2.0, -1.0);
		tri_t.v1 = vec3(1.0, 2.0, -1.0);
		tri_t.v2 = vec3(-1.0, 3.0, -1.0);
	}
	return tri_t;
}

Triangle getTriangle(int index){
	int offset = index * SIZE_TRIANGLE;
	Triangle t;

	t.v0 = texelFetch(triangles, offset).xyz;
	t.v1 = texelFetch(triangles, offset + 1).xyz;
	t.v2 = texelFetch(triangles, offset + 2).xyz;

	return t;
}

// ********* 击中场景的相关函数 ********* // 

// 返回值：ray到球交点的距离
float hitSphere(Sphere s, Ray r) {
	vec3 oc = r.origin - s.center;
	float a = dot(r.direction, r.direction);
	float b = 2.0 * dot(oc, r.direction);
	float c = dot(oc, oc) - s.radius * s.radius;
	float discriminant = b * b - 4 * a * c;
	if (discriminant > 0.0) {
		float dis = (-b - sqrt(discriminant)) / (2.0 * a);
		if (dis > 0.0) return dis;
		else return -1.0;
	}
	else return -1.0;
}

float hitTriangle(Triangle tri, Ray r) {
	// 找到三角形所在平面法向量
	vec3 A = tri.v1 - tri.v0;
	vec3 B = tri.v2 - tri.v0;
	vec3 N = normalize(cross(A, B));
	// Ray与平面平行，没有交点
	if (dot(N, r.direction) == 0) return -1.0;
	float D = -dot(N, tri.v0);
	float t = -(dot(N, r.origin) + D) / dot(N, r.direction);
	if (t < 0) return -1.0;
	// 计算交点
	vec3 pHit = r.origin + t * r.direction;
	vec3 edge0 = tri.v1 - tri.v0;
	vec3 C0 = pHit - tri.v0;
	if (dot(N, cross(edge0, C0)) < 0) return -1.0;
	vec3 edge1 = tri.v2 - tri.v1;
	vec3 C1 = pHit - tri.v1;
	if (dot(N, cross(edge1, C1)) < 0) return -1.0;
	vec3 edge2 = tri.v0 - tri.v2;
	vec3 C2 = pHit - tri.v2;
	if (dot(N, cross(edge2, C2)) < 0) return -1.0;
	// 光线与Ray相交
	return t - 0.00001;
}

// 返回值：ray到球交点的距离
bool hitWorld(Ray r) {
	float dis = 100000;
	bool hitAnything = false;
	int hitSphereIndex;
	for (int i = 0; i < 4; i++) {
		float dis_t = hitSphere(sphere[i], r);
		if (dis_t > 0 && dis_t < dis) {
			dis = dis_t;
			hitSphereIndex = i;
			hitAnything = true;
		}
	}
	if (hitAnything) {
		rec.Pos = r.origin + dis * r.direction;
		rec.Normal = normalize(r.origin + dis * r.direction - sphere[hitSphereIndex].center);
		return true;
	}
	else return false;
}

bool hitWorldTri(Ray r){
	float dis = 100000;
	bool hitAnything = false;
	//triangles_count
	for (int i = 0; i < 2700; i++){
		float dis_t = hitTriangle(getTriangle(i), r);
		if (dis_t > 0 && dis_t < dis) {
			dis = dis_t;
			hitAnything = true;
		}
	}
	
	if (hitAnything) {
		rec.Pos = r.origin + dis * r.direction;
		rec.Normal = vec3(0.0,1.0,0.0);
		return true;
	}
	return false;
}

vec3 random_in_unit_sphere() {
	vec3 p;
	do {
		p = 2.0 * vec3(rand(), rand() ,rand()) - vec3(1, 1, 1);
	} while (dot(p, p) >= 1.0);
	return p;
}

vec3 pathTracing(Ray r) {
	vec3 color = vec3(1.0,1.0,1.0);
	bool hitAnything = false;
	for (int i = 0; i < 10; i++) {
		if (hitWorldTri(r)) {
			r.origin = rec.Pos;
			r.direction = normalize(rec.Normal + random_in_unit_sphere());
			color *= 0.8;
			hitAnything = true;
		}
		else {
			break;
		}
	}
	if(!hitAnything) color = vec3(0.0, 0.0, 0.0);
	return color;
}