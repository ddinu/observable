#include <memory>
#include <QApplication>
#include <QMainWindow>
#include "game_controller.h"
#include "game_model.h"
#include "game_view.h"

int main(int argc, char ** argv)
{
    QApplication app { argc, argv };

    // Create the model, view and controller and setup the game inside a window.

    auto window = std::make_shared<QMainWindow>();

    auto view = std::shared_ptr<GameView> { window, new GameView { } };
    window->setCentralWidget(view.get());

    auto model = std::make_shared<GameModel>();
    window->resize(model->scene_width.get(), model->scene_height.get());

    GameController game { view, model };
    game.start();

    window->show();
    return app.exec();
}
