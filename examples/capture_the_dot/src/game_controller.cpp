#include "game_controller.h"
#include <cassert>
#include <chrono>
#include "game_view.h"
#include "game_model.h"

using namespace std::chrono_literals;

GameController::GameController(std::shared_ptr<GameView> view,
                               std::shared_ptr<GameModel> model) :
    view_ { std::move(view) },
    model_ { std::move(model) }
{
    assert(view_);
    assert(model_);

    // Link the model to the view.

    cons_.emplace_back(
        QObject::connect(view_.get(), &GameView::resized,
                         [&](auto w, auto h) { model_->resize_scene(w, h); })
    );

    cons_.emplace_back(
        QObject::connect(view_.get(), &GameView::clicked,
                         [&](auto x, auto y) { model_->try_capture({ x, y }); })
    );

    // Link the view to the model.

    subs_.emplace_back(model_->is_captured.subscribe([&](auto captured) {
        view_->highlight_dot(captured);
    }));

    subs_.emplace_back(model_->dot_position.subscribe([&](auto const & pos) {
        view_->update_dot(pos.x, pos.y, model_->dot_radius.get());
    }));

    subs_.emplace_back(model_->dot_radius.subscribe([&](auto r) {
        auto const pos = model_->dot_position.get();
        view_->update_dot(pos.x, pos.y, r);
    }));

    subs_.emplace_back(model_->min_scene_width.subscribe([&](auto w) {
        view_->setMinimumWidth(w);
    }));

    subs_.emplace_back(model_->min_scene_height.subscribe([&](auto h) {
        view_->setMinimumHeight(h);
    }));

    subs_.emplace_back(model_->score.subscribe([&](auto s) {
        view_->update_score(s);
    }));

    // Setup the game timer links.

    cons_.emplace_back(
        QObject::connect(timer_.get(), &QTimer::timeout,
                         [&]() { model_->randomize_dot(); })
    );

    subs_.emplace_back(model_->delay.subscribe([&](auto d) {
        using namespace std::chrono;
        timer_->start(duration_cast<milliseconds>(d).count());
    }));
}

void GameController::start()
{
    using namespace std::chrono;
    timer_->start(duration_cast<milliseconds>(model_->delay.get()).count());
    model_->randomize_dot();
}

void GameController::stop()
{
    timer_->stop();
}

GameController::~GameController()
{
    stop();

    for(auto && c : cons_)
        QObject::disconnect(c);
}
