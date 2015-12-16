#include <jni.h>
#include <android\log.h>

#include <GLES2\gl2.h>
#include <GLES2\gl2ext.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define LOG_TAG		"libgl2jani"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

static void printGLString(const char *name, GLenum s) {
	const char *v = (const char *)glGetString(s);
	LOGI("GL %s = %s\n", name, v);
}

static void checkGlError(const char* op)
{
	for (GLint error = glGetError(); error; error = glGetError())
	{
		LOGI("after %s() glError (0x%x)\n", op, error);
	}
}

static const char gVertexShader[] =
	"attribute vec4 vPosition;\n"
	"void main() {\n"
	"	gl_Position = vPosition;\n"
	"}\n";

static const char gFragmentShader[] =  // SININEN
"precision mediump float;\n"
"void main() {\n"
"	gl_FragColor = vec4(1.0, 0.0, 0.2, 1.0);\n"
"}\n";

static const char gFragmentShader2[] =  // LIILA
"precision mediump float;\n"
"void main() {\n"
"	gl_FragColor = vec4(0.7, 0.0, 1.0, 1.0);\n"
"}\n";

GLuint loadShader(GLenum shaderType, const char* pSource)
{
	GLuint shader = glCreateShader(shaderType);

	if (shader)
	{
		glShaderSource(shader, 1, &pSource, NULL);
		glCompileShader(shader);
		GLint compiled = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

		if (!compiled)
		{
			GLint infoLen = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

			if (infoLen)
			{
				char* buf = (char*)malloc(infoLen);

				if (buf)
				{
					glGetShaderInfoLog(shader, infoLen, NULL, buf);
					LOGE("Could not compile shader %d: \n%s\n", shaderType, buf);
					free(buf);
				}
				glDeleteShader(shader);
				shader = 0;
			}
		}
	}
	return shader;
}

GLuint createProgram(const char* pVertexSource, const char* pFragmentSource)
{
	GLuint vertexShader = loadShader(GL_VERTEX_SHADER, pVertexSource);

	if (!vertexShader)
	{
		return 0;
	}
	GLuint pixelShader = loadShader(GL_FRAGMENT_SHADER, pFragmentSource);

	if (!pixelShader)
	{
		return 0;
	}

	GLuint program = glCreateProgram();

	if (program)
	{
		glAttachShader(program, vertexShader);
		checkGlError("glAttachShader");
		glAttachShader(program, pixelShader);
		checkGlError("glAttachShader");
		glLinkProgram(program);
		GLint linkStatus = GL_FALSE;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus != GL_TRUE)
		{
			GLint bufLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);

			if (bufLength)
			{
				char* buf = (char*)malloc(bufLength);

				if (buf)
				{
					glGetProgramInfoLog(program, bufLength, NULL, buf);
					LOGE("Could not link program:\n%s\n", buf);
					free(buf);
				}
			}
			glDeleteProgram(program);
			program = 0;
		}
	}
	return program;
}

GLuint gProgramBlue;
GLuint gProgramLila;
GLuint gvPositionHandleBlue;
GLuint gvPositionHandleLila;

