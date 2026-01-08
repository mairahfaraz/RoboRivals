#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

enum class GameState { SELECTION, BATTLE, GAMEOVER };

struct Robot {
    sf::Sprite sprite;
    int health;
};

struct Bullet {
    sf::RectangleShape shape;
    float speed = 0.f;   
    int damage = 0;     
};

struct Pickup {
    sf::CircleShape shape;
    bool collected = false; 
};

bool firstSpawnDone = false;


int main() {
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(1280, 750), "RoboRivals");
    window.setFramerateLimit(60); //60 fps

    GameState state = GameState::SELECTION; //1st stage

    sf::Font font;
    if (!font.loadFromFile("ScienceGothic-VariableFont_CTRS,slnt,wdth,wght.ttf")) return -1;

    sf::Texture robot1Tex, robot2Tex;
    if (!robot1Tex.loadFromFile("Robot1.png") || !robot2Tex.loadFromFile("Robot2.png")) return -1;

    Robot robot1{ sf::Sprite(robot1Tex), 200 }; // 200 health
    Robot robot2{ sf::Sprite(robot2Tex), 200 };
    robot1.sprite.setScale(1.5f, 1.5f); // size of sprite
    robot2.sprite.setScale(1.5f, 1.5f);

    int r1bullets = 10;       // robot starts with 10 bullets
    int r2bullets = 10;       

    int orbSpawnTimer = 0;
    int magSpawnTimer = 0;


    sf::Music selectionMusic;
    if (!selectionMusic.openFromFile("Audio/selection.ogg.ogg")) return -1;

    sf::Music battleMusic;
    if (!battleMusic.openFromFile("Audio/battle.ogg.ogg")) return -1;

    sf::SoundBuffer pewBuffer;
    if (!pewBuffer.loadFromFile("Audio/pew.wav.wav")) return -1;
    sf::Sound pewSound;
    pewSound.setBuffer(pewBuffer);


    sf::Text title("RoboRivals", font, 60);
    title.setFillColor(sf::Color::White);
    title.setPosition(1280 / 2 - title.getLocalBounds().width / 2, 20);

    sf::RectangleShape r1Box(sf::Vector2f(250, 150));
    r1Box.setPosition(50, 200);
    r1Box.setFillColor(sf::Color(30, 30, 30, 200));
    std::string robot1Info = "Robot 1\nHealth: 200\nSpeed: 5\nWeapon: Choose";
    sf::Text r1Stats(robot1Info, font, 24);
    r1Stats.setFillColor(sf::Color::White);
    r1Stats.setPosition(60, 200);

    sf::RectangleShape r2Box(sf::Vector2f(250, 150));
    r2Box.setPosition(990, 200);
    r2Box.setFillColor(sf::Color(30, 30, 30, 200));
    std::string robot2Info = "Robot 2\nHealth: 200\nSpeed: 5\nWeapon: Choose";
    sf::Text r2Stats(robot2Info, font, 24);
    r2Stats.setFillColor(sf::Color::White);
    r2Stats.setPosition(1000, 200);

    sf::RectangleShape arena(sf::Vector2f(600, 200));
    arena.setPosition(340, 150); 
    arena.setFillColor(sf::Color(30, 30, 30, 200));

    sf::Text optionPrompt("", font, 30);
    optionPrompt.setFillColor(sf::Color::White);
    optionPrompt.setPosition(1280 / 2 - 240, 440);
    std::vector<std::string> options = { "Laser", "Shotgun", "Missile" };//imp
    int selectedOption = 0;
    int robot1Choice = -1; //no choice initialised
    int robot2Choice = -1;
    int selectionStage = 1;

    optionPrompt.setString("Choose your weapon (Robot 1)");

    sf::Text optionTexts[3];
    for (int i = 0; i < 3; i++) {
        optionTexts[i].setFont(font);
        optionTexts[i].setString(options[i]);
        optionTexts[i].setCharacterSize(28);
        optionTexts[i].setPosition(1280 / 2 - 60, 480 + i * 40);
    }

    sf::Vector2f r1Pos, r2Pos;
    sf::Vector2f r1Vel(0.f, 0.f), r2Vel(0.f, 0.f);
    const float moveSpeed = 5.f, jumpSpeed = -12.f, gravity = 0.5f;
    const float floorY = 600.f;

    const int maxBullets = 10;         
    const int bulletsPerMagazine = 5;  // Bullets added when collecting a magazine
    std::vector<Pickup> magazines;     

    int r1Bullets = maxBullets;
    int r2Bullets = maxBullets;

    const int maxMagazines = 3;        // Max magazines on screen


    std::vector<Bullet> robot1Bullets;
    std::vector<Bullet> robot2Bullets;

    bool r1Shot = false, r2Shot = false;
    static int r1Cooldown = 0, r2Cooldown = 0;
    const int cooldownMax = 20;

    std::vector<Pickup> pickups;
    const int maxPickups = 5;
    const float orbHealAmount = 20.f;
    const int maxHealth = 200;

    std::vector<sf::CircleShape> stars;
    for (int i = 0; i < 100; i++) {
        sf::CircleShape star(1.f);
        star.setFillColor(sf::Color::Yellow);
        star.setPosition(rand() % 1280, rand() % 300);
        stars.push_back(star); // imp
    }

    std::vector<sf::RectangleShape> buildings;
    for (int i = 0; i < 10; i++) {
        float width = 60 + rand() % 40; // 60 - 99
        float height = 150 + rand() % 150; // 150 - 299
        sf::RectangleShape building(sf::Vector2f(width, height));
        building.setFillColor(sf::Color(30, 30, 40));
        building.setPosition(i * 120, floorY - height);
        buildings.push_back(building);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            // --- Restart game ---
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                state = GameState::SELECTION; 

                selectionStage = 1;
                selectedOption = 0;
                optionPrompt.setString("Choose your weapon (Robot 1)");

                robot1Choice = -1;
                robot2Choice = -1;

                robot1.health = maxHealth;
                robot2.health = maxHealth;

                r1Bullets = maxBullets;
                r2Bullets = maxBullets;

                pickups.clear();
                magazines.clear();
                firstSpawnDone = false;
                robot1Bullets.clear();
                robot2Bullets.clear();

                orbSpawnTimer = 0;
                magSpawnTimer = 0;

                r1Pos = sf::Vector2f(100.f, floorY - robot1.sprite.getGlobalBounds().height);
                r2Pos = sf::Vector2f(1080.f, floorY - robot2.sprite.getGlobalBounds().height);

                if (battleMusic.getStatus() == sf::Music::Playing)
                    battleMusic.stop(); 
            }


            if (state == GameState::SELECTION && event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::U) selectedOption = (selectedOption - 1 + 3) % 3;
                if (event.key.code == sf::Keyboard::D) selectedOption = (selectedOption + 1) % 3;

                if (event.key.code == sf::Keyboard::Enter) {
                    if (selectionStage == 1) {
                        robot1Choice = selectedOption;
                        selectionStage = 2;
                        optionPrompt.setString("Choose your weapon (Robot 2)");
                    }
                    else if (selectionStage == 2) {
                        robot2Choice = selectedOption;
                        state = GameState::BATTLE;
                        r1Pos = sf::Vector2f(100.f, floorY - robot1.sprite.getGlobalBounds().height);
                        r2Pos = sf::Vector2f(1080.f, floorY - robot2.sprite.getGlobalBounds().height);
                    }
                }

                // Update option colors to highlight the selected one
                for (int i = 0; i < 3; i++)
                    optionTexts[i].setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);
            }

        }

        window.clear(state == GameState::SELECTION ? sf::Color(0, 0, 50) : sf::Color(10, 10, 50));


        if (state == GameState::SELECTION) {
            if (battleMusic.getStatus() == sf::Music::Playing) battleMusic.stop();
            if (selectionMusic.getStatus() != sf::Music::Playing) selectionMusic.play();
        }
        else if (state == GameState::BATTLE) {
            if (selectionMusic.getStatus() == sf::Music::Playing) selectionMusic.stop();
            if (battleMusic.getStatus() != sf::Music::Playing) battleMusic.play();
        }

        for (int i = 0; i < 3; i++)
            optionTexts[i].setFillColor(i == selectedOption ? sf::Color::Yellow : sf::Color::White);


        if (state == GameState::SELECTION) {
            window.draw(title);
            window.draw(r1Box); window.draw(r2Box);
            window.draw(r1Stats); window.draw(r2Stats);
            window.draw(arena);
            window.draw(optionPrompt);
            for (int i = 0; i < 3; i++) window.draw(optionTexts[i]); // selection of weapon
            robot1.sprite.setPosition(arena.getPosition().x + 20,
                arena.getPosition().y + (arena.getSize().y - robot1.sprite.getGlobalBounds().height) / 2); 
            robot2.sprite.setPosition(arena.getPosition().x + arena.getSize().x - robot2.sprite.getGlobalBounds().width - 20,
                arena.getPosition().y + (arena.getSize().y - robot2.sprite.getGlobalBounds().height) / 2);//
            window.draw(robot1.sprite);
            window.draw(robot2.sprite);
        }
        else if (state == GameState::BATTLE) {
            sf::RectangleShape bg(sf::Vector2f(1280, 720));
            bg.setFillColor(sf::Color(10, 10, 50)); // dark blue
            window.draw(bg);

            for (auto& star : stars) window.draw(star); //circleshape vector
            for (auto& b : buildings) window.draw(b);

            sf::RectangleShape floor(sf::Vector2f(1280, 50));
            floor.setPosition(0, floorY);
            floor.setFillColor(sf::Color(100, 100, 100));
            window.draw(floor);

            // Robot Movement
            // Robot 1
            r1Vel.x = 0.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && r1Pos.x > 0) {
                sf::FloatRect nextPos(r1Pos.x - moveSpeed, r1Pos.y, robot1.sprite.getGlobalBounds().width, robot1.sprite.getGlobalBounds().height);
                // Only block if robots are roughly on the same vertical level
                if (!(nextPos.intersects(robot2.sprite.getGlobalBounds()) && abs(r1Pos.y - r2Pos.y) < 20)) {
                    r1Vel.x = -moveSpeed;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && r1Pos.x + robot1.sprite.getGlobalBounds().width < 1280) {
                sf::FloatRect nextPos(r1Pos.x + moveSpeed, r1Pos.y, robot1.sprite.getGlobalBounds().width, robot1.sprite.getGlobalBounds().height);
                if (!(nextPos.intersects(robot2.sprite.getGlobalBounds()) && abs(r1Pos.y - r2Pos.y) < 20)) {
                    r1Vel.x = moveSpeed;
                }//x
            }

            // Robot 2
            r2Vel.x = 0.f;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && r2Pos.x > 0) {
                sf::FloatRect nextPos(r2Pos.x - moveSpeed, r2Pos.y, robot2.sprite.getGlobalBounds().width, robot2.sprite.getGlobalBounds().height);
                if (!(nextPos.intersects(robot1.sprite.getGlobalBounds()) && abs(r2Pos.y - r1Pos.y) < 20)) {
                    r2Vel.x = -moveSpeed;
                }
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && r2Pos.x + robot2.sprite.getGlobalBounds().width < 1280) {
                sf::FloatRect nextPos(r2Pos.x + moveSpeed, r2Pos.y, robot2.sprite.getGlobalBounds().width, robot2.sprite.getGlobalBounds().height);
                if (!(nextPos.intersects(robot1.sprite.getGlobalBounds()) && abs(r2Pos.y - r1Pos.y) < 20)) {
                    r2Vel.x = moveSpeed;
                }
            }

            // --- Vertical movement (jumping)
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && r1Pos.y >= floorY - robot1.sprite.getGlobalBounds().height) r1Vel.y = jumpSpeed;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && r2Pos.y >= floorY - robot2.sprite.getGlobalBounds().height) r2Vel.y = jumpSpeed;

            // --- Gravity
            r1Vel.y += gravity;
            r2Vel.y += gravity;

            // --- Update positions
            r1Pos += r1Vel;
            r2Pos += r2Vel;

            // --- Prevent falling below floor
            if (r1Pos.y > floorY - robot1.sprite.getGlobalBounds().height) { r1Pos.y = floorY - robot1.sprite.getGlobalBounds().height; r1Vel.y = 0; }
            if (r2Pos.y > floorY - robot2.sprite.getGlobalBounds().height) { r2Pos.y = floorY - robot2.sprite.getGlobalBounds().height; r2Vel.y = 0; }

            // --- Update sprites
            robot1.sprite.setPosition(r1Pos);
            robot2.sprite.setPosition(r2Pos);

            //Decrease cooldowns each frame
            if (r1Cooldown > 0) r1Cooldown--;
            if (r2Cooldown > 0) r2Cooldown--;

            // Robot 1 shooting
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::V) && !r1Shot && r1Cooldown == 0 && r1Bullets > 0) {
                pewSound.play();
                r1Bullets--;  // decrease bullet count

                // Spawn bullets based on weapon type
                if (robot1Choice == 0) { // Laser
                    Bullet b;
                    b.shape = sf::RectangleShape(sf::Vector2f(16, 4));
                    b.shape.setFillColor(sf::Color::Cyan);
                    b.shape.setPosition(r1Pos.x + robot1.sprite.getGlobalBounds().width, r1Pos.y + robot1.sprite.getGlobalBounds().height / 2);
                    b.speed = 10.f;
                    b.damage = 15;
                    robot1Bullets.push_back(b);//
                }
                else if (robot1Choice == 1) { // Shotgun
                    for (int i = -1; i <= 1; i++) {
                        Bullet b;
                        b.shape = sf::RectangleShape(sf::Vector2f(8, 4));
                        b.shape.setFillColor(sf::Color::Red);
                        b.shape.setPosition(r1Pos.x + robot1.sprite.getGlobalBounds().width, r1Pos.y + robot1.sprite.getGlobalBounds().height / 2 + i * 5);
                        b.speed = 8.f;
                        b.damage = 10;
                        robot1Bullets.push_back(b);
                    }
                }
                else { // Missile
                    Bullet b;
                    b.shape = sf::RectangleShape(sf::Vector2f(14, 6));
                    b.shape.setFillColor(sf::Color::Magenta);
                    b.shape.setPosition(r1Pos.x + robot1.sprite.getGlobalBounds().width, r1Pos.y + robot1.sprite.getGlobalBounds().height / 2);
                    b.speed = 6.f;
                    b.damage = 25;
                    robot1Bullets.push_back(b);
                }

                r1Shot = true;
                r1Cooldown = cooldownMax;
            }
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::V)) r1Shot = false;

            // Robot 2 shooting
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::K) && !r2Shot && r2Cooldown == 0 && r2Bullets > 0) {
                pewSound.play();
                r2Bullets--;  // decrease bullet count

                // Spawn bullets based on weapon type
                if (robot2Choice == 0) { // Laser
                    Bullet b;
                    b.shape = sf::RectangleShape(sf::Vector2f(16, 4));
                    b.shape.setFillColor(sf::Color::Yellow);
                    b.shape.setPosition(r2Pos.x, r2Pos.y + robot2.sprite.getGlobalBounds().height / 2);
                    b.speed = -10.f;
                    b.damage = 15;
                    robot2Bullets.push_back(b);
                }
                else if (robot2Choice == 1) { // Shotgun
                    for (int i = -1; i <= 1; i++) {
                        Bullet b;
                        b.shape = sf::RectangleShape(sf::Vector2f(8, 4));
                        b.shape.setFillColor(sf::Color::Cyan);
                        b.shape.setPosition(r2Pos.x, r2Pos.y + robot2.sprite.getGlobalBounds().height / 2 + i * 5);
                        b.speed = -8.f;
                        b.damage = 10;
                        robot2Bullets.push_back(b);
                    }
                }
                else { // Missile
                    Bullet b;
                    b.shape = sf::RectangleShape(sf::Vector2f(14, 6));
                    b.shape.setFillColor(sf::Color::Magenta);
                    b.shape.setPosition(r2Pos.x, r2Pos.y + robot2.sprite.getGlobalBounds().height / 2);
                    b.speed = -6.f;
                    b.damage = 25;
                    robot2Bullets.push_back(b);
                }

                r2Shot = true;
                r2Cooldown = cooldownMax;
            }
            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::K)) r2Shot = false;

            // --- Bullet updates ---
            for (int i = 0; i < robot1Bullets.size(); i++) {
                robot1Bullets[i].shape.move(robot1Bullets[i].speed, 0);
                if (robot1Bullets[i].shape.getGlobalBounds().intersects(robot2.sprite.getGlobalBounds())) {
                    robot2.health -= robot1Bullets[i].damage;
                    if (robot2.health < 0) robot2.health = 0;
                    robot1Bullets.erase(robot1Bullets.begin() + i); i--;
                }
                else if (robot1Bullets[i].shape.getPosition().x > 1280) { robot1Bullets.erase(robot1Bullets.begin() + i); i--; }
            }

            for (int i = 0; i < robot2Bullets.size(); i++) {
                robot2Bullets[i].shape.move(robot2Bullets[i].speed, 0);
                if (robot2Bullets[i].shape.getGlobalBounds().intersects(robot1.sprite.getGlobalBounds())) {
                    robot1.health -= robot2Bullets[i].damage;
                    if (robot1.health < 0) robot1.health = 0;
                    robot2Bullets.erase(robot2Bullets.begin() + i); i--;
                }
                else if (robot2Bullets[i].shape.getPosition().x < 0) { robot2Bullets.erase(robot2Bullets.begin() + i); i--; }
            }

            //Remove collected orbs 
            for (int i = 0; i < pickups.size(); i++) {
                if (pickups[i].collected) {
                    pickups.erase(pickups.begin() + i);
                    i--;
                }
            }

            //Remove collected magazines
            for (int i = 0; i < magazines.size(); i++) {
                if (magazines[i].collected) {
                    magazines.erase(magazines.begin() + i);
                    i--;
                }
            }

            // --- Random spawn logic ---
            orbSpawnTimer++;
            magSpawnTimer++;

            if (orbSpawnTimer > 120) { // roughly every 3 seconds
                if (pickups.size() < maxPickups) {
                    if (rand() % 2 == 0) { // 50% chance to spawn an orb
                        Pickup p;
                        p.shape = sf::CircleShape(10.f);
                        p.shape.setFillColor(sf::Color::Green);
                        p.collected = false;

                        float x = 50 + rand() % 1180;
                        sf::FloatRect orbRect(x, floorY - p.shape.getRadius() * 2, p.shape.getRadius() * 2, p.shape.getRadius() * 2);
                        if (!orbRect.intersects(robot1.sprite.getGlobalBounds()) &&
                            !orbRect.intersects(robot2.sprite.getGlobalBounds())) {
                            p.shape.setPosition(x, floorY - p.shape.getRadius() * 2);
                            pickups.push_back(p);
                        }
                    }
                }
                orbSpawnTimer = 0;
            }

            if (magSpawnTimer > 120) { // roughly every 5 seconds
                if (magazines.size() < maxMagazines) {
                    if (rand() % 2 == 0) { // 50% chance to spawn a magazine
                        Pickup mag;
                        mag.shape = sf::CircleShape(12.f);
                        mag.shape.setFillColor(sf::Color::Red);
                        mag.collected = false;

                        float x = 50 + rand() % 1180;
                        sf::FloatRect magRect(x, floorY - mag.shape.getRadius() * 2, mag.shape.getRadius() * 2, mag.shape.getRadius() * 2);
                        if (!magRect.intersects(robot1.sprite.getGlobalBounds()) &&
                            !magRect.intersects(robot2.sprite.getGlobalBounds())) {
                            mag.shape.setPosition(x, floorY - mag.shape.getRadius() * 2);
                            magazines.push_back(mag);
                        }
                    }
                }
                magSpawnTimer = 0;
            }

            // --- Orb collision & healing ---
            for (int i = 0; i < pickups.size(); i++) {
                if (!pickups[i].collected) {
                    if (robot1.sprite.getGlobalBounds().intersects(pickups[i].shape.getGlobalBounds())) {
                        robot1.health += orbHealAmount;
                        if (robot1.health > maxHealth) robot1.health = maxHealth;
                        pickups[i].collected = true;
                    }
                    if (robot2.sprite.getGlobalBounds().intersects(pickups[i].shape.getGlobalBounds())) {
                        robot2.health += orbHealAmount;
                        if (robot2.health > maxHealth) robot2.health = maxHealth;
                        pickups[i].collected = true;
                    }
                }
            }

            // --- Magazine collection ---
            for (int i = 0; i < magazines.size(); i++) {
                if (!magazines[i].collected) {
                    if (robot1.sprite.getGlobalBounds().intersects(magazines[i].shape.getGlobalBounds())) {
                        r1Bullets = std::min(r1Bullets + bulletsPerMagazine, maxBullets);
                        magazines[i].collected = true;
                    }
                    if (robot2.sprite.getGlobalBounds().intersects(magazines[i].shape.getGlobalBounds())) {
                        r2Bullets = std::min(r2Bullets + bulletsPerMagazine, maxBullets);
                        magazines[i].collected = true;
                    }
                }
            }

            // --- Spawn a few items at game start ---
            if (!firstSpawnDone) {
                // Spawn 1–2 orbs
                for (int i = 0; i < 1 + rand() % 2; i++) {
                    Pickup p;
                    p.shape = sf::CircleShape(10.f);
                    p.shape.setFillColor(sf::Color::Green);
                    p.collected = false;
                    float x = 50 + rand() % 1180;
                    p.shape.setPosition(x, floorY - p.shape.getRadius() * 2);
                    pickups.push_back(p);
                }
                // Spawn 1–2 magazines
                for (int i = 0; i < 1 + rand() % 2; i++) {
                    Pickup mag;
                    mag.shape = sf::CircleShape(12.f);
                    mag.shape.setFillColor(sf::Color::Red);
                    mag.collected = false;
                    float x = 50 + rand() % 1180;
                    mag.shape.setPosition(x, floorY - mag.shape.getRadius() * 2);
                    magazines.push_back(mag);
                }
                firstSpawnDone = true;
            }


            // --- Draw ---
            window.draw(robot1.sprite);
            window.draw(robot2.sprite);
            for (auto& b : robot1Bullets) window.draw(b.shape);
            for (auto& b : robot2Bullets) window.draw(b.shape);
            for (auto& p : pickups) if (!p.collected) window.draw(p.shape);
            for (auto& m : magazines) if (!m.collected) window.draw(m.shape);


            // --- Draw Health Bars ---
            sf::RectangleShape r1Bar(sf::Vector2f(robot1.health * 2.f, 20));
            r1Bar.setFillColor(sf::Color::Green);
            r1Bar.setPosition(50, 50);
            window.draw(r1Bar);

            sf::RectangleShape r2Bar(sf::Vector2f(robot2.health * 2.f, 20));
            r2Bar.setFillColor(sf::Color::Green);
            r2Bar.setPosition(1280 - 50 - r2Bar.getSize().x, 50);
            window.draw(r2Bar);

            // Health numbers
            sf::Text r1HealthText(std::to_string(robot1.health) + " HP", font, 20);
            r1HealthText.setFillColor(sf::Color::White);
            r1HealthText.setPosition(355, 75);
            window.draw(r1HealthText);

            sf::Text r2HealthText(std::to_string(robot2.health) + " HP", font, 20);
            r2HealthText.setFillColor(sf::Color::White);
            r2HealthText.setPosition(1280 - 50 - r2Bar.getSize().x, 75);
            window.draw(r2HealthText);

            // --- Draw Bullet Bars (vertical, red) ---
            float bulletBarWidth = 20.f;
            float bulletBarMaxHeight = 100.f; // max height for 10 bullets
            float r1BarHeight = (r1Bullets / (float)maxBullets) * bulletBarMaxHeight;
            float r2BarHeight = (r2Bullets / (float)maxBullets) * bulletBarMaxHeight;

            sf::RectangleShape r1BulletBar(sf::Vector2f(bulletBarWidth, r1BarHeight));
            r1BulletBar.setFillColor(sf::Color::Red);
            r1BulletBar.setPosition(50, 50 + 20 + (bulletBarMaxHeight - r1BarHeight)); // below health bar
            window.draw(r1BulletBar);

            sf::RectangleShape r2BulletBar(sf::Vector2f(bulletBarWidth, r2BarHeight));
            r2BulletBar.setFillColor(sf::Color::Red);
            r2BulletBar.setPosition(1280 - 50 - bulletBarWidth, 50 + 20 + (bulletBarMaxHeight - r2BarHeight));
            window.draw(r2BulletBar);

            // Optional: Draw bullet numbers
            sf::Text r1BulletText(std::to_string(r1Bullets), font, 18);
            r1BulletText.setFillColor(sf::Color::White);
            r1BulletText.setPosition(50, 50 + 20 + bulletBarMaxHeight + 5);
            window.draw(r1BulletText);

            sf::Text r2BulletText(std::to_string(r2Bullets), font, 18);
            r2BulletText.setFillColor(sf::Color::White);
            r2BulletText.setPosition(1280 - 50 - bulletBarWidth, 50 + 20 + bulletBarMaxHeight + 5);
            window.draw(r2BulletText);



            sf::Text gameOnText("GAME ON!", font, 30);
            gameOnText.setFillColor(sf::Color::White);
            gameOnText.setPosition(1280 / 2 - gameOnText.getLocalBounds().width / 2, 20);
            window.draw(gameOnText);

            if (robot1.health <= 0 || robot2.health <= 0) state = GameState::GAMEOVER;
        }
        else if (state == GameState::GAMEOVER) {
            sf::Text winnerText("", font, 50);
            winnerText.setFillColor(sf::Color::Yellow);
            winnerText.setPosition(1280 / 2 - 200, 300);
            if (robot1.health <= 0) winnerText.setString("Robot 2 Wins!");
            if (robot2.health <= 0) winnerText.setString("Robot 1 Wins!");
            window.draw(winnerText);

            sf::Text restartText("Press R to Play Again", font, 30);
            restartText.setFillColor(sf::Color::White);
            restartText.setPosition(1280 / 2 - restartText.getLocalBounds().width / 2, 400);
            window.draw(restartText);

            sf::Text exitText("Press E to Exit", font, 30);
            exitText.setFillColor(sf::Color::White);
            exitText.setPosition(1280 / 2 - exitText.getLocalBounds().width / 2, 450);
            window.draw(exitText);

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
                state = GameState::SELECTION;
                robot1.health = robot2.health = 200;
                robot1Bullets.clear(); robot2Bullets.clear();
                pickups.clear();
                r1Pos = sf::Vector2f(100.f, floorY - robot1.sprite.getGlobalBounds().height);
                r2Pos = sf::Vector2f(1080.f, floorY - robot2.sprite.getGlobalBounds().height);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E)) window.close();
        }

        window.display();
    }

    return 0;
}