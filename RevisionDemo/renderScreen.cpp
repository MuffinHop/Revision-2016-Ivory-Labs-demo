#include "superHeader.hpp"

#include "VertexObject.hpp"
#include "Camera.hpp"
#include "Texture.hpp"
#include "renderScreen.hpp"
#include "initScreen.hpp"
#include "changeViewport.hpp"
#include <iomanip>

//gnurocket sync start

#define SYNC_PLAYER

HSTREAM stream;

const float bpm = 122.0f; /* beats per minute */
const int rpb = 4; /* rows per beat */
const double row_rate = (double(bpm) / 60) * rpb;




double bass_get_row(HSTREAM h) {
	QWORD pos = BASS_ChannelGetPosition(h, BASS_POS_BYTE);
	double time = BASS_ChannelBytes2Seconds(h, pos);
	return time * row_rate;
}

#ifndef SYNC_PLAYER
	void bass_pause(void *d, int flag) {
		if (flag)
			BASS_ChannelPause((HSTREAM)d);
		else
			BASS_ChannelPlay((HSTREAM)d, false);
	}

	void bass_set_row(void *d, int row) {
		QWORD pos = BASS_ChannelSeconds2Bytes((HSTREAM)d, row / row_rate);
		BASS_ChannelSetPosition((HSTREAM)d, pos, BASS_POS_BYTE);
	}

	int bass_is_playing(void *d) {
		return BASS_ChannelIsActive((HSTREAM)d) == BASS_ACTIVE_PLAYING;
	}

	struct sync_cb bass_cb = {
		bass_pause,
		bass_set_row,
		bass_is_playing
	};
#endif /* !defined(SYNC_PLAYER) */

void die(const char *fmt, ...){
		char temp[4096];
		va_list va;
		va_start(va, fmt);
		vsnprintf(temp, sizeof(temp), fmt, va);
		va_end(va);

		cout << "*** error: %s\n";

		exit(EXIT_FAILURE);
}
//gnurocket sync end






#define fov 45.0f

GLuint FBO;
GLuint RBO;
GLuint FBO2;
GLuint RBO2;
GLuint FBO2prev;
GLuint RBO2prev;
GLuint FBO3;
GLuint RBO3;
GLuint FBO4;
GLuint RBO4;
extern VertexObject *postprocessVO;

extern VertexObject *obj0VO;
extern VertexObject *obj1VO;
extern VertexObject *obj2VO;
extern VertexObject *obj3VO;

extern Shader *postprocess;

extern Shader *shader0;
extern Shader *shader1;
extern Shader *shader2;
extern Shader *shader3;
extern Shader *shader0B;
extern Shader *shader1B;
extern Shader *shader2B;
extern Shader *shader3B;
extern Shader *shader0C;
extern Shader *shader1C;
extern Shader *shader2C;
extern Shader *shader3C;
extern Shader *shader0D;
extern Shader *shader1D;
extern Shader *shader2D;
extern Shader *shader3D;
extern Shader *shader0E;
extern Shader *shader1E;
extern Shader *shader2E;
extern Shader *shader3E;
extern Shader *shader0F;
extern Shader *shader1F;
extern Shader *shader2F;
extern Shader *shader3F;
extern Texture *logo;
extern HSTREAM stream;
sync_device *rocket;

const struct sync_track
		*iGlobalTime,
		*scene,
		*close_blur,
		*far_blur,
		*cam_rot_x,
		*cam_rot_y,
		*cam_rot_z,
		*cam_dist_x,
		*cam_dist_y,
		*cam_dist_z,
		*dir_rot_x,
		*dir_rot_y,
		*dir_rot_z,
		*gain_r,
		*gain_g,
		*gain_b,
		*field_of_view,
		*gamma_r,
		*gamma_g,
		*gamma_b,
		*lift_r,
		*lift_g,
		*lift_b,
		*saturation_r,
		*saturation_g,
		*saturation_b,
		*light_x,
		*light_y,
		*light_z,
		*moveground,
		*logosync;

