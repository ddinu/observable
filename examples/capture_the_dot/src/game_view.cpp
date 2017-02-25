#include "game_view.h"
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>

namespace {
    auto const background_color = QColor { 236, 240, 241 };
    auto const regular_color = QColor { 41, 128, 185, 200 };
    auto const highlight_color = QColor { 231, 76, 60, 200 };
    auto const score_color = QColor { 44, 62, 80 };

    auto const click_threshold = 20; // Pixels.
}

GameView::GameView(QWidget * parent) :
    QWidget(parent),
    color_ { regular_color }
{
}

void GameView::highlight_dot(bool highlight)
{
    color_ = highlight ? highlight_color : regular_color;
    update();
}

void GameView::update_dot(double x, double y, double r)
{
    r_ = r;
    x_ = x;
    y_ = y;
    update();
}

void GameView::update_score(unsigned long score)
{
    score_ = score;
    update();
}

void GameView::mousePressEvent(QMouseEvent * ev)
{
    click_start_ = ev->pos();
}

void GameView::mouseReleaseEvent(QMouseEvent * ev)
{
    auto const pos = ev->pos();
    if((pos - click_start_).manhattanLength() <= click_threshold)
        emit clicked(pos.x(), pos.y());
}

void GameView::resizeEvent(QResizeEvent *)
{
    auto s = size();
    emit resized(s.width(), s.height());
}

void GameView::paintEvent(QPaintEvent * ev)
{
    QPainter p { this };
    p.setRenderHint(QPainter::Antialiasing);
    p.setBackgroundMode(Qt::OpaqueMode);
    p.setBackground(background_color);
    p.fillRect(ev->rect(), background_color);

    auto font = p.font();
    font.setPixelSize(18);
    p.setFont(font);
    p.setPen(score_color);
    p.drawText(QRect { 10, 10, width() - 20, 40 }, QString { "Score: %1" }.arg(score_));

    p.setPen({ QColor { 0, 0, 0, 50 }, 0.1 });
    p.setBrush(color_);
    p.drawEllipse({ x_, y_ }, r_, r_);

    QWidget::paintEvent(ev);
}
