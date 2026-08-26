/*
* Escenario con planetario, baño, personajes animados y peces nadando en un estanque
*/
//******************************//
#include <iomanip>
//******************************//
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <vector>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>
#include <model.h>
#include <animatedmodel.h>
#include <material.h>
#include <light.h>
#include <cubemap.h>

#include <irrKlang.h>
#include <text_render.h>
using namespace irrklang;

// Functions
bool Start();
bool Update();

// Funciones para las 3 cámaras y el personaje jugador
Camera* getActiveCamera();
void updatePlayerForward();
void updateCharacterCameras();
glm::vec3 getInteractionPosition();

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// Globals
GLFWwindow* window;

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// =====================
// CÁMARAS
// =====================
// F1 = libre, F2 = primera persona, F3 = tercera persona
Camera camera(glm::vec3(47.6358f, 4.84023f, 75.0113f));
Camera camera1st(glm::vec3(75.923f, 7.7f, -6.389f));
Camera camera3rd(glm::vec3(75.923f, 10.0f, 91.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;
float elapsedTime = 0.0f;
float sunTime = 0.0f;
float orbitAngle = 0.0f;

glm::vec3 position(0.0f, 0.0f, 0.0f);

glm::vec3 forwardView(0.0f, 0.0f, 1.0f);
float     trdpersonOffset = 1.5f;
float     scaleV = 0.025f;
float     rotateCharacter = 0.0f;
float	  door_offset = 0.0f;
float	  door_rotation = 0.0f;
float     asteroid_offset = 0.0f;
float     starTime = 0.0f;

// Shaders
Shader* mLightsShader;
Shader* aguaShader;
Shader* metalShader;
Shader* proceduralShader;
Shader* wavesShader;
Shader* cubemapShader;
Shader* dynamicShader;
Shader* particlesShader;
Shader* textShader;
TextRenderer textRenderer;
Shader* debugShader;

// Modelos estáticos
Model* terreno;
Model* door;
Model* taza;
Model* Bote;
Model* helices;
Model* gridMesh;

Model* sol;
Model* agua;
Model* mercurio;
Model* venus;
Model* tierra;
Model* marte;
Model* jupiter;
Model* saturno;
Model* urano;
Model* neptuno;
Model* asteroide;
Model* asteroide2;
Model* estrella;
Shader* imageShader;

// Modelos animados
AnimatedModel* character01;
AnimatedModel* character02;
AnimatedModel* AstroA;
AnimatedModel* playerCharacter;

// =====================================================
// PECES (configuración del estanque)
// =====================================================
const float POOL_RADIUS = 15.0f;                    // antes: 7.0f (mucho más pequeño)
const glm::vec3 POOL_CENTER(4.0f, -7.3f, -0.234f);     // antes: (-5.5873, 2.5, 3.6328) — fuera del área de las hélices

const char* fishFiles[] = {
	"models/pez.fbx",
	"models/pez2.fbx",
	"models/pez3.fbx",
	"models/pez4.fbx",
	"models/pez5.fbx",
	"models/pez10.fbx",
	"models/guppy.fbx",
	"models/pezPayaso.fbx",
	"models/pez.fbx",
	"models/pez2.fbx",
	"models/pez3.fbx",
	"models/pez4.fbx",
	"models/pez5.fbx",
	"models/pez10.fbx",
	"models/guppy.fbx",
	"models/pezPayaso.fbx"
};
const int NUM_FISH = sizeof(fishFiles) / sizeof(fishFiles[0]);

struct Fish {
	AnimatedModel* model;
	glm::vec3 position;
	glm::vec3 direction;
	float speed;
	float rotationY;
	float scale;
	float bobPhase;
};

std::vector<Fish> fishes;

struct InfoAnimal {
	std::string nombre;
	std::string descripcion;
	glm::vec3   posicion;
	float       radio;
};

std::vector<InfoAnimal> g_infoAnimales;
InfoAnimal* g_animalCercano = nullptr;   // animal dentro del radio
bool g_mostrandoInfo = false;            // ¿el panel está visible?


struct Sala {
	std::string nombre;
	glm::vec3   centro;
	float       radio;
	bool        visitada;   // ya entró alguna vez
	bool        dentro;     // está dentro ahora mismo
};

std::vector<Sala> g_salas;


unsigned int g_texControles = 0;
bool g_mostrandoControles = false;

// =====================================================
// CARTELES DE INFORMACIÓN (con zoom V)
// =====================================================
struct Cartel {
	std::string nombre;
	glm::vec3   posicionTrigger;   // dónde aparece "Presiona V" (lugar lejano)
	glm::vec3   camPosZoom;        // desde dónde mira la cámara al hacer zoom
	glm::vec3   lookAtZoom;        // a qué punto mira la cámara
	float       radio;
};

std::vector<Cartel> g_carteles;
Cartel* g_cartelCercano = nullptr;
bool g_viendoCartel = false;

// Botón de planetas
glm::vec3 g_botonPlanetasPos = glm::vec3(116.167f, 1.3f, -39.4583f);
float g_botonPlanetasRadio = 2.0f;
bool g_cercaBotonPlanetas = false;



// =====================================================



bool  g_museoCompletado = false;     // ¿ya se completaron las 3 salas?
float g_tiempoCompletado = 0.0f;     // momento en que se completó
const float DURACION_MENSAJE = 6.0f; // segundos que dura visible el mensaje
float g_tiempoBienvenida = -1.0f;   // se inicializa en Start()
const float DURACION_BIENVENIDA = 6.0f;

// =====================================================

float tradius = 10.0f;
float theta = 0.0f;
float alpha = 0.0f;

CubeMap* mainCubeMap;
std::vector<Light> gLights;
Material material01;
Material MaterialAgua;
Material MaterialMetalico;
float proceduralTime = 0.0f;
float wavesTime = 0.0f;
bool planetasEnMovimiento = false;  // false = estáticos, true = orbitando

ISoundEngine* SoundEngine = createIrrKlangDevice();
ISound* g_sonidoActual = nullptr;
int g_salaActual = -1; // -1 = ninguna sala
// =====================
// PERSONAJE JUGADOR
// =====================
// Posición inicial del jugador dentro del museo.
// Ajusta la Y si el modelo queda flotando o enterrado.
glm::vec3 playerPosition(47.6013f, 1.3f, 75.3573f);
glm::vec3 playerForward(0.0f, 0.0f, 1.0f);

float playerRotation = 180.0f;
float playerScale = 0.013f;
float playerMoveSpeed = 12.0f;
float playerTurnSpeed = 90.0f;
float playerPitch = 0.0f;   // ángulo vertical de la mirada (grados)

unsigned int g_cuboVAO = 0, g_cuboVBO = 0;
std::vector<AnimatedModel*> g_pecesVitrina;
int g_pezVitrinaIdx = 0;
float g_pezCristalAngulo = 0.0f;
bool g_teclaEPresionada = false;

// Configuración de cámara del jugador
float eyeHeight = 3.5f;
float thirdPersonDistance = 15.0f;
float thirdPersonHeight = 4.0f;
float thirdPersonTargetHeight = 1.5f;

// Si el modelo mira al revés, cambia este valor a 180.0f.
float playerModelYawOffset = 0.0f;

// 0 = cámara libre, 1 = primera persona, 3 = tercera persona
int activeCamera = 0;


void CambiarSonidoSala(int nuevaSala) {
	if (nuevaSala == g_salaActual) return;

	if (g_sonidoActual) {
		g_sonidoActual->stop();
		g_sonidoActual->drop();
		g_sonidoActual = nullptr;
	}

	g_salaActual = nuevaSala;

	const char* archivo = nullptr;
	if (nuevaSala == 0) archivo = "sounds/acuario.wav";
	else if (nuevaSala == 1) archivo = "sounds/planetario.wav";
	else if (nuevaSala == 2) archivo = "sounds/especies.wav";
	else if (nuevaSala == -1) archivo = "sounds/museo.mp3";


	if (archivo)
		g_sonidoActual = SoundEngine->play2D(archivo, true, false, true);
}


int main()
{
	if (!Start())
		return -1;

	while (!glfwWindowShouldClose(window))
	{
		if (!Update())
			break;
	}

	glfwTerminate();
	return 0;
}

bool Start() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Animation", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glEnable(GL_DEPTH_TEST);

	// Shaders
	mLightsShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
	aguaShader = new Shader("shaders/11_PhongShaderMultLights.vs", "shaders/11_PhongShaderMultLights.fs");
	metalShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");
	proceduralShader = new Shader("shaders/12_ProceduralAnimation.vs", "shaders/12_ProceduralAnimation.fs");
	wavesShader = new Shader("shaders/15_picking.vs", "shaders/15_picking.fs");
	cubemapShader = new Shader("shaders/10_vertex_cubemap.vs", "shaders/10_fragment_cubemap.fs");
	dynamicShader = new Shader("shaders/10_vertex_skinning-IT.vs", "shaders/10_fragment_skinning-IT.fs");
	particlesShader = new Shader("shaders/13_particles.vs", "shaders/13_particles.fs");
	textShader = new Shader("shaders/text_shader.vs", "shaders/text_shader.fs");
	textRenderer.Init(textShader->ID, SCR_WIDTH, SCR_HEIGHT, "fonts/cambriab.ttf");
	debugShader = new Shader("shaders/shader_debug.vs", "shaders/shader_debug.fs");
	imageShader = new Shader("shaders/image2d.vs", "shaders/image2d.fs");

	dynamicShader->setBonesIDs(MAX_RIGGING_BONES);
	metalShader->setBonesIDs(MAX_RIGGING_BONES);

	// Modelos estáticos
	terreno = new Model("models/v3.fbx");
	door = new Model("models/Puerta.fbx");
	taza = new Model("models/TazaB.fbx");
	Bote = new Model("models/TapaB.fbx");
	helices = new Model("models/Helices.fbx");
	gridMesh = new Model("models/Agua.fbx");

	sol = new Model("models/Sol.fbx");
	agua = new Model("models/aguapozo.fbx");
	mercurio = new Model("models/Mercurio.fbx");
	venus = new Model("models/Venus.fbx");
	tierra = new Model("models/tierraa.fbx");
	marte = new Model("models/Tierra.fbx");
	jupiter = new Model("models/Jupiter.fbx");
	saturno = new Model("models/Saturno.fbx");
	urano = new Model("models/Urano.fbx");
	neptuno = new Model("models/Neptuno.fbx");
	asteroide = new Model("models/Asteroides.fbx");
	asteroide2 = new Model("models/Asteroide2.fbx");
	estrella = new Model("models/Estrella.fbx");


	// Personajes animados
	character01 = new AnimatedModel("models/Secado.fbx");
	character02 = new AnimatedModel("models/Lavado.fbx");
	AstroA = new AnimatedModel("models/AstroA.fbx");

	// Personaje jugador.
	// Recomendado: renombrar Niño.fbx a Nino.fbx para evitar problemas con la ñ.
	playerCharacter = new AnimatedModel("models/persona.fbx");


	// Cargar textura de controles
	int texW, texH, texChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* texData = stbi_load("textures/Controles.png", &texW, &texH, &texChannels, 0);
	if (texData) {
		glGenTextures(1, &g_texControles);
		glBindTexture(GL_TEXTURE_2D, g_texControles);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0,
			(texChannels == 4 ? GL_RGBA : GL_RGB), GL_UNSIGNED_BYTE, texData);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(texData);
		stbi_set_flip_vertically_on_load(false);
	}
	// =====================================================
	// CARGA DE LOS PECES
	// =====================================================
	for (int i = 0; i < NUM_FISH; i++) {
		Fish f;

		std::cout << "----> Cargando: " << fishFiles[i] << std::endl;
		f.model = new AnimatedModel(fishFiles[i]);

		float angle = ((float)i / (float)NUM_FISH) * 2.0f * 3.14159265f;
		float r = POOL_RADIUS * 0.5f;
		f.position = POOL_CENTER + glm::vec3(cos(angle) * r, 0.0f, sin(angle) * r);

		f.direction = glm::normalize(glm::vec3(-sin(angle), 0.0f, cos(angle)));

		f.speed = 0.8f + (float)((i * 37) % 100) / 100.0f * 1.2f;        // 0.8 a 2.0
		f.rotationY = glm::degrees(atan2(f.direction.x, f.direction.z));
		f.scale = 0.01f + (float)((i * 53) % 100) / 100.0f * 0.005f;  // 0.0025 a 0.0045
		f.bobPhase = (float)i * 1.3f;

		fishes.push_back(f);
	}
	std::cout << "Total peces cargados: " << fishes.size() << std::endl;
	// =====================================================


	// Vitrina de peces
	float hw = 6.0f, hh = 4.0f, hd = 4.0f;
	float cuboVerts[] = {
		-hw,-hh,-hd, -hw,-hh, hd,  hw,-hh, hd,  hw,-hh, hd,  hw,-hh,-hd, -hw,-hh,-hd,
		-hw, hh,-hd,  hw, hh,-hd,  hw, hh, hd,  hw, hh, hd, -hw, hh, hd, -hw, hh,-hd,
		-hw,-hh,-hd, -hw, hh,-hd, -hw, hh, hd, -hw, hh, hd, -hw,-hh, hd, -hw,-hh,-hd,
		 hw,-hh,-hd,  hw,-hh, hd,  hw, hh, hd,  hw, hh, hd,  hw, hh,-hd,  hw,-hh,-hd,
		-hw,-hh, hd, -hw, hh, hd,  hw, hh, hd,  hw, hh, hd,  hw,-hh, hd, -hw,-hh, hd,
		-hw,-hh,-hd,  hw,-hh,-hd,  hw, hh,-hd,  hw, hh,-hd, -hw, hh,-hd, -hw,-hh,-hd
	};
	glGenVertexArrays(1, &g_cuboVAO);
	glGenBuffers(1, &g_cuboVBO);
	glBindVertexArray(g_cuboVAO);
	glBindBuffer(GL_ARRAY_BUFFER, g_cuboVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cuboVerts), cuboVerts, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	const char* pecesVitrinaFiles[] = {
		"models/pez.fbx","models/pez2.fbx","models/pez3.fbx",
		"models/pez4.fbx","models/pez5.fbx","models/pez10.fbx",
		"models/pezPayaso.fbx"
	};
	for (int i = 0; i < 7; i++)
		g_pecesVitrina.push_back(new AnimatedModel(pecesVitrinaFiles[i]));

	// Cubemap
	std::vector<std::string> faces
	{
		"textures/cubemap/03/posx.png",
		"textures/cubemap/03/negx.png",
		"textures/cubemap/03/posy.png",
		"textures/cubemap/03/negy.png",
		"textures/cubemap/03/posz.png",
		"textures/cubemap/03/negz.png"
	};
	mainCubeMap = new CubeMap();
	mainCubeMap->loadCubemap(faces);

	// Inicializar dirección y cámaras del jugador
	updatePlayerForward();
	updateCharacterCameras();



	// Lights
	gLights.clear();

	Light light01;
	light01.Position = glm::vec3(105.810f, 29.408f, -33.957f);
	light01.Color = glm::vec4(1.0f, 0.95f, 0.9f, 1.0f);
	light01.Power = glm::vec4(30.0f);
	light01.distance = 5.0f;
	light01.alphaIndex = 32;
	gLights.push_back(light01);

	Light light02;
	light02.Position = glm::vec3(-1.4336f, 39.766f, -0.178f);
	light02.Color = glm::vec4(1.0f);
	light02.Power = glm::vec4(10.0f);
	light02.distance = 8.0f;
	light02.alphaIndex = 16;
	gLights.push_back(light02);

	Light light03;
	light03.Position = glm::vec3(5.651f, 13.699f, 61.580f);
	light03.Color = glm::vec4(1.0f);
	light03.Power = glm::vec4(25.0f);
	light03.distance = 8.0f;
	light03.alphaIndex = 16;
	gLights.push_back(light03);

	Light light04;
	light04.Position = glm::vec3(104.334f, 29.406f, 51.451f);
	light04.Color = glm::vec4(1.0f);
	light04.Power = glm::vec4(20.0f);
	light04.distance = 10.0f;
	light04.alphaIndex = 8;
	gLights.push_back(light04);

	


	MaterialAgua.ambient = glm::vec4(0.0f, 0.05f, 0.1f, 1.0f);   // azul oscuro suave
	MaterialAgua.diffuse = glm::vec4(0.0f, 0.3f, 0.5f, 1.0f);     // azul agua
	MaterialAgua.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);    // reflejo fuerte
	MaterialAgua.transparency = 0.5f;

	MaterialMetalico.ambient = glm::vec4(0.05f, 0.10f, 0.20f, 1.0f);
	MaterialMetalico.diffuse = glm::vec4(0.10f, 0.35f, 0.60f, 1.0f);
	MaterialMetalico.specular = glm::vec4(0.70f, 0.80f, 1.00f, 1.0f);
	MaterialMetalico.transparency = 0.1f;


	// ===== INFORMACIÓN DE ANIMALES - SALA ACUARIO =====
	g_infoAnimales.push_back({
	"Vaquita Marina",
	"El cetaceo con mayor riesgo en el mundo\nHabita en el Golfo de California\nSu poblacion es menor a 20 ejemplares",
	glm::vec3(25.7459f, 4.2229f, -13.5223f),
	8.0f
		});

	g_infoAnimales.push_back({
		"Orca",
		"Es el delfin de mayor peso que existe\nVive en grupos familiares bien unidos\nGran depredador del ancho mar",
		glm::vec3(18.8443f, 6.07593f, -22.8096f),
		8.0f
		});

	g_infoAnimales.push_back({
		"Ballena",
		"El gigante colosal del planeta\nUsa cantos de baja frecuencia para hablar\nSufre por la caza y el paso de barcos",
		glm::vec3(14.1392f, 6.07593f, -27.1198f),
		8.0f
		});

	g_infoAnimales.push_back({
		"Delfin",
		"Especie marina de gran intelecto\nUsa el eco para rumbos y rutas\nVive en grupos sociales compactos",
		glm::vec3(-11.7558f, 6.07593f, -25.7809f),
		8.0f
		});

	g_infoAnimales.push_back({
		"Tiburon",
		"Habita este mundo desde tiempos remotos\nPosee un olfato de enorme capacidad\nVital para el balance de los mares",
		glm::vec3(-19.0211f, 6.07593f, -18.1533f),
		8.0f
		});

	g_infoAnimales.push_back({
		"Pez Espada",
		"Logra nadar a mas de 90 km por hora\nUsa su largo pico como defensa activa\nBusca aguas de clima calido o templado",
		glm::vec3(-26.7312f, 6.07593f, -3.70558f),
		8.0f
		});

	g_infoAnimales.push_back({
		"Vaquita Marina",
		"Segundo ejemplar que puedes ver aca\nLas redes de pesca bajan su numero\nGran emblema de la proteccion en Mexico",
		glm::vec3(-23.9665f, 6.07593f, 6.46041f),
		8.0f
		});

	g_infoAnimales.push_back({
		"Ajolote",
		"Anfibio mexicano bajo grave peligro\nSana y duplica sus miembros de forma facil\nHabitante de los lagos de Xochimilco",
		glm::vec3(-12.1983f, 6.07593f, 21.8934f),
		8.0f
		});

	g_infoAnimales.push_back({
		"Pulpo",
		"Posee tres corazones y sangre azul\nLogra variar su color de forma veloz\nUn ser sin huesos de enorme intelecto",
		glm::vec3(-4.0152f, 6.07593f, 26.184f),
		8.0f
		});

	g_infoAnimales.push_back({
	"Tortuga Gigante",
	"Su vida sobrepasa los 100 abriles\nEspecie bajo resguardo de forma global\nSufre por basura de plastico en el mar",
	glm::vec3(11.1258f, 6.07593f, 22.8601f),   // <-- ESTA AÚN HAY QUE REMEDIRLA
	8.0f
		});

	g_infoAnimales.push_back({
		"Rana",
		"Muestra el estado real del ecosistema\nLogra respirar por medio de su piel\nGran parte de ellas sufren riesgo",
		glm::vec3(21.5307f, 6.07593f, 19.7936f),   // <-- corregida
		8.0f
		});

	g_infoAnimales.push_back({
		"Bienvenido al Acuario",
		"Mira la sala y camina ante cada ser\nPulsas F para ver los datos de cada uno\nCuidemos juntos el entorno marino",
		glm::vec3(27.9666f, 5.78948f, -13.0701f),    // <-- corregida
		8.0f
		});
	// ===== FIN INFORMACIÓN =====