bool pause = false;
void getKeyboard(unsigned char key, int x, int y) {
	QWORD pos;
	switch (key) {
	case 27:
		BASS_StreamFree(stream);
		BASS_Free();
		glutLeaveMainLoop();
		break;
	case 'a':
	case 'A':
		//left
		cout << "BACKWARDS\n";
		pos = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
		BASS_ChannelSetPosition(stream, pos - 110000, BASS_POS_BYTE);
		break;
	case 'd':
	case 'D':
		//right
		cout << "FORWARDS\n";
		pos = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
		BASS_ChannelSetPosition(stream, pos + 110000, BASS_POS_BYTE);
		break;
	case 32:
		//pause
		if (!pause) {
			BASS_ChannelPause(stream);
		}
		else {
			BASS_ChannelPlay(stream, false);
		}
		pause = !pause;
		break;

	}
}
float W, H;
void initRender() {
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glGenFramebuffers(1, &FBO2);
	glGenRenderbuffers(1, &RBO2);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO2);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glGenFramebuffers(1, &FBO3);
	glGenRenderbuffers(1, &RBO3);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO3);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glGenFramebuffers(1, &FBO4);
	glGenRenderbuffers(1, &RBO4);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO4);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	glutKeyboardFunc(getKeyboard);



	//music

	BASS_Start();
	BASS_Init(-1, 44100, 0, 0, NULL);
	//BASS_SetVolume(0.1);
	stream = BASS_StreamCreateFile(false, "tune.ogg", 0, 0, 0);


	//music_end

	rocket = sync_create_device("sync");

	/* get tracks */
	iGlobalTime = sync_get_track(rocket, "iGlobalTime");
	scene = sync_get_track(rocket, "scene");
	close_blur = sync_get_track(rocket, "close_blur");
	far_blur = sync_get_track(rocket, "far_blur");
	cam_dist_x = sync_get_track(rocket, "cam.dist.x");
	cam_dist_y = sync_get_track(rocket, "cam.dist.y");
	cam_dist_z = sync_get_track(rocket, "cam.dist.z");
	cam_rot_x = sync_get_track(rocket, "cam.rot.x");
	cam_rot_y = sync_get_track(rocket, "cam.rot.y");
	cam_rot_z = sync_get_track(rocket, "cam.rot.z");
	dir_rot_x = sync_get_track(rocket, "dir.rot.x");
	dir_rot_y = sync_get_track(rocket, "dir.rot.y");
	dir_rot_z = sync_get_track(rocket, "dir.rot.z");
	field_of_view = sync_get_track(rocket, "field_of_view");
	gain_r = sync_get_track(rocket, "gain.r");
	gain_g = sync_get_track(rocket, "gain.g");
	gain_b = sync_get_track(rocket, "gain.b");
	gamma_r = sync_get_track(rocket, "gamma.r");
	gamma_g = sync_get_track(rocket, "gamma.g");
	gamma_b = sync_get_track(rocket, "gamma.b");
	lift_r = sync_get_track(rocket, "lift.r");
	lift_g = sync_get_track(rocket, "lift.g");
	lift_b = sync_get_track(rocket, "lift.b");
	saturation_r = sync_get_track(rocket, "saturation.r");
	saturation_g = sync_get_track(rocket, "saturation.g");
	saturation_b = sync_get_track(rocket, "saturation.b");
	light_x = sync_get_track(rocket, "light.x");
	light_y = sync_get_track(rocket, "light.y");
	light_z = sync_get_track(rocket, "light.z");
	moveground = sync_get_track(rocket, "moveground");
	logosync = sync_get_track(rocket, "logosync");
	Sleep(200);

	W = glutGet(GLUT_WINDOW_WIDTH);
	H = glutGet(GLUT_WINDOW_HEIGHT);


}
void idle() {

	glutPostRedisplay();
}
bool play = false;

