#include "superHeader.hpp"

#include "changeViewport.hpp"
#include "renderScreen.hpp"

void initScreen() {
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutCreateWindow("Be patient");
	glutReshapeFunc(changeViewport);
	glewExperimental = GL_TRUE;
	if (glewInit()) {
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}
	glutFullScreen();
	glutSetCursor(GLUT_CURSOR_NONE);


	
	glutIdleFunc(idle);
	glutDisplayFunc(render);
	
}