// ===== INFORMACION - SALA ANIMALES EN PELIGRO / EXTINTOS =====
	g_infoAnimales.push_back({
		"Rinoceronte Blanco",
		"Quedan muy pocos ejemplares del tipo del norte\nSufre por la caza ilegal debido a su cuerno\nHabita en las sabanas del suelo africano",
		glm::vec3(86.4472f, 1.0f, 41.9314f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Oso Polar",
		"Depende del hielo marino para buscar alimento\nSufre por el cambio del clima global\nPuede nadar mas de 100 km sin una pausa",
		glm::vec3(86.8881f, 1.0f, 55.286f),
		5.0f
		});

	g_infoAnimales.push_back({
			"Mamut",
			"Especie extinta desde tiempos remotos\nVivio durante la era del hielo glaciar\nPariente lejano del elefante actual",
			glm::vec3(98.8414f, 1.0f, 61.0833f),
			5.0f
		});

	g_infoAnimales.push_back({
		"Oso Panda",
		"Se alimenta casi por completo de bambu\nEmblema mundial de la conservacion verde\nHabita en las zonas altas de China",
		glm::vec3(112.966f, 1.0f, 63.3401f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Lobo Mexicano",
		"El lobo con mayor riesgo en todo el norte\nEstuvo cerca de la extincion total\nSu numero crece gracias a programas de cria",
		glm::vec3(123.284f, 1.0f, 61.5831f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Oso Negro",
		"Especie bajo resguardo en Mexico\nSufre por la perdida de zonas boscosas\nExperto al trepar grandes arboles",
		glm::vec3(123.995f, 1.0f, 52.7835f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Quetzal",
		"Ave sagrada para las culturas del pasado\nSufre por la perdida de su entorno natural\nHabita en los bosques con mucha niebla",
		glm::vec3(125.736f, 1.0f, 32.8425f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Guacamaya",
		"Ave de gran color en peligro por el comercio ilegal\nPuede vivir mas de 50 abriles\nHabita en zonas de selva tropical",
		glm::vec3(117.674f, 1.0f, 27.6653f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Lemur",
		"Primate exclusivo de la zona de Madagascar\nMuchas de sus variantes sufren un riesgo critico\nSufre por la tala de arboles en su entorno",
		glm::vec3(109.926f, 1.0f, 22.5851f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Tigre Dientes de Sable",
		"Especie ausente del mundo desde hace mucho tiempo\nPoseia colmillos de hasta 20 cm de largo\nGran cazador durante la era glaciar",
		glm::vec3(100.058f, 1.0f, 20.4944f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Conejo de los Volcanes",
		"Dos especies nativas de Mexico en peligro\nSuelen vivir en zonas de bosque y volcan\nSufren por el danio a su entorno silvestre",
		glm::vec3(129.264f, 1.0f, 42.1742f),
		5.0f
		});

	g_infoAnimales.push_back({
		"Mariposa Monarca",
		"Realiza un viaje de miles de kilometros\nSufre por la perdida de su entorno natural\nPasa el invierno en los bosques de Mexico",
		glm::vec3(84.9008f, 6.07593f, 38.5078f),
		5.0f
		});


// ===== SALAS =====
	g_salas.push_back({ "Acuario",
		glm::vec3(-1.4336f, 1.3f, -0.178644f), 40.0f, false, false });

	g_salas.push_back({ "Planetario",
		glm::vec3(101.726f, 4.79888f, -33.8511f),  35.0f, false, false });

	g_salas.push_back({ "Especies Extintas y en Peligro de Extincion",
		glm::vec3(106.333f, 1.3f, 48.1119f), 35.0f, false, false });
	// ===== FIN SALAS =====


// ===== CARTELES =====
	g_carteles.push_back({
		"Cartel del satelite",   // <-- cambia al tema que tenga tu cartel
		glm::vec3(105.396f, 1.3f, -30.8592f),     // trigger: aparece "Presiona V" cuando te acercas
		glm::vec3(102.55f, 3.5f, -16.9963f),       // camPosZoom: desde dónde mira la cámara
		glm::vec3(102.273f, 5.0f, -11.9435f),      // lookAtZoom: punto exacto del cartel
		3.0f                                        // radio del trigger
		});

	g_carteles.push_back({
	"Cartel del telescopio",
	glm::vec3(95.2187f, 1.3f, -37.5728f),
	glm::vec3(88.8614f, 3.5, -44.7571f),
	glm::vec3(85.3006f, 5.0f, -46.6244f),
	3.0f
		});


	g_carteles.push_back({
	"Cartel de Capas de la Tierra",
	glm::vec3(105.332f, 1.3f, -43.7668f),      // trigger: personaje cerca del cartel
	glm::vec3(106.597f, 3.5f, -54.5148f),  // camPosZoom: camara enfrente del cartel
	glm::vec3(106.623f, 5.0f, -59.1159f),       // lookAtZoom: punto al que apunta
	3.0f
		});

	g_carteles.push_back({
	"Cartel del cohete",
	glm::vec3(116.799f, 1.3f, -43.6584f),
	glm::vec3(121.0f, 4.83294f, -49.1166f),
	glm::vec3(121.0f, 5.5f, -55.0f),
	3.0f
		});
	// ===== FIN CARTELES =====

	g_tiempoBienvenida = (float)glfwGetTime();
	return true;

}

void SetLightUniformInt(Shader* shader, const char* propertyName, size_t lightIndex, int value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setInt(uniformName.c_str(), value);
}
void SetLightUniformFloat(Shader* shader, const char* propertyName, size_t lightIndex, float value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setFloat(uniformName.c_str(), value);
}
void SetLightUniformVec4(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec4 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setVec4(uniformName.c_str(), value);
}
void SetLightUniformVec3(Shader* shader, const char* propertyName, size_t lightIndex, glm::vec3 value) {
	std::ostringstream ss;
	ss << "allLights[" << lightIndex << "]." << propertyName;
	std::string uniformName = ss.str();
	shader->setVec3(uniformName.c_str(), value);
}


//SE AGREGO ESTO PARA PODER HACER LOS CUADROS DE INFORMACION
void DrawQuad(float x, float y, float width, float height, glm::vec4 color, glm::mat4 projection)
{
	float vertices[] = {
		x,         y,          0.0f,
		x + width, y,          0.0f,
		x + width, y - height, 0.0f,
		x,         y - height, 0.0f
	};
	unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };

	unsigned int quadVAO, quadVBO, quadEBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glGenBuffers(1, &quadEBO);

	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	debugShader->use();
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 viewQ = glm::mat4(1.0f);
	debugShader->setMat4("model", model);
	debugShader->setMat4("view", viewQ);
	debugShader->setMat4("projection", projection);
	debugShader->setVec3("color", glm::vec3(color.r, color.g, color.b));
	debugShader->setFloat("alpha", color.a);
	debugShader->setBool("useAlpha", true);

	glBindVertexArray(quadVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDeleteVertexArrays(1, &quadVAO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &quadEBO);
}





bool Update() {

	float currentFrame = (float)glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;
	elapsedTime += deltaTime;

	processInput(window);
	updateCharacterCameras();

	glm::vec3 detectorPosition = getInteractionPosition();

	g_animalCercano = nullptr;
	for (auto& animal : g_infoAnimales) {
		float distancia = glm::distance(detectorPosition, animal.posicion);
		if (distancia < animal.radio) {
			g_animalCercano = &animal;
			break;
		}
	}
	if (g_animalCercano == nullptr) {
		g_mostrandoInfo = false;
	}
	// Detectar cartel cercano (para mostrar "Presiona V")
	g_cartelCercano = nullptr;
	for (auto& cartel : g_carteles) {
		float distancia = glm::distance(detectorPosition, cartel.posicionTrigger);
		if (distancia < cartel.radio) {
			g_cartelCercano = &cartel;
			break;
		}
	}
	// Si te alejas y estabas en modo zoom, salir
	if (g_cartelCercano == nullptr) {
		g_viendoCartel = false;
	}
	float distBoton = glm::distance(detectorPosition, g_botonPlanetasPos);
	g_cercaBotonPlanetas = (distBoton < g_botonPlanetasRadio);

	//SALAS DE VISITA
	for (auto& sala : g_salas) {
		float distancia = glm::distance(detectorPosition, sala.centro);
		if (distancia < sala.radio) {
			sala.dentro = true;
			sala.visitada = true;   // una vez dentro, queda marcada como visitada para siempre
		}
		else {
			sala.dentro = false;
		}
	}


	// Sonido ambiental por sala
	int salaDetectada = -1;
	for (int i = 0; i < (int)g_salas.size(); i++) {
		if (g_salas[i].dentro) {
			salaDetectada = i;
			break;
		}
	}
	CambiarSonidoSala(salaDetectada);

	// Detectar el momento exacto en que se completan las 3 salas
	// Detectar cuando las 3 salas están en VERDE (visitadas y ya salió de todas)
	if (!g_museoCompletado) {
		bool todasVerdes = true;
		for (auto& sala : g_salas) {
			// verde = visitada Y no estás dentro de ella ahora
			if (!sala.visitada || sala.dentro) {
				todasVerdes = false;
				break;
			}
		}
		if (todasVerdes) {
			g_museoCompletado = true;
			g_tiempoCompletado = (float)glfwGetTime();
		}
	}




	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection;
	glm::mat4 view;

	Camera* activeCam = getActiveCamera();

	projection = glm::perspective(
		glm::radians(activeCam->Zoom),
		(float)SCR_WIDTH / (float)SCR_HEIGHT,
		0.1f,
		10000.0f
	);

	if (g_viendoCartel && g_cartelCercano != nullptr) {
		// Modo zoom: cámara fija frente al cartel
		view = glm::lookAt(g_cartelCercano->camPosZoom,
			g_cartelCercano->lookAtZoom,
			glm::vec3(0.0f, 1.0f, 0.0f));
	}
	else {
		view = activeCam->GetViewMatrix();
	}

	// Cubemap (fondo)
	{
		mainCubeMap->drawCubeMap(*cubemapShader, projection, view);
	}

	// Modelos con Phong (planetario, asteroides)
	{
		mLightsShader->use();

		mLightsShader->setMat4("projection", projection);
		mLightsShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
		mLightsShader->setMat4("model", model);

		mLightsShader->setInt("numLights", (int)gLights.size());
		for (size_t i = 0; i < gLights.size(); ++i) {
			SetLightUniformVec3(mLightsShader, "Position", i, gLights[i].Position);
			SetLightUniformVec3(mLightsShader, "Direction", i, gLights[i].Direction);
			SetLightUniformVec4(mLightsShader, "Color", i, gLights[i].Color);
			SetLightUniformVec4(mLightsShader, "Power", i, gLights[i].Power);
			SetLightUniformInt(mLightsShader, "alphaIndex", i, gLights[i].alphaIndex);
			SetLightUniformFloat(mLightsShader, "distance", i, gLights[i].distance);
		}

		mLightsShader->setVec3("eye", activeCam->Position);
		mLightsShader->setVec4("MaterialAmbientColor", material01.ambient);
		mLightsShader->setVec4("MaterialDiffuseColor", material01.diffuse);
		mLightsShader->setVec4("MaterialSpecularColor", material01.specular);
		mLightsShader->setFloat("transparency", material01.transparency);

		// Puertas, taza, bote 
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(28.560f, 5.761f, 62.455 + door_offset));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		door->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-15.19f, 5.761f, 62.455 + door_offset));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		door->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(31.886f, 2.1461f, 71.962f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(door_rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		taza->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-9.3275f, 1.27f, 70.169 + door_offset));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(door_rotation), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		Bote->Draw(*mLightsShader);


		// ASTEROIDES
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(107.148f, 27.423f, -28.322f + asteroid_offset));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		mLightsShader->setMat4("model", model);
		asteroide->Draw(*mLightsShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(111.069f, 26.035f, -46.411f + asteroid_offset));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		mLightsShader->setMat4("model", model);
		asteroide2->Draw(*mLightsShader);

		// Baño
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, 0, 0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		mLightsShader->setMat4("model", model);
		terreno->Draw(*mLightsShader);
	}

	glUseProgram(0);
	{
		proceduralShader->use();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		proceduralShader->setMat4("projection", projection);
		proceduralShader->setMat4("view", view);

		glm::vec3 sunPos = glm::vec3(108.0f, 24.009f, -35.915f);



		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(-29.0f, 9.99f, 62.082f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		proceduralShader->setInt("mode", 1);
		proceduralShader->setMat4("model", model);
		proceduralShader->setFloat("time", proceduralTime);
		helices->Draw(*proceduralShader);

		glm::mat4 model2 = glm::mat4(1.0f);
		model2 = glm::translate(model2, glm::vec3(14.446, 9.9f, 62.082f));
		model2 = glm::rotate(model2, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		proceduralShader->setInt("mode", 1);
		proceduralShader->setMat4("model", model2);
		proceduralShader->setFloat("time", proceduralTime);
		helices->Draw(*proceduralShader);

		model = glm::mat4(1.0f);
		model = glm::translate(model, sunPos);
		model = glm::rotate(model, sunTime, glm::vec3(0, 1, 0));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
		proceduralShader->setInt("mode", 1); // Solo rotación propia
		proceduralShader->setMat4("model", model);
		proceduralShader->setFloat("time", sunTime);
		sol->Draw(*proceduralShader);



		//Planetas
		proceduralShader->use();
		proceduralShader->setMat4("projection", projection);
		proceduralShader->setMat4("view", view);
		proceduralShader->setInt("mode", 3);

		auto drawPlanet = [&](Model* planeta, float orbitRadius, float orbitSpeed,
			float planetScale, float startAngle)
			{
				float ang = startAngle + proceduralTime * orbitSpeed;
				glm::vec3 orbitPos = sunPos + glm::vec3(cos(ang) * orbitRadius,
					0.0f,
					sin(ang) * orbitRadius);

				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, orbitPos);
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
				model = glm::scale(model, glm::vec3(planetScale));

				proceduralShader->setMat4("model", model);
				proceduralShader->setFloat("time", proceduralTime);
				planeta->Draw(*proceduralShader);
			};

		//            modelo    radio  velocidad  escala   ángulo inicial
		drawPlanet(mercurio, 5.0f, 10.0f, 0.55f, 0.6f);
		drawPlanet(venus, 5.5f, 9.5f, 0.60f, 0.8f);
		drawPlanet(tierra, 6.0f, 8.0f, 0.45f, 1.6f);
		drawPlanet(marte, 7.5f, 6.5f, 0.40f, 2.4f);
		drawPlanet(jupiter, 9.5f, 5.0f, 1.05f, 3.2f);
		drawPlanet(saturno, 11.5f, 4.0f, 1.00f, 4.0f);
		drawPlanet(urano, 13.0f, 3.0f, 0.60f, 4.8f);
		drawPlanet(neptuno, 14.5f, 2.5f, 0.60f, 5.6f);

		if (planetasEnMovimiento)
			proceduralTime += 0.001f;





	}




	glUseProgram(0);

	// Estrellas (partículas)
	{
		particlesShader->use();
		particlesShader->setMat4("projection", projection);
		particlesShader->setMat4("view", view);
		particlesShader->setFloat("time", (float)glfwGetTime());
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(109.0f, 24.259f, -30.915f));
		particlesShader->setMat4("model", model);
		estrella->Draw(*particlesShader);
	}

	glUseProgram(0);



	// Personajes animados (Secado / Lavado)
	{
		//Secado
		character01->UpdateAnimation(deltaTime);
		dynamicShader->use();
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::translate(model, glm::vec3(6.458f, 6.003f, 70.88f));
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

		dynamicShader->setMat4("model", model);
		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, character01->gBones);
		character01->Draw(*dynamicShader);
	}

	glUseProgram(0);

	{
		//Lavado
		character02->UpdateAnimation(deltaTime);
		dynamicShader->use();
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, position);
		model = glm::translate(model, glm::vec3(3.6702f, 6.1374f, 63.986f));
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

		dynamicShader->setMat4("model", model);
		dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, character02->gBones);
		character02->Draw(*dynamicShader);
	}

	glUseProgram(0);

	// Astronauta
	{
		AstroA->UpdateAnimation(deltaTime);

		metalShader->use();
		metalShader->setMat4("projection", projection);
		metalShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(124.75f, 1.5f, -23.011f));
		model = glm::rotate(model, glm::radians(rotateCharacter), glm::vec3(0.0, 1.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.01f, 0.01f, 0.01f));

		metalShader->setMat4("model", model);
		metalShader->setMat4("gBones", MAX_RIGGING_BONES, AstroA->gBones);
		metalShader->setVec3("eye", activeCam->Position);

		metalShader->setVec4("MaterialAmbientColor", MaterialMetalico.ambient);
		metalShader->setVec4("MaterialDiffuseColor", MaterialMetalico.diffuse);
		metalShader->setVec4("MaterialSpecularColor", MaterialMetalico.specular);
		metalShader->setFloat("transparency", MaterialMetalico.transparency);

		AstroA->Draw(*metalShader);
	}
	glUseProgram(0);
	// =====================================================
	// PECES NADANDO EN EL ESTANQUE
	// =====================================================
	{
		dynamicShader->use();
		dynamicShader->setMat4("projection", projection);
		dynamicShader->setMat4("view", view);

		for (auto& f : fishes) {
			// 1) Animación de keyframes
			f.model->UpdateAnimation(deltaTime);

			// 2) Movimiento procedural en plano XZ
			glm::vec3 nextPos = f.position + f.direction * f.speed * deltaTime;

			// 3) Rebote en el círculo (reflexión vectorial)
			glm::vec3 toCenter = nextPos - POOL_CENTER;
			toCenter.y = 0.0f;
			float dist = glm::length(toCenter);

			if (dist >= POOL_RADIUS) {
				glm::vec3 normal = glm::normalize(toCenter);
				f.direction = glm::normalize(glm::reflect(f.direction, normal));
				nextPos = POOL_CENTER + normal * (POOL_RADIUS - 0.01f);
				nextPos.y = f.position.y;
				f.rotationY = glm::degrees(atan2(f.direction.x, f.direction.z));
			}
			f.position = nextPos;

			// 4) Oscilación vertical
			float bobY = sin(elapsedTime * 1.5f + f.bobPhase) * 0.15f;

			// 5) Matriz del modelo
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, f.position + glm::vec3(0.0f, bobY, 0.0f));
			model = glm::rotate(model, glm::radians(f.rotationY), glm::vec3(0, 1, 0));
			model = glm::scale(model, glm::vec3(f.scale));

			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, f.model->gBones);

			f.model->Draw(*dynamicShader);
		}
	}

	glUseProgram(0);
	// =====================================================
	// Agua de la llave
	{
		wavesShader->use();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		wavesShader->setMat4("projection", projection);
		wavesShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(1.3114f, 4.440f, 64.948f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));
		wavesShader->setMat4("model", model);

		wavesShader->setFloat("time", wavesTime);
		wavesShader->setFloat("radius", 5.0f);
		wavesShader->setFloat("height", 5.0f);

		gridMesh->Draw(*wavesShader);
		wavesTime += 0.01f;
	}

	glUseProgram(0);

	{
		aguaShader->use();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		aguaShader->setMat4("projection", projection);
		aguaShader->setMat4("view", view);

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(2.11f, -5.3f, 0.0f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(17.0f, 18.3f, 5.0f));
		aguaShader->setMat4("model", model);

		aguaShader->setInt("numLights", (int)gLights.size());
		for (size_t i = 0; i < gLights.size(); ++i) {
			SetLightUniformVec3(aguaShader, "Position", i, gLights[i].Position);
			SetLightUniformVec3(aguaShader, "Direction", i, gLights[i].Direction);
			SetLightUniformVec4(aguaShader, "Color", i, gLights[i].Color);
			SetLightUniformVec4(aguaShader, "Power", i, gLights[i].Power);
			SetLightUniformInt(aguaShader, "alphaIndex", i, gLights[i].alphaIndex);
			SetLightUniformFloat(aguaShader, "distance", i, gLights[i].distance);
		}

		aguaShader->setVec3("eye", activeCam->Position);

		aguaShader->setVec4("MaterialAmbientColor", MaterialAgua.ambient);
		aguaShader->setVec4("MaterialDiffuseColor", MaterialAgua.diffuse);
		aguaShader->setVec4("MaterialSpecularColor", MaterialAgua.specular);
		aguaShader->setFloat("transparency", MaterialAgua.transparency);

		agua->Draw(*aguaShader);
		glDisable(GL_BLEND);
	}



	glUseProgram(0);


	// Vitrina de peces
	g_pezCristalAngulo += deltaTime * 40.0f;
	if (g_pezCristalAngulo > 360.0f) g_pezCristalAngulo -= 360.0f;
	g_pecesVitrina[g_pezVitrinaIdx]->UpdateAnimation(deltaTime);

	glm::vec3 centroCubo = glm::vec3(10.6898f, 5.5f, 1.6184f);
	float px = centroCubo.x + 2.5f * cos(glm::radians(g_pezCristalAngulo));
	float pz = centroCubo.z + 2.5f * sin(glm::radians(g_pezCristalAngulo));
	glm::mat4 modelPez = glm::mat4(1.0f);
	modelPez = glm::translate(modelPez, glm::vec3(px, centroCubo.y, pz));
	modelPez = glm::rotate(modelPez, glm::radians(g_pezCristalAngulo + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	float escalasVitrina[] = { 0.025f, 0.025f, 0.015f, 0.015f, 0.01f, 0.015f, 0.022f };
	modelPez = glm::scale(modelPez, glm::vec3(escalasVitrina[g_pezVitrinaIdx]));	dynamicShader->use();
	dynamicShader->setMat4("projection", projection);
	dynamicShader->setMat4("view", view);
	dynamicShader->setMat4("model", modelPez);
	dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, g_pecesVitrina[g_pezVitrinaIdx]->gBones);
	g_pecesVitrina[g_pezVitrinaIdx]->Draw(*dynamicShader);
	glUseProgram(0);

	// Cubo cristal
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	debugShader->use();
	glm::mat4 modelCubo = glm::mat4(1.0f);
	modelCubo = glm::translate(modelCubo, centroCubo);
	debugShader->setMat4("model", modelCubo);
	debugShader->setMat4("view", view);
	debugShader->setMat4("projection", projection);
	debugShader->setVec3("color", glm::vec3(0.6f, 0.9f, 1.0f));
	debugShader->setFloat("alpha", 0.2f);
	debugShader->setBool("useAlpha", true);
	glBindVertexArray(g_cuboVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);
	glUseProgram(0);

	// =====================================================
	// PERSONAJE JUGADOR: Nino
	// =====================================================
	{
		playerCharacter->UpdateAnimation(deltaTime);

		// En primera persona no se dibuja para evitar ver el interior del modelo.
		if (activeCamera != 1)
		{
			dynamicShader->use();
			dynamicShader->setMat4("projection", projection);
			dynamicShader->setMat4("view", view);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, playerPosition);
			model = glm::rotate(model, glm::radians(playerRotation + playerModelYawOffset), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(playerScale, playerScale, playerScale));

			dynamicShader->setMat4("model", model);
			dynamicShader->setMat4("gBones", MAX_RIGGING_BONES, playerCharacter->gBones);
			playerCharacter->Draw(*dynamicShader);
		}
	}

	glUseProgram(0);

	// =====================================================
