#include "game_model.h"
#include <algorithm>
#include <cmath>
#include <QDebug>

namespace {
    constexpr auto const max_radius = 50.0;
    constexpr auto const padding = 10.0;

    //! Constrain the provided point to a rectangle.
    GameModel::Point captive_(GameModel::Point const & p,
                              double min_x, double max_x,
                              double min_y, double max_y)
    {
        return {
            std::min(std::max(p.x, min_x), max_x),
            std::min(std::max(p.y, min_y), max_y)
        };
    }

    OBSERVABLE_ADAPT_FILTER(captive, captive_);

    //! Compute the distance between two points.
    double distance(GameModel::Point const & p1, GameModel::Point const & p2)
    {
        return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
    }
}

GameModel::GameModel()
{
    min_scene_width = observe(dot_radius * 2 + padding);

    min_scene_height = observe(dot_radius * 2 + padding);

    scene_width = observe(max(min_scene_width, width_));

    scene_height = observe(max(min_scene_height, height_));

    dot_position = observe(
                        captive(pos_,
                                dot_radius + padding,
                                scene_width - dot_radius - padding,
                                dot_radius + padding,
                                scene_height - dot_radius - padding)
                   );

    using namespace std::chrono_literals;
    using namespace std::chrono;
    using observable::static_expr_cast;
    using observable::construct;

    delay = observe(
                    select(is_captured,
                           500ms,
                           construct<milliseconds>(clamp(
                                static_expr_cast<int>(2200 / log10(40 + score / 3)),
                                600,
                                1500
                           ))
                    )
                 );

    is_captured.subscribe([this](auto captured) {
        if(!captured)
            return;

        auto const k = 1 - dot_radius.get() / max_radius;
        auto const a = scene_width.get() * scene_height.get();
        auto const n = std::pow(k * 10, 2) + a / 10000;
        score = score.get() + static_cast<unsigned long>(n);
    }).release();
}

void GameModel::try_capture(Point const & cursor_position)
{
    is_captured = distance(cursor_position, dot_position.get()) <= dot_radius.get();
}

void GameModel::randomize_dot()
{
    is_captured = false;

    auto xs = std::uniform_real_distribution<double> { 0, scene_width.get() };
    auto ys = std::uniform_real_distribution<double> { 0, scene_height.get() };
    auto rs = std::uniform_real_distribution<double> { max_radius / 4, max_radius };

    pos_ = Point { xs(rd_), ys(rd_) };
    dot_radius = rs(rd_);
}

void GameModel::resize_scene(double width, double height)
{
    assert(width > 0);
    assert(height > 0);

    width_ = width;
    height_ = height;
}
