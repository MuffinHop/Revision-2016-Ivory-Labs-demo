#include <windows.h>
#include <stdlib.h> 
#include "VertexObject.hpp"
#include "renderScreen.hpp"
#include "superHeader.hpp"
#include "initScreen.hpp"
#include "Texture.hpp"

GLfloat vertices[] = {
	5.0f, 5.0f, 1.0f,
	5.0f,-5.0f, 1.0f,
	-5.0f,-5.0f, 1.0f,
	-5.0f, 5.0f, 1.0f
};
/*Two random VertexObjects to be created*/
GLfloat basictexCoord[] = {
	1.0f, 1.0f,
	1.0f, 0.0f,
	0.0f ,0.0f,
	0.0f, 1.0f
};
GLuint indices[] = { 0, 1, 2, 3, 2, 0 };
VertexObject *postprocessVO;
Shader *postprocess;
VertexObject *obj0VO;
VertexObject *obj1VO;
VertexObject *obj2VO;
VertexObject *obj3VO;
Shader *shader0;
Shader *shader1;
Shader *shader2;
Shader *shader3;
Shader *shader0B;
Shader *shader1B;
Shader *shader2B;
Shader *shader3B;
Shader *shader0C;
Shader *shader1C;
Shader *shader2C;
Shader *shader3C;
Shader *shader0D;
Shader *shader1D;
Shader *shader2D;
Shader *shader3D;
Shader *shader0E;
Shader *shader1E;
Shader *shader2E;
Shader *shader3E;
Shader *shader0F;
Shader *shader1F;
Shader *shader2F;
Shader *shader3F;
Texture *logo;
int main(int argc, char* argv[]) {
	glutInit(&argc, argv);


	initScreen();
	initRender();
	cout << "----------------\n";
	logo = new Texture("logo.png");
	postprocess = new Shader(buildFragmentShader(readShader("AA.fsh")), buildVertexShader(readShader("test.vsh")));
	shader0 = new Shader(buildFragmentShader(readShader("hall.fsh")), buildVertexShader(readShader("test.vsh")));
	shader1 = new Shader(buildFragmentShader(readShader("hall_b.fsh")), buildVertexShader(readShader("test.vsh")));
	shader2 = new Shader(buildFragmentShader(readShader("hall_c.fsh")), buildVertexShader(readShader("test.vsh")));
	shader3 = new Shader(buildFragmentShader(readShader("hall_d.fsh")), buildVertexShader(readShader("test.vsh")));

	shader0B = new Shader(buildFragmentShader(readShader("whirligig.fsh")), buildVertexShader(readShader("test.vsh")));
	shader1B = new Shader(buildFragmentShader(readShader("whirligig_b.fsh")), buildVertexShader(readShader("test.vsh")));
	shader2B = new Shader(buildFragmentShader(readShader("hall_c.fsh")), buildVertexShader(readShader("test.vsh")));
	shader3B = new Shader(buildFragmentShader(readShader("hall_d.fsh")), buildVertexShader(readShader("test.vsh")));

	shader0C = new Shader(buildFragmentShader(readShader("ball_scene.fsh")), buildVertexShader(readShader("test.vsh")));
	shader1C = new Shader(buildFragmentShader(readShader("ball_scene_b.fsh")), buildVertexShader(readShader("test.vsh")));
	shader2C = new Shader(buildFragmentShader(readShader("hall_c.fsh")), buildVertexShader(readShader("test.vsh")));
	shader3C = new Shader(buildFragmentShader(readShader("hall_d.fsh")), buildVertexShader(readShader("test.vsh")));

	shader0D = new Shader(buildFragmentShader(readShader("flyby.fsh")), buildVertexShader(readShader("test.vsh")));
	shader1D = new Shader(buildFragmentShader(readShader("flyby_b.fsh")), buildVertexShader(readShader("test.vsh")));
	shader2D = new Shader(buildFragmentShader(readShader("hall_c.fsh")), buildVertexShader(readShader("test.vsh")));
	shader3D = new Shader(buildFragmentShader(readShader("hall_d.fsh")), buildVertexShader(readShader("test.vsh")));

	shader0E = new Shader(buildFragmentShader(readShader("tunnel.fsh")), buildVertexShader(readShader("test.vsh")));
	shader1E = new Shader(buildFragmentShader(readShader("tunnel_b.fsh")), buildVertexShader(readShader("test.vsh")));
	shader2E = new Shader(buildFragmentShader(readShader("hall_c.fsh")), buildVertexShader(readShader("test.vsh")));
	shader3E = new Shader(buildFragmentShader(readShader("hall_d.fsh")), buildVertexShader(readShader("test.vsh")));

	shader0F = new Shader(buildFragmentShader(readShader("ribbon.fsh")), buildVertexShader(readShader("test.vsh")));
	shader1F = new Shader(buildFragmentShader(readShader("ribbon_b.fsh")), buildVertexShader(readShader("test.vsh")));
	shader2F = new Shader(buildFragmentShader(readShader("hall_c.fsh")), buildVertexShader(readShader("test.vsh")));
	shader3F = new Shader(buildFragmentShader(readShader("hall_d.fsh")), buildVertexShader(readShader("test.vsh")));

	postprocessVO = new VertexObject(vertices, basictexCoord, indices, sizeofa(vertices) / 3, sizeofa(basictexCoord) / 2, sizeofa(indices));
	(*postprocessVO).attachShaderProgram(postprocess);

	obj0VO = new VertexObject(vertices, basictexCoord, indices, sizeofa(vertices) / 3, sizeofa(basictexCoord) / 2, sizeofa(indices));
	(*obj0VO).attachShaderProgram(shader0);

	obj1VO = new VertexObject(vertices, basictexCoord, indices, sizeofa(vertices) / 3, sizeofa(basictexCoord) / 2, sizeofa(indices));
	(*obj1VO).attachShaderProgram(shader1);

	obj2VO = new VertexObject(vertices, basictexCoord, indices, sizeofa(vertices) / 3, sizeofa(basictexCoord) / 2, sizeofa(indices));
	(*obj2VO).attachShaderProgram(shader2);

	obj3VO = new VertexObject(vertices, basictexCoord, indices, sizeofa(vertices) / 3, sizeofa(basictexCoord) / 2, sizeofa(indices));
	(*obj3VO).attachShaderProgram(shader3);


	glutMainLoop();

	return 0;
}