// INTERFAZ 2D (carteles y panel) - FASE 1: FONDOS
// =====================================================
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glm::mat4 proj2D = glm::ortho(0.0f, (float)SCR_WIDTH, 0.0f, (float)SCR_HEIGHT);

	// --- Fondo del cartel de animales (solo si está abierto) ---
	float cartelX = 80.0f, cartelY = 320.0f, cartelW = 560.0f, cartelH = 200.0f;
	if (g_animalCercano != nullptr && g_mostrandoInfo) {
		DrawQuad(cartelX - 6.0f, cartelY + 6.0f, cartelW + 12.0f, cartelH + 12.0f,
			glm::vec4(0.25f, 0.13f, 0.05f, 0.95f), proj2D);
		DrawQuad(cartelX, cartelY, cartelW, cartelH,
			glm::vec4(0.45f, 0.28f, 0.13f, 0.95f), proj2D);
	}

	// --- Fondo del panel de salas (siempre visible) ---
	float panelW = 430.0f, panelH = 160.0f;
	float panelX = SCR_WIDTH - panelW - 15.0f;
	float panelY = SCR_HEIGHT - 15.0f;
	DrawQuad(panelX, panelY, panelW, panelH,
		glm::vec4(0.30f, 0.18f, 0.08f, 0.80f), proj2D);
	DrawQuad(panelX - 4.0f, panelY + 4.0f, panelW + 8.0f, 4.0f,
		glm::vec4(0.6f, 0.45f, 0.2f, 0.9f), proj2D);


	// --- Fondo del mensaje final (temporal) ---
	bool mostrarMensaje = false;
	if (g_museoCompletado) {
		float transcurrido = (float)glfwGetTime() - g_tiempoCompletado;
		if (transcurrido < DURACION_MENSAJE) {
			mostrarMensaje = true;
		}
	}

	float msgW = 620.0f, msgH = 90.0f;
	float msgX = (SCR_WIDTH - msgW) / 2.0f;
	float msgY = SCR_HEIGHT / 2.0f + msgH / 2.0f;

	if (mostrarMensaje) {
		DrawQuad(msgX - 6.0f, msgY + 6.0f, msgW + 12.0f, msgH + 12.0f,
			glm::vec4(0.15f, 0.10f, 0.03f, 0.95f), proj2D);
		DrawQuad(msgX, msgY, msgW, msgH,
			glm::vec4(0.35f, 0.22f, 0.10f, 0.95f), proj2D);
	}

	// --- Fondo del mensaje de bienvenida ---
	bool mostrarBienvenida = false;
	if (g_tiempoBienvenida >= 0.0f) {
		float transcurrido = (float)glfwGetTime() - g_tiempoBienvenida;
		if (transcurrido < DURACION_BIENVENIDA)
			mostrarBienvenida = true;
	}

	float bienW = 750.0f, bienH = 90.0f;
	float bienX = (SCR_WIDTH - bienW) / 2.0f;
	float bienY = SCR_HEIGHT / 2.0f + bienH / 2.0f;

	if (mostrarBienvenida) {
		DrawQuad(bienX - 6.0f, bienY + 6.0f, bienW + 12.0f, bienH + 12.0f,
			glm::vec4(0.15f, 0.10f, 0.03f, 0.95f), proj2D);
		DrawQuad(bienX, bienY, bienW, bienH,
			glm::vec4(0.35f, 0.22f, 0.10f, 0.95f), proj2D);
	}

	// Hint TAB (siempre visible arriba)
	if (!g_mostrandoControles) {
		textRenderer.RenderText("Presiona TAB para los controles",
			20.0f, SCR_HEIGHT - 20.0f, 0.4f, glm::vec3(1.0f, 1.0f, 0.85f));
	}

	// Pista astronauta
	{
		float distAstro = glm::distance(
			getInteractionPosition(),
			glm::vec3(119.351f, 1.3f, -27.8479f));
		if (distAstro < 8.0f) {
			textRenderer.RenderText("Presiona R para interactuar con el astronauta",
				20.0f, SCR_HEIGHT - 160.0f, 0.45f, glm::vec3(1.0f, 1.0f, 0.85f));
		}
	}

	{
		float distVitrina = glm::distance(playerPosition, glm::vec3(26.9132f, 1.3f, 2.91424f));
		if (distVitrina < 10.0f) {
			textRenderer.RenderText("Presiona E para cambiar el pez",
				20.0f, SCR_HEIGHT - 185.0f, 0.45f, glm::vec3(1.0f, 1.0f, 0.85f));
		}
	}

	// Overlay de controles
	if (g_mostrandoControles) {
		// Tamaño y posición del overlay (no ocupa toda la pantalla)
		float imgW = 500.0f, imgH = 600.0f;
		float imgX = (SCR_WIDTH - imgW) / 2.0f;
		float imgY = SCR_HEIGHT - 50.0f;

		// Fondo oscuro detrás
		DrawQuad(imgX - 10.0f, imgY + 10.0f, imgW + 20.0f, imgH + 20.0f,
			glm::vec4(0.0f, 0.0f, 0.0f, 0.75f), proj2D);

		// Dibujar la imagen con un shader simple
		// (usa el mismo debugShader pero con textura)
		float verts[] = {
			imgX,        imgY,         0.0f, 0.0f, 1.0f,
			imgX + imgW,   imgY,         0.0f, 1.0f, 1.0f,
			imgX + imgW,   imgY - imgH,    0.0f, 1.0f, 0.0f,
			imgX,        imgY - imgH,    0.0f, 0.0f, 0.0f
		};
		unsigned int idx[] = { 0,1,2, 2,3,0 };

		unsigned int vao, vbo, ebo;
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(idx), idx, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		imageShader->use();
		imageShader->setMat4("projection", proj2D);
		imageShader->setInt("image", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_texControles);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ebo);

		// Texto de salida
		textRenderer.RenderText("Presiona TAB para volver",
			20.0f, SCR_HEIGHT - 20.0f, 0.4f, glm::vec3(1.0f, 1.0f, 0.85f));
	}


	// =====================================================
	// FASE 2: TEXTO (siempre después de los fondos)
	// =====================================================

	// Pista "Presiona F"
	// Pista "Presiona F" (dos renglones, color claro)
	if (g_animalCercano != nullptr && !g_mostrandoInfo) {
		glm::vec3 colorPista = glm::vec3(1.0f, 1.0f, 0.85f);  // crema claro, visible en cualquier sala

		textRenderer.RenderText("Presiona F",
			430.0f, 110.0f, 0.65f, colorPista);
		textRenderer.RenderText("para ver informacion",
			375.0f, 80.0f, 0.5f, colorPista);
	}
	// Pista "Presiona V para ver el cartel" (esquina superior izquierda)
	if (g_cartelCercano != nullptr && !g_viendoCartel) {
		glm::vec3 colorPista = glm::vec3(1.0f, 1.0f, 0.85f);
		textRenderer.RenderText("Presiona V para ver",
			20.0f, SCR_HEIGHT - 75.0f, 0.55f, colorPista);
		textRenderer.RenderText("la informacion del cartel",
			20.0f, SCR_HEIGHT - 100.0f, 0.45f, colorPista);
	}

	// Pista "Presiona V para salir" cuando ya estás en zoom
	if (g_viendoCartel) {
		glm::vec3 colorPista = glm::vec3(1.0f, 1.0f, 0.85f);
		textRenderer.RenderText("Presiona V para volver",
			20.0f, SCR_HEIGHT - 40.0f, 0.55f, colorPista);
	}

	// Texto del cartel de animales
	if (g_animalCercano != nullptr && g_mostrandoInfo) {
		// Escala del nombre: se reduce si es muy largo
		float nombreScale = 0.9f;
		if (g_animalCercano->nombre.length() > 25) nombreScale = 0.55f;
		else if (g_animalCercano->nombre.length() > 18) nombreScale = 0.7f;

		textRenderer.RenderText(g_animalCercano->nombre,
			cartelX + 25.0f, cartelY - 45.0f, nombreScale, glm::vec3(1.0f, 0.95f, 0.7f));
		textRenderer.RenderText(g_animalCercano->descripcion,
			cartelX + 25.0f, cartelY - 90.0f, 0.45f, glm::vec3(1.0f, 1.0f, 1.0f));
	}

	// Texto del panel de salas
	textRenderer.RenderText("SALAS",
		panelX + 15.0f, panelY - 28.0f, 0.55f, glm::vec3(1.0f, 0.95f, 0.7f));

	float textoY = panelY - 60.0f;
	for (auto& sala : g_salas) {
		glm::vec3 color;
		if (sala.dentro)
			color = glm::vec3(1.0f, 0.9f, 0.1f);
		else if (sala.visitada)
			color = glm::vec3(0.2f, 1.0f, 0.2f);
		else
			color = glm::vec3(0.85f, 0.85f, 0.85f);

		float salaScale = (sala.nombre.length() > 30) ? 0.35f : 0.42f;
		textRenderer.RenderText(sala.nombre,
			panelX + 15.0f, textoY, salaScale, color);
		textoY -= 32.0f;
	}


	// --- Texto del mensaje final (temporal) ---
	if (mostrarMensaje) {
		textRenderer.RenderText("Felicidades!",
			msgX + 200.0f, msgY - 35.0f, 0.8f, glm::vec3(1.0f, 0.9f, 0.3f));
		textRenderer.RenderText("Has terminado de recorrer el museo",
			msgX + 40.0f, msgY - 70.0f, 0.5f, glm::vec3(1.0f, 1.0f, 0.85f));
	}

	// --- Texto del mensaje de bienvenida ---
	if (mostrarBienvenida) {
		textRenderer.RenderText("Bienvenido al Museo Virtual",
			bienX + 180.0f, bienY - 25.0f, 0.65f, glm::vec3(1.0f, 0.9f, 0.3f));
		textRenderer.RenderText("Haz el recorrido e interactua con cada una de las salas",
			bienX + 30.0f, bienY - 62.0f, 0.42f, glm::vec3(1.0f, 1.0f, 0.85f));
	}

	if (g_cercaBotonPlanetas) {
		glm::vec3 colorPista = glm::vec3(1.0f, 1.0f, 0.85f);
		textRenderer.RenderText("Presiona P",
			20.0f, SCR_HEIGHT - 110.0f, 0.55f, colorPista);
		textRenderer.RenderText("y mira arriba",
			20.0f, SCR_HEIGHT - 135.0f, 0.45f, colorPista);
	}



	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	// =====================================================
	// FIN INTERFAZ 2D

	glfwSwapBuffers(window);
	glfwPollEvents();

	return true;
}

