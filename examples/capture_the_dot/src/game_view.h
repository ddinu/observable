#pragma once
#include <QWidget>

class QMouseEvent;
class QResizeEvent;
class QPaintEvent;

//! Render the game scene.
//!
//! This class is a QWidget.
class GameView : public QWidget
{
    Q_OBJECT

signals:
    //! Signal emitted when the widget has been resized.
    //!
    //! \param width The widget's new width.
    //! \param height The widget's new height.
    void resized(double width, double height) const;

    //! Signal emitted when the scene is clicked.
    //!
    //! \param x The click x coordinate.
    //! \param y The click y coordinate.
    void clicked(double x, double y) const;

public:
    //! Create a new game view with the provided parent.
    explicit GameView(QWidget * parent=nullptr);

    //! Set the dot's highlighted state.
    //!
    //! When the dot is highlighted is drawn in a different way.
    //!
    //! \param highlight True if the dot should be highlighted, false otherwise.
    void highlight_dot(bool highlight);

    //! Update the dot's position and radius.
    //!
    //! \param x The dot's x coordinate.
    //! \param y The dot's y coordinate.
    //! \param r The dot's radius.
    void update_dot(double x, double y, double r);

    //! Update the score.
    //!
    //! \param score The new score.
    void update_score(unsigned long score);

private:
    //! \see QWidget::mousePressEvent
    void mousePressEvent(QMouseEvent * ev) override;

    //! \see QWidget::mouseReleaseEvent
    void mouseReleaseEvent(QMouseEvent * ev) override;

    //! \see QWidget::resizeEvent
    void resizeEvent(QResizeEvent * ev) override;

    //! \see QWidget::paintEvent
    void paintEvent(QPaintEvent * ev) override;

private:
    double x_ { 0 };
    double y_ { 0 };
    double r_ { 0 };
    QColor color_;
    unsigned long score_ { 0 };

    QPoint click_start_;
};
