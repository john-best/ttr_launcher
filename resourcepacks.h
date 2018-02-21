#ifndef RESOURCEPACKS_H
#define RESOURCEPACKS_H

#include <QDialog>

namespace Ui {
class ResourcePacks;
}

class ResourcePacks : public QDialog
{
    Q_OBJECT

public:
    explicit ResourcePacks(QWidget *parent = 0, std::string curr_res_pack = "");
    ~ResourcePacks();

signals:
    void send_pack_dir_to_main(std::string res_pack_dir);

private slots:
    void handle_help_clicked();
    void handle_save_clicked();
    void handle_cancel_clicked();

private:
    Ui::ResourcePacks *ui;
    void load_resource_packs(std::string curr_res_pack);
    void too_many_checks();
    std::string res_pack_dir ="";
};

#endif // RESOURCEPACKS_H
