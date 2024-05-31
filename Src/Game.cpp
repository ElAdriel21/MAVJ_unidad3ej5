#include "Game.h"
#include "Box2DHelper.h"

// Constructor de la clase Game
Game::Game(int ancho, int alto, std::string titulo)
{
    // Inicialización de la ventana de renderizado
    wnd = new RenderWindow(VideoMode(ancho, alto), titulo);
    wnd->setVisible(true);
    fps = 60;
    wnd->setFramerateLimit(fps);
    frameTime = 1.0f / fps;
    SetZoom(); // Configuración del zoom de la cámara
    InitPhysics(); // Inicialización del mundo físico
}

// Método principal que maneja el bucle del juego
void Game::Loop()
{
    while (wnd->isOpen())
    {
        wnd->clear(clearColor); // Limpia la ventana con un color especificado
        DoEvents(); // Procesa los eventos del sistema
        UpdatePhysics(); // Actualiza la simulación física
        DrawGame(); // Dibuja el juego en la ventana
        wnd->display(); // Muestra la ventana renderizada
    }
}

// Actualiza la simulación física
void Game::UpdatePhysics()
{
    phyWorld->Step(frameTime, 8, 8); // Avanza la simulación física
    phyWorld->ClearForces(); // Limpia las fuerzas aplicadas a los cuerpos
    phyWorld->DebugDraw(); // Dibuja el mundo físico (para depuración)
}

// Dibuja los elementos del juego en la ventana
void Game::DrawGame()
{
    // Aquí puedes agregar el código para dibujar tus objetos del juego
}

// Procesa los eventos del sistema
void Game::DoEvents()
{
    Event evt;
    while (wnd->pollEvent(evt))
    {
        switch (evt.type)
        {
        case Event::Closed:
            wnd->close(); // Cierra la ventana
            break;
        }
    }

    // Mueve el cuerpo controlado por el teclado
    b2Vec2 velocity(0.0f, 0.0f);
    if (Keyboard::isKeyPressed(Keyboard::Left))
        velocity.x = -50.0f;
    if (Keyboard::isKeyPressed(Keyboard::Right))
        velocity.x = 50.0f;
    if (Keyboard::isKeyPressed(Keyboard::Down))
        velocity.y = 50.0f;
    if (Keyboard::isKeyPressed(Keyboard::Up))
        velocity.y = -50.0f;

    controlBody->SetLinearVelocity(velocity);
    controlBody->SetAwake(true); // Activa el cuerpo para que responda a fuerzas y colisiones
}

// Configura el área visible en la ventana de renderizado
void Game::SetZoom()
{
    View camara;
    camara.setSize(100.0f, 100.0f); // Tamaño del área visible
    camara.setCenter(50.0f, 50.0f); // Centra la vista en estas coordenadas
    wnd->setView(camara); // Asigna la vista a la ventana
}

// Inicializa el mundo físico y los elementos estáticos del juego
void Game::InitPhysics()
{
    // Inicializa el mundo físico con la gravedad por defecto
    phyWorld = new b2World(b2Vec2(0.0f, 9.8f));

    // Inicializa el renderizador de depuración para el mundo físico
    debugRender = new SFMLRenderer(wnd);
    debugRender->SetFlags(UINT_MAX); // Configura el renderizado para que muestre todo
    phyWorld->SetDebugDraw(debugRender);

    // Crea los elementos estáticos del juego (suelo y paredes)
    b2Body* groundBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    groundBody->SetTransform(b2Vec2(50.0f, 100.0f), 0.0f);

    b2Body* leftWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    leftWallBody->SetTransform(b2Vec2(0.0f, 50.0f), 0.0f);

    b2Body* rightWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 10, 100);
    rightWallBody->SetTransform(b2Vec2(100.0f, 50.0f), 0.0f);

    b2Body* topWallBody = Box2DHelper::CreateRectangularStaticBody(phyWorld, 100, 10);
    topWallBody->SetTransform(b2Vec2(50.0f, 0.0f), 0.0f);

    // Torso
    controlBody = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 10.0f, 15.0f, 1.0f, 0.5f, 0.1f);
    controlBody->SetTransform(b2Vec2(50.0f, 50.0f), 0.0f);

    // Cabeza
    b2Body* head = Box2DHelper::CreateCircularDynamicBody(phyWorld, 5.0f, 1.0f, 0.5f, 0.1f);
    head->SetTransform(b2Vec2(50.0f, 50.0f + 12.5f), 0.0f);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, controlBody->GetWorldCenter() + b2Vec2(0.0f, 7.5f), head,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    // Brazos
    b2Body* upperLeftArm = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 12.0f, 4.0f, 1.0f, 0.5f, 0.1f);
    upperLeftArm->SetTransform(b2Vec2(50.0f - 11.0f, 50.0f), 0.0f);

    b2Body* upperRightArm = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 12.0f, 4.0f, 1.0f, 0.5f, 0.1f);
    upperRightArm->SetTransform(b2Vec2(50.0f + 11.0f, 50.0f), 0.0f);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(50.0f - 5.0f, 50.0f + 7.5f), upperLeftArm,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(50.0f + 5.0f, 50.0f + 7.5f), upperRightArm,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    // Piernas
    b2Body* upperLeftLeg = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 6.0f, 12.0f, 1.0f, 0.5f, 0.1f);
    upperLeftLeg->SetTransform(b2Vec2(50.0f - 3.0f, 50.0f - 15.0f), 0.0f);

    b2Body* upperRightLeg = Box2DHelper::CreateRectangularDynamicBody(phyWorld, 6.0f, 12.0f, 1.0f, 0.5f, 0.1f);
    upperRightLeg->SetTransform(b2Vec2(50.0f + 3.0f, 50.0f - 15.0f), 0.0f);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(50.0f - 2.0f, 50.0f - 7.5f), upperLeftLeg,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);

    Box2DHelper::CreateRevoluteJoint(phyWorld, controlBody, b2Vec2(50.0f + 2.0f, 50.0f - 7.5f), upperRightLeg,
        -0.5f, 0.5f, 0.0f, 10.0f, false, true);
}


