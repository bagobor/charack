#include "CharackTerrain.h"

CharackTerrain::CharackTerrain(int w2, int l2) {
	w = w2;
	l = l2;
	
	hs = new float*[l];
	for(int i = 0; i < l; i++) {
		hs[i] = new float[w];
	}
	
	normals = new Vec3f*[l];
	for(int i = 0; i < l; i++) {
		normals[i] = new Vec3f[w];
	}
	
	computedNormals = false;
}
		
CharackTerrain::~CharackTerrain() {
	for(int i = 0; i < l; i++) {
		delete[] hs[i];
	}
	delete[] hs;
	
	for(int i = 0; i < l; i++) {
		delete[] normals[i];
	}
	delete[] normals;
}
		
int CharackTerrain::width() {
	return w;
}

int CharackTerrain::length() {
	return l;
}


void CharackTerrain::setHeight(int x, int z, float y) {
	hs[z][x] = y;
	computedNormals = false;
}


float CharackTerrain::getHeight(int x, int z) {
	return hs[z][x];
}

void CharackTerrain::computeNormals() {
	if (computedNormals) {
		return;
	}
	
	//Compute the rough version of the normals
	Vec3f** normals2 = new Vec3f*[l];
	for(int i = 0; i < l; i++) {
		normals2[i] = new Vec3f[w];
	}
	
	for(int z = 0; z < l; z++) {
		for(int x = 0; x < w; x++) {
			Vec3f sum(0.0f, 0.0f, 0.0f);
			
			Vec3f out;
			if (z > 0) {
				out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
			}
			Vec3f in;
			if (z < l - 1) {
				in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
			}
			Vec3f left;
			if (x > 0) {
				left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
			}
			Vec3f right;
			if (x < w - 1) {
				right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
			}
			
			if (x > 0 && z > 0) {
				sum += out.cross(left).normalize();
			}
			if (x > 0 && z < l - 1) {
				sum += left.cross(in).normalize();
			}
			if (x < w - 1 && z < l - 1) {
				sum += in.cross(right).normalize();
			}
			if (x < w - 1 && z > 0) {
				sum += right.cross(out).normalize();
			}
			
			normals2[z][x] = sum;
		}
	}
	
	//Smooth out the normals
	const float FALLOUT_RATIO = 0.5f;
	for(int z = 0; z < l; z++) {
		for(int x = 0; x < w; x++) {
			Vec3f sum = normals2[z][x];
			
			if (x > 0) {
				sum += normals2[z][x - 1] * FALLOUT_RATIO;
			}
			if (x < w - 1) {
				sum += normals2[z][x + 1] * FALLOUT_RATIO;
			}
			if (z > 0) {
				sum += normals2[z - 1][x] * FALLOUT_RATIO;
			}
			if (z < l - 1) {
				sum += normals2[z + 1][x] * FALLOUT_RATIO;
			}
			
			if (sum.magnitude() == 0) {
				sum = Vec3f(0.0f, 1.0f, 0.0f);
			}
			normals[z][x] = sum;
		}
	}
	
	for(int i = 0; i < l; i++) {
		delete[] normals2[i];
	}
	delete[] normals2;
	
	computedNormals = true;
}


Vec3f CharackTerrain::getNormal(int x, int z) {
	if (!computedNormals) {
		computeNormals();
	}
	return normals[z][x];
}

void CharackTerrain::loadData(unsigned char *theData) {
	int i = 0;
	for(int y = 0; y < CK_DIM_TERRAIN; y++) {
		for(int x = 0; x < CK_DIM_TERRAIN; x++) {
			//unsigned char color =
				//(unsigned char)image->pixels[3 * (y * image->width + x)];
			//float h = height * ((color / 255.0f) - 0.5f);
			//float h = height * ((color / 255.0f) - 0.5f);
			//if(theData[i] > 10) {
			//	printf("theData[%d] = %u, ", i, theData[i]);
			//}
			setHeight(x, y, theData[i++]);
		}
	}
	
	//delete image;
	computeNormals();
	//return t;
}

void CharackTerrain::render() {
	float scale = 50.0f / max(width() - 1, length() - 1);
	glScalef(scale, scale, scale);
	glTranslatef(-(float)(width() - 1) / 2,
				 0.0f,
				 -(float)(length() - 1) / 2);
	
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < width(); x++) {
			Vec3f normal = getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, getHeight(x, z), z);
			//if(getHeight(x, z) > 5) {
			//	printf("(%.2f),", getHeight(x, z));
			//}
			normal = getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, getHeight(x, z + 1), z + 1);
			//printf("(%.2f),", getHeight(x, z + 1));
		}
		glEnd();
	}
}