void render() {
	if (play == false) {
		BASS_ChannelPlay(stream, false);
		play = true;
	}
	BASS_Update(0);
	QWORD pos = BASS_ChannelGetPosition(stream, BASS_POS_BYTE);
	double row = bass_get_row(stream);
	double time = BASS_ChannelBytes2Seconds(stream, pos);
#ifndef SYNC_PLAYER
	if (sync_update(rocket, (int)floor(row)))
		sync_connect(rocket, "localhost", SYNC_DEFAULT_PORT);
#endif

	float cdx = sync_get_val(cam_dist_x, row);
	float cdy = sync_get_val(cam_dist_y, row);
	float cdz = sync_get_val(cam_dist_z, row);
	float crx = sync_get_val(cam_rot_x, row);
	float cry = sync_get_val(cam_rot_y, row);
	float crz = sync_get_val(cam_rot_z, row);
	float drx = sync_get_val(dir_rot_x, row);
	float dry = sync_get_val(dir_rot_y, row);
	float drz = sync_get_val(dir_rot_z, row);
	

	time = sync_get_val(iGlobalTime, row);

	static Texture renderTargetTexture2history(W, H , 32, false);
	GLuint shaderID;
	cout << "0x" << std::hex << (int)row;
	cout << "; scene:"<< (int)sync_get_val(scene, row) << "\n";
	Texture renderTargetTexture(W, H, GL_RGBA, false);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTargetTexture.getTextureID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_BLEND);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_TRIANGLES);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_TRIANGLES);
	glViewport(0, 0, W, H);

	switch ((int)sync_get_val(scene, row)) {
		case 0:
			shaderID = (*shader0).getShaderProgram();
			(*obj0VO).attachShaderProgram(shader0);
			break;
		case 1:
			shaderID = (*shader0B).getShaderProgram();
			(*obj0VO).attachShaderProgram(shader0B);
			break;
		case 2:
			shaderID = (*shader0C).getShaderProgram();
			(*obj0VO).attachShaderProgram(shader0C);
			break;
		case 3:
			shaderID = (*shader0D).getShaderProgram();
			(*obj0VO).attachShaderProgram(shader0D);
			break;
		case 4:
			shaderID = (*shader0E).getShaderProgram();
			(*obj0VO).attachShaderProgram(shader0E);
			break;
		case 5:
			shaderID = (*shader0F).getShaderProgram();
			(*obj0VO).attachShaderProgram(shader0F);
			break;
		default:
			shaderID = (*shader0).getShaderProgram();
			(*obj0VO).attachShaderProgram(shader0B);
			break;
	}

	glUseProgram(shaderID);

	Camera obj0Camera(fov, W / H, 0.01f, 100.0f);
	obj0Camera.setCameraPosition(vec3(0.0f, 0.0f, -0.1f));
	obj0Camera.setLookAtPosition(vec3(0.0f, 0.0f, 0.0f));

	(*obj0VO).setPosition(vec3(0.0f, 0.0f, 0.0f));
	(*obj0VO).setRotation(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	GLuint resolutionID = glGetUniformLocation(shaderID, "iResolution");
	float width = (float)W;
	float height = (float)H;
	glUniform2f(resolutionID, width, height);
	GLuint timeID = glGetUniformLocation(shaderID, "iGlobalTime");
	glUniform1f(timeID, time);
	GLuint field_of_viewID = glGetUniformLocation(shaderID, "field_of_view");
	glUniform1f(field_of_viewID, sync_get_val(field_of_view, row));
	GLuint cam_distID = glGetUniformLocation(shaderID, "cam_dist");
	GLuint cam_rotID = glGetUniformLocation(shaderID, "cam_rot");
	GLuint dir_rotID = glGetUniformLocation(shaderID, "dir_rot");
	glUniform3f(cam_distID, cdx, cdy, cdz);
	glUniform3f(cam_rotID, crx, cry, crz);
	glUniform3f(dir_rotID, drx, dry, drz);

	GLuint movegroundID = glGetUniformLocation(shaderID, "moveground");
	glUniform1f(movegroundID, sync_get_val(moveground, row));

	GLuint lightSourceID = glGetUniformLocation(shaderID, "lightSource");
	glUniform3f(lightSourceID, sync_get_val(light_x, row), sync_get_val(light_y, row), sync_get_val(light_z, row));

	(*obj0VO).render(obj0Camera);


	/////////////////////////////
	/////////////////////////////
	/////////////////////////////


	Texture renderTargetTexture2(W, H, GL_RGBA, false);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO2);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTargetTexture2.getTextureID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO2);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_TRIANGLES);
	glViewport(0, 0, W, H);

	switch ((int)sync_get_val(scene, row)) {
	case 0:
		shaderID = (*shader1).getShaderProgram();
		(*obj1VO).attachShaderProgram(shader1);
		break;
	case 1:
		shaderID = (*shader1B).getShaderProgram();
		(*obj1VO).attachShaderProgram(shader1B);
		break;
	case 2:
		shaderID = (*shader1C).getShaderProgram();
		(*obj1VO).attachShaderProgram(shader1C);
		break;
	case 3:
		shaderID = (*shader1D).getShaderProgram();
		(*obj1VO).attachShaderProgram(shader1D);
		break;
	case 4:
		shaderID = (*shader1E).getShaderProgram();
		(*obj1VO).attachShaderProgram(shader1E);
		break;
	case 5:
		shaderID = (*shader1F).getShaderProgram();
		(*obj1VO).attachShaderProgram(shader1F);
		break;
	default:
		shaderID = (*shader1).getShaderProgram();
		(*obj1VO).attachShaderProgram(shader1);
		break;
	}

	glUseProgram(shaderID);

	Camera obj1Camera(fov, W / H, 0.01f, 100.0f);
	obj1Camera.setCameraPosition(vec3(0.0f, 0.0f, -0.1f));
	obj1Camera.setLookAtPosition(vec3(0.0f, 0.0f, 0.0f));

	(*obj1VO).setPosition(vec3(0.0f, 0.0f, 0.0f));
	(*obj1VO).setRotation(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	GLuint texlocation1 = glGetUniformLocation(shaderID, "iChannel0");
	glUniform1i(texlocation1, 0);
	GLuint texlocation2 = glGetUniformLocation(shaderID, "iChannel1");
	glUniform1i(texlocation2, 1);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, renderTargetTexture.getTextureID());
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, renderTargetTexture2history.getTextureID());


	resolutionID = glGetUniformLocation(shaderID, "iResolution");
	width = (float)W;
	height = (float)H;
	glUniform2f(resolutionID, width, height);
	timeID = glGetUniformLocation(shaderID, "iGlobalTime");
	glUniform1f(timeID, time);

	(*obj1VO).render(obj1Camera);


	/////////////////////////////
	/////////////////////////////
	/////////////////////////////


	Texture renderTargetTexture3(W, H, GL_RGBA, false);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO3);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTargetTexture3.getTextureID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO3);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_TRIANGLES);
	glViewport(0, 0, W, H);

	switch ((int)sync_get_val(scene, row)) {
	case 0:
		shaderID = (*shader2).getShaderProgram();
		(*obj2VO).attachShaderProgram(shader2);
		break;
	case 1:
		shaderID = (*shader2B).getShaderProgram();
		(*obj2VO).attachShaderProgram(shader2B);
		break;
	case 2:
		shaderID = (*shader2C).getShaderProgram();
		(*obj2VO).attachShaderProgram(shader2C);
		break;
	case 3:
		shaderID = (*shader2D).getShaderProgram();
		(*obj2VO).attachShaderProgram(shader2D);
		break;
	case 4:
		shaderID = (*shader2E).getShaderProgram();
		(*obj2VO).attachShaderProgram(shader2E);
		break;
	case 5:
		shaderID = (*shader2F).getShaderProgram();
		(*obj2VO).attachShaderProgram(shader2F);
		break;
	default:
		shaderID = (*shader2).getShaderProgram();
		(*obj2VO).attachShaderProgram(shader2);
		break;
	}

	glUseProgram(shaderID);

	Camera obj2Camera(fov, W / H, 0.01f, 100.0f);
	obj2Camera.setCameraPosition(vec3(0.0f, 0.0f, -0.1f));
	obj2Camera.setLookAtPosition(vec3(0.0f, 0.0f, 0.0f));

	(*obj2VO).setPosition(vec3(0.0f, 0.0f, 0.0f));
	(*obj2VO).setRotation(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	texlocation1 = glGetUniformLocation(shaderID, "iChannel0");
	glUniform1i(texlocation1, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, renderTargetTexture2.getTextureID());


	resolutionID = glGetUniformLocation(shaderID, "iResolution");
	width = (float)W;
	height = (float)H;
	glUniform2f(resolutionID, width, height);
	timeID = glGetUniformLocation(shaderID, "iGlobalTime");
	glUniform1f(timeID, time);
	GLint close_blurID = glGetUniformLocation(shaderID, "close_blur");
	glUniform1f(close_blurID, sync_get_val(close_blur, row));
	GLint far_blurID = glGetUniformLocation(shaderID, "far_blur");
	glUniform1f(far_blurID, sync_get_val(far_blur, row));
	
	(*obj2VO).render(obj2Camera);



	/////////////////////////////
	/////////////////////////////
	/////////////////////////////


	Texture renderTargetTexture4(W, H, GL_RGBA, false);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO4);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renderTargetTexture4.getTextureID(), 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO4);
	assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_TRIANGLES);
	glViewport(0, 0, W, H);

	switch ((int)sync_get_val(scene, row)) {
	case 0:
		shaderID = (*shader3).getShaderProgram();
		(*obj3VO).attachShaderProgram(shader3);
		break;
	case 1:
		shaderID = (*shader3B).getShaderProgram();
		(*obj3VO).attachShaderProgram(shader3B);
		break;
	case 2:
		shaderID = (*shader3C).getShaderProgram();
		(*obj3VO).attachShaderProgram(shader3C);
		break;
	case 3:
		shaderID = (*shader3D).getShaderProgram();
		(*obj3VO).attachShaderProgram(shader3D);
		break;
	case 4:
		shaderID = (*shader3E).getShaderProgram();
		(*obj3VO).attachShaderProgram(shader3E);
		break;
	case 5:
		shaderID = (*shader3F).getShaderProgram();
		(*obj3VO).attachShaderProgram(shader3F);
		break;
	default:
		shaderID = (*shader3).getShaderProgram();
		(*obj3VO).attachShaderProgram(shader3);
		break;
	}

	glUseProgram(shaderID);

	Camera obj3Camera(fov, W / H, 0.01f, 100.0f);
	obj3Camera.setCameraPosition(vec3(0.0f, 0.0f, -0.1f));
	obj3Camera.setLookAtPosition(vec3(0.0f, 0.0f, 0.0f));

	(*obj3VO).setPosition(vec3(0.0f, 0.0f, 0.0f));
	(*obj3VO).setRotation(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	texlocation1 = glGetUniformLocation(shaderID, "iChannel0");
	glUniform1i(texlocation1, 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, renderTargetTexture3.getTextureID());
	texlocation2 = glGetUniformLocation(shaderID, "iChannel1");
	glUniform1i(texlocation2, 1);
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, (*logo).getTextureID());


	resolutionID = glGetUniformLocation(shaderID, "iResolution");
	width = (float)W;
	height = (float)H;
	glUniform2f(resolutionID, width, height);
	timeID = glGetUniformLocation(shaderID, "iGlobalTime");
	glUniform1f(timeID, time);
	GLuint gainID = glGetUniformLocation(shaderID, "gain");
	glUniform3f(gainID, (float)sync_get_val(gain_r, row) , (float)sync_get_val(gain_g, row), (float)sync_get_val(gain_b, row));
	GLuint gammaID = glGetUniformLocation(shaderID, "gamma");
	glUniform3f(gammaID, (float)sync_get_val(gamma_r, row), (float)sync_get_val(gamma_g, row), (float)sync_get_val(gamma_b, row));
	GLuint liftID = glGetUniformLocation(shaderID, "lift");
	glUniform3f(liftID, (float)sync_get_val(lift_r, row), (float)sync_get_val(lift_g, row), (float)sync_get_val(lift_b, row));
	GLuint saturationID = glGetUniformLocation(shaderID, "saturation");
	glUniform3f(saturationID, (float)sync_get_val(saturation_r, row), (float)sync_get_val(saturation_g, row), (float)sync_get_val(saturation_b, row));

	GLuint logoID = glGetUniformLocation(shaderID, "logo");
	glUniform1f(logoID, (float)sync_get_val(logosync, row));

	(*obj3VO).render(obj3Camera);




	/////////////////////////////
	/////////////////////////////
	/////////////////////////////


	glBindRenderbuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0); //ID = 0 is the window
	glViewport(0, 0, W, H);
	
	GLuint postshaderID = (*postprocess).getShaderProgram();

	glUseProgram(postshaderID);
	texlocation1 = glGetUniformLocation(postshaderID, "iChannel0");
	texlocation2 = glGetUniformLocation(postshaderID, "iChannel1");
	GLuint texlocation3 = glGetUniformLocation(postshaderID, "iChannel2");
	GLuint texlocation4 = glGetUniformLocation(postshaderID, "iChannel3");
	GLuint texlocation5 = glGetUniformLocation(postshaderID, "iChannel4");
	GLuint texlocation6 = glGetUniformLocation(postshaderID, "iChannel5");
	GLuint texlocation7 = glGetUniformLocation(postshaderID, "iChannel6");
	GLuint texlocation8 = glGetUniformLocation(postshaderID, "iChannel7");



	glUniform1i(texlocation1, 0);
	glUniform1i(texlocation2, 1);
	glUniform1i(texlocation3, 2);
	glUniform1i(texlocation4, 3);
	glUniform1i(texlocation5, 4);
	glUniform1i(texlocation6, 5);
	glUniform1i(texlocation7, 6);
	glUniform1i(texlocation8, 7);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, renderTargetTexture4.getTextureID());
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, renderTargetTexture.getTextureID());
	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, renderTargetTexture.getTextureID());
	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, (*logo).getTextureID());

	resolutionID = glGetUniformLocation(postshaderID, "iResolution");
	glUniform2f(resolutionID, width, height);


	Camera finalCamera(fov, W / H, 0.01f, 100.0f);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_TRIANGLES);

	finalCamera.setCameraPosition(vec3(0.0f, 0.0f,-0.1f));
	finalCamera.setLookAtPosition(vec3(0.0f, 0.0f, 0.0f));

	(*postprocessVO).setPosition(vec3(0.0f, 0.0f, 0.0f));
	(*postprocessVO).setRotation(vec4(0.0f, 0.0f, 0.0f, 1.0f));

	(*postprocessVO).render(finalCamera);
	
	//texcopy

	glCopyImageSubData(
		renderTargetTexture2.getTextureID(), GL_TEXTURE_2D, 0, 0, 0, 0,
		renderTargetTexture2history.getTextureID(), GL_TEXTURE_2D, 0, 0, 0, 0,
		(GLint)width, (GLint)height, 1);

	//texcopy
	glutSwapBuffers();
	BASS_Update(0);

	if (row > 1147.5) {
		sync_destroy_device(rocket);

		BASS_StreamFree(stream);
		BASS_Free();
		glutLeaveMainLoop();
	}
}