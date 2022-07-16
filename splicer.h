#ifndef SPLICER_H
#define SPLICER_H

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>

QT_BEGIN_NAMESPACE
namespace Ui { class Splicer; }
QT_END_NAMESPACE

QList<uint> resolve(uint);

QVector<QRectF> calculate(QSizeF, uint);

class Splicer : public QWidget
{
    Q_OBJECT

public:
    Splicer(QWidget *parent = nullptr);
    ~Splicer();

protected:
    virtual void keyReleaseEvent(QKeyEvent *e) override;

private slots:
    void on_pushButton_clicked();

    void render_images();

    void on_row_lineEdit_editingFinished();

    void on_col_lineEdit_editingFinished();

    void on_lr_slider_valueChanged(int value);

    void on_tb_slider_valueChanged(int value);

    void on_pushButton_2_clicked();

signals:
    void images_selected();

private:
    Ui::Splicer *ui;

    void _render();

    int lr_margin;
    int tb_margin;

    int row_num;
    int col_num;

    bool is_rendered;

    QGraphicsScene *scene;
    QList<QString> curImagePaths;
};
#endif // SPLICER_H
