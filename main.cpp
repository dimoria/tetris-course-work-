//Список бібліотек
#include <SFML/Graphics.hpp>
#include <time.h>


using namespace sf;

//параметри нашого вікна
const int height = 20;
const int width = 10;

//Поле нашої гри
int field[height][width] = { 0 };
int barSize = 34;//розмір плити

struct Point
{
    int x, y;
} a[4], b[4];

//Всі наші фігури
int figure[7][4] = {
    1,3,5,7,
    2,4,5,7,
    3,5,4,6,
    3,5,4,7,
    2,3,5,7,
    3,5,7,6,
    2,3,4,5,
};

//Функція перевірки за межі графічного вікна
bool check() {
    for (int i = 0; i < 4; i++)
        if (a[i].x < 0 || a[i].x >= width || a[i].y >= height)
            return 0;
        else if (field[a[i].y][a[i].x])
            return 0;

    return 1;
}


//Основний цикл
int main()
{
    srand(time(0));

    RenderWindow window(VideoMode(width * barSize, height * barSize), "Tetris");

    //кольора фігур
    Texture appearance;
    appearance.loadFromFile("D:/course work/tetris/tiles.png");

    //кольорова схема
    Sprite tiles(appearance);

    //Завантаження текстури Game Over
    Texture gameOverTexture;
    gameOverTexture.loadFromFile("D:/course work/tetris/gameOver.png");
    Sprite gameOverSprite(gameOverTexture);

    float scaleX = (width * barSize) / static_cast<float>(gameOverTexture.getSize().x);
    float scaleY = (height * barSize) / static_cast<float>(gameOverTexture.getSize().y);
    gameOverSprite.setScale(scaleX, scaleY);


    //Задаєм горизонтальне переміщення й отацію
    int directionHorizont = 0;
    int colorType = 1;
    bool rotation = false;

    //Таймер
    float timer = 0, delay = 0.3;

    //Годинник
    Clock clock;
    bool ad = true;

    bool gameOver = false;  // Додаємо стан гри

    //Робота вікна
    while (window.isOpen())
    {
        //Час з початку, яке переробили в секунди
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();

            //Настроки калвіатури
            if (event.type == Event::KeyPressed && !gameOver) {
                if (event.key.code == Keyboard::W)
                    rotation = true;
                else if (event.key.code == Keyboard::D)
                    directionHorizont = 1;
                else if (event.key.code == Keyboard::A)
                    directionHorizont = -1;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::S) && !gameOver)
            delay = 0.05;

        if (!gameOver) {
            //Переміщення по горизонту
            for (int i = 0; i < 4; i++) {
                b[i] = a[i];
                a[i].x += directionHorizont;
            }

            //Обмеження кордону (попадання на кордон => повернення на початкову координату)
            if (!check()) {
                for (int i = 0; i < 4; i++)
                    a[i] = b[i];
            }

            //Процес ротації
            if (rotation) {
                Point centr = a[1];//Центр повертання
                for (int i = 0; i < 4; i++) {
                    int x = a[i].y - centr.y;
                    int y = a[i].x - centr.x;

                    a[i].x = centr.x - x;
                    a[i].y = centr.y + y;
                }

                //Вихід за границю 
                if (!check()) {
                    for (int i = 0; i < 4; i++)
                        a[i] = b[i];
                }
            }

            //Рух до низу
            if (timer > delay) {
                //Горизонтальне переміщення
                for (int i = 0; i < 4; i++) {
                    b[i] = a[i];
                    a[i].y += 1;
                }

                if (!check()) {
                    for (int i = 0; i < 4; i++)
                        field[b[i].y][b[i].x] = colorType;

                    //Перевірка на "Game Over"
                    for (int i = 0; i < 4; i++) {
                        if (b[i].y == 0) {
                            gameOver = true;
                            break;
                        }
                    }

                    if (!gameOver) {
                        colorType = 1 + rand() % 7;
                        //Задаєм рандомні параметри фігури
                        int figureN = rand() % 7;
                        for (int i = 0; i < 4; i++) {
                            a[i].x = figure[figureN][i] % 2;
                            a[i].y = figure[figureN][i] / 2;
                        }
                    }
                }

                timer = 0;
            }

            //Поява фігури
            if (ad) {
                int figureN = rand() % 7;
                if (a[0].x == 0)
                    for (int i = 0; i < 4; i++) {
                        a[i].x = figure[figureN][i] % 2;
                        a[i].y = figure[figureN][i] / 2;
                    }
                ad = false;
            }

            //Зникнення заповненого ряду
            int line = height - 1;
            for (int i = height - 1; i > 0; i--) {
                int count = 0;
                for (int j = 0; j < width; j++) {
                    if (field[i][j])
                        count++;
                    field[line][j] = field[i][j];
                }
                if (count < width)
                    line--;
            }
        }

        directionHorizont = 0;
        rotation = false;
        delay = 0.3;

        window.clear(Color::Black);

        if (!gameOver) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++) {
                    if (field[i][j] == 0)
                        continue;
                    tiles.setTextureRect(IntRect(field[i][j] * barSize, 0, barSize, barSize));
                    tiles.setPosition(j * barSize, i * barSize);
                    window.draw(tiles);
                }

            for (int i = 0; i < 4; i++) {
                tiles.setTextureRect(IntRect(colorType * barSize, 0, barSize, barSize));
                //Позиціонка фігури
                tiles.setPosition(a[i].x * barSize, a[i].y * barSize);
                window.draw(tiles);
            }

        }
        else {
            //Відображення картинки Game Over
            gameOverSprite.setPosition((width * barSize - gameOverTexture.getSize().x) / 2, (height * barSize - gameOverTexture.getSize().y) / 2);
            gameOverSprite.setPosition(0, 0);
            window.draw(gameOverSprite);
        }
        
        window.display();
    }

    return 0;
}
