#include "resourcepacks.h"
#include "ui_resourcepacks.h"
#include <QDir>
#include <QDesktopServices>
#include <QStandardItemModel>
#include <QUrl>
#include <QDebug>

ResourcePacks::ResourcePacks(QWidget *parent, std::string curr_res_pack) :
    QDialog(parent),
    ui(new Ui::ResourcePacks)
{
    ui->setupUi(this);
    ui->label->setText("Resource pack folders go under the resource_packs directory.");

    connect(ui->help_button, SIGNAL(clicked()), this, SLOT(handle_help_clicked()));
    connect(ui->cancel_button, SIGNAL(clicked()), this, SLOT(handle_cancel_clicked()));
    connect(ui->save_button, SIGNAL(clicked()), this, SLOT(handle_save_clicked()));

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->horizontalHeader()->hide();
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->verticalHeader()->hide();
    ui->tableView->setEditTriggers(ui->tableView->editTriggers() & QAbstractItemView::NoEditTriggers);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableView->setSelectionMode(QAbstractItemView::SingleSelection);

    load_resource_packs(curr_res_pack);
}

ResourcePacks::~ResourcePacks()
{
    delete ui;
}

void ResourcePacks::handle_help_clicked() {
    // TODO: in-launcher dialog for help maybe?
    QDesktopServices::openUrl(QUrl("http://toontownrewritten.wikia.com/wiki/Resource_pack"));
}


// is this needed? for consistency, maybe?
void ResourcePacks::handle_cancel_clicked() {
    this->close();
}

void ResourcePacks::handle_save_clicked() {
    QStandardItemModel *model = (QStandardItemModel *)ui->tableView->model();

    int checked = 0;
    int saved_index = -1;

    for (int i = 0; i < model->rowCount(); i++) {
        Qt::CheckState check_state = static_cast<Qt::CheckState>(model->data(model->index(i, 0), Qt::CheckStateRole).toUInt());
        if (check_state == Qt::Checked) {
            if (checked >= 1) {
                too_many_checks();
                return;
            }
            saved_index = i;
            checked += 1;
        }
    }

    if (saved_index == -1) {
        res_pack_dir = "";
    } else {
        res_pack_dir = model->index(saved_index, 0).data().toString().toStdString();
    }

    emit send_pack_dir_to_main(res_pack_dir);
    this->close();
    return;
}

void ResourcePacks::load_resource_packs(std::string curr_res_pack) {
    QDir resource_packs = ("resource_packs/");
    if (!resource_packs.exists()) {
        resource_packs.mkdir(".");
    }

    resource_packs.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);

    QStringList list;
    list = resource_packs.entryList();

    QStandardItemModel *model = new QStandardItemModel(this);
    ui->tableView->setModel(model);
    model->setColumnCount(1);

    for (QString dir : list) {
        QList<QStandardItem *> items;
        QStandardItem *element = new QStandardItem();
        element->setCheckable(true);

        if (dir.compare(QString::fromStdString(curr_res_pack)) == 0) {
            element->setCheckState(Qt::Checked);
            res_pack_dir = curr_res_pack;
        } else {
            element->setCheckState(Qt::Unchecked);
        }

        element->setText(dir);
        items.append(element);
        model->appendRow(items);
    }
}

void ResourcePacks::too_many_checks() {
    ui->label->setText("You cannot select more than one resource pack.");
}