bool setupGraphics(int w, int h)
{
	printGLString("Version", GL_VERSION);
	printGLString("Vendor", GL_VENDOR);
	printGLString("Renderer", GL_RENDERER);
	printGLString("Extensions", GL_EXTENSIONS);


	//////////////////////////// eka shader
	LOGI("setupGraphics(%d, %d)", w, h);
	gProgramBlue = createProgram(gVertexShader, gFragmentShader);
	if (!gProgramBlue)
	{
		LOGE("Could not create program.");
		return false;
	}
	gvPositionHandleBlue = glGetAttribLocation(gProgramBlue, "vPosition");
	checkGlError("glGetAttribLocation");
	LOGI("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandleBlue);

	/////////////////// toka shader
	gProgramLila = createProgram(gVertexShader, gFragmentShader2);
	if (!gProgramLila)
	{
		LOGE("Could not create program.");
		return false;
	}
	gvPositionHandleLila = glGetAttribLocation(gProgramLila, "vPosition");
	checkGlError("glGetAttribLocation");
	LOGI("glGetAttribLocation(\"vPosition\") = %d\n", gvPositionHandleLila);

	glViewport(0, 0, w, h);
	checkGlError("glViewPort");
	return true;
}

const GLfloat gTriangleVertices[] = {
	0.0f, 0.2f, -0.5f,
	-0.5f, 0.5f, -0.5f
};
GLfloat Red = 1.0;
GLfloat Green = 0.0;
GLfloat Blue = 0.0;
GLfloat Alpha = 1.0;

bool red = false;
bool green = false;
bool blue = false;

void renderFrame()
{
	static float angle = 0.01f;
	GLfloat PI = 3.1415926;

	glClearColor(Red, Green, Blue, Alpha);
	checkGlError("glClearColor");
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	checkGlError("glClear");

	{
		glUseProgram(gProgramLila);
		checkGlError("glUseProgram");

		GLfloat vertexBuffer[6 * 100 * 6];
		
		for (int i = 0; i <= 100; i++)
		{
			float angle2 = 2* PI * i / 100;
			float a = PI / 100;
			const float gTriangleVertices[] =
			{
				cos(angle2)*0.05, sin(angle2)*0.05,
				cos(angle2 + a - 0.1)*0.9, sin(angle2 + a - 0.1)*0.9,
				cos(angle2 - a + 0.1)*0.9, sin(angle2 - a + 0.1)*0.9,
			};
			for (int j = 0; j < 6; j++)
			{
				vertexBuffer[j + i * 6] = gTriangleVertices[j];
			}
		}
		glVertexAttribPointer(gvPositionHandleLila, 2, GL_FLOAT, GL_FALSE, 0, vertexBuffer);
		checkGlError("glVertexAttribPointer");
		glEnableVertexAttribArray(gvPositionHandleLila);
		checkGlError("glEnableVertexAttribArray");
		glDrawArrays(GL_TRIANGLES, 0, 3 * 100);
		checkGlError("glDrawArrays");
	};

	glUseProgram(gProgramBlue);
	checkGlError("glUseProgram");

	glVertexAttribPointer(gvPositionHandleLila, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
	checkGlError("glVertexAttribPointer");
	glEnableVertexAttribArray(gvPositionHandleLila);
	checkGlError("glEnableVertexAttribArray");
	glDrawArrays(GL_TRIANGLES, 0, 3);
	checkGlError("glDraw Arrays");
}

extern "C" {
	JNIEXPORT void JNICALL Java_fi_lobster_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_fi_lobster_GL2JNILib_step(JNIEnv * env, jobject obj);
};

JNIEXPORT void JNICALL Java_fi_lobster_GL2JNILib_init(JNIEnv * env, jobject obj, jint width, jint height)
{
	setupGraphics(width, height);
}

JNIEXPORT void JNICALL Java_fi_lobster_GL2JNILib_step(JNIEnv * env, jobject obj)
{
	////////////////////// RED
	if (Red < 1.0 && !red)
	{
		Red += 0.05;
	}

	else if (Red >= 1.0 || red)
	{
		Red -= 0.05;
		red = true;
	}
	if (Red <= 0.0)
		red = false;

	//////////////////// GREEN
	if (Green < 1.0 && !green)
	{
		Green += 0.025;
	}
	else if (Green >= 1.0 || green)
	{
		Green -= 0.025;
		green = true;
	}
	if (Green <= 0.0)
		green = false;

	///////////////////// BLUE
	if (Blue < 1.0 && !blue)
	{
		Blue += 0.01;
		
	}
	else if (Blue >= 1.0 || blue)
	{
		Blue -= 0.01;
		blue = true;
	}
	if (Blue <= 0.0)
	{
		blue = false;
	}
	renderFrame();
}