Camera* getActiveCamera()
{
	if (activeCamera == 1)
		return &camera1st;

	if (activeCamera == 3)
		return &camera3rd;

	return &camera;
}

void updatePlayerForward()
{
	// Dirección de la mirada con giro horizontal (yaw) y vertical (pitch)
	float yawRad = glm::radians(playerRotation);
	float pitchRad = glm::radians(playerPitch);

	glm::vec3 dir;
	dir.x = sin(yawRad) * cos(pitchRad);
	dir.y = sin(pitchRad);
	dir.z = cos(yawRad) * cos(pitchRad);

	playerForward = glm::normalize(dir);
}

void updateCharacterCameras()
{
	glm::vec3 up(0.0f, 1.0f, 0.0f);

	// Primera persona: a la altura de los ojos y un poco al frente.
	camera1st.Position = playerPosition;
	camera1st.Position.y += eyeHeight;
	camera1st.Position += 0.35f * playerForward;
	camera1st.Front = glm::normalize(playerForward);

	// Tercera persona: detrás y arriba del personaje.
	camera3rd.Position = playerPosition;
	camera3rd.Position -= thirdPersonDistance * playerForward;
	camera3rd.Position += thirdPersonHeight * up;

	glm::vec3 target = playerPosition + glm::vec3(0.0f, thirdPersonTargetHeight, 0.0f);
	camera3rd.Front = glm::normalize(target - camera3rd.Position);
}

