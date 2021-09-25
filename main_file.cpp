/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "lodepng.h"
#include "shaderprogram.h"
#include "iostream"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

float speed_x = 0;//[radiany/s]
float speed_y = 0;//[radiany/s]
float speed_z = 0;

float x = 0;
float y = 0;

float movet_x = 0;
float movet_y = 0;
float movet_z = 0.0f;


float move_x = 0;
float move_y = 0;
float move_z = 9.0f;

float rotate_kola = 0;

class ModelObject {

public:
	GLuint tex;
	std::vector<glm::vec4> verts;
	std::vector<glm::vec4> norms;
	std::vector<glm::vec2> texCoords;
	std::vector<unsigned int> indices;

	ModelObject(std::string plik, const char* filename) {
		std::cout << "Model "<< plik << "loading..." << std::endl;
		loadModel(plik);
		std::cout << "Texture loading..." << std::endl;
		readTexture(filename);
		std::cout << "Texture loaded..." << std::endl;

	}

	~ModelObject() {}

	void loadModel(std::string plik) {
		Assimp::Importer importer;

		const aiScene* scene = importer.ReadFile(plik, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenSmoothNormals);
		aiMesh* mesh = scene->mMeshes[0];
		std::cout << plik << std::endl;

		for (int i = 0; i < mesh->mNumVertices; i++) {
			aiVector3D vertex = mesh->mVertices[i];
			this->verts.push_back(glm::vec4(vertex.x, vertex.y, vertex.z, 1));

			aiVector3D normal = mesh->mNormals[i];
			this->norms.push_back(glm::vec4(normal.x, normal.y, normal.z, 0));


			unsigned int liczba_zest = mesh->GetNumUVChannels();

			unsigned int wymiar_ws_tex = mesh->mNumUVComponents[0];

			aiVector3D texCoord = mesh->mTextureCoords[0][i];
			this->texCoords.push_back(glm::vec2(texCoord.x, texCoord.y));
		}
		std::cout << "Assimp first loop loading..." << std::endl;

		for (int i = 0; i < mesh->mNumFaces; i++) {
			aiFace& face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++) {
				indices.push_back(face.mIndices[j]);
			}
		}
		std::cout << "Assimp second loop loading..." << std::endl;

	}


	//Funkcja wczytująca teksturę 
	GLuint readTexture(const char* filename) {
		glActiveTexture(GL_TEXTURE0);

		std::vector<unsigned char> image;
		unsigned width, height;
		unsigned error = lodepng::decode(image, width, height, filename);

		glGenTextures(1, &(this->tex));
		glBindTexture(GL_TEXTURE_2D, this->tex);
		glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)image.data());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		return tex;
	}
};

ModelObject* kola_small;
ModelObject* kola_med;
ModelObject* kola_big;
ModelObject* pociag;
ModelObject* terrain;

void drawObject(ModelObject* modelObject, glm::mat4 objectMatrix) {
	glUniformMatrix4fv(spLambertTextured->u("M"), 1, false, value_ptr(objectMatrix));

	glVertexAttribPointer(spLambertTextured->a("vertex"), 4, GL_FLOAT, false, 0, modelObject->verts.data());
	glVertexAttribPointer(spLambertTextured->a("normal"), 4, GL_FLOAT, false, 0, modelObject->norms.data());
	glVertexAttribPointer(spLambertTextured->a("texCoord"), 2, GL_FLOAT, false, 0, modelObject->texCoords.data());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, modelObject->tex);
	glUniform1i(spLambertTextured->u("tex"), 0);
	glDrawElements(GL_TRIANGLES, modelObject->indices.size(), GL_UNSIGNED_INT, modelObject->indices.data());
}

//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void key_callback(
	GLFWwindow* window,
	int key,
	int scancode,
	int action,
	int mod
) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_A:
			std::cout << "left" << std::endl;
			speed_y = -PI;
			break;

		case GLFW_KEY_D:
			std::cout << "left" << std::endl;
			speed_y = PI;
			break;
		case GLFW_KEY_W:
			std::cout << "left" << std::endl;
			speed_x = PI;
			break;
		case GLFW_KEY_S:
			std::cout << "left" << std::endl;
			speed_x = -PI;
			break;
		case GLFW_KEY_R:
			std::cout << "left" << std::endl;
			speed_z = PI;
			break;
		case GLFW_KEY_F:
			std::cout << "left" << std::endl;
			speed_z = -PI;
			break;
		case GLFW_KEY_KP_ADD:
			std::cout << "left" << std::endl;
			movet_z = -PI*4;
			break;
		case GLFW_KEY_KP_SUBTRACT:
			std::cout << "left" << std::endl;
			movet_z = PI*4;
			break;
		}
	}
	if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_A) speed_y = 0;
		if (key == GLFW_KEY_D) speed_y = 0;
		if (key == GLFW_KEY_W) speed_x = 0;
		if (key == GLFW_KEY_S) speed_x = 0;
		if (key == GLFW_KEY_R) speed_z = 0;
		if (key == GLFW_KEY_F) speed_z = 0;
		if (key == GLFW_KEY_KP_ADD) movet_z = 0;
		if (key == GLFW_KEY_KP_SUBTRACT) movet_z = 0;
		if (key == GLFW_KEY_LEFT) movet_x = 0;
		if (key == GLFW_KEY_RIGHT) movet_x = 0;
		if (key == GLFW_KEY_UP) movet_y = 0;
		if (key == GLFW_KEY_DOWN) movet_y = 0;
	}
}

