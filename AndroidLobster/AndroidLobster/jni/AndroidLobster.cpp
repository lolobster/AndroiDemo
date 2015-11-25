/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>

/** This is the C++ implementation of the Java native method.
@param env Pointer to JVM environment
@param thiz Reference to Java this object
*/
extern "C"

JNIEXPORT jstring JNICALL
Java_fi_lobster_AndroidLobster_headerText( JNIEnv* env, jobject thiz )
{
	return env->NewStringUTF("Hello World");
}

JNIEXPORT void JNICALL
Java_fi_lobster_AndroidLobster_lobsterSquare()
{

}