glm::vec3 getInteractionPosition()
{
	// En cámara libre, las interacciones dependen de la cámara.
	// En primera/tercera, dependen del personaje.
	if (activeCamera == 0)
		return camera.Position;

	return playerPosition;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	// =====================
	// CAMBIO DE CÁMARA
	// =====================
	static bool f1Pressed = false, f2Pressed = false, f3Pressed = false;

	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS) {
		if (!f1Pressed) {
			// Al cambiar a libre: colocar cámara donde están los ojos del jugador
			camera.Position = playerPosition + glm::vec3(0.0f, eyeHeight, 0.0f);
			camera.Front = glm::normalize(playerForward);
			camera.Yaw = -90.0f + glm::degrees(atan2(playerForward.x, playerForward.z));
			camera.Pitch = glm::degrees(asin(playerForward.y));
			activeCamera = 0;
			f1Pressed = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE) f1Pressed = false;

	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		if (!f2Pressed) { activeCamera = 1; f2Pressed = true; }
	}
	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE) f2Pressed = false;

	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		if (!f3Pressed) { activeCamera = 3; f3Pressed = true; }
	}
	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE) f3Pressed = false;

	// =====================
	// MOVIMIENTO
	// =====================
	if (activeCamera == 0)
	{
		// Cámara libre
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			camera.ProcessKeyboard(FORWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			camera.ProcessKeyboard(BACKWARD, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			camera.ProcessKeyboard(LEFT, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			camera.ProcessKeyboard(RIGHT, deltaTime);

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
			camera.ProcessKeyboard(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
			camera.ProcessKeyboard(DOWN, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			camera.ProcessKeyboard(UP, deltaTime);
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			camera.ProcessKeyboard(DOWN, deltaTime);
	}
	else
	{
		float moveStep = playerMoveSpeed * deltaTime;

		// Avance SOLO horizontal (ignora el componente vertical de la mirada)
		glm::vec3 flatForward = glm::normalize(
			glm::vec3(playerForward.x, 0.0f, playerForward.z));
		glm::vec3 playerRight = glm::normalize(
			glm::cross(flatForward, glm::vec3(0.0f, 1.0f, 0.0f)));

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		{
			playerPosition += moveStep * flatForward;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		{
			playerPosition -= moveStep * flatForward;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		{
			playerPosition -= moveStep * playerRight;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ||
			glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		{
			playerPosition += moveStep * playerRight;
		}
	}

	// Modos de dibujo
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

	// Puertas / objetos existentes
	if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
		if (door_offset < 2.5f) door_offset += 0.01f;
	}
	if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
		if (door_offset > 0.05f) door_offset -= 0.01f;
	}

	float speed = 80.0f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
		door_rotation += speed;
		if (door_rotation > 80.0f) door_rotation = 80.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
		door_rotation -= speed;
		if (door_rotation < 0.5f) door_rotation = 0.05f;
	}

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		if (asteroid_offset > -2.0f) asteroid_offset -= 0.02f;
	}
	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		if (asteroid_offset < 3.0f) asteroid_offset += 0.02f;
	}

	// P = activar/desactivar el movimiento de los planetas
	static bool teclaPPresionada = false;
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
		if (!teclaPPresionada) {
			planetasEnMovimiento = !planetasEnMovimiento;
			teclaPPresionada = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
		teclaPPresionada = false;
	}

	// C = imprime posición de cámara activa y personaje
	static bool teclaCPresionada = false;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		if (!teclaCPresionada) {
			Camera* activeCam = getActiveCamera();
			std::cout << "Posicion camara activa: ("
				<< activeCam->Position.x << "f, "
				<< activeCam->Position.y << "f, "
				<< activeCam->Position.z << "f)" << std::endl;

			std::cout << "Posicion personaje: ("
				<< playerPosition.x << "f, "
				<< playerPosition.y << "f, "
				<< playerPosition.z << "f)" << std::endl;
			teclaCPresionada = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
		teclaCPresionada = false;
	}

	// Escala de peces
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	{
		for (auto& f : fishes)
		{
			f.scale -= 0.001f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	{
		for (auto& f : fishes)
		{
			f.scale += 0.001f;
		}
	}

	// R = rota los personajes existentes Secado/Lavado/AstroA
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		rotateCharacter += 1.5f;
	}

	// F = abrir/cerrar cartel informativo
	static bool teclaFPresionada = false;
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (!teclaFPresionada) {
			if (g_animalCercano != nullptr) {
				g_mostrandoInfo = !g_mostrandoInfo;
			}
			teclaFPresionada = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		teclaFPresionada = false;
	}

	// O = volver al punto de inicio
	static bool teclaOPresionada = false;
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
		if (!teclaOPresionada) {
			// Reinicia personaje al inicio
			playerPosition = glm::vec3(47.6013f, 1.3f, 75.3573f);
			playerRotation = 180.0f;
			updatePlayerForward();
			updateCharacterCameras();

			// Reinicia también la cámara libre
			camera.Position = glm::vec3(47.6358f, 4.84023f, 75.0113f);

			teclaOPresionada = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_RELEASE) {
		teclaOPresionada = false;
	}
	// V = activar/desactivar zoom al cartel cercano
	static bool teclaVPresionada = false;
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
		if (!teclaVPresionada) {
			if (g_cartelCercano != nullptr) {
				g_viendoCartel = !g_viendoCartel;
			}
			teclaVPresionada = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_V) == GLFW_RELEASE) {
		teclaVPresionada = false;
	}

	static bool teclaTABPresionada = false;
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		if (!teclaTABPresionada) {
			g_mostrandoControles = !g_mostrandoControles;
			teclaTABPresionada = true;
		}
	}
	if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE) {
		teclaTABPresionada = false;
	}

	// Tecla E — cambiar pez en vitrina
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		if (!g_teclaEPresionada) {
			g_pezVitrinaIdx = (g_pezVitrinaIdx + 1) % (int)g_pecesVitrina.size();
			g_teclaEPresionada = true;
		}
	}
	else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
		g_teclaEPresionada = false;
	}

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;

	lastX = (float)xpos;
	lastY = (float)ypos;

	if (activeCamera == 0)
	{
		// Cámara libre: igual que antes
		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else
	{
		// 1ra/3ra persona: el mouse mira alrededor (horizontal Y vertical)
		float sensibilidad = 0.15f;
		playerRotation -= xoffset * sensibilidad;
		playerPitch += yoffset * sensibilidad;

		// Límite para no voltear la cámara de cabeza
		if (playerPitch > 89.0f)  playerPitch = 89.0f;
		if (playerPitch < -89.0f) playerPitch = -89.0f;

		updatePlayerForward();
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (activeCamera == 0)
	{
		camera.ProcessMouseScroll((float)yoffset);
	}
	else if (activeCamera == 1)
	{
		camera1st.ProcessMouseScroll((float)yoffset);
	}
	else if (activeCamera == 3)
	{
		camera3rd.ProcessMouseScroll((float)yoffset);
	}
}