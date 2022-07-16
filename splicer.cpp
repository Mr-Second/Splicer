#include "splicer.h"
#include "ui_splicer.h"

#include <QPixmap>
#include <QKeyEvent>
#include <QGraphicsItem>
#include <QFileDialog>

Splicer::Splicer(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Splicer)
{
    ui->setupUi(this);
    scene = new QGraphicsScene(this);
    row_num = 0;
    col_num = 0;
    lr_margin = 0;
    tb_margin = 0;
    is_rendered = false;
    connect(this, &Splicer::images_selected, this, &Splicer::render_images);
}

Splicer::~Splicer()
{
    this->scene->deleteLater();
    delete ui;
}

void Splicer::keyReleaseEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Space && this->scene->selectedItems().length() == 2) {
        auto items = this->scene->selectedItems();
        auto first_pos = items[0]->pos();
        auto second_pos = items[1]->pos();
        auto first_z_value = items[0]->zValue();
        auto second_z_value = items[1]->zValue();
        items[0]->setPos(second_pos);
        items[1]->setPos(first_pos);
        items[0]->setZValue(second_z_value);
        items[1]->setZValue(first_z_value);
    }
}


void Splicer::on_pushButton_clicked()
{

    auto filenames = QFileDialog::getOpenFileNames(this, "Select Images to be spliced", ".", "image file(*.jpg *.jpeg *.png *.bmp *.tif)");
    if(!filenames.empty()) {
        ui->row_lineEdit->setEnabled(true);
        ui->col_lineEdit->setEnabled(true);
        ui->lr_slider->setEnabled(true);
        ui->tb_slider->setEnabled(true);

        this->curImagePaths = filenames;
        emit images_selected();
    }
}

void Splicer::render_images()
{
    qreal cur_z = 0;
    for(auto& path: this->curImagePaths) {
        auto item = new QGraphicsPixmapItem();
        item->setPixmap(QPixmap{path});
        item->setFlag(QGraphicsItem::ItemIsMovable);
        item->setFlag(QGraphicsItem::ItemIsSelectable);
        item->setZValue(cur_z++);
        this->scene->addItem(item);
    }
    ui->graphicsView->setScene(this->scene);
}


QList<uint> resolve(uint num) {
    auto start = uint(qSqrt(num));
    auto factor = num / start;
    while(start * factor != num) {
        start += 1;
        factor = num / start;
    }
    return QList<uint>{factor, start};
}

void Splicer::on_row_lineEdit_editingFinished()
{
    row_num = ui->row_lineEdit->text().toInt();
    if(this->row_num * this->col_num == this->curImagePaths.length()) {
        this->_render();
    }
}


void Splicer::on_col_lineEdit_editingFinished()
{
    col_num = ui->col_lineEdit->text().toInt();
    if(this->row_num * this->col_num == this->curImagePaths.length()) {
        this->_render();
    }
}

void Splicer::_render()
{

    auto items = this->scene->items();

    uint col_len = items[0]->boundingRect().width();
    uint row_len = items[0]->boundingRect().height();
    for(int i = 0; i < row_num; ++i) {
        for(int j = 0; j < col_num; ++j) {
            auto idx = i * col_num + j;
            auto pos = QPointF(j * col_len + (j + 1) * this->lr_margin, i * row_len + (i + 1) * this->tb_margin);
            items[idx]->setPos(pos);
        }
    }
    this->is_rendered = true;
}


void Splicer::on_lr_slider_valueChanged(int value)
{
    this->lr_margin = double(value / (double) ui->lr_slider->maximum() * 10);
    this->_render();
}


void Splicer::on_tb_slider_valueChanged(int value)
{
    this->tb_margin = double(value / (double) ui->tb_slider->maximum() * 10);
    this->_render();
}


void Splicer::on_pushButton_2_clicked()
{
    if(!this->is_rendered)
        return;
    auto items = this->scene->items();
    int total_width = col_num * lr_margin;
    int total_height = row_num * tb_margin;
    total_width += items[0]->boundingRect().width() * col_num;
    total_height += items[0]->boundingRect().height() * row_num;

    QImage img(QSize(total_width, total_height), QImage::Format_RGB32);
    QPainter p(&img);
    this->scene->render(&p);
    QString save_path = QFileDialog::getSaveFileName(this, "Save Result...","result.png","Images (*.png *.jpg)");
    img.save(save_path, nullptr, 100);
}

