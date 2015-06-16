#ifndef ISLAND_FORM
#define ISLAND_FORM

#include "include/domain/mesh_polygon.h"
#include "include/domain/mesh.h"
#include <QDialog>
#include <QSettings>
#include <QAbstractButton>

namespace Ui {
    class IslandForm;
}

class IslandForm : public QDialog {
    Q_OBJECT
    
public:
    explicit IslandForm(QDialog *parent, Mesh *mesh);
    ~IslandForm();
    
    MeshPolygon* getMeshPolygon() const;
    
private slots:
    void on_btnBrowse_clicked();
    
private:
    const QString ISLAND_FORM_DEFAULT_DIR_KEY;
    
    Ui::IslandForm *ui;
    QSettings *appSettings;
    Mesh *mesh;
    MeshPolygon *meshPolygon;
    
    QString getDefaultDirectory();
    bool isValid();
    virtual void accept();
};

#endif
