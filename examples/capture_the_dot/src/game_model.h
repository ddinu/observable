#pragma once
#include <chrono>
#include <random>
#include <observable/observable.hpp>

//! Capture the dot game logic.
class GameModel
{
    OBSERVABLE_PROPERTIES(GameModel)

public:
    struct Point { double x; double y; };

    //! Position of the dot.
    observable_property<Point> dot_position;

    //! Dot's radius.
    observable_property<double> dot_radius { 10 };

    //! True if the dot has been captured, false otherwise.
    observable_property<bool> is_captured { false };

    //! The game's current score.
    observable_property<unsigned long> score { 0 };

    //! Minimum allowed scene width.
    observable_property<double> min_scene_width;

    //! Minimum allowed scene height.
    observable_property<double> min_scene_height;

    //! Current scene width.
    observable_property<double> scene_width;

    //! Current scene height.
    observable_property<double> scene_height;

    //! The current delay between random moves.
    observable_property<std::chrono::milliseconds> delay;

public:
    //! Create a game model.
    GameModel();

    //! Try to capture the dot for a current position.
    //!
    //! If the current position is intersecting the dot, the ``is_captured``
    //! property will become true.
    void try_capture(Point const & cursor_position);

    //! Move the dot to a random position and change it's radius.
    //!
    //! The dot will be inside the scene as defined by the ``scene_width`` and
    //! ``scene_height`` properties.
    //!
    //! A call to this method will also reset the ``is_capture`` property if it
    //! has been set by try_capture().
    //!
    //! You should call this method continuously, with a delay prescribed by the
    //! ``delay`` property between two successive calls.
    void randomize_dot();

    //! Resize the scene.
    //!
    //! This will affect both the ``scene_width`` and ``scene_height`` properties
    //! as well as the ``dot_position`` property.
    void resize_scene(double width, double height);

private:
    observable::value<Point> pos_ { { 250, 250 } };
    observable::value<double> width_ { 500 };
    observable::value<double> height_ { 500 };

    std::random_device rd_;
};