void initObjects() {
	kola_small = new ModelObject(std::string("kola_small.obj"), "kola.png");
	kola_med = new ModelObject(std::string("kola_med.obj"), "kola.png");
	kola_big = new ModelObject(std::string("kola_big.obj"), "kola.png");
	pociag = new ModelObject(std::string("train.obj"), "train.png");
	terrain = new ModelObject(std::string("terrain.obj"), "grass.png");
}

//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {
	initShaders();
	initObjects();
	glClearColor(0.23, 0.51, 0.94, 1); 
	glEnable(GL_DEPTH_TEST); 
	glfwSetKeyCallback(window, key_callback);
}



void freeOpenGLProgram(GLFWwindow* window) {
	freeShaders();

}

void drawScene(GLFWwindow* window, float angle_x, float angle_y, float angle_z) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	spLambertTextured->use();
	glEnableVertexAttribArray(spLambertTextured->a("vertex"));
	glEnableVertexAttribArray(spLambertTextured->a("texCoord"));
	glEnableVertexAttribArray(spLambertTextured->a("normal"));

	glm::vec3 camPosition(move_x, move_y+9, move_z+4);
	glm::vec3 worldUp(0.0f, 3.0f, 0.0f);
	glm::vec3 camFront(0.0f, 3.0f, 0.0f);
	glm::mat4 V = glm::lookAt(
		camPosition,
		camFront,
		worldUp);

	V = glm::rotate(V, angle_x, glm::vec3(1.0f, 0.0f, 0.0f));
	V = glm::rotate(V, angle_y, glm::vec3(0.0f, 1.0f, 0.0f));
	V = glm::rotate(V, angle_z, glm::vec3(0.0f, 0.0f, 1.0f));

	float fov = 50.0f;
	float aspectRatio = 1.0f;
	float nearPlane = 0.1f;
	float farPlane = 1000.0f;

	glm::mat4 P = glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);


	glm::mat4 M = glm::mat4(1.0f);
	M = glm::translate(M, glm::vec3(x, 0.0f, 0.0f));
	M = glm::rotate(M, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	M = glm::rotate(M, rotate_kola, glm::vec3(1.0f, 0.0f, 0.0f));



	glUniformMatrix4fv(spLambertTextured->u("P"), 1, false, glm::value_ptr(P));
	glUniformMatrix4fv(spLambertTextured->u("V"), 1, false, glm::value_ptr(V));
	drawObject(kola_med, M);

	glm::mat4 M2 = glm::mat4(1.0f);
	M2 = glm::translate(M2, glm::vec3(-2.2f + x, 0.3f, 0.0f));
	M2 = glm::rotate(M2, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	M2 = glm::rotate(M2, rotate_kola, glm::vec3(1.0f, 0.0f, 0.0f));
	drawObject(kola_big, M2);

	glm::mat4 M3 = glm::mat4(1.0f);
	M3 = glm::translate(M3, glm::vec3(-8.33f + x, -0.34f, 0.0f));
	M3 = glm::rotate(M3, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	drawObject(pociag, M3);

	glm::mat4 M4 = glm::mat4(1.0f);
	M4 = glm::translate(M4, glm::vec3(-4.83f + x, 0.0f, 0.0f));
	M4 = glm::rotate(M4, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	M4 = glm::rotate(M4, rotate_kola, glm::vec3(1.0f, 0.0f, 0.0f));
	drawObject(kola_med, M4);

	glm::mat4 M5 = glm::mat4(1.0f);
	M5 = glm::translate(M5, glm::vec3(-7.0f + x, 0.0f, 0.0f));
	M5 = glm::rotate(M5, PI / 2, glm::vec3(0.0f, 1.0f, 0.0f));
	M5 = glm::rotate(M5, rotate_kola, glm::vec3(1.0f, 0.0f, 0.0f));
	drawObject(kola_med, M5);

	glm::mat4 M6 = glm::mat4(1.0f);
	M6 = glm::translate(M6, glm::vec3(0.0f, -2.5f, 0.0f));

	drawObject(terrain, M6);



	glDisableVertexAttribArray(spLambertTextured->a("vertex"));
	glDisableVertexAttribArray(spLambertTextured->a("texCoord"));
	glDisableVertexAttribArray(spLambertTextured->a("normal"));


	glfwSwapBuffers(window);
}


int main(void)
{
	GLFWwindow* window;

	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1920, 1920, "Lokomotywa", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	//Główna pętla
	float angle_x = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	float angle_y = 0; //zadeklaruj zmienną przechowującą aktualny kąt obrotu
	float angle_z = 0;

	glfwSetTime(0); //Wyzeruj licznik czasu
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{
		angle_x += speed_x * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki
		angle_y += speed_y * glfwGetTime(); //Oblicz kąt o jaki obiekt obrócił się podczas poprzedniej klatki
		angle_z += speed_z * glfwGetTime();
		move_z += movet_z * float(glfwGetTime());
		move_x += movet_x * float(glfwGetTime());
		move_y += movet_y * float(glfwGetTime());
		rotate_kola += PI * glfwGetTime();

		x += +1*0.02;
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window, angle_x, angle_y, angle_z); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram(window);

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}