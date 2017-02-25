#pragma once
#include <memory>
#include <vector>
#include <QTimer>
#include "observable/subscription.hpp"

class GameView;
class GameModel;

//! Link a game view to a game model and manage the game play timer.
class GameController
{
public:
    //! Create a new game controller.
    //!
    //! \param view The game view to use and link to the model.
    //! \param model The game model to use and link to the view.
    GameController(std::shared_ptr<GameView> view, std::shared_ptr<GameModel> model);

    //! Start the game.
    void start();

    //! Stop the game.
    //!
    //! \note This only stops the game timer.
    void stop();

    //! Destructor.
    //!
    //! The destructor will unlink the view and the model, unsubscribing all
    //! observers and disconnecting all view signals.
    ~GameController();

private:
    std::shared_ptr<GameView> view_;
    std::shared_ptr<GameModel> model_;
    std::vector<observable::unique_subscription> subs_;
    std::vector<QMetaObject::Connection> cons_;

    std::unique_ptr<QTimer> timer_ { std::make_unique<QTimer>() };
};
