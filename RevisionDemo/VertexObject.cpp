#include "VertexObject.hpp"



int maxVertexObjectIndex = 0;
VertexObject::VertexObject(GLfloat *vertices, GLfloat *texCoords, GLuint *indices, GLuint NUM_VERTICES, GLuint NUM_TEXCOORDS, GLuint NUM_INDICES) {
	this->vertices = vertices;
	this->indices = indices;
	this->texCoords = texCoords;
	this->NUM_VERTICES = NUM_VERTICES;
	this->NUM_INDICES = NUM_INDICES;
	this->NUM_TEXCOORDS = NUM_TEXCOORDS;
	this->position = vec3(0.0f, 0.0f, 0.0f);
	this->rotation = vec4(1.0f, 1.0f, 1.0f, 1.0f);


	this->vao = maxVertexObjectIndex++;
	cout << "\n[\n Vertex Array Object: " << this->vao << "\n";
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);



	cout << " Number of vertices: " << this->NUM_VERTICES << "\n";
	cout << " Number of indices: " << this->NUM_INDICES << "\n";
	cout << " Number of texture coordinates: " << this->NUM_TEXCOORDS << "\n";

	cout << "]\n";




	glGenBuffers(1, &this->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
	glBufferData(GL_ARRAY_BUFFER, (2 * this->NUM_TEXCOORDS + 3 * this->NUM_VERTICES) * sizeof(GLfloat), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, 3 * this->NUM_VERTICES * sizeof(GLfloat), this->vertices);
	glBufferSubData(GL_ARRAY_BUFFER, 3 * this->NUM_VERTICES * sizeof(GLfloat), 2 * this->NUM_TEXCOORDS * sizeof(GLfloat), this->texCoords);

	glGenBuffers(1, &this->indxBufID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->indxBufID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->NUM_INDICES * sizeof(GLuint), this->indices, GL_STATIC_DRAW);

}


VertexObject::~VertexObject() {
}


GLuint VertexObject::getVertID() {
	return this->vertID;
}
GLuint VertexObject::getVAO() {
	return this->vao;
}
GLuint VertexObject::getVBO() {
	return this->vbo;
}
GLuint VertexObject::getShader() {
	return (*(this->shader)).getShaderProgram();
}
void VertexObject::render(Camera camera) {
	GLuint shaderID = (*(this->shader)).getShaderProgram();
	glUseProgram(shaderID);

	GLuint transformID = glGetUniformLocation(shaderID, "transform");
	mat4 proj = camera.getMVP();
	glUniformMatrix4fv(transformID, 1, GL_FALSE, &proj[0][0]);

	GLuint cameraPositionID = glGetUniformLocation(shaderID, "cameraPosition");
	vec3 cameraPosition = camera.getCameraPosition();
	glUniform3f(cameraPositionID, cameraPosition.x, cameraPosition.y, cameraPosition.z);

	GLuint objpositionID = glGetUniformLocation(shaderID, "obj_position");
	glUniform3f(objpositionID, this->position.x, this->position.y, this->position.z);

	GLuint objrotationID = glGetUniformLocation(shaderID, "obj_rotation");
	glUniform4f(objrotationID, this->rotation.x, this->rotation.y, this->rotation.z, this->rotation.w);

	glEnableVertexAttribArray(this->vertID);
	glEnableVertexAttribArray(this->textureCoordID);
	glBindVertexArray(this->vao);
	glDrawElements(GL_TRIANGLES, this->NUM_INDICES, GL_UNSIGNED_INT, NULL);
}
void VertexObject::attachShaderProgram(Shader *shader) {
	GLuint shaderID = (*(shader)).getShaderProgram();
	this->shader = shader;
	this->vertID = glGetAttribLocation(shaderID, "s_vPosition");
	this->textureCoordID = glGetAttribLocation(shaderID, "texCoord");
	glVertexAttribPointer(this->vertID, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(this->textureCoordID, 2, GL_FLOAT, GL_FALSE, 0, OFFSET_FIX(3 * this->NUM_VERTICES * sizeof(GLfloat)));
}
GLuint VertexObject::setVertID(GLuint vertID) {
	this->vertID = vertID;
	return this->vertID;
}

vec3 VertexObject::setPosition(vec3 position) {
	this->position = position;
	return position;
}
vec3 VertexObject::getPosition() {
	return this->position;
}
vec4 VertexObject::setRotation(vec4 rotationVector) {
	this->rotation = rotationVector;
	return this->rotation;
}
vec4 VertexObject::getRotation() {
	return this->rotation